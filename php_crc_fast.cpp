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

// Define htonll/ntohll for platforms that don't provide them
#if defined(_WIN32) || defined(_WIN64)
    // Windows doesn't provide htonll/ntohll, so we define them here
    // Windows is always little-endian on x86/x64/ARM
    #ifndef htonll
        inline uint64_t htonll(uint64_t x) {
            return (((uint64_t)htonl((uint32_t)(x & 0xFFFFFFFF))) << 32) | htonl((uint32_t)(x >> 32));
        }
        inline uint64_t ntohll(uint64_t x) {
            return (((uint64_t)ntohl((uint32_t)(x & 0xFFFFFFFF))) << 32) | ntohl((uint32_t)(x >> 32));
        }
    #endif
#else
    // Unix-like platforms
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

/* CrcFast\Params class */
zend_class_entry *php_crc_fast_params_ce;
static zend_object_handlers php_crc_fast_params_object_handlers;

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
    obj->is_custom = false; // Initialize to false
    memset(&obj->custom_params, 0, sizeof(CrcFastParams)); // Initialize custom params to zero

    return &obj->std;
}

/* Free the Params object */
static void php_crc_fast_params_free_obj(zend_object *object)
{
    php_crc_fast_params_obj *obj = php_crc_fast_params_from_obj(object);

    // Free the allocated keys storage if it exists
    if (obj->keys_storage) {
        efree(obj->keys_storage);
        obj->keys_storage = NULL;
    }

    zend_object_std_dtor(&obj->std);
}

/* Create a new Params object */
static zend_object *php_crc_fast_params_create_object(zend_class_entry *ce)
{
    php_crc_fast_params_obj *obj = (php_crc_fast_params_obj*)ecalloc(1, sizeof(php_crc_fast_params_obj) + zend_object_properties_size(ce));

    zend_object_std_init(&obj->std, ce);
    object_properties_init(&obj->std, ce);

    obj->std.handlers = &php_crc_fast_params_object_handlers;
    
    // Initialize the CrcFastParams struct to zero
    memset(&obj->params, 0, sizeof(CrcFastParams));
    obj->keys_storage = NULL;

    return &obj->std;
}

