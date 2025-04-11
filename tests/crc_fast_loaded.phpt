--TEST--
Check if crc_fast is loaded
--EXTENSIONS--
crc_fast
--FILE--
<?php
echo 'The extension "crc_fast" is available';
?>
--EXPECT--
The extension "crc_fast" is available
