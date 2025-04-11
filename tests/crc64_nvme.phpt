--TEST--
CRC-64/NVME test
--EXTENSIONS--
crc_fast
--FILE--
<?php
// hex output
var_dump(CrcFast\hash(CrcFast\CRC_64_NVME, '123456789', false));

// binary output
var_dump(bin2hex(CrcFast\hash(CrcFast\CRC_64_NVME, '123456789', true)));
?>
--EXPECT--
string(16) "ae8b14860a799888"
string(16) "ae8b14860a799888"
