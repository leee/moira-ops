/* $Id: ticket.c,v 1.20 1998-02-15 17:49:30 danw Exp $
 *
 * Copyright (C) 1988-1998 by the Massachusetts Institute of Technology.
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include <mit-copyright.h>
#include <moira.h>

#include <sys/stat.h>

#include <stdio.h>
#include <string.h>

#include <krb.h>
#include <update.h>

RCSID("$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/update/ticket.c,v 1.20 1998-02-15 17:49:30 danw Exp $");

static char *srvtab = KEYFILE; /* default == /etc/srvtab */
static char realm[REALM_SZ];
static char master[INST_SZ] = "sms";
static char service[ANAME_SZ] = "rcmd";
des_cblock session;

static int get_mr_tgt(void);

int get_mr_update_ticket(char *host, KTEXT ticket)
{
  int code, pass;
  char phost[BUFSIZ];
  CREDENTIALS cr;

  pass = 1;
  if (krb_get_lrealm(realm, 1))
    strcpy(realm, KRB_REALM);
  strcpy(phost, (char *)krb_get_phost(host));

try_it:
  code = krb_mk_req(ticket, service, phost, realm, (long)0);
  if (code)
    {
      if (pass == 1)
	{
	  /* maybe we're taking too long? */
	  if ((code = get_mr_tgt()))
	    {
	      com_err(whoami, code, "can't get Kerberos TGT");
	      return code;
	    }
	  pass++;
	  goto try_it;
	}
      code += ERROR_TABLE_BASE_krb;
      com_err(whoami, code, "in krb_mk_req");
    }
  else
    {
      code = krb_get_cred(service, phost, realm, &cr);
      if (code)
	code += ERROR_TABLE_BASE_krb;
      memcpy(session, cr.session, sizeof(session));
    }
  return code;
}

static int get_mr_tgt(void)
{
  int code;
  char linst[INST_SZ], kinst[INST_SZ];

  linst[0] = '\0';
  strcpy(kinst, "krbtgt");
  code = krb_get_svc_in_tkt(master, linst, realm, kinst, realm,
			    DEFAULT_TKT_LIFE, srvtab);
  if (!code)
    return 0;
  else
    return code + ERROR_TABLE_BASE_krb;
}
