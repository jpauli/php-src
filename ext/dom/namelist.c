/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_dom.h"


/*
* class domnamelist 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#NameList
* Since: DOM Level 3
*/

zend_function_entry php_dom_namelist_class_functions[] = {
	PHP_FALIAS(getName, dom_namelist_get_name, NULL)
	PHP_FALIAS(getNamespaceURI, dom_namelist_get_namespace_uri, NULL)
	{NULL, NULL, NULL}
};

/* {{{ attribute protos, not implemented yet */

/* {{{ proto length	unsigned long	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#NameList-length
Since: 
*/
int dom_namelist_length_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	ALLOC_ZVAL(*retval);
	ZVAL_STRING(*retval, "TEST", 1);
	return SUCCESS;
}

/* }}} */




/* {{{ proto domstring dom_namelist_get_name(unsigned long index);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#NameList-getName
Since: 
*/
PHP_FUNCTION(dom_namelist_get_name)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_namelist_get_name */


/* {{{ proto domstring dom_namelist_get_namespace_uri(unsigned long index);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#NameList-getNamespaceURI
Since: 
*/
PHP_FUNCTION(dom_namelist_get_namespace_uri)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_namelist_get_namespace_uri */
