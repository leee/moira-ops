/* $Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/incremental/afs.c,v 1.32 1992-07-27 20:25:46 probe Exp $
 *
 * Do AFS incremental updates
 *
 * Copyright (C) 1989,1992 by the Massachusetts Institute of Technology
 * for copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include <sys/types.h>
#include <sys/file.h>
#include <strings.h>

#include <krb.h>
#include <moira.h>
#include <moira_site.h>

#include <afs/param.h>
#include <afs/cellconfig.h>
#include <afs/venus.h>
#include <afs/ptclient.h>
#include <afs/pterror.h>

#define STOP_FILE "/moira/afs/noafs"
#define file_exists(file) (access((file), F_OK) == 0)

char *whoami;

/* Main stub routines */
int do_user();
int do_list();
int do_member();
int do_filesys();
int do_quota();

/* Support stub routines */
int run_cmd();
int add_user_lists();
int get_members();
int edit_group();
int pr_try();
int check_afs();

/* libprot.a routines */
extern long pr_Initialize();
extern long pr_CreateUser();
extern long pr_CreateGroup();
extern long pr_DeleteByID();
extern long pr_ChangeEntry();
extern long pr_SetFieldsEntry();
extern long pr_AddToGroup();
extern long pr_RemoveUserFromGroup();

static char tbl_buf[1024];

main(argc, argv)
char **argv;
int argc;
{
    int beforec, afterc, i;
    char *table, **before, **after;

    for (i = getdtablesize() - 1; i > 2; i--)
      close(i);

    table = argv[1];
    beforec = atoi(argv[2]);
    before = &argv[4];
    afterc = atoi(argv[3]);
    after = &argv[4 + beforec];
    whoami = argv[0];

    setlinebuf(stdout);

    strcpy(tbl_buf, table);
    strcat(tbl_buf, " (");
    for (i = 0; i < beforec; i++) {
	if (i > 0)
	  strcat(tbl_buf, ",");
	strcat(tbl_buf, before[i]);
    }
    strcat(tbl_buf, ")->(");
    for (i = 0; i < afterc; i++) {
	if (i > 0)
	  strcat(tbl_buf, ",");
	strcat(tbl_buf, after[i]);
    }
    strcat(tbl_buf, ")");
#ifdef DEBUG
    printf("%s\n", tbl_buf);
#endif

    initialize_sms_error_table();
    initialize_krb_error_table();

    if (!strcmp(table, "users")) {
	do_user(before, beforec, after, afterc);
    } else if (!strcmp(table, "list")) {
	do_list(before, beforec, after, afterc);
    } else if (!strcmp(table, "members")) {
	do_member(before, beforec, after, afterc);
    } else if (!strcmp(table, "filesys")) {
	do_filesys(before, beforec, after, afterc);
    } else if (!strcmp(table, "quota")) {
	do_quota(before, beforec, after, afterc);
    }
    exit(0);
}


