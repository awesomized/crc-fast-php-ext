/* crc_fast extension for PHP */
/* Copyright 2025 Don MacAskill. Licensed under MIT or Apache-2.0. */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

extern "C" {
#include "php.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
}

#include "php_crc_fast.h"
#include "crc_fast_arginfo.h"
#include <string>

#ifndef htonll
#if __BYTE_ORDER == __LITTLE_ENDIAN
uint64_t htonll(uint64_t x) {
    return (((uint64_t)htonl(x & 0xFFFFFFFF)) << 32) | htonl(x >> 32);
}
#define ntohll htonll
#else
#define htonll(x) (x)
#define ntohll(x) (x)
#endif
#endif

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/* CrcFast\Digest class */
zend_class_entry *php_crc_fast_digest_ce;
static zend_object_handlers php_crc_fast_digest_object_handlers;

/* Free the Digest object */
static void php_crc_fast_digest_free_obj(zend_object *object)
{
    php_crc_fast_digest_obj *obj = php_crc_fast_digest_from_obj(object);

    if (obj->digest) {
        crc_fast_digest_free(obj->digest);
        obj->digest = NULL;
    }

    zend_object_std_dtor(&obj->std);
}

/* Create a new Digest object */
static zend_object *php_crc_fast_digest_create_object(zend_class_entry *ce)
{
    php_crc_fast_digest_obj *obj = (php_crc_fast_digest_obj*)ecalloc(1, sizeof(php_crc_fast_digest_obj) + zend_object_properties_size(ce));

    zend_object_std_init(&obj->std, ce);
    object_properties_init(&obj->std, ce);

    obj->std.handlers = &php_crc_fast_digest_object_handlers;
    obj->digest = NULL;
    obj->algorithm = 0; // Explicitly initialize to 0 to prevent garbage values

    return &obj->std;
}

/* Helper function to format checksum output */
static inline void php_crc_fast_format_result(INTERNAL_FUNCTION_PARAMETERS, zend_long algorithm, uint64_t result, zend_bool binary)
{
    if (binary) {
        // For binary output, return the raw bytes
        size_t result_size;

        // Determine if this is a 32-bit or 64-bit algorithm
        if (algorithm <= PHP_CRC_FAST_CRC32_XFER) {
            // 32-bit CRC
            uint32_t result32 = (uint32_t)result;
            result32 = htonl(result32);
            RETURN_STRINGL((char*)&result32, sizeof(result32));
        } else {
            // 64-bit CRC
            result = htonll(result);
            RETURN_STRINGL((char*)&result, sizeof(result));
        }
    } else {
        if (algorithm <= PHP_CRC_FAST_CRC32_XFER) {
            // 32-bit CRC
            char checksum_str[9]; // 8 hex digits + null terminator
            snprintf(checksum_str, sizeof(checksum_str), "%08x", (uint32_t)result);
            RETURN_STRING(checksum_str);
        } else {
            // 64-bit CRC
            char checksum_str[17]; // 16 hex digits + null terminator
            snprintf(checksum_str, sizeof(checksum_str), "%016" PRIx64, result);
            RETURN_STRING(checksum_str);
        }
    }
}

