/* $Id: mr_main.c,v 1.39 1998-02-05 22:51:42 danw Exp $
 *
 * Moira server process.
 *
 * Copyright (C) 1987-1998 by the Massachusetts Institute of Technology
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 *
 */

#include <mit-copyright.h>
#include "mr_server.h"

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

RCSID("$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/server/mr_main.c,v 1.39 1998-02-05 22:51:42 danw Exp $");

extern char *krb_get_lrealm(char *, int);

extern CONNECTION newconn, listencon;

extern int nclients;
extern client **clients, *cur_client;

extern OPERATION listenop;
extern LIST_OF_OPERATIONS op_list;

extern struct sockaddr_in client_addr;
extern int client_addrlen;
extern TUPLE client_tuple;

extern char *whoami;
extern char buf1[BUFSIZ];
extern char *takedown;
extern FILE *journal;

extern time_t now;

char *host;

void reapchild(int x);
void godormant(int x);
void gowakeup(int x);
int do_listen(char *port);
void do_reset_listen(void);
void clist_append(client *cp);
void oplist_append(LIST_OF_OPERATIONS *oplp, OPERATION op);
void oplist_delete(LIST_OF_OPERATIONS oplp, OPERATION op);
void mr_setup_signals(void);
int new_connection(void);

/*
 * Main Moira server loop.
 *
 * Initialize the world, then start accepting connections and
 * making progress on current connections.
 */

int main(int argc, char **argv)
{
  int status, i;
  time_t tardy;
  char *port, *p;
  extern char *database;
  struct stat stbuf;
  struct utsname uts;

  whoami = argv[0];
  /*
   * Error handler init.
   */
  initialize_sms_error_table();
  initialize_krb_error_table();
  initialize_gdss_error_table();
  set_com_err_hook(mr_com_err);
  setvbuf(stderr, NULL, _IOLBF, BUFSIZ);

  port = strchr(MOIRA_SERVER, ':') + 1;

  for (i = 1; i < argc; i++)
    {
      if (!strcmp(argv[i], "-db") && i + 1 < argc)
	{
	  database = argv[i + 1];
	  i++;
	}
      else if (!strcmp(argv[i], "-p") && i + 1 < argc)
	{
	  port = argv[i + 1];
	  i++;
	}
      else
	{
	  com_err(whoami, 0, "Usage: moirad [-db database][-p port]");
	  exit(1);
	}
    }

  /*
   * GDB initialization.
   */
  if (gdb_init() != 0)
    {
      com_err(whoami, 0, "GDB initialization failed.");
      exit(1);
    }
  gdb_debug(0); /* this can be patched, if necessary, to enable */
		/* GDB level debugging .. */
  krb_realm = malloc(REALM_SZ);
  krb_get_lrealm(krb_realm, 1);

  /*
   * Database initialization.  Only init if database should be open.
   */

  if (stat(MOIRA_MOTD_FILE, &stbuf) != 0)
    {
      if ((status = mr_open_database()))
	{
	  com_err(whoami, status, " when trying to open database.");
	  exit(1);
	}
      sanity_check_database();
    }
  else
    {
      dormant = ASLEEP;
      com_err(whoami, 0, "sleeping, not opening database");
    }

  sanity_check_queries();

  /*
   * Get moira server hostname for authentication
   */
  if (uname(&uts) < 0)
    {
      com_err(whoami, errno, "Unable to get local hostname");
      exit(1);
    }
  host = canonicalize_hostname(strdup(uts.nodename));
  for (p = host; *p && *p != '.'; p++)
    {
      if (isupper(*p))
	*p = tolower(*p);
    }
  *p = '\0';

  /*
   * Set up client array handler.
   */
  nclients = 0;
  clients = malloc(0);

  mr_setup_signals();

  journal = fopen(JOURNAL, "a");
  if (!journal)
    {
      com_err(whoami, errno, " while opening journal file");
      exit(1);
    }

  /*
   * Establish template connection.
   */
  if ((status = do_listen(port)))
    {
      com_err(whoami, status, " while trying to create listening connection");
      exit(1);
    }

  op_list = create_list_of_operations(1, listenop);

  com_err(whoami, 0, "started (pid %d)", getpid());
  com_err(whoami, 0, rcsid);
  if (dormant != ASLEEP)
    send_zgram("MOIRA", "server started");
  else
    send_zgram("MOIRA", "server started, but database closed");

  /*
   * Run until shut down.
   */
  while (!takedown)
    {
      int i;
      /*
       * Block until something happens.
       */
      if (dormant == SLEEPY)
	{
	  mr_close_database();
	  com_err(whoami, 0, "database closed");
	  mr_setup_signals();
	  send_zgram("MOIRA", "database closed");
	  dormant = ASLEEP;
	}
      else if (dormant == GROGGY)
	{
	  mr_open_database();
	  com_err(whoami, 0, "database open");
	  mr_setup_signals();
	  send_zgram("MOIRA", "database open again");
	  dormant = AWAKE;
	}

      errno = 0;
      status = op_select_any(op_list, 0, NULL, NULL, NULL, NULL);
      if (status == -1)
	{
	  if (errno != EINTR)
	    com_err(whoami, errno, " error from op_select");
	  if (!inc_running || now - inc_started > INC_TIMEOUT)
	    next_incremental();
	  continue;
	}
      else if (status != -2)
	{
	  com_err(whoami, 0, " wrong return from op_select_any");
	  continue;
	}
      if (takedown)
	break;
      time(&now);
      if (!inc_running || now - inc_started > INC_TIMEOUT)
	next_incremental();

      /*
       * Handle any new connections; this comes first so
       * errno isn't tromped on.
       */
      if (OP_DONE(listenop))
	{
	  if (OP_STATUS(listenop) == OP_CANCELLED)
	    {
	      if (errno == EWOULDBLOCK)
		do_reset_listen();
	      else
		{
		  static int count = 0;
		  com_err(whoami, errno, " error (%d) on listen", count);
		  if (count++ > 10)
		    exit(1);
		}
	    }
	  else if ((status = new_connection()))
	    {
	      com_err(whoami, errno, " Error on listening operation.");
	      /*
	       * Sleep here to prevent hosing?
	       */
	    }
	  /* if the new connection is our only connection,
	   * and the server is supposed to be down, then go
	   * down now.
	   */
	  if ((dormant == AWAKE) && (nclients == 1) &&
	      (stat(MOIRA_MOTD_FILE, &stbuf) == 0))
	    {
	      com_err(whoami, 0, "motd file exists, slumbertime");
	      dormant = SLEEPY;
	    }
	  /* on new connection, if we are no longer supposed
	   * to be down, then wake up.
	   */
	  if ((dormant == ASLEEP) && (stat(MOIRA_MOTD_FILE, &stbuf) == -1) &&
	      (errno == ENOENT))
	    {
	      com_err(whoami, 0, "motd file no longer exists, waking up");
	      dormant = GROGGY;
	    }
	}
      /*
       * Handle any existing connections.
       */
      tardy = now - 30 * 60;

      for (i = 0; i < nclients; i++)
	{
	  cur_client = clients[i];
	  if (OP_DONE(clients[i]->pending_op))
	    {
	      cur_client->last_time_used = now;
	      do_client(cur_client);
	    }
	  else if (clients[i]->last_time_used < tardy)
	    {
	      com_err(whoami, 0, "Shutting down connection due to inactivity");
	      shutdown(cur_client->con->in.fd, 2);
	    }
	  cur_client = NULL;
	  if (takedown)
	    break;
	}
    }
  com_err(whoami, 0, "%s", takedown);
  if (dormant != ASLEEP)
    mr_close_database();
  send_zgram("MOIRA", takedown);
  return 0;
}

