/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ChallengeModes.h"
#include "Survival.h"
#include "World.h"
#include "WorldSessionMgr.h"
#include "Player.h"
#include "Chat.h"
#include "Channel.h"
#include "ChannelMgr.h"

ChallengeModes* ChallengeModes::instance()
{
    static ChallengeModes instance;
    return &instance;
}

bool ChallengeModes::challengeEnabledForPlayer(ChallengeModeSettings setting, Player* player) const
{
    if (!enabled() || !challengeEnabled(setting))
    {
        return false;
    }
    return player->GetPlayerSetting("mod-challenge-modes", setting).value;
}

bool ChallengeModes::challengeEnabled(ChallengeModeSettings setting) const
{
    switch (setting)
    {
        case SETTING_HARDCORE:
            return hardcoreEnable;
        case SETTING_SEMI_HARDCORE:
            return semiHardcoreEnable;
        case SETTING_SELF_CRAFTED:
            return selfCraftedEnable;
        case SETTING_ITEM_QUALITY_LEVEL:
            return itemQualityLevelEnable;
        case SETTING_SLOW_XP_GAIN:
            return slowXpGainEnable;
        case SETTING_VERY_SLOW_XP_GAIN:
            return verySlowXpGainEnable;
        case SETTING_QUEST_XP_ONLY:
            return questXpOnlyEnable;
        case SETTING_IRON_MAN:
            return ironManEnable;
        case HARDCORE_DEAD:
            return true;
    }
    return false;
}

uint32 ChallengeModes::getDisableLevel(ChallengeModeSettings setting) const
{
    switch (setting)
    {
        case SETTING_HARDCORE:
            return hardcoreDisableLevel;
        case SETTING_SEMI_HARDCORE:
            return semiHardcoreDisableLevel;
        case SETTING_SELF_CRAFTED:
            return selfCraftedDisableLevel;
        case SETTING_ITEM_QUALITY_LEVEL:
            return itemQualityLevelDisableLevel;
        case SETTING_SLOW_XP_GAIN:
            return slowXpGainDisableLevel;
        case SETTING_VERY_SLOW_XP_GAIN:
            return verySlowXpGainDisableLevel;
        case SETTING_QUEST_XP_ONLY:
            return questXpOnlyDisableLevel;
        case SETTING_IRON_MAN:
            return ironManDisableLevel;
        case HARDCORE_DEAD:
            break;
    }
    return 0;
}

float ChallengeModes::getXpBonusForChallenge(ChallengeModeSettings setting) const
{
    switch (setting)
    {
        case SETTING_HARDCORE:
            return hardcoreXpBonus;
        case SETTING_SEMI_HARDCORE:
            return semiHardcoreXpBonus;
        case SETTING_SELF_CRAFTED:
            return selfCraftedXpBonus;
        case SETTING_ITEM_QUALITY_LEVEL:
            return itemQualityLevelXpBonus;
        case SETTING_SLOW_XP_GAIN:
            return slowXpGainBonus;
        case SETTING_VERY_SLOW_XP_GAIN:
            return verySlowXpGainBonus;
        case SETTING_QUEST_XP_ONLY:
            return questXpOnlyXpBonus;
        case SETTING_IRON_MAN:
            return ironManXpBonus;
        case HARDCORE_DEAD:
            break;
    }
    return 1;
}

const std::unordered_map<uint8, uint32> *ChallengeModes::getTitleMapForChallenge(ChallengeModeSettings setting) const
{
    switch (setting)
    {
        case SETTING_HARDCORE:
            return &hardcoreTitleRewards;
        case SETTING_SEMI_HARDCORE:
            return &semiHardcoreTitleRewards;
        case SETTING_SELF_CRAFTED:
            return &selfCraftedTitleRewards;
        case SETTING_ITEM_QUALITY_LEVEL:
            return &itemQualityLevelTitleRewards;
        case SETTING_SLOW_XP_GAIN:
            return &slowXpGainTitleRewards;
        case SETTING_VERY_SLOW_XP_GAIN:
            return &verySlowXpGainTitleRewards;
        case SETTING_QUEST_XP_ONLY:
            return &questXpOnlyTitleRewards;
        case SETTING_IRON_MAN:
            return &ironManTitleRewards;
        case HARDCORE_DEAD:
            break;
    }
    return {};
}

const std::unordered_map<uint8, uint32> *ChallengeModes::getTalentMapForChallenge(ChallengeModeSettings setting) const
{
    switch (setting)
    {
        case SETTING_HARDCORE:
            return &hardcoreTalentRewards;
        case SETTING_SEMI_HARDCORE:
            return &semiHardcoreTalentRewards;
        case SETTING_SELF_CRAFTED:
            return &selfCraftedTalentRewards;
        case SETTING_ITEM_QUALITY_LEVEL:
            return &itemQualityLevelTalentRewards;
        case SETTING_SLOW_XP_GAIN:
            return &slowXpGainTalentRewards;
        case SETTING_VERY_SLOW_XP_GAIN:
            return &verySlowXpGainTalentRewards;
        case SETTING_QUEST_XP_ONLY:
            return &questXpOnlyTalentRewards;
        case SETTING_IRON_MAN:
            return &ironManTalentRewards;
        case HARDCORE_DEAD:
            break;
    }
    return {};
}

const std::unordered_map<uint8, uint32> *ChallengeModes::getItemMapForChallenge(ChallengeModeSettings setting) const
{
    switch (setting)
    {
        case SETTING_HARDCORE:
            return &hardcoreItemRewards;
        case SETTING_SEMI_HARDCORE:
            return &semiHardcoreItemRewards;
        case SETTING_SELF_CRAFTED:
            return &selfCraftedItemRewards;
        case SETTING_ITEM_QUALITY_LEVEL:
            return &itemQualityLevelItemRewards;
        case SETTING_SLOW_XP_GAIN:
            return &slowXpGainItemRewards;
        case SETTING_VERY_SLOW_XP_GAIN:
            return &verySlowXpGainItemRewards;
        case SETTING_QUEST_XP_ONLY:
            return &questXpOnlyItemRewards;
        case SETTING_IRON_MAN:
            return &ironManItemRewards;
        case HARDCORE_DEAD:
            break;
    }
    return {};
}

uint32 ChallengeModes::getItemRewardAmount(ChallengeModeSettings setting) const
{
    switch (setting)
    {
        case SETTING_HARDCORE:
            return hardcoreItemRewardAmount;
        case SETTING_SEMI_HARDCORE:
            return semiHardcoreItemRewardAmount;
        case SETTING_SELF_CRAFTED:
            return selfCraftedItemRewardAmount;
        case SETTING_ITEM_QUALITY_LEVEL:
            return itemQualityLevelItemRewardAmount;
        case SETTING_SLOW_XP_GAIN:
            return slowXpGainItemRewardAmount;
        case SETTING_VERY_SLOW_XP_GAIN:
            return verySlowXpGainItemRewardAmount;
        case SETTING_QUEST_XP_ONLY:
            return questXpOnlyItemRewardAmount;
        case SETTING_IRON_MAN:
            return ironManItemRewardAmount;
        case HARDCORE_DEAD:
            break;
    }
    return 0;
}