/* Helper function to convert PHP algorithm int to FFICrcAlgorithm enum */
static inline CrcFastAlgorithm php_crc_fast_get_algorithm(zend_long algo) {
    switch (algo) {
        case PHP_CRC_FAST_CRC32_AIXM:      return CrcFastAlgorithm::Crc32Aixm;
        case PHP_CRC_FAST_CRC32_AUTOSAR:   return CrcFastAlgorithm::Crc32Autosar;
        case PHP_CRC_FAST_CRC32_BASE91D:   return CrcFastAlgorithm::Crc32Base91D;
        case PHP_CRC_FAST_CRC32_BZIP2:     return CrcFastAlgorithm::Crc32Bzip2;
        case PHP_CRC_FAST_CRC32_CDROM_EDC: return CrcFastAlgorithm::Crc32CdRomEdc;
        case PHP_CRC_FAST_CRC32_CKSUM:     return CrcFastAlgorithm::Crc32Cksum;
        case PHP_CRC_FAST_CRC32_ISCSI:     return CrcFastAlgorithm::Crc32Iscsi;
        case PHP_CRC_FAST_CRC32_ISO_HDLC:  return CrcFastAlgorithm::Crc32IsoHdlc;
        case PHP_CRC_FAST_CRC32_JAMCRC:    return CrcFastAlgorithm::Crc32Jamcrc;
        case PHP_CRC_FAST_CRC32_MEF:       return CrcFastAlgorithm::Crc32Mef;
        case PHP_CRC_FAST_CRC32_MPEG2:     return CrcFastAlgorithm::Crc32Mpeg2;
        case PHP_CRC_FAST_CRC32_XFER:      return CrcFastAlgorithm::Crc32Xfer;
        case PHP_CRC_FAST_CRC64_ECMA182:   return CrcFastAlgorithm::Crc64Ecma182;
        case PHP_CRC_FAST_CRC64_GO_ISO:    return CrcFastAlgorithm::Crc64GoIso;
        case PHP_CRC_FAST_CRC64_MS:        return CrcFastAlgorithm::Crc64Ms;
        case PHP_CRC_FAST_CRC64_NVME:      return CrcFastAlgorithm::Crc64Nvme;
        case PHP_CRC_FAST_CRC64_REDIS:     return CrcFastAlgorithm::Crc64Redis;
        case PHP_CRC_FAST_CRC64_WE:        return CrcFastAlgorithm::Crc64We;
        case PHP_CRC_FAST_CRC64_XZ:        return CrcFastAlgorithm::Crc64Xz;

        // this is a special flower, just to handle PHP's `hash('crc32')` operation,
        // which is actually byte-reversed CRC-32/BZIP2
        case PHP_CRC_FAST_CRC32_PHP:       return CrcFastAlgorithm::Crc32Bzip2;

        default:
            zend_throw_exception(zend_ce_exception, "Invalid algorithm specified", 0);
    }
}

/* Helper function to reverse bytes for CRC-32/PHP compatibility which matches PHP's `hash('crc32')` output */
static inline uint64_t php_crc_fast_reverse_bytes_if_needed(uint64_t result, zend_long algorithm)
{
    // Only reverse bytes for PHP_CRC_FAST_CRC32_PHP
    if (algorithm == PHP_CRC_FAST_CRC32_PHP) {
        // For CRC32, we only need to reverse the lower 32 bits
        uint32_t result32 = (uint32_t)result;
        result32 = ((result32 & 0xFF) << 24) |
                  ((result32 & 0xFF00) << 8) |
                  ((result32 & 0xFF0000) >> 8) |
                  ((result32 & 0xFF000000) >> 24);
        return result32;
    }

    return result;
}

/* {{{ CrcFast\crc32(string $data): int */
PHP_FUNCTION(CrcFast_crc32)
{
    char *data;
    size_t data_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(data, data_len)
    ZEND_PARSE_PARAMETERS_END();

    uint64_t result = crc_fast_checksum(CrcFastAlgorithm::Crc32IsoHdlc, data, data_len);

    // Return as integer
    RETURN_LONG((zend_long)result);
}
/* }}} */

/* {{{ CrcFast\hash(int $algorithm, string $data, bool $binary = false): string */
PHP_FUNCTION(CrcFast_hash)
{
    zend_long algorithm;
    char *data;
    size_t data_len;
    zend_bool binary = 0;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_LONG(algorithm)
        Z_PARAM_STRING(data, data_len)
        Z_PARAM_BOOL(binary)
    ZEND_PARSE_PARAMETERS_END();

    CrcFastAlgorithm algo = php_crc_fast_get_algorithm(algorithm);
    uint64_t result = crc_fast_checksum(algo, data, data_len);

    // Apply byte reversal if needed
    result = php_crc_fast_reverse_bytes_if_needed(result, algorithm);

    php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, algorithm, result, binary);
}
/* }}} */

/* {{{ CrcFast\hash_file(int $algorithm, string $filename, bool $binary = false, ?int $chunk_size = null): string */
PHP_FUNCTION(CrcFast_hash_file)
{
    zend_long algorithm;
    char *filename;
    size_t filename_len;
    zend_bool binary = 0;
    zval *chunk_size_zval = NULL;

    ZEND_PARSE_PARAMETERS_START(3, 4)
        Z_PARAM_LONG(algorithm)
        Z_PARAM_STRING(filename, filename_len)
        Z_PARAM_BOOL(binary)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL_OR_NULL(chunk_size_zval)
    ZEND_PARSE_PARAMETERS_END();

    CrcFastAlgorithm algo = php_crc_fast_get_algorithm(algorithm);

    uint64_t result = crc_fast_checksum_file(algo, (const uint8_t*)filename, filename_len);

    // Apply byte reversal if needed
    result = php_crc_fast_reverse_bytes_if_needed(result, algorithm);

    php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, algorithm, result, binary);
}
/* }}} */

