--TEST--
CrcFast function error handling test
--FILE--
<?php

use CrcFast\Params;

// Test invalid algorithm constant
try {
    CrcFast\hash(99999, "test", false);
    echo "FAIL: Should have thrown exception for invalid algorithm\n";
} catch (Exception $e) {
    echo "PASS: Invalid algorithm error: " . $e->getMessage() . "\n";
}

// Test invalid algorithm type
try {
    CrcFast\hash("invalid", "test", false);
    echo "FAIL: Should have thrown exception for invalid algorithm type\n";
} catch (Exception $e) {
    echo "PASS: Invalid algorithm type error: " . $e->getMessage() . "\n";
}

// Test empty filename
try {
    CrcFast\hash_file(CrcFast\CRC_32_ISO_HDLC, "", false);
    echo "FAIL: Should have thrown exception for empty filename\n";
} catch (Exception $e) {
    echo "PASS: Empty filename error: " . $e->getMessage() . "\n";
}

// Test negative length in combine
try {
    CrcFast\combine(CrcFast\CRC_32_ISO_HDLC, "12345678", "87654321", -1, false);
    echo "FAIL: Should have thrown exception for negative length\n";
} catch (Exception $e) {
    echo "PASS: Negative length error: " . $e->getMessage() . "\n";
}

// Test invalid checksum length in combine (this generates a warning, not an exception)
$result = @CrcFast\combine(CrcFast\CRC_32_ISO_HDLC, "123", "87654321", 100, false);
if ($result === false) {
    echo "PASS: Invalid checksum length generates warning and returns false\n";
} else {
    echo "FAIL: Should have returned false for invalid checksum length\n";
}

echo "All function error handling tests completed\n";

?>
--EXPECT--
PASS: Invalid algorithm error: Invalid algorithm constant 99999. Use CrcFast\get_supported_algorithms() to see valid values
PASS: Invalid algorithm type error: Algorithm parameter must be an integer constant or CrcFast\Params object, string given
PASS: Empty filename error: Filename cannot be empty
PASS: Negative length error: Length parameter must be non-negative, got -1
PASS: Invalid checksum length generates warning and returns false
All function error handling tests completed