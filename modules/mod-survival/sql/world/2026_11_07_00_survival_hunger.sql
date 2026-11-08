-- Dedicated server-only passive aura. Does not replace any stock spell.
-- Refuse conflicting ownership in the deployment preflight; never use REPLACE here.
-- 536870976 = PASSIVE (0x40) | NO_IMMUNITIES (0x20000000).
-- Effect 137 multiplies all five primary stats by 0.5 and survives native buff recalculation.
-- Passive auras are neither sent to the 3.3.5 client nor saved in character_aura.
INSERT INTO `spell_dbc`
(`ID`, `Attributes`, `AttributesEx2`, `EquippedItemClass`, `RangeIndex`, `SchoolMask`,
 `Effect_1`, `EffectDieSides_1`, `EffectBasePoints_1`, `ImplicitTargetA_1`,
 `EffectAura_1`, `EffectMiscValue_1`, `Name_Lang_enUS`, `Name_Lang_frFR`)
SELECT 900101, 536870976, 67108864, -1, 1, 1,
       6, 1, -51, 1, 137, -1, 'Sylvania Survival - Starving', 'Sylvania Survival - Affame'
WHERE NOT EXISTS (SELECT 1 FROM `spell_dbc` WHERE `ID` = 900101);
