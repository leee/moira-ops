#!/bin/sh
# This script performs updates of hesiod files on hesiod servers.
# $Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/gen/hesiod.sh,v 1.18 1999-09-29 21:46:26 kcr Exp $

exec >/tmp/moira_update.log 2>&1
set -x 

PATH=/etc:/bin:/usr/bin:/usr/etc:/usr/athena/etc
export PATH

# The following exit codes are defined and MUST BE CONSISTENT with the
# error codes the library uses:
MR_HESFILE=47836472
MR_MISSINGFILE=47836473
MR_NAMED=47836475
MR_TARERR=47836476

umask 022

# File that will contain the necessary information to be updated
TARFILE=/var/tmp/hesiod.out
# Directory into which we will empty the tarfile
SRC_DIR=/etc/athena/_nameserver
# Directory into which we will put the final product
DEST_DIR=/etc/athena/nameserver

NAMED=/etc/athena/named
NAMED_PID=/var/athena/named.pid

# Create the destination directory if it doesn't exist
if test ! -d $DEST_DIR
then
   rm -f $DEST_DIR
   mkdir $DEST_DIR
   chmod 755 $DEST_DIR
fi

# If $SRC_DIR does not already exist, make sure that it gets created
# on the same parition as $DEST_DIR.
if test ! -d $SRC_DIR
then
	chdir $DEST_DIR
	mkdir ../_nameserver
	chdir ../_nameserver
	if test $SRC_DIR != `pwd`
	then
		ln -s `pwd` $SRC_DIR
	fi
fi

# make sure SRC_DIR is empty
/bin/rm -rf $SRC_DIR/*

# Alert if tarfile doesn't exist
if test ! -r $TARFILE 
then
	exit $MR_MISSINGFILE
fi

cd $SRC_DIR
tar xvf $TARFILE
# Don't put up with errors extracting the information
if test $? -ne 0
then
   exit $MR_TARERR
fi
for file in *
do
   # Make sure the file is not zero-length
   if test ! -z $file
   then
      mv -f $file $DEST_DIR
      if test $? -ne 0
      then
          exit $MR_HESFILE
      fi
   else
      rm -f $file
      exit $MR_MISSINGFILE
   fi
done

# Kill off the current named and remove the named.pid file.  It is
# important that this file be removed since the script uses its
# existance as evidence that named as has been successfully restarted.

# First, get statistics
kill -ILL `cat $NAMED_PID`
sleep 1
kill -KILL `cat $NAMED_PID`
rm -f $NAMED_PID

# Restart named.
$NAMED
echo named started

sleep 1
# This timeout is implemented by having the shell check TIMEOUT times
# for the existance of $NAMED_PID and to sleep INTERVAL seconds
# between each check.

TIMEOUT=60			# number of INTERVALS until timeout
INTERVAL=60			# number of seconds between checks
i=0
while test $i -lt $TIMEOUT
do
   if test -f $NAMED_PID
   then
	break
   fi
   sleep $INTERVAL
   i=`expr $i + 1`
done
echo out of timeout loop
# Did it time out?
if test $i -eq $TIMEOUT 
then
	exit $MR_NAMED
fi
echo no timeout
# Clean up!
rm -f $TARFILE
echo removed tarfile
rm -f $0
echo removed self

exit 0
