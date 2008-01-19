/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Gregory Beaver <cellog@php.net>                             |
  |          Marcus Boerger <helly@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include "php.h"
#if HAVE_PHAR_ZIP
#include "ext/zip/lib/zip.h"
#include "ext/zip/lib/zipint.h"
#endif
#include "tar.h"
#include "php_ini.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_exceptions.h"
#include "zend_hash.h"
#include "zend_interfaces.h"
#include "zend_operators.h"
#include "zend_qsort.h"
#include "zend_vm.h"
#include "main/php_streams.h"
#include "main/streams/php_stream_plain_wrapper.h"
#include "main/SAPI.h"
#include "main/php_main.h"
#include "main/php_open_temporary_file.h"
#include "ext/standard/info.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/file.h"
#include "ext/standard/php_string.h"
#include "ext/standard/url.h"
#include "ext/standard/crc32.h"
#include "ext/standard/md5.h"
#include "ext/standard/sha1.h"
#include "ext/standard/php_var.h"
#include "ext/standard/php_smart_str.h"
#include "ext/standard/php_versioning.h"
#ifndef PHP_WIN32
#include "TSRM/tsrm_strtok_r.h"
#endif
#include "TSRM/tsrm_virtual_cwd.h"
#if HAVE_SPL
#include "ext/spl/spl_array.h"
#include "ext/spl/spl_directory.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#endif
#include "php_phar.h"
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#if HAVE_HASH_EXT
#include "ext/hash/php_hash.h"
#include "ext/hash/php_hash_sha.h"
#endif

#ifndef E_RECOVERABLE_ERROR
#define E_RECOVERABLE_ERROR E_ERROR
#endif

#define PHAR_EXT_VERSION_STR      "2.0.0"
#define PHAR_API_VERSION_STR      "1.1.1"
/* x.y.z maps to 0xyz0 */
#define PHAR_API_VERSION          0x1110
/* if we bump PHAR_API_VERSION, change this from 0x1100 to PHAR_API_VERSION */
#define PHAR_API_VERSION_NODIR    0x1100
#define PHAR_API_MIN_DIR          0x1110
#define PHAR_API_MIN_READ         0x1000
#define PHAR_API_MAJORVERSION     0x1000
#define PHAR_API_MAJORVER_MASK    0xF000
#define PHAR_API_VER_MASK         0xFFF0

#define PHAR_HDR_COMPRESSION_MASK 0x0000F000
#define PHAR_HDR_COMPRESSED_NONE  0x00000000
#define PHAR_HDR_COMPRESSED_GZ    0x00001000
#define PHAR_HDR_COMPRESSED_BZ2   0x00002000
#define PHAR_HDR_SIGNATURE        0x00010000

/* flags for defining that the entire file should be compressed */
#define PHAR_FILE_COMPRESSION_MASK 0x00F00000
#define PHAR_FILE_COMPRESSED_NONE  0x00000000
#define PHAR_FILE_COMPRESSED_GZ    0x00100000
#define PHAR_FILE_COMPRESSED_BZ2   0x00200000

#define PHAR_SIG_MD5              0x0001
#define PHAR_SIG_SHA1             0x0002
#define PHAR_SIG_SHA256           0x0003
#define PHAR_SIG_SHA512           0x0004
#define PHAR_SIG_PGP              0x0010

/* flags byte for each file adheres to these bitmasks.
   All unused values are reserved */
#define PHAR_ENT_COMPRESSION_MASK 0x0000F000
#define PHAR_ENT_COMPRESSED_NONE  0x00000000
#define PHAR_ENT_COMPRESSED_GZ    0x00001000
#define PHAR_ENT_COMPRESSED_BZ2   0x00002000

#define PHAR_ENT_PERM_MASK        0x000001FF
#define PHAR_ENT_PERM_MASK_USR    0x000001C0
#define PHAR_ENT_PERM_SHIFT_USR   6
#define PHAR_ENT_PERM_MASK_GRP    0x00000038
#define PHAR_ENT_PERM_SHIFT_GRP   3
#define PHAR_ENT_PERM_MASK_OTH    0x00000007
#define PHAR_ENT_PERM_DEF_FILE    0x000001B6
#define PHAR_ENT_PERM_DEF_DIR     0x000001FF

#define TAR_FILE    '0'
#define TAR_LINK    '1'
#define TAR_SYMLINK '2'
#define TAR_DIR     '5'
#define TAR_NEW     '8'

