#!/moira/bin/perl -Tw
# $Id: grouper.pl,v 1.2 2000-03-16 22:18:56 zacheiss Exp $

die "Usage: $0 password\n" unless ($#ARGV == 0);
$whpassword = $ARGV[0];
$db = "";
$mrtest = "mrtest";
$ENV{'PATH'} = "/moira/bin";
use DBI;

$warehouse = DBI->connect("dbi:Oracle:warehouse", "moira", $whpassword,
			  { RaiseError => 1 });
$moira = DBI->connect("dbi:Oracle:moira", "moira", "moira",
		      { RaiseError => 1});

# Get the current term
($term) = $warehouse->selectrow_array("SELECT term_code ".
				    "FROM wareuser.whsis_academic_terms ".
				    "WHERE is_current_term='Y'");
# Convert from "2000FA" to "FA00"
$term =~ s/\d\d(\d\d)(..)/$2$1/;

# Get list of current classes
$classes =
    $warehouse->selectcol_arrayref("SELECT UNIQUE master_subject ".
				   "FROM wareuser.subject_enrollment_moira ".
				   "WHERE term = " . $warehouse->quote($term));

# Get names of current Grouper lists
$sth = $moira->prepare("SELECT name FROM list");
$sth->execute;
while (($name) = $sth->fetchrow_array) {
    next if $name !~ /^(fa|sp|su|ja)\d\d-/;
    $lists{$name} = $name;
}

$lists{"registrar"} = "registrar";

# And MIT ID to username mappings
$sth = $moira->prepare("SELECT login, clearid FROM users ".
		       "WHERE status = 1 OR status = 2");
$sth->execute;
while (($user, $mitid) = $sth->fetchrow_array) {
    $users{$mitid} = $user;
}

($root_id) = $moira->selectrow_array("SELECT users_id FROM users ".
				   "WHERE login = 'root'");

open(MRTEST, "|$mrtest");
print MRTEST "connect $db\n";
print MRTEST "auth\n";

# Create any lists that don't already exist in Moira
foreach $class (@$classes) {
     $base = "\L$term-$class";
     $staff = "$base-staff";

     # check_list(name, owner, export, parent, desc)

     &check_list($base, "registrar", 1, "",
 		"Automatically-created class participants list for $class");
     &check_list("$base-reg", "registrar", 0, $base,
 		"Automatically-generated registered students list for $class");
     &check_list($staff, $staff, 0, $base,
 		"Automatically-created teaching staff list for $class");
     &check_list("$base-others", $staff, 0, $base,
 		"Automatically-created non-registered students and miscellaneous people list for $class");
}

# Now fill in -reg lists
foreach $class (@$classes) {
    $changed = 0;
    $clist = "\L$term-$class-reg";

    # Get current list membership in Moira 
    %mstudents = (); 
    $sth = $moira->prepare("SELECT u.login FROM users u, imembers i, list l ".
			   "WHERE l.list_id = i.list_id AND i.member_id = ".
			   "u.users_id AND i.direct = 1 AND i.member_type = ".
			   "'USER' AND l.name = " . $moira->quote($clist)); 
    $sth->execute; 
    while (($login) = $sth->fetchrow_array) { 
	$mstudents{$login} = $login; 
    }

    $wstudents = $warehouse->selectcol_arrayref("SELECT UNIQUE mit_id ".
			     "FROM wareuser.subject_enrollment_moira ".
			     "WHERE term = " . $warehouse->quote($term) .
			     " AND master_subject = " . 
			     $warehouse->quote($class));

    foreach $mitid (@$wstudents) {
	$login = $users{$mitid};
	next if !$login;
	if (!$mstudents{$login}) {
	    print "Adding $login to $clist\n";
	    &add_member($login, USER, $clist);
	    $changed = 1;
	} else {
	    delete $mstudents{$login};
	}
    }

    # Everyone in wstudents will have been removed from mstudents
    # now, so delete the remaining users since they don't belong
    foreach $login (keys(%mstudents)) {
	print "Deleting $login from $clist\n";
	&del_member($login, USER, $clist);
	$changed = 1;
    }

    if ($changed) {
	$moira->do("UPDATE list SET modtime = SYSDATE, modby = $root_id, ".
		   "modwith = 'grouper' WHERE name = " . 
		   $moira->quote($clist));
    }
}

print MRTEST "quit\n";
close(MRTEST);
$moira->disconnect;
$warehouse->disconnect;

exit 0;

sub check_list {
    my ( $name, $owner, $export, $parent, $desc ) = @_;
    if (!$lists{$name}) {
	print "Creating $name\n";
	print MRTEST "qy alis $name 1 0 1 $export $export \"create unique GID\" LIST $owner \"$desc\"\n";
        if ($parent) {
	    &add_member($name, LIST, $parent);
	}
    }
}

sub add_member {
    my ( $user, $type, $list ) = @_;
    print MRTEST "qy amtl $list $type $user\n";
}

sub del_member {
    my ( $user, $type, $list ) = @_;
    print MRTEST "qy dmfl $list $type $user\n";
}