do_user(before, beforec, after, afterc)
char **before;
int beforec;
char **after;
int afterc;
{
    int astate, bstate, auid, buid, code;
    char hostname[64];
    char *av[2];

    auid = buid = astate = bstate = 0;
    if (afterc > U_STATE) astate = atoi(after[U_STATE]);
    if (beforec > U_STATE) bstate = atoi(before[U_STATE]);
    if (afterc > U_UID) auid = atoi(after[U_UID]);
    if (beforec > U_UID) buid = atoi(before[U_UID]);

    /* We consider "half-registered" users to be active */
    if (astate == 2) astate = 1;
    if (bstate == 2) bstate = 1;

    if (astate != 1 && bstate != 1)		/* inactive user */
	return;

    if (astate == bstate && auid == buid && 
	!strcmp(before[U_NAME], after[U_NAME]))
	/* No AFS related attributes have changed */
	return;

    if (astate == bstate) {
	/* Only a modify has to be done */
	code = pr_try(pr_ChangeEntry, before[U_NAME], after[U_NAME], auid, "");
	if (code) {
	    critical_alert("incremental",
			   "Couldn't change user %s (id %d) to %s (id %d): %s",
			   before[U_NAME], buid, after[U_NAME], auid,
			   error_message(code));
	}
	return;
    }
    if (bstate == 1) {
	code = pr_try(pr_DeleteByID, buid);
	if (code && code != PRNOENT) {
	    critical_alert("incremental",
			   "Couldn't delete user %s (id %d): %s",
			   before[U_NAME], buid, error_message(code));
	}
	return;
    }
    if (astate == 1) {
	code = pr_try(pr_CreateUser, after[U_NAME], &auid);
	if (code) {
	    critical_alert("incremental",
			   "Couldn't create user %s (id %d): %s",
			   after[U_NAME], auid, error_message(code));
	    return;
	}

	if (beforec) {
	    /* Reactivating a user; get his group list */
	    gethostname(hostname, sizeof(hostname));
	    code = mr_connect(hostname);
	    if (!code) code = mr_auth("afs.incr");
	    if (code) {
		critical_alert("incremental",
			       "Error contacting Moira server to retrieve grouplist of user %s: %s",
			       after[U_NAME], error_message(code));
		return;
	    }
	    av[0] = "ruser";
	    av[1] = after[U_NAME];
	    code = mr_query("get_lists_of_member", 2, av,
			    add_user_lists, after[U_NAME]);
	    if (code)
		critical_alert("incremental",
			       "Couldn't retrieve membership of user %s: %s",
			       after[U_NAME], error_message(code));
	    mr_disconnect();
	}
	return;
    }
}


do_list(before, beforec, after, afterc)
char **before;
int beforec;
char **after;
int afterc;
{
    register int agid, bgid;
    int ahide, bhide;
    long code, id;
    char hostname[64];
    char g1[PR_MAXNAMELEN], g2[PR_MAXNAMELEN];
    char *av[2];

    agid = bgid = 0;
    if (beforec > L_GID && atoi(before[L_ACTIVE]) && atoi(before[L_GROUP])) {
	bgid = atoi(before[L_GID]);
	bhide = atoi(before[L_HIDDEN]);
    }
    if (afterc > L_GID && atoi(after[L_ACTIVE]) && atoi(after[L_GROUP])) {
	agid = atoi(after[L_GID]);
	ahide = atoi(after[L_HIDDEN]);
    }

    if (agid == 0 && bgid == 0)			/* Not active groups */
	return;

    if (agid && bgid) {
	if (strcmp(after[L_NAME], before[L_NAME])) {
	    /* Only a modify is required */
	    strcpy(g1, "system:");
	    strcpy(g2, "system:");
	    strcat(g1, before[L_NAME]);
	    strcat(g2, after[L_NAME]);
	    code = pr_try(pr_ChangeEntry, g1, g2, -agid, "");
	    if (code) {
		critical_alert("incremental",
			       "Couldn't change group %s (id %d) to %s (id %d): %s",
			       before[L_NAME], -bgid, after[L_NAME], -agid,
			       error_message(code));
	    }
	}
	if (ahide != bhide) {
	    code = pr_try(pr_SetFieldsEntry, -agid, PR_SF_ALLBITS,
			  (ahide ? PRP_STATUS_ANY : PRP_GROUP_DEFAULT) >>PRIVATE_SHIFT,
			  0 /*ngroups*/, 0 /*nusers*/);
	    if (code) {
		critical_alert("incremental",
			       "Couldn't set flags of group %s: %s",
			       after[L_NAME], error_message(code));
	    }
	}
	return;
    }
    if (bgid) {
	code = pr_try(pr_DeleteByID, -bgid);
	if (code && code != PRNOENT) {
	    critical_alert("incremental",
			   "Couldn't delete group %s (id %d): %s",
			   before[L_NAME], -bgid, error_message(code));
	}
	return;
    }
    if (agid) {
	strcpy(g1, "system:");
	strcat(g1, after[L_NAME]);
	strcpy(g2, "system:administrators");
	id = -agid;
	code = pr_try(pr_CreateGroup, g1, g2, &id);
	if (code) {
	    critical_alert("incremental",
			   "Couldn't create group %s (id %d): %s",
			   after[L_NAME], id, error_message(code));
	    return;
	}
	if (ahide) {
	    code = pr_try(pr_SetFieldsEntry, -agid, PR_SF_ALLBITS,
			  (ahide ? PRP_STATUS_ANY : PRP_GROUP_DEFAULT) >>PRIVATE_SHIFT,
			  0 /*ngroups*/, 0 /*nusers*/);
	    if (code) {
		critical_alert("incremental",
			       "Couldn't set flags of group %s: %s",
			       after[L_NAME], error_message(code));
	    }
	}

	/* We need to make sure the group is properly populated */
	if (beforec < L_ACTIVE || atoi(before[L_ACTIVE]) == 0) return;

	gethostname(hostname, sizeof(hostname));
	code = mr_connect(hostname);
	if (!code) code = mr_auth("afs.incr");
	if (code) {
	    critical_alert("incremental",
			   "Error contacting Moira server to resolve %s: %s",
			   after[L_NAME], error_message(code));
	    return;
	}
	av[0] = "LIST";
	av[1] = after[L_NAME];
	get_members(2, av, after[L_NAME]);

	mr_disconnect();
	return;
    }
}


