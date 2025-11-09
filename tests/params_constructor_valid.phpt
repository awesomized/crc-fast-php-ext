--TEST--
CrcFast\Params constructor with valid parameters test
--EXTENSIONS--
crc_fast
--FILE--
<?php

use CrcFast\Params;

echo "Testing valid 32-bit CRC parameters (CRC-32/ISO-HDLC equivalent):\n";
$params32 = new Params(
    width: 32,
    poly: 0x04C11DB7,
    init: 0xFFFFFFFF,
    refin: true,
    refout: true,
    xorout: 0xFFFFFFFF,
    check: 0xCBF43926
);

echo "32-bit params created successfully\n";
echo "Width: " . $params32->getWidth() . "\n";
echo "Poly: 0x" . strtoupper(dechex($params32->getPoly())) . "\n";
echo "Init: 0x" . strtoupper(dechex($params32->getInit())) . "\n";
echo "Refin: " . ($params32->getRefin() ? "true" : "false") . "\n";
echo "Refout: " . ($params32->getRefout() ? "true" : "false") . "\n";
echo "Xorout: 0x" . strtoupper(dechex($params32->getXorout())) . "\n";
echo "Check: 0x" . strtoupper(dechex($params32->getCheck())) . "\n";

$keys32 = $params32->getKeys();
echo "Keys array length: " . count($keys32) . "\n";
echo "Keys are integers: " . (is_int($keys32[0]) ? "true" : "false") . "\n";

echo "\nTesting valid 32-bit CRC parameters (different configuration):\n";
$params32_alt = new Params(
    width: 32,
    poly: 0x1021,  // CRC-16 CCITT polynomial extended to 32-bit
    init: 0,
    refin: false,
    refout: false,
    xorout: 0,
    check: 0x70a79f31  // Computed check value for these parameters
);

echo "Alternative 32-bit params created successfully\n";
echo "Width: " . $params32_alt->getWidth() . "\n";
echo "Poly: 0x" . strtoupper(dechex($params32_alt->getPoly())) . "\n";
echo "Init: 0x" . strtoupper(dechex($params32_alt->getInit())) . "\n";
echo "Refin: " . ($params32_alt->getRefin() ? "true" : "false") . "\n";
echo "Refout: " . ($params32_alt->getRefout() ? "true" : "false") . "\n";
echo "Xorout: 0x" . strtoupper(dechex($params32_alt->getXorout())) . "\n";
echo "Check: 0x" . strtoupper(dechex($params32_alt->getCheck())) . "\n";

$keys32_alt = $params32_alt->getKeys();
echo "Keys array length: " . count($keys32_alt) . "\n";
echo "Keys are integers: " . (is_int($keys32_alt[0]) ? "true" : "false") . "\n";

echo "\nTesting keys auto-generation:\n";
$keys_test = $params32->getKeys();
echo "Keys auto-generated: " . (count($keys_test) === 23 ? "true" : "false") . "\n";
echo "Keys are integers: " . (is_int($keys_test[0]) ? "true" : "false") . "\n";

echo "\nAll valid parameter tests passed\n";

?>
--EXPECT--
Testing valid 32-bit CRC parameters (CRC-32/ISO-HDLC equivalent):
32-bit params created successfully
Width: 32
Poly: 0x4C11DB7
Init: 0xFFFFFFFF
Refin: true
Refout: true
Xorout: 0xFFFFFFFF
Check: 0xCBF43926
Keys array length: 23
Keys are integers: true

Testing valid 32-bit CRC parameters (different configuration):
Alternative 32-bit params created successfully
Width: 32
Poly: 0x1021
Init: 0x0
Refin: false
Refout: false
Xorout: 0x0
Check: 0x70A79F31
Keys array length: 23
Keys are integers: true

Testing keys auto-generation:
Keys auto-generated: true
Keys are integers: true

All valid parameter tests passed