/*
 *	$Source: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/server/qvalidate.c,v $
 *	$Author: delgado $
 *	$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/server/qvalidate.c,v 1.1 1994-11-03 13:50:15 delgado Exp $
 *
 *	Copyright (C) 1987 by the Massachusetts Institute of Technology
 *	For copying and distribution information, please see the file
 *	<mit-copyright.h>.
 *
 */

#ifndef lint
static char *rcsid_qsupport_dc = "$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/server/qvalidate.c,v 1.1 1994-11-03 13:50:15 delgado Exp $";
#endif lint

#include <mit-copyright.h>
#include <unistd.h>
#include "query.h"
#include "mr_server.h"
#include <ctype.h>
EXEC SQL INCLUDE sqlca;
EXEC SQL INCLUDE sqlda;
#include "qrtn.h"

extern char *whoami;
extern int ingres_errno, mr_errcode;

EXEC SQL BEGIN DECLARE SECTION;
extern char stmt_buf[];
EXEC SQL END DECLARE SECTION;

EXEC SQL WHENEVER SQLERROR CALL ingerr;

#ifdef _DEBUG_MALLOC_INC
#undef index
#define dbg_index(str1,c)             DBindex(__FILE__, __LINE__, str1, c)
#else
#define dbg_index index
#endif

/* Validation Routines */

validate_row(q, argv, v)
    register struct query *q;
    char *argv[];
    register struct validate *v;
{
    EXEC SQL BEGIN DECLARE SECTION;
    char *name;
    char qual[128];
    int rowcount;
    EXEC SQL END DECLARE SECTION;

    /* build where clause */
    build_qual(v->qual, v->argc, argv, qual);

    if (log_flags & LOG_VALID)
	/* tell the logfile what we're doing */
	com_err(whoami, 0, "validating row: %s", qual);

    /* look for the record */
    sprintf(stmt_buf,"SELECT COUNT (*) FROM %s WHERE %s",q->rtable,qual);
    EXEC SQL PREPARE stmt INTO :SQLDA USING NAMES FROM :stmt_buf;
    if(sqlca.sqlcode)
      return(MR_INTERNAL);
    EXEC SQL DECLARE csr126 CURSOR FOR stmt;
    EXEC SQL OPEN csr126;
    EXEC SQL FETCH csr126 USING DESCRIPTOR :SQLDA;
    EXEC SQL CLOSE csr126;
    rowcount = *(int *)SQLDA->sqlvar[0].sqldata;

    if (ingres_errno) return(mr_errcode);
    if (rowcount == 0) return(MR_NO_MATCH);
    if (rowcount > 1) return(MR_NOT_UNIQUE);
    return(MR_EXISTS);
}

validate_fields(q, argv, vo, n)
    struct query *q;
    register char *argv[];
    register struct valobj *vo;
    register int n;
{
    register int status;

    while (--n >= 0) {
	switch (vo->type) {
	case V_NAME:
	    if (log_flags & LOG_VALID)
		com_err(whoami, 0, "validating %s in %s: %s",
		    vo->namefield, vo->table, argv[vo->index]);
	    status = validate_name(argv, vo);
	    break;

	case V_ID:
	    if (log_flags & LOG_VALID)
		com_err(whoami, 0, "validating %s in %s: %s",
		    vo->idfield, vo->table, argv[vo->index]);
	    status = validate_id(q, argv, vo);
	    break;

	case V_DATE:
	    if (log_flags & LOG_VALID)
		com_err(whoami, 0, "validating date: %s", argv[vo->index]);
	    status = validate_date(argv, vo);
	    break;

	case V_TYPE:
	    if (log_flags & LOG_VALID)
		com_err(whoami, 0, "validating %s type: %s",
		    vo->table, argv[vo->index]);
	    status = validate_type(argv, vo);
	    break;

	case V_TYPEDATA:
	    if (log_flags & LOG_VALID)
		com_err(whoami, 0, "validating typed data (%s): %s",
		    argv[vo->index - 1], argv[vo->index]);
	    status = validate_typedata(q, argv, vo);
	    break;

	case V_RENAME:
	    if (log_flags & LOG_VALID)
	        com_err(whoami, 0, "validating rename %s in %s",
			argv[vo->index], vo->table);
	    status = validate_rename(argv, vo);
	    break;

	case V_CHAR:
	    if (log_flags & LOG_VALID)
	      com_err(whoami, 0, "validating chars: %s", argv[vo->index]);
	    status = validate_chars(argv[vo->index]);
	    break;

	case V_SORT:
	    status = MR_EXISTS;
	    break;

	case V_LOCK:
	    status = lock_table(vo);
	    break;

        case V_RLOCK:
            status = readlock_table(vo);
            break;
        case V_WILD:
	    status = convert_wildcards(argv[vo->index]);
	    break;

	case V_UPWILD:
	    status = convert_wildcards_uppercase(argv[vo->index]);
	    break;

	}

	if (status != MR_EXISTS){
           com_err(whoami,0,"validation failed type=%ld, code=%ld\n",vo->type, status);
           return(status);
        }
	vo++;
    }

    if (ingres_errno) return(mr_errcode);
    return(MR_SUCCESS);
}


