--TEST--
Digest constructor with custom parameters test
--EXTENSIONS--
crc_fast
--FILE--
<?php
// Test Digest constructor with custom parameters that match CRC-32/ISO-HDLC
$params = new CrcFast\Params(
    width: 32,
    poly: 0x04C11DB7,
    init: 0xFFFFFFFF,
    refin: true,
    refout: true,
    xorout: 0xFFFFFFFF,
    check: 0xCBF43926
);

// Test with predefined algorithm
$digest1 = new CrcFast\Digest(CrcFast\CRC_32_ISO_HDLC);
$digest1->update("123456789");
$result1 = $digest1->finalize();

// Test with custom parameters
$digest2 = new CrcFast\Digest($params);
$digest2->update("123456789");
$result2 = $digest2->finalize();

// Results should match
var_dump($result1 === $result2);
var_dump($result1);
var_dump($result2);

// Test binary output with custom parameters
$digest3 = new CrcFast\Digest($params);
$digest3->update("123456789");
$result3 = $digest3->finalize(true);
var_dump(strlen($result3));
var_dump(bin2hex($result3));

// Test finalizeReset with custom parameters
$digest4 = new CrcFast\Digest($params);
$digest4->update("123456789");
$result4 = $digest4->finalizeReset();
var_dump($result4);
?>
--EXPECT--
bool(true)
string(8) "cbf43926"
string(8) "cbf43926"
int(4)
string(8) "cbf43926"
string(8) "cbf43926"