/*
 * Set up the template connection and queue the first accept.
 */

int do_listen(char *port)
{
  listencon = create_listening_connection(port);

  if (!listencon)
    return errno;

  listenop = create_operation();
  client_addrlen = sizeof(client_addr);

  start_accepting_client(listencon, listenop, &newconn, (char *)&client_addr,
			 &client_addrlen, &client_tuple);
  return 0;
}


void do_reset_listen(void)
{
  client_addrlen = sizeof(client_addr);
  start_accepting_client(listencon, listenop, &newconn, (char *)&client_addr,
			 &client_addrlen, &client_tuple);
}

/*
 * This routine is called when a new connection comes in.
 *
 * It sets up a new client and adds it to the list of currently active clients.
 */
int new_connection(void)
{
  client *cp;
  static counter = 0;

  /*
   * Make sure there's been no error
   */
  if (OP_STATUS(listenop) != OP_COMPLETE)
    return errno;

  if (!newconn)
    return MR_NOT_CONNECTED;

  /*
   * Set up the new connection and reply to the client
   */
  cp = malloc(sizeof(client));
  memset(cp, 0, sizeof(*cp));
  cp->action = CL_ACCEPT;
  cp->con = newconn;
  cp->id = counter++;
  cp->args = NULL;
  cp->clname[0] = '\0';
  cp->reply.mr_argv = NULL;
  cp->first = NULL;
  cp->last = NULL;
  cp->last_time_used = now;
  newconn = NULL;

  cp->pending_op = create_operation();
  reset_operation(cp->pending_op);
  oplist_append(&op_list, cp->pending_op);
  cur_client = cp;

  /*
   * Add a new client to the array..
   */
  clist_append(cp);

  /*
   * Let him know we heard him.
   */
  start_replying_to_client(cp->pending_op, cp->con, GDB_ACCEPTED, "", "");

  cp->haddr = client_addr;

  /*
   * Log new connection.
   */
  com_err(whoami, 0, "New connection from %s port %d (now %d client%s)",
	  inet_ntoa(cp->haddr.sin_addr), (int)ntohs(cp->haddr.sin_port),
	  nclients, nclients != 1 ? "s" : "");

  /*
   * Get ready to accept the next connection.
   */
  reset_operation(listenop);
  client_addrlen = sizeof(client_addr);

  start_accepting_client(listencon, listenop, &newconn, (char *)&client_addr,
			 &client_addrlen, &client_tuple);
  return 0;
}

