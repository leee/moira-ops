create table users
(
	login		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	users_id	INTEGER		DEFAULT 0	NOT NULL,
	unix_uid	SMALLINT 	DEFAULT 0	NOT NULL,
	shell		VARCHAR(32)	DEFAULT CHR(0)	NOT NULL,
	last		VARCHAR(30) 	DEFAULT CHR(0)	NOT NULL,
	first		VARCHAR(30) 	DEFAULT CHR(0)	NOT NULL,
	middle		VARCHAR(30) 	DEFAULT CHR(0)	NOT NULL,
	status		SMALLINT 	DEFAULT 0	NOT NULL,
	clearid		VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	type		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	comments        INTEGER		DEFAULT 0	NOT NULL,
	modtime		DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	fullname	VARCHAR(92) 	DEFAULT CHR(0)	NOT NULL,
	nickname	VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	home_addr	VARCHAR(82) 	DEFAULT CHR(0)	NOT NULL,
	home_phone	VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	office_addr	VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	office_phone	VARCHAR(12) 	DEFAULT CHR(0)	NOT NULL,
	department	VARCHAR(50) 	DEFAULT CHR(0)	NOT NULL,
	affiliation	VARCHAR(50) 	DEFAULT CHR(0)	NOT NULL,
	fmodtime	DATE    	DEFAULT SYSDATE	NOT NULL,
	fmodby		INTEGER		DEFAULT 0	NOT NULL,
	fmodwith	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	potype		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	pop_id		INTEGER		DEFAULT 0	NOT NULL,
	box_id		INTEGER		DEFAULT 0	NOT NULL,
	pmodtime	DATE    	DEFAULT SYSDATE	NOT NULL,
	pmodby		INTEGER		DEFAULT 0	NOT NULL,
	pmodwith	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	xname           VARCHAR(30)	DEFAULT CHR(0)	NOT NULL,
	xdept        	VARCHAR(50)	DEFAULT CHR(0)	NOT NULL,
	xtitle       	VARCHAR(50)	DEFAULT CHR(0)	NOT NULL,
	xaddress     	VARCHAR(82)	DEFAULT CHR(0)	NOT NULL,
	xphone1      	VARCHAR(12)	DEFAULT CHR(0)	NOT NULL,
	xphone2      	VARCHAR(12)	DEFAULT CHR(0)	NOT NULL,
	xmodtime     	DATE         	DEFAULT SYSDATE	NOT NULL,
	sigdate      	INTEGER		DEFAULT 0	NOT NULL,
	sigwho       	INTEGER		DEFAULT 0	NOT NULL,
	signature    	VARCHAR(68)	DEFAULT CHR(0)	NOT NULL,
	secure       	INTEGER		DEFAULT 0	NOT NULL,       
	flag         	INTEGER		DEFAULT 0	NOT NULL
);

create table krbmap
(
	users_id	INTEGER		DEFAULT 0	NOT NULL,
	string_id	INTEGER		DEFAULT 0	NOT NULL
);

create table machine
(
	name		VARCHAR(255) 	DEFAULT CHR(0)	NOT NULL,
	mach_id		INTEGER		DEFAULT 0	NOT NULL,
	vendor		VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	model		VARCHAR(24) 	DEFAULT CHR(0)	NOT NULL,
	os		VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	location	VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	contact		VARCHAR(32) 	DEFAULT CHR(0)	NOT NULL,
	use		INTEGER		DEFAULT 0	NOT NULL,
	status		INTEGER		DEFAULT 0	NOT NULL,
	statuschange	DATE    	DEFAULT SYSDATE	NOT NULL,
	snet_id		INTEGER		DEFAULT 0	NOT NULL,
	address		VARCHAR(16)	DEFAULT CHR(0)	NOT NULL,
	hwaddr		VARCHAR(12)	DEFAULT CHR(0)	NOT NULL,
	owner_type	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	owner_id	INTEGER		DEFAULT 0	NOT NULL,
	acomment	INTEGER		DEFAULT 0	NOT NULL,
	ocomment	INTEGER		DEFAULT 0	NOT NULL,
	created		DATE 		DEFAULT SYSDATE	NOT NULL,
	creator		INTEGER		DEFAULT 0	NOT NULL,
	inuse		DATE		DEFAULT SYSDATE	NOT NULL,
	modtime		DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL
);

