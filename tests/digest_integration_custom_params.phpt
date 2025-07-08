--TEST--
CrcFast\Digest class with custom parameters integration test
--EXTENSIONS--
crc_fast
--FILE--
<?php

echo "Testing CrcFast\Digest class with custom parameters\n\n";

// Test 1: Digest creation, update, and finalization
echo "Test 1: Basic digest operations with custom parameters\n";
$params_iso_hdlc = new CrcFast\Params(
    width: 32,
    poly: 0x04C11DB7,
    init: 0xFFFFFFFF,
    refin: true,
    refout: true,
    xorout: 0xFFFFFFFF,
    check: 0xCBF43926
);

// Test with predefined algorithm
$digest_predefined = new CrcFast\Digest(CrcFast\CRC_32_ISO_HDLC);
$digest_predefined->update("123456789");
$result_predefined = $digest_predefined->finalize();

// Test with custom parameters
$digest_custom = new CrcFast\Digest($params_iso_hdlc);
$digest_custom->update("123456789");
$result_custom = $digest_custom->finalize();

echo "Predefined result: " . $result_predefined . "\n";
echo "Custom result: " . $result_custom . "\n";
echo "Results match: " . ($result_predefined === $result_custom ? "true" : "false") . "\n\n";

// Test 2: Verify digest matches hash() function
echo "Test 2: Digest vs hash() function consistency\n";
$hash_result = CrcFast\hash($params_iso_hdlc, "123456789", false);
echo "Hash result: " . $hash_result . "\n";
echo "Digest result: " . $result_custom . "\n";
echo "Hash vs digest match: " . ($hash_result === $result_custom ? "true" : "false") . "\n\n";

// Test 3: Multiple updates vs single update
echo "Test 3: Multiple updates consistency\n";
$digest_multi = new CrcFast\Digest($params_iso_hdlc);
$digest_multi->update("123");
$digest_multi->update("456");
$digest_multi->update("789");
$result_multi = $digest_multi->finalize();

$digest_single = new CrcFast\Digest($params_iso_hdlc);
$digest_single->update("123456789");
$result_single = $digest_single->finalize();

echo "Multi-update result: " . $result_multi . "\n";
echo "Single-update result: " . $result_single . "\n";
echo "Multi vs single match: " . ($result_multi === $result_single ? "true" : "false") . "\n\n";

// Test 4: Binary output format
echo "Test 4: Binary output format test\n";
$digest_binary = new CrcFast\Digest($params_iso_hdlc);
$digest_binary->update("123456789");
$result_binary = $digest_binary->finalize(true);

echo "Binary result length: " . strlen($result_binary) . " bytes\n";
echo "Binary result hex: " . bin2hex($result_binary) . "\n";
echo "Binary matches hex: " . (bin2hex($result_binary) === $result_custom ? "true" : "false") . "\n\n";

// Test 5: Reset functionality
echo "Test 5: Reset functionality test\n";
$digest_reset = new CrcFast\Digest($params_iso_hdlc);
$digest_reset->update("wrong data");
$digest_reset->reset();
$digest_reset->update("123456789");
$result_reset = $digest_reset->finalize();

echo "Reset result: " . $result_reset . "\n";
echo "Reset matches expected: " . ($result_reset === $result_custom ? "true" : "false") . "\n\n";

// Test 6: FinalizeReset functionality
echo "Test 6: FinalizeReset functionality test\n";
$digest_finalize_reset = new CrcFast\Digest($params_iso_hdlc);
$digest_finalize_reset->update("123456789");
$result_finalize_reset = $digest_finalize_reset->finalizeReset();

// After finalizeReset, digest should be reset
$empty_result = $digest_finalize_reset->finalize();

echo "FinalizeReset result: " . $result_finalize_reset . "\n";
echo "FinalizeReset matches expected: " . ($result_finalize_reset === $result_custom ? "true" : "false") . "\n";
echo "Empty result after reset: " . $empty_result . "\n";
echo "Empty result is initial value: " . ($empty_result === "00000000" ? "true" : "false") . "\n\n";

// Test 7: Digest combination with custom parameters
echo "Test 7: Digest combination test\n";
$params_iscsi = new CrcFast\Params(
    width: 32,
    poly: 0x1EDC6F41,
    init: 0xFFFFFFFF,
    refin: true,
    refout: true,
    xorout: 0xFFFFFFFF,
    check: 0xE3069283
);

$digest1 = new CrcFast\Digest($params_iscsi);
$digest1->update("1234");

$digest2 = new CrcFast\Digest($params_iscsi);
$digest2->update("56789");

// Combine the digests
$digest1->combine($digest2);
$combined_result = $digest1->finalize();

// Verify against manual calculation
$manual_combined = CrcFast\hash($params_iscsi, "123456789", false);

echo "Combined result: " . $combined_result . "\n";
echo "Manual combined: " . $manual_combined . "\n";
echo "Combine matches manual: " . ($combined_result === $manual_combined ? "true" : "false") . "\n";

// Verify against predefined algorithm
$predefined_digest1 = new CrcFast\Digest(CrcFast\CRC_32_ISCSI);
$predefined_digest1->update("1234");
$predefined_digest1->finalize(); // Need to finalize before combine

$predefined_digest2 = new CrcFast\Digest(CrcFast\CRC_32_ISCSI);
$predefined_digest2->update("56789");

$predefined_digest1->combine($predefined_digest2);
$predefined_combined = $predefined_digest1->finalize();

echo "Predefined combined: " . $predefined_combined . "\n";
echo "Custom vs predefined combine: " . ($combined_result === $predefined_combined ? "true" : "false") . "\n\n";

// Test 8: Different algorithms comparison
echo "Test 8: Different custom algorithms test\n";
$test_data = "Hello, World!";

// ISO-HDLC
$digest_iso = new CrcFast\Digest($params_iso_hdlc);
$digest_iso->update($test_data);
$result_iso = $digest_iso->finalize();

// ISCSI
$digest_iscsi = new CrcFast\Digest($params_iscsi);
$digest_iscsi->update($test_data);
$result_iscsi = $digest_iscsi->finalize();

echo "ISO-HDLC result: " . $result_iso . "\n";
echo "ISCSI result: " . $result_iscsi . "\n";
echo "Results are different: " . ($result_iso !== $result_iscsi ? "true" : "false") . "\n\n";

echo "All Digest custom parameter tests completed\n";

?>
--EXPECT--
Testing CrcFast\Digest class with custom parameters

Test 1: Basic digest operations with custom parameters
Predefined result: cbf43926
Custom result: cbf43926
Results match: true

Test 2: Digest vs hash() function consistency
Hash result: cbf43926
Digest result: cbf43926
Hash vs digest match: true

Test 3: Multiple updates consistency
Multi-update result: cbf43926
Single-update result: cbf43926
Multi vs single match: true

Test 4: Binary output format test
Binary result length: 4 bytes
Binary result hex: cbf43926
Binary matches hex: true

Test 5: Reset functionality test
Reset result: cbf43926
Reset matches expected: true

Test 6: FinalizeReset functionality test
FinalizeReset result: cbf43926
FinalizeReset matches expected: true
Empty result after reset: 00000000
Empty result is initial value: true

Test 7: Digest combination test
Combined result: e3069283
Manual combined: e3069283
Combine matches manual: true
Predefined combined: e3069283
Custom vs predefined combine: true

Test 8: Different custom algorithms test
ISO-HDLC result: ec4ac3d0
ISCSI result: 4d551068
Results are different: true

All Digest custom parameter tests completed