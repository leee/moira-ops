@part[serverarr, root="sms.mss"]
@section(Server Arrangement)

Currently, sms acts to update a variety of servers.  Although the data 
control manager performs this update task, each server requires a 
different set of update parameters.  To date, the DCM uses c programs,
not SDFs, to implement the construction of the server specific files. 
Each c program is checked in via the dcm_maint program.   The DCM then
calls the appropriate module when the update interval is reached.

For each server file propagated, ther is at least one application 
interface which provides the capability to manipulate the sms database.
Since the sms database acts as a single point of contact, the
changes made to the database are reflected in the contents of 
recipient servers. 

The following diagram:

@blankspace(3 inches)

This section focusses on the above component contained within
the dotted-lined box.  This section of the system is the server side,
receiving propagated data.  Reference, however, is made to 
each application interface which ultimately effects the server contents.

The services which sms now supports are:
@begin(itemize, spread .5)
Hesiod - The athena nameserver.

RVD - Remote virtual disk.

NFS - Network file system.

/usr/lib/aliases - Mail Service.

MDQS - Multiple device queueing system. (currently not avaliable)

Zephyr - The athena notification service. (currently not available)
@end(itemize)

@SubSection(Server Assumptions)

The requirements of each server suggests a level of detail describing
the following:

@Begin(Itemize, Spread .5, Above 1)

Service name.

Service description.

Propagation interval.

Data format.

Target location.

Generated files.

File description.

Queries used to generate the file (including fields queried).

How the file is modified (application interface).

Example of file contents.
@End(Itemize)

@SubSection(Server Descriptions)

@Begin(Description, Spread 1)
Service:@\@U(Hesiod)

Description:@\@Multiple[The hesiod server is a primary source of contact for
many athena operations.  It is reponsible for providing information reliably
and quickly.  SMS's responsibility to hesiod is to provide authoritative
data.  Hesiod uses a BIND data format in all of it's data files.  SMS will
provide BIND format to hesiod.  There are several files which hesiod uses.
To date, they are known to include the following:

@Begin(Itemize, Spread 0, Above 1, Below 1)

cluster.db

service.db 

printers.db

lpr.db

pobox.db

sloc.db

rvdtab.db

passwd.db

printcap.db
@End(Itemize)

Each of these files are described in detail below.  The hesiod server uses
these files from virtual memory on the target machine.  The server
automatically loads the files from disk into memory when it is started.
SMS will propagate hesiod files to the target disk and the run a shell script 
which will kill the running server and then restart it, causing the newly
updated files to be read into memory. 

With hesiod, all target machines receive identical files.  Practically, 
therefore, the DCM will prepare only one set of files and then will
propagate to several target hosts.

For additional technical information on @i[hesiod], please refer to the
Hesiod technical plan.
]

Propagation interval:@\6 Hours, 0:00, 6:00, 12:00, 18:00

Data format:@\BIND

Target locations:@\
@begin(Format, Above 0, Below 0) 
JASON.MIT.EDU: /etc/athena/nameserver
ZEUS.MIT.EDU: /etc/athena/nameserver
MENELAUS.MIT.EDU: /etc/athena/nameserver
@end(Format)

Files:@\
@Begin(Display, LeftMargin 0, RightMargin 0)
@B(HESIOD.DB) - Hesiod data 

Description:
   Contains hesiod specific data.                 

Queries used:
   NOT CREATED FROM SMS QUERY

How modified:
   EDITOR by system administrator.

Client(s):
   Hesiod

Example contents:
@Begin(Verbatim)
   ; Hesiod-specific cache data (for ATHENA.MIT.EDU)
   ; pointers to Hesiod name servers
   ;
   NS.ATHENA.MIT.EDU. 99999999 HS NS JASON.MIT.EDU.
   NS.ATHENA.MIT.EDU. 99999999 HS NS ZEUS.MIT.EDU.
   NS.ATHENA.MIT.EDU. 99999999 HS NS MENELAUS.MIT.EDU.
   ;
   ; Hesiod address records (simply duplicates of IN address records)
   ;
   JASON.MIT.EDU.     99999999 HS  A 18.71.0.7
   ZEUS.MIT.EDU.      99999999 HS  A 18.58.0.2
   MENELAUS.MIT.EDU.  99999999 HS  A 18.72.0.7
   ;
   ; Internet address records for the same Hesiod servers
   ; required because of implementations of gethostbyname() which
   ; use C_ANY/T_A queries.
   ;
   JASON.MIT.EDU.     99999999 IN  A 18.71.0.7
   ZEUS.MIT.EDU.      99999999 IN  A 18.58.0.2
   MENELAUS.MIT.EDU.  99999999 IN  A 18.72.0.7
   ;