/* {{{ CrcFast\get_supported_algorithms(): array */
PHP_FUNCTION(CrcFast_get_supported_algorithms)
{
    ZEND_PARSE_PARAMETERS_NONE();

    array_init(return_value);

    add_assoc_long(return_value, "CRC-32/AIXM", PHP_CRC_FAST_CRC32_AIXM);
    add_assoc_long(return_value, "CRC32_AUTOSAR", PHP_CRC_FAST_CRC32_AUTOSAR);
    add_assoc_long(return_value, "CRC-32/BASE-91-D", PHP_CRC_FAST_CRC32_BASE91D);
    add_assoc_long(return_value, "CRC-32/BZIP2", PHP_CRC_FAST_CRC32_BZIP2);
    add_assoc_long(return_value, "CRC-32/CD-ROM-EDC", PHP_CRC_FAST_CRC32_CDROM_EDC);
    add_assoc_long(return_value, "CRC-32/CKSUM", PHP_CRC_FAST_CRC32_CKSUM);
    add_assoc_long(return_value, "CRC-32/ISCSI", PHP_CRC_FAST_CRC32_ISCSI);
    add_assoc_long(return_value, "CRC-32/ISO_HDLC", PHP_CRC_FAST_CRC32_ISO_HDLC);
    add_assoc_long(return_value, "CRC-32/JAMCRC", PHP_CRC_FAST_CRC32_JAMCRC);
    add_assoc_long(return_value, "CRC-32/MEF", PHP_CRC_FAST_CRC32_MEF);
    add_assoc_long(return_value, "CRC-32/MPEG-2", PHP_CRC_FAST_CRC32_MPEG2);
    add_assoc_long(return_value, "CRC-32/XFER", PHP_CRC_FAST_CRC32_XFER);
    add_assoc_long(return_value, "CRC-64/ECMA-182", PHP_CRC_FAST_CRC64_ECMA182);
    add_assoc_long(return_value, "CRC-64/GO-ISO", PHP_CRC_FAST_CRC64_GO_ISO);
    add_assoc_long(return_value, "CRC-64/MS", PHP_CRC_FAST_CRC64_MS);
    add_assoc_long(return_value, "CRC-64/NVME", PHP_CRC_FAST_CRC64_NVME);
    add_assoc_long(return_value, "CRC-64/REDIS", PHP_CRC_FAST_CRC64_REDIS);
    add_assoc_long(return_value, "CRC-64/WE", PHP_CRC_FAST_CRC64_WE);
    add_assoc_long(return_value, "CRC-64/XZ", PHP_CRC_FAST_CRC64_XZ);
}
/* }}} */

