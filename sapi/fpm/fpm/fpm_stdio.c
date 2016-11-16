
	/* $Id: fpm_stdio.c,v 1.22.2.2 2008/12/13 03:32:24 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>

#include "php_syslog.h"

#include "fpm.h"
#include "fpm_children.h"
#include "fpm_events.h"
#include "fpm_sockets.h"
#include "fpm_stdio.h"
#include "zlog.h"

static int fd_stdout[2];
static int fd_stderr[2];

int fpm_stdio_init_main() /* {{{ */
{
	int fd = open("/dev/null", O_RDWR);

	if (0 > fd) {
		zlog(ZLOG_SYSERROR, "failed to init stdio: open(\"/dev/null\")");
		return -1;
	}

	if (0 > dup2(fd, STDIN_FILENO) || 0 > dup2(fd, STDOUT_FILENO)) {
		zlog(ZLOG_SYSERROR, "failed to init stdio: dup2()");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}
/* }}} */

static inline int fpm_use_error_log() {  /* {{{ */
	/*
	 * the error_log is NOT used when running in foreground
	 * and from a tty (user looking at output).
	 * So, error_log is used by
	 * - SysV init launch php-fpm as a daemon
	 * - Systemd launch php-fpm in foreground
	 */
#if HAVE_UNISTD_H
	if (fpm_global_config.daemonize || (!isatty(STDERR_FILENO) && !fpm_globals.force_stderr)) {
#else
	if (fpm_global_config.daemonize) {
#endif
		return 1;
	}
	return 0;
}

/* }}} */
int fpm_stdio_init_final() /* {{{ */
{
	if (fpm_use_error_log()) {
		/* prevent duping if logging to syslog */
		if (fpm_globals.error_log_fd > 0 && fpm_globals.error_log_fd != STDERR_FILENO) {

			/* there might be messages to stderr from other parts of the code, we need to log them all */
			if (0 > dup2(fpm_globals.error_log_fd, STDERR_FILENO)) {
				zlog(ZLOG_SYSERROR, "failed to init stdio: dup2()");
				return -1;
			}
		}
	}
	zlog_set_launched();
	return 0;
}
/* }}} */

int fpm_stdio_init_child(struct fpm_worker_pool_s *wp) /* {{{ */
{
#ifdef HAVE_SYSLOG_H
	if (fpm_globals.error_log_fd == ZLOG_SYSLOG) {
		closelog(); /* ensure to close syslog not to interrupt with PHP syslog code */
	} else
#endif

	/* Notice: child cannot use master error_log
	 * because not aware when being reopen
	 * else, should use if (!fpm_use_error_log())
	 */
	if (fpm_globals.error_log_fd > 0) {
		close(fpm_globals.error_log_fd);
	}
	fpm_globals.error_log_fd = -1;
	zlog_set_fd(-1);

	if (wp->listening_socket != STDIN_FILENO) {
		if (0 > dup2(wp->listening_socket, STDIN_FILENO)) {
			zlog(ZLOG_SYSERROR, "failed to init child stdio: dup2()");
			return -1;
		}
	}
	return 0;
}
/* }}} */

static void fpm_stdio_child_said(struct fpm_event_s *ev, short which, void *arg) /* {{{ */
{
	static const int default_read_size = 1024;
	int fd = ev->fd;
	char *buf, *bbuf, *nl;
	size_t buf_size;
	struct fpm_child_s *child;
	int is_stdout;
	struct fpm_event_s *event;
	int res;

	if (!arg) {
		return;
	}
	child = (struct fpm_child_s *)arg;
	is_stdout = (fd == child->fd_stdout);
	if (is_stdout) {
		event = &child->ev_stdout;
	} else {
		event = &child->ev_stderr;
	}

	bbuf = buf = calloc(1, default_read_size);
	buf_size   = default_read_size;

	while (1) {
		res = read(fd, buf, default_read_size - 1);
		if (res == default_read_size - 1) {
			ptrdiff_t offset = buf - bbuf;
			buf_size *= 2;
			bbuf      = realloc(bbuf, buf_size);
			buf       += (default_read_size - 1 + offset);
		} else if (res < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				/* no more data */
				break;
			} else {
				/* error or pipe is closed */
				zlog(ZLOG_SYSERROR, "unable to read what child says");

				fpm_event_del(event);

				if (is_stdout) {
					close(child->fd_stdout);
					child->fd_stdout = -1;
				} else {
					close(child->fd_stderr);
					child->fd_stderr = -1;
				}
				break;
			}
		} else {
			/* Finished reading */
			break;
		}
	}
	bbuf[buf_size -1] = '\0';
	buf = bbuf;

	/* Something to write ? */
	if (res > 0) {
		while ((nl = strchr(buf, '\n'))) {
			*nl = '\0';
			zlog(ZLOG_WARNING, "[pool %s] %s", child->wp->config->name, buf);
			buf = nl + 1;
		}
		if (bbuf == buf) {
			zlog(ZLOG_WARNING, "[pool %s] %s", child->wp->config->name, buf);
		}
	}

	free(bbuf);
}
/* }}} */

int fpm_stdio_prepare_pipes(struct fpm_child_s *child) /* {{{ */
{
	if (0 == child->wp->config->catch_workers_output) { /* not required */
		return 0;
	}

	if (0 > pipe(fd_stdout)) {
		zlog(ZLOG_SYSERROR, "failed to prepare the stdout pipe");
		return -1;
	}

	if (0 > pipe(fd_stderr)) {
		zlog(ZLOG_SYSERROR, "failed to prepare the stderr pipe");
		close(fd_stdout[0]);
		close(fd_stdout[1]);
		return -1;
	}

	if (0 > fd_set_blocked(fd_stdout[0], 0) || 0 > fd_set_blocked(fd_stderr[0], 0)) {
		zlog(ZLOG_SYSERROR, "failed to unblock pipes");
		close(fd_stdout[0]);
		close(fd_stdout[1]);
		close(fd_stderr[0]);
		close(fd_stderr[1]);
		return -1;
	}
	return 0;
}
/* }}} */

int fpm_stdio_parent_use_pipes(struct fpm_child_s *child) /* {{{ */
{
	if (0 == child->wp->config->catch_workers_output) { /* not required */
		return 0;
	}

	close(fd_stdout[1]);
	close(fd_stderr[1]);

	child->fd_stdout = fd_stdout[0];
	child->fd_stderr = fd_stderr[0];

	fpm_event_set(&child->ev_stdout, child->fd_stdout, FPM_EV_READ, fpm_stdio_child_said, child);
	fpm_event_add(&child->ev_stdout, 0);

	fpm_event_set(&child->ev_stderr, child->fd_stderr, FPM_EV_READ, fpm_stdio_child_said, child);
	fpm_event_add(&child->ev_stderr, 0);
	return 0;
}
/* }}} */

int fpm_stdio_discard_pipes(struct fpm_child_s *child) /* {{{ */
{
	if (0 == child->wp->config->catch_workers_output) { /* not required */
		return 0;
	}

	close(fd_stdout[1]);
	close(fd_stderr[1]);

	close(fd_stdout[0]);
	close(fd_stderr[0]);
	return 0;
}
/* }}} */

void fpm_stdio_child_use_pipes(struct fpm_child_s *child) /* {{{ */
{
	if (child->wp->config->catch_workers_output) {
		dup2(fd_stdout[1], STDOUT_FILENO);
		dup2(fd_stderr[1], STDERR_FILENO);
		close(fd_stdout[0]); close(fd_stdout[1]);
		close(fd_stderr[0]); close(fd_stderr[1]);
	} else {
		/* stdout of parent is always /dev/null */
		dup2(STDOUT_FILENO, STDERR_FILENO);
	}
}
/* }}} */

int fpm_stdio_open_error_log(int reopen) /* {{{ */
{
	int fd;

#ifdef HAVE_SYSLOG_H
	if (!strcasecmp(fpm_global_config.error_log, "syslog")) {
		openlog(fpm_global_config.syslog_ident, LOG_PID | LOG_CONS, fpm_global_config.syslog_facility);
		fpm_globals.error_log_fd = ZLOG_SYSLOG;
		if (fpm_use_error_log()) {
			zlog_set_fd(fpm_globals.error_log_fd);
		}
		return 0;
	}
#endif

	fd = open(fpm_global_config.error_log, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
	if (0 > fd) {
		zlog(ZLOG_SYSERROR, "failed to open error_log (%s)", fpm_global_config.error_log);
		return -1;
	}

	if (reopen) {
		if (fpm_use_error_log()) {
			dup2(fd, STDERR_FILENO);
		}

		dup2(fd, fpm_globals.error_log_fd);
		close(fd);
		fd = fpm_globals.error_log_fd; /* for FD_CLOSEXEC to work */
	} else {
		fpm_globals.error_log_fd = fd;
		if (fpm_use_error_log()) {
			zlog_set_fd(fpm_globals.error_log_fd);
		}
	}
	if (0 > fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC)) {
		zlog(ZLOG_WARNING, "failed to change attribute of error_log");
	}
	return 0;
}
/* }}} */