@End(Verbatim)

@Hinge()
@B(CLUSTER.DB) - Cluster data

Description:
   Cluster.db holds the relationships between machines,
   clusters, and services to service clusters.           

Queries used:
   get_all_service_clusers()@ @ -->@ @ 
       (cluster, service_label, service_cluster)
   get_machine_to_cluster_map(*,*)@ @ -->@ @ (machine, cluster)

How modified:
   cluster_maint

Client(s):

Example contents:
@Begin(Verbatim, Above 1)
   ; Cluster info for timesharing machines and workstations
   ; format is:
   ; lines for per-cluster info (both vs and rt) (type UNSPECA)
   ; followed by line for each machine (CNAME referring to one 
   ; of the lines above)
   ;
   ; E40 cluster
   ;
   bldge40-vs.cluster HS UNSPECA "zephyr neskaya.mit.edu"
   bldge40-rt.cluster HS UNSPECA "zephyr neskaya.mit.edu"
   bldge40-vs.cluster HS UNSPECA "lpr e40"
   bldge40-rt.cluster HS UNSPECA "lpr e40"
   ;
@End(Verbatim)


@Hinge()
@B(SERVICE.DB) - services

Description:
   Holds the relationship between a canonical service name and
   its physical protocol, port, and tranlation.

Queries used:
   get_all_services@ @ -->@ @ (service, protocol, port, description)
   get_alias(*, SERVICE)@ @ -->@ @ (name, type, translation)

How modified:
   service_maint

Client(s):


Example contents:
@Begin(Verbatim, Above 1)
   ;
   ; Network services, Internet style
   ;
   echo.service    HS      UNSPECA "echo tcp 7"
   echo.service    HS      UNSPECA "echo udp 7"
   discard.service HS      UNSPECA "discard tcp 9 sink null"
   sink.service    HS      CNAME   discard.service
   null.service    HS      CNAME   discard.service
   discard.service HS      UNSPECA "discard udp 9 sink null"
   systat.service  HS      UNSPECA "systat tcp 11 users"
   users.service   HS      CNAME   systat.service
   daytime.service HS      UNSPECA "daytime tcp 13"
@End(Verbatim)


@Hinge()
@B(PASSWD.DB) - username and group information

Description:
@Begin(Text, Above 0, Below 0, Spacing 1, Spread 0, LeftMargin +2, Indent 0)
This file is used as a template for toehold. Its contents
are username, uid, gid (all users get gid = 101), fullname,
home filesys (now limited to /mit/<username>), and shell.
@End(Text)

Queries used:
   get_all_active_logins()@ @ -->@ @ returns all active logins.           

How modified:
   ueser_maint
   userreg@ @ -->@ @ initial info
   attach_maint

Client(s):
   Toehold

Example contents:
@Begin(Verbatim, Above 1)
   pjlevine.passwd HS    UNSPECA \
       "pjlevine:*:1:101:Peter J. Levine,,,,:/mit/pjlevine:/bin/csh"
@End(Verbatim)


@Hinge()
@B(PRINTERS.DB) - MDQS printer info

Description:
   Maps printer clusters to physical locations.

Queries used:
   get_all_printer_clusters()@ @ -->@ @ (cluster)
   get_printers_of_cluster(cluster)@ @ -->@ @ 
       (pname, qname, serverhost, ability, hwtype)

How modified:
   printer_maint

Client(s):
   MDQS

Example contents:
@Begin(Verbatim, Above 1)
    ; MDQS Hesiod printer info
    ;
    ; prclusterlist returns all print clusters
    ;
    *.prclusterlist HS UNSPECA bldge40
    *.prclusterlist HS UNSPECA bldg1
    *.prclusterlist HS UNSPECA bldgw20