const std::unordered_map<uint8, uint32> *ChallengeModes::getAchievementMapForChallenge(ChallengeModeSettings setting) const
{
    switch (setting)
    {
        case SETTING_HARDCORE:
            return &hardcoreAchievementReward;
        case SETTING_SEMI_HARDCORE:
            return &semiHardcoreAchievementReward;
        case SETTING_SELF_CRAFTED:
            return &selfCraftedAchievementReward;
        case SETTING_ITEM_QUALITY_LEVEL:
            return &itemQualityLevelAchievementReward;
        case SETTING_SLOW_XP_GAIN:
            return &slowXpGainAchievementReward;
        case SETTING_VERY_SLOW_XP_GAIN:
            return &verySlowXpGainAchievementReward;
        case SETTING_QUEST_XP_ONLY:
            return &questXpOnlyAchievementReward;
        case SETTING_IRON_MAN:
            return &ironManAchievementReward;
        case HARDCORE_DEAD:
            break;
    }
    return {};
}

// =====================================================================================
// Canal de discussion des défis (2026-07-12)
// -------------------------------------------------------------------------------------
// ⚠️ Le client 3.3.5 N'ENREGISTRE PAS un canal rejoint côté serveur (la notif
// YOU_JOINED non sollicitée est ignorée par l'UI) et un join forcé bloque ensuite le
// /join du joueur (« déjà membre »). → Modèle « /join initié client » :
//  - activation d'un défi : annonce MONDIALE « pseudo + nom du défi » + invitation
//    à taper /join Defis (une fois fait, le client re-rejoint SEUL à chaque login) ;
//  - login (défi actif, lvl<80) : rappel /join si pas encore dans le canal ;
//  - canal réservé : sans défi actif on ne peut pas y parler et on en est retiré ;
//  - niveau 80 atteint (ou plus aucun défi) : retrait automatique du canal.
// AllowTwoSide.Interaction.Channel=1 sur le royaume → canal commun aux 2 factions.
// =====================================================================================
namespace
{
    bool IsMartyrChallengeActive(Player* player)
    {
        QueryResult result = CharacterDatabase.Query(
            "SELECT active FROM character_martyr_mode WHERE guid = {} AND active = 1",
            player->GetGUID().GetCounter());
        return (bool)result;
    }

    bool HasAnyActiveChallenge(Player* player)
    {
        for (uint8 setting = SETTING_HARDCORE; setting <= SETTING_IRON_MAN; ++setting)
        {
            // Un hardcore mort a échoué son défi : il ne compte plus pour le canal.
            if (setting == SETTING_HARDCORE && sChallengeModes->challengeEnabledForPlayer(HARDCORE_DEAD, player))
                continue;
            if (sChallengeModes->challengeEnabledForPlayer(static_cast<ChallengeModeSettings>(setting), player))
                return true;
        }
        return SylvaniaSurvival::IsParticipant(player) || IsMartyrChallengeActive(player);
    }

    Channel* GetChallengeChannel(Player* player)
    {
        if (!sChallengeModes->enabled() || !sChallengeModes->challengeChannelEnable || sChallengeModes->challengeChannelName.empty())
            return nullptr;
        if (ChannelMgr* cMgr = ChannelMgr::forTeam(player->GetTeamId()))
            return cMgr->GetJoinChannel(sChallengeModes->challengeChannelName, 0);
        return nullptr;
    }

    void RemindJoinChallengeChannel(Player* player)
    {
        if (!GetChallengeChannel(player))
            return;
        ChatHandler(player->GetSession()).PSendSysMessage(
            "|cff00ff00[Défis]|r Rejoignez le canal des joueurs en défi : tapez |cffffcc00/join {}|r (une seule fois, votre client le retiendra).",
            sChallengeModes->challengeChannelName);
    }

    void SweepFromChallengeChannel(Player* player)
    {
        Channel* channel = GetChallengeChannel(player);
        if (!channel)
            return;
        if (channel->IsMember(player->GetGUID()))
            channel->LeaveChannel(player, true);
    }

    std::string ChallengeDisplayName(uint32 action)
    {
        switch (action)
        {
            case SETTING_HARDCORE:           return "Hardcore (1 Vie)";
            case SETTING_SEMI_HARDCORE:      return "Semi-Hardcore";
            case SETTING_SELF_CRAFTED:       return "Auto-artisanat";
            case SETTING_ITEM_QUALITY_LEVEL: return "Qualité d'Objet Limitée";
            case SETTING_SLOW_XP_GAIN:       return "XP Lente (x0.5)";
            case SETTING_VERY_SLOW_XP_GAIN:  return "XP Très Lente (x0.25)";
            case SETTING_QUEST_XP_ONLY:      return "XP de Quêtes Uniquement";
            case SETTING_IRON_MAN:           return "Iron Man";
            case 100:                        return "Martyr";
            case SylvaniaSurvival::ActivationAction: return "Survival";
            default:                         return "Défi";
        }
    }

    void AnnounceChallengeActivation(Player* player, uint32 action)
    {
        std::string broadcastMsg = Acore::StringFormat(
            "|cffFFD700[Défis]|r {} se lance dans le défi : {} !",
            player->GetName(), ChallengeDisplayName(action));
        sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, broadcastMsg);
        RemindJoinChallengeChannel(player);
    }
}

class ChallengeModes_WorldScript : public WorldScript
{
public:
    ChallengeModes_WorldScript()
        : WorldScript("ChallengeModes_WorldScript")
    {}

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        LoadConfig();
    }

