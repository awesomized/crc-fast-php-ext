--TEST--
hash_file() test
--EXTENSIONS--
crc_fast
--FILE--
<?php
$tempfile = tmpfile();
fwrite($tempfile, '123456789');
fsync($tempfile);

$path = stream_get_meta_data($tempfile)['uri'];

// hex output
var_dump(CrcFast\hash_file(CrcFast\CRC_32_ISCSI, $path, false));

// binary output
var_dump(bin2hex(CrcFast\hash_file(CrcFast\CRC_32_ISCSI, $path, true)));

fclose($tempfile);
?>
--EXPECT--
string(8) "e3069283"
string(8) "e3069283"