@End(Verbatim)

@Hinge()
@B(LPR.DB) - lpr printer info

Description:
   Line printer information.

Queries used:
   NOT GENERATED BY SMS

How modified:
   HAND

Example contents:


@Hinge()
@B(PRINTCAP.DB) - line printer information

Description:
   Line printer info, derived from /etc/printcap

Queries used:
   NOT GENERATED BY SMS

How modified:
   HAND

Example contents:

@Hinge()
@B(POBOX.DB) - post office info

Description: 
   Contains a username to post office mapping.

Queries used:
   get_all_poboxes()@ @ -->@ @ (login, type, machine, box)

How modified:
   userreg
   usermaint
   chpobox

Client(s):

Example contents:
@Begin(Verbatim, Above 1)
   abbate.pobox     HS UNSPECA "POP e40-po.mit.edu abbate"
   ackerman.pobox   HS UNSPECA "POP e40-po.mit.edu ackerman"
   ajericks.pobox   HS UNSPECA "POP e40-po.mit.edu ajericks"
   ambar.pobox      HS UNSPECA "POP e40-po.mit.edu ambar"
   andrew.pobox     HS UNSPECA "POP e40-po.mit.edu andrew"
   annette.pobox    HS UNSPECA "POP e40-po.mit.edu annette"
   austin.pobox     HS UNSPECA "POP e40-po.mit.edu austin"
@End(Verbatim)


@Hinge()
@B(SLOC.DB) - service location

Description:
   This file maps a service name to a machine name.

Queries used:
   get_server_locations(*)@ @ -->@ @ (server, location)

How modified:
   dcm_maint

Client(s):

Example contents:
@Begin(Verbatim, Above 1)
   lcprimaryhost.sloc   HS      UNSPECA matisse.mit.edu
   olctesthost.sloc     HS      UNSPECA castor.mit.edu
   kerberos.sloc        HS      UNSPECA kerberos.mit.edu
   ;
   zephyr.sloc          HS      UNSPECA neskaya.mit.edu
   zephyr.sloc          HS      UNSPECA arilinn.mit.edu
   zephyr.sloc          HS      UNSPECA hobbes.mit.edu
   zephyr.sloc          HS      UNSPECA orpheus.mit.edu
@End(Verbatim)


@Hinge()
@B(FILESYS.DB) - Filesystem info

Description:
@Begin(Text, Above 0, Below 0, Spacing 1, Spread 0, LeftMargin +2, Indent 0)
This file contains all the filesystems and their related 
information.  The information presented in this file
is a filesystem name relating to the following information:
filesystem type, server name, filesystem name, default mount
point, and access mode.    
@End(Text)

Queries used:
   get_all_filesys()@ @ -->@ @ (label, type, machine, name, mount, access)
   get_alias(*, FILESYS)@ @ -->@ @ (name, type, trans)

How modified:
   attach_maint   
   user_reg@ @ -->@ @ associates user to a new filesys.

Client(s):
   attach

Example contents:
@Begin(Verbatim, Above 1)
   NewrtStaffTool.filsys   HS      UNSPECA 
       "RVD NewrtStaffTool helen r /mit/StaffTools"
   NewvsStaffTool.filsys   HS      UNSPECA 
       "RVD NewvsStaffTool helen r /mit/StaffTools"
   Saltzer.filsys          HS      UNSPECA 
       "RVD Saltzer helen r /mnt"
   athena-backup.filsys    HS      UNSPECA 
       "RVD athena-backup castor r /mnt"
@End(Verbatim)

Update mechanism:
@Begin(Text, Above 0, Below 0, Spacing 1, Spread 0, LeftMargin +2, Indent 0)
Updating hesiod is a relatively simple process.  Every
six hours the DCM will initiate a build on each of the above files(assuming
the informtion has changed).  Once a file is constructed, the update
mechanism will transport the file to each of the above machine.
@End(Text)
@end(Display)
@NewPage()

Service:@\@U(RVD)

