/*
 *	$Source: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/include/dcm.h,v $
 *	$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/include/dcm.h,v 1.2 1989-08-24 23:23:03 mar Exp $
 */

#define UPCALL_CONT	0
#define UPCALL_STOP	1

#define HARD_FAIL(x)	(((x) != 0 ) && ((x) != SMS_NO_CHANGE))
#define SOFT_FAIL(x)	(((x) == SMS_NO_MEM) || ((x) == SMS_CANT_CONNECT) || ((x) == SMS_CCONFIG) || ((x) == SMS_DEADLOCK))

#define SMS_DIR		"/u1/sms"
#define LOCK_DIR	"/u1/sms/dcm/locks"
#define UMASK		0007

extern char *malloc(), *calloc(), *realloc();
extern int errno;
extern char *whoami;

#define DBG_PLAIN	01
#define DBG_VERBOSE	02
#define DBG_TRACE	04

#define     strrel(s,op,t)      (strcmp((s), (t)) op 0)
#define     strnrel(s,op,t,n)     (strncmp((s), (t), (n)) op 0)

#define streq(a,b) (strcmp(a,b) == 0)
	
#ifndef BUFSIZ
#define BUFSIZ 1024
#endif
#ifndef NULL
#define NULL (char *) 0
#endif

/* globals */
extern int dbg;			/* utils.c: set by main() */

#define CHECK_UPCALL_ARGS(num, upcall_name)\
  if (argc != num) {\
      fprintf(stderr, "%s upcall with %d arguments instead of %d\n",\
	      upcall_name, argc, num);\
      return UPCALL_STOP;\
  }


struct service {
    char *service;
    int interval;
    char *target;
    char *script;
    int dfgen;
    int dfcheck;
    char *type;
    int enable;
    int inprogress;
    int harderror;
    char *errmsg;
};

struct svrhost {
    char *service;
    char *machine;
    int enable;
    int override;
    int success;
    int inprogress;
    int hosterror;
    char *errmsg;
    int lasttry;
    int lastsuccess;
    int value1;
    int value2;
    char *value3;
};


/*--> utils.c <--*/
extern void init_sms();
extern void dcm_com_err_hook();
extern void leave();
extern void scream();
extern char *strsave();
extern long file_last_mod_time();
extern long current_time();
#define file_exists(file) (access((file), F_OK) == 0)

/*--> update.c <--*/
extern void perform_update();
extern int maybe_lock_update();
extern void unlock();
