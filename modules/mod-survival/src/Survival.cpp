#include "Survival.h"
#include "SurvivalModel.h"

#include "Chat.h"
#include "CellImpl.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "DataMap.h"
#include "GameObject.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Log.h"
#include "Map.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"

#include <atomic>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace SylvaniaSurvival
{
    namespace
    {
        constexpr char Source[] = "mod-survival";
        constexpr char RuntimeKey[] = "sylvania-survival-runtime";
        constexpr uint32 ColdSpell = 6136; // Stock WotLK Transi / Chilled: no shared spell edits.
        constexpr uint32 HungerSpell = 900101; // Dedicated passive server aura; its icon is drawn by the addon.
        constexpr uint32 StepMs = 5000;
        enum Setting : uint32 { Status, Food, Water, Cold, Schema, HealthCeiling };

        struct Settings
        {
            bool enabled = false;
            bool pauseAfk = false;
            uint32 stopLevel = 80;
            uint32 altarEntry = 254605;
            Rules rules;
            std::unordered_set<uint32> coldZones;
            std::unordered_set<uint32> hotZones;
            std::vector<uint32> fires;
        };

        std::atomic<std::shared_ptr<Settings const>> settings{std::make_shared<Settings>()};
        std::atomic_bool hungerReady{false};

        struct Runtime : DataMap::Base
        {
            State state;
            uint32 elapsedMs = 0;
            uint32 persistedStatus = 1;
            Aura* ownedColdAura = nullptr;
            Aura* ownedHungerAura = nullptr;
            bool addon = false;
        };

        Runtime* GetRuntime(Player* player)
        {
            return player ? player->CustomData.Get<Runtime>(RuntimeKey) : nullptr;
        }

        bool French(Player* player)
        {
            return player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_frFR;
        }

        void Message(Player* player, char const* fr, char const* en)
        {
            ChatHandler(player->GetSession()).SendSysMessage(std::string("|cff80d050[Survival]|r ") + (French(player) ? fr : en));
        }

        void Save(Player* player, Runtime const& runtime)
        {
            player->UpdatePlayerSetting(Source, Status, runtime.persistedStatus);
            player->UpdatePlayerSetting(Source, Food, runtime.state.food);
            player->UpdatePlayerSetting(Source, Water, runtime.state.water);
            player->UpdatePlayerSetting(Source, Cold, runtime.state.cold);
            player->UpdatePlayerSetting(Source, Schema, 1);
            player->UpdatePlayerSetting(Source, HealthCeiling, runtime.state.healthCeiling);
        }

        void RemoveOwnedCold(Player* player, Runtime& runtime)
        {
            Aura* aura = player->GetAura(ColdSpell, player->GetGUID());
            if (runtime.ownedColdAura && aura == runtime.ownedColdAura)
                aura->Remove();
            runtime.ownedColdAura = nullptr;
        }

        void UpdateCold(Player* player, Runtime& runtime)
        {
            if (runtime.state.cold < Full * 3 / 4)
            {
                RemoveOwnedCold(player, runtime);
                return;
            }
            Aura* own = player->GetAura(ColdSpell, player->GetGUID());
            if (runtime.ownedColdAura && own == runtime.ownedColdAura)
            {
                own->SetDuration(15000);
                return;
            }
            runtime.ownedColdAura = nullptr;
            // An existing combat snare takes precedence; never replace or remove it.
            if (player->HasAura(ColdSpell))
                return;
            if (Aura* aura = player->AddAura(ColdSpell, player))
            {
                aura->SetMaxDuration(15000);
                aura->SetDuration(15000);
                runtime.ownedColdAura = aura;
            }
        }

        void RemoveOwnedHunger(Player* player, Runtime& runtime)
        {
            Aura* aura = player->GetAura(HungerSpell, player->GetGUID());
            if (runtime.ownedHungerAura && aura == runtime.ownedHungerAura)
                aura->Remove();
            runtime.ownedHungerAura = nullptr;
        }

        void UpdateHunger(Player* player, Runtime& runtime)
        {
            if (runtime.state.food != 0)
            {
                RemoveOwnedHunger(player, runtime);
                return;
            }
            Aura* aura = player->GetAura(HungerSpell, player->GetGUID());
            if (runtime.ownedHungerAura && aura == runtime.ownedHungerAura)
                return;
            runtime.ownedHungerAura = nullptr;
            if (!player->HasAura(HungerSpell))
                runtime.ownedHungerAura = player->AddAura(HungerSpell, player);
        }

        void SendAddon(Player* player, Runtime const& runtime, bool running)
        {
            if (!runtime.addon)
                return;
            uint32 flags = 0;
            if (running)
            {
                if (runtime.ownedHungerAura && player->GetAura(HungerSpell, player->GetGUID()) == runtime.ownedHungerAura)
                    flags |= 1;
                if (!runtime.state.water && runtime.state.healthCeiling < Full)
                    flags |= 2;
                if (runtime.ownedColdAura && player->GetAura(ColdSpell, player->GetGUID()) == runtime.ownedColdAura)
                    flags |= 4;
            }
            std::string payload = Acore::StringFormat("SYLSURV\tV2;{};{};{};{};{};{}", running ? 1 : 0,
                runtime.state.food, runtime.state.water, runtime.state.cold, runtime.state.healthCeiling, flags);
            WorldPacket packet;
            ChatHandler::BuildChatPacket(packet, CHAT_MSG_WHISPER, LANG_ADDON, player, player, payload);
            player->SendDirectMessage(&packet);
        }

        void Report(Player* player, Runtime const& runtime)
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                French(player) ? "|cff80d050[Survival]|r Nourriture : {} % — Hydratation : {} % — Froid : {} %."
                              : "|cff80d050[Survival]|r Food: {}% — Hydration: {}% — Cold: {}%.",
                runtime.state.food / 10000, runtime.state.water / 10000, runtime.state.cold / 10000);
        }

        bool IsPaused(Player* player, Settings const& config)
        {
            return !player->IsAlive() || player->IsInFlight() || player->IsGameMaster()
                || (config.pauseAfk && player->isAFK()) || player->GetMap()->IsBattlegroundOrArena();
        }

        Environment ReadEnvironment(Player* player, Settings const& config)
        {
            Environment env;
            Map* map = player->GetMap();
            env.paused = IsPaused(player, config);
            if (env.paused)
                return env;

            for (auto const& entry : player->GetAppliedAuras())
            {
                Aura* aura = entry.second->GetBase();
                if (aura->GetCastItemGUID().IsEmpty())
                    continue;
                SpellSpecificType type = aura->GetSpellInfo()->GetSpellSpecific();
                env.eating |= type == SPELL_SPECIFIC_FOOD || type == SPELL_SPECIFIC_FOOD_AND_DRINK;
                env.drinking |= type == SPELL_SPECIFIC_DRINK || type == SPELL_SPECIFIC_FOOD_AND_DRINK;
            }
            // Dungeon cold mechanics belong to their encounters, not to outdoor Survival.
            env.cold = !map->Instanceable() && config.coldZones.count(player->GetZoneId());
            env.hot = !map->Instanceable() && config.hotZones.count(player->GetZoneId());
            env.wet = player->IsInWater();
            env.sheltered = !player->IsOutdoors() || player->HasPlayerFlag(PLAYER_FLAGS_RESTING);
            if (!env.sheltered && env.cold)
            {
                GameObject* fire = nullptr;
                auto isWarmFire = [player, &config](GameObject* object)
                {
                    bool cookingFire = object->GetGoType() == GAMEOBJECT_TYPE_SPELL_FOCUS
                        && object->GetGOInfo()->spellFocus.focusId == 4;
                    bool listed = std::find(config.fires.begin(), config.fires.end(), object->GetEntry()) != config.fires.end();
                    return (cookingFire || listed) && object->isSpawned()
                        && player->IsWithinDistInMap(object, 8.0f) && player->IsWithinLOSInMap(object);
                };
                Acore::GameObjectLastSearcher<decltype(isWarmFire)> searcher(player, fire, isWarmFire);
                Cell::VisitObjects(player, searcher, 8.0f);
                env.atFire = fire != nullptr;
            }
            return env;
        }

        void WarnChanges(Player* player, State const& before, State const& after)
        {
            if (NeedBand(after.food) > NeedBand(before.food))
                Message(player, after.food == 0 ? "Vous êtes affamé : vos cinq caractéristiques principales sont réduites de 50 %. Mangez !" : "Vous avez faim. Pensez à manger.",
                    after.food == 0 ? "You are starving: all five primary stats are reduced by 50%. Eat!" : "You are hungry. Remember to eat.");
            if (NeedBand(after.water) > NeedBand(before.water))
                Message(player, after.water == 0 ? "Vous êtes déshydraté : buvez ! Votre vie va descendre jusqu'à 1 %." : "Vous avez soif. Pensez à boire.",
                    after.water == 0 ? "You are dehydrated: drink! Your health will drop towards 1%." : "You are thirsty. Remember to drink.");
            if (ColdBand(after.cold) > ColdBand(before.cold))
                Message(player, after.cold >= Full * 3 / 4 ? "Le froid vous ralentit. Cherchez un abri ou un feu de cuisine." : "Vous commencez à avoir froid.",
                    after.cold >= Full * 3 / 4 ? "The cold is slowing you down. Find shelter or a cooking fire." : "You are starting to feel cold.");
        }

        uint32 Duration(char const* key, uint32 fallback)
        {
            return std::clamp(sConfigMgr->GetOption<uint32>(key, fallback), 30u, 86400u);
        }

        std::vector<uint32> ParseIds(std::string text)
        {
            std::replace(text.begin(), text.end(), ',', ' ');
            std::istringstream input(text);
            std::vector<uint32> values;
            uint32 value;
            while (input >> value)
                if (value)
                    values.push_back(value);
            return values;
        }

        class SurvivalWorld : public WorldScript
        {
        public:
            SurvivalWorld() : WorldScript("sylvania_survival_world") { }

            void OnStartup() override
            {
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(HungerSpell);
                hungerReady = spell && spell->IsPassive() && spell->Dispel == DISPEL_NONE
                    && spell->Effects[EFFECT_0].Effect == SPELL_EFFECT_APPLY_AURA
                    && spell->Effects[EFFECT_0].ApplyAuraName == SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE
                    && spell->Effects[EFFECT_0].MiscValue == -1
                    && spell->Effects[EFFECT_0].CalcValue() == -50
                    && !spell->Effects[EFFECT_1].IsEffect() && !spell->Effects[EFFECT_2].IsEffect();
                if (!hungerReady)
                    LOG_ERROR("module.survival", "Survival disabled: dedicated hunger aura 900101 is missing or invalid. Apply sql/world/2026_11_07_00_survival_hunger.sql and restart R1.");
                auto config = settings.load();
                LOG_INFO("module.survival", "Sylvania Survival v0.2 loaded: enabled={}, hunger aura ready={}, altar={}, completion level={}; opt-in characters only.",
                    IsAvailable(), hungerReady.load(), config->altarEntry, config->stopLevel);
            }

            void OnAfterConfigLoad(bool /*reload*/) override
            {
                auto config = std::make_shared<Settings>();
                config->enabled = CanRun(sConfigMgr->GetOption<bool>("Survival.Enabled", false),
                    sConfigMgr->GetOption<uint32>("RealmID", 0), sConfigMgr->GetOption<uint32>("Survival.RealmID", 1));
                config->pauseAfk = sConfigMgr->GetOption<bool>("Survival.PauseAFK", false);
                config->stopLevel = std::clamp(sConfigMgr->GetOption<uint32>("Survival.StopLevel", 80), 2u, 80u);
                config->altarEntry = sConfigMgr->GetOption<uint32>("Survival.AltarEntry", 254605);
                config->rules.foodSeconds = Duration("Survival.FoodSeconds", 5400);
                config->rules.waterSeconds = Duration("Survival.WaterSeconds", 3600);
                config->rules.mealSeconds = Duration("Survival.MealSeconds", 30);
                config->rules.coldSeconds = Duration("Survival.ColdSeconds", 600);
                config->rules.warmSeconds = Duration("Survival.WarmSeconds", 120);
                auto cold = ParseIds(sConfigMgr->GetOption<std::string>("Survival.ColdZones", "1,65,67,210,618"));
                auto hot = ParseIds(sConfigMgr->GetOption<std::string>("Survival.HotZones", "3,46,51,440,3483"));
                config->coldZones.insert(cold.begin(), cold.end());
                config->hotZones.insert(hot.begin(), hot.end());
                config->fires = ParseIds(sConfigMgr->GetOption<std::string>("Survival.CampfireEntries", "2061"));
                settings.store(config);
            }
        };

        class SurvivalPlayer : public PlayerScript
        {
        public:
            SurvivalPlayer() : PlayerScript("sylvania_survival_player") { }

            bool OnPlayerCanUseChat(Player* player, uint32 /*type*/, uint32 language, std::string& message, Player* receiver) override
            {
                if (receiver != player || language != LANG_ADDON || message != "SYLSURV\tHELLO")
                    return true;
                if (Runtime* runtime = GetRuntime(player))
                {
                    runtime->addon = true;
                    SendAddon(player, *runtime, IsAvailable() && runtime->state.active);
                }
                return false;
            }

            void OnPlayerLogin(Player* player) override
            {
                if (!IsAvailable())
                    return;
                // A read never creates a Survival settings row on a non-participant.
                QueryResult result = CharacterDatabase.Query(
                    "SELECT data FROM character_settings WHERE guid = {} AND source = 'mod-survival'",
                    player->GetGUID().GetCounter());
                if (!result)
                    return;
                uint32 status = 0, food = Full, water = Full, cold = 0, version = 0;
                std::istringstream data(result->Fetch()[0].Get<std::string>());
                if (!(data >> status >> food >> water >> cold >> version) || status != 1 || version != 1)
                    return;
                auto runtime = new Runtime;
                runtime->state = { true, std::min(food, Full), std::min(water, Full), std::min(cold, Full) };
                uint32 ceiling = Full;
                data >> ceiling;
                // Migrate the legacy food-only health cap: hunger now affects stats instead.
                runtime->state.healthCeiling = runtime->state.water ? Full : std::clamp(ceiling, Full / 100, Full);
                player->CustomData.Set(RuntimeKey, runtime);
                if (player->GetLevel() >= settings.load()->stopLevel)
                    Complete(player, *runtime);
                else
                {
                    if (!IsPaused(player, *settings.load()))
                        UpdateHunger(player, *runtime);
                    Message(player, "Défi actif : gérez votre faim, votre soif et votre exposition au froid. État : .survival",
                        "Challenge active: manage hunger, thirst and cold exposure. Status: .survival");
                    Report(player, *runtime);
                }
            }

            void OnPlayerUpdate(Player* player, uint32 diff) override
            {
                Runtime* runtime = GetRuntime(player);
                if (!runtime || !runtime->state.active)
                    return;
                auto config = settings.load();
                if (!IsAvailable())
                {
                    RemoveOwnedCold(player, *runtime);
                    RemoveOwnedHunger(player, *runtime);
                    SendAddon(player, *runtime, false);
                    runtime->addon = false;
                    return;
                }
                if (player->GetLevel() >= config->stopLevel)
                {
                    Complete(player, *runtime);
                    return;
                }
                if (!IsPaused(player, *config))
                {
                    uint32 limit = HealthLimit(player->GetMaxHealth(), runtime->state.healthCeiling);
                    if (player->GetHealth() > limit)
                        player->SetHealth(limit);
                }
                // No catch-up damage after a server stall, loading screen or reconnect.
                runtime->elapsedMs += std::min(diff, StepMs);
                if (runtime->elapsedMs < StepMs)
                    return;
                runtime->elapsedMs %= StepMs;
                Environment env = ReadEnvironment(player, *config);
                if (env.paused)
                {
                    RemoveOwnedCold(player, *runtime);
                    RemoveOwnedHunger(player, *runtime);
                    SendAddon(player, *runtime, false);
                    return;
                }
                State before = runtime->state;
                Advance(runtime->state, config->rules, env, StepMs / 1000);
                UpdateHunger(player, *runtime);
                WarnChanges(player, before, runtime->state);
                if (!player->GetMap()->Instanceable())
                    UpdateCold(player, *runtime);
                else
                    RemoveOwnedCold(player, *runtime);
                uint32 limit = HealthLimit(player->GetMaxHealth(), runtime->state.healthCeiling);
                if (player->GetHealth() > limit)
                    player->SetHealth(limit);
                uint32 drain = 0;
                if (runtime->state.cold == Full && env.cold && !env.sheltered && !env.atFire)
                    drain = std::max(drain, 1u);
                player->SetHealth(HealthAfterDrain(player->GetHealth(), player->GetMaxHealth(), drain));
                Save(player, *runtime);
                SendAddon(player, *runtime, true);
            }

            void OnPlayerJustDied(Player* player) override
            {
                if (Runtime* runtime = GetRuntime(player))
                {
                    RemoveOwnedCold(player, *runtime);
                    RemoveOwnedHunger(player, *runtime);
                }
            }

            void OnPlayerLogout(Player* player) override
            {
                if (Runtime* runtime = GetRuntime(player))
                {
                    RemoveOwnedCold(player, *runtime);
                    RemoveOwnedHunger(player, *runtime);
                    Save(player, *runtime);
                }
            }

            void OnPlayerSave(Player* player) override
            {
                if (Runtime* runtime = GetRuntime(player))
                    Save(player, *runtime);
            }

            static void Complete(Player* player, Runtime& runtime)
            {
                runtime.state.active = false;
                runtime.persistedStatus = 2;
                RemoveOwnedCold(player, runtime);
                RemoveOwnedHunger(player, runtime);
                Save(player, runtime);
                SendAddon(player, runtime, false);
                Message(player, "Défi Survival accompli ! Les contraintes de survie sont levées.",
                    "Survival challenge completed! Survival restrictions are lifted.");
            }
        };

        class SurvivalHealing : public UnitScript
        {
        public:
            SurvivalHealing() : UnitScript("sylvania_survival_healing") { }

            void OnHeal(Unit* /*healer*/, Unit* receiver, uint32& gain) override
            {
                Player* player = receiver ? receiver->ToPlayer() : nullptr;
                Runtime* runtime = GetRuntime(player);
                auto config = settings.load();
                if (!runtime || !runtime->state.active || !IsAvailable()
                    || player->GetLevel() >= config->stopLevel || IsPaused(player, *config))
                    return;
                gain = AllowedHeal(player->GetHealth(), player->GetMaxHealth(), runtime->state.healthCeiling, gain);
            }
        };

        class SurvivalCommands : public CommandScript
        {
        public:
            SurvivalCommands() : CommandScript("sylvania_survival_commands") { }

            Acore::ChatCommands::ChatCommandTable GetCommands() const override
            {
                using namespace Acore::ChatCommands;
                return { { "survival", HandleStatus, SEC_PLAYER, Console::No } };
            }

            static bool HandleStatus(ChatHandler* handler, char const* args)
            {
                Player* player = handler->GetPlayer();
                Runtime* runtime = GetRuntime(player);
                if (!runtime || !runtime->state.active || !IsAvailable())
                {
                    Message(player, "Aucun défi Survival actif sur ce personnage.", "No active Survival challenge on this character.");
                    return true;
                }
                if (std::string_view(args) == "hud")
                {
                    runtime->addon = true;
                    SendAddon(player, *runtime, true);
                }
                else
                    Report(player, *runtime);
                return true;
            }
        };
    }

    bool IsAvailable()
    {
        return settings.load()->enabled && hungerReady.load();
    }

    bool IsParticipant(Player* player)
    {
        if (!player || !IsAvailable())
            return false;
        if (Runtime* runtime = GetRuntime(player))
            return runtime->state.active && player->GetLevel() < settings.load()->stopLevel;
        // Channel checks can precede our login hook. Never materialize a zero-valued row.
        QueryResult row = CharacterDatabase.Query(
            "SELECT data FROM character_settings WHERE guid = {} AND source = 'mod-survival'",
            player->GetGUID().GetCounter());
        if (!row)
            return false;
        std::istringstream data(row->Fetch()[0].Get<std::string>());
        uint32 status = 0;
        data >> status;
        return status == 1 && player->GetLevel() < settings.load()->stopLevel;
    }

    void AddAltarOption(Player* player)
    {
        if (!IsAvailable())
            return;
        QueryResult row = CharacterDatabase.Query(
            "SELECT data FROM character_settings WHERE guid = {} AND source = 'mod-survival'", player->GetGUID().GetCounter());
        bool chosen = bool(row);
        if (!CanActivate(true, chosen, player->GetLevel(), player->getClass() == CLASS_DEATH_KNIGHT))
            return;
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, French(player) ? "Activer le Mode Survival (Faim, soif et froid)" : "Enable Survival Mode (Hunger, thirst and cold)",
            0, ActivationAction,
            French(player) ? "Gérez la survie jusqu'au niveau 80. Nourriture vide : -50 % aux cinq caractéristiques. Hydratation vide : vie jusqu'à 1 %. Ce choix est définitif."
                           : "Survive until level 80. Empty food: -50% to all five stats. Empty hydration: health towards 1%. This choice is permanent.", 0, false);
    }

    bool ActivateAtAltar(Player* player, GameObject* altar)
    {
        auto config = settings.load();
        if (!player || !altar || !IsAvailable() || altar->GetEntry() != config->altarEntry
            || !player->IsAlive() || player->IsInCombat() || !player->IsWithinDistInMap(altar, INTERACTION_DISTANCE))
            return false;
        QueryResult row = CharacterDatabase.Query(
            "SELECT data FROM character_settings WHERE guid = {} AND source = 'mod-survival'", player->GetGUID().GetCounter());
        bool chosen = bool(row);
        if (!CanActivate(true, chosen, player->GetLevel(), player->getClass() == CLASS_DEATH_KNIGHT))
            return false;
        auto runtime = new Runtime;
        runtime->state.active = true;
        player->CustomData.Set(RuntimeKey, runtime);
        Save(player, *runtime);
        player->SaveToDB(false, false);
        Message(player, "Survival activé ! Mangez et buvez en restant assis. Abritez-vous ou approchez d'un feu de cuisine pour vous réchauffer.",
            "Survival enabled! Eat and drink while seated. Seek shelter or a cooking fire to warm up.");
        Report(player, *runtime);
        return true;
    }
}

void AddSylvaniaSurvivalScripts()
{
    new SylvaniaSurvival::SurvivalWorld();
    new SylvaniaSurvival::SurvivalPlayer();
    new SylvaniaSurvival::SurvivalCommands();
    new SylvaniaSurvival::SurvivalHealing();
}
