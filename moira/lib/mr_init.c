/*
 *	$Source: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/lib/mr_init.c,v $
 *	$Author: danw $
 *	$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/lib/mr_init.c,v 1.8 1998-01-07 17:13:30 danw Exp $
 *
 *	Copyright (C) 1987, 1990 by the Massachusetts Institute of Technology
 *	For copying and distribution information, please see the file
 *	<mit-copyright.h>.
 */

#ifndef lint
static char *rcsid_mr_init_c = "$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/lib/mr_init.c,v 1.8 1998-01-07 17:13:30 danw Exp $";
#endif

#include <mit-copyright.h>
#include "mr_private.h"

int mr_inited = 0;

/* the reference to link_against_the_moira_version_of_gdb is to make
 * sure that this is built with the proper libraries.
 */
mr_init(void)
{
  extern int link_against_the_moira_version_of_gdb;
  if (mr_inited)
    return;

  gdb_init();
  initialize_sms_error_table();
  initialize_krb_error_table();
  link_against_the_moira_version_of_gdb = 0;
  mr_inited = 1;
}
