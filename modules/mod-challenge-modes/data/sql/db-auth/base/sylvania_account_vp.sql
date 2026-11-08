-- Run in the auth database. Sylvania Coins rewards use account.vp.
-- Preserve an existing column and its balances; add only when absent.
SET @sylvania_vp_ddl = (
  SELECT IF(COUNT(*) = 0,
    'ALTER TABLE `account` ADD COLUMN `vp` int unsigned NOT NULL DEFAULT 0',
    'SELECT 1')
  FROM information_schema.COLUMNS
  WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'account' AND COLUMN_NAME = 'vp'
);
PREPARE sylvania_vp_statement FROM @sylvania_vp_ddl;
EXECUTE sylvania_vp_statement;
DEALLOCATE PREPARE sylvania_vp_statement;