ZEND_BEGIN_MODULE_GLOBALS(phar)
	HashTable   phar_fname_map;
	HashTable   phar_alias_map;
	HashTable   phar_plain_map;
	HashTable   phar_SERVER_mung_list;
	char*       extract_list;
	int         readonly;
	zend_bool   readonly_orig;
	zend_bool   require_hash_orig;
	int         request_init;
	int         require_hash;
	int         request_done;
	int         request_ends;
	void        (*orig_fopen)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_file_get_contents)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_is_file)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_is_link)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_is_dir)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_opendir)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_file_exists)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_fileperms)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_fileinode)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_filesize)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_fileowner)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_filegroup)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_fileatime)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_filemtime)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_filectime)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_filetype)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_is_writable)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_is_readable)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_is_executable)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_lstat)(INTERNAL_FUNCTION_PARAMETERS);
	void        (*orig_stat)(INTERNAL_FUNCTION_PARAMETERS);
	/* used for includes with . in them inside front controller */
	char*       cwd;
	int         cwd_len;
ZEND_END_MODULE_GLOBALS(phar)

ZEND_EXTERN_MODULE_GLOBALS(phar)

int phar_has_bz2;
int phar_has_zlib;
int phar_has_zip;
char *phar_zip_ver;

#ifdef ZTS
#	include "TSRM.h"
#	define PHAR_G(v) TSRMG(phar_globals_id, zend_phar_globals *, v)
#	define PHAR_GLOBALS ((zend_phar_globals *) (*((void ***) tsrm_ls))[TSRM_UNSHUFFLE_RSRC_ID(phar_globals_id)])
#else
#	define PHAR_G(v) (phar_globals.v)
#	define PHAR_GLOBALS (&phar_globals)
#endif

#ifndef php_uint16
# if SIZEOF_SHORT == 2
#  define php_uint16 unsigned short
# else
#  define php_uint16 uint16_t
# endif
#endif

#if HAVE_SPL
typedef union _phar_archive_object  phar_archive_object;
typedef union _phar_entry_object    phar_entry_object;
#endif

typedef struct _phar_archive_data phar_archive_data;
/* entry for one file in a phar file */
typedef struct _phar_entry_info {
	/* first bytes are exactly as in file */
	php_uint32               uncompressed_filesize;
	php_uint32               timestamp;
	php_uint32               compressed_filesize;
	php_uint32               crc32;
	php_uint32               flags;
	/* remainder */
	/* when changing compression, save old flags in case fp is NULL */
	php_uint32               old_flags;
	zval                     *metadata;
	php_uint32               filename_len;
	char                     *filename;
	long                     offset_within_phar;
	php_stream               *fp;
	php_stream               *cfp;
	int                      fp_refcount;
	int                      is_crc_checked:1;
	int                      is_modified:1;
	int                      is_deleted:1;
	int                      is_dir:1;
	/* used when iterating */
	int                      is_temp_dir:1;
	phar_archive_data        *phar;
	smart_str                metadata_str;
	/* tar-based phar file stuff */
	int                      is_tar:1;
	char                     *link; /* symbolic link to another file */
	char                     tar_type;
	/* zip-based phar file stuff */
	int                      is_zip:1;
#if HAVE_PHAR_ZIP
	int                      index;
	struct zip_file          *zip;
#endif
} phar_entry_info;

/* information about a phar file (the archive itself) */
struct _phar_archive_data {
	char                     *fname;
	int                      fname_len;
	char                     *alias;
	int                      alias_len;
	char                     version[12];
	size_t                   internal_file_start;
	size_t                   halt_offset;
	HashTable                manifest;
	php_uint32               flags;
	php_uint32               min_timestamp;
	php_uint32               max_timestamp;
	php_stream               *fp;
	int                      refcount;
	php_uint32               sig_flags;
	int                      sig_len;
	char                     *signature;
	zval                     *metadata;
	int                      is_explicit_alias:1;
	int                      is_modified:1;
	int                      is_writeable:1;
	int                      is_brandnew:1;
	/* defer phar creation */
	int                      donotflush:1;
	/* zip-based phar variables */
	int                      is_zip:1;
	/* tar-based phar variables */
	int                      is_tar:1;
#if HAVE_PHAR_ZIP
	struct zip               *zip;
#endif
};

#define PHAR_MIME_PHP '\0'
#define PHAR_MIME_PHPS '\1'
#define PHAR_MIME_OTHER '\2'

typedef struct _phar_mime_type {
	char *mime;
	int len;
	/* one of PHAR_MIME_* */
	char type;
} phar_mime_type;

