--TEST--
Digest::reset() test
--EXTENSIONS--
crc_fast
--FILE--
<?php
// digest CRC-32
$digest = new CrcFast\Digest(CrcFast\CRC_32_ISCSI);
$digest->update('123');
$digest->update('456');
$digest->update('789');
var_dump($digest->finalize(false));

$digest->reset();
var_dump($digest->finalize(false));

$digest->update('123456789');
var_dump($digest->finalizeReset(false));

var_dump($digest->finalize(false));

// digest CRC-64
$digest = new CrcFast\Digest(CrcFast\CRC_64_NVME);
$digest->update('123');
$digest->update('456');
$digest->update('789');
var_dump($digest->finalize(false));

$digest->reset();
var_dump($digest->finalize(false));

$digest->update('123456789');
var_dump($digest->finalizeReset(false));

var_dump($digest->finalize(false));

?>
--EXPECT--
string(8) "e3069283"
string(8) "00000000"
string(8) "e3069283"
string(8) "00000000"
string(16) "ae8b14860a799888"
string(16) "0000000000000000"
string(16) "ae8b14860a799888"
string(16) "0000000000000000"
