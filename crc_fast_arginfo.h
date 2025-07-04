/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 96eb8b181c4a140e8c3740856b2c851cbfc30cbb */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_CrcFast_hash, 0, 2, IS_STRING, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, algorithm, CrcFast\\Params, MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, binary, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_CrcFast_hash_file, 0, 2, IS_STRING, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, algorithm, CrcFast\\Params, MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, binary, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_CrcFast_get_supported_algorithms, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_CrcFast_combine, 0, 4, IS_STRING, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, algorithm, CrcFast\\Params, MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO(0, checksum1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, checksum2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, binary, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_CrcFast_crc32, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CrcFast_Params___construct, 0, 0, 7)
	ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, poly, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, init, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, refin, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, refout, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, xorout, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, check, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, keys, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CrcFast_Params_getWidth, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CrcFast_Params_getPoly arginfo_class_CrcFast_Params_getWidth

#define arginfo_class_CrcFast_Params_getInit arginfo_class_CrcFast_Params_getWidth

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CrcFast_Params_getRefin, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CrcFast_Params_getRefout arginfo_class_CrcFast_Params_getRefin

#define arginfo_class_CrcFast_Params_getXorout arginfo_class_CrcFast_Params_getWidth

#define arginfo_class_CrcFast_Params_getCheck arginfo_class_CrcFast_Params_getWidth

#define arginfo_class_CrcFast_Params_getKeys arginfo_CrcFast_get_supported_algorithms

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CrcFast_Digest___construct, 0, 0, 1)
	ZEND_ARG_OBJ_TYPE_MASK(0, algorithm, CrcFast\\Params, MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CrcFast_Digest_update, 0, 1, CrcFast\\Digest, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CrcFast_Digest_finalize, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, binary, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_class_CrcFast_Digest_finalizeReset arginfo_class_CrcFast_Digest_finalize

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CrcFast_Digest_reset, 0, 0, CrcFast\\Digest, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CrcFast_Digest_combine, 0, 1, CrcFast\\Digest, 0)
	ZEND_ARG_OBJ_INFO(0, digest, CrcFast\\Digest, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(CrcFast_hash);
ZEND_FUNCTION(CrcFast_hash_file);
ZEND_FUNCTION(CrcFast_get_supported_algorithms);
ZEND_FUNCTION(CrcFast_combine);
ZEND_FUNCTION(CrcFast_crc32);
ZEND_METHOD(CrcFast_Params, __construct);
ZEND_METHOD(CrcFast_Params, getWidth);
ZEND_METHOD(CrcFast_Params, getPoly);
ZEND_METHOD(CrcFast_Params, getInit);
ZEND_METHOD(CrcFast_Params, getRefin);
ZEND_METHOD(CrcFast_Params, getRefout);
ZEND_METHOD(CrcFast_Params, getXorout);
ZEND_METHOD(CrcFast_Params, getCheck);
ZEND_METHOD(CrcFast_Params, getKeys);
ZEND_METHOD(CrcFast_Digest, __construct);
ZEND_METHOD(CrcFast_Digest, update);
ZEND_METHOD(CrcFast_Digest, finalize);
ZEND_METHOD(CrcFast_Digest, finalizeReset);
ZEND_METHOD(CrcFast_Digest, reset);
ZEND_METHOD(CrcFast_Digest, combine);


static const zend_function_entry ext_functions[] = {
	ZEND_NS_FALIAS("CrcFast", hash, CrcFast_hash, arginfo_CrcFast_hash)
	ZEND_NS_FALIAS("CrcFast", hash_file, CrcFast_hash_file, arginfo_CrcFast_hash_file)
	ZEND_NS_FALIAS("CrcFast", get_supported_algorithms, CrcFast_get_supported_algorithms, arginfo_CrcFast_get_supported_algorithms)
	ZEND_NS_FALIAS("CrcFast", combine, CrcFast_combine, arginfo_CrcFast_combine)
	ZEND_NS_FALIAS("CrcFast", crc32, CrcFast_crc32, arginfo_CrcFast_crc32)
	ZEND_FE_END
};


static const zend_function_entry class_CrcFast_Params_methods[] = {
	ZEND_ME(CrcFast_Params, __construct, arginfo_class_CrcFast_Params___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(CrcFast_Params, getWidth, arginfo_class_CrcFast_Params_getWidth, ZEND_ACC_PUBLIC)
	ZEND_ME(CrcFast_Params, getPoly, arginfo_class_CrcFast_Params_getPoly, ZEND_ACC_PUBLIC)
	ZEND_ME(CrcFast_Params, getInit, arginfo_class_CrcFast_Params_getInit, ZEND_ACC_PUBLIC)
	ZEND_ME(CrcFast_Params, getRefin, arginfo_class_CrcFast_Params_getRefin, ZEND_ACC_PUBLIC)
	ZEND_ME(CrcFast_Params, getRefout, arginfo_class_CrcFast_Params_getRefout, ZEND_ACC_PUBLIC)
	ZEND_ME(CrcFast_Params, getXorout, arginfo_class_CrcFast_Params_getXorout, ZEND_ACC_PUBLIC)
	ZEND_ME(CrcFast_Params, getCheck, arginfo_class_CrcFast_Params_getCheck, ZEND_ACC_PUBLIC)
	ZEND_ME(CrcFast_Params, getKeys, arginfo_class_CrcFast_Params_getKeys, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_CrcFast_Digest_methods[] = {
	ZEND_ME(CrcFast_Digest, __construct, arginfo_class_CrcFast_Digest___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(CrcFast_Digest, update, arginfo_class_CrcFast_Digest_update, ZEND_ACC_PUBLIC)
	ZEND_ME(CrcFast_Digest, finalize, arginfo_class_CrcFast_Digest_finalize, ZEND_ACC_PUBLIC)
	ZEND_ME(CrcFast_Digest, finalizeReset, arginfo_class_CrcFast_Digest_finalizeReset, ZEND_ACC_PUBLIC)
	ZEND_ME(CrcFast_Digest, reset, arginfo_class_CrcFast_Digest_reset, ZEND_ACC_PUBLIC)
	ZEND_ME(CrcFast_Digest, combine, arginfo_class_CrcFast_Digest_combine, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static void register_crc_fast_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_32_AIXM", 10000, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_32_AUTOSAR", 10010, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_32_BASE_91_D", 10020, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_32_BZIP2", 10030, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_32_CD_ROM_EDC", 10040, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_32_CKSUM", 10050, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_32_ISCSI", 10060, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_32_ISO_HDLC", 10070, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_32_JAMCRC", 10080, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_32_MEF", 10090, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_32_MPEG_2", 10100, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_32_PHP", 10200, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_32_XFER", 10300, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_64_ECMA_182", 20000, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_64_GO_ISO", 20010, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_64_MS", 20020, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_64_NVME", 20030, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_64_REDIS", 20040, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_64_WE", 20050, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CrcFast\\CRC_64_XZ", 20060, CONST_PERSISTENT);
}

static zend_class_entry *register_class_CrcFast_Params(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "CrcFast", "Params", class_CrcFast_Params_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}

static zend_class_entry *register_class_CrcFast_Digest(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "CrcFast", "Digest", class_CrcFast_Digest_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}
