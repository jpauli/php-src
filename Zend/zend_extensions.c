/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend_extensions.h"
#include "tsrm_virtual_cwd.h" /* DEFAULT_SLASH */

ZEND_API zend_llist zend_extensions;
static int last_resource_number;

/* {{{ match the executing build against build id in version info */
#define BUILD_MATCH(version)   (strcmp(ZEND_EXTENSION_BUILD_ID, version->build_id) == SUCCESS) /* }}} */

/* {{{ execute the build id check handler for ext, when necessary */
#define BUILD_CHECK(ext)       (!ext->build_id_check || (ext->build_id_check(ZEND_EXTENSION_BUILD_ID) == SUCCESS)) /* }}} */

/* {{{ match the executing API against API in version info */
#define API_MATCH(version)     (version->zend_extension_api_no == ZEND_EXTENSION_API_NO) /* }}} */

/* {{{ execute the API check handler for ext */
#define API_CHECK(ext)         (!ext->api_no_check || ext->api_no_check(ZEND_EXTENSION_API_NO) == SUCCESS) /* }}} */

/* {{{ tell if the current API is old or new */
#define API_PROBLEM(version)   ((version->zend_extension_api_no < ZEND_EXTENSION_API_NO) ? "new" : "old") /* }}} */

/* {{{ load and register an extension using absolute paths only */
int zend_load_extension(const char *path)
{
#if ZEND_EXTENSIONS_SUPPORT
	DL_HANDLE handle;
#define ERROR_TYPE E_CORE_WARNING
	zend_extension *extension;
	zend_extension_version_info *version;

	handle = DL_LOAD(path);
	if (!handle) {
#ifndef ZEND_WIN32
		zend_error(ERROR_TYPE, "Failed loading %s:  %s", path, DL_ERROR());
#else
		zend_error(ERROR_TYPE, "Failed loading %s", path);
#endif
		return FAILURE;
	}

	version = (zend_extension_version_info *) DL_FETCH_SYMBOL(handle, "extension_version_info");
	if (!version) {
		version = (zend_extension_version_info *) DL_FETCH_SYMBOL(handle, "_extension_version_info");
	}
	
	extension = (zend_extension *) DL_FETCH_SYMBOL(handle, "zend_extension_entry");
	if (!extension) {
		extension = (zend_extension *) DL_FETCH_SYMBOL(handle, "_zend_extension_entry");
	}
	
	if (!version || !extension) {
		if (DL_FETCH_SYMBOL(handle, "get_module") || DL_FETCH_SYMBOL(handle, "_get_module")) {
			zend_error(ERROR_TYPE, "%s appears to be a PHP extension, try to load it using extension=%s", path, strrchr(path, DEFAULT_SLASH) + 1);
		} else zend_error(ERROR_TYPE, "%s doesn't appear to be a valid Zend or PHP extension", path);
		DL_UNLOAD(handle);
		return FAILURE;
	}

	/* check for API compatibility */
	if (!API_MATCH(version) && !API_CHECK(extension)) {
	    if (extension->author && extension->URL) {
	       zend_error(
	            ERROR_TYPE, 
	            "%s requires Zend Engine API version %d, the installed version %d is too %s. "
	            "Try contacting %s <%s> for assistance.",
				extension->name,
				version->zend_extension_api_no,
				ZEND_EXTENSION_API_NO,
				API_PROBLEM(version),
				extension->author,
				extension->URL
		    );
	    } else {
	       zend_error(
	            ERROR_TYPE, 
	            "%s requires Zend Engine API version %d, the installed version %d is too %s.",
				extension->name,
				version->zend_extension_api_no,
				ZEND_EXTENSION_API_NO,
				API_PROBLEM(version)
		    );
	    }
	    
	    DL_UNLOAD(handle);
	    return FAILURE;
	} 
	
	/* check for build compatibility */
	if (!BUILD_MATCH(version) && !BUILD_CHECK(extension)) {
	    zend_error(
	        ERROR_TYPE, 
	        "The Zend Extension %s asserts that it cannot be loaded in this environment. "
	        "The extension was built using build %s, the current build is %s",
		    extension->name, version->build_id, ZEND_EXTENSION_BUILD_ID
		);
		DL_UNLOAD(handle);
		return FAILURE;
	}

	return zend_register_extension(extension, handle);
#else
	zend_error(ERROR_TYPE,"Extensions are not supported on this platform.");
	return FAILURE;
#endif
} /* }}} */

