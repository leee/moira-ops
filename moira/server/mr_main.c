/*
 *	$Source: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/server/mr_main.c,v $
 *	$Author: wesommer $
 *	$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/server/mr_main.c,v 1.4 1987-06-02 20:05:11 wesommer Exp $
 *
 *	Copyright (C) 1987 by the Massachusetts Institute of Technology
 *
 *
 * 	SMS server process.
 *
 * 	Most of this is stolen from ../gdb/tsr.c
 *
 * 	You are in a maze of twisty little finite automata, all different.
 * 	Let the reader beware.
 * 
 *	$Log: not supported by cvs2svn $
 * Revision 1.3  87/06/01  04:34:27  wesommer
 * Changed returned error code.
 * 
 * Revision 1.2  87/06/01  03:34:53  wesommer
 * Added shutdown, logging.
 * 
 * Revision 1.1  87/05/31  22:06:56  wesommer
 * Initial revision
 * 
 */

static char *rcsid_sms_main_c = "$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/server/mr_main.c,v 1.4 1987-06-02 20:05:11 wesommer Exp $";

#include <strings.h>
#include <sys/errno.h>
#include <sys/signal.h>
#include "sms_private.h"
#include "sms_server.h"

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
extern int errno;


extern char *malloc();
extern char *inet_ntoa();
extern void sms_com_err();
extern void do_client();

extern int sigshut();
void clist_append();
void oplist_append();
extern u_short ntohs();

/*
 * Main SMS server loop.
 *
 * Initialize the world, then start accepting connections and
 * making progress on current connections.
 */

/*ARGSUSED*/
int
main(argc, argv)
	int argc;
	char **argv;
{
	int status;
	
	whoami = argv[0];
	/*
	 * Error handler init.
	 */
	init_sms_err_tbl();
	init_krb_err_tbl();
	set_com_err_hook(sms_com_err);

	if (argc != 1) {
		com_err(whoami, 0, "Usage: smsd");
		exit(1);
	}		
	
	/*
	 * GDB initialization.
	 */
	gdb_init();

	/*
	 * Set up client array handler.
	 */
	nclients = 0;
	clients = (client **) malloc(0);
	
	/*
	 * Signal handlers
	 */
	
	if ((((int)signal (SIGTERM, sigshut)) < 0) ||
	    (((int)signal (SIGHUP, sigshut)) < 0)) {
		com_err(whoami, errno, "Unable to establish signal handler.");
		exit(1);
	}
	
	/*
	 * Establish template connection.
	 */
	if ((status = do_listen()) != 0) {
		com_err(whoami, status,
			"while trying to create listening connection");
		exit(1);
	}
	
	op_list = create_list_of_operations(1, listenop);
	
	(void) sprintf(buf1, "started (pid %d)", getpid());
	com_err(whoami, 0, buf1);
	com_err(whoami, 0, rcsid_sms_main_c);

	/*
	 * Run until shut down.
	 */
	while(!takedown) {		
		register int i;
		/*
		 * Block until something happens.
		 */
		op_select_any(op_list, 0, NULL, NULL, NULL, NULL);
		if (takedown) break;
#ifdef notdef
		fprintf(stderr, "    tick\n");
#endif notdef
		/*
		 * Handle any existing connections.
		 */
		for (i=0; i<nclients; i++) {
			if (OP_DONE(clients[i]->pending_op)) {
				cur_client = clients[i];
				do_client(cur_client);
				cur_client = NULL;
				if (takedown) break;
			}
		}
		/*
		 * Handle any new connections.
		 */
		if (OP_DONE(listenop)) {
			if ((status = new_connection(listenop)) != 0) {
				com_err(whoami, errno,
					"Error on listening operation.");
				/*
				 * Sleep here to prevent hosing?
				 */
			}
		}
	}
	com_err(whoami, 0, takedown);
	return 0;
}

/*
 * Set up the template connection and queue the first accept.
 */

