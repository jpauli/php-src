/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stig Venaas <venaas@uninett.no>                              |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifndef _PHP_NETWORK_H
#define _PHP_NETWORK_H

#ifdef PHP_WIN32
# ifndef WINNT
#  define WINNT 1
# endif
# undef FD_SETSIZE
# include "arpa/inet.h"
# define socklen_t unsigned int
#else
# undef closesocket
# define closesocket close
#endif

#ifndef HAVE_SHUTDOWN
#undef shutdown
#define shutdown(s,n)	/* nothing */
#endif

#ifdef PHP_WIN32
#define EWOULDBLOCK WSAEWOULDBLOCK
#	define fsync _commit
#	define ftruncate(a, b) chsize(a, b)
#endif /* defined(PHP_WIN32) */

#ifdef PHP_WIN32
#define streams_socket_errno WSAGetLastError()
#else
#define streams_socket_errno errno
#endif

#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#if HAVE_OPENSSL_EXT
#include <openssl/ssl.h>
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#ifdef PHP_WIN32
# define SOCK_ERR INVALID_SOCKET
# define SOCK_CONN_ERR SOCKET_ERROR
# define SOCK_RECV_ERR SOCKET_ERROR
#else
# define SOCK_ERR -1
# define SOCK_CONN_ERR -1
# define SOCK_RECV_ERR -1
#endif

#define PHP_SOCK_CHUNK_SIZE	8192

#ifdef HAVE_SOCKADDR_STORAGE
typedef struct sockaddr_storage php_sockaddr_storage;
#else
typedef struct {
        unsigned short ss_family;
        char info[256];
} php_sockaddr_storage;
#endif


int php_hostconnect(const char *host, unsigned short port, int socktype, struct timeval *timeout);
PHPAPI int php_connect_nonb(int sockfd, const struct sockaddr *addr, socklen_t addrlen, struct timeval *timeout);

#ifdef PHP_WIN32
PHPAPI int php_connect_nonb_win32(SOCKET sockfd, const struct sockaddr *addr, socklen_t addrlen, struct timeval *timeout);
#endif

void php_any_addr(int family, php_sockaddr_storage *addr, unsigned short port);
int php_sockaddr_size(php_sockaddr_storage *addr);

struct _php_netstream_data_t	{
	int socket;
	unsigned char *readbuf;
	size_t readbuflen;
	size_t readpos;
	size_t writepos;
	char eof;
	char is_blocked;
	size_t chunk_size;
	struct timeval timeout;
	char timeout_event;
#if HAVE_OPENSSL_EXT
	/* openssl specific bits here */
	SSL *ssl_handle;
	int ssl_active;
#endif
};
typedef struct _php_netstream_data_t php_netstream_data_t;

#define PHP_NETSTREAM_DATA_FROM_STREAM(stream)		(php_netstream_data_t*)(stream)->abstract

extern php_stream_ops php_stream_socket_ops;
#define PHP_STREAM_IS_SOCKET	(&php_stream_socket_ops)

PHPAPI php_stream *_php_stream_sock_open_from_socket(int socket, int persistent STREAMS_DC TSRMLS_DC );
/* open a connection to a host using php_hostconnect and return a stream */
PHPAPI php_stream *_php_stream_sock_open_host(const char *host, unsigned short port,
		int socktype, struct timeval *timeout, int persistent STREAMS_DC TSRMLS_DC);
PHPAPI php_stream *_php_stream_sock_open_unix(const char *path, int pathlen, int persistent,
		struct timeval *timeout STREAMS_DC TSRMLS_DC);

#define php_stream_sock_open_from_socket(socket, persistent)	_php_stream_sock_open_from_socket((socket), (persistent) STREAMS_CC TSRMLS_CC)
#define php_stream_sock_open_host(host, port, socktype, timeout, persistent)	_php_stream_sock_open_host((host), (port), (socktype), (timeout), (persistent) STREAMS_CC TSRMLS_CC)
#define php_stream_sock_open_unix(path, pathlen, persistent, timeval)	_php_stream_sock_open_unix((path), (pathlen), (persistent), (timeval) STREAMS_CC TSRMLS_CC)

/* {{{ memory debug */
#define php_stream_sock_open_from_socket_rel(socket, persistent)	_php_stream_sock_open_from_socket((socket), (persistent) STREAMS_REL_CC TSRMLS_CC)
#define php_stream_sock_open_host_rel(host, port, socktype, timeout, persistent)	_php_stream_sock_open_host((host), (port), (socktype), (timeout), (persistent) STREAMS_REL_CC TSRMLS_CC)
#define php_stream_sock_open_unix_rel(path, pathlen, persistent, timeval)	_php_stream_sock_open_unix((path), (pathlen), (persistent), (timeval) STREAMS_REL_CC TSRMLS_CC)

/* }}} */

PHPAPI void php_stream_sock_set_timeout(php_stream *stream, struct timeval *timeout TSRMLS_DC);
PHPAPI int php_stream_sock_set_blocking(php_stream *stream, int mode TSRMLS_DC);
/* set the chunk size for the stream; return the old chunk size */
PHPAPI size_t php_stream_sock_set_chunk_size(php_stream *stream, size_t size TSRMLS_DC);

#if HAVE_OPENSSL_EXT
PHPAPI int php_stream_sock_ssl_activate_with_method(php_stream *stream, int activate, SSL_METHOD *method TSRMLS_DC);
#define php_stream_sock_ssl_activate(stream, activate)	php_stream_sock_ssl_activate_with_method((stream), (activate), SSLv23_client_method() TSRMLS_CC)

#endif

#endif /* _PHP_NETWORK_H */

/*
 * Local variables:
 * tab-width: 8
 * c-basic-offset: 8
 * End:
 */
