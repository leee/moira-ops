/*
 *	$Source: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/lib/mr_param.c,v $
 *	$Author: danw $
 *	$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/lib/mr_param.c,v 1.8 1997-01-29 23:24:19 danw Exp $
 *
 *	Copyright (C) 1987, 1990 by the Massachusetts Institute of Technology
 *	For copying and distribution information, please see the file
 *	<mit-copyright.h>.
 *
 */

#ifndef lint
static char *rcsid_sms_param_c = "$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/lib/mr_param.c,v 1.8 1997-01-29 23:24:19 danw Exp $";
#endif

#include <mit-copyright.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "mr_private.h"
#include <string.h>
#include <stdlib.h>

/*
 * GDB operations to send and recieve RPC requests and replies.
 */

/*
 * This doesn't get called until after the actual buffered write completes.
 * In a non-preflattening version of this, this would then queue the
 * write of the next bunch of data.
 */

/*ARGSUSED*/
mr_cont_send(op, hcon, arg)
    OPERATION op;
    HALF_CONNECTION hcon;
    struct mr_params *arg;
{
    op->result = OP_SUCCESS;
    free(arg->mr_flattened);
    arg->mr_flattened = NULL;
    
    return OP_COMPLETE;
}

mr_start_send(op, hcon, arg)
    OPERATION op;
    HALF_CONNECTION hcon;
    register struct mr_params *arg;
{
    int i, len;
    unsigned int mr_size;
    int *argl;
    char *buf, *bp;
	
    /*
     * This should probably be split into several routines.
     * It could also probably be made more efficient (punting most
     * of the argument marshalling stuff) by doing I/O directly
     * from the strings.  Anyone for a scatter/gather mr_send_data?
     *
     * that would look a lot like the uio stuff in the kernel..  hmm.
     */
	
    /*
     * Marshall the entire data right now..
     * We are sending the version number,
     * total request size, request number, 
     * argument count, and then each argument.
     * At least for now, each argument is a string, which is
     * sent as a count of bytes followed by the bytes
     * (including the trailing '\0'), padded
     * to a 32-bit boundary.
     */

    mr_size = 4 * sizeof(int32);

    argl = (int *)malloc((unsigned)(sizeof(int) * arg->mr_argc));

    /*
     * For each argument, figure out how much space is needed.
     */
	
    for (i = 0; i < arg->mr_argc; ++i) {
	if (arg->mr_argl)
	    argl[i] = len = arg->mr_argl[i];
	else
	    argl[i] = len = strlen(arg->mr_argv[i]) + 1;
	mr_size += sizeof(int32) + len;
	/* Round up to next 32-bit boundary.. */
	mr_size = sizeof(int32) * howmany(mr_size, sizeof(int32));
    }
	
    arg->mr_flattened = buf = malloc(mr_size);

    memset(arg->mr_flattened, 0, mr_size);
	
    arg->mr_size = mr_size;
	
    ((int32 *)buf)[0] = htonl(mr_size);
    ((int32 *)buf)[1] = htonl(arg->mr_version_no);
    ((int32 *)buf)[2] = htonl(arg->mr_procno);
    ((int32 *)buf)[3] = htonl(arg->mr_argc);

    /*
     * bp is a pointer into the point in the buffer to put
     * the next argument.
     */
	
    bp = (char *)(((int32 *)buf) + 4);
	
    for (i = 0; i<arg->mr_argc; ++i) {
	len = argl[i];
	*((int32 *)bp) = htonl(len);
	bp += sizeof(int32);
	memcpy(bp, arg->mr_argv[i], len);
	bp += sizeof(int32) * howmany(len, sizeof(int32));
    }
    op->fcn.cont = mr_cont_send;
    arg->mr_size = mr_size;

    free(argl);
    
    if (gdb_send_data(hcon, arg->mr_flattened, mr_size) == OP_COMPLETE)
	return mr_cont_send(op, hcon, arg);
    else return OP_RUNNING;
}	
	
