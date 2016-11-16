
	/* $Id: zlog.c,v 1.7 2008/05/22 21:08:32 anight Exp $ */
	/* (c) 2004-2007 Andrei Nigmatulin */

#include "fpm_config.h"

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <errno.h>

#include "php_syslog.h"

#include "zlog.h"
#include "fpm.h"
#include "zend_portability.h"

#define MAX_LINE_LENGTH 1024

static int zlog_fd = -1;
static int zlog_level = ZLOG_NOTICE;
static int launched = 0;
static void (*external_logger)(int, char *, size_t) = NULL;

static const char *level_names[] = {
	[ZLOG_DEBUG]   = "DEBUG",
	[ZLOG_NOTICE]  = "NOTICE",
	[ZLOG_WARNING] = "WARNING",
	[ZLOG_ERROR]   = "ERROR",
	[ZLOG_ALERT]   = "ALERT",
};

#ifdef HAVE_SYSLOG_H
const int syslog_priorities[] = {
	[ZLOG_DEBUG]   = LOG_DEBUG,
	[ZLOG_NOTICE]  = LOG_NOTICE,
	[ZLOG_WARNING] = LOG_WARNING,
	[ZLOG_ERROR]   = LOG_ERR,
	[ZLOG_ALERT]   = LOG_ALERT,
};
#endif

void zlog_set_external_logger(void (*logger)(int, char *, size_t)) /* {{{ */
{
	external_logger = logger;
}
/* }}} */

const char *zlog_get_level_name(int log_level) /* {{{ */
{
	if (log_level < 0) {
		log_level = zlog_level;
	} else if (log_level < ZLOG_DEBUG || log_level > ZLOG_ALERT) {
		return "unknown value";
	}

	return level_names[log_level];
}
/* }}} */

void zlog_set_launched(void) {
	launched = 1;
}

size_t zlog_print_time(struct timeval *tv, char *timebuf, size_t timebuf_len) /* {{{ */
{
	struct tm t;
	size_t len;

	len = strftime(timebuf, timebuf_len, "[%d-%b-%Y %H:%M:%S", localtime_r((const time_t *) &tv->tv_sec, &t));
	if (zlog_level == ZLOG_DEBUG) {
		len += snprintf(timebuf + len, timebuf_len - len, ".%06d", (int) tv->tv_usec);
	}
	len += snprintf(timebuf + len, timebuf_len - len, "] ");
	return len;
}
/* }}} */

int zlog_set_fd(int new_fd) /* {{{ */
{
	int old_fd = zlog_fd;

	zlog_fd = new_fd;
	return old_fd;
}
/* }}} */

int zlog_set_level(int new_value) /* {{{ */
{
	int old_value = zlog_level;

	if (new_value < ZLOG_DEBUG || new_value > ZLOG_ALERT) return old_value;

	zlog_level = new_value;
	return old_value;
}
/* }}} */

void zlog(int flags, ...) /* {{{ */
{
	va_list args;
	const char *fmt;
	char tmp_buf[MAX_LINE_LENGTH];
	int tmp_buf_size = MAX_LINE_LENGTH;
	int tmp_buf_len  = 0;

	char *final_buf;
	size_t final_buf_len;

	char *fmt_buf;
	int fmt_buf_len;

	int saved_errno;

	va_start(args, flags);
	fmt = va_arg(args, const char *);

	if (external_logger) {
		va_list ap;
		va_copy(ap, args);
		fmt_buf_len = vasprintf(&fmt_buf, fmt, args);
		va_end(ap);
		external_logger(flags & ZLOG_LEVEL_MASK, fmt_buf, fmt_buf_len);
		fmt_buf_len = 0;
		free(fmt_buf);
	}

	va_end(args);

	if ((flags & ZLOG_LEVEL_MASK) < zlog_level) {
		return;
	}

	saved_errno = errno;
#ifdef HAVE_SYSLOG_H
	if (zlog_fd == ZLOG_SYSLOG /* && !fpm_globals.is_child */) {
		tmp_buf_len = snprintf(tmp_buf, tmp_buf_size, "[%s] ", level_names[flags & ZLOG_LEVEL_MASK]);
	} else
#endif
	{
		if (!fpm_globals.is_child) {
			struct timeval tv;
			gettimeofday(&tv, 0);
			tmp_buf_len = zlog_print_time(&tv, tmp_buf, tmp_buf_size);
		}
		if (zlog_level == ZLOG_DEBUG) {
			if (!fpm_globals.is_child) {
				tmp_buf_len += snprintf(tmp_buf + tmp_buf_len, tmp_buf_size - tmp_buf_len, "%s: pid %d: ", level_names[flags & ZLOG_LEVEL_MASK], getpid());
			} else {
				tmp_buf_len += snprintf(tmp_buf + tmp_buf_len, tmp_buf_size - tmp_buf_len, "%s: ", level_names[flags & ZLOG_LEVEL_MASK]);
			}
		} else {
			tmp_buf_len += snprintf(tmp_buf + tmp_buf_len, tmp_buf_size - tmp_buf_len, "%s: ", level_names[flags & ZLOG_LEVEL_MASK]);
		}
	}

	if (flags & ZLOG_HAVE_ERRNO) {
		tmp_buf_len += snprintf(tmp_buf + tmp_buf_len, tmp_buf_size - tmp_buf_len, ": %s (%d)", strerror(saved_errno), saved_errno);
	}

	fmt_buf_len   = vasprintf(&fmt_buf, fmt, args);
	final_buf_len = tmp_buf_len + fmt_buf_len + strlen("\n") + 1;

	final_buf = calloc(1, final_buf_len);
	memcpy(final_buf, tmp_buf, tmp_buf_len);
	memcpy(final_buf + tmp_buf_len, fmt_buf, fmt_buf_len);
	free(fmt_buf);

#ifdef HAVE_SYSLOG_H
	if (zlog_fd == ZLOG_SYSLOG) {
		php_syslog(syslog_priorities[zlog_level], "%s", final_buf);
	} else
#endif
	{
		final_buf[final_buf_len - 2] = '\n';
		zend_quiet_write(zlog_fd > -1 ? zlog_fd : STDERR_FILENO, final_buf, final_buf_len);
	}

	if (zlog_fd != STDERR_FILENO && zlog_fd != -1 && !launched && (flags & ZLOG_LEVEL_MASK) >= ZLOG_NOTICE) {
		zend_quiet_write(STDERR_FILENO, final_buf, final_buf_len);
	}

	free(final_buf);
}
/* }}} */