/* Helper function to format checksum output */
static inline void php_crc_fast_format_result(INTERNAL_FUNCTION_PARAMETERS, zend_long algorithm, uint64_t result, zend_bool binary, bool is_custom = false, uint8_t custom_width = 0)
{
    bool is_32bit;
    
    if (is_custom) {
        // For custom parameters, use the width from the parameters
        is_32bit = (custom_width == 32);
    } else {
        // For predefined algorithms, determine width based on algorithm constant
        is_32bit = (algorithm <= PHP_CRC_FAST_CRC32_XFER);
    }

    if (binary) {
        // For binary output, return the raw bytes
        if (is_32bit) {
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
        if (is_32bit) {
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
            zend_throw_exception_ex(zend_ce_exception, 0,
                "Invalid algorithm constant %lld. Use CrcFast\\get_supported_algorithms() to see valid values", algo);
            return CrcFastAlgorithm::Crc32IsoHdlc; // Fallback (never reached due to exception)
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

/* Helper function to detect parameter type and extract CrcFastParams if needed */
static inline bool php_crc_fast_get_params_from_zval(zval *algorithm_zval, zend_long *algorithm_out, CrcFastParams *params_out)
{
    if (Z_TYPE_P(algorithm_zval) == IS_LONG) {
        // It's an integer algorithm constant
        *algorithm_out = Z_LVAL_P(algorithm_zval);
        return false; // Not custom parameters
    } else if (Z_TYPE_P(algorithm_zval) == IS_OBJECT && 
               instanceof_function(Z_OBJCE_P(algorithm_zval), php_crc_fast_params_ce)) {
        // It's a CrcFast\Params object
        php_crc_fast_params_obj *params_obj = Z_CRC_FAST_PARAMS_P(algorithm_zval);
        if (!params_obj) {
            zend_throw_exception(zend_ce_exception, "Invalid CrcFast\\Params object", 0);
            return false;
        }
        *params_out = params_obj->params;
        *algorithm_out = 0; // Not used for custom parameters
        return true; // Custom parameters
    } else {
        // Invalid type - provide more descriptive error message
        const char *type_name = zend_get_type_by_const(Z_TYPE_P(algorithm_zval));
        zend_throw_exception_ex(zend_ce_exception, 0, 
            "Algorithm parameter must be an integer constant or CrcFast\\Params object, %s given", type_name);
        return false;
    }
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

/* {{{ CrcFast\hash(int|CrcFast\Params $algorithm, string $data, bool $binary = false): string */
PHP_FUNCTION(CrcFast_hash)
{
    zval *algorithm_zval;
    char *data;
    size_t data_len;
    zend_bool binary = 0;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_ZVAL(algorithm_zval)
        Z_PARAM_STRING(data, data_len)
        Z_PARAM_BOOL(binary)
    ZEND_PARSE_PARAMETERS_END();

    // Validate data parameter
    if (!data) {
        zend_throw_exception(zend_ce_exception, "Data parameter cannot be null", 0);
        return;
    }

    zend_long algorithm;
    CrcFastParams custom_params;
    bool is_custom = php_crc_fast_get_params_from_zval(algorithm_zval, &algorithm, &custom_params);
    
    if (EG(exception)) {
        return; // Exception was thrown by helper function
    }

    uint64_t result;
    if (is_custom) {
        // Use custom parameters - handle potential C library errors
        try {
            result = crc_fast_checksum_with_params(custom_params, data, data_len);
        } catch (...) {
            zend_throw_exception(zend_ce_exception, "Failed to compute CRC checksum with custom parameters", 0);
            return;
        }
        php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, result, binary, true, custom_params.width);
    } else {
        // Use predefined algorithm - handle potential C library errors
        CrcFastAlgorithm algo = php_crc_fast_get_algorithm(algorithm);
        if (EG(exception)) {
            return; // Exception was thrown by get_algorithm
        }
        
        try {
            result = crc_fast_checksum(algo, data, data_len);
        } catch (...) {
            zend_throw_exception_ex(zend_ce_exception, 0, 
                "Failed to compute CRC checksum for algorithm %lld", algorithm);
            return;
        }

        // Apply byte reversal if needed
        result = php_crc_fast_reverse_bytes_if_needed(result, algorithm);

        php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, algorithm, result, binary);
    }
}
/* }}} */

/* {{{ CrcFast\hash_file(int|CrcFast\Params $algorithm, string $filename, bool $binary = false, ?int $chunk_size = null): string */
PHP_FUNCTION(CrcFast_hash_file)
{
    zval *algorithm_zval;
    char *filename;
    size_t filename_len;
    zend_bool binary = 0;
    zval *chunk_size_zval = NULL;

    ZEND_PARSE_PARAMETERS_START(3, 4)
        Z_PARAM_ZVAL(algorithm_zval)
        Z_PARAM_STRING(filename, filename_len)
        Z_PARAM_BOOL(binary)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL_OR_NULL(chunk_size_zval)
    ZEND_PARSE_PARAMETERS_END();

    // Validate filename parameter
    if (!filename || filename_len == 0) {
        zend_throw_exception(zend_ce_exception, "Filename cannot be empty", 0);
        return;
    }

    // Check if file exists and is readable
    if (php_check_open_basedir(filename)) {
        zend_throw_exception_ex(zend_ce_exception, 0, "File '%s' is not within the allowed path(s)", filename);
        return;
    }

    zend_long algorithm;
    CrcFastParams custom_params;
    bool is_custom = php_crc_fast_get_params_from_zval(algorithm_zval, &algorithm, &custom_params);
    
    if (EG(exception)) {
        return; // Exception was thrown by helper function
    }

    uint64_t result;
    if (is_custom) {
        // Use custom parameters - handle potential C library errors
        try {
            result = crc_fast_checksum_file_with_params(custom_params, (const uint8_t*)filename, filename_len);
        } catch (...) {
            zend_throw_exception_ex(zend_ce_exception, 0, 
                "Failed to compute CRC checksum for file '%s' with custom parameters", filename);
            return;
        }
        php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, result, binary, true, custom_params.width);
    } else {
        // Use predefined algorithm - handle potential C library errors
        CrcFastAlgorithm algo = php_crc_fast_get_algorithm(algorithm);
        if (EG(exception)) {
            return; // Exception was thrown by get_algorithm
        }
        
        try {
            result = crc_fast_checksum_file(algo, (const uint8_t*)filename, filename_len);
        } catch (...) {
            zend_throw_exception_ex(zend_ce_exception, 0, 
                "Failed to compute CRC checksum for file '%s' with algorithm %lld", filename, algorithm);
            return;
        }

        // Apply byte reversal if needed
        result = php_crc_fast_reverse_bytes_if_needed(result, algorithm);

        php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, algorithm, result, binary);
    }
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

/* {{{ CrcFast\combine(int|CrcFast\Params $algorithm, string $checksum1, string $checksum2, int $length2, bool $binary = false): string */
PHP_FUNCTION(CrcFast_combine)
{
    zval *algorithm_zval;
    char *checksum1, *checksum2;
    size_t checksum1_len, checksum2_len;
    zend_long length2;
    zend_bool binary = 0;

    ZEND_PARSE_PARAMETERS_START(5, 5)
        Z_PARAM_ZVAL(algorithm_zval)
        Z_PARAM_STRING(checksum1, checksum1_len)
        Z_PARAM_STRING(checksum2, checksum2_len)
        Z_PARAM_LONG(length2)
        Z_PARAM_BOOL(binary)
    ZEND_PARSE_PARAMETERS_END();

    // Validate parameters
    if (!checksum1 || !checksum2) {
        zend_throw_exception(zend_ce_exception, "Checksum parameters cannot be null", 0);
        return;
    }

    if (length2 < 0) {
        zend_throw_exception_ex(zend_ce_exception, 0, 
            "Length parameter must be non-negative, got %lld", length2);
        return;
    }

    zend_long algorithm;
    CrcFastParams custom_params;
    bool is_custom = php_crc_fast_get_params_from_zval(algorithm_zval, &algorithm, &custom_params);
    
    if (EG(exception)) {
        return; // Exception was thrown by helper function
    }

    uint64_t cs1 = 0, cs2 = 0;

    // Determine if we're dealing with 32-bit or 64-bit CRC
    bool is_crc32;
    if (is_custom) {
        is_crc32 = (custom_params.width == 32);
    } else {
        is_crc32 = (algorithm <= PHP_CRC_FAST_CRC32_XFER);
    }
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

    uint64_t result;
    if (is_custom) {
        // Use custom parameters - handle potential C library errors
        try {
            result = crc_fast_checksum_combine_with_params(custom_params, cs1, cs2, length2);
        } catch (...) {
            zend_throw_exception(zend_ce_exception, "Failed to combine CRC checksums with custom parameters", 0);
            return;
        }
        php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, result, binary, true, custom_params.width);
    } else {
        // Use predefined algorithm - handle potential C library errors
        CrcFastAlgorithm algo = php_crc_fast_get_algorithm(algorithm);
        if (EG(exception)) {
            return; // Exception was thrown by get_algorithm
        }
        
        try {
            result = crc_fast_checksum_combine(algo, cs1, cs2, length2);
        } catch (...) {
            zend_throw_exception_ex(zend_ce_exception, 0, 
                "Failed to combine CRC checksums for algorithm %lld", algorithm);
            return;
        }

        // Apply byte reversal if needed
        result = php_crc_fast_reverse_bytes_if_needed(result, algorithm);

        php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, algorithm, result, binary);
    }
}
/* }}} */

