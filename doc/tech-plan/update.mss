@SubHeading(Strategy)

@define(en=enumerate)
@define(it=itemize)
@define(m=multiple)
@define(f, facecode f)

@begin(en)
@begin(m)

preparation phase.  This phase is initiated by the SMS when it
determines that an update should be performed.  This can be triggered by
a change in data, by an administrator, by a timer (run by cron), or by a
restart of the SMS machine.

@begin(en)

Check whether a data file is being constructed for transmission.  If
so, do nothing and report the fact.  Otherwise, build a data file from
the SMS database.  (Building the data file is handled with a locking
strategy that ensures that "the" data file available for distribution
is not an incomplete one.  The new data file is placed in position for
transfer once it is complete using the @f(rename) system
call.)@foot(Does this mean that SMS can only update one file at any one
time? -- AMBAR)

Extract from SMS the list of server machines to update, and the
instructions for installing the file.  Perform the remaining steps
independently for each host.

Connect to the server host and send authentication.

Transfer the files to be installed to the server.  These are stored in
@f(filename.sms_update) until the update is actually performed.  At
the same time, the existing file is linked to @f(filename.sms_backup)
for later deinstallations, and to minimize the overhead required in
the actual installation (freeing disk pages).  (The locking strategy
employed throughout also ensures that this will not occur twice
simultaneously.)  A checksum is also transmitted to insure integrity
of the data.

Transfer the installation instruction sequence to the server.

Flush all data on the server to disk.

@end(en)
@end(m)
@begin(m)

Commit phase.  If all portions of the preparation phase are completed
without error, the commit phase is initiated by the SMS.

On a single command from the SMS, the server begins execution of the
instruction sequence supplied.  These can include the following:

@begin(en)

Swap new data files in.  This is done using atomic filesystem
@f(rename) operations.  The cost of this step is kept to an absolute
minimum by keeping both files in the same directory and by retaining
the @f(filename.sms_backup) link to the file.

Revert the file -- identical to swapping in the new data file, but
instead uses @f(filename.sms_backup).  May be useful in the case of an
erroneous installation.

Send a signal to a specified process.  The process_id is assumed to be
recorded in a file; the pathname of this file is a parameter to this
instruction.  The process_id is read out of the file at the time of
execution of this instruction.@foot(Does this mean that we're going to
have to be keeping around lots more @f(daemon.pid) files than we do now?
-- AMBAR)

Execute a supplied command.

@end(en)
@end(m)

Confirm installation.
The server sends back a reply indicating that the installation was
successful.  The SMS then updates the last-update-tried field of the
update table, clears the override value, and sets the 'success' flag.

@end(en)

@SubHeading(Trouble Recovery Procedures)

@begin(en)
@begin(m)

Server fails to perform action.

If an error is detected in the update procedure, the information is
relayed back to the SMS.  The last-update-tried flag is set, and the
'success' flag is cleared, in the update table.  The override value
may be set, depending on the error condition and the default update
interval.

The error value returned is logged to the appropriate file; at some
point it may be desirable to use Zephyr to notify the system maintainers
when failures occur.

A timeout is used in both sides of the connection during the preparation
phase, and during the actual installation on the SMS.  If any single
operation takes longer than a reasonable amount of time, the connection
is closed, and the installation assumed to have failed.  This is to
prevent network lossage and machine crashes from causing arbitrarily
long delays, and instead falls back to the error condition, so that the
installation will be attempted again later.  (Sinc the all the data
files being prepared are valid, extra installations are not harmful.)

@end(m)
@begin(m)

Server crashes.

If a server crashes, it may fail to respond to the next attempted SMS
update.  In this case, it is (generally) tagged for retry at a later
time, say ten or fifteen minutes later.  This retry interval will be
repeated until an attempt to update the server succeeds (or fails due to
another error).

If a server crashes while it is receiving an update, either the file
will have been installed or it will not have been installed.  If it
has been installed, normal system startup procedures should take care
of any followup operations that would have been performed as part of
the update (such as [re]starting the server using the data file).  If
the file has not been installed, it will be updated again from the
SMS, and the existing @f(filename.sms_update) file will be deleted (as
it may be incomplete) when the next update starts.

@end(m)
@begin(m)

SMS crashes.

Since the SMS update table is driven by absolute times and offsets,
crashes of the SMS machine will result in (at worst) delays in updates.
If updates were in progress when the SMS crashed, those that did not
have the install command sent will have a few extra files on the
servers, which will be deleted in the update that will be started the
first time the update table is scanned for updates due to be performed.
Updates for which the install command had been issued may get repeated
if notification of completion was not returned to the SMS.

@end(m)
@end(en)

@comment[Should this be someplace else?]

@SubHeading(Considerations)

What happens if the SMS broadcasts an invalid data file to servers?
In the case of name service, the SMS may not be able to locate the
servers again if the name service is lost.  Also, if the server
machine crashes, it may not be able to come up to full operational
capacity if it relies on the databases which have been corrupted; in
this case, it is possible that the database may not be easily
replacable.  Manual intervention would be required for recovery.