int
do_listen()
{
	char *service = index(SMS_GDB_SERV, ':') + 1;

	listencon = create_listening_connection(service);

	if (listencon == NULL)
		return errno;

	listenop = create_operation();
	client_addrlen = sizeof(client_addr);

	start_accepting_client(listencon, listenop, &newconn,
			       (char *)&client_addr,
			       &client_addrlen, &client_tuple);
	return 0;
}

/*
 * This routine is called when a new connection comes in.
 *
 * It sets up a new client and adds it to the list of currently active clients.
 */
int
new_connection(listenop)
	OPERATION listenop;
{
	register client *cp = (client *)malloc(sizeof *cp);
	static counter = 0;
	
	/*
	 * Make sure there's been no error
	 */
	if(OP_STATUS(listenop) != OP_COMPLETE ||
	   newconn == NULL) {
		return errno;
#ifdef notdef
		exit(8); /* XXX */
#endif notdef
	}

	/*
	 * Set up the new connection and reply to the client
	 */

	cp->state = CL_STARTING;
	cp->action = CL_ACCEPT;
	cp->con = newconn;
	cp->id = counter++;
	cp->args = NULL;
	cp->clname = NULL;
	cp->reply.sms_argv = NULL;

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
	start_replying_to_client(cp->pending_op, cp->con, GDB_ACCEPTED,
				 "", "");

	cp->haddr = client_addr;
	
	/*
	 * Log new connection.
	 */
	
	(void) sprintf(buf1,
		       "New connection from %s port %d (now %d client%s)",
		       inet_ntoa(cp->haddr.sin_addr),
		       (int)ntohs(cp->haddr.sin_port),
		       nclients,
		       nclients!=1?"s":"");
	com_err(whoami, 0, buf1);
	
	/*
	 * Get ready to accept the next connection.
	 */
	reset_operation(listenop);
	client_addrlen = sizeof(client_addr);

	start_accepting_client(listencon, listenop, &newconn,
			       (char *)&client_addr,
			       &client_addrlen, &client_tuple);
	return 0;
}

/*
 * Add a new client to the known clients.
 */
void
clist_append(cp)
	client *cp;
{		
	client **clients_n;
	
	nclients++;
	clients_n = (client **)malloc
		((unsigned)(nclients * sizeof(client *)));
	bcopy((char *)clients, (char *)clients_n, (nclients-1)*sizeof(cp));
	clients_n[nclients-1] = cp;
	free((char *)clients);
	clients = clients_n;
	clients_n = NULL;
}

		
void
clist_delete(cp)
	client *cp;
{
	client **clients_n, **scpp, **dcpp; /* source and dest client */
					    /* ptr ptr */
	
	int found_it = 0;
	
	clients_n = (client **)malloc
		((unsigned)((nclients - 1)* sizeof(client *)));
	for (scpp = clients, dcpp = clients_n; scpp < clients+nclients; ) {
		if (*scpp != cp) {
			*dcpp++ = *scpp++;
		} else {
			scpp++;
			if (found_it) abort();
			found_it = 1;
		}			
	}
	--nclients;	
	free((char *)clients);
	clients = clients_n;
	clients_n = NULL;

	reset_operation(cp->pending_op);
	delete_operation(cp->pending_op);
	free((char *)cp);
}

/*
 * Add a new operation to a list of operations.
 *
 * This should be rewritten to use realloc instead, since in most
 * cases it won't have to copy the array.
 */

void
oplist_append(oplp, op)
	LIST_OF_OPERATIONS *oplp;
	OPERATION op;
{
	int count = (*oplp)->count+1;
	LIST_OF_OPERATIONS newlist = (LIST_OF_OPERATIONS)
		db_alloc(size_of_list_of_operations(count));
	bcopy((char *)(*oplp), (char *)newlist,
	      size_of_list_of_operations((*oplp)->count));
	newlist->count++;
	newlist->op[count-1] = op;
	db_free((*oplp), size_of_list_of_operations(count-1));
	(*oplp) = newlist;
}

