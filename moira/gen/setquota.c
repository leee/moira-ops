/*
 *      $Source: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/gen/setquota.c,v $
 *      $Author: mar $
 *      $Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/gen/setquota.c,v 1.6 1990-03-07 17:41:22 mar Exp $
 *
 *      Copyright (C) 1987 by the Massachusetts Institute of Technology
 *
 *      Set quota on specified device for specified user to specified value.
 *
 *      Uses the NFS style quota system/quotactl rather than the Melbourne
 * quota system.
 *
 *      $Log: not supported by cvs2svn $
 * Revision 1.5  90/03/06  15:59:08  jnrees
 * x*.5 replaced with x/2;
 * x*1.2 with (x*6)/5;
 * This avoids linking in the floating-point library.
 * 
 * Revision 1.4  90/01/27  19:48:05  jnrees
 * Grows quotas file if necessary
 * Allows setting quota for a range of uid's.
 * 
 * Revision 1.3  88/10/06  10:46:08  raeburn
 * (mar) Don't expire timers on users currently over quota.
 *
 * Revision 1.1  87/09/04  21:32:47  wesommer
 * Initial revision
 *
 */

#ifndef lint
static char *rcsid_setquota_c = "$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/gen/setquota.c,v 1.6 1990-03-07 17:41:22 mar Exp $";
#endif lint

#include <stdio.h>
#include <ctype.h>
#include <mntent.h>

#include <sys/file.h>
#include <sys/param.h>
#include <sys/time.h>
#include <ufs/quota.h>

#define kb(n)	(howmany(dbtob(n), 1024))

static char device[20];
static char quotafilename[30];
static struct dqblk zblk = {0};

main(argc, argv)
	int argc;
	char **argv;
{
	int uid, uid_low, uid_high, soft_quota, qfd;
	struct dqblk db, odb;
	int uflag = 0;
	int range_mode = 0;
	
	while (argc > 4 && *argv[1] == '-') {
	  switch(argv[1][1]) {
	  case 'u':
	    uflag = 1;
	    --argc;
	    ++argv;
	    break;
	  case 'r':
	    range_mode = 1;
	    --argc;
	    ++argv;
	    break;
	  default:
	    goto usage;
	  }
	}
	
	if ((argc != 4 && !range_mode) || (argc != 5 && range_mode)) {
	usage:
		fprintf(stderr, "usage: setquota [-u] special uid quota\n\
       setquota -r [-u] special uid_low uid_high quota\n\
-u means set limit to <quota> + cur usage\n\
special is a mounted filesystem special device\n\
quota is in 1KB units\n");
		exit(1);
	}

	if ((!range_mode &&
	     (!isdigit(*argv[2]) || !isdigit(*argv[3]))) ||
	    (range_mode &&
	     (!isdigit(*argv[2]) || !isdigit(*argv[3]) ||
	      !isdigit(*argv[4])))) {
	  fprintf(stderr, "setquota: uid and quota must be numeric\n");
	  goto usage;
	}

	if (range_mode){
	  uid_low = atoi(argv[2]);
	  uid_high = atoi(argv[3]);
	  soft_quota = atoi(argv[4]);
	  if (uid_low > uid_high){
	    fprintf(stderr, "setquota: range error\n");
	    exit(1);
	  }
	}
	else{
	  uid_low = uid_high = atoi(argv[2]);
	  soft_quota = atoi(argv[3]);
	}
	
	get_device(argv[1]);

	for(uid = uid_low; uid <= uid_high; uid++){

	  if (quotactl(Q_GETQUOTA, device, uid, &odb) != 0) {

	    if (!(qfd = open(quotafilename, O_RDWR))){
	      perror("No quota file");
	      exit(1);
	    }

	    lseek(qfd, 32767*sizeof(struct dqblk), L_SET);
	    write(qfd, &zblk, sizeof(struct dqblk));
	    close(qfd);

	    if (quotactl(Q_GETQUOTA, device, uid, &odb) != 0) {
	      perror("Can't get current quota info");
	      exit(1);
	    }
	  }

	  db.dqb_bsoftlimit = soft_quota;
	  db.dqb_bhardlimit = (db.dqb_bsoftlimit * 6) / 5;
	  db.dqb_fsoftlimit = soft_quota / 2;
	  db.dqb_fhardlimit = (db.dqb_fsoftlimit * 6) / 5;
	  db.dqb_btimelimit = odb.dqb_btimelimit;
	  db.dqb_ftimelimit = odb.dqb_ftimelimit;

	  db.dqb_bsoftlimit *= btodb(1024);
	  db.dqb_bhardlimit *= btodb(1024);

	  if (uflag) {
	    db.dqb_bhardlimit += odb.dqb_curblocks;
	    db.dqb_bsoftlimit += odb.dqb_curblocks;
	    db.dqb_fhardlimit += odb.dqb_curfiles;
	    db.dqb_fsoftlimit += odb.dqb_curfiles;
	  }
	
	  if (quotactl(Q_SETQLIM, device, uid, &db) < 0) {
	    fprintf (stderr, "quotactl: %d on ", uid);
	    perror (device);
	    exit (1);
	  }
	}
	
	if (quotactl(Q_SYNC, device, 0, 0) < 0) {
	  perror ("can't sync disk quota");
	  exit (1);
	}

	exit (0);
}
	
get_device(device_or_dir)
char *device_or_dir;
{
  register struct mntent *mntp;
  FILE *fstab;

  fstab = setmntent(MNTTAB, "r");
  while(mntp = getmntent(fstab)){
    if ((strcmp(mntp->mnt_fsname, device_or_dir) == 0) ||
	(strcmp(mntp->mnt_dir, device_or_dir) == 0)){
      strcpy(device, mntp->mnt_fsname);
      sprintf(quotafilename, "%s/quotas", mntp->mnt_dir);
      endmntent(fstab);
      return;
    }
  }
  fprintf(stderr, "%s not mounted.\n", device_or_dir);
  exit(1);
}