/*
 * Verify that all MOIRA updates are successful
 *
 * Copyright 1988, 1991 by the Massachusetts Institute of Technology. 
 * For copying and distribution information, see the file "mit-copyright.h". 
 *
 * $Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/clients/mrcheck/mrcheck.c,v 1.8 1992-03-10 16:40:26 mar Exp $
 * $Author: mar $
 */

#ifndef lint
static char *rcsid_chsh_c = "$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/clients/mrcheck/mrcheck.c,v 1.8 1992-03-10 16:40:26 mar Exp $";
#endif	lint

#include <stdio.h>
#include <moira.h>
#include <moira_site.h>
#include "mit-copyright.h"
#include <sys/time.h>
#include <strings.h>

char *malloc();

static int count = 0;
static char *whoami;
static struct timeval now;


struct service {
    char name[17];
    char update_int[10];
};


/* turn an ascii string containing the number of seconds sinc the epoch
 * into an ascii string containing the corresponding time & date
 */

char *atot(itime)
char *itime;
{
    int time;
    char *ct, *ctime();

    time = atoi(itime);
    ct = ctime(&time);
    ct[24] = 0;
    return(&ct[4]);
}


/* Decide if the server has an error or not.  Also, save the name and
 * interval for later use.
 */

process_server(argc, argv, sq)
int argc;
char **argv;
struct save_queue *sq;
{
    struct service *s;

    if (atoi(argv[SVC_ENABLE])) {
	s = (struct service *)malloc(sizeof(struct service));
	strcpy(s->name, argv[SVC_SERVICE]);
	strcpy(s->update_int, argv[SVC_INTERVAL]);
	sq_save_data(sq, s);
    }

    if (atoi(argv[SVC_HARDERROR]) && atoi(argv[SVC_ENABLE])) {
	disp_svc(argv, "Error needs to be reset\n");
    } else if (atoi(argv[SVC_HARDERROR]) ||
	       (!atoi(argv[SVC_ENABLE]) && atoi(argv[SVC_DFCHECK]))) {
	disp_svc(argv, "Should this be enabled?\n");
    } else if (atoi(argv[SVC_ENABLE]) &&
	       60 * atoi(argv[SVC_INTERVAL]) + 86400 + atoi(argv[SVC_DFCHECK])
	       < now.tv_sec) {
	disp_svc(argv, "Service has not been updated\n");
    }
    return(MR_CONT);
}


/* Format the information about a service. */

disp_svc(argv, msg)
char **argv;
char *msg;
{
    char *tmp = strsave(atot(argv[SVC_DFGEN]));

    printf("Service %s Interval %s %s/%s/%s %s\n",
	   argv[SVC_SERVICE], argv[SVC_INTERVAL],
	   atoi(argv[SVC_ENABLE]) ? "Enabled" : "Disabled",
	   atoi(argv[SVC_INPROGRESS]) ? "InProgress" : "Idle",
	   atoi(argv[SVC_HARDERROR]) ? "Error" : "NoError",
	   atoi(argv[SVC_HARDERROR]) ? argv[SVC_ERRMSG] : "");
    printf("  Generated %s; Last checked %s\n", tmp, atot(argv[SVC_DFCHECK]));
    printf("  Last modified by %s at %s with %s\n",
	   argv[SVC_MODBY], argv[SVC_MODTIME], argv[SVC_MODWITH]);
    printf(" * %s\n", msg);
    count++;
    free(tmp);
}


/* Decide if the host has an error or not.
 */

process_host(argc, argv, sq)
int argc;
char **argv;
struct save_queue *sq;
{
    struct service *s = NULL;
    struct save_queue *sq1;
    char *update_int = NULL;

    for (sq1 = sq->q_next; sq1 != sq; sq1 = sq1->q_next)
      if ((s = (struct service *)sq1->q_data) &&
	  !strcmp(s->name, argv[SH_SERVICE]))
	break;
    if (s && !strcmp(s->name, argv[SH_SERVICE]))
      update_int = s->update_int;