do_member(before, beforec, after, afterc)
char **before;
int beforec;
char **after;
int afterc;
{
    int code;
    char *p;
    
    if ((beforec < 4 || !atoi(before[LM_END])) &&
	(afterc < 4 || !atoi(after[LM_END])))
	return;

    if (afterc) 
	edit_group(1, after[LM_LIST], after[LM_TYPE], after[LM_MEMBER]);
    if (beforec)
	edit_group(0, before[LM_LIST], before[LM_TYPE], before[LM_MEMBER]);
}


do_filesys(before, beforec, after, afterc)
char **before;
int beforec;
char **after;
int afterc;
{
    char cmd[1024];
    
    if (beforec < FS_CREATE) {
	if (afterc < FS_CREATE || atoi(after[FS_CREATE])==0 ||
	    strcmp(after[FS_TYPE], "AFS"))
	    return;

	/* new locker creation */
	sprintf(cmd, "%s/perl -I%s %s/afs_create.pl %s %s %s %s %s %s",
		BIN_DIR, BIN_DIR, BIN_DIR,
		after[FS_NAME], after[FS_L_TYPE], after[FS_MACHINE],
		after[FS_PACK], after[FS_OWNER], after[FS_OWNERS]);
	run_cmd(cmd);
	return;
    }

    /* What do we do?  When do we use FS_CREATE?
     * 
     * TYPE change:  AFS->ERR, ERR->AFS: rename/unmount/remount
     * LOCKERTYPE change: rename/remount
     * PACK change: remount
     * LABEL change: rename/remount
     * Deletion: rename/unmount
     */
    if (afterc < FS_CREATE) {
	if (!strcmp(before[FS_TYPE], "AFS"))
	    critical_alert("incremental",
			   "Could not delete AFS filesystem %s: Operation not supported",
			   before[FS_NAME]);
	return;
    }

    if (!strcmp(after[FS_TYPE], "AFS")) {
	if (strcmp(before[FS_TYPE], "AFS")) {
	    critical_alert("incremental",
			   "Cannot convert %s to an AFS filesystem: Operation not supported",
			   after[FS_NAME]);
	} else {
	    critical_alert("incremental",
			   "Cannot change attributes of AFS filesystem %s: Operation not supported",
			   after[FS_NAME]);
	}
	return;
    }
}


do_quota(before, beforec, after, afterc)
char **before;
int beforec;
char **after;
int afterc;
{
    char cmd[1024];

    if (afterc < Q_DIRECTORY || strcmp("ANY", after[Q_TYPE]) ||
	strncmp("/afs/", after[Q_DIRECTORY], 5))
	return;

    sprintf(cmd, "%s/perl -I%s %s/afs_quota.pl %s %s",
	    BIN_DIR, BIN_DIR, BIN_DIR,
	    after[Q_DIRECTORY], after[Q_QUOTA]);
    run_cmd(cmd);
    return;
}


run_cmd(cmd)
char *cmd;
{
    int success=0, tries=0;

    check_afs();
    
    while (success == 0 && tries < 2) {
	if (tries++)
	    sleep(90);
	com_err(whoami, 0, "Executing command: %s", cmd);
	if (system(cmd) == 0)
	    success++;
    }
    if (!success)
	critical_alert("incremental", "failed command: %s", cmd);
}


