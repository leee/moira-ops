/*
 *	$Source: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/server/mr_server.h,v $
 *	$Author: wesommer $
 *	$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/server/mr_server.h,v 1.4 1987-06-21 16:42:07 wesommer Exp $
 *
 *	Copyright (C) 1987 by the Massachusetts Institute of Technology
 *
 *	$Log: not supported by cvs2svn $
 * Revision 1.3  87/06/03  16:07:59  wesommer
 * Fixes for lint.
 * 
 * Revision 1.2  87/06/01  03:35:10  wesommer
 * added stuff for authentication.
 * 
 */

#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "sms_proto.h"

typedef struct returned_tuples {
	struct returned_tuples *next;
	OPERATION op;
} returned_tuples;

typedef struct _client {
	OPERATION pending_op;
	CONNECTION con;
	int state;
	int action;
	sms_params *args, reply;
	int id;			/* Unique id of client */
	struct sockaddr_in haddr;
	char *clname;
	returned_tuples *first, *last;
} client;

/*
 * States
 */

#define CL_DEAD 0
#define CL_STARTING 1

/*
 * Actions.
 */

#define CL_ACCEPT 0
#define CL_RECEIVE 1
#define CL_SEND 2

