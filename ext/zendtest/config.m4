dnl $Id$
dnl config.m4 for extension zend test extension

PHP_ARG_ENABLE(zend-test-extension, whether to enable zend test extension support,
[  --enable-zend-test-extension  Enable zend test extension support])

PHP_ARG_WITH(zend-test-extension-build-engine, which engine to use to build zend-test-extension, 
[  --with-zend-test-extension-build-engine Build as zend or php extension, or both], php, no)


PHP_ARG_WITH(module-api-no, PHP extension API number, 
[  --with-module-api-no PHP extension API number], 0, no)

PHP_ARG_WITH(module-build-id, PHP extension build id, 
[  --with-module-build-id PHP extension build id], 0, no)

PHP_ARG_WITH(zend-build-id, Zend extension build id, 
[  --with-zend-build-id Zend extension build id], 0, no)

PHP_ARG_WITH(zend-api-no, Zend extension API number, 
[  --with-zend-api-no Zend extension API number], 0, no)

if test "$PHP_ZEND_TEST_EXTENSION" != "no"; then

  AC_MSG_CHECKING(which mode to build zend test extension in)
  if test "$PHP_ZEND_TEST_EXTENSION_BUILD_ENGINE" == "both"; then
    AC_DEFINE(BUILD_ENGINE_ZEND, 1, [ ])
    AC_DEFINE(BUILD_ENGINE_PHP, 1, [ ])
  else 
   if test "$PHP_ZEND_TEST_EXTENSION_BUILD_ENGINE" == "zend"; then
    AC_DEFINE(BUILD_ENGINE_ZEND, 1, [ ])
   else
    AC_DEFINE(BUILD_ENGINE_PHP, 1, [ ])
   fi
  fi
  AC_MSG_RESULT($PHP_ZEND_TEST_EXTENSION_BUILD_ENGINE)
  AC_DEFINE_UNQUOTED(BUILD_ENGINE, [$PHP_ZEND_TEST_EXTENSION_BUILD_ENGINE], [build engine for zend test extension])
 
  AC_MSG_CHECKING(if PHP extension check overwrite has been provided)
  if test "$PHP_MODULE_API_NO" != 0; then
    AC_DEFINE_UNQUOTED(FORCE_ZEND_MODULE_API_NO, $PHP_MODULE_API_NO, [ ])
  fi
  if test "$PHP_MODULE_BUILD_ID" != 0; then
    AC_DEFINE_UNQUOTED(FORCE_ZEND_MODULE_BUILD_ID, "$PHP_MODULE_BUILD_ID", [ ])
  fi
  
  AC_MSG_CHECKING(if Zend Engine extension check overwrite has been provided)
  if test "$PHP_ZEND_API_NO" != 0; then
    AC_DEFINE_UNQUOTED(FORCE_ZEND_EXTENSION_API_NO, $PHP_ZEND_API_NO, [ ])
  fi
  if test "$PHP_ZEND_BUILD_ID" != 0; then
    AC_DEFINE_UNQUOTED(FORCE_ZEND_EXTENSION_BUILD_ID, "$PHP_ZEND_BUILD_ID", [ ])
  fi
  
  PHP_NEW_EXTENSION(zend_test_extension, zend_test_extension.c, $ext_shared)
fi