private:
    static void LoadStringToMap(std::unordered_map<uint8, uint32> &mapToLoad, const std::string &configString)
    {
        std::string delimitedValue;
        std::stringstream configIdStream;

        configIdStream.str(configString);
        // Process each config ID in the string, delimited by the comma - "," and then space " "
        while (std::getline(configIdStream, delimitedValue, ','))
        {
            std::string pairOne, pairTwo;
            std::stringstream configPairStream(delimitedValue);
            configPairStream>>pairOne>>pairTwo;
            auto configLevel = atoi(pairOne.c_str());
            auto rewardValue = atoi(pairTwo.c_str());
            mapToLoad[configLevel] = rewardValue;
        }
    }

    static void LoadConfig()
    {
        sChallengeModes->challengesEnabled = sConfigMgr->GetOption<bool>("ChallengeModes.Enable", false);
        if (sChallengeModes->enabled())
        {
            sChallengeModes->challengeChannelEnable = sConfigMgr->GetOption<bool>("ChallengeModes.Channel.Enable", true);
            sChallengeModes->challengeChannelName   = sConfigMgr->GetOption<std::string>("ChallengeModes.Channel.Name", "Defis");
            for (auto& [confName, rewardMap] : sChallengeModes->rewardConfigMap)
            {
                rewardMap->clear();
                LoadStringToMap(*rewardMap, sConfigMgr->GetOption<std::string>(confName, ""));
            }

            sChallengeModes->hardcoreEnable          = sConfigMgr->GetOption<bool>("Hardcore.Enable", true);
            sChallengeModes->semiHardcoreEnable      = sConfigMgr->GetOption<bool>("SemiHardcore.Enable", true);
            sChallengeModes->selfCraftedEnable       = sConfigMgr->GetOption<bool>("SelfCrafted.Enable", true);
            sChallengeModes->itemQualityLevelEnable  = sConfigMgr->GetOption<bool>("ItemQualityLevel.Enable", true);
            sChallengeModes->slowXpGainEnable        = sConfigMgr->GetOption<bool>("SlowXpGain.Enable", true);
            sChallengeModes->verySlowXpGainEnable    = sConfigMgr->GetOption<bool>("VerySlowXpGain.Enable", true);
            sChallengeModes->questXpOnlyEnable       = sConfigMgr->GetOption<bool>("QuestXpOnly.Enable", true);
            sChallengeModes->ironManEnable           = sConfigMgr->GetOption<bool>("IronMan.Enable", true);

            sChallengeModes->hardcoreDisableLevel          = sConfigMgr->GetOption<uint32>("Hardcore.DisableLevel", 0);
            sChallengeModes->semiHardcoreDisableLevel      = sConfigMgr->GetOption<uint32>("SemiHardcore.DisableLevel", 0);
            sChallengeModes->selfCraftedDisableLevel       = sConfigMgr->GetOption<uint32>("SelfCrafted.DisableLevel", 0);
            sChallengeModes->itemQualityLevelDisableLevel  = sConfigMgr->GetOption<uint32>("ItemQualityLevel.DisableLevel", 0);
            sChallengeModes->slowXpGainDisableLevel        = sConfigMgr->GetOption<uint32>("SlowXpGain.DisableLevel", 0);
            sChallengeModes->verySlowXpGainDisableLevel    = sConfigMgr->GetOption<uint32>("VerySlowXpGain.DisableLevel", 0);
            sChallengeModes->questXpOnlyDisableLevel       = sConfigMgr->GetOption<uint32>("QuestXpOnly.DisableLevel", 0);
            sChallengeModes->ironManDisableLevel           = sConfigMgr->GetOption<uint32>("IronMan.DisableLevel", 0);

            sChallengeModes->hardcoreXpBonus         = sConfigMgr->GetOption<float>("Hardcore.XPMultiplier", 1.0f);
            sChallengeModes->semiHardcoreXpBonus     = sConfigMgr->GetOption<float>("SemiHardcore.XPMultiplier", 1.0f);
            sChallengeModes->selfCraftedXpBonus      = sConfigMgr->GetOption<float>("SelfCrafted.XPMultiplier", 1.0f);
            sChallengeModes->itemQualityLevelXpBonus = sConfigMgr->GetOption<float>("ItemQualityLevel.XPMultiplier", 1.0f);
            sChallengeModes->questXpOnlyXpBonus      = sConfigMgr->GetOption<float>("QuestXpOnly.XPMultiplier", 1.0f);
            sChallengeModes->slowXpGainBonus         = sConfigMgr->GetOption<float>("SlowXpGain.XPMultiplier", 0.50f);
            sChallengeModes->verySlowXpGainBonus     = sConfigMgr->GetOption<float>("VerySlowXpGain.XPMultiplier", 0.25f);
            sChallengeModes->ironManXpBonus          = sConfigMgr->GetOption<float>("IronMan.XPMultiplier", 1.0f);

            sChallengeModes->hardcoreItemRewardAmount         = sConfigMgr->GetOption<uint32>("Hardcore.ItemRewardAmount", 1);
            sChallengeModes->semiHardcoreItemRewardAmount     = sConfigMgr->GetOption<uint32>("SemiHardcore.ItemRewardAmount", 1);
            sChallengeModes->selfCraftedItemRewardAmount      = sConfigMgr->GetOption<uint32>("SelfCrafted.ItemRewardAmount", 1);
            sChallengeModes->itemQualityLevelItemRewardAmount = sConfigMgr->GetOption<uint32>("ItemQualityLevel.ItemRewardAmount", 1);
            sChallengeModes->slowXpGainItemRewardAmount       = sConfigMgr->GetOption<uint32>("SlowXpGain.ItemRewardAmount", 1);
            sChallengeModes->verySlowXpGainItemRewardAmount   = sConfigMgr->GetOption<uint32>("VerySlowXpGain.ItemRewardAmount", 1);
            sChallengeModes->questXpOnlyItemRewardAmount      = sConfigMgr->GetOption<uint32>("QuestXpOnly.ItemRewardAmount", 1);
            sChallengeModes->ironManItemRewardAmount          = sConfigMgr->GetOption<uint32>("IronMan.ItemRewardAmount", 1);

            LoadStringToMap(sChallengeModes->hardcoreAchievementReward, sConfigMgr->GetOption<std::string>("Hardcore.AchievementReward", ""));
            LoadStringToMap(sChallengeModes->semiHardcoreAchievementReward, sConfigMgr->GetOption<std::string>("SemiHardcore.AchievementReward", ""));
            LoadStringToMap(sChallengeModes->selfCraftedAchievementReward, sConfigMgr->GetOption<std::string>("SelfCrafted.AchievementReward", ""));
            LoadStringToMap(sChallengeModes->itemQualityLevelAchievementReward, sConfigMgr->GetOption<std::string>("ItemQualityLevel.AchievementReward", ""));
            LoadStringToMap(sChallengeModes->slowXpGainAchievementReward, sConfigMgr->GetOption<std::string>("SlowXpGain.AchievementReward", ""));
            LoadStringToMap(sChallengeModes->verySlowXpGainAchievementReward, sConfigMgr->GetOption<std::string>("VerySlowXpGain.AchievementReward", ""));
            LoadStringToMap(sChallengeModes->questXpOnlyAchievementReward, sConfigMgr->GetOption<std::string>("QuestXpOnly.AchievementReward", ""));
            LoadStringToMap(sChallengeModes->ironManAchievementReward, sConfigMgr->GetOption<std::string>("IronMan.AchievementReward", ""));
        }
    }
};

class ChallengeMode : public PlayerScript
{
public:
    explicit ChallengeMode(const char *scriptName,
                           ChallengeModeSettings settingName)
            : PlayerScript(scriptName), settingName(settingName)
    { }

    static bool mapContainsKey(const std::unordered_map<uint8, uint32>* mapToCheck, uint8 key)
    {
        return (mapToCheck->find(key) != mapToCheck->end());
    }

    // Récompense de complétion au niveau 80 (coins + objet par courrier). Idempotente via un
    // marqueur durable "mod-cm-rewarded" (une seule fois par personnage et par mode) et
    // journalisée (LOG_INFO [REWARD] GIVEN). Appelée par le chemin naturel
    // (OnPlayerLevelChanged) ET par le filet de sécurité au login (OnPlayerLogin).
    void grantCompletionReward(Player* player)
    {
        if (player->GetPlayerSetting("mod-cm-rewarded", settingName).value)
        {
            return; // déjà récompensé : ne rien faire
        }

        uint32 coinsReward = 0;
        std::string modeName = "";
        switch (settingName)
        {
            case SETTING_HARDCORE:      coinsReward = 2000; modeName = "HARDCORE";        break;
            case SETTING_SEMI_HARDCORE: coinsReward = 1000; modeName = "Semi-Hardcore";   break;
            case SETTING_IRON_MAN:      coinsReward = 200;  modeName = "Iron Man";        break;
            case SETTING_SELF_CRAFTED:  coinsReward = 200;  modeName = "Auto-Artisannat"; break;
            default:                    coinsReward = 200;  modeName = "Défi";            break;
        }

        std::string rewardList = Acore::StringFormat("{} Sylvania Coins", coinsReward);

        // Créditer les Sylvania Coins
        if (coinsReward > 0)
        {
            LoginDatabase.Execute("UPDATE account SET vp = vp + {} WHERE id = {}", coinsReward, player->GetSession()->GetAccountId());
        }

        // Objet de complétion (ex. Deuillegivre en HC) via la config, envoyé par COURRIER (durable)
        const std::unordered_map<uint8, uint32>* itemRewardMap = sChallengeModes->getItemMapForChallenge(settingName);
        if (mapContainsKey(itemRewardMap, 80))
        {
            uint32 itemEntry = itemRewardMap->at(80);
            uint32 itemAmount = sChallengeModes->getItemRewardAmount(settingName);
            player->SendItemRetrievalMail({ { itemEntry, itemAmount } });
            if (settingName == SETTING_HARDCORE)
            {
                rewardList += " + Deuillegivre (Liée au compte)";
            }
            else
            {
                rewardList += Acore::StringFormat(" + objet #{}", itemEntry);
            }
        }

        ChatHandler(player->GetSession()).PSendSysMessage("|cff00ff00[Récompense]|r Félicitations ! Vous avez atteint le niveau 80 en mode {}. Vous avez gagné : {} !", modeName, rewardList);

        // Annonce Mondiale (Dorée)
        std::string broadcastMsg = Acore::StringFormat("|cffFFD700[Sylvania]|r FÉLICITATIONS ! Le personnage {} a triomphé du mode {} en atteignant le niveau 80 !", player->GetName(), modeName);
        sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, broadcastMsg);

