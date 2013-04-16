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

#ifndef PHP_ZEND_TEST_EXTENSION_H
#define PHP_ZEND_TEST_EXTENSION_H

#ifdef BUILD_ENGINE_PHP
extern zend_module_entry zend_test_extension_module_entry;
#define phpext_zend_test_extension_ptr &zend_test_extension_module_entry

PHP_MINIT_FUNCTION(zend_test_extension);
PHP_MSHUTDOWN_FUNCTION(zend_test_extension);
PHP_RINIT_FUNCTION(zend_test_extension);
PHP_RSHUTDOWN_FUNCTION(zend_test_extension);
PHP_MINFO_FUNCTION(zend_test_extension);
#endif

#ifdef BUILD_ENGINE_ZEND
#include "Zend/zend_extensions.h"
extern zend_extension zend_extension_entry;
extern zend_extension_version_info extension_version_info;
#endif

#ifdef PHP_WIN32
#	define PHP_ZEND_TEST_EXTENSION_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_ZEND_TEST_EXTENSION_API __attribute__ ((visibility("default")))
#else
#	define PHP_ZEND_TEST_EXTENSION_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif



#ifdef ZTS
#define ZEND_TEST_EXTENSION_G(v) TSRMG(zend_test_extension_globals_id, zend_zend_test_extension_globals *, v)
#else
#define ZEND_TEST_EXTENSION_G(v) (zend_test_extension_globals.v)
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
