--TEST--
Digest::update() test
--EXTENSIONS--
crc_fast
--FILE--
<?php
// digest CRC-32
$first = new CrcFast\Digest(CrcFast\CRC_32_ISCSI);
$first->update('123');
$first->update('456');
$first->update('789');
var_dump($first->finalize(false));

// digest CRC-64
$first = new CrcFast\Digest(CrcFast\CRC_64_NVME);
$first->update('123');
$first->update('456');
$first->update('789');
var_dump($first->finalize(false));
?>
--EXPECT--
string(8) "e3069283"
string(16) "ae8b14860a799888"