/*
 * Add a new client to the known clients.
 */
void clist_append(client *cp)
{
  client **clients_n;

  nclients++;
  clients_n = malloc(nclients * sizeof(client *));
  memcpy(clients_n, clients, (nclients - 1) * sizeof(cp));
  clients_n[nclients - 1] = cp;
  free(clients);
  clients = clients_n;
  clients_n = NULL;
}


void clist_delete(client *cp)
{
  client **clients_n, **scpp, **dcpp; /* source and dest client ptr ptr */

  int found_it = 0;

  clients_n = malloc((nclients - 1) * sizeof(client *));
  for (scpp = clients, dcpp = clients_n; scpp < clients + nclients; )
    {
      if (*scpp != cp)
	*dcpp++ = *scpp++;
      else
	{
	  scpp++;
	  if (found_it)
	    abort();
	  found_it = 1;
	}
    }
  --nclients;
  free(clients);
  clients = clients_n;
  clients_n = NULL;
  oplist_delete(op_list, cp->pending_op);
  reset_operation(cp->pending_op);
  delete_operation(cp->pending_op);
  sever_connection(cp->con);
  free(cp);
}

/*
 * Add a new operation to a list of operations.
 *
 * This should be rewritten to use realloc instead, since in most
 * cases it won't have to copy the array.
 */

void oplist_append(LIST_OF_OPERATIONS *oplp, OPERATION op)
{
  int count = (*oplp)->count + 1;
  LIST_OF_OPERATIONS newlist = (LIST_OF_OPERATIONS)
    db_alloc(size_of_list_of_operations(count));
  memcpy(newlist, *oplp, size_of_list_of_operations((*oplp)->count));
  newlist->count++;
  newlist->op[count - 1] = op;
  db_free(*oplp, size_of_list_of_operations(count - 1));
  *oplp = newlist;
}

void oplist_delete(LIST_OF_OPERATIONS oplp, OPERATION op)
{
  OPERATION *s;
  int c;

  for (s = oplp->op, c = oplp->count; c; --c, ++s)
    {
      if (*s == op)
	{
	  while (c > 0)
	    {
	      *s = *(s + 1);
	      ++s;
	      --c;
	    }
	  oplp->count--;
	  return;
	}
    }
  abort();
}


void reapchild(int x)
{
  int status, pid;

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
      if (pid == inc_pid)
	inc_running = 0;
      if (!takedown && (WTERMSIG(status) != 0 || WEXITSTATUS(status) != 0))
	com_err(whoami, 0, "%d: child exits with signal %d status %d",
		pid, WTERMSIG(status), WEXITSTATUS(status));
    }
}


void godormant(int x)
{
  switch (dormant)
    {
    case AWAKE:
    case GROGGY:
      com_err(whoami, 0, "requested to go dormant");
      break;
    case ASLEEP:
      com_err(whoami, 0, "already asleep");
      break;
    case SLEEPY:
      break;
    }
  dormant = SLEEPY;
}


void gowakeup(int x)
{
  switch (dormant)
    {
    case ASLEEP:
    case SLEEPY:
      com_err(whoami, 0, "Good morning");
      break;
    case AWAKE:
      com_err(whoami, 0, "already awake");
      break;
    case GROGGY:
      break;
    }
  dormant = GROGGY;
}


void mr_setup_signals(void)
{
  struct sigaction action;

  action.sa_flags = 0;
  sigemptyset(&action.sa_mask);

  /* There should probably be a few more of these. */

  action.sa_handler = sigshut;
  if ((sigaction(SIGTERM, &action, NULL) < 0) ||
      (sigaction(SIGINT, &action, NULL) < 0) ||
      (sigaction(SIGHUP, &action, NULL) < 0))
    {
      com_err(whoami, errno, "Unable to establish signal handlers.");
      exit(1);
    }

  action.sa_handler = godormant;
  if (sigaction(SIGUSR1, &action, NULL) < 0)
    {
      com_err(whoami, errno, "Unable to establish signal handlers.");
      exit(1);
    }

  action.sa_handler = gowakeup;
  if (sigaction(SIGUSR2, &action, NULL) < 0)
    {
      com_err(whoami, errno, "Unable to establish signal handlers.");
      exit(1);
    }

  action.sa_handler = reapchild;
  sigaddset(&action.sa_mask, SIGCHLD);
  if (sigaction(SIGCHLD, &action, NULL) < 0)
    {
      com_err(whoami, errno, "Unable to establish signal handlers.");
      exit(1);
    }
}
