--TEST--
CrcFast\combine() with custom parameters integration test
--EXTENSIONS--
crc_fast
--FILE--
<?php

echo "Testing CrcFast\combine() with custom parameters\n\n";

// Test data
$data1 = '1234';
$data2 = '56789';
$combined_data = $data1 . $data2;

// Test 1: CRC-32/ISO-HDLC custom parameters vs predefined
echo "Test 1: CRC-32/ISO-HDLC combine comparison\n";
$params_iso_hdlc = new CrcFast\Params(
    width: 32,
    poly: 0x04C11DB7,
    init: 0xFFFFFFFF,
    refin: true,
    refout: true,
    xorout: 0xFFFFFFFF,
    check: 0xCBF43926
);

// Get individual checksums with predefined algorithm
$predefined_crc1 = CrcFast\hash(CrcFast\CRC_32_ISO_HDLC, $data1, false);
$predefined_crc2 = CrcFast\hash(CrcFast\CRC_32_ISO_HDLC, $data2, false);
$predefined_combined = CrcFast\combine(
    CrcFast\CRC_32_ISO_HDLC,
    $predefined_crc1,
    $predefined_crc2,
    strlen($data2),
    false
);

// Get individual checksums with custom parameters
$custom_crc1 = CrcFast\hash($params_iso_hdlc, $data1, false);
$custom_crc2 = CrcFast\hash($params_iso_hdlc, $data2, false);
$custom_combined = CrcFast\combine(
    $params_iso_hdlc,
    $custom_crc1,
    $custom_crc2,
    strlen($data2),
    false
);

echo "Predefined CRC1: " . $predefined_crc1 . "\n";
echo "Custom CRC1: " . $custom_crc1 . "\n";
echo "CRC1 match: " . ($predefined_crc1 === $custom_crc1 ? "true" : "false") . "\n";
echo "Predefined CRC2: " . $predefined_crc2 . "\n";
echo "Custom CRC2: " . $custom_crc2 . "\n";
echo "CRC2 match: " . ($predefined_crc2 === $custom_crc2 ? "true" : "false") . "\n";
echo "Predefined combined: " . $predefined_combined . "\n";
echo "Custom combined: " . $custom_combined . "\n";
echo "Combined match: " . ($predefined_combined === $custom_combined ? "true" : "false") . "\n\n";

// Test 2: Verify combine result matches manual calculation
echo "Test 2: Combine vs manual calculation verification\n";
$manual_combined = CrcFast\hash($params_iso_hdlc, $combined_data, false);
echo "Manual combined: " . $manual_combined . "\n";
echo "Combine result: " . $custom_combined . "\n";
echo "Manual vs combine match: " . ($manual_combined === $custom_combined ? "true" : "false") . "\n\n";

// Test 3: Binary input/output format
echo "Test 3: Binary input/output format test\n";
$binary_crc1 = CrcFast\hash($params_iso_hdlc, $data1, true);
$binary_crc2 = CrcFast\hash($params_iso_hdlc, $data2, true);

// Combine with binary inputs, hex output
$binary_to_hex = CrcFast\combine(
    $params_iso_hdlc,
    $binary_crc1,
    $binary_crc2,
    strlen($data2),
    false
);

// Combine with binary inputs, binary output
$binary_to_binary = CrcFast\combine(
    $params_iso_hdlc,
    $binary_crc1,
    $binary_crc2,
    strlen($data2),
    true
);

echo "Binary to hex: " . $binary_to_hex . "\n";
echo "Binary to binary (hex): " . bin2hex($binary_to_binary) . "\n";
echo "Binary formats match: " . ($binary_to_hex === bin2hex($binary_to_binary) ? "true" : "false") . "\n";
echo "Binary result matches hex: " . ($binary_to_hex === $custom_combined ? "true" : "false") . "\n\n";

// Test 4: CRC-32/ISCSI custom parameters
echo "Test 4: CRC-32/ISCSI combine test\n";
$params_iscsi = new CrcFast\Params(
    width: 32,
    poly: 0x1EDC6F41,
    init: 0xFFFFFFFF,
    refin: true,
    refout: true,
    xorout: 0xFFFFFFFF,
    check: 0xE3069283
);

$iscsi_crc1 = CrcFast\hash($params_iscsi, $data1, false);
$iscsi_crc2 = CrcFast\hash($params_iscsi, $data2, false);
$iscsi_combined = CrcFast\combine(
    $params_iscsi,
    $iscsi_crc1,
    $iscsi_crc2,
    strlen($data2),
    false
);

// Verify against predefined algorithm
$predefined_iscsi_combined = CrcFast\combine(
    CrcFast\CRC_32_ISCSI,
    CrcFast\hash(CrcFast\CRC_32_ISCSI, $data1, false),
    CrcFast\hash(CrcFast\CRC_32_ISCSI, $data2, false),
    strlen($data2),
    false
);

echo "ISCSI custom combined: " . $iscsi_combined . "\n";
echo "ISCSI predefined combined: " . $predefined_iscsi_combined . "\n";
echo "ISCSI combine match: " . ($iscsi_combined === $predefined_iscsi_combined ? "true" : "false") . "\n";

// Verify against manual calculation
$iscsi_manual = CrcFast\hash($params_iscsi, $combined_data, false);
echo "ISCSI manual: " . $iscsi_manual . "\n";
echo "ISCSI manual vs combine: " . ($iscsi_manual === $iscsi_combined ? "true" : "false") . "\n\n";

// Test 5: Different data lengths
echo "Test 5: Different data lengths test\n";
$short_data = 'A';
$long_data = 'BCDEFGHIJKLMNOP';

$short_crc = CrcFast\hash($params_iso_hdlc, $short_data, false);
$long_crc = CrcFast\hash($params_iso_hdlc, $long_data, false);
$length_combined = CrcFast\combine(
    $params_iso_hdlc,
    $short_crc,
    $long_crc,
    strlen($long_data),
    false
);

$length_manual = CrcFast\hash($params_iso_hdlc, $short_data . $long_data, false);
echo "Length test combined: " . $length_combined . "\n";
echo "Length test manual: " . $length_manual . "\n";
echo "Length test match: " . ($length_combined === $length_manual ? "true" : "false") . "\n\n";

echo "All combine() custom parameter tests completed\n";

?>
--EXPECT--
Testing CrcFast\combine() with custom parameters

Test 1: CRC-32/ISO-HDLC combine comparison
Predefined CRC1: 9be3e0a3
Custom CRC1: 9be3e0a3
CRC1 match: true
Predefined CRC2: 131da070
Custom CRC2: 131da070
CRC2 match: true
Predefined combined: cbf43926
Custom combined: cbf43926
Combined match: true

Test 2: Combine vs manual calculation verification
Manual combined: cbf43926
Combine result: cbf43926
Manual vs combine match: true

Test 3: Binary input/output format test
Binary to hex: cbf43926
Binary to binary (hex): cbf43926
Binary formats match: true
Binary result matches hex: true

Test 4: CRC-32/ISCSI combine test
ISCSI custom combined: e3069283
ISCSI predefined combined: e3069283
ISCSI combine match: true
ISCSI manual: e3069283
ISCSI manual vs combine: true

Test 5: Different data lengths test
Length test combined: e0e8ff4d
Length test manual: e0e8ff4d
Length test match: true

All combine() custom parameter tests completed