create table hostalias
(
	name		VARCHAR(255) 	DEFAULT CHR(0)	NOT NULL,
	mach_id		INTEGER	 	DEFAULT 0	NOT NULL
);

create table subnet
(
	name		VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	snet_id		INTEGER		DEFAULT 0	NOT NULL,
	description	VARCHAR(48) 	DEFAULT CHR(0)	NOT NULL,
	saddr		INTEGER		DEFAULT 0	NOT NULL,
	mask		INTEGER		DEFAULT 0	NOT NULL,
	low		INTEGER		DEFAULT 0	NOT NULL,
	high		INTEGER		DEFAULT 0	NOT NULL,
	prefix		VARCHAR(8)	DEFAULT CHR(0)	NOT NULL,
	owner_type	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	owner_id	INTEGER		DEFAULT 0	NOT NULL,
	modtime		DATE 		DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL
);

create table clusters
(
	name		VARCHAR(32) 	DEFAULT CHR(0)	NOT NULL,
	clu_id		INTEGER		DEFAULT 0	NOT NULL,
	description	VARCHAR(32) 	DEFAULT CHR(0)	NOT NULL,
	location	VARCHAR(64) 	DEFAULT CHR(0)	NOT NULL,
	modtime	 	DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL
);

create table mcmap
(
	mach_id		INTEGER		DEFAULT 0	NOT NULL,
	clu_id		INTEGER		DEFAULT 0	NOT NULL
);

create table svc
(
	clu_id		INTEGER		DEFAULT 0	NOT NULL,
	serv_label	VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	serv_cluster	VARCHAR(32) 	DEFAULT CHR(0)	NOT NULL
);

create table list
(
	name		VARCHAR(32) 	DEFAULT CHR(0)	NOT NULL,
	list_id		INTEGER		DEFAULT 0	NOT NULL,
	active		INTEGER 	DEFAULT 0	NOT NULL,
	publicflg	INTEGER 	DEFAULT 0	NOT NULL,
	hidden		INTEGER 	DEFAULT 0	NOT NULL,
	maillist	INTEGER 	DEFAULT 0	NOT NULL,
	grouplist	INTEGER 	DEFAULT 0	NOT NULL,
	gid		SMALLINT 	DEFAULT 0	NOT NULL,
	description	VARCHAR(255) 	DEFAULT CHR(0)	NOT NULL,
	acl_type	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	acl_id		INTEGER		DEFAULT 0	NOT NULL,
	modtime		DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL
);

create table imembers 
(
	list_id		INTEGER		DEFAULT 0	NOT NULL,
	member_type	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	member_id	INTEGER		DEFAULT 0	NOT NULL,
	tag		INTEGER		DEFAULT 0	NOT NULL,
	ref_count	SMALLINT 	DEFAULT 0	NOT NULL,
	direct		SMALLINT 	DEFAULT 0	NOT NULL
);

create table servers 
(
	name		VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	update_int	INTEGER		DEFAULT 0	NOT NULL,
	target_file	VARCHAR(64) 	DEFAULT CHR(0)	NOT NULL,
	script		VARCHAR(128) 	DEFAULT CHR(0)	NOT NULL,
	dfgen		INTEGER		DEFAULT 0	NOT NULL,
	dfcheck		INTEGER		DEFAULT 0	NOT NULL,
	type		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	enable		INTEGER 	DEFAULT 0	NOT NULL,
	inprogress	INTEGER 	DEFAULT 0	NOT NULL,
	harderror	INTEGER		DEFAULT 0	NOT NULL,
	errmsg		VARCHAR(80) 	DEFAULT CHR(0)	NOT NULL,
	acl_type	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	acl_id		INTEGER		DEFAULT 0	NOT NULL,
	modtime		DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0) 	NOT NULL
);

