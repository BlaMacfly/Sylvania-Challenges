#pragma once

#include <algorithm>
#include <cstdint>

namespace SylvaniaSurvival
{
    constexpr std::uint32_t Full = 1000000;
    constexpr std::uint32_t ActivationAction = 101;

    struct State
    {
        bool active = false;
        std::uint32_t food = Full;
        std::uint32_t water = Full;
        std::uint32_t cold = 0;
        std::uint32_t healthCeiling = Full;
    };

    struct Rules
    {
        std::uint32_t foodSeconds = 5400;
        std::uint32_t waterSeconds = 3600;
        std::uint32_t mealSeconds = 30;
        std::uint32_t coldSeconds = 600;
        std::uint32_t warmSeconds = 120;
    };

    struct Environment
    {
        bool paused = false;
        bool eating = false;
        bool drinking = false;
        bool cold = false;
        bool hot = false;
        bool wet = false;
        bool sheltered = false;
        bool atFire = false;
    };

    inline std::uint32_t Change(std::uint32_t value, std::uint64_t amount, bool increase)
    {
        value = std::min(value, Full);
        if (increase)
            return static_cast<std::uint32_t>(std::min<std::uint64_t>(Full, value + amount));
        return amount >= value ? 0 : value - static_cast<std::uint32_t>(amount);
    }

    inline std::uint64_t Rate(std::uint32_t seconds, std::uint32_t duration)
    {
        return std::uint64_t(Full) * seconds / std::max<std::uint32_t>(1, duration);
    }

    inline bool CanRun(bool enabled, std::uint32_t realm, std::uint32_t allowedRealm)
    {
        return enabled && allowedRealm != 0 && realm == allowedRealm;
    }

    inline bool CanActivate(bool enabled, bool alreadyChosen, std::uint32_t level, bool deathKnight)
    {
        return enabled && !alreadyChosen && level == 1 && !deathKnight;
    }

    inline void Advance(State& state, Rules const& rules, Environment const& env, std::uint32_t seconds)
    {
        if (!state.active || env.paused || seconds == 0)
            return;

        state.food = Change(state.food, Rate(seconds, env.eating ? rules.mealSeconds : rules.foodSeconds), env.eating);
        std::uint64_t waterRate = Rate(seconds, env.drinking ? rules.mealSeconds : rules.waterSeconds);
        if (env.hot && !env.sheltered && !env.drinking)
            waterRate *= 2;
        state.water = Change(state.water, waterRate, env.drinking);

        bool chilling = env.cold && !env.sheltered && !env.atFire;
        std::uint64_t coldRate = Rate(seconds, chilling ? rules.coldSeconds : rules.warmSeconds);
        if (chilling && env.wet)
            coldRate *= 2;
        state.cold = Change(state.cold, coldRate, chilling);
        if (state.water == 0)
            state.healthCeiling = std::max(Full / 100, Change(state.healthCeiling, Rate(seconds, 100), false));
        else
            state.healthCeiling = Full;
    }

    inline std::uint32_t HealthLimit(std::uint32_t maxHealth, std::uint32_t ceiling)
    {
        ceiling = std::clamp(ceiling, Full / 100, Full);
        return static_cast<std::uint32_t>(std::max<std::uint64_t>(1,
            (std::uint64_t(maxHealth) * ceiling + Full - 1) / Full));
    }

    inline std::uint32_t AllowedHeal(std::uint32_t health, std::uint32_t maxHealth, std::uint32_t ceiling, std::uint32_t gain)
    {
        std::uint32_t limit = HealthLimit(maxHealth, ceiling);
        return health >= limit ? 0 : std::min(gain, limit - health);
    }

    inline unsigned NeedBand(std::uint32_t value)
    {
        return value == 0 ? 3 : value <= Full / 10 ? 2 : value <= Full / 4 ? 1 : 0;
    }

    inline unsigned ColdBand(std::uint32_t value)
    {
        return value >= Full ? 4 : value >= Full * 3 / 4 ? 3 : value >= Full / 2 ? 2 : value >= Full / 4 ? 1 : 0;
    }

    // This floor applies only to our own deprivation/exposure drain. Combat damage is untouched.
    inline std::uint32_t HealthAfterDrain(std::uint32_t health, std::uint32_t maxHealth, std::uint32_t percent)
    {
        if (!health || !percent || !maxHealth)
            return health;
        std::uint64_t const floor = std::max<std::uint64_t>(1, (std::uint64_t(maxHealth) + 99) / 100);
        if (health <= floor)
            return health;
        std::uint64_t const drain = std::max<std::uint64_t>(1, std::uint64_t(maxHealth) * percent / 100);
        return static_cast<std::uint32_t>(std::max<std::uint64_t>(floor, health > drain ? health - drain : 0));
    }
}