/* {{{ CrcFast\Digest::__construct(int|CrcFast\Params $algorithm) */
PHP_METHOD(CrcFast_Digest, __construct)
{
    php_crc_fast_digest_obj *obj = Z_CRC_FAST_DIGEST_P(getThis());
    zval *algorithm_zval;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(algorithm_zval)
    ZEND_PARSE_PARAMETERS_END();

    if (!obj) {
        zend_throw_exception(zend_ce_exception, "Failed to initialize Digest object", 0);
        return;
    }

    // Free previous digest if it exists
    if (obj->digest) {
        crc_fast_digest_free(obj->digest);
        obj->digest = NULL;
    }

    zend_long algorithm;
    CrcFastParams custom_params;
    bool is_custom = php_crc_fast_get_params_from_zval(algorithm_zval, &algorithm, &custom_params);
    
    if (EG(exception)) {
        return; // Exception was thrown by helper function
    }

    if (is_custom) {
        // Use custom parameters - handle potential C library errors
        try {
            obj->digest = crc_fast_digest_new_with_params(custom_params);
        } catch (...) {
            zend_throw_exception(zend_ce_exception, "Failed to create digest with custom parameters", 0);
            return;
        }
        
        if (!obj->digest) {
            zend_throw_exception(zend_ce_exception, "C library failed to create digest with custom parameters", 0);
            return;
        }
        
        obj->is_custom = true;
        obj->custom_params = custom_params;
        obj->algorithm = 0; // Not used for custom parameters
    } else {
        // Use predefined algorithm - handle potential C library errors
        CrcFastAlgorithm algo = php_crc_fast_get_algorithm(algorithm);
        if (EG(exception)) {
            return; // Exception was thrown by get_algorithm
        }
        
        try {
            obj->digest = crc_fast_digest_new(algo);
        } catch (...) {
            zend_throw_exception_ex(zend_ce_exception, 0, 
                "Failed to create digest for algorithm %lld", algorithm);
            return;
        }
        
        if (!obj->digest) {
            zend_throw_exception_ex(zend_ce_exception, 0, 
                "C library failed to create digest for algorithm %lld", algorithm);
            return;
        }
        
        obj->is_custom = false;
        obj->algorithm = algorithm;
        // Initialize custom_params to zero for safety
        memset(&obj->custom_params, 0, sizeof(CrcFastParams));
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

    if (!obj) {
        zend_throw_exception(zend_ce_exception, "Invalid Digest object", 0);
        return;
    }

    if (!obj->digest) {
        zend_throw_exception(zend_ce_exception, "Digest object not initialized. Call constructor first", 0);
        return;
    }

    // Validate data parameter
    if (!data) {
        zend_throw_exception(zend_ce_exception, "Data parameter cannot be null", 0);
        return;
    }

    // Handle potential C library errors
    try {
        crc_fast_digest_update(obj->digest, data, data_len);
    } catch (...) {
        zend_throw_exception(zend_ce_exception, "Failed to update digest with data", 0);
        return;
    }

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

    if (!obj) {
        zend_throw_exception(zend_ce_exception, "Invalid Digest object", 0);
        return;
    }

    if (!obj->digest) {
        zend_throw_exception(zend_ce_exception, "Digest object not initialized. Call constructor first", 0);
        return;
    }

    uint64_t result;
    try {
        result = crc_fast_digest_finalize(obj->digest);
    } catch (...) {
        zend_throw_exception(zend_ce_exception, "Failed to finalize digest", 0);
        return;
    }

    if (obj->is_custom) {
        // Use custom parameter formatting
        php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, result, binary, true, obj->custom_params.width);
    } else {
        // Apply byte reversal if needed for predefined algorithms
        result = php_crc_fast_reverse_bytes_if_needed(result, obj->algorithm);
        // Format and return the result using predefined algorithm formatting
        php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, obj->algorithm, result, binary);
    }
}
/* }}} */

/* {{{ CrcFast\Digest::reset(): void */
PHP_METHOD(CrcFast_Digest, reset)
{
    php_crc_fast_digest_obj *obj = Z_CRC_FAST_DIGEST_P(getThis());

    ZEND_PARSE_PARAMETERS_NONE();

    if (!obj) {
        zend_throw_exception(zend_ce_exception, "Invalid Digest object", 0);
        return;
    }

    if (!obj->digest) {
        zend_throw_exception(zend_ce_exception, "Digest object not initialized. Call constructor first", 0);
        return;
    }

    try {
        crc_fast_digest_reset(obj->digest);
    } catch (...) {
        zend_throw_exception(zend_ce_exception, "Failed to reset digest", 0);
        return;
    }

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

    if (!obj) {
        zend_throw_exception(zend_ce_exception, "Invalid Digest object", 0);
        return;
    }

    if (!obj->digest) {
        zend_throw_exception(zend_ce_exception, "Digest object not initialized. Call constructor first", 0);
        return;
    }

    uint64_t result;
    try {
        result = crc_fast_digest_finalize_reset(obj->digest);
    } catch (...) {
        zend_throw_exception(zend_ce_exception, "Failed to finalize and reset digest", 0);
        return;
    }

    if (obj->is_custom) {
        // Use custom parameter formatting
        php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, result, binary, true, obj->custom_params.width);
    } else {
        // Apply byte reversal if needed for predefined algorithms
        result = php_crc_fast_reverse_bytes_if_needed(result, obj->algorithm);
        php_crc_fast_format_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, obj->algorithm, result, binary);
    }
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

    if (!obj) {
        zend_throw_exception(zend_ce_exception, "Invalid Digest object", 0);
        return;
    }

    if (!obj->digest) {
        zend_throw_exception(zend_ce_exception, "Digest object not initialized. Call constructor first", 0);
        return;
    }

    other_obj = Z_CRC_FAST_DIGEST_P(other_zval);
    if (!other_obj) {
        zend_throw_exception(zend_ce_exception, "Invalid other Digest object", 0);
        return;
    }

    if (!other_obj->digest) {
        zend_throw_exception(zend_ce_exception, "Other digest object not initialized. Call constructor first", 0);
        return;
    }

    try {
        crc_fast_digest_combine(obj->digest, other_obj->digest);
    } catch (...) {
        zend_throw_exception(zend_ce_exception, "Failed to combine digest objects", 0);
        return;
    }

    // Return $this for method chaining
    RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/* {{{ CrcFast\Params::__construct(int $width, int $poly, int $init, bool $refin, bool $refout, int $xorout, int $check, ?array $keys = null) */
PHP_METHOD(CrcFast_Params, __construct)
{
    php_crc_fast_params_obj *obj = Z_CRC_FAST_PARAMS_P(getThis());
    zend_long width, poly, init, xorout, check;
    zend_bool refin, refout;
    zval *keys_array = NULL;

    ZEND_PARSE_PARAMETERS_START(7, 8)
        Z_PARAM_LONG(width)
        Z_PARAM_LONG(poly)
        Z_PARAM_LONG(init)
        Z_PARAM_BOOL(refin)
        Z_PARAM_BOOL(refout)
        Z_PARAM_LONG(xorout)
        Z_PARAM_LONG(check)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY_OR_NULL(keys_array)
    ZEND_PARSE_PARAMETERS_END();

    // Validate width - only 32 and 64 are supported
    if (width != 32 && width != 64) {
        zend_throw_exception_ex(zend_ce_exception, 0, 
            "Invalid width %lld. Only 32 and 64 bit widths are supported", width);
        return;
    }

    // Validate parameters are not negative
    if (poly < 0) {
        zend_throw_exception_ex(zend_ce_exception, 0, 
            "Polynomial value %lld cannot be negative", poly);
        return;
    }

    if (init < 0) {
        zend_throw_exception_ex(zend_ce_exception, 0, 
            "Init value %lld cannot be negative", init);
        return;
    }

    if (xorout < 0) {
        zend_throw_exception_ex(zend_ce_exception, 0, 
            "Xorout value %lld cannot be negative", xorout);
        return;
    }

    if (check < 0) {
        zend_throw_exception_ex(zend_ce_exception, 0, 
            "Check value %lld cannot be negative", check);
        return;
    }

    // Validate polynomial fits within width
    uint64_t max_poly = (width == 32) ? 0xFFFFFFFFULL : 0xFFFFFFFFFFFFFFFFULL;
    if ((uint64_t)poly > max_poly) {
        zend_throw_exception_ex(zend_ce_exception, 0, 
            "Polynomial 0x%llx exceeds maximum value for %lld-bit width", poly, width);
        return;
    }

    // Validate init value fits within width
    uint64_t max_init = (width == 32) ? 0xFFFFFFFFULL : 0xFFFFFFFFFFFFFFFFULL;
    if ((uint64_t)init > max_init) {
        zend_throw_exception_ex(zend_ce_exception, 0, 
            "Init value 0x%llx exceeds maximum value for %lld-bit width", init, width);
        return;
    }

    // Validate xorout value fits within width
    uint64_t max_xorout = (width == 32) ? 0xFFFFFFFFULL : 0xFFFFFFFFFFFFFFFFULL;
    if ((uint64_t)xorout > max_xorout) {
        zend_throw_exception_ex(zend_ce_exception, 0, 
            "Xorout value 0x%llx exceeds maximum value for %lld-bit width", xorout, width);
        return;
    }

    // Validate check value fits within width
    uint64_t max_check = (width == 32) ? 0xFFFFFFFFULL : 0xFFFFFFFFFFFFFFFFULL;
    if ((uint64_t)check > max_check) {
        zend_throw_exception_ex(zend_ce_exception, 0, 
            "Check value 0x%llx exceeds maximum value for %lld-bit width", check, width);
        return;
    }

    // Set up the CrcFastParams struct
    obj->params.algorithm = (width == 32) ? CrcFastAlgorithm::Crc32Custom : CrcFastAlgorithm::Crc64Custom;
    obj->params.width = (uint8_t)width;
    obj->params.poly = (uint64_t)poly;
    obj->params.init = (uint64_t)init;
    obj->params.refin = refin;
    obj->params.refout = refout;
    obj->params.xorout = (uint64_t)xorout;
    obj->params.check = (uint64_t)check;

    // Allocate memory for keys array (23 elements)
    obj->keys_storage = (uint64_t*)emalloc(23 * sizeof(uint64_t));
    obj->params.key_count = 23;
    obj->params.keys = obj->keys_storage;

    // Handle keys parameter
    if (keys_array && Z_TYPE_P(keys_array) == IS_ARRAY) {
        // Validate keys array has exactly 23 elements
        if (zend_hash_num_elements(Z_ARRVAL_P(keys_array)) != 23) {
            zend_throw_exception_ex(zend_ce_exception, 0, 
                "Keys array must contain exactly 23 elements, got %d", 
                zend_hash_num_elements(Z_ARRVAL_P(keys_array)));
            return;
        }

        // Copy keys from PHP array to C array
        zval *key_val;
        int i = 0;
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(keys_array), key_val) {
            if (Z_TYPE_P(key_val) != IS_LONG) {
                zend_throw_exception_ex(zend_ce_exception, 0, 
                    "All keys must be integers, element %d is not an integer", i);
                return;
            }
            
            zend_long key_value = Z_LVAL_P(key_val);
            if (key_value < 0) {
                zend_throw_exception_ex(zend_ce_exception, 0, 
                    "Key values cannot be negative, element %d has value %lld", i, key_value);
                return;
            }
            
            obj->keys_storage[i] = (uint64_t)key_value;
            i++;
        } ZEND_HASH_FOREACH_END();
    } else {
        // Generate keys using the C library helper function - handle potential errors
        CrcFastParams temp_params;
        try {
            temp_params = crc_fast_get_custom_params(
                "", // name is not used for key generation
                (uint8_t)width,
                (uint64_t)poly,
                (uint64_t)init,
                refin,
                (uint64_t)xorout,
                (uint64_t)check
            );
        } catch (...) {
            zend_throw_exception(zend_ce_exception, "Failed to generate keys for custom CRC parameters", 0);
            return;
        }
        
        // Copy the generated keys
        memcpy(obj->keys_storage, temp_params.keys, 23 * sizeof(uint64_t));
    }

    // Validate the parameters by checking if they produce the expected check value
    // This is done by computing CRC of "123456789" and comparing with check parameter
    const char *test_data = "123456789";
    uint64_t computed_check;
    
    try {
        computed_check = crc_fast_checksum_with_params(obj->params, test_data, 9);
    } catch (...) {
        zend_throw_exception(zend_ce_exception, "Failed to validate custom CRC parameters", 0);
        return;
    }
    
    if (computed_check != (uint64_t)check) {
        zend_throw_exception_ex(zend_ce_exception, 0, 
            "Parameters validation failed: computed check 0x%016" PRIx64 " does not match expected check 0x%016" PRIx64 ". "
            "Please verify your CRC parameters are correct", 
            computed_check, (uint64_t)check);
        return;
    }
}
/* }}} */

