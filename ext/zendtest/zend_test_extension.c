/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Julien Pauli jpauli@php.net                                  |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_zend_test_extension.h"
#include "Zend/zend_extensions.h"

#ifdef BUILD_ENGINE_PHP

#ifdef FORCE_ZEND_MODULE_API_NO
#undef ZEND_MODULE_API_NO
#define ZEND_MODULE_API_NO FORCE_ZEND_MODULE_API_NO
#endif

#ifdef FORCE_ZEND_MODULE_BUILD_ID
#undef ZEND_MODULE_BUILD_ID
#define ZEND_MODULE_BUILD_ID FORCE_ZEND_MODULE_BUILD_ID
#endif

zend_module_entry zend_test_extension_module_entry = {
	STANDARD_MODULE_HEADER,
	"Zend test extension PHP",
	NULL,
	PHP_MINIT(zend_test_extension),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(zend_test_extension),
	"1.0",
	0,
	NULL,
	NULL,
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

# ifdef COMPILE_DL_ZEND_TEST_EXTENSION
ZEND_GET_MODULE(zend_test_extension)
# endif

PHP_MINIT_FUNCTION(zend_test_extension)
{
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(zend_test_extension)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(zend_test_extension)
{
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(zend_test_extension)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(zend_test_extension)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "zend test extension PHP support", "enabled");
	php_info_print_table_end();
}
#endif /* BUILD_ENGINE_PHP */

#ifdef BUILD_ENGINE_ZEND
ZEND_DLEXPORT zend_extension zend_extension_entry = {
		"Zend test extension Zend Engine",
		"1.0",
		"Julien Pauli",
		"http://www.php.net",
		NULL,

		NULL,
		NULL,
		NULL,
		NULL,

		NULL,

		NULL,

		NULL,
		NULL,
		NULL,

		NULL,
		NULL,

		STANDARD_ZEND_EXTENSION_PROPERTIES
};

#ifdef FORCE_ZEND_EXTENSION_API_NO
#undef ZEND_EXTENSION_API_NO
#define ZEND_EXTENSION_API_NO FORCE_ZEND_EXTENSION_API_NO
#endif

#ifdef FORCE_ZEND_EXTENSION_BUILD_ID
#undef ZEND_EXTENSION_BUILD_ID
#define ZEND_EXTENSION_BUILD_ID FORCE_ZEND_EXTENSION_BUILD_ID
#endif

ZEND_DLEXPORT zend_extension_version_info extension_version_info = { ZEND_EXTENSION_API_NO, ZEND_EXTENSION_BUILD_ID };
#endif /* BUILD_ENGINE_ZEND */
