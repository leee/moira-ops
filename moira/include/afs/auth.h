/*
 * auth.h:
 * This file is automatically generated; please do not edit it.
 */
/* Including auth.p.h at beginning of auth.h file. */

#ifndef __AUTH_AFS_INCL_
#define	__AUTH_AFS_INCL_    1

		/* no ticket good for longer than 30 days */
#define MAXKTCTICKETLIFETIME (30*24*3600)
#define MINKTCTICKETLEN	      32
#define	MAXKTCTICKETLEN	      344
#define	MAXKTCNAMELEN	      64	/* name & inst should be 256 */
#define MAXKTCREALMLEN	      64	/* should be 256 */
#define KTC_TIME_UNCERTAINTY (15*60)	/* max skew separating machines' clocks */

struct ktc_encryptionKey {
    char data[8];
};

struct ktc_token {
    long startTime;
    long endTime;
    struct ktc_encryptionKey sessionKey;
    short kvno;
    int ticketLen;
    char ticket[MAXKTCTICKETLEN];
};

struct ktc_principal {
    char name[MAXKTCNAMELEN];
    char instance[MAXKTCNAMELEN];
    char cell[MAXKTCREALMLEN];
};

#if 0
#define	KTC_ERROR	1	/* an unexpected error was encountered */
#define	KTC_TOOBIG	2	/* a buffer was too small for the response */
#define	KTC_INVAL	3	/* an invalid argument was passed in */
#define	KTC_NOENT	4	/* no such entry */
#endif

#endif __AUTH_AFS_INCL_

/* End of prolog file auth.p.h. */

#define KTC_ERROR                                (11862784L)
#define KTC_TOOBIG                               (11862785L)
#define KTC_INVAL                                (11862786L)
#define KTC_NOENT                                (11862787L)
extern void initialize_ktc_error_table ();
#define ERROR_TABLE_BASE_ktc (11862784L)

/* for compatibility with older versions... */
#define init_ktc_err_tbl initialize_ktc_error_table
#define ktc_err_base ERROR_TABLE_BASE_ktc