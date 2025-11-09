--TEST--
CrcFast\hash_file() with custom parameters integration test
--EXTENSIONS--
crc_fast
--FILE--
<?php

echo "Testing CrcFast\hash_file() with custom parameters\n\n";

// Create test files with different content
$tempfile1 = tmpfile();
$test_data1 = '123456789';
fwrite($tempfile1, $test_data1);
fsync($tempfile1);
$path1 = stream_get_meta_data($tempfile1)['uri'];

$tempfile2 = tmpfile();
$test_data2 = 'Hello, World!';
fwrite($tempfile2, $test_data2);
fsync($tempfile2);
$path2 = stream_get_meta_data($tempfile2)['uri'];

// Test 1: CRC-32/ISO-HDLC custom parameters vs predefined
echo "Test 1: CRC-32/ISO-HDLC file hashing comparison\n";
$params_iso_hdlc = new CrcFast\Params(
    width: 32,
    poly: 0x04C11DB7,
    init: 0xFFFFFFFF,
    refin: true,
    refout: true,
    xorout: 0xFFFFFFFF,
    check: 0xCBF43926
);

// Compare hex output for file
$predefined_file_hex = CrcFast\hash_file(CrcFast\CRC_32_ISO_HDLC, $path1, false);
$custom_file_hex = CrcFast\hash_file($params_iso_hdlc, $path1, false);
echo "Predefined file hex: " . $predefined_file_hex . "\n";
echo "Custom file hex: " . $custom_file_hex . "\n";
echo "File hex match: " . ($predefined_file_hex === $custom_file_hex ? "true" : "false") . "\n";

// Compare binary output for file
$predefined_file_bin = CrcFast\hash_file(CrcFast\CRC_32_ISO_HDLC, $path1, true);
$custom_file_bin = CrcFast\hash_file($params_iso_hdlc, $path1, true);
echo "Predefined file binary: " . bin2hex($predefined_file_bin) . "\n";
echo "Custom file binary: " . bin2hex($custom_file_bin) . "\n";
echo "File binary match: " . ($predefined_file_bin === $custom_file_bin ? "true" : "false") . "\n\n";

// Test 2: Verify hash_file() matches hash() for same data
echo "Test 2: hash_file() vs hash() consistency\n";
$string_result = CrcFast\hash($params_iso_hdlc, $test_data1, false);
$file_result = CrcFast\hash_file($params_iso_hdlc, $path1, false);
echo "String hash result: " . $string_result . "\n";
echo "File hash result: " . $file_result . "\n";
echo "String vs file match: " . ($string_result === $file_result ? "true" : "false") . "\n\n";

// Test 3: CRC-32/ISCSI custom parameters with different file
echo "Test 3: CRC-32/ISCSI with different file content\n";
$params_iscsi = new CrcFast\Params(
    width: 32,
    poly: 0x1EDC6F41,
    init: 0xFFFFFFFF,
    refin: true,
    refout: true,
    xorout: 0xFFFFFFFF,
    check: 0xE3069283
);

$predefined_file2_hex = CrcFast\hash_file(CrcFast\CRC_32_ISCSI, $path2, false);
$custom_file2_hex = CrcFast\hash_file($params_iscsi, $path2, false);
echo "Predefined file2 hex: " . $predefined_file2_hex . "\n";
echo "Custom file2 hex: " . $custom_file2_hex . "\n";
echo "File2 hex match: " . ($predefined_file2_hex === $custom_file2_hex ? "true" : "false") . "\n";

// Verify this matches the string hash too
$string2_result = CrcFast\hash($params_iscsi, $test_data2, false);
echo "String2 hash result: " . $string2_result . "\n";
echo "String2 vs file2 match: " . ($string2_result === $custom_file2_hex ? "true" : "false") . "\n\n";

// Test 4: Binary output format consistency
echo "Test 4: Binary output format consistency\n";
$string_binary = CrcFast\hash($params_iso_hdlc, $test_data1, true);
$file_binary = CrcFast\hash_file($params_iso_hdlc, $path1, true);
echo "String binary length: " . strlen($string_binary) . " bytes\n";
echo "File binary length: " . strlen($file_binary) . " bytes\n";
echo "String binary hex: " . bin2hex($string_binary) . "\n";
echo "File binary hex: " . bin2hex($file_binary) . "\n";
echo "Binary format match: " . ($string_binary === $file_binary ? "true" : "false") . "\n\n";

// Test 5: Empty file test
echo "Test 5: Empty file test\n";
$empty_file = tmpfile();
fsync($empty_file);
$empty_path = stream_get_meta_data($empty_file)['uri'];

$empty_string_result = CrcFast\hash($params_iso_hdlc, "", false);
$empty_file_result = CrcFast\hash_file($params_iso_hdlc, $empty_path, false);
echo "Empty string result: " . $empty_string_result . "\n";
echo "Empty file result: " . $empty_file_result . "\n";
echo "Empty file match: " . ($empty_string_result === $empty_file_result ? "true" : "false") . "\n\n";

// Clean up
fclose($tempfile1);
fclose($tempfile2);
fclose($empty_file);

echo "All hash_file() custom parameter tests completed\n";

?>
--EXPECT--
Testing CrcFast\hash_file() with custom parameters

Test 1: CRC-32/ISO-HDLC file hashing comparison
Predefined file hex: cbf43926
Custom file hex: cbf43926
File hex match: true
Predefined file binary: cbf43926
Custom file binary: cbf43926
File binary match: true

Test 2: hash_file() vs hash() consistency
String hash result: cbf43926
File hash result: cbf43926
String vs file match: true

Test 3: CRC-32/ISCSI with different file content
Predefined file2 hex: 4d551068
Custom file2 hex: 4d551068
File2 hex match: true
String2 hash result: 4d551068
String2 vs file2 match: true

Test 4: Binary output format consistency
String binary length: 4 bytes
File binary length: 4 bytes
String binary hex: cbf43926
File binary hex: cbf43926
Binary format match: true

Test 5: Empty file test
Empty string result: 00000000
Empty file result: 00000000
Empty file match: true

All hash_file() custom parameter tests completed