    if (atoi(argv[SH_HOSTERROR]) && atoi(argv[SH_ENABLE])) {
	disp_sh(argv, "Error needs to be reset\n");
    } else if (atoi(argv[SH_HOSTERROR]) ||
	       (!atoi(argv[SH_ENABLE]) && atoi(argv[SH_LASTTRY]))) {
	disp_sh(argv, "Should this be enabled?\n");
    } else if (atoi(argv[SH_ENABLE]) && update_int &&
	       60 * atoi(update_int) + 86400 + atoi(argv[SH_LASTSUCCESS])
	       < now.tv_sec) {
	disp_sh(argv, "Host has not been updated\n");
    }
    return(MR_CONT);
}


/* Format the information about a host. */

disp_sh(argv, msg)
char **argv;
char *msg;
{
    char *tmp = strsave(atot(argv[SH_LASTTRY]));

    printf("Host %s:%s %s/%s/%s/%s/%s %s\n",
	   argv[SH_SERVICE], argv[SH_MACHINE],
	   atoi(argv[SH_ENABLE]) ? "Enabled" : "Disabled",
	   atoi(argv[SH_SUCCESS]) ? "Success" : "Failure",
	   atoi(argv[SH_INPROGRESS]) ? "InProgress" : "Idle",
	   atoi(argv[SH_OVERRIDE]) ? "Override" : "Normal",
	   atoi(argv[SH_HOSTERROR]) ? "Error" : "NoError",
	   atoi(argv[SH_HOSTERROR]) ? argv[SH_ERRMSG] : "");
    printf("  Last try %s; Last success %s\n", tmp, atot(argv[SH_LASTSUCCESS]));
    printf("  Last modified by %s at %s with %s\n",
	   argv[SH_MODBY], argv[SH_MODTIME], argv[SH_MODWITH]);
    printf(" * %s\n", msg);
    count++;
    free(tmp);
}




main(argc, argv)
int argc;
char *argv[];
{
    char *args[2], buf[BUFSIZ], *motd;
    struct save_queue *sq;
    int status;
    int scream();
    int auth_required = 1;

    if ((whoami = rindex(argv[0], '/')) == NULL)
	whoami = argv[0];
    else
	whoami++;

    if (argc == 2 && !strcmp(argv[1], "-noauth"))
      auth_required = 0;
    else if (argc > 1)
      usage();

    status = mr_connect(NULL);
    if (status) {
	(void) sprintf(buf, "\nConnection to the Moira server failed.");
	goto punt;
    }

    status = mr_motd(&motd);
    if (status) {
        com_err(whoami, status, " unable to check server status");
	exit(2);
    }
    if (motd) {
	fprintf(stderr, "The Moira server is currently unavailable:\n%s\n",
		motd);
	mr_disconnect();
	exit(2);
    }
    status = mr_auth("mrcheck");
    if (status && auth_required) {
	(void) sprintf(buf, "\nAuthorization failure -- run \"kinit\" \
and try again");
	goto punt;
    }

    gettimeofday(&now, 0);
    sq = sq_create();

    /* Check services first */
    args[0] = "*";
    if ((status = mr_query("get_server_info", 1, args,
			   process_server, (char *)sq)) &&
	status != MR_NO_MATCH)
      com_err(whoami, status, " while getting servers");

    args[1] = "*";
    if ((status = mr_query("get_server_host_info", 2, args,
			   process_host, (char *)sq)) &&
	status != MR_NO_MATCH)
      com_err(whoami, status, " while getting servers");

    if (!count)
      printf("Nothing has failed at this time\n");
    else
      printf("%d thing%s ha%s failed at this time\n", count,
	     count == 1 ? "" : "s", count == 1 ? "s" : "ve");

    mr_disconnect();
    exit(0);

punt:
    com_err(whoami, status, buf);
    mr_disconnect();
    exit(1);
}


scream()
{
    com_err(whoami, 0,
	    "Update to Moira returned a value -- programmer botch.\n");
    mr_disconnect();
    exit(1);
}

usage()
{
    fprintf(stderr, "Usage: %s [-noauth]\n", whoami);
    exit(1);
}