add_user_lists(ac, av, user)
    int ac;
    char *av[];
    char *user;
{
    if (atoi(av[5]))
	edit_group(1, av[0], "USER", user);
}


get_members(ac, av, group)
    int ac;
    char *av[];
    char *group;
{
    int code=0;

    if (strcmp(av[0], "LIST")) {
	edit_group(1, group, av[0], av[1]);
    } else {
	code = mr_query("get_end_members_of_list", 1, &av[1],
			get_members, group);
	if (code)
	    critical_alert("incremental",
			   "Couldn't retrieve full membership of %s: %s",
			   group, error_message(code));
    }
    return code;
}


edit_group(op, group, type, member)
    int op;
    char *group;
    char *type;
    char *member;
{
    char *p = 0;
    char buf[PR_MAXNAMELEN];
    int code;
    static char local_realm[REALM_SZ+1] = "";

    /* The following KERBEROS code allows for the use of entities
     * user@foreign_cell.
     */
    if (!local_realm[0])
	krb_get_lrealm(local_realm, 1);
    if (!strcmp(type, "KERBEROS")) {
	p = index(member, '@');
	if (p && !strcasecmp(p+1, local_realm))
	    *p = 0;
    } else if (strcmp(type, "USER"))
	return;					/* invalid type */

    strcpy(buf, "system:");
    strcat(buf, group);
    code=pr_try(op ? pr_AddToGroup : pr_RemoveUserFromGroup, member, buf);
    if (code) {
	if (op==0 && code == PRNOENT) return;
	if (op==1 && code == PRIDEXIST) return;
	if (strcmp(type, "KERBEROS") || code != PRNOENT) {
	    critical_alert("incremental",
			   "Couldn't %s %s %s %s: %s",
			   op ? "add" : "remove", member,
			   op ? "to" : "from", buf,
			   error_message(code));
	}
    }
}


long pr_try(fn, a1, a2, a3, a4, a5, a6, a7, a8)
    long (*fn)();
    char *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8;
{
    static int initd=0;
    register long code;
    register int tries = 0;
#ifdef DEBUG
    char fname[64];
#endif

    check_afs();
    
    if (!initd) {
	code=pr_Initialize(1, AFSCONF_CLIENTNAME, 0);
	if (code) {
	    critical_alert("incremental", "Couldn't initialize libprot: %s",
			   error_message(code));
	    return;
	}
	initd = 1;
    } else {
	sleep(1);				/* give ptserver room */
    }

    while (code = (*fn)(a1, a2, a3, a4, a5, a6, a7, a8)) {
#ifdef DEBUG
	long t;
	t = time(0);
	if (fn == pr_AddToGroup) strcpy(fname, "pr_AddToGroup");
	else if (fn == pr_RemoveUserFromGroup)
	    strcpy(fname, "pr_RemoveUserFromGroup");
	else if (fn == pr_CreateUser) strcpy(fname, "pr_CreateUser");
	else if (fn == pr_CreateGroup) strcpy(fname, "pr_CreateGroup");
	else if (fn == pr_DeleteByID) strcpy(fname, "pr_DeleteByID");
	else if (fn == pr_ChangeEntry) strcpy(fname, "pr_ChangeEntry");
	else if (fn == pr_SetFieldsEntry) strcpy(fname, "pr_SetFieldsEntry");
	else if (fn == pr_AddToGroup) strcpy(fname, "pr_AddToGroup");
	else {
	    sprintf(fname, "pr_??? (0x%08x)", (long)fn);
	}

	com_err(whoami, code, "%s failed (try %d @%u)", fname, tries+1, t);
#endif
	if (++tries > 2)
	    return code;
	if (code == UNOQUORUM) { sleep(90); continue; }
	else { sleep(15); continue; }
    }
    return code;
}


check_afs()
{
    int i;
    
    for (i=0; file_exists(STOP_FILE); i++) {
	if (i > 30) {
	    critical_alert("incremental",
			   "AFS incremental failed (%s exists): %s",
			   STOP_FILE, tbl_buf);
	    exit(1);
	}
	sleep(60);
    }
}
