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
     * Custom CRC parameters class for defining custom CRC algorithms.
     */
    class Params
    {
        /**
         * @param int        $width   CRC width (32 or 64 bits)
         * @param int        $poly    CRC polynomial
         * @param int        $init    Initial CRC value
         * @param bool       $refin   Reflect input bytes
         * @param bool       $refout  Reflect output CRC
         * @param int        $xorout  XOR output with this value
         * @param int        $check   Expected CRC of "123456789"
         * @param array|null $keys    Optional pre-computed keys array (23 elements)
         */
        public function __construct(
            int $width,
            int $poly,
            int $init,
            bool $refin,
            bool $refout,
            int $xorout,
            int $check,
            ?array $keys = null
        ) {
        }

        /**
         * Gets the CRC width.
         *
         * @return int
         */
        public function getWidth(): int
        {
        }

        /**
         * Gets the CRC polynomial.
         *
         * @return int
         */
        public function getPoly(): int
        {
        }

        /**
         * Gets the initial CRC value.
         *
         * @return int
         */
        public function getInit(): int
        {
        }

        /**
         * Gets whether input bytes are reflected.
         *
         * @return bool
         */
        public function getRefin(): bool
        {
        }

        /**
         * Gets whether output CRC is reflected.
         *
         * @return bool
         */
        public function getRefout(): bool
        {
        }

        /**
         * Gets the XOR output value.
         *
         * @return int
         */
        public function getXorout(): int
        {
        }

        /**
         * Gets the expected CRC check value.
         *
         * @return int
         */
        public function getCheck(): int
        {
        }

        /**
         * Gets the pre-computed keys array.
         *
         * @return array
         */
        public function getKeys(): array
        {
        }
    }

    /**
     * Calculates the CRC checksum of the given data.
     *
     * @param int|Params $algorithm
     * @param string     $data
     * @param bool       $binary Output binary string or hex?
     *
     * @return string
     */
    function hash(int|Params $algorithm, string $data, bool $binary = false): string
    {
    }

    /**
     * Calculates the CRC checksum of the given file.
     *
     * @param int|Params $algorithm
     * @param string     $filename
     * @param bool       $binary Output binary string or hex?
     *
     * @return string
     */
    function hash_file(int|Params $algorithm, string $filename, bool $binary = false): string
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
     * @param int|Params $algorithm
     * @param string     $checksum1 The first checksum to combine (binary or hex)
     * @param string     $checksum2 The second checksum to combine (binary or hex)
     * @param int        $length2   The length of the _input_ to the second checksum
     * @param bool       $binary    Output binary string or hex?
     *
     * @return string
     */
    function combine(
        int|Params $algorithm,
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
         * @param int|Params $algorithm
         */
        public function __construct(int|Params $algorithm)
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
