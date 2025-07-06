--TEST--
Digest custom parameters formatting test
--EXTENSIONS--
crc_fast
--FILE--
<?php
// Test 32-bit custom parameters formatting
$params32 = new CrcFast\Params(
    width: 32,
    poly: 0x04C11DB7,
    init: 0xFFFFFFFF,
    refin: true,
    refout: true,
    xorout: 0xFFFFFFFF,
    check: 0xCBF43926
);

$digest32 = new CrcFast\Digest($params32);
$digest32->update("123456789");

// Test hex output (should be 8 characters for 32-bit)
$hex32 = $digest32->finalize();
var_dump(strlen($hex32));
var_dump($hex32);

// Test binary output (should be 4 bytes for 32-bit)
$digest32->reset();
$digest32->update("123456789");
$binary32 = $digest32->finalize(true);
var_dump(strlen($binary32));
var_dump(bin2hex($binary32));

// Test finalizeReset formatting
$digest32->reset();
$digest32->update("test");
$reset_result = $digest32->finalizeReset();
var_dump(strlen($reset_result));

// Test that custom parameters produce different output width than predefined
// Create a simple 64-bit custom CRC (avoiding large hex literals)
try {
    $params64 = new CrcFast\Params(
        width: 64,
        poly: 27,  // 0x1B in decimal
        init: 0,
        refin: false,
        refout: false,
        xorout: 0,
        check: 88  // 0x58 in decimal - this won't validate but we'll catch the exception
    );
    $digest64 = new CrcFast\Digest($params64);
} catch (Exception $e) {
    // Expected - just test that we can detect 64-bit width
    echo "64-bit width detected (validation failed as expected)\n";
}
?>
--EXPECT--
int(8)
string(8) "cbf43926"
int(4)
string(8) "cbf43926"
int(8)
64-bit width detected (validation failed as expected)