        // Journal d'audit
        LOG_INFO("mod-challenge-modes", "[REWARD] GIVEN mode={} char={} guid={} account={} coins={}",
                 modeName, player->GetName(), player->GetGUID().GetCounter(), player->GetSession()->GetAccountId(), coinsReward);

        // Marqueur durable : récompense donnée (évite tout doublon)
        player->UpdatePlayerSetting("mod-cm-rewarded", settingName, 1);
    }

    void OnPlayerGiveXP(Player* player, uint32& amount, Unit* /*victim*/, uint8 /*xpSource*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(settingName, player))
        {
            return;
        }
        amount *= sChallengeModes->getXpBonusForChallenge(settingName);
    }

    void OnPlayerLevelChanged(Player* player, uint8 oldlevel) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(settingName, player))
        {
            return;
        }

        // 🛡️ Anti-exploit (2026-05-30) : ne grant les récompenses (coins, titres, talents,
        // achievements, objets) QUE si le saut de niveau est naturel (+1 via XP).
        // Empêche les rewards de fire sur .character level X (GM/SOAP/Sésame 80), ce
        // qui permettrait d'activer N modes au lvl 1 puis insta-80 pour farmer les SC.
        if (oldlevel + 1 != player->GetLevel())
        {
            return;
        }

        const std::unordered_map<uint8, uint32>* titleRewardMap = sChallengeModes->getTitleMapForChallenge(settingName);
        const std::unordered_map<uint8, uint32>* talentRewardMap = sChallengeModes->getTalentMapForChallenge(settingName);
        const std::unordered_map<uint8, uint32>* itemRewardMap = sChallengeModes->getItemMapForChallenge(settingName);
        const std::unordered_map<uint8, uint32>* achievementRewardMap = sChallengeModes->getAchievementMapForChallenge(settingName);
        uint8 level = player->GetLevel();

        // Récompense de complétion au niveau 80 (coins + objet + annonce), idempotente et journalisée
        if (level == 80)
        {
            grantCompletionReward(player);
        }

        if (mapContainsKey(titleRewardMap, level))
    {
        CharTitlesEntry const* titleInfo = sCharTitlesStore.LookupEntry(titleRewardMap->at(level));
        if (!titleInfo)
        {
            LOG_ERROR("mod-challenge-modes", "Invalid title ID {}!", titleRewardMap->at(level));
            return;
        }
        ChatHandler handler(player->GetSession());
        std::string tNameLink = handler.GetNameLink(player);
        std::string titleNameStr = Acore::StringFormat(player->getGender() == GENDER_MALE ? titleInfo->nameMale[handler.GetSessionDbcLocale()] : titleInfo->nameFemale[handler.GetSessionDbcLocale()], player->GetName());
        player->SetTitle(titleInfo);
    }

    if (mapContainsKey(talentRewardMap, level))
    {
        player->RewardExtraBonusTalentPoints(talentRewardMap->at(level));
    }

    if (mapContainsKey(achievementRewardMap, level))
    {
        AchievementEntry const* achievementInfo = sAchievementStore.LookupEntry(achievementRewardMap->at(level));
        if (!achievementInfo)
        {
            LOG_ERROR("mod-challenge-modes", "Invalid Achievement ID {}!", achievementRewardMap->at(level));
            return;
        }

        ChatHandler handler(player->GetSession());
        std::string tNameLink = handler.GetNameLink(player);
        player->CompletedAchievement(achievementInfo);
    }

    if (level != 80 && mapContainsKey(itemRewardMap, level))
    {
        // (Le niveau 80 est traité par grantCompletionReward ci-dessus pour éviter tout doublon)
        uint32 itemEntry = itemRewardMap->at(level);
        uint32 itemAmount = sChallengeModes->getItemRewardAmount(settingName); // Fetch item amount from config
        player->SendItemRetrievalMail({ { itemEntry, itemAmount } });
    }

    if (sChallengeModes->getDisableLevel(settingName) && sChallengeModes->getDisableLevel(settingName) <= level)
    {
        player->UpdatePlayerSetting("mod-challenge-modes", settingName, 0);
    }
}

private:
    ChallengeModeSettings settingName;
};

class ChallengeMode_Hardcore : public ChallengeMode
{
public:
    ChallengeMode_Hardcore() : ChallengeMode("ChallengeMode_Hardcore", SETTING_HARDCORE) {}

    void OnPlayerLogin(Player* player) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_HARDCORE, player))
        {
            return;
        }

        // Vérification persistante (Simulation de bannissement personnage)
        QueryResult result = CharacterDatabase.Query("SELECT is_dead FROM character_challenges WHERE guid = {} AND mode = {}", player->GetGUID().GetCounter(), SETTING_HARDCORE);
        if (result)
        {
            Field* fields = result->Fetch();
            if (fields[0].Get<uint8>() == 1)
            {
                player->KillPlayer();
                player->SetPhaseMask(0, true); // Invisibilité totale / Hors du monde
                player->GetSession()->KickPlayer(std::string("Ce personnage est banni. Raison : Mort permanente en mode Hardcore."));
                return;
            }
        }

        if (sChallengeModes->challengeEnabledForPlayer(HARDCORE_DEAD, player))
        {
            player->KillPlayer();
            player->GetSession()->KickPlayer(std::string("Mort d'un personnage Hardcore."));
            return;
        }

        // Filet de sécurité récompense HC : rattrape une récompense manquée (arrivée à 80 non
        // captée par le hook de niveau, incident de restart, etc.). Idempotent via le marqueur
        // "mod-cm-rewarded". Anti-farm : exige un temps de jeu réel — un insta-80 boosté a un
        // /played quasi nul et n'est donc jamais rattrapé ici.
        if (player->GetLevel() == 80
            && !player->GetPlayerSetting("mod-cm-rewarded", SETTING_HARDCORE).value
            && player->GetTotalPlayedTime() >= 18000) // >= 5h de jeu effectif
        {
            LOG_INFO("mod-challenge-modes", "[REWARD] RECONCILE-LOGIN char={} guid={} account={} played={}s",
                     player->GetName(), player->GetGUID().GetCounter(), player->GetSession()->GetAccountId(), player->GetTotalPlayedTime());
            grantCompletionReward(player);
        }
    }

    void OnPlayerReleasedGhost(Player* player) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_HARDCORE, player))
        {
            return;
        }

        player->UpdatePlayerSetting("mod-challenge-modes", HARDCORE_DEAD, 1);
        CharacterDatabase.Execute("REPLACE INTO character_challenges (guid, mode, is_dead) VALUES ({}, {}, 1)", player->GetGUID().GetCounter(), SETTING_HARDCORE);

        std::string broadcastMsg = Acore::StringFormat("|cffff0000[Challenge]|r Le personnage {} est mort de façon PERMANENTE au niveau {} en mode Hardcore !", player->GetName(), player->GetLevel());
        sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, broadcastMsg);

        player->GetSession()->KickPlayer(std::string("Mort permanente (Hardcore) : Votre âme a quitté le monde."));
    }

    void OnPlayerPVPKill(Player* /*killer*/, Player* killed) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_HARDCORE, killed))
        {
            return;
        }
        killed->UpdatePlayerSetting("mod-challenge-modes", HARDCORE_DEAD, 1);
        CharacterDatabase.Execute("REPLACE INTO character_challenges (guid, mode, is_dead) VALUES ({}, {}, 1)", killed->GetGUID().GetCounter(), SETTING_HARDCORE);
    }

    void OnPlayerKilledByCreature(Creature* /*killer*/, Player* killed) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_HARDCORE, killed))
        {
            return;
        }
        killed->UpdatePlayerSetting("mod-challenge-modes", HARDCORE_DEAD, 1);
        CharacterDatabase.Execute("REPLACE INTO character_challenges (guid, mode, is_dead) VALUES ({}, {}, 1)", killed->GetGUID().GetCounter(), SETTING_HARDCORE);
    }

    void OnPlayerResurrect(Player* player, float /*restore_percent*/, bool /*applySickness*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_HARDCORE, player))
        {
            return;
        }

        // Blocage immédiat de la résurrection
        player->UpdatePlayerSetting("mod-challenge-modes", HARDCORE_DEAD, 1);
        CharacterDatabase.Execute("REPLACE INTO character_challenges (guid, mode, is_dead) VALUES ({}, {}, 1)", player->GetGUID().GetCounter(), SETTING_HARDCORE);

        std::string broadcastMsg = Acore::StringFormat("|cffff0000[Challenge]|r Le personnage {} est mort au niveau {} en mode Hardcore ! (Tentative de résurrection bloquée)", player->GetName(), player->GetLevel());
        sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, broadcastMsg);

        player->KillPlayer();
        player->GetSession()->KickPlayer(std::string("Résurrection interdite en mode Hardcore."));
    }

    void OnPlayerGiveXP(Player* player, uint32& amount, Unit* victim, uint8 xpSource) override
    {
        ChallengeMode::OnPlayerGiveXP(player, amount, victim, xpSource);
    }

    void OnPlayerLevelChanged(Player* player, uint8 oldlevel) override
    {
        ChallengeMode::OnPlayerLevelChanged(player, oldlevel);
    }
};

class ChallengeMode_SemiHardcore : public ChallengeMode
{
public:
    ChallengeMode_SemiHardcore() : ChallengeMode("ChallengeMode_SemiHardcore", SETTING_SEMI_HARDCORE) {}

    void DoPenalty(Player* player)
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_SEMI_HARDCORE, player))
        {
            return;
        }
        
        bool lostItem = false;
        for (uint8 i = 0; i < EQUIPMENT_SLOT_END; ++i)
        {
            if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            {
                if (pItem->GetTemplate() && !pItem->IsEquipped()) continue;
                uint8 slot = pItem->GetSlot();
                ChatHandler(player->GetSession()).PSendSysMessage("|cffDA70D6%s |cffffffff|Hitem:%d:0:0:0:0:0:0:0:0|h[%s]|h|r", "Vous avez perdu votre équipement :", pItem->GetEntry(), pItem->GetTemplate()->Name1.c_str());
                player->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);
                lostItem = true;
            }
        }
        
        if (player->GetMoney() > 0) {
            player->SetMoney(0);
            lostItem = true;
        }
        
        if (lostItem) {
             ChatHandler(player->GetSession()).PSendSysMessage("|cffFF0000[Semi-Hardcore]|r Vous êtes tombé au combat. La Faucheuse a réclamé votre or et tout l'équipement que vous portiez !");
             std::string broadcastMsg = Acore::StringFormat("|cffff0000[Challenge]|r Le personnage {} a péri au niveau {} en mode Semi-Hardcore et a tout perdu !", player->GetName(), player->GetLevel());
             sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, broadcastMsg);
        }
    }

    void OnPlayerKilledByCreature(Creature* /*killer*/, Player* player) override
    {
        DoPenalty(player);
    }
    
    void OnPlayerPVPKill(Player* /*killer*/, Player* player) override
    {
        DoPenalty(player);
    }
    
    void OnPlayerReleasedGhost(Player* player) override
    {
        DoPenalty(player);
    }
    
    void OnPlayerResurrect(Player* player, float /*restore_percent*/, bool /*applySickness*/) override
    {
        DoPenalty(player);
    }

    void OnPlayerGiveXP(Player* player, uint32& amount, Unit* victim, uint8 xpSource) override
    {
        ChallengeMode::OnPlayerGiveXP(player, amount, victim, xpSource);
    }

    void OnPlayerLevelChanged(Player* player, uint8 oldlevel) override
    {
        ChallengeMode::OnPlayerLevelChanged(player, oldlevel);
    }
};

