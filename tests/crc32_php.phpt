--TEST--
CRC-32/PHP (matches hash('crc32') in PHP) test
--EXTENSIONS--
crc_fast
--FILE--
<?php
// checksum hex output
var_dump(CrcFast\hash(CrcFast\CRC_32_PHP, '123456789', false));

// checksum binary output
var_dump(bin2hex(CrcFast\hash(CrcFast\CRC_32_PHP, '123456789', true)));

$digest = new CrcFast\Digest(CrcFast\CRC_32_PHP);
$digest->update('123456789');

// digest hex output
var_dump($digest->finalize(false));

// digest binary output
var_dump(bin2hex($digest->finalize(true)));
?>
--EXPECT--
string(8) "181989fc"
string(8) "181989fc"
string(8) "181989fc"
string(8) "181989fc"
