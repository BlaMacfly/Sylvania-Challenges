-- Sylvania dependencies exported from the installed schema; no player data.
-- Numeric columns match R1. A portable utf8mb4 collation is used for new tables.
CREATE TABLE IF NOT EXISTS `character_challenges` (
  `guid` int unsigned NOT NULL,
  `mode` int unsigned NOT NULL,
  `is_dead` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`, `mode`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `character_martyr_mode` (
  `guid` int unsigned NOT NULL,
  `active` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
