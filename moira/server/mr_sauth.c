/* $Id: mr_sauth.c,v 1.28.4.1 2002-08-14 21:30:48 zacheiss Exp $
 *
 * Handle server side of authentication
 *
 * Copyright (C) 1987-1998 by the Massachusetts Institute of Technology
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 *
 */

#include <mit-copyright.h>
#include "mr_server.h"

#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <stdlib.h>
#include <string.h>

RCSID("$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/server/mr_sauth.c,v 1.28.4.1 2002-08-14 21:30:48 zacheiss Exp $");

extern char *whoami, *host;
extern int proxy_acl;
extern krb5_context context;

static int set_client(client *cl, char *kname,
		      char *name, char *inst, char *realm);

typedef struct _replay_cache {
  KTEXT_ST auth;
  time_t expires;
  struct _replay_cache *next;
} replay_cache;

replay_cache *rcache = NULL;

/*
 * Handle a MOIRA_AUTH RPC request.
 *
 * argv[0] is a kerberos authenticator.  Decompose it, and if
 * successful, store the name the user authenticated to in
 * cl->cl_name.
 */

void do_auth(client *cl)
{
  KTEXT_ST auth;
  AUTH_DAT ad;
  int status;
  replay_cache *rc, *rcnew;
  time_t now;

  auth.length = cl->req.mr_argl[0];
  memcpy(auth.dat, cl->req.mr_argv[0], auth.length);
  auth.mbz = 0;

  if ((status = krb_rd_req(&auth, MOIRA_SNAME, host,
			   cl->haddr.sin_addr.s_addr, &ad, "")))
    {
      status += ERROR_TABLE_BASE_krb;
      client_reply(cl, status);
      com_err(whoami, status, " (authentication failed)");
      return;
    }

  if (!rcache)
    {
      rcache = xmalloc(sizeof(replay_cache));
      memset(rcache, 0, sizeof(replay_cache));
    }

  /* scan replay cache */
  for (rc = rcache->next; rc; rc = rc->next)
    {
      if (auth.length == rc->auth.length &&
	  !memcmp(&(auth.dat), &(rc->auth.dat), auth.length))
	{
	  com_err(whoami, 0,
		  "Authenticator replay from %s using authenticator for %s",
		  inet_ntoa(cl->haddr.sin_addr),
		  kname_unparse(ad.pname, ad.pinst, ad.prealm));
	  com_err(whoami, KE_RD_AP_REPEAT, " (authentication failed)");
	  client_reply(cl, KE_RD_AP_REPEAT);
	  return;
	}
    }

  /* add new entry */
  time(&now);
  rcnew = xmalloc(sizeof(replay_cache));
  memcpy(&(rcnew->auth), &auth, sizeof(KTEXT_ST));
  rcnew->expires = now + 2 * CLOCK_SKEW;
  rcnew->next = rcache->next;
  rcache->next = rcnew;

  /* clean cache */
  for (rc = rcnew; rc->next; )
    {
      if (rc->next->expires < now)
	{
	  rcnew = rc->next;
	  rc->next = rc->next->next;
	  free(rcnew);
	}
      else
	rc = rc->next;
    }

  status = set_client(cl, kname_unparse(ad.pname, ad.pinst, ad.prealm),
		      ad.pname, ad.pinst, ad.prealm);

  strncpy(cl->entity, cl->req.mr_argv[1], sizeof(cl->entity) - 1);
  cl->entity[sizeof(cl->entity) - 1] = 0;

  memset(&ad, 0, sizeof(ad));	/* Clean up session key, etc. */

  com_err(whoami, 0, "Auth to %s using %s, uid %d cid %d",
	  cl->clname, cl->entity, cl->users_id, cl->client_id);

  if (status != MR_SUCCESS || cl->users_id != 0)
    client_reply(cl, status);
  else
    client_reply(cl, MR_USER_AUTH);
}