class ChallengeMode_SelfCrafted : public ChallengeMode
{
public:
    ChallengeMode_SelfCrafted() : ChallengeMode("ChallengeMode_SelfCrafted", SETTING_SELF_CRAFTED) {}

    bool OnPlayerCanEquipItem(Player* player, uint8 /*slot*/, uint16& /*dest*/, Item* pItem, bool /*swap*/, bool /*not_loading*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_SELF_CRAFTED, player))
        {
            return true;
        }
        // Sylvania fix (tickets GHZ 2026-07-09) : un seul metier ne peut fabriquer
        // a la fois armes, armures et sacs -> armes, conteneurs (sacs) et carquois
        // sont exemptes de l'auto-artisanat (l'armure reste auto-craftee).
        if (pItem->GetTemplate()->Class == ITEM_CLASS_WEAPON
            || pItem->GetTemplate()->Class == ITEM_CLASS_CONTAINER
            || pItem->GetTemplate()->Class == ITEM_CLASS_QUIVER)
        {
            return true;
        }
        if (!pItem->GetTemplate()->HasSignature())
        {
            return false;
        }
        return pItem->GetGuidValue(ITEM_FIELD_CREATOR) == player->GetGUID();
    }

    void OnPlayerGiveXP(Player* player, uint32& amount, Unit* victim, uint8 xpSource) override
    {
        ChallengeMode::OnPlayerGiveXP(player, amount, victim, xpSource);
    }

    void OnPlayerLevelChanged(Player* player, uint8 oldlevel) override
    {
        ChallengeMode::OnPlayerLevelChanged(player, oldlevel);
    }
};

class ChallengeMode_ItemQualityLevel : public ChallengeMode
{
public:
    ChallengeMode_ItemQualityLevel() : ChallengeMode("ChallengeMode_ItemQualityLevel", SETTING_ITEM_QUALITY_LEVEL) {}

    bool OnPlayerCanEquipItem(Player* player, uint8 /*slot*/, uint16& /*dest*/, Item* pItem, bool /*swap*/, bool /*not_loading*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_ITEM_QUALITY_LEVEL, player))
        {
            return true;
        }
        return pItem->GetTemplate()->Quality <= ITEM_QUALITY_NORMAL;
    }

    void OnPlayerGiveXP(Player* player, uint32& amount, Unit* victim, uint8 xpSource) override
    {
        ChallengeMode::OnPlayerGiveXP(player, amount, victim, xpSource);
    }

    void OnPlayerLevelChanged(Player* player, uint8 oldlevel) override
    {
        ChallengeMode::OnPlayerLevelChanged(player, oldlevel);
    }
};

class ChallengeMode_SlowXpGain : public ChallengeMode
{
public:
    ChallengeMode_SlowXpGain() : ChallengeMode("ChallengeMode_SlowXpGain", SETTING_SLOW_XP_GAIN) {}

    void OnPlayerGiveXP(Player* player, uint32& amount, Unit* victim, uint8 xpSource) override
    {
        ChallengeMode::OnPlayerGiveXP(player, amount, victim, xpSource);
    }

    void OnPlayerLevelChanged(Player* player, uint8 oldlevel) override
    {
        ChallengeMode::OnPlayerLevelChanged(player, oldlevel);
    }
};

class ChallengeMode_VerySlowXpGain : public ChallengeMode
{
public:
    ChallengeMode_VerySlowXpGain() : ChallengeMode("ChallengeMode_VerySlowXpGain", SETTING_VERY_SLOW_XP_GAIN) {}

    void OnPlayerGiveXP(Player* player, uint32& amount, Unit* victim, uint8 xpSource) override
    {
        ChallengeMode::OnPlayerGiveXP(player, amount, victim, xpSource);
    }

    void OnPlayerLevelChanged(Player* player, uint8 oldlevel) override
    {
        ChallengeMode::OnPlayerLevelChanged(player, oldlevel);
    }
};

class ChallengeMode_QuestXpOnly : public ChallengeMode
{
public:
    ChallengeMode_QuestXpOnly() : ChallengeMode("ChallengeMode_QuestXpOnly", SETTING_QUEST_XP_ONLY) {}

    void OnPlayerGiveXP(Player* player, uint32& amount, Unit* victim, uint8 xpSource) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_QUEST_XP_ONLY, player))
        {
            return;
        }
        if (victim)
        {
            // Still award XP to pets - they won't be able to pass the player's level
            Pet* pet = player->GetPet();
            if (pet && xpSource == XPSOURCE_KILL)
                pet->GivePetXP(player->GetGroup() ? amount / 2 : amount);
            amount = 0;
        }
        else
        {
            ChallengeMode::OnPlayerGiveXP(player, amount, victim, xpSource);
        }
    }

    void OnPlayerLevelChanged(Player* player, uint8 oldlevel) override
    {
        ChallengeMode::OnPlayerLevelChanged(player, oldlevel);
    }
};

class ChallengeMode_IronMan : public ChallengeMode
{
public:
    ChallengeMode_IronMan() : ChallengeMode("ChallengeMode_IronMan", SETTING_IRON_MAN) {}

    void OnPlayerResurrect(Player* player, float /*restore_percent*/, bool /*applySickness*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return;
        }
        // A better implementation is to not allow the resurrect but this will need a new hook added first
        player->KillPlayer();
    }

    void OnPlayerGiveXP(Player* player, uint32& amount, Unit* victim, uint8 xpSource) override
    {
        ChallengeMode::OnPlayerGiveXP(player, amount, victim, xpSource);
    }

    void OnPlayerLevelChanged(Player* player, uint8 oldlevel) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return;
        }
        player->SetFreeTalentPoints(0); // Remove all talent points
        ChallengeMode::OnPlayerLevelChanged(player, oldlevel);
    }

    void OnPlayerTalentsReset(Player* player, bool /*noCost*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return;
        }
        player->SetFreeTalentPoints(0); // Remove all talent points
    }

    bool OnPlayerCanEquipItem(Player* player, uint8 /*slot*/, uint16& /*dest*/, Item* pItem, bool /*swap*/, bool /*not_loading*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return true;
        }
        return pItem->GetTemplate()->Quality <= ITEM_QUALITY_NORMAL;
    }

    bool OnPlayerCanApplyEnchantment(Player* player, Item* /*item*/, EnchantmentSlot /*slot*/, bool /*apply*/, bool /*apply_dur*/, bool /*ignore_condition*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return true;
        }
        // Are there any exceptions in WotLK? If so need to be added here
        return false;
    }

    void OnPlayerLearnSpell(Player* player, uint32 spellID) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return;
        }
        // These professions are class skills so they are always acceptable
        switch (spellID)
        {
            case RUNEFORGING:
            case POISONS:
            case BEAST_TRAINING:
                return;
            default:
                break;
        }
        // Do not allow learning any trade skills
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellID);
        if (!spellInfo)
            return;
        bool shouldForget = false;
        for (uint8 i = 0; i < 3; i++)
        {
            if (spellInfo->Effects[i].Effect == SPELL_EFFECT_TRADE_SKILL)
            {
                shouldForget = true;
            }
        }
        if (shouldForget)
        {
            player->removeSpell(spellID, SPEC_MASK_ALL, false);
        }
    }

    bool OnPlayerCanUseItem(Player* player, ItemTemplate const* proto, InventoryResult& /*result*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return true;
        }
        // Do not allow using elixir, potion, or flask
        if (proto->Class == ITEM_CLASS_CONSUMABLE &&
                (proto->SubClass == ITEM_SUBCLASS_POTION ||
                proto->SubClass == ITEM_SUBCLASS_ELIXIR ||
                proto->SubClass == ITEM_SUBCLASS_FLASK))
        {
            return false;
        }
        // Do not allow food that gives food buffs
        if (proto->Class == ITEM_CLASS_CONSUMABLE && proto->SubClass == ITEM_SUBCLASS_FOOD)
        {
            for (const auto & Spell : proto->Spells)
            {
                SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(Spell.SpellId);
                if (!spellInfo)
                    continue;

                for (uint8 i = 0; i < 3; i++)
                {
                    if (spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_PERIODIC_TRIGGER_SPELL)
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    bool OnPlayerCanGroupInvite(Player* player, std::string& /*membername*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return true;
        }
        return false;
    }

    bool OnPlayerCanGroupAccept(Player* player, Group* /*group*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return true;
        }
        return false;
    }

};

class gobject_challenge_modes : public GameObjectScript
{
private:
    static bool playerSettingEnabled(Player* player, uint8 settingIndex)
    {
        return player->GetPlayerSetting("mod-challenge-modes", settingIndex).value;
    }

public:
    gobject_challenge_modes() : GameObjectScript("gobject_challenge_modes") { }

    struct gobject_challenge_modesAI: GameObjectAI
    {
        explicit gobject_challenge_modesAI(GameObject* object) : GameObjectAI(object) { };

        bool CanBeSeen(Player const* player) override
        {
            if ((player->GetLevel() > 1 && player->getClass() != CLASS_DEATH_KNIGHT) || (player->GetLevel() > 55))
            {
                return false;
            }
            return sChallengeModes->enabled();
        }
    };

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        if (player->GetLevel() > 1)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Vous devez être niveau 1 pour activer un mode de défi.");
            CloseGossipMenuFor(player);
            return false;
        }

        if (sChallengeModes->challengeEnabled(SETTING_HARDCORE) && !playerSettingEnabled(player, SETTING_HARDCORE) && !playerSettingEnabled(player, SETTING_SEMI_HARDCORE))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Activer le Mode Hardcore (1 Vie)", 0, SETTING_HARDCORE, "Êtes-vous sûr de vouloir activer ce mode de défi ?\n\nCe choix est DÉFINITIF et ne pourra pas être annulé.", 0, false);
        }
        if (sChallengeModes->challengeEnabled(SETTING_SEMI_HARDCORE) && !playerSettingEnabled(player, SETTING_HARDCORE) && !playerSettingEnabled(player, SETTING_SEMI_HARDCORE))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Activer le Mode Semi-Hardcore", 0, SETTING_SEMI_HARDCORE, "Êtes-vous sûr de vouloir activer ce mode de défi ?\n\nCe choix est DÉFINITIF et ne pourra pas être annulé.", 0, false);
        }
        if (sChallengeModes->challengeEnabled(SETTING_SELF_CRAFTED) && !playerSettingEnabled(player, SETTING_SELF_CRAFTED) && !playerSettingEnabled(player, SETTING_IRON_MAN))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Activer le Mode Auto-artisanat (Craft uniquement)", 0, SETTING_SELF_CRAFTED, "Êtes-vous sûr de vouloir activer ce mode de défi ?\n\nCe choix est DÉFINITIF et ne pourra pas être annulé.", 0, false);
        }
        if (sChallengeModes->challengeEnabled(SETTING_ITEM_QUALITY_LEVEL) && !playerSettingEnabled(player, SETTING_ITEM_QUALITY_LEVEL))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Activer le Mode Qualité d'Objet Limitée", 0, SETTING_ITEM_QUALITY_LEVEL, "Êtes-vous sûr de vouloir activer ce mode de défi ?\n\nCe choix est DÉFINITIF et ne pourra pas être annulé.", 0, false);
        }
        if (sChallengeModes->challengeEnabled(SETTING_SLOW_XP_GAIN) && !playerSettingEnabled(player, SETTING_SLOW_XP_GAIN) && !playerSettingEnabled(player, SETTING_VERY_SLOW_XP_GAIN))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Activer le Mode XP Lente (x0.5)", 0, SETTING_SLOW_XP_GAIN, "Êtes-vous sûr de vouloir activer ce mode de défi ?\n\nCe choix est DÉFINITIF et ne pourra pas être annulé.", 0, false);
        }
        if (sChallengeModes->challengeEnabled(SETTING_VERY_SLOW_XP_GAIN) && !playerSettingEnabled(player, SETTING_SLOW_XP_GAIN) && !playerSettingEnabled(player, SETTING_VERY_SLOW_XP_GAIN))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Activer le Mode XP Très Lente (x0.25)", 0, SETTING_VERY_SLOW_XP_GAIN, "Êtes-vous sûr de vouloir activer ce mode de défi ?\n\nCe choix est DÉFINITIF et ne pourra pas être annulé.", 0, false);
        }
        if (sChallengeModes->challengeEnabled(SETTING_QUEST_XP_ONLY) && !playerSettingEnabled(player, SETTING_QUEST_XP_ONLY))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Activer le Mode XP de Quêtes Uniquement", 0, SETTING_QUEST_XP_ONLY, "Êtes-vous sûr de vouloir activer ce mode de défi ?\n\nCe choix est DÉFINITIF et ne pourra pas être annulé.", 0, false);
        }
        if (sChallengeModes->challengeEnabled(SETTING_IRON_MAN) && !playerSettingEnabled(player, SETTING_IRON_MAN) && !playerSettingEnabled(player, SETTING_SELF_CRAFTED))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Activer le Mode Iron Man (Règlement Strict)", 0, SETTING_IRON_MAN, "Êtes-vous sûr de vouloir activer ce mode de défi ?\n\nCe choix est DÉFINITIF et ne pourra pas être annulé.", 0, false);
        }
        // Mode Martyr (porté du Lua vers C++ le 2026-05-30) — stocké dans character_martyr_mode
        // (active : 0=off, 1=on, 2=récompense déjà réclamée).
        // On n'offre l'activation que si NON déjà active=1 ET pas déjà claim (active=2).
        {
            QueryResult martyrCheck = CharacterDatabase.Query(
                "SELECT active FROM character_martyr_mode WHERE guid = {}",
                player->GetGUID().GetCounter());
            bool offerMartyr = true;
            if (martyrCheck)
            {
                uint8 state = martyrCheck->Fetch()[0].Get<uint8>();
                if (state >= 1) // déjà actif OU déjà réclamé
                    offerMartyr = false;
            }
            if (offerMartyr)
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|cFF800080Activer le Mode Martyr (Perte XP sur mort)|r", 0, 100, "Êtes-vous sûr de vouloir activer ce mode de défi ?\n\nCe choix est DÉFINITIF et ne pourra pas être annulé.", 0, false);
            }
        }
        SylvaniaSurvival::AddAltarOption(player);
        SendGossipMenuFor(player, 12669, go->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action) override
    {
        if (action == SylvaniaSurvival::ActivationAction)
        {
            if (SylvaniaSurvival::ActivateAtAltar(player, go))
                AnnounceChallengeActivation(player, action);
            CloseGossipMenuFor(player);
            return true;
        }
        // Action 100 = activation Mode Martyr (porté du Lua, hors framework PlayerSetting).
        if (action == 100)
        {
            // Si déjà claim (active=2), on refuse de réactiver pour éviter un re-grant.
            QueryResult existing = CharacterDatabase.Query(
                "SELECT active FROM character_martyr_mode WHERE guid = {}",
                player->GetGUID().GetCounter());
            if (existing && existing->Fetch()[0].Get<uint8>() == 2)
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Vous avez déjà reçu la récompense Martyr sur ce personnage.");
                CloseGossipMenuFor(player);
                return true;
            }
            CharacterDatabase.Execute(
                "REPLACE INTO character_martyr_mode (guid, active) VALUES ({}, 1)",
                player->GetGUID().GetCounter());
            player->GetSession()->SendAreaTriggerMessage("Mode Martyr activé !");
            AnnounceChallengeActivation(player, 100);
            CloseGossipMenuFor(player);
            return true;
        }
        player->UpdatePlayerSetting("mod-challenge-modes", action, 1);
        ChatHandler(player->GetSession()).PSendSysMessage("Mode Défi activé avec succès !");
        AnnounceChallengeActivation(player, action);
        CloseGossipMenuFor(player);
        return true;
    }

    GameObjectAI* GetAI(GameObject* object) const override
    {
        return new gobject_challenge_modesAI(object);
    }
};

// =====================================================================================
// Mode Martyr (porté du Lua ChallengeMultiLang.lua vers C++ le 2026-05-30)
// -------------------------------------------------------------------------------------
// Pénalité  : -10% de l'XP du niveau suivant à chaque mort PvP ou PvE.
//             Si l'XP courant ne couvre pas la pénalité → perte d'un niveau
//             (XP reset à 80% du nouveau "next level XP"). Si déjà lvl 1 → XP = 0.
// Récompense : 500 Sylvania Coins à l'atteinte du niveau 80 (une fois ; on passe
//              active=2 pour empêcher un re-claim).
// Stockage  : table character_martyr_mode (guid PK, active TINYINT — 0/1/2).
//              0 = off (ou ligne absente), 1 = actif, 2 = récompense déjà réclamée.
// Anti-Sésame : le reward 500 SC ne fire QUE si le saut de niveau est naturel (+1).
// =====================================================================================
class ChallengeMode_Martyr : public PlayerScript
{
public:
    ChallengeMode_Martyr() : PlayerScript("ChallengeMode_Martyr") { }

    void OnPlayerLogin(Player* player) override
    {
        if (!isMartyrActive(player))
            return;
        ChatHandler(player->GetSession()).SendSysMessage("|cFF800080[Martyr]|r Mode Martyr : |cFF00FF00ACTIF|r.");
    }

    void OnPlayerPVPKill(Player* /*killer*/, Player* killed) override
    {
        ApplyMartyrPenalty(killed);
    }