create table serverhosts 
(
	service		VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	mach_id		INTEGER		DEFAULT 0	NOT NULL,
	success		INTEGER 	DEFAULT 0	NOT NULL,
	enable		INTEGER 	DEFAULT 0	NOT NULL,
	override	INTEGER 	DEFAULT 0	NOT NULL,
	inprogress	INTEGER 	DEFAULT 0	NOT NULL,
	hosterror	INTEGER		DEFAULT 0	NOT NULL,
	hosterrmsg	VARCHAR(80) 	DEFAULT CHR(0)	NOT NULL,
	ltt		INTEGER		DEFAULT 0	NOT NULL,
	lts		INTEGER		DEFAULT 0	NOT NULL,
	value1		INTEGER		DEFAULT 0	NOT NULL,
	value2		INTEGER		DEFAULT 0	NOT NULL,
	value3		VARCHAR(32) 	DEFAULT CHR(0)	NOT NULL,
	modtime		DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL
);

create table filesys 
(
	filsys_id	INTEGER		DEFAULT 0	NOT NULL,
	phys_id		INTEGER		DEFAULT 0	NOT NULL,
	label		VARCHAR(32) 	DEFAULT CHR(0)	NOT NULL,
	fs_order	INTEGER		DEFAULT 0	NOT NULL,
	type		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	mach_id		INTEGER		DEFAULT 0	NOT NULL,
	name		VARCHAR(80) 	DEFAULT CHR(0)	NOT NULL,
	mount		VARCHAR(32) 	DEFAULT CHR(0)	NOT NULL,
	rwaccess	CHAR(1) 	DEFAULT CHR(0)	NOT NULL,
	comments	VARCHAR(64) 	DEFAULT CHR(0)	NOT NULL,
	owner		INTEGER		DEFAULT 0	NOT NULL,
	owners		INTEGER		DEFAULT 0	NOT NULL,
	createflg	INTEGER 	DEFAULT 0	NOT NULL,
	lockertype	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	modtime		DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0) 	NOT NULL
);

create table fsgroup
(
	group_id	INTEGER		DEFAULT 0	NOT NULL,
	filsys_id	INTEGER		DEFAULT 0	NOT NULL,
	key		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL
);

create table nfsphys 
(
	nfsphys_id	INTEGER		DEFAULT 0	NOT NULL,
	mach_id		INTEGER		DEFAULT 0	NOT NULL,
	device		VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	dir		VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	status		SMALLINT 	DEFAULT 0	NOT NULL,
	allocated	INTEGER		DEFAULT 0	NOT NULL,
	partsize	INTEGER		DEFAULT 0	NOT NULL,
	modtime		DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL
);

create table quota 
(
	filsys_id	INTEGER		DEFAULT 0	NOT NULL,
	type		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	entity_id	INTEGER		DEFAULT 0	NOT NULL,
	phys_id		INTEGER		DEFAULT 0	NOT NULL,
	quota		INTEGER		DEFAULT 0	NOT NULL,
	modtime		DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0) 	NOT NULL
);

create table zephyr 
(
	class		VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	xmt_type	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	xmt_id		INTEGER		DEFAULT 0	NOT NULL,
	sub_type	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	sub_id		INTEGER		DEFAULT 0	NOT NULL,
	iws_type	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	iws_id		INTEGER		DEFAULT 0	NOT NULL,
	iui_type	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	iui_id		INTEGER		DEFAULT 0	NOT NULL,
	modtime 	DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0) 	NOT NULL
);

create table hostaccess 
(
	mach_id		INTEGER		DEFAULT 0	NOT NULL,
	acl_type	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	acl_id		INTEGER		DEFAULT 0	NOT NULL,
	modtime		DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL
);

