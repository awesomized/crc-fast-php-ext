--TEST--
CrcFast\Digest error handling test
--FILE--
<?php

use CrcFast\Digest;
use CrcFast\Params;

// Test invalid algorithm in Digest constructor
try {
    new Digest(99999);
    echo "FAIL: Should have thrown exception for invalid algorithm\n";
} catch (Exception $e) {
    echo "PASS: Invalid algorithm in Digest constructor: " . $e->getMessage() . "\n";
}

// Test invalid algorithm type in Digest constructor
try {
    new Digest("invalid");
    echo "FAIL: Should have thrown exception for invalid algorithm type\n";
} catch (Exception $e) {
    echo "PASS: Invalid algorithm type in Digest constructor: " . $e->getMessage() . "\n";
}

// Test operations on uninitialized digest (this shouldn't happen in normal usage, but test defensive programming)
// We can't easily test this since the constructor will always initialize the digest or throw an exception

// Test valid digest operations to ensure they work
$digest = new Digest(CrcFast\CRC_32_ISO_HDLC);

// Test update with valid data
$digest->update("test");
echo "PASS: Update with valid data succeeded\n";

// Test finalize
$result = $digest->finalize();
echo "PASS: Finalize succeeded, result: " . $result . "\n";

// Test reset
$digest->reset();
echo "PASS: Reset succeeded\n";

// Test finalizeReset
$digest->update("test");
$result = $digest->finalizeReset();
echo "PASS: FinalizeReset succeeded, result: " . $result . "\n";

// Test combine with another digest
$digest1 = new Digest(CrcFast\CRC_32_ISO_HDLC);
$digest2 = new Digest(CrcFast\CRC_32_ISO_HDLC);
$digest1->update("hello");
$digest2->update("world");
$digest1->combine($digest2);
echo "PASS: Combine succeeded\n";

// Test with custom parameters
$params = new Params(32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0xFFFFFFFF, 0xCBF43926);
$digest = new Digest($params);
$digest->update("123456789");
$result = $digest->finalize();
echo "PASS: Custom parameters digest succeeded, result: " . $result . "\n";

echo "All Digest error handling tests completed\n";

?>
--EXPECT--
PASS: Invalid algorithm in Digest constructor: Invalid algorithm constant 99999. Use CrcFast\get_supported_algorithms() to see valid values
PASS: Invalid algorithm type in Digest constructor: Algorithm parameter must be an integer constant or CrcFast\Params object, string given
PASS: Update with valid data succeeded
PASS: Finalize succeeded, result: d87f7e0c
PASS: Reset succeeded
PASS: FinalizeReset succeeded, result: d87f7e0c
PASS: Combine succeeded
PASS: Custom parameters digest succeeded, result: cbf43926
All Digest error handling tests completed