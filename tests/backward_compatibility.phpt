--TEST--
Backward compatibility test - all existing functionality unchanged
--EXTENSIONS--
crc_fast
--FILE--
<?php
// Test 1: All predefined algorithm constants are available and have correct values
echo "=== Testing Algorithm Constants ===\n";
$expected_constants = [
    'CRC_32_AIXM' => 10000,
    'CRC_32_AUTOSAR' => 10010,
    'CRC_32_BASE_91_D' => 10020,
    'CRC_32_BZIP2' => 10030,
    'CRC_32_CD_ROM_EDC' => 10040,
    'CRC_32_CKSUM' => 10050,
    'CRC_32_ISCSI' => 10060,
    'CRC_32_ISO_HDLC' => 10070,
    'CRC_32_JAMCRC' => 10080,
    'CRC_32_MEF' => 10090,
    'CRC_32_MPEG_2' => 10100,
    'CRC_32_PHP' => 10200,
    'CRC_32_XFER' => 10300,
    'CRC_64_ECMA_182' => 20000,
    'CRC_64_GO_ISO' => 20010,
    'CRC_64_MS' => 20020,
    'CRC_64_NVME' => 20030,
    'CRC_64_REDIS' => 20040,
    'CRC_64_WE' => 20050,
    'CRC_64_XZ' => 20060,
];

foreach ($expected_constants as $name => $expected_value) {
    $constant_name = "CrcFast\\$name";
    if (defined($constant_name)) {
        $actual_value = constant($constant_name);
        if ($actual_value === $expected_value) {
            echo "PASS: $constant_name = $actual_value\n";
        } else {
            echo "FAIL: $constant_name expected $expected_value, got $actual_value\n";
        }
    } else {
        echo "FAIL: $constant_name not defined\n";
    }
}

// Test 2: All original function signatures work with integer algorithm parameters
echo "\n=== Testing Original Function Signatures ===\n";

// Test CrcFast\hash() with int parameter
$hash_result = CrcFast\hash(CrcFast\CRC_32_ISCSI, '123456789', false);
echo "CrcFast\\hash() with int: " . ($hash_result === 'e3069283' ? 'PASS' : 'FAIL') . "\n";

// Test CrcFast\hash_file() with int parameter
$tempfile = tmpfile();
fwrite($tempfile, '123456789');
fsync($tempfile);
$path = stream_get_meta_data($tempfile)['uri'];
$file_hash_result = CrcFast\hash_file(CrcFast\CRC_32_ISCSI, $path, false);
fclose($tempfile);
echo "CrcFast\\hash_file() with int: " . ($file_hash_result === 'e3069283' ? 'PASS' : 'FAIL') . "\n";

// Test CrcFast\combine() with int parameter
$combine_result = CrcFast\combine(
    CrcFast\CRC_32_ISCSI,
    CrcFast\hash(CrcFast\CRC_32_ISCSI, '1234', false),
    CrcFast\hash(CrcFast\CRC_32_ISCSI, '56789', false),
    5,
    false
);
echo "CrcFast\\combine() with int: " . ($combine_result === 'e3069283' ? 'PASS' : 'FAIL') . "\n";

// Test CrcFast\get_supported_algorithms() - should return same array
$algorithms = CrcFast\get_supported_algorithms();
$expected_count = 19;
echo "CrcFast\\get_supported_algorithms() count: " . (count($algorithms) === $expected_count ? 'PASS' : 'FAIL') . "\n";

// Test CrcFast\crc32() - should work unchanged
$crc32_result = CrcFast\crc32('123456789');
echo "CrcFast\\crc32(): " . (is_int($crc32_result) ? 'PASS' : 'FAIL') . "\n";

// Test 3: CrcFast\Digest class with integer algorithm parameters
echo "\n=== Testing Digest Class with Original Signatures ===\n";

// Test Digest constructor with int parameter
$digest = new CrcFast\Digest(CrcFast\CRC_32_ISCSI);
echo "Digest constructor with int: PASS\n";

// Test all Digest methods work as before
$digest->update('123456789');
$digest_result = $digest->finalize(false);
echo "Digest methods: " . ($digest_result === 'e3069283' ? 'PASS' : 'FAIL') . "\n";

// Test Digest reset functionality
$digest->reset();
$reset_result = $digest->finalize(false);
echo "Digest reset: " . ($reset_result === '00000000' ? 'PASS' : 'FAIL') . "\n";

