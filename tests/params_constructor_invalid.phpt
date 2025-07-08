--TEST--
CrcFast\Params constructor with invalid parameters test
--EXTENSIONS--
crc_fast
--FILE--
<?php

use CrcFast\Params;

echo "Testing invalid width values:\n";

// Test width = 16 (unsupported)
try {
    new Params(16, 0x1021, 0xFFFF, true, true, 0x0000, 0x29B1);
    echo "FAIL: Should have thrown exception for width 16\n";
} catch (Exception $e) {
    echo "PASS: Width 16 rejected: " . $e->getMessage() . "\n";
}

// Test width = 8 (unsupported)
try {
    new Params(8, 0x07, 0x00, false, false, 0x00, 0xF4);
    echo "FAIL: Should have thrown exception for width 8\n";
} catch (Exception $e) {
    echo "PASS: Width 8 rejected: " . $e->getMessage() . "\n";
}

// Test width = 128 (unsupported)
try {
    new Params(128, 0x1021, 0xFFFF, true, true, 0x0000, 0x29B1);
    echo "FAIL: Should have thrown exception for width 128\n";
} catch (Exception $e) {
    echo "PASS: Width 128 rejected: " . $e->getMessage() . "\n";
}

echo "\nTesting out-of-range values for 32-bit width:\n";

// Test polynomial too large for 32-bit
try {
    new Params(32, 0x1FFFFFFFF, 0xFFFFFFFF, true, true, 0x00000000, 0x29B1);
    echo "FAIL: Should have thrown exception for polynomial too large\n";
} catch (Exception $e) {
    echo "PASS: Large polynomial rejected: " . $e->getMessage() . "\n";
}

// Test init value too large for 32-bit
try {
    new Params(32, 0x04C11DB7, 0x1FFFFFFFF, true, true, 0x00000000, 0x29B1);
    echo "FAIL: Should have thrown exception for init value too large\n";
} catch (Exception $e) {
    echo "PASS: Large init value rejected: " . $e->getMessage() . "\n";
}

// Test xorout value too large for 32-bit
try {
    new Params(32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0x1FFFFFFFF, 0x29B1);
    echo "FAIL: Should have thrown exception for xorout value too large\n";
} catch (Exception $e) {
    echo "PASS: Large xorout value rejected: " . $e->getMessage() . "\n";
}

// Test check value too large for 32-bit
try {
    new Params(32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0x00000000, 0x1FFFFFFFF);
    echo "FAIL: Should have thrown exception for check value too large\n";
} catch (Exception $e) {
    echo "PASS: Large check value rejected: " . $e->getMessage() . "\n";
}

echo "\nTesting invalid keys array:\n";

// Test keys array with wrong length
try {
    new Params(32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0x00000000, 0xCBF43926, [1, 2, 3]);
    echo "FAIL: Should have thrown exception for wrong keys array length\n";
} catch (Exception $e) {
    echo "PASS: Wrong keys length rejected: " . $e->getMessage() . "\n";
}

// Test keys array with non-integer values
try {
    $keys = array_fill(0, 23, 0);
    $keys[5] = "not an integer";
    new Params(32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0x00000000, 0xCBF43926, $keys);
    echo "FAIL: Should have thrown exception for non-integer key values\n";
} catch (Exception $e) {
    echo "PASS: Non-integer key rejected: " . $e->getMessage() . "\n";
}

// Test keys array with negative values
try {
    $keys = array_fill(0, 23, 0);
    $keys[10] = -1;
    new Params(32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0x00000000, 0xCBF43926, $keys);
    echo "FAIL: Should have thrown exception for negative key values\n";
} catch (Exception $e) {
    echo "PASS: Negative key rejected: " . $e->getMessage() . "\n";
}

echo "\nTesting invalid check value (parameters that don't produce expected check):\n";

// Test with wrong check value
try {
    new Params(32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0x00000000, 0x12345678);
    echo "FAIL: Should have thrown exception for invalid check value\n";
} catch (Exception $e) {
    echo "PASS: Invalid check value rejected: " . $e->getMessage() . "\n";
}

echo "\nTesting negative parameter values:\n";

// Test negative polynomial
try {
    new Params(32, -1, 0xFFFFFFFF, true, true, 0x00000000, 0xCBF43926);
    echo "FAIL: Should have thrown exception for negative polynomial\n";
} catch (Exception $e) {
    echo "PASS: Negative polynomial rejected: " . $e->getMessage() . "\n";
}

// Test negative init value
try {
    new Params(32, 0x04C11DB7, -1, true, true, 0x00000000, 0xCBF43926);
    echo "FAIL: Should have thrown exception for negative init value\n";
} catch (Exception $e) {
    echo "PASS: Negative init value rejected: " . $e->getMessage() . "\n";
}

echo "\nAll invalid parameter tests completed\n";

?>
--EXPECT--
Testing invalid width values:
PASS: Width 16 rejected: Invalid width 16. Only 32 and 64 bit widths are supported
PASS: Width 8 rejected: Invalid width 8. Only 32 and 64 bit widths are supported
PASS: Width 128 rejected: Invalid width 128. Only 32 and 64 bit widths are supported

Testing out-of-range values for 32-bit width:
PASS: Large polynomial rejected: Polynomial 0x1ffffffff exceeds maximum value for 32-bit width
PASS: Large init value rejected: Init value 0x1ffffffff exceeds maximum value for 32-bit width
PASS: Large xorout value rejected: Xorout value 0x1ffffffff exceeds maximum value for 32-bit width
PASS: Large check value rejected: Check value 0x1ffffffff exceeds maximum value for 32-bit width

Testing invalid keys array:
PASS: Wrong keys length rejected: Keys array must contain exactly 23 elements, got 3
PASS: Non-integer key rejected: All keys must be integers, element 5 is not an integer
PASS: Negative key rejected: Key values cannot be negative, element 10 has value -1

Testing invalid check value (parameters that don't produce expected check):
PASS: Invalid check value rejected: Parameters validation failed: computed check 0x00000000340bc6d9 does not match expected check 0x12345678. Please verify your CRC parameters are correct

Testing negative parameter values:
PASS: Negative polynomial rejected: Polynomial value -1 cannot be negative
PASS: Negative init value rejected: Init value -1 cannot be negative

All invalid parameter tests completed