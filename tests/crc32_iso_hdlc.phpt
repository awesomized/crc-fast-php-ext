--TEST--
CRC-32/ISO-HDLC test
--EXTENSIONS--
crc_fast
--FILE--
<?php
// checksum hex output
var_dump(CrcFast\hash(CrcFast\CRC_32_ISO_HDLC, '123456789', false));

// checksum binary output
var_dump(bin2hex(CrcFast\hash(CrcFast\CRC_32_ISO_HDLC, '123456789', true)));

$digest = new CrcFast\Digest(CrcFast\CRC_32_ISO_HDLC);
$digest->update('123456789');

// digest hex output
var_dump($digest->finalize(false));

// digest binary output
var_dump(bin2hex($digest->finalize(true)));
?>
--EXPECT--
string(8) "cbf43926"
string(8) "cbf43926"
string(8) "cbf43926"
string(8) "cbf43926"
