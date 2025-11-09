--TEST--
CrcFast\Params getter methods test
--EXTENSIONS--
crc_fast
--FILE--
<?php

use CrcFast\Params;

echo "Testing getter methods with 32-bit CRC parameters:\n";

$params32 = new Params(
    width: 32,
    poly: 0x04C11DB7,
    init: 0xFFFFFFFF,
    refin: true,
    refout: true,
    xorout: 0xFFFFFFFF,
    check: 0xCBF43926
);

// Test all getter methods
echo "getWidth(): " . $params32->getWidth() . "\n";
echo "getPoly(): 0x" . strtoupper(dechex($params32->getPoly())) . "\n";
echo "getInit(): 0x" . strtoupper(dechex($params32->getInit())) . "\n";
echo "getRefin(): " . ($params32->getRefin() ? "true" : "false") . "\n";
echo "getRefout(): " . ($params32->getRefout() ? "true" : "false") . "\n";
echo "getXorout(): 0x" . strtoupper(dechex($params32->getXorout())) . "\n";
echo "getCheck(): 0x" . strtoupper(dechex($params32->getCheck())) . "\n";

$keys32 = $params32->getKeys();
echo "getKeys() returns array: " . (is_array($keys32) ? "true" : "false") . "\n";
echo "getKeys() array length: " . count($keys32) . "\n";
echo "getKeys() first element is int: " . (is_int($keys32[0]) ? "true" : "false") . "\n";

echo "\nTesting getter methods with alternative 32-bit CRC parameters:\n";

$params32_alt = new Params(
    width: 32,
    poly: 0x1021,
    init: 0,
    refin: false,
    refout: false,
    xorout: 0,
    check: 0x70a79f31
);

echo "getWidth(): " . $params32_alt->getWidth() . "\n";
echo "getPoly(): 0x" . strtoupper(dechex($params32_alt->getPoly())) . "\n";
echo "getInit(): 0x" . strtoupper(dechex($params32_alt->getInit())) . "\n";
echo "getRefin(): " . ($params32_alt->getRefin() ? "true" : "false") . "\n";
echo "getRefout(): " . ($params32_alt->getRefout() ? "true" : "false") . "\n";
echo "getXorout(): 0x" . strtoupper(dechex($params32_alt->getXorout())) . "\n";
echo "getCheck(): 0x" . strtoupper(dechex($params32_alt->getCheck())) . "\n";

$keys32_alt = $params32_alt->getKeys();
echo "getKeys() returns array: " . (is_array($keys32_alt) ? "true" : "false") . "\n";
echo "getKeys() array length: " . count($keys32_alt) . "\n";
echo "getKeys() first element is int: " . (is_int($keys32_alt[0]) ? "true" : "false") . "\n";

echo "\nTesting getter methods consistency:\n";

// Test that getters return the same values used in constructor
$test_width = 32;
$test_poly = 0x1021;
$test_init = 0x12345678;
$test_refin = true;
$test_refout = false;
$test_xorout = 0x87654321;
$test_check = 0x1234ABCD; // This will likely fail validation, but we'll catch it

try {
    $params_test = new Params($test_width, $test_poly, $test_init, $test_refin, $test_refout, $test_xorout, $test_check);
    echo "Getter consistency test created successfully\n";
    echo "Width matches: " . ($params_test->getWidth() === $test_width ? "true" : "false") . "\n";
    echo "Poly matches: " . ($params_test->getPoly() === $test_poly ? "true" : "false") . "\n";
    echo "Init matches: " . ($params_test->getInit() === $test_init ? "true" : "false") . "\n";
    echo "Refin matches: " . ($params_test->getRefin() === $test_refin ? "true" : "false") . "\n";
    echo "Refout matches: " . ($params_test->getRefout() === $test_refout ? "true" : "false") . "\n";
    echo "Xorout matches: " . ($params_test->getXorout() === $test_xorout ? "true" : "false") . "\n";
} catch (Exception $e) {
    echo "Getter consistency test failed validation (expected)\n";
}

echo "\nTesting edge case values:\n";

// Test with minimum values
$params_min = new Params(
    width: 32,
    poly: 1,
    init: 0,
    refin: false,
    refout: false,
    xorout: 0,
    check: 0x04040c3d  // Computed check for these parameters
);

echo "Minimum poly value: " . $params_min->getPoly() . "\n";
echo "Zero init value: " . $params_min->getInit() . "\n";
echo "False refin: " . ($params_min->getRefin() ? "true" : "false") . "\n";
echo "False refout: " . ($params_min->getRefout() ? "true" : "false") . "\n";

// Test with maximum 32-bit values
$params_max = new Params(
    width: 32,
    poly: 0xFFFFFFFF,
    init: 0xFFFFFFFF,
    refin: true,
    refout: true,
    xorout: 0xFFFFFFFF,
    check: 0x2aa0a2a4  // Computed check for these parameters
);

echo "Maximum poly value: 0x" . strtoupper(dechex($params_max->getPoly())) . "\n";
echo "Maximum init value: 0x" . strtoupper(dechex($params_max->getInit())) . "\n";
echo "Maximum xorout value: 0x" . strtoupper(dechex($params_max->getXorout())) . "\n";

echo "\nAll getter method tests completed\n";

?>
--EXPECT--
Testing getter methods with 32-bit CRC parameters:
getWidth(): 32
getPoly(): 0x4C11DB7
getInit(): 0xFFFFFFFF
getRefin(): true
getRefout(): true
getXorout(): 0xFFFFFFFF
getCheck(): 0xCBF43926
getKeys() returns array: true
getKeys() array length: 23
getKeys() first element is int: true

Testing getter methods with alternative 32-bit CRC parameters:
getWidth(): 32
getPoly(): 0x1021
getInit(): 0x0
getRefin(): false
getRefout(): false
getXorout(): 0x0
getCheck(): 0x70A79F31
getKeys() returns array: true
getKeys() array length: 23
getKeys() first element is int: true

Testing getter methods consistency:
Getter consistency test failed validation (expected)

Testing edge case values:
Minimum poly value: 1
Zero init value: 0
False refin: false
False refout: false
Maximum poly value: 0xFFFFFFFF
Maximum init value: 0xFFFFFFFF
Maximum xorout value: 0xFFFFFFFF

All getter method tests completed