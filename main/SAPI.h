#ifndef _NEW_SAPI_H
#define _NEW_SAPI_H

#include "zend.h"
#include "zend_llist.h"


#if WIN32||WINNT
#	ifdef SAPI_EXPORTS
#	define SAPI_API __declspec(dllexport) 
#	else
#	define SAPI_API __declspec(dllimport) 
#	endif
#else
#define SAPI_API
#endif


typedef struct {
	char *header;
	uint header_len;
} sapi_header_struct;


typedef struct {
	zend_llist headers;
	sapi_header_struct content_type;
	int http_response_code;
} sapi_headers_struct;


typedef struct _sapi_module_struct sapi_module_struct;


extern sapi_module_struct sapi_module;  /* true global */


typedef struct {
	char *query_string;

	char *path_translated;
	char *request_uri;

	unsigned char headers_only;
} sapi_request_info;


typedef struct {
	void *server_context;
	sapi_request_info request_info;
	sapi_headers_struct sapi_headers;
	unsigned char headers_sent;
} sapi_globals_struct;


SAPI_API void sapi_startup(sapi_module_struct *sf);
SAPI_API void sapi_activate(SLS_D);
SAPI_API void sapi_deactivate(SLS_D);

SAPI_API int sapi_add_header(const char *header_line, uint header_line_len);
SAPI_API int sapi_send_headers();

#ifdef ZTS
# define SLS_D	sapi_globals_struct *sapi_globals
# define SLS_DC	, SLS_D
# define SLS_C	sapi_globals
# define SLS_CC , SLS_C
# define SG(v) (sapi_globals->v)
# define SLS_FETCH()	sapi_globals_struct *sapi_globals = ts_resource(sapi_globals_id)
SAPI_API extern int sapi_globals_id;
#else
# define SLS_D
# define SLS_DC
# define SLS_C
# define SLS_CC
# define SG(v) (sapi_globals.v)
# define SLS_FETCH()
extern SAPI_API sapi_globals_struct sapi_globals;
#endif



struct _sapi_module_struct {
	char *name;

	int (*startup)(struct _sapi_module_struct *sapi_module);
	int (*shutdown)(struct _sapi_module_struct *sapi_module);

	int (*ub_write)(const char *str, unsigned int str_length);

	int (*header_handler)(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers);
	int (*send_headers)(sapi_headers_struct *sapi_headers SLS_DC);
	void (*send_header)(sapi_header_struct *sapi_header, void *server_context);
};



/* header_handler() constants */
#define SAPI_HEADER_ADD			(1<<0)
#define SAPI_HEADER_DELETE_ALL	(1<<1)
#define SAPI_HEADER_SEND_NOW	(1<<2)


#define SAPI_HEADER_SENT_SUCCESSFULLY	1
#define SAPI_HEADER_DO_SEND				2
#define SAPI_HEADER_SEND_FAILED			3

#define DEFAULT_CONTENT_TYPE "Content-Type: text/html"

#endif /* _NEW_SAPI_H */