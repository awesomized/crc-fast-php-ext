--TEST--
combine() test
--EXTENSIONS--
crc_fast
--FILE--
<?php
// checksum CRC-32, binary inputs
$combined = CrcFast\combine(
    CrcFast\CRC_32_ISCSI,
    CrcFast\hash(CrcFast\CRC_32_ISCSI, '1234', true),
    CrcFast\hash(CrcFast\CRC_32_ISCSI, '56789', true),
    5,
    false
);

var_dump($combined);

// checksum CRC-32, hex inputs
$combined = CrcFast\combine(
    CrcFast\CRC_32_ISCSI,
    CrcFast\hash(CrcFast\CRC_32_ISCSI, '1234', false),
    CrcFast\hash(CrcFast\CRC_32_ISCSI, '56789', false),
    5,
    false
);

var_dump($combined);

// digest CRC-32
$first = new CrcFast\Digest(CrcFast\CRC_32_ISCSI);
$first->update('1234');

$first->finalize(false);

$second = new CrcFast\Digest(CrcFast\CRC_32_ISCSI);
$second->update('56789');

$first->combine($second);

var_dump($first->finalize(false));


// checksum CRC-64, binary inputs
$combined = CrcFast\combine(
    CrcFast\CRC_64_NVME,
    CrcFast\hash(CrcFast\CRC_64_NVME, '1234', true),
    CrcFast\hash(CrcFast\CRC_64_NVME, '56789', true),
    5,
    false
);

var_dump($combined);

// checksum CRC-64, hex inputs
$combined = CrcFast\combine(
    CrcFast\CRC_64_NVME,
    CrcFast\hash(CrcFast\CRC_64_NVME, '1234', false),
    CrcFast\hash(CrcFast\CRC_64_NVME, '56789', false),
    5,
    false
);

var_dump($combined);

// digest CRC-64
$first = new CrcFast\Digest(CrcFast\CRC_64_NVME);
$first->update('1234');

$second = new CrcFast\Digest(CrcFast\CRC_64_NVME);
$second->update('56789');

$first->combine($second);

var_dump($first->finalize(false));
?>
--EXPECT--
string(8) "e3069283"
string(8) "e3069283"
string(8) "e3069283"
string(16) "ae8b14860a799888"
string(16) "ae8b14860a799888"
string(16) "ae8b14860a799888"