/* validate_chars: verify that there are no illegal characters in
 * the string.  Legal characters are printing chars other than
 * ", *, ?, \, [ and ].
 */
static int illegalchars[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* ^@ - ^O */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* ^P - ^_ */
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, /* SPACE - / */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, /* 0 - ? */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* : - O */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, /* P - _ */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* ` - o */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, /* p - ^? */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

validate_chars(s)
register char *s;
{
    while (*s)
      if (illegalchars[*s++])
	return(MR_BAD_CHAR);
    return(MR_EXISTS);
}


validate_id(q, argv, vo)
    struct query *q;
    char *argv[];
    register struct valobj *vo;
{
    EXEC SQL BEGIN DECLARE SECTION;
    char *name, *tbl, *namefield, *idfield;
    int id, rowcount;
    EXEC SQL END DECLARE SECTION;
    int status;
    register char *c;

    name = argv[vo->index];
    tbl = vo->table;
    namefield = vo->namefield;
    idfield = vo->idfield;

    if ((!strcmp(tbl, "users") && !strcmp(namefield, "login")) ||
	!strcmp(tbl, "machine") ||
	!strcmp(tbl, "subnet") ||
	!strcmp(tbl, "filesys") ||
	!strcmp(tbl, "list") ||
	!strcmp(tbl, "cluster") ||
	!strcmp(tbl, "strings")) {
	if (!strcmp(tbl, "machine") || !strcmp(tbl, "subnet"))
	  for (c = name; *c; c++) if (islower(*c)) *c = toupper(*c);
	status = name_to_id(name, tbl, &id);
	if (status == 0) {
	    *(int *)argv[vo->index] = id;
	    return(MR_EXISTS);
	} else if (status == MR_NO_MATCH && !strcmp(tbl, "strings") &&
		   (q->type == APPEND || q->type == UPDATE)) {
	    id=add_string(name);
	    cache_entry(name, "STRING", id);
	    *(int *)argv[vo->index] = id;
	    return(MR_EXISTS);
	} else if (status == MR_NO_MATCH || status == MR_NOT_UNIQUE)
	  return(vo->error);
	else
	  return(status);
    }

    if (!strcmp(namefield, "uid")) {
	sprintf(stmt_buf,"SELECT %s FROM %s WHERE %s = %s",idfield,tbl,namefield,name);
    } else {
	sprintf(stmt_buf,"SELECT %s FROM %s WHERE %s = '%s'",idfield,tbl,namefield,name);
    }
    EXEC SQL PREPARE stmt INTO :SQLDA USING NAMES FROM :stmt_buf;
    if(sqlca.sqlcode)
      return(MR_INTERNAL);
    EXEC SQL DECLARE csr127 CURSOR FOR stmt;
    EXEC SQL OPEN csr127;
    rowcount=0;
    EXEC SQL FETCH csr127 USING DESCRIPTOR :SQLDA;
    if(sqlca.sqlcode == 0) {
	rowcount++;
	EXEC SQL FETCH csr127 USING DESCRIPTOR :SQLDA;
	if(sqlca.sqlcode == 0) rowcount++;
    }
    EXEC SQL CLOSE csr127;
    if (ingres_errno)
      return(mr_errcode);

    if (rowcount != 1) return(vo->error);
    bcopy(SQLDA->sqlvar[0].sqldata,argv[vo->index],sizeof(int));
    return(MR_EXISTS);
}

validate_name(argv, vo)
    char *argv[];
    register struct valobj *vo;
{
    EXEC SQL BEGIN DECLARE SECTION;
    char *name, *tbl, *namefield;
    int rowcount;
    EXEC SQL END DECLARE SECTION;
    register char *c;

    name = argv[vo->index];
    tbl = vo->table;
    namefield = vo->namefield;
    if (!strcmp(tbl, "servers") && !strcmp(namefield, "name")) {
	for (c = name; *c; c++)
	  if (islower(*c))
	    *c = toupper(*c);
    }
    sprintf(stmt_buf,"SELECT DISTINCT COUNT(*) FROM %s WHERE %s.%s = '%s'",
	    tbl,tbl,namefield,name);
    EXEC SQL PREPARE stmt INTO :SQLDA USING NAMES FROM :stmt_buf;
    if(sqlca.sqlcode)
      return(MR_INTERNAL);
    EXEC SQL DECLARE csr128 CURSOR FOR stmt;
    EXEC SQL OPEN csr128;
    EXEC SQL FETCH csr128 USING DESCRIPTOR :SQLDA;
    rowcount = *(int *)SQLDA->sqlvar[0].sqldata;
    EXEC SQL CLOSE csr128;

    if (ingres_errno) return(mr_errcode);
    return ((rowcount == 1) ? MR_EXISTS : vo->error);
}

validate_date(argv, vo)
    char *argv[];
    struct valobj *vo;
{
    EXEC SQL BEGIN DECLARE SECTION;
    char *idate;
    double dd;
    int errorno;
    EXEC SQL END DECLARE SECTION;

    idate = argv[vo->index];
    EXEC SQL SELECT interval('years',date(:idate)-date('today')) INTO :dd;

    if (sqlca.sqlcode != 0 || dd > 5.0) return(MR_DATE);
    return(MR_EXISTS);
}


validate_rename(argv, vo)
char *argv[];
struct valobj *vo;
{
    EXEC SQL BEGIN DECLARE SECTION;
    char *name, *tbl, *namefield, *idfield;
    int id;
    EXEC SQL END DECLARE SECTION;
    int status;
    register char *c;

    c = name = argv[vo->index];
    while (*c)
      if (illegalchars[*c++])
	return(MR_BAD_CHAR);
    tbl = vo->table;
    /* minor kludge to upcasify machine names */
    if (!strcmp(tbl, "machine"))
	for (c = name; *c; c++) if (islower(*c)) *c = toupper(*c);
    namefield = vo->namefield;
    idfield = vo->idfield;
    id = -1;
    if (idfield == 0) {
	if (!strcmp(argv[vo->index], argv[vo->index - 1]))
	  return(MR_EXISTS);
	sprintf(stmt_buf,"SELECT %s FROM %s WHERE %s = LEFT('%s',SIZE(%s))",
		namefield,tbl,namefield,name,namefield);
	EXEC SQL PREPARE stmt INTO :SQLDA USING NAMES FROM :stmt_buf;
	if(sqlca.sqlcode)
	  return(MR_INTERNAL);
        EXEC SQL DECLARE csr129 CURSOR FOR stmt;
	EXEC SQL OPEN csr129;
        EXEC SQL FETCH csr129 USING DESCRIPTOR :SQLDA;
	if(sqlca.sqlcode == 0) id=1; else id=0;
	EXEC SQL CLOSE csr129;

	if (ingres_errno) return(mr_errcode);
	if (id)
	  return(vo->error);
	else
	  return(MR_EXISTS);
    }
    status = name_to_id(name, tbl, &id);
    if (status == MR_NO_MATCH || id == *(int *)argv[vo->index - 1])
      return(MR_EXISTS);
    else
      return(vo->error);
}


validate_type(argv, vo)
    char *argv[];
    register struct valobj *vo;
{
    EXEC SQL BEGIN DECLARE SECTION;
    char *typename;
    char *val;
    int cnt;
    EXEC SQL END DECLARE SECTION;
    register char *c;

    typename = vo->table;
    c = val = argv[vo->index];
    while (*c) {
	if (illegalchars[*c++])
	  return(MR_BAD_CHAR);
    }

    /* uppercase type fields */
    for (c = val; *c; c++) if (islower(*c)) *c = toupper(*c);

    EXEC SQL SELECT COUNT(trans) INTO :cnt FROM alias
      WHERE name = :typename AND type='TYPE' AND trans = :val;
    if (ingres_errno) return(mr_errcode);
    return (cnt ? MR_EXISTS : vo->error);
}

/* validate member or type-specific data field */

validate_typedata(q, argv, vo)
    register struct query *q;
    register char *argv[];
    register struct valobj *vo;
{
    EXEC SQL BEGIN DECLARE SECTION;
    char *name;
    char *field_type;
    char data_type[129];
    int id;
    EXEC SQL END DECLARE SECTION;
    int status;
    register char *c;

    /* get named object */
    name = argv[vo->index];

    /* get field type string (known to be at index-1) */
    field_type = argv[vo->index-1];

    /* get corresponding data type associated with field type name */
    EXEC SQL SELECT trans INTO :data_type FROM alias
      WHERE name = :field_type AND type='TYPEDATA';
    if (ingres_errno) return(mr_errcode);
    if (sqlca.sqlerrd[2] != 1) return(MR_TYPE);

    /* now retrieve the record id corresponding to the named object */
    if (dbg_index(data_type, ' '))
	*dbg_index(data_type, ' ') = 0;
    if (!strcmp(data_type, "user")) {
	/* USER */
	if (dbg_index(name, '@'))
	  return(MR_USER);
	status = name_to_id(name, data_type, &id);
	if (status && (status == MR_NO_MATCH || status == MR_NOT_UNIQUE))
	  return(MR_USER);
	if (status) return(status);
    } else if (!strcmp(data_type, "list")) {
	/* LIST */
	status = name_to_id(name, data_type, &id);
	if (status && status == MR_NOT_UNIQUE)
	  return(MR_LIST);
	if (status == MR_NO_MATCH) {
	    /* if idfield is non-zero, then if argv[0] matches the string
	     * that we're trying to resolve, we should get the value of
	     * numvalues.[idfield] for the id.
	     */
	    if (vo->idfield && !strcmp(argv[0], argv[vo->index])) {
		set_next_object_id(q->validate->object_id, q->rtable, 0);
		name = vo->idfield;
		EXEC SQL REPEATED SELECT value INTO :id FROM numvalues
		  WHERE name = :name;
		if (sqlca.sqlerrd[2] != 1) return(MR_LIST);
	    } else
	      return(MR_LIST);
	} else if (status) return(status);
    } else if (!strcmp(data_type, "machine")) {
	/* MACHINE */
	for (c = name; *c; c++) if (islower(*c)) *c = toupper(*c);
	status = name_to_id(name, data_type, &id);
	if (status && (status == MR_NO_MATCH || status == MR_NOT_UNIQUE))
	  return(MR_MACHINE);
	if (status) return(status);
    } else if (!strcmp(data_type, "string")) {
	/* STRING */
	status = name_to_id(name, data_type, &id);
	if (status && status == MR_NOT_UNIQUE)
	  return(MR_STRING);
	if (status == MR_NO_MATCH) {
	    if (q->type != APPEND && q->type != UPDATE) return(MR_STRING);
	    id=add_string(name);
	    cache_entry(name, "STRING", id);
	} else if (status) return(status);
    } else if (!strcmp(data_type, "none")) {
	id = 0;
    } else {
	return(MR_TYPE);
    }

    /* now set value in argv */
    *(int *)argv[vo->index] = id;

    return (MR_EXISTS);
}


/* Lock the table named by the validation object */

lock_table(vo)
struct valobj *vo;
{
    sprintf(stmt_buf,"UPDATE %s SET modtime='now' WHERE %s.%s = 0",
	    vo->table,vo->table,vo->idfield);
    EXEC SQL EXECUTE IMMEDIATE :stmt_buf;
    if (sqlca.sqlcode == 100){
        fprintf(stderr,"readlock_table: no matching rows found for %s\n",
                stmt_buf);
        return(MR_INTERNAL);
    }
    if (ingres_errno) 
        return(mr_errcode);
    if (sqlca.sqlerrd[2] != 1)
      return(vo->error);
    else
      return(MR_EXISTS);
}

/*
 * Get a read lock on the table by accessing the magic lock
 * record.  Certain tables are constructed so that they contain
 * an id field whose value is zero and a modtime field.  We 
 * manipulate the modtime field of the id 0 record to effect 
 * locking of the table
 */

readlock_table(vo)
    struct valobj *vo;
{
    EXEC SQL BEGIN DECLARE SECTION;
        int id;
        char buf[256];
        char *tbl, *idfield;
    EXEC SQL END DECLARE SECTION;

    tbl=vo->table;
    idfield=vo->idfield;
    sprintf(buf,"SELECT %s FROM %s WHERE %s.%s = 0",
        vo->idfield, vo->table, vo->table, vo->idfield);
    EXEC SQL PREPARE stmt FROM :buf;
    EXEC SQL DESCRIBE stmt INTO SQLDA;
    EXEC SQL DECLARE rcsr CURSOR FOR stmt;
    EXEC SQL OPEN rcsr;
    EXEC SQL FETCH rcsr USING DESCRIPTOR :SQLDA;
    /* Check for no matching rows found - this is 
     * flagged as an internal error since the table should
     * have a magic lock record.
     */
    if (sqlca.sqlcode == 100){
        EXEC SQL CLOSE rcsr;
        com_err(whoami,0,"readlock_table: no matching rows found for %s\n",
                buf);
        return(MR_INTERNAL);
    }
    EXEC SQL CLOSE rcsr;
    if (ingres_errno)
        return(mr_errcode);
    if (sqlca.sqlcode)
        return(vo->error);
    return(MR_EXISTS);  /* validate_fields expects us to return
                           * this value if everything went okay
                           */
}

/* Check the database at startup time.  For now this just resets the
 * inprogress flags that the DCM uses.
 */

sanity_check_database()
{
}


/* Dynamic SQL support routines */
MR_SQLDA_T *mr_alloc_SQLDA()
{
    MR_SQLDA_T *it;
    short *null_indicators;
    register int j;

    if((it=(MR_SQLDA_T *)malloc(sizeof(MR_SQLDA_T)))==NULL) {
	com_err(whoami, MR_NO_MEM, "setting up SQLDA");
	exit(1);
    }

    if((null_indicators=(short *)calloc(QMAXARGS,sizeof(short)))==NULL) {
	com_err(whoami, MR_NO_MEM, "setting up SQLDA null indicators");
	exit(1);
    }

    for(j=0; j<QMAXARGS; j++) {
	if((it->sqlvar[j].sqldata=(char *)malloc(sizeof(short)+ARGLEN))==NULL) {
	    com_err(whoami, MR_NO_MEM, "setting up SQLDA variables");
	    exit(1);
	}
	it->sqlvar[j].sqllen=ARGLEN;
	it->sqlvar[j].sqlind=null_indicators+j;
	null_indicators[j]=0;
    }
    it->sqln=QMAXARGS;
    return it;
}


/* Use this after FETCH USING DESCRIPTOR one or more
 * result columns may contain NULLs.  This routine is
 * not currently needed, since db/schema creates all
 * columns with a NOT NULL WITH DEFAULT clause.
 *
 * This is currently dead flesh, since no Moira columns
 * allow null values; all use default values.
 */
mr_fix_nulls_in_SQLDA(da)
    MR_SQLDA_T *da;
{
    register IISQLVAR *var;
    register int j;
    int *intp;

    for(j=0, var=da->sqlvar; j<da->sqld; j++, var++) {
	switch(var->sqltype) {
	  case -IISQ_CHA_TYPE:
	    if(*var->sqlind)
	      *var->sqldata='\0';
	    break;
	  case -IISQ_INT_TYPE:
	    if(*var->sqlind) {
		intp=(int *)var->sqldata;
		*intp=0;
	    }
	    break;
	}
    }
}

/* Convert normal Unix-style wildcards to SQL voodoo */
convert_wildcards(arg)
    char *arg;
{
    static char buffer[ARGLEN];
    register char *s, *d;

    for(d=buffer,s=arg;*s;s++) {
	switch(*s) {
	  case '*': *d++='%'; *d++='%'; break;
	  case '?': *d++='_'; break;
	  case '_':
	  case '[':
	  case ']': *d++='*'; *d++ = *s; break;
	  case '%': *d++='*'; *d++='%'; *d++='%'; break;
	  default: *d++ = *s; break;
	}
    }
    *d='\0';

    /* Copy back into argv */
    strcpy(arg,buffer);

    return(MR_EXISTS);
}

/* This version includes uppercase conversion, for things like gmac.
 * This is necessary because "LIKE" doesn't work with "uppercase()".
 * Including it in a wildcard routine saves making two passes over
 * the argument string.
 */
convert_wildcards_uppercase(arg)
    char *arg;
{
    static char buffer[ARGLEN];
    register char *s, *d;

    for(d=buffer,s=arg;*s;s++) {
	switch(*s) {
	  case '*': *d++='%'; *d++='%'; break;
	  case '?': *d++='_'; break;
	  case '_':
	  case '[':
	  case ']': *d++='*'; *d++ = *s; break;
	  case '%': *d++='*'; *d++='%'; *d++='%'; break;
	  default: *d++=toupper(*s); break;       /* This is the only diff. */
	}
    }
    *d='\0';

    /* Copy back into argv */
    strcpy(arg,buffer);

    return(MR_EXISTS);
}


/* Looks like it's time to build an abstraction barrier, Yogi */
mr_select_any(stmt)
    EXEC SQL BEGIN DECLARE SECTION; 
    char *stmt;
    EXEC SQL END DECLARE SECTION; 
{
    int result=0;

    EXEC SQL PREPARE stmt FROM :stmt;
    EXEC SQL DESCRIBE stmt INTO :SQLDA;
    if(SQLDA->sqld==0)                       /* Not a SELECT */
        return(MR_INTERNAL);        
    EXEC SQL DECLARE csr CURSOR FOR stmt;
    EXEC SQL OPEN csr;
    EXEC SQL FETCH csr USING DESCRIPTOR :SQLDA;
    if(sqlca.sqlcode==0) 
        result=MR_EXISTS;
    else if((sqlca.sqlcode<0) && mr_errcode)
        result=mr_errcode;
    else
        result=0;
    EXEC SQL CLOSE csr;
    return(result);
} 



/*  Adds a string to the string table.  Returns the id number.
 * 
 */
int add_string(name)
    EXEC SQL BEGIN DECLARE SECTION; 
    char *name;
    EXEC SQL END DECLARE SECTION;
{
    EXEC SQL BEGIN DECLARE SECTION;
    char buf[256];
    int id;
    EXEC SQL END DECLARE SECTION; 

    EXEC SQL SELECT value INTO :id FROM numvalues WHERE name = 'strings_id';
    id++;
    EXEC SQL UPDATE numvalues SET value = :id WHERE name = 'strings_id';
    
    /* Use sprintf to get around problem with doubled single quotes */
    sprintf(buf,"INSERT INTO strings (string_id, string) VALUES (%d, '%s')",id,name);
    EXEC SQL EXECUTE IMMEDIATE :buf;
 
    return(id);
}