// Test Digest finalizeReset functionality
$digest->update('123456789');
$finalize_reset_result = $digest->finalizeReset(false);
$after_reset_result = $digest->finalize(false);
echo "Digest finalizeReset: " . ($finalize_reset_result === 'e3069283' && $after_reset_result === '00000000' ? 'PASS' : 'FAIL') . "\n";

// Test Digest combine functionality
$digest1 = new CrcFast\Digest(CrcFast\CRC_32_ISCSI);
$digest1->update('1234');
$digest2 = new CrcFast\Digest(CrcFast\CRC_32_ISCSI);
$digest2->update('56789');
$digest1->combine($digest2);
$combine_digest_result = $digest1->finalize(false);
echo "Digest combine: " . ($combine_digest_result === 'e3069283' ? 'PASS' : 'FAIL') . "\n";

// Test 4: Binary output modes work unchanged
echo "\n=== Testing Binary Output Modes ===\n";

$binary_hash = CrcFast\hash(CrcFast\CRC_32_ISCSI, '123456789', true);
echo "Binary hash output: " . (strlen($binary_hash) === 4 ? 'PASS' : 'FAIL') . "\n";

$digest_binary = new CrcFast\Digest(CrcFast\CRC_32_ISCSI);
$digest_binary->update('123456789');
$binary_digest_result = $digest_binary->finalize(true);
echo "Binary digest output: " . (strlen($binary_digest_result) === 4 ? 'PASS' : 'FAIL') . "\n";

// Test 5: 64-bit algorithms work unchanged
echo "\n=== Testing 64-bit Algorithms ===\n";

$hash64_result = CrcFast\hash(CrcFast\CRC_64_NVME, '123456789', false);
echo "64-bit hash: " . ($hash64_result === 'ae8b14860a799888' ? 'PASS' : 'FAIL') . "\n";

$digest64 = new CrcFast\Digest(CrcFast\CRC_64_NVME);
$digest64->update('123456789');
$digest64_result = $digest64->finalize(false);
echo "64-bit digest: " . ($digest64_result === 'ae8b14860a799888' ? 'PASS' : 'FAIL') . "\n";

echo "\n=== Backward Compatibility Test Complete ===\n";
?>
--EXPECT--
=== Testing Algorithm Constants ===
PASS: CrcFast\CRC_32_AIXM = 10000
PASS: CrcFast\CRC_32_AUTOSAR = 10010
PASS: CrcFast\CRC_32_BASE_91_D = 10020
PASS: CrcFast\CRC_32_BZIP2 = 10030
PASS: CrcFast\CRC_32_CD_ROM_EDC = 10040
PASS: CrcFast\CRC_32_CKSUM = 10050
PASS: CrcFast\CRC_32_ISCSI = 10060
PASS: CrcFast\CRC_32_ISO_HDLC = 10070
PASS: CrcFast\CRC_32_JAMCRC = 10080
PASS: CrcFast\CRC_32_MEF = 10090
PASS: CrcFast\CRC_32_MPEG_2 = 10100
PASS: CrcFast\CRC_32_PHP = 10200
PASS: CrcFast\CRC_32_XFER = 10300
PASS: CrcFast\CRC_64_ECMA_182 = 20000
PASS: CrcFast\CRC_64_GO_ISO = 20010
PASS: CrcFast\CRC_64_MS = 20020
PASS: CrcFast\CRC_64_NVME = 20030
PASS: CrcFast\CRC_64_REDIS = 20040
PASS: CrcFast\CRC_64_WE = 20050
PASS: CrcFast\CRC_64_XZ = 20060

=== Testing Original Function Signatures ===
CrcFast\hash() with int: PASS
CrcFast\hash_file() with int: PASS
CrcFast\combine() with int: PASS
CrcFast\get_supported_algorithms() count: PASS
CrcFast\crc32(): PASS

=== Testing Digest Class with Original Signatures ===
Digest constructor with int: PASS
Digest methods: PASS
Digest reset: PASS
Digest finalizeReset: PASS
Digest combine: PASS

=== Testing Binary Output Modes ===
Binary hash output: PASS
Binary digest output: PASS

=== Testing 64-bit Algorithms ===
64-bit hash: PASS
64-bit digest: PASS

=== Backward Compatibility Test Complete ===