/* {{{ CrcFast\Params::getWidth(): int */
PHP_METHOD(CrcFast_Params, getWidth)
{
    php_crc_fast_params_obj *obj = Z_CRC_FAST_PARAMS_P(getThis());

    ZEND_PARSE_PARAMETERS_NONE();

    if (!obj) {
        zend_throw_exception(zend_ce_exception, "Invalid Params object", 0);
        return;
    }

    RETURN_LONG((zend_long)obj->params.width);
}
/* }}} */

/* {{{ CrcFast\Params::getPoly(): int */
PHP_METHOD(CrcFast_Params, getPoly)
{
    php_crc_fast_params_obj *obj = Z_CRC_FAST_PARAMS_P(getThis());

    ZEND_PARSE_PARAMETERS_NONE();

    if (!obj) {
        zend_throw_exception(zend_ce_exception, "Invalid Params object", 0);
        return;
    }

    RETURN_LONG((zend_long)obj->params.poly);
}
/* }}} */

/* {{{ CrcFast\Params::getInit(): int */
PHP_METHOD(CrcFast_Params, getInit)
{
    php_crc_fast_params_obj *obj = Z_CRC_FAST_PARAMS_P(getThis());

    ZEND_PARSE_PARAMETERS_NONE();

    if (!obj) {
        zend_throw_exception(zend_ce_exception, "Invalid Params object", 0);
        return;
    }

    RETURN_LONG((zend_long)obj->params.init);
}
/* }}} */

