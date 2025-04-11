--TEST--
crc32() test
--EXTENSIONS--
crc_fast
--FILE--
<?php
// crc32() output
var_dump(CrcFast\crc32('123456789'));
?>
--EXPECT--
int(3421780262)
