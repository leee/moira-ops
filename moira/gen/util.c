/* $Id: util.c,v 1.13 1998-02-05 22:51:13 danw Exp $
 *
 * Utility routines used by the MOIRA extraction programs.
 *
 * Copyright (C) 1988-1998 by the Massachusetts Institute of Technology.
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include <mit-copyright.h>
#include <moira.h>
#include <moira_site.h>

#include <stdio.h>
#include <unistd.h>

#include "util.h"

RCSID("$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/gen/util.c,v 1.13 1998-02-05 22:51:13 danw Exp $");

extern void sqlglm(char buf[], int *, int *);

void fix_file(char *targetfile)
{
  char oldfile[64], filename[64];

  sprintf(oldfile, "%s.old", targetfile);
  sprintf(filename, "%s~", targetfile);
  if (rename(targetfile, oldfile) == 0)
    {
      if (rename(filename, targetfile) < 0)
	{
	  rename(oldfile, targetfile);
	  perror("Unable to install new file (rename failed)\n");
	  fprintf(stderr, "Filename = %s\n", targetfile);
	  exit(MR_CCONFIG);
	}
    }
  else
    {
      if (rename(filename, targetfile) < 0)
	{
	  perror("Unable to rename old file\n");
	  fprintf(stderr, "Filename = %s\n", targetfile);
	  exit(MR_CCONFIG);
	}
    }
  unlink(oldfile);
}


char *dequote(char *s)
{
  char *last = s;

  while (*s)
    {
      if (*s == '"')
	*s = '\'';
      else if (*s != ' ')
	last = s;
      s++;
    }
  if (*last == ' ')
    *last = '\0';
  else
    *(++last) = '\0';
  return s;
}


void db_error(int code)
{
  extern char *whoami;
  char buf[256];
  int bufsize = 256, len = 0;

  if (code == -1013)
    {
      com_err(whoami, 0, "build cancelled by user");
      exit(MR_ABORT);
    }

  com_err(whoami, MR_DBMS_ERR, " code %d\n", code);
  sqlglm(buf, &bufsize, &len);
  buf[len] = 0;
  com_err(whoami, 0, "SQL error text = %s", buf);
  critical_alert("DCM", "%s build encountered DATABASE ERROR %d\n%s",
		 whoami, code, buf);
  exit(MR_DBMS_ERR);
}
