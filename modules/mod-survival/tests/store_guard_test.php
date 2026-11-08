<?php
// Exercise the real function without running the store's API bootstrap.
$text = file_get_contents($argv[1]);
if (!preg_match('/function isCharacterInChallenge\([^\n]*\)\s*\{.*?\n\}/s', $text, $match)) {
    throw new RuntimeException('Challenge guard not found');
}
eval($match[0]);

class SurvivalFakeStatement {
    private $row;
    public function __construct($row) { $this->row = $row; }
    public function execute($parameters) { return true; }
    public function fetch($mode) { return $this->row; }
}
class SurvivalFakeDatabase {
    private $rows;
    public function __construct($rows) { $this->rows = $rows; }
    public function prepare($sql) {
        if (stripos($sql, 'SELECT ') !== 0) {
            throw new RuntimeException('Guard must be read-only');
        }
        foreach (['mod-challenge-modes', 'mod-survival', 'character_hardcore', 'character_martyr_mode'] as $key) {
            if (strpos($sql, $key) !== false) {
                return new SurvivalFakeStatement($this->rows[$key] ?? false);
            }
        }
        throw new RuntimeException('Unexpected query');
    }
}
$cases = [
    [[], false],
    [['mod-survival' => ['data' => '1 1000000 1000000 0 1 1000000 ']], true],
    [['mod-survival' => ['data' => '2 1000000 1000000 0 1 1000000 ']], false],
    [['mod-survival' => ['data' => '0 1000000 1000000 0 1 1000000 ']], false],
    [['mod-challenge-modes' => ['data' => '0 0 1 0 0 0 0 0 0 ']], true],
    [['character_hardcore' => ['is_hardcore' => 1]], true],
    [['character_martyr_mode' => ['active' => 1]], true],
    [['character_martyr_mode' => ['active' => 2]], false],
];
foreach ($cases as $i => $case) {
    if (isCharacterInChallenge(new SurvivalFakeDatabase($case[0]), 42) !== $case[1]) {
        throw new RuntimeException('Failed store guard case ' . $i);
    }
}
echo count($cases) . " store guard checks passed\n";
