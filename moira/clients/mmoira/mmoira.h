/* $Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/clients/mmoira/mmoira.h,v 1.7 1992-12-10 10:58:24 mar Exp $ */

#include "data.h"

extern EntryForm *GetAndClearForm(), *GetForm();
extern char *user, *program_name;
extern int tty;
extern char *user_states[], *nfs_states[];
extern char *StringValue();
extern int DisplayCallback(), MoiraValueChanged();
extern int NumChildren();
extern Widget NthChild();

typedef struct _MoiraResources {
    String form_trans;
    String text_trans;
    String log_trans;
    String help_file;
    String db;
    Boolean noauth;
    int maxlogsize;
} MoiraResources;
extern MoiraResources resources;

#define HELPFILE	"/usr/athena/lib/moira.help"
#define MAXLOGSIZE	10000

#define MM_STATS	1
#define MM_CLIENTS	2
#define MM_SHOW_VALUE	3
#define MM_SHOW_ALIAS	4
#define MM_SHOW_HOST	5
#define MM_ADD_HOST	6
#define MM_MOD_HOST	7
#define MM_DEL_HOST	8
#define MM_CLEAR_HOST	9
#define MM_RESET_HOST	10
#define MM_SHOW_SERVICE	11
#define MM_ADD_SERVICE	12
#define MM_MOD_SERVICE	13
#define MM_DEL_SERVICE	14
#define MM_CLEAR_SERVICE	15
#define MM_RESET_SERVICE	16
#define MM_SHOW_DCM	17
#define MM_ENABLE_DCM	18
#define MM_TRIGGER_DCM	19
#define MM_SHOW_ZEPHYR	20
#define MM_ADD_ZEPHYR	21
#define MM_MOD_ZEPHYR	22
#define MM_DEL_ZEPHYR	23
#define MM_SHOW_PCAP	24
#define MM_ADD_PCAP	25
#define MM_MOD_PCAP	26
#define MM_DEL_PCAP	27
#define MM_SHOW_CLDATA	28
#define MM_ADD_CLDATA	29
#define MM_DEL_CLDATA	30
#define MM_SHOW_MCMAP	31
#define MM_ADD_MCMAP	32
#define MM_DEL_MCMAP	33
#define MM_SHOW_CLUSTER	34
#define MM_ADD_CLUSTER	35
#define MM_MOD_CLUSTER	36
#define MM_DEL_CLUSTER	37
#define MM_SHOW_MACH	38
#define MM_ADD_MACH	39
#define MM_MOD_MACH	40
#define MM_DEL_MACH	41
#define MM_SHOW_MEMBERS	42
#define MM_ADD_MEMBER	43
#define MM_DEL_MEMBER	44
#define MM_DEL_ALL_MEMBER	45
#define MM_SHOW_LIST	46
#define MM_SHOW_MAILLIST	47
#define MM_SHOW_ACE_USE	48
#define MM_ADD_LIST	49
#define MM_MOD_LIST	50
#define MM_DEL_LIST	51
#define MM_SHOW_QUOTA	52
#define MM_ADD_QUOTA	53
#define MM_MOD_QUOTA	54
#define MM_DEL_QUOTA	55
#define MM_SHOW_DQUOTA	56
#define MM_SET_DQUOTA	57
#define MM_SHOW_NFS	58
#define MM_ADD_NFS	59
#define MM_MOD_NFS	60
#define MM_DEL_NFS	61
#define MM_SHOW_FS_ALIAS	62
#define MM_ADD_FS_ALIAS	63
#define MM_DEL_FS_ALIAS	64
#define MM_SHOW_FSGROUP	65
#define MM_ADD_FSGROUP	66
#define MM_MOV_FSGROUP	67
#define MM_DEL_FSGROUP	68
#define MM_SHOW_FILSYS	69
#define MM_ADD_FILSYS	70
#define MM_MOD_FILSYS	71
#define MM_DEL_FILSYS	72
#define MM_SHOW_KRBMAP	73
#define MM_ADD_KRBMAP	74
#define MM_DEL_KRBMAP	75
#define MM_SHOW_POBOX	76
#define MM_SET_POBOX	77
#define MM_DEL_POBOX	78
#define MM_SHOW_USER	79
#define MM_ADD_USER	80
#define MM_REGISTER	81
#define MM_MOD_USER	82
#define MM_DEACTIVATE	83
#define MM_EXPUNGE	84
#define MM_SHOW_FINGER	85
#define MM_MOD_FINGER	86
#define MM_RESET_POBOX	87
#define MM_HELP_MOIRA	88
#define MM_HELP_WILDCARDS 89
#define MM_HELP_AUTHORS 90
#define MM_HELP_BUGS	91
#define MM_SAVE_LOG	92
#define MM_NEW_VALUE	93
#define MM_QUIT		94
#define MM_HELP_KEYBOARD 95
#define MM_HELP_MOUSE	96