Description:@\Multiple[The nature of RVD servers recognizes a very different
approach from that of the hesiod discussion.  The RVD mechanism is updated
through two different means.  The first method is for RVD_MAINT (an
application interface) to talk to the RVD server directly.  This program is
described in detail in the section Specialized Management Interfaces.  The
important note here is that the RVD is updated by feeding the server
directly with specific information, not complete files.  The current program
vdbdb performs the updating process to each RVD server.  RVD_MAINT will use
the same protocol.  This process affords instantaneous changes to RVD's.

Secondly, when invoked, RVD_MAINT will also communicate with the sms database.
This communication path will allow the updating of all the fields necessary
to create rvddb, a RVD server specific file.  The generation of this file is
inherent to the DCM. 
If information has changed (via RVD_MAINT),
the dcm will invoke a module which creates an rvddb file.  This file
is then propagated to the relevant RVD server.  This file resides on the
target disk and is used in the event of server failure.

During a session with RVD_MAINT an administrator may make several changes
to the RVD server.  These changes will go into effect immediately.  In addition
to rvddb, a few acls must be present with the propagation.  These files are:

@Begin(Itemize, Spread 0)
/site/rvd/acl/AD.acl

/site/rvd/acl/OP.acl

/site/rvd/acl/SH.acl

/site/rvd/acl/file.acl - where file is host-packname.
@End(Itemize)

RVD support can best be illustrated by the following diagram:
@blankspace(3 inches)

The discussion which follows describes the generation and contents of the
rvddb file.
]

Propagation interval:@\15 minutes, hour aligned

Data Format:@\RVD specific,  ASCII

Target Machines:@\
@begin(Format)
andromache
gaea
hactar
helen
jinx
m4-035-s
m4-035-v
m4-035-w
oath
persephone
prak
slartibartfast
socrates
zarquon
calliope
polyhymnia
@end(format)

Target Path:@\/site/rvd/rvddb 

File(s):@\
@Begin(Display, LeftMargin 0, RightMargin 0)
@B(RVDDB) - RVD specific file

Description:
@Begin(Text, Above 0, Below 0, Spacing 1, Spread 0, LeftMargin +2, Indent 0)
RVDDB is athe rvd specific file which is used by an rvd server.
This file is only used in the event of a catastrophic failure
with the rvd server.  Nonetheless, this file represents all of the
information integral to rvd servers.  
@End(Text)

Queries used:
   get_@^rvd_server(machine)@ @ -->
@\(oper_acl, admin_acl, shutdown_acl)
   get_all_rvd_physical(machine)@ @ -->
@\(device, size, created, modified)
   get_all_rvd_virtual(machine)@ @ -->
@\(name, device, packid, owner, rocap, excap, shcap,
@\ modes, offset, size, created, modified, ownhost)
   get_members_of_list(list)@ @ -->
@\(member_type, member_name)

How modified:
   rvd_maint

Client(s):
   RVD server.

Content example:
@Begin(Verbatim, Above 1)
     operation = add_physical |
     filename = /dev/	      |
     blocks=		      | - This is the header 
     created=		      |   unique to each physical
     modified=		      |   disk on a machine

     operation = add_virtual  |
     physical=		      |
     name=		      |
     owner=		      |
     rocap=		      | - This is the information
     excap=		      |   unique to each virtual
     shcap=		      |   disk.
     modes=		      |  
     offset=		      |   Block gets repeated n 
     blocks=		      |   times.
     created=		      |   Where n is the number
     modified=		      |   of allocated RVDs on a 
     ownhost=		      |   physical disk.
     uid=		      |
@end(Verbatim)
@End(Display)
@NewPage()

Service:@\@U(NFS)