void do_proxy(client *cl)
{
  char name[ANAME_SZ], inst[INST_SZ], realm[REALM_SZ];
  char kname[MAX_K_NAME_SZ];

  if (cl->proxy_id)
    {
      com_err(whoami, MR_PERM, "Cannot re-proxy");
      client_reply(cl, MR_PERM);
      return;
    }

  if (kname_parse(name, inst, realm, cl->req.mr_argv[0]) != KSUCCESS)
    {
      com_err(whoami, KE_KNAME_FMT, "while parsing proxy name %s",
	      cl->req.mr_argv);
      client_reply(cl, KE_KNAME_FMT);
      return;
    }

  if (!*realm)
    {
      strcpy(realm, krb_realm);
      sprintf(kname, "%s@%s", cl->req.mr_argv[0], realm);
    }
  else
    strcpy(kname, cl->req.mr_argv[0]);
    
  if (find_member("LIST", proxy_acl, cl))
    {
      cl->proxy_id = cl->client_id;
      set_client(cl, kname, name, inst, realm);
      com_err(whoami, 0, "Proxy authentication as %s (uid %d cid %d) via %s",
	      kname, cl->users_id, cl->client_id, cl->req.mr_argv[1]);
      client_reply(cl, MR_SUCCESS);
    }
  else
    {
      com_err(whoami, MR_PERM, "Proxy authentication denied");
      client_reply(cl, MR_PERM);
    }
}

static int set_client(client *cl, char *kname,
		      char *name, char *inst, char *realm)
{
  int ok;

  strncpy(cl->clname, kname, sizeof(cl->clname));
  cl->clname[sizeof(cl->clname) - 1] = '\0';

  if ((!inst || inst[0] == 0) && !strcmp(realm, krb_realm))
    ok = 1;
  else
    ok = 0;
  /* this is in a separate function because it accesses the database */
  return set_krb_mapping(cl->clname, name, ok, &cl->client_id, &cl->users_id);
}

void do_krb5_auth(client *cl)
{
  krb5_data auth;
  krb5_auth_context auth_con = NULL;
  krb5_principal server = NULL, client = NULL;
  krb5_ticket *ticket;
  char name[ANAME_SZ], inst[INST_SZ], realm[REALM_SZ];
  int status;

  ticket = NULL;

  status = krb5_auth_con_init(context, &auth_con);
  if (status)
    {
      client_reply(cl, status);
      com_err(whoami, status, "(krb5 auth context init failed)");
      goto out;
    }

  status = krb5_sname_to_principal(context, host, MOIRA_SNAME, 
				    KRB5_NT_SRV_HST, &server);
  if (status)
    {
      client_reply(cl, status);
      com_err(whoami, status, "(krb5_sname_to_principal failed)");
      goto out;
    }

  auth.length = cl->req.mr_argl[0];
  auth.data = cl->req.mr_argv[0];

  status = krb5_rd_req(context, &auth_con, &auth, server, NULL, NULL,
			&ticket);
  if (status)
    {
      client_reply(cl, status);
      com_err(whoami, status, " (krb5 authentication failed)");
      goto out;
    }

  status = krb5_copy_principal(context, ticket->enc_part2->client, &client);
  if (status)
    {
      client_reply(cl, status);
      com_err(whoami, status, " (krb5_copy_principal failed)");
      goto out;
    }

  /* Always convert to krb4 style principal name for now. */
  status = krb5_524_conv_principal(context, client, name, inst, realm);
  if (status)
    {
      client_reply(cl, status);
      com_err(whoami, status, " (krb5_524_conv_principal failed)");
      goto out;
    }
  status = set_client(cl, kname_unparse(name, inst, realm), name, inst,
		      realm);

  strncpy(cl->entity, cl->req.mr_argv[1], sizeof(cl->entity) - 1);
  cl->entity[sizeof(cl->entity) - 1] = 0;

  memset(&ticket, 0, sizeof(ticket));	/* Clean up session key, etc. */

  com_err(whoami, 0, "krb5 auth to %s using %s, uid %d cid %d",
	  cl->clname, cl->entity, cl->users_id, cl->client_id);

  if (status != MR_SUCCESS || cl->users_id != 0)
    client_reply(cl, status);
  else
    client_reply(cl, MR_USER_AUTH);

 out:
  if (client)
    krb5_free_principal(context, client);
  if (server)
    krb5_free_principal(context, server);
  if (ticket)
    krb5_free_ticket(context, ticket);
  if (auth.data)
    krb5_free_data_contents(context, &auth);
  if (auth_con)
    krb5_auth_con_free(context, auth_con);
  return;
}