/* {{{ register a loaded extension */
int zend_register_extension(zend_extension *new_extension, DL_HANDLE handle)
{
#if ZEND_EXTENSIONS_SUPPORT
	zend_extension extension;

	extension = *new_extension;
	extension.handle = handle;

	zend_extension_dispatch_message(ZEND_EXTMSG_NEW_EXTENSION, &extension);

	zend_llist_add_element(&zend_extensions, &extension);

#if ZEND_DEBUG
	fprintf(stderr, "Loaded zend_extension '%s', version %s\n", extension.name, extension.version);
#ifdef PHP_WIN32
	fflush(stderr);
#endif
#endif

#endif

	return SUCCESS;
} /* }}} */

static void zend_extension_shutdown(zend_extension *extension TSRMLS_DC)
{
#if ZEND_EXTENSIONS_SUPPORT
	if (extension->shutdown) {
		extension->shutdown(extension);
	}
#endif
}

static int zend_extension_startup(zend_extension *extension)
{
#if ZEND_EXTENSIONS_SUPPORT
	if (extension->startup) {
		if (extension->startup(extension)!=SUCCESS) {
			return 1;
		}
		zend_append_version_info(extension);
	}
#endif
	return 0;
}


int zend_startup_extensions_mechanism()
{
	/* Startup extensions mechanism */
	zend_llist_init(&zend_extensions, sizeof(zend_extension), (void (*)(void *)) zend_extension_dtor, 1);
	last_resource_number = 0;
	return SUCCESS;
}


int zend_startup_extensions()
{
	zend_llist_apply_with_del(&zend_extensions, (int (*)(void *)) zend_extension_startup);
	return SUCCESS;
}


void zend_shutdown_extensions(TSRMLS_D)
{
	zend_llist_apply(&zend_extensions, (llist_apply_func_t) zend_extension_shutdown TSRMLS_CC);
	zend_llist_destroy(&zend_extensions);
}


void zend_extension_dtor(zend_extension *extension)
{
#if ZEND_EXTENSIONS_SUPPORT && !ZEND_DEBUG
	if (extension->handle) {
		DL_UNLOAD(extension->handle);
	}
#endif
}


static void zend_extension_message_dispatcher(const zend_extension *extension, int num_args, va_list args TSRMLS_DC)
{
	int message;
	void *arg;

	if (!extension->message_handler || num_args!=2) {
		return;
	}
	message = va_arg(args, int);
	arg = va_arg(args, void *);
	extension->message_handler(message, arg);
}


ZEND_API void zend_extension_dispatch_message(int message, void *arg)
{
	TSRMLS_FETCH();

	zend_llist_apply_with_arguments(&zend_extensions, (llist_apply_with_args_func_t) zend_extension_message_dispatcher TSRMLS_CC, 2, message, arg);
}


ZEND_API int zend_get_resource_handle(zend_extension *extension)
{
	if (last_resource_number<ZEND_MAX_RESERVED_RESOURCES) {
		extension->resource_number = last_resource_number;
		return last_resource_number++;
	} else {
		return -1;
	}
}


ZEND_API zend_extension *zend_get_extension(const char *extension_name)
{
	zend_llist_element *element;

	for (element = zend_extensions.head; element; element = element->next) {
		zend_extension *extension = (zend_extension *) element->data;

		if (!strcmp(extension->name, extension_name)) {
			return extension;
		}
	}
	return NULL;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