Description:@\@Multiple[Sms supports two files which are necessary
components of NFS operation.  These files are:

@Begin(Itemize, Spread 0)
/site/nfsid

/mit/quota
@End(Itemize)

These files reside on the NFS target machine and are used to allocate NFS 
directories on a per user basis.  The mechanism employed is for all
programs to communicate to the sms database, and then for the dcm to handle
the propagation and creating of NFS lockers.  The best illustration of this 
process is indicated by the following example:

During new user registration, a person will sit down to a workstation and
type 'userreg' for his login name.  When validated the user will 
type a 'real' login name and a password.  In addition, the userreg
program will allocate, automatically, for the user a post office and
an NFS directory.  However, the user will not benefit from this
allocation for a maximum of six hours.  This lag time is due to the
operation of sms and its creation of NFS lockers.  During registration,
the userreg program communicates exclusively with
the sms database for NFS allocation.  Since the NFS file generation
is started by the DCM every 6 hours, the real change is not noticed
for a period of time.  When the 6 hour time is reached the DCM will
create the above two files and send them to the appropriate
target servers.  Once on the target machine, the dcm will invoke a shell 
script which reads the /mit/quota file and then creates the NFS directory.
The basic operation of the script is:

@Begin(Format, Above 1, LeftMargin +4)
mkdir <username> - using /mit/quota file
chown  <UID> - using /site/nfsid
setquota <quota> - using /mit/quota
@End(Format)
]

Propagation interval:@\6 hours, 0:00, 6:00, 12:00, 18:00

Data Format:@\ASCII

Client(s):@\
@begin(Format)
NFS server
sms shell script for creating directories and user quotas.   
@end(Format)

Files updated:@\
@Begin(Display, LeftMargin 0)
@B(/SITE/NFSID) - username to uid/gid mapping.

Description:
@Begin(Text, Above 0, Below 0, Spacing 1, Spread 0, LeftMargin +2, Indent 0)
This file is used for both the nfs server information and
for the sms shell script.  It provides a username to uid/gid
mapping.  The file is distributed to every NFS server and 
is identical on all.
@End(Text)

Queries used:
   get_all_active_users()@ @ -->@ @ (login, uid)
   get_gids_of_user(login)@ @ -->@ @ (gid)

How updated:
   created at registation time with userreg.
   maintained with user_maint.  

Contents example:
@Begin(Verbatim, Above 1, Below 1)
   <username> <UID> <GID1, GID2,...GID32>
@End(Verbatim)
   where: @^username is the user's login name (Ex: pjlevine)
@\UID is the users id number (Ex: 123456)
@\GIDn are the groups in which the user is a member
@\     (max 32)
@TabClear()
@Hinge()
@B(/MIT/QUOTA) - file containing username to quota mapping.

Description:
@Begin(Text, Above 0, Below 0, Spacing 1, Spread 0, LeftMargin +2, Indent 0)
This file contains the mapping between username and quota.
The file is distributed to each filesystem on the recipient 
machine.  The contents of this file is used to create
the NFS directory on the target machine.  Each of the file's 
contents is unique to the filesystem which it represents.
@End(Text)

Queries used:
   get_all_nfsphys()@ @ -->@ @ (machine, dir, status, allocated, size)
   get_nfs_quotas(machine, device)@ @ -->@ @ (login, quota)

How updated:
   created at registation time with userreg.
   maintained with user_maint.  

Contents example:
@Begin(Verbatim, Above 1, Below 1)
   <username> <quota>
@End(Verbatim)
   where: @^username is the user's login name (Ex. pjlevine)
@\quota is the per user allocation (in Mbytes)
@TabClear()
@end(display)

@NewPage()
Service:@\@U(Mail)

Description:@\The generation of /usr/lib/aliases is a process which makes
use of a currently existing program aliasbld.c.  This program 
is called by the dcm every 24 hours.  The hooks into the sms database
are the significant changes made to aliasbld.c.  The /usr/lib/aliases
file is created and propagated to athena.mit.edu.  Only one file and
one propagation is required.  The use of /usr/lib/alaises file
is done manually by executing a shell script extract_aliases.  The use, 
however, is not an sms-related function or responsibility.

Data Type:@\ASCII

Propagation interval:@\24 hours, 3:00

Target:@\ATHENA.MIT.EDU

File(s):@\


@Begin(Display, LeftMargin 0)

@B(/USR/LIB/ALIASES) - mail forwarding information

Description:

Queries Used:
   get_all_maillists()@ @ -->@ @ (list)
   get_members_of_list(list)@ @ -->@ @ (member_type, member_name)
   get_all_poboxes()@ @ -->@ @ (login, type, machine, box) 

How updated:
   listmaint@ @ -->@ @ for all mail list info.
   user_maint@ @ -->@ @ for pobox info.
   userreg@ @ -->@ @ initial info for poboxes.

Contents example:
@end(display)

@End(Description)
