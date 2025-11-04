/* crc_fast extension for PHP */
/* Copyright 2025 Don MacAskill. Licensed under MIT or Apache-2.0. */

#ifndef PHP_CRC_FAST_H
# define PHP_CRC_FAST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "php.h"

#ifdef __cplusplus
}
#endif

#include "libcrc_fast.h"

#ifdef __cplusplus
extern "C" {
#endif

extern zend_module_entry crc_fast_module_entry;

#ifdef __cplusplus
}
#endif

# define phpext_crc_fast_ptr &crc_fast_module_entry

# define PHP_CRC_FAST_VERSION "1.0.0"

# if defined(ZTS) && defined(COMPILE_DL_CRC_FAST)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

/* Define the CrcFast\Digest class */
typedef struct _php_crc_fast_digest_obj {
    CrcFastDigestHandle *digest;
    zend_object std;
    zend_long algorithm;
} php_crc_fast_digest_obj;

static inline php_crc_fast_digest_obj *php_crc_fast_digest_from_obj(zend_object *obj) {
    return (php_crc_fast_digest_obj*)((char*)(obj) - XtOffsetOf(php_crc_fast_digest_obj, std));
}

#define Z_CRC_FAST_DIGEST_P(zv) php_crc_fast_digest_from_obj(Z_OBJ_P(zv))

/* Algorithm constants that will be exposed to PHP, with room for expansion */
// CRC-32
#define PHP_CRC_FAST_CRC32_AIXM       10000
#define PHP_CRC_FAST_CRC32_AUTOSAR    10010
#define PHP_CRC_FAST_CRC32_BASE91D    10020
#define PHP_CRC_FAST_CRC32_BZIP2      10030
#define PHP_CRC_FAST_CRC32_CDROM_EDC  10040
#define PHP_CRC_FAST_CRC32_CKSUM      10050
#define PHP_CRC_FAST_CRC32_ISCSI      10060
#define PHP_CRC_FAST_CRC32_ISO_HDLC   10070
#define PHP_CRC_FAST_CRC32_JAMCRC     10080
#define PHP_CRC_FAST_CRC32_MEF        10090
#define PHP_CRC_FAST_CRC32_MPEG2      10100

// this is a special flower, just to handle PHP's `hash('crc32')` operation,
// which is actually byte-reversed CRC-32/BZIP2
#define PHP_CRC_FAST_CRC32_PHP        10200

#define PHP_CRC_FAST_CRC32_XFER       10300

// CRC-64
#define PHP_CRC_FAST_CRC64_ECMA182    20000
#define PHP_CRC_FAST_CRC64_GO_ISO     20010
#define PHP_CRC_FAST_CRC64_MS         20020
#define PHP_CRC_FAST_CRC64_NVME       20030
#define PHP_CRC_FAST_CRC64_REDIS      20040
#define PHP_CRC_FAST_CRC64_WE         20050
#define PHP_CRC_FAST_CRC64_XZ         20060

#endif	/* PHP_CRC_FAST_H */
