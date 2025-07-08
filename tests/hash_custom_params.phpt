--TEST--
CrcFast\hash() with custom parameters integration test
--EXTENSIONS--
crc_fast
--FILE--
<?php

echo "Testing CrcFast\hash() with custom parameters matching predefined algorithms\n\n";

// Test 1: CRC-32/ISO-HDLC custom parameters vs predefined
echo "Test 1: CRC-32/ISO-HDLC comparison\n";
$params_iso_hdlc = new CrcFast\Params(
    width: 32,
    poly: 0x04C11DB7,
    init: 0xFFFFFFFF,
    refin: true,
    refout: true,
    xorout: 0xFFFFFFFF,
    check: 0xCBF43926
);

$test_data = "123456789";

// Compare hex output
$predefined_hex = CrcFast\hash(CrcFast\CRC_32_ISO_HDLC, $test_data, false);
$custom_hex = CrcFast\hash($params_iso_hdlc, $test_data, false);
echo "Predefined hex: " . $predefined_hex . "\n";
echo "Custom hex: " . $custom_hex . "\n";
echo "Hex match: " . ($predefined_hex === $custom_hex ? "true" : "false") . "\n";

// Compare binary output
$predefined_bin = CrcFast\hash(CrcFast\CRC_32_ISO_HDLC, $test_data, true);
$custom_bin = CrcFast\hash($params_iso_hdlc, $test_data, true);
echo "Predefined binary: " . bin2hex($predefined_bin) . "\n";
echo "Custom binary: " . bin2hex($custom_bin) . "\n";
echo "Binary match: " . ($predefined_bin === $custom_bin ? "true" : "false") . "\n\n";

// Test 2: CRC-32/ISCSI custom parameters vs predefined
echo "Test 2: CRC-32/ISCSI comparison\n";
$params_iscsi = new CrcFast\Params(
    width: 32,
    poly: 0x1EDC6F41,
    init: 0xFFFFFFFF,
    refin: true,
    refout: true,
    xorout: 0xFFFFFFFF,
    check: 0xE3069283
);

// Compare hex output
$predefined_hex = CrcFast\hash(CrcFast\CRC_32_ISCSI, $test_data, false);
$custom_hex = CrcFast\hash($params_iscsi, $test_data, false);
echo "Predefined hex: " . $predefined_hex . "\n";
echo "Custom hex: " . $custom_hex . "\n";
echo "Hex match: " . ($predefined_hex === $custom_hex ? "true" : "false") . "\n";

// Compare binary output
$predefined_bin = CrcFast\hash(CrcFast\CRC_32_ISCSI, $test_data, true);
$custom_bin = CrcFast\hash($params_iscsi, $test_data, true);
echo "Predefined binary: " . bin2hex($predefined_bin) . "\n";
echo "Custom binary: " . bin2hex($custom_bin) . "\n";
echo "Binary match: " . ($predefined_bin === $custom_bin ? "true" : "false") . "\n\n";

// Test 3: Test 32-bit vs 64-bit width difference
echo "Test 3: Width difference test (32-bit vs 64-bit output)\n";
// Use the same 32-bit parameters as Test 1 to show output format consistency
$result_32 = CrcFast\hash($params_iso_hdlc, $test_data, false);
echo "32-bit result: " . $result_32 . " (length: " . strlen($result_32) . ")\n";

// Note: We'll skip the 64-bit custom parameter test due to PHP integer limitations
// with large check values, but the 32-bit tests demonstrate the functionality
echo "64-bit test skipped due to PHP integer overflow with large check values\n";
echo "\n";

// Test 4: Test with different data to ensure consistency
echo "Test 4: Different data consistency test\n";
$different_data = "Hello, World!";

$predefined_result = CrcFast\hash(CrcFast\CRC_32_ISO_HDLC, $different_data, false);
$custom_result = CrcFast\hash($params_iso_hdlc, $different_data, false);
echo "Different data - Predefined: " . $predefined_result . "\n";
echo "Different data - Custom: " . $custom_result . "\n";
echo "Different data match: " . ($predefined_result === $custom_result ? "true" : "false") . "\n\n";

// Test 5: Empty string test
echo "Test 5: Empty string test\n";
$empty_data = "";

$predefined_empty = CrcFast\hash(CrcFast\CRC_32_ISO_HDLC, $empty_data, false);
$custom_empty = CrcFast\hash($params_iso_hdlc, $empty_data, false);
echo "Empty string - Predefined: " . $predefined_empty . "\n";
echo "Empty string - Custom: " . $custom_empty . "\n";
echo "Empty string match: " . ($predefined_empty === $custom_empty ? "true" : "false") . "\n\n";

echo "All hash() custom parameter tests completed\n";

?>
--EXPECT--
Testing CrcFast\hash() with custom parameters matching predefined algorithms

Test 1: CRC-32/ISO-HDLC comparison
Predefined hex: cbf43926
Custom hex: cbf43926
Hex match: true
Predefined binary: cbf43926
Custom binary: cbf43926
Binary match: true

Test 2: CRC-32/ISCSI comparison
Predefined hex: e3069283
Custom hex: e3069283
Hex match: true
Predefined binary: e3069283
Custom binary: e3069283
Binary match: true

Test 3: Width difference test (32-bit vs 64-bit output)
32-bit result: cbf43926 (length: 8)
64-bit test skipped due to PHP integer overflow with large check values

Test 4: Different data consistency test
Different data - Predefined: ec4ac3d0
Different data - Custom: ec4ac3d0
Different data match: true

Test 5: Empty string test
Empty string - Predefined: 00000000
Empty string - Custom: 00000000
Empty string match: true

All hash() custom parameter tests completed