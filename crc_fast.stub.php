<?php

/**
 * @copyright 2025 Don MacAskill
 * @license MIT or Apache 2.0
 *
 * @generate-class-entries
 * @undocumentable
 */

namespace CrcFast {

    /** @var int */
    const CRC_32_AIXM = 10000;

    /** @var int */
    const CRC_32_AUTOSAR = 10010;

    /** @var int */
    const CRC_32_BASE_91_D = 10020;

    /** @var int */
    const CRC_32_BZIP2 = 10030;

    /** @var int */
    const CRC_32_CD_ROM_EDC = 10040;

    /** @var int */
    const CRC_32_CKSUM = 10050;

    /** @var int */
    const CRC_32_ISCSI = 10060;

    /** @var int */
    const CRC_32_ISO_HDLC = 10070;

    /** @var int */
    const CRC_32_JAMCRC = 10080;

    /** @var int */
    const CRC_32_MEF = 10090;

    /** @var int */
    const CRC_32_MPEG_2 = 10100;

    /**
     * CRC-32/PHP is a special flower, in that it's actually CRC-32/BZIP2 then byte-reversed.
     *
     * It matches the output of `hash('crc32')` in PHP (but not 'crc32()').
     *
     * @var int
     */
    const CRC_32_PHP = 10200;

    /** @var int */
    const CRC_32_XFER = 10300;

    /** @var int */
    const CRC_64_ECMA_182 = 20000;

    /** @var int */
    const CRC_64_GO_ISO = 20010;

    /** @var int */
    const CRC_64_MS = 20020;

    /** @var int */
    const CRC_64_NVME = 20030;

    /** @var int */
    const CRC_64_REDIS = 20040;

    /** @var int */
    const CRC_64_WE = 20050;

    /** @var int */
    const CRC_64_XZ = 20060;

    /**
     * Calculates the CRC checksum of the given data.
     *
     * @param int    $algorithm
     * @param string $data
     * @param bool   $binary Output binary string or hex?
     *
     * @return string
     */
    function hash(int $algorithm, string $data, bool $binary = false): string
    {
    }

    /**
     * Calculates the CRC checksum of the given file.
     *
     * @param int      $algorithm
     * @param string   $data
     * @param bool     $binary Output binary string or hex?
     * @param null|int $chunkSize
     *
     * @return string
     */
    function hash_file(int $algorithm, string $data, bool $binary = false, ?int $chunkSize = null): string
    {
    }

    /**
     * Gets a list of supported algorithms.
     *
     * @return array<string, int>
     */
    function get_supported_algorithms(): array
    {
    }

    /**
     * Combines two CRC checksums into one.
     *
     * @param int    $algorithm
     * @param string $checksum1 The first checksum to combine (binary or hex)
     * @param string $checksum2 The second checksum to combine (binary or hex)
     * @param int    $length2   The length of the _input_ to the second checksum
     * @param bool   $binary    Output binary string or hex?
     *
     * @return string
     */
    function combine(
        int $algorithm,
        string $checksum1,
        string $checksum2,
        int $length2,
        bool $binary = false
    ): string {
    }

    /**
     * Calculates the CRC-32/ISO-HDLC checksum of the given data.
     *
     * @param string $data
     *
     * @return int
     */
    function crc32(string $data): int
    {
    }

    /**
     * Digest class for calculating CRC checksums.
     */
    class Digest
    {
        /**
         * @param int $algorithm
         */
        public function __construct(int $algorithm)
        {
        }

        /**
         * Updates the checksum state with the given data.
         *
         * @param string $data
         *
         * @return Digest
         */
        public function update(string $data): Digest
        {
        }

        /**
         * Returns the computed checksum.
         *
         * @param bool $binary Output binary string or hex?
         *
         * @return string
         */
        public function finalize(bool $binary = false): string
        {
        }

        /**
         * Returns  the computed checksum, plus resets the internal state.
         *
         * @param bool $binary Output binary string or hex?
         *
         * @return string
         */
        public function finalizeReset(bool $binary = false): string
        {
        }

        /**
         * Resets the internal state of the checksum.
         *
         * @return Digest
         */
        public function reset(): Digest
        {
        }

        /**
         * Combines another digest into this one.
         *
         * @param Digest $digest
         *
         * @return Digest
         */
        public function combine(Digest $digest): Digest
        {
        }
    }
}