/* {{{ CrcFast\combine(int $algorithm, string $checksum1, string $checksum2, int $length2, bool $binary = false): string */
PHP_FUNCTION(CrcFast_combine)
{
    zend_long algorithm;
    char *checksum1, *checksum2;
    size_t checksum1_len, checksum2_len;
    zend_long length2;
    zend_bool binary = 0;

    ZEND_PARSE_PARAMETERS_START(5, 5)
        Z_PARAM_LONG(algorithm)
        Z_PARAM_STRING(checksum1, checksum1_len)
        Z_PARAM_STRING(checksum2, checksum2_len)
        Z_PARAM_LONG(length2)
        Z_PARAM_BOOL(binary)
    ZEND_PARSE_PARAMETERS_END();

    CrcFastAlgorithm algo = php_crc_fast_get_algorithm(algorithm);

    uint64_t cs1 = 0, cs2 = 0;

    // Determine if we're dealing with 32-bit or 64-bit CRC
    bool is_crc32 = (algorithm <= PHP_CRC_FAST_CRC32_XFER);
    size_t expected_binary_size = is_crc32 ? 4 : 8;  // 4 bytes for CRC32, 8 bytes for CRC64
    size_t expected_hex_size = is_crc32 ? 8 : 16;    // 8 hex chars for CRC32, 16 for CRC64

    // Process checksum1
    if (checksum1_len == expected_binary_size) {
        // Binary input
        if (is_crc32) {
            cs1 = ntohl(*((uint32_t*)checksum1));
        } else {
            cs1 = ntohll(*((uint64_t*)checksum1));
        }
    } else if (checksum1_len == expected_hex_size) {
        // Hex input
        if (is_crc32) {
            uint32_t value = 0;
            if (sscanf(checksum1, "%8x", &value) == 1) {
                cs1 = value;
            } else {
                php_error_docref(NULL, E_WARNING, "Invalid hexadecimal input for checksum1");
                RETURN_FALSE;
            }
        } else {
            // For 64-bit values, we need to parse it manually because sscanf might not
            // handle uint64_t correctly on all platforms
            cs1 = 0;
            for (size_t i = 0; i < 16; i++) {
                char c = checksum1[i];
                uint64_t digit;

                if (c >= '0' && c <= '9') {
                    digit = c - '0';
                } else if (c >= 'a' && c <= 'f') {
                    digit = c - 'a' + 10;
                } else if (c >= 'A' && c <= 'F') {
                    digit = c - 'A' + 10;
                } else {
                    php_error_docref(NULL, E_WARNING, "Invalid hexadecimal character in checksum1");
                    RETURN_FALSE;
                }

                cs1 = (cs1 << 4) | digit;
            }
        }
    } else {
        php_error_docref(NULL, E_WARNING, "Invalid checksum1 length (expected %zu bytes for binary or %zu chars for hex)",
                         expected_binary_size, expected_hex_size);
        RETURN_FALSE;
    }

    // Process checksum2
    if (checksum2_len == expected_binary_size) {
        // Binary input
        if (is_crc32) {
            cs2 = ntohl(*((uint32_t*)checksum2));
        } else {
            cs2 = ntohll(*((uint64_t*)checksum2));
        }
    } else if (checksum2_len == expected_hex_size) {
        // Hex input
        if (is_crc32) {
            uint32_t value = 0;
            if (sscanf(checksum2, "%8x", &value) == 1) {
                cs2 = value;
            } else {
                php_error_docref(NULL, E_WARNING, "Invalid hexadecimal input for checksum2");
                RETURN_FALSE;
            }
        } else {
            // For 64-bit values, parse it manually
            cs2 = 0;
            for (size_t i = 0; i < 16; i++) {
                char c = checksum2[i];
                uint64_t digit;

                if (c >= '0' && c <= '9') {
                    digit = c - '0';
                } else if (c >= 'a' && c <= 'f') {
                    digit = c - 'a' + 10;
                } else if (c >= 'A' && c <= 'F') {
                    digit = c - 'A' + 10;
                } else {
                    php_error_docref(NULL, E_WARNING, "Invalid hexadecimal character in checksum2");
                    RETURN_FALSE;
                }

                cs2 = (cs2 << 4) | digit;
            }
        }
    } else {
        php_error_docref(NULL, E_WARNING, "Invalid checksum2 length (expected %zu bytes for binary or %zu chars for hex)",
                         expected_binary_size, expected_hex_size);
        RETURN_FALSE;
    }

    uint64_t result = crc_fast_checksum_combine(algo, cs1, cs2, length2);

    // Apply byte reversal if needed
    result = php_crc_fast_reverse_bytes_if_needed(result, algorithm);

    php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, algorithm, result, binary);
}
/* }}} */

/* {{{ CrcFast\Digest::__construct(int $algorithm) */
PHP_METHOD(CrcFast_Digest, __construct)
{
    php_crc_fast_digest_obj *obj = Z_CRC_FAST_DIGEST_P(getThis());
    zend_long algorithm;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(algorithm)
    ZEND_PARSE_PARAMETERS_END();

    CrcFastAlgorithm algo = php_crc_fast_get_algorithm(algorithm);

    // Free previous digest if it exists
    if (obj->digest) {
        crc_fast_digest_free(obj->digest);
    }

    obj->digest = crc_fast_digest_new(algo);
    obj->algorithm = algorithm;  // Store algorithm ID explicitly

    if (!obj->digest) {
        zend_throw_exception(zend_ce_exception, "Failed to create digest", 0);
    }
}
/* }}} */

/* {{{ CrcFast\Digest::update(string $data): void */
PHP_METHOD(CrcFast_Digest, update)
{
    php_crc_fast_digest_obj *obj = Z_CRC_FAST_DIGEST_P(getThis());
    char *data;
    size_t data_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(data, data_len)
    ZEND_PARSE_PARAMETERS_END();

    if (!obj->digest) {
        zend_throw_exception(zend_ce_exception, "Digest object not initialized", 0);
        return;
    }

    crc_fast_digest_update(obj->digest, data, data_len);

    // Return $this for method chaining
    RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/* {{{ CrcFast\Digest::finalize(bool $binary = false): string */
PHP_METHOD(CrcFast_Digest, finalize)
{
    php_crc_fast_digest_obj *obj = Z_CRC_FAST_DIGEST_P(getThis());
    zend_bool binary = 0;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(binary)
    ZEND_PARSE_PARAMETERS_END();

    if (!obj->digest) {
        zend_throw_exception(zend_ce_exception, "Digest object not initialized", 0);
        return;
    }

    uint64_t result = crc_fast_digest_finalize(obj->digest);

    // Apply byte reversal if needed
    result = php_crc_fast_reverse_bytes_if_needed(result, obj->algorithm);

    // Format and return the result
    php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, obj->algorithm, result, binary);
}
/* }}} */