    void OnPlayerKilledByCreature(Creature* /*killer*/, Player* killed) override
    {
        ApplyMartyrPenalty(killed);
    }

    void OnPlayerLevelChanged(Player* player, uint8 oldlevel) override
    {
        if (!isMartyrActive(player))
            return;
        // 🛡️ Anti-exploit : ne grant la récompense Martyr que si le saut est naturel (+1).
        if (oldlevel + 1 != player->GetLevel())
            return;
        if (player->GetLevel() != 80)
            return;

        LoginDatabase.Execute("UPDATE account SET vp = vp + 500 WHERE id = {}",
                              player->GetSession()->GetAccountId());
        CharacterDatabase.Execute("UPDATE character_martyr_mode SET active = 2 WHERE guid = {}",
                                  player->GetGUID().GetCounter());

        ChatHandler(player->GetSession()).SendSysMessage("|cFF800080[Martyr]|r Félicitations ! Vous avez atteint le niveau 80 en mode Martyr. +500 Sylvania Coins attribués !");
        std::string broadcastMsg = Acore::StringFormat(
            "|cFFFFD700[Sylvania]|r Le personnage {} a triomphé du mode Martyr en atteignant le niveau 80 !",
            player->GetName());
        sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, broadcastMsg);
    }

private:
    static bool isMartyrActive(Player* player)
    {
        if (!player)
            return false;
        QueryResult result = CharacterDatabase.Query(
            "SELECT active FROM character_martyr_mode WHERE guid = {} AND active = 1",
            player->GetGUID().GetCounter());
        return (bool)result;
    }

    static void ApplyMartyrPenalty(Player* player)
    {
        if (!player || !player->IsInWorld())
            return;
        uint8 currentLvl = player->GetLevel();
        if (currentLvl < 1 || currentLvl >= 80)
            return;
        if (!isMartyrActive(player))
            return;

        uint32 nextLvlXP = player->GetUInt32Value(PLAYER_NEXT_LEVEL_XP);
        if (nextLvlXP == 0)
            nextLvlXP = 1;
        uint32 currentXP = player->GetUInt32Value(PLAYER_XP);
        uint32 penalty = nextLvlXP * 10 / 100; // 10%

        if (currentXP >= penalty)
        {
            player->SetUInt32Value(PLAYER_XP, currentXP - penalty);
            ChatHandler(player->GetSession()).PSendSysMessage(
                "|cFF800080[Martyr]|r Vous avez perdu {} points d'expérience.", penalty);
        }
        else
        {
            if (currentLvl > 1)
            {
                player->SetLevel(currentLvl - 1);
                uint32 newNextXP = player->GetUInt32Value(PLAYER_NEXT_LEVEL_XP);
                player->SetUInt32Value(PLAYER_XP, newNextXP * 80 / 100);
                ChatHandler(player->GetSession()).PSendSysMessage(
                    "|cFF800080[Martyr]|r Pénalité de mort : Niveau réduit au niveau {} !",
                    currentLvl - 1);
            }
            else
            {
                player->SetUInt32Value(PLAYER_XP, 0);
            }
        }
    }
};

