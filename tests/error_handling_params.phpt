--TEST--
CrcFast\Params error handling test
--FILE--
<?php

use CrcFast\Params;

// Test invalid width
try {
    new Params(16, 0x1021, 0xFFFF, true, true, 0x0000, 0x29B1);
    echo "FAIL: Should have thrown exception for invalid width\n";
} catch (Exception $e) {
    echo "PASS: Invalid width error: " . $e->getMessage() . "\n";
}

// Test polynomial too large for width
try {
    new Params(32, 0x1FFFFFFFF, 0xFFFFFFFF, true, true, 0x00000000, 0x29B1);
    echo "FAIL: Should have thrown exception for polynomial too large\n";
} catch (Exception $e) {
    echo "PASS: Polynomial too large error: " . $e->getMessage() . "\n";
}

// Test init value too large for width
try {
    new Params(32, 0x04C11DB7, 0x1FFFFFFFF, true, true, 0x00000000, 0x29B1);
    echo "FAIL: Should have thrown exception for init value too large\n";
} catch (Exception $e) {
    echo "PASS: Init value too large error: " . $e->getMessage() . "\n";
}

// Test xorout value too large for width
try {
    new Params(32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0x1FFFFFFFF, 0x29B1);
    echo "FAIL: Should have thrown exception for xorout value too large\n";
} catch (Exception $e) {
    echo "PASS: Xorout value too large error: " . $e->getMessage() . "\n";
}

// Test check value too large for width
try {
    new Params(32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0x00000000, 0x1FFFFFFFF);
    echo "FAIL: Should have thrown exception for check value too large\n";
} catch (Exception $e) {
    echo "PASS: Check value too large error: " . $e->getMessage() . "\n";
}

// Test invalid keys array length
try {
    new Params(32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0x00000000, 0x29B1, [1, 2, 3]);
    echo "FAIL: Should have thrown exception for invalid keys array length\n";
} catch (Exception $e) {
    echo "PASS: Invalid keys array length error: " . $e->getMessage() . "\n";
}

// Test non-integer key values
try {
    $keys = array_fill(0, 23, 0);
    $keys[5] = "not an integer";
    new Params(32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0x00000000, 0x29B1, $keys);
    echo "FAIL: Should have thrown exception for non-integer key values\n";
} catch (Exception $e) {
    echo "PASS: Non-integer key values error: " . $e->getMessage() . "\n";
}

// Test negative key values
try {
    $keys = array_fill(0, 23, 0);
    $keys[10] = -1;
    new Params(32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0x00000000, 0x29B1, $keys);
    echo "FAIL: Should have thrown exception for negative key values\n";
} catch (Exception $e) {
    echo "PASS: Negative key values error: " . $e->getMessage() . "\n";
}

// Test invalid check value (parameters that don't produce expected check)
try {
    new Params(32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0x00000000, 0x12345678);
    echo "FAIL: Should have thrown exception for invalid check value\n";
} catch (Exception $e) {
    echo "PASS: Invalid check value error: " . $e->getMessage() . "\n";
}

echo "All error handling tests completed\n";

?>
--EXPECT--
PASS: Invalid width error: Invalid width 16. Only 32 and 64 bit widths are supported
PASS: Polynomial too large error: Polynomial 0x1ffffffff exceeds maximum value for 32-bit width
PASS: Init value too large error: Init value 0x1ffffffff exceeds maximum value for 32-bit width
PASS: Xorout value too large error: Xorout value 0x1ffffffff exceeds maximum value for 32-bit width
PASS: Check value too large error: Check value 0x1ffffffff exceeds maximum value for 32-bit width
PASS: Invalid keys array length error: Keys array must contain exactly 23 elements, got 3
PASS: Non-integer key values error: All keys must be integers, element 5 is not an integer
PASS: Negative key values error: Key values cannot be negative, element 10 has value -1
PASS: Invalid check value error: Parameters validation failed: computed check 0x00000000340bc6d9 does not match expected check 0x0000000012345678. Please verify your CRC parameters are correct
All error handling tests completed