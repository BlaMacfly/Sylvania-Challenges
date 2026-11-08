#include "SurvivalModel.h"

#include <cstdlib>
#include <iostream>
#include <limits>

using namespace SylvaniaSurvival;

static unsigned checks = 0;
static void Expect(bool value, char const* message)
{
    ++checks;
    if (!value)
    {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

int main()
{
    Rules rules;
    Environment outside;
    outside.cold = true;
    outside.hot = true;
    State normal;
    for (unsigned i = 0; i < 10000; ++i)
        Advance(normal, rules, outside, 5);
    Expect(normal.food == Full && normal.water == Full && normal.cold == 0 && normal.healthCeiling == Full,
        "a non-participant stays untouched even in harsh weather");
    Expect(!CanRun(false, 1, 1) && !CanRun(true, 2, 1) && !CanRun(true, 1, 0) && CanRun(true, 1, 1),
        "module disable and realm boundary fail closed");
    Expect(CanActivate(true, false, 1, false) && !CanActivate(true, false, 2, false)
        && !CanActivate(true, true, 1, false) && !CanActivate(true, false, 1, true)
        && !CanActivate(false, false, 1, false), "activation cannot be forged, repeated or used by a death knight");

    State survivor; survivor.active = true;
    Environment paused = outside; paused.paused = true;
    Advance(survivor, rules, paused, 86400);
    Expect(survivor.food == Full && survivor.water == Full && survivor.cold == 0,
        "ghosts, travel, battlegrounds and AFK pause all meters");

    Environment temperate;
    Advance(survivor, rules, temperate, 600);
    Expect(survivor.food < Full && survivor.water < Full && survivor.cold == 0, "ordinary travel consumes reserves");
    State partial = survivor;
    Environment meal; meal.eating = true;
    Advance(survivor, rules, meal, 5);
    Expect(survivor.food > partial.food && survivor.water < partial.water, "food refills progressively and does not count as water");
    meal.drinking = true;
    Advance(survivor, rules, meal, 30);
    Expect(survivor.food == Full && survivor.water == Full, "a complete food-and-drink restores both reserves");

    State thirsty; thirsty.active = true;
    State hot = thirsty;
    Environment desert; desert.hot = true;
    Advance(thirsty, rules, temperate, 600);
    Advance(hot, rules, desert, 600);
    Expect(Full - hot.water == 2 * (Full - thirsty.water), "heat doubles water consumption");

    State cold; cold.active = true;
    Environment snow; snow.cold = true;
    Advance(cold, rules, snow, rules.coldSeconds);
    Expect(cold.cold == Full && ColdBand(cold.cold) == 4, "long exposure reaches severe cold");
    Environment fire = snow; fire.atFire = true;
    Advance(cold, rules, fire, rules.warmSeconds);
    Expect(cold.cold == 0, "a cooking fire warms a participant despite the snowy zone");
    Environment indoor = snow; indoor.sheltered = true;
    cold.cold = Full;
    Advance(cold, rules, indoor, rules.warmSeconds);
    Expect(cold.cold == 0, "shelter provides an alternative to a campfire");
    State wet; wet.active = true;
    Environment icyWater = snow; icyWater.wet = true;
    Advance(wet, rules, icyWater, rules.coldSeconds / 2);
    Expect(wet.cold == Full, "icy water accelerates exposure");

    State starving; starving.active = true; starving.food = 0;
    Advance(starving, rules, temperate, 100);
    Expect(starving.healthCeiling == Full, "food deprivation never imposes the dehydration health cap");
    starving.water = 0;
    Advance(starving, rules, temperate, 100);
    Expect(starving.healthCeiling == Full / 100, "dehydration progressively imposes a one-percent ceiling");
    Expect(AllowedHeal(100, 10000, starving.healthCeiling, 9999) == 0, "healing cannot bypass dehydration");
    Expect(AllowedHeal(50, 10000, starving.healthCeiling, 9999) == 50, "healing below the ceiling remains possible");
    Environment onlyFood; onlyFood.eating = true;
    Advance(starving, rules, onlyFood, 5);
    Expect(starving.food > 0 && starving.water == 0 && starving.healthCeiling == Full / 100,
        "food cannot lift a dehydration penalty");
    Environment onlyWater; onlyWater.drinking = true;
    starving.food = 0;
    Advance(starving, rules, onlyWater, 5);
    Expect(starving.food == 0 && starving.water > 0 && starving.healthCeiling == Full,
        "water lifts the health cap even when hunger persists");
    Advance(starving, rules, meal, 5);
    Expect(starving.healthCeiling == Full, "eating and drinking lift the ceiling without awarding free health");

    for (auto maxHp : {1u, 70u, 99u, 100u, 101u, 10000u, std::numeric_limits<unsigned>::max()})
    {
        auto hp = maxHp;
        for (unsigned i = 0; i < 200; ++i)
            hp = HealthAfterDrain(hp, maxHp, 5);
        Expect(hp == HealthLimit(maxHp, Full / 100) && hp > 0, "exposure cannot kill, including tiny or extreme health pools");
    }
    Expect(HealthAfterDrain(0, 10000, 5) == 0 && HealthAfterDrain(25, 10000, 5) == 25,
        "the drain neither resurrects nor heals someone already below the floor");
    State completed = cold; completed.active = false;
    State snapshot = completed;
    Advance(completed, rules, outside, 86400);
    Expect(completed.food == snapshot.food && completed.water == snapshot.water && completed.cold == snapshot.cold,
        "completed challenges cannot continue draining");
    std::cout << checks << " Survival checks passed\n";
}