// Gestion du canal des défis : rappel « /join Defis » au login tant qu'un défi est
// actif (lvl<80) et que le joueur n'y est pas ; canal réservé (les non-défi ne peuvent
// pas y parler et en sont retirés) ; retrait automatique au niveau 80 ou quand plus
// aucun défi n'est actif. Enregistré en DERNIER dans AddSC pour passer APRÈS les
// ChallengeMode (qui posent setting=0 au DisableLevel) sur OnPlayerLevelChanged.
class ChallengeModes_ChannelMembership : public PlayerScript
{
public:
    ChallengeModes_ChannelMembership() : PlayerScript("ChallengeModes_ChannelMembership") { }

    void OnPlayerLogin(Player* player) override
    {
        if (!sChallengeModes->enabled() || !sChallengeModes->challengeChannelEnable)
            return;
        // Différé de 8 s : on laisse le client finir son chargement et re-rejoindre
        // LUI-MÊME ses canaux sauvegardés avant de vérifier la présence.
        // (L'évènement appartient au joueur : annulé automatiquement à la déco.)
        player->m_Events.AddEventAtOffset([player]()
        {
            if (!player->IsInWorld())
                return;
            Channel* channel = GetChallengeChannel(player);
            if (!channel)
                return;
            if (player->GetLevel() < 80 && HasAnyActiveChallenge(player))
            {
                if (!channel->IsMember(player->GetGUID()))
                    RemindJoinChallengeChannel(player);
            }
            else if (channel->IsMember(player->GetGUID()))
                channel->LeaveChannel(player, true);
        }, Milliseconds(8000));
    }