create table acl
(
	mach_id		INTEGER		DEFAULT 0	NOT NULL,
	target		VARCHAR(128)	DEFAULT CHR(0)	NOT NULL,
	kind		VARCHAR(16)	DEFAULT CHR(0)	NOT NULL,
	list_id		INTEGER		DEFAULT 0	NOT NULL,
	modtime		DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL
);

create table strings 
(
	string_id	INTEGER		DEFAULT 0	NOT NULL,
	string		VARCHAR(128)	DEFAULT CHR(0)	NOT NULL
);

create table services 
(
	name		VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	protocol	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	port		SMALLINT 	DEFAULT 0	NOT NULL,
	description	VARCHAR(64) 	DEFAULT CHR(0)	NOT NULL,
	modtime		DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0) 	NOT NULL
);

create table printers
(
	name		VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	type		VARCHAR(8)	DEFAULT CHR(0)	NOT NULL,
	hwtype		VARCHAR(16)	DEFAULT CHR(0)	NOT NULL,
	duplexname	VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	mach_id		INTEGER		DEFAULT 0	NOT NULL,
	loghost		INTEGER		DEFAULT 0	NOT NULL,
	rm		INTEGER		DEFAULT 0	NOT NULL,
	rp		VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	rq		INTEGER		DEFAULT 0	NOT NULL,
	ka		SMALLINT 	DEFAULT 0	NOT NULL,
	pc		SMALLINT 	DEFAULT 0	NOT NULL,
	mc		SMALLINT	DEFAULT 0	NOT NULL,
	ac		INTEGER		DEFAULT 0	NOT NULL,
	lpc_acl		INTEGER		DEFAULT 0	NOT NULL,
	banner		INTEGER		DEFAULT 0	NOT NULL,
	location	VARCHAR(64)	DEFAULT CHR(0)	NOT NULL,
	contact		VARCHAR(64)	DEFAULT CHR(0)	NOT NULL,
	modtime		DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL
);

create table printservers
(
	mach_id		INTEGER		DEFAULT 0	NOT NULL,
	kind		VARCHAR(8)	DEFAULT CHR(0)	NOT NULL,
	printer_types	INTEGER		DEFAULT 0	NOT NULL,
	owner_type	VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL,
	owner_id	INTEGER		DEFAULT 0	NOT NULL,
	lpc_acl		INTEGER		DEFAULT 0	NOT NULL,
	modtime		DATE    	DEFAULT SYSDATE	NOT NULL,
	modby		INTEGER		DEFAULT 0	NOT NULL,
	modwith		VARCHAR(8) 	DEFAULT CHR(0)	NOT NULL
);

create table capacls 
(
	capability	VARCHAR(32) 	DEFAULT CHR(0)	NOT NULL,
	tag		VARCHAR(4) 	DEFAULT CHR(0)	NOT NULL,
	list_id		INTEGER		DEFAULT 0	NOT NULL
);

create table alias 
(
	name		VARCHAR(32) 	DEFAULT CHR(0)	NOT NULL,
	type		VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL,
	trans		VARCHAR(128) 	DEFAULT CHR(0)	NOT NULL
);

create table numvalues 
(
	name		VARCHAR(32) 	DEFAULT CHR(0)	NOT NULL,
	value		INTEGER		DEFAULT 0	NOT NULL
);

create table tblstats 
(
	table_name	VARCHAR(16)	DEFAULT CHR(0)	NOT NULL,
	modtime		DATE    	DEFAULT SYSDATE	NOT NULL,
	appends		INTEGER		DEFAULT 0	NOT NULL,
	updates		INTEGER		DEFAULT 0	NOT NULL,
	deletes		INTEGER		DEFAULT 0	NOT NULL
);

create table incremental
(
	table_name	VARCHAR(16)	DEFAULT CHR(0)	NOT NULL,
	service		VARCHAR(16) 	DEFAULT CHR(0)	NOT NULL
);
