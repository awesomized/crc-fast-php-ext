dnl config.m4 for extension crc_fast

PHP_ARG_WITH([crc_fast],
  [for crc_fast support],
  [AS_HELP_STRING([--with-crc-fast[=DIR]],
    [Include crc_fast support. DIR is the crc_fast library install prefix])],
  [no])

if test "$PHP_CRC_FAST" != "no"; then

  PHP_REQUIRE_CXX()

  AC_MSG_CHECKING([PHP version])

  if test -z "$PHP_CONFIG"; then
    AC_MSG_ERROR([php-config not found])
  fi
  php_version=`$PHP_CONFIG --vernum`

  if test -z "$php_version"; then
    AC_MSG_ERROR([failed to detect PHP version, please report])
  fi

  if test "$php_version" -lt "80100"; then
    AC_MSG_ERROR([You need at least PHP 8.1.0 to be able to use this version of simdutf. PHP $php_version found])
  else
    AC_MSG_RESULT([$php_version, ok])
  fi

  dnl # --with-crc-fast -> check with-path
  SEARCH_PATH="/usr/local /usr"
  SEARCH_FOR="/include/libcrc_fast.h"
  if test -r $PHP_CRC_FAST/$SEARCH_FOR; then # path given as parameter
    LIBCRC_FAST_DIR=$PHP_CRC_FAST
  else # search default path list
    AC_MSG_CHECKING([for crc_fast files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        LIBCRC_FAST_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$LIBCRC_FAST_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the libcrc_fast distribution])
  fi

  dnl # --with-crc-fast -> add include path
  PHP_ADD_INCLUDE($LIBCRC_FAST_DIR/include)

  dnl # --with-crc-fast -> check for lib and symbol presence
  LIBNAME=crc_fast
  LIBSYMBOL=crc_fast_digest_new

  PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL, [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LIBCRC_FAST_DIR/lib, CRC_FAST_SHARED_LIBADD)
    AC_DEFINE(HAVE_CRC_FAST, 1, [whether crc_fast is enabled])
  ], [
    AC_MSG_ERROR([crc_fast library not found])
  ], [
   -L$LIBCRC_FAST_DIR/lib -lm
  ])

  PHP_SUBST(CRC_FAST_SHARED_LIBADD)

  dnl Mark symbols hidden by default if the compiler (for example, gcc >= 4)
  dnl supports it. This can help reduce the binary size and startup time.
  AX_CHECK_COMPILE_FLAG([-fvisibility=hidden],
                        [CXXFLAGS="$CXXFLAGS -fvisibility=hidden"])

  dnl Disable exceptions because PHP is written in C and loads this C++ module, handle errors manually.
  dnl Disable development checks of C crc_fast library in php debug builds (can manually override)
  PHP_NEW_EXTENSION(crc_fast,
  [php_crc_fast.cpp],
    $ext_shared,, "-std=c++17 -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1 -DCRC_FAST_EXCEPTIONS=0 -DCRC_FAST_DEVELOPMENT_CHECKS=0", cxx)

  PHP_INSTALL_HEADERS([ext/crc_fast], [php_crc_fast.h])
  PHP_ADD_MAKEFILE_FRAGMENT
  PHP_ADD_BUILD_DIR(src, 1)
fi