/* {{{ CrcFast\Digest::reset(): void */
PHP_METHOD(CrcFast_Digest, reset)
{
    php_crc_fast_digest_obj *obj = Z_CRC_FAST_DIGEST_P(getThis());

    ZEND_PARSE_PARAMETERS_NONE();

    if (!obj->digest) {
        zend_throw_exception(zend_ce_exception, "Digest object not initialized", 0);
        return;
    }

    crc_fast_digest_reset(obj->digest);

    // Return $this for method chaining
    RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/* {{{ CrcFast\Digest::finalizeReset(bool $binary = false): string */
PHP_METHOD(CrcFast_Digest, finalizeReset)
{
    php_crc_fast_digest_obj *obj = Z_CRC_FAST_DIGEST_P(getThis());
    zend_bool binary = 0;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(binary)
    ZEND_PARSE_PARAMETERS_END();

    if (!obj->digest) {
        zend_throw_exception(zend_ce_exception, "Digest object not initialized", 0);
        return;
    }

    uint64_t result = crc_fast_digest_finalize_reset(obj->digest);

    // Apply byte reversal if needed
    result = php_crc_fast_reverse_bytes_if_needed(result, obj->algorithm);

    php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, obj->algorithm, result, binary);
}
/* }}} */

/* {{{ CrcFast\Digest::combine(CrcFast\Digest $other): void */
PHP_METHOD(CrcFast_Digest, combine)
{
    php_crc_fast_digest_obj *obj = Z_CRC_FAST_DIGEST_P(getThis());
    zval *other_zval;
    php_crc_fast_digest_obj *other_obj;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(other_zval, php_crc_fast_digest_ce)
    ZEND_PARSE_PARAMETERS_END();

    if (!obj->digest) {
        zend_throw_exception(zend_ce_exception, "Digest object not initialized", 0);
        return;
    }

    other_obj = Z_CRC_FAST_DIGEST_P(other_zval);
    if (!other_obj->digest) {
        zend_throw_exception(zend_ce_exception, "Other digest object not initialized", 0);
        return;
    }

    crc_fast_digest_combine(obj->digest, other_obj->digest);

    // Return $this for method chaining
    RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(crc_fast)
{
#if defined(ZTS) && defined(COMPILE_DL_CRC_FAST)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(crc_fast)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "crc_fast support", "enabled");
	php_info_print_table_row(2, "crc_fast extension version", PHP_CRC_FAST_VERSION);
	php_info_print_table_row(2, "crc_fast library version", crc_fast_get_version());
	php_info_print_table_row(2, "crc_fast CRC-32/ISCSI target", crc_fast_get_calculator_target(CrcFastAlgorithm::Crc32Iscsi));
	php_info_print_table_row(2, "crc_fast CRC-32/ISO-HDLC target", crc_fast_get_calculator_target(CrcFastAlgorithm::Crc32IsoHdlc));
	php_info_print_table_row(2, "crc_fast all other targets", crc_fast_get_calculator_target(CrcFastAlgorithm::Crc64Nvme));
	php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(crc_fast)
{
// Register constants and symbols
    register_crc_fast_symbols(0);

    // Register the Digest class using the auto-generated function
    php_crc_fast_digest_ce = register_class_CrcFast_Digest();

    // Set up the create_object handler for the class
    php_crc_fast_digest_ce->create_object = php_crc_fast_digest_create_object;

    // Initialize the object handlers
    memcpy(&php_crc_fast_digest_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    php_crc_fast_digest_object_handlers.offset = XtOffsetOf(php_crc_fast_digest_obj, std);
    php_crc_fast_digest_object_handlers.free_obj = php_crc_fast_digest_free_obj;
    php_crc_fast_digest_object_handlers.clone_obj = NULL; // No cloning support

    return SUCCESS;
}

/* {{{ crc_fast_module_entry */
zend_module_entry crc_fast_module_entry = {
	STANDARD_MODULE_HEADER,
	"crc_fast",					/* Extension name */
	ext_functions,			    /* zend_function_entry */
	PHP_MINIT(crc_fast),		/* PHP_MINIT - Module initialization */
	NULL,						/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(crc_fast),		/* PHP_RINIT - Request initialization */
	NULL,						/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(crc_fast),		/* PHP_MINFO - Module info */
	PHP_CRC_FAST_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CRC_FAST
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(crc_fast)
#endif