/* stream access data for one file entry in a phar file */
typedef struct _phar_entry_data {
	phar_archive_data        *phar;
	/* stream position proxy, allows multiple open streams referring to the same fp */
	php_stream               *fp;
	off_t                    position;
	/* for copies of the phar fp, defines where 0 is */
	off_t                    zero;
	int                      for_write:1;
	int                      is_zip:1;
	int                      is_tar:1;
	phar_entry_info          *internal_file;
} phar_entry_data;

#if HAVE_SPL
/* archive php object */
union _phar_archive_object {
	zend_object              std;
	spl_filesystem_object    spl;
	struct {
	    zend_object          std;
	    phar_archive_data    *archive;
	} arc;
};
#endif

#if HAVE_SPL
/* entry php object */
union _phar_entry_object {
	zend_object              std;
	spl_filesystem_object    spl;
	struct {
	    zend_object          std;
	    phar_entry_info      *entry;
	} ent;
};
#endif

BEGIN_EXTERN_C()
#ifdef PHP_WIN32
char *tsrm_strtok_r(char *s, const char *delim, char **last);
#endif

void phar_request_initialize(TSRMLS_D);

void phar_object_init(TSRMLS_D);

int phar_open_filename(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC);
int phar_open_or_create_filename(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC);
int phar_create_or_parse_filename(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC);
int phar_open_compiled_file(char *alias, int alias_len, char **error TSRMLS_DC);
int phar_get_archive(phar_archive_data **archive, char *fname, int fname_len, char *alias, int alias_len, char **error TSRMLS_DC);
int phar_open_loaded(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC);
char *phar_create_default_stub(const char *index_php, size_t *len, char **error TSRMLS_DC);

char *phar_fix_filepath(char *path, int *new_len, int use_cwd TSRMLS_DC);
phar_entry_info * phar_open_jit(phar_archive_data *phar, phar_entry_info *entry, php_stream *fp,
				      char **error, int for_write TSRMLS_DC);
int phar_parse_metadata(char **buffer, zval **metadata, int is_zip TSRMLS_DC);
void destroy_phar_manifest_entry(void *pDest);

/* tar functions in tar.c */
int phar_is_tar(char *buf);
int phar_open_tarfile(php_stream* fp, char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, php_uint32 compression, char **error TSRMLS_DC);
int phar_flush(phar_archive_data *archive, char *user_stub, long len, char **error TSRMLS_DC);
int phar_open_or_create_tar(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC);
int phar_tar_flush(phar_archive_data *phar, char *user_stub, long len, char **error TSRMLS_DC);

/* zip functions in zip.c */
int phar_open_zipfile(char *fname, int fname_len, char *alias, int alias_len, phar_archive_data** pphar, char **error TSRMLS_DC);
int phar_open_or_create_zip(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC);
int phar_zip_flush(phar_archive_data *archive, char *user_stub, long len, char **error TSRMLS_DC);

#ifdef PHAR_MAIN
static int phar_open_fp(php_stream* fp, char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC);
extern php_stream_wrapper php_stream_phar_wrapper;
#endif

int phar_archive_delref(phar_archive_data *phar TSRMLS_DC);
int phar_entry_delref(phar_entry_data *idata TSRMLS_DC);

phar_entry_info *phar_get_entry_info(phar_archive_data *phar, char *path, int path_len, char **error TSRMLS_DC);
phar_entry_info *phar_get_entry_info_dir(phar_archive_data *phar, char *path, int path_len, char dir, char **error TSRMLS_DC);
phar_entry_data *phar_get_or_create_entry_data(char *fname, int fname_len, char *path, int path_len, char *mode, char allow_dir, char **error TSRMLS_DC);
int phar_get_entry_data(phar_entry_data **ret, char *fname, int fname_len, char *path, int path_len, char *mode, char allow_dir, char **error TSRMLS_DC);
int phar_flush(phar_archive_data *archive, char *user_stub, long len, char **error TSRMLS_DC);
int phar_detect_phar_fname_ext(const char *filename, int check_length, char **ext_str, int *ext_len);
int phar_split_fname(char *filename, int filename_len, char **arch, int *arch_len, char **entry, int *entry_len TSRMLS_DC);

typedef enum {
	pcr_use_query,
	pcr_is_ok,
	pcr_err_double_slash,
	pcr_err_up_dir,
	pcr_err_curr_dir,
	pcr_err_back_slash,
	pcr_err_star,
	pcr_err_illegal_char,
	pcr_err_empty_entry
} phar_path_check_result;

phar_path_check_result phar_path_check(char **p, int *len, const char **error);

END_EXTERN_C()

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