    void OnPlayerLevelChanged(Player* player, uint8 /*oldlevel*/) override
    {
        if (!sChallengeModes->enabled() || !sChallengeModes->challengeChannelEnable)
            return;
        if (player->GetLevel() < 80 && HasAnyActiveChallenge(player))
            return;
        SweepFromChallengeChannel(player);
    }

    bool OnPlayerCanUseChat(Player* player, uint32 /*type*/, uint32 /*language*/, std::string& /*msg*/, Channel* channel) override
    {
        if (!channel || !sChallengeModes->enabled() || !sChallengeModes->challengeChannelEnable)
            return true;
        if (channel != GetChallengeChannel(player))
            return true;
        if (player->GetLevel() < 80 && HasAnyActiveChallenge(player))
            return true;
        ChatHandler(player->GetSession()).PSendSysMessage(
            "Le canal {} est réservé aux joueurs en mode défi.", sChallengeModes->challengeChannelName);
        SweepFromChallengeChannel(player);
        return false;
    }
};

// Add all scripts in one
void AddSC_mod_challenge_modes()
{
    new ChallengeModes_WorldScript();
    new gobject_challenge_modes();
    new ChallengeMode_Hardcore();
    new ChallengeMode_SemiHardcore();
    new ChallengeMode_SelfCrafted();
    new ChallengeMode_ItemQualityLevel();
    new ChallengeMode_SlowXpGain();
    new ChallengeMode_VerySlowXpGain();
    new ChallengeMode_QuestXpOnly();
    new ChallengeMode_IronMan();
    new ChallengeMode_Martyr();
    new ChallengeModes_ChannelMembership();
}
