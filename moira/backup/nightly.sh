#!/bin/sh -x
#
#	Nightly script for backing up SMS.
#
#
BKUPDIRDIR=/u3/sms_backup
PATH=/bin:/usr/bin:/usr/ucb:/usr/new; export PATH
chdir ${BKUPDIRDIR}

/u1/sms/backup/counts </dev/null	

if [ -d in_progress ] 
then
	echo "Two backups running?"
	exit 1
fi

trap "rm -rf ${BKUPDIRDIR}/in_progress" 0 1 15 

if mkdir in_progress 
then
	echo "Backup in progress."
else
	echo "Cannot create backup directory"
	exit 1
fi
if /u1/sms/backup/smsbackup ${BKUPDIRDIR}/in_progress/
then
	echo "Backup successful"
else
	echo "Backup failed!"
	exit 1
fi

if [ -d stale ]
then
	echo -n "Stale backup "
	rm -r stale
	echo "removed"
fi
echo -n "Shifting backups "

mv backup_3 stale
echo -n "3"
mv backup_2 backup_3
echo -n "2"
mv backup_1 backup_2
echo -n "1"
mv in_progress backup_1
echo 
echo -n "deleting last backup"
rm -rf stale
echo "Shipping over the net:"
su wesommer -fc "rdist -c ${BKUPDIRDIR} apollo:/site/sms/sms_backup"
su wesommer -fc "rdist -c ${BKUPDIRDIR} zeus:/site/sms/sms_backup"
su wesommer -fc "rdist -c ${BKUPDIRDIR} jason:/site/sms/sms_backup"
su wesommer -fc "rdist -c ${BKUPDIRDIR} trillian:/site/sms/sms_backup"
exit 0
