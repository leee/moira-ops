/*
 *	$Source: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/update/exec_002.c,v $
 *	$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/update/exec_002.c,v 1.10 1992-08-25 14:43:12 mar Exp $
 */
/*  (c) Copyright 1988 by the Massachusetts Institute of Technology. */
/*  For copying and distribution information, please see the file */
/*  <mit-copyright.h>. */

#ifndef lint
static char *rcsid_exec_002_c = "$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/update/exec_002.c,v 1.10 1992-08-25 14:43:12 mar Exp $";
#endif	lint

#include <mit-copyright.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <gdb.h>
#include <moira.h>
#include "update.h"

extern CONNECTION conn;
extern int code, errno, uid;
extern char *whoami;


int
exec_002(str)
    char *str;
{
    union wait waitb;
    int n, pid, mask;

    if (config_lookup("noexec")) {
	code = EPERM;
	code = send_object(conn, (char *)&code, INTEGER_T);
	com_err(whoami, code, "Not allowed to execute");
	return;
    }
    str += 8;
    while (*str == ' ')
	str++;
    mask = sigblock(sigmask(SIGCHLD));
    pid = fork();
    switch (pid) {
    case -1:
	n = errno;
	sigsetmask(mask);
	log_priority = log_ERROR;
	com_err(whoami, errno, ": can't fork to run install script");
	code = send_object(conn, (char *)&n, INTEGER_T);
	if (code)
	    exit(1);
	return;
    case 0:
	if (setuid(uid) < 0) {
	    com_err(whoami, errno, "Unable to setuid to %d\n", uid);
	    exit(1);
	}
	sigsetmask(mask);
	execlp(str, str, (char *)NULL);
	n = errno;
	sigsetmask(mask);
	log_priority = log_ERROR;
	com_err(whoami, n, ": %s", str);
	(void) send_object(conn, (char *)&n, INTEGER_T);
	exit(1);
    default:
	do {
	    n = wait(&waitb);
	} while (n != -1 && n != pid);
	sigsetmask(mask);
	if (waitb.w_status) {
	    n = waitb.w_retcode + ERROR_TABLE_BASE_sms;
	    log_priority = log_ERROR;
	    com_err(whoami, n, " child exited with status %d", waitb.w_retcode);
	    code = send_object(conn, (char *)&n, INTEGER_T);
	    if (code) {
		exit(1);
	    }
	} else {
	    code = send_ok();
	    if (code)
	      exit(1);
	}
    }
}