/* {{{ CrcFast\Params::getRefin(): bool */
PHP_METHOD(CrcFast_Params, getRefin)
{
    php_crc_fast_params_obj *obj = Z_CRC_FAST_PARAMS_P(getThis());

    ZEND_PARSE_PARAMETERS_NONE();

    if (!obj) {
        zend_throw_exception(zend_ce_exception, "Invalid Params object", 0);
        return;
    }

    RETURN_BOOL(obj->params.refin);
}
/* }}} */

/* {{{ CrcFast\Params::getRefout(): bool */
PHP_METHOD(CrcFast_Params, getRefout)
{
    php_crc_fast_params_obj *obj = Z_CRC_FAST_PARAMS_P(getThis());

    ZEND_PARSE_PARAMETERS_NONE();

    if (!obj) {
        zend_throw_exception(zend_ce_exception, "Invalid Params object", 0);
        return;
    }

    RETURN_BOOL(obj->params.refout);
}
/* }}} */

/* {{{ CrcFast\Params::getXorout(): int */
PHP_METHOD(CrcFast_Params, getXorout)
{
    php_crc_fast_params_obj *obj = Z_CRC_FAST_PARAMS_P(getThis());

    ZEND_PARSE_PARAMETERS_NONE();

    if (!obj) {
        zend_throw_exception(zend_ce_exception, "Invalid Params object", 0);
        return;
    }

    RETURN_LONG((zend_long)obj->params.xorout);
}
/* }}} */

/* {{{ CrcFast\Params::getCheck(): int */
PHP_METHOD(CrcFast_Params, getCheck)
{
    php_crc_fast_params_obj *obj = Z_CRC_FAST_PARAMS_P(getThis());

    ZEND_PARSE_PARAMETERS_NONE();

    if (!obj) {
        zend_throw_exception(zend_ce_exception, "Invalid Params object", 0);
        return;
    }

    RETURN_LONG((zend_long)obj->params.check);
}
/* }}} */

/* {{{ CrcFast\Params::getKeys(): array */
PHP_METHOD(CrcFast_Params, getKeys)
{
    php_crc_fast_params_obj *obj = Z_CRC_FAST_PARAMS_P(getThis());

    ZEND_PARSE_PARAMETERS_NONE();

    if (!obj) {
        zend_throw_exception(zend_ce_exception, "Invalid Params object", 0);
        return;
    }

    array_init(return_value);

    // Add all 23 keys to the array
    for (int i = 0; i < 23; i++) {
        add_index_long(return_value, i, (zend_long)obj->params.keys[i]);
    }
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
    
    // With std first, offset should be 0
    php_crc_fast_digest_object_handlers.offset = offsetof(php_crc_fast_digest_obj, std);
    php_crc_fast_digest_object_handlers.free_obj = php_crc_fast_digest_free_obj;
    php_crc_fast_digest_object_handlers.clone_obj = NULL;

    // Register the Params class using the auto-generated function
    php_crc_fast_params_ce = register_class_CrcFast_Params();

    // Set up the create_object handler for the class
    php_crc_fast_params_ce->create_object = php_crc_fast_params_create_object;

    // Initialize the object handlers
    memcpy(&php_crc_fast_params_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    php_crc_fast_params_object_handlers.offset = offsetof(php_crc_fast_params_obj, std);
    php_crc_fast_params_object_handlers.free_obj = php_crc_fast_params_free_obj;
    php_crc_fast_params_object_handlers.clone_obj = NULL; // No cloning support

    return SUCCESS;
}

/* {{{ crc_fast_module_entry */
extern "C" {
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
}
/* }}} */

#ifdef COMPILE_DL_CRC_FAST
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
extern "C" {
ZEND_GET_MODULE(crc_fast)
}
#endif
