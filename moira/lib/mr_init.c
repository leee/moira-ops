/*
 *	$Source: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/lib/mr_init.c,v $
 *	$Author: mar $
 *	$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/lib/mr_init.c,v 1.5 1990-03-17 16:36:59 mar Exp $
 *
 *	Copyright (C) 1987, 1990 by the Massachusetts Institute of Technology
 *	For copying and distribution information, please see the file
 *	<mit-copyright.h>.
 */

#ifndef lint
static char *rcsid_sms_init_c = "$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/lib/mr_init.c,v 1.5 1990-03-17 16:36:59 mar Exp $";
#endif lint

#include <mit-copyright.h>
#include "mr_private.h"

int mr_inited = 0;

/* the reference to link_against_the_moira_version_of_gdb is to make
 * sure that this is built with the proper libraries.
 */
mr_init()
{
	extern int link_against_the_moira_version_of_gdb;
	if (mr_inited) return;
	
	gdb_init();
	initialize_sms_error_table();
	initialize_krb_error_table();
	link_against_the_moira_version_of_gdb = 0;
	mr_inited=1;
}