/*ARGSUSED*/
mr_cont_recv(op, hcon, argp)
    OPERATION op;
    HALF_CONNECTION hcon;
    mr_params **argp;
{
    int done = FALSE;
    char *cp;
    int *ip;
    int i;
    register mr_params *arg = *argp;
						       
    while (!done) {
	switch (arg->mr_state) {
	case S_RECV_START:
	    arg->mr_state = S_RECV_DATA;
	    if (gdb_receive_data(hcon, (caddr_t)&arg->mr_size,
				 sizeof(int32)) == OP_COMPLETE)
		continue;
	    done = TRUE;
	    break;
	case S_RECV_DATA:
	    fflush(stdout);
	    /* Should validate that length is reasonable */
	    arg->mr_size = ntohl(arg->mr_size);
	    if (arg->mr_size > 65536) {
		return OP_CANCELLED;
	    }
	    arg->mr_flattened = malloc(arg->mr_size);
	    arg->mr_state = S_DECODE_DATA;
	    memcpy(arg->mr_flattened, (caddr_t)&arg->mr_size, sizeof(int32));
			
	    if (gdb_receive_data(hcon,
				 arg->mr_flattened + sizeof(int32),
				 arg->mr_size - sizeof(int32))
		== OP_COMPLETE)
		continue;
	    done = TRUE;
	    break;
	case S_DECODE_DATA:
	    cp = arg->mr_flattened;
	    ip = (int *) cp;
	    /* we already got the overall length.. */
	    for(i=1; i <4; i++) ip[i] = ntohl(ip[i]);
	    arg->mr_version_no = ip[1];
	    if (arg->mr_version_no != MR_VERSION_1 &&
		arg->mr_version_no != MR_VERSION_2)
		arg->mr_status = MR_VERSION_MISMATCH;
	    else arg->mr_status = ip[2];
	    arg->mr_argc = ip[3];
	    cp += 4 * sizeof(int);
	    arg->mr_argv=(char **)malloc(arg->mr_argc *sizeof(char **));
	    arg->mr_argl=(int *)malloc(arg->mr_argc *sizeof(int *));
			
	    for (i = 0; i<arg->mr_argc; ++i) {
		u_short nlen = ntohl(* (int *) cp);
		cp += sizeof (int32);
		if (cp + nlen > arg->mr_flattened + arg->mr_size) {
		    free(arg->mr_flattened);
		    arg->mr_flattened = NULL;
		    return OP_CANCELLED;
		}		    
		arg->mr_argv[i] = (char *)malloc(nlen);
		memcpy(arg->mr_argv[i], cp, nlen);
		arg->mr_argl[i]=nlen;
		cp += sizeof(int32) * howmany(nlen, sizeof(int32));
	    }
	    free(arg->mr_flattened);
	    arg->mr_flattened = NULL;
	    return OP_COMPLETE;
	}
    }
    return OP_RUNNING;
}
			

mr_start_recv(op, hcon, argp)
    OPERATION op;
    HALF_CONNECTION hcon;
    struct mr_params **argp;
{
    register mr_params *arg = *argp;
    if (!arg) {
	*argp = arg = (mr_params *)malloc(sizeof(mr_params));
	arg->mr_argl = NULL;
	arg->mr_argv = NULL;
	arg->mr_flattened = NULL;
    }
    arg->mr_state = S_RECV_START;
    op->fcn.cont = mr_cont_recv;
    return mr_cont_recv(op, hcon, argp);
}

mr_destroy_reply(reply)
    mr_params *reply;
{
    int i;
    if (reply) {
	if (reply->mr_argl)
	    free(reply->mr_argl);
	reply->mr_argl = NULL;
	if (reply->mr_flattened)
	    free(reply->mr_flattened);
	reply->mr_flattened = NULL;
	if (reply->mr_argv) {
	    for (i=0; i<reply->mr_argc; i++) {
		if (reply->mr_argv[i])
		    free (reply->mr_argv[i]);
		reply->mr_argv[i] = NULL;
	    }
	    free(reply->mr_argv);
	}
	reply->mr_argv = NULL;
	free(reply);
    }
}
