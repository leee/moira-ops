/* This file defines the query dispatch table for version 2 of the protocol
 *
 * $Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/server/queries2.c,v 2.9 1992-08-23 21:37:02 genoa Exp $
 *
 * Copyright 1987, 1988 by the Massachusetts Institute of Technology.
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include <mit-copyright.h>
#include "query.h"
#include "mr_et.h"

/* Specialized Support Query Routines */

/* Special Access Check Routines */
int access_user();
int access_login();
int access_list();
int access_visible_list();
int access_vis_list_by_name();
int access_member();
int access_qgli();
int access_service();
int access_filesys();

/* Query Setup Routines */
int prefetch_value();
int prefetch_filesys();
int setup_ausr();
int setup_dusr();
int setup_spop();
int setup_dpob();
int setup_dmac();
int setup_dclu();
int setup_alis();
int setup_dlis();
int setup_dsin();
int setup_dshi();
int setup_afil();
int setup_ufil();
int setup_dfil();
int setup_aftg();
int setup_dnfp();
int setup_dqot();
int setup_sshi();
int setup_akum();

/* Query Followup Routines */
int followup_fix_modby();
int followup_ausr();
int followup_gpob();
int followup_glin();
int followup_aqot();
int followup_gzcl();
int followup_gsha();
int followup_gqot();
int followup_gpce();
int followup_guax();
int followup_uuac();

int set_modtime();
int set_modtime_by_id();
int set_finger_modtime();
int set_pobox_modtime();
int set_uppercase_modtime();
int set_mach_modtime_by_id();
int set_cluster_modtime_by_id();
int set_serverhost_modtime();
int set_nfsphys_modtime();
int set_filesys_modtime();
int set_zephyr_modtime();

/* Special Queries */
int set_pobox();
int get_list_info();
int add_member_to_list();
int delete_member_from_list();
int get_ace_use();
int qualified_get_lists();
int get_members_of_list();
int get_end_members_of_list();
int qualified_get_server();
int qualified_get_serverhost();
int trigger_dcm();
int count_members_of_list();
int get_lists_of_member();
int register_user();



/* String compression
 * These are commonly used strings in the table, defined here so that
 * they can be shared.
 */

static char ACE_NAME[] = "ace_name";
static char ACE_TYPE[] = "ace_type";
static char CLASS[] = "class";
static char CLU_ID[] = "clu_id";
static char CLUSTER[] = "cluster";
static char COMMENTS[] = "comments";
static char DESC[] = "description";
static char DEVICE[] = "device";
static char DIR[] = "dir";
static char FILESYS[] = "filesys";
static char FILSYS_ID[] = "filsys_id";
static char FIRST[] = "firstname";
static char LABEL[] = "label";
static char LAST[] = "lastname";
static char LIST[] = "list";
static char LIST_ID[] = "list_id";
static char LOCATION[] = "location";
static char LOGIN[] = "login";
static char MACH_ID[] = "mach_id";
static char MACHINE[] = "machine";
static char MIDDLE[] = "middle";
static char MIT_ID[] = "clearid";
static char MOD1[] = "modtime";
static char MOD2[] = "modby";
static char MOD3[] = "modwith";
static char NAME[] = "name";
static char QUOTA[] = "quota";
static char QUOTA_TYPE[] = "quota_type";
static char SECURE[] = "secure";
static char SERVICE[] = "service";
static char SHELL[] = "shell";
static char SIGNATURE[] = "signature";
static char STATUS[] = "status";
static char TYPE[] = "type";
static char USERS[] = "users";
static char USERS_ID[] = "users_id";
static char UID[] = "uid";



/* VALOBJS
 * These are commonly used validation objects, defined here so that they
 * can be shared.
 */

static struct valobj VOsort0[] = {
  {V_SORT, 0},
};

static struct valobj VOwild0[] = {
  {V_WILD, 0},
};

static struct valobj VOupwild0[] = {
  {V_UPWILD, 0},
};

static struct valobj VOwild0sort[] = {
  {V_WILD, 0},
  {V_SORT, 0},
};

static struct valobj VOupwild0sort[] = {
  {V_UPWILD, 0},
  {V_SORT, 0},
};

static struct valobj VOwild01sort0[] = {
  {V_WILD, 0},
  {V_WILD, 1},
  {V_SORT, 0},
};

static struct valobj VOwild01sort01[] = {
  {V_WILD, 0},
  {V_WILD, 1},
  {V_SORT, 0},
  {V_SORT, 1}, 
};

static struct valobj VOwild012sort0[] = {  /* get_alias */
  {V_WILD, 0},
  {V_WILD, 1},
  {V_WILD, 2},
  {V_SORT, 0},
};

static struct valobj VOdate1[] = {
  {V_DATE, 1, 0, 0, 0, MR_DATE},
};

static struct valobj VOuser0[] = {
  {V_ID, 0, USERS, LOGIN, USERS_ID, MR_USER},
};

static struct valobj VOuser0lock[] = {
  {V_LOCK, 0, USERS, 0, USERS_ID, MR_DEADLOCK},
  {V_ID, 0, USERS, LOGIN, USERS_ID, MR_USER},
};

static struct valobj VOmach0[] = {
  {V_ID, 0, MACHINE, NAME, MACH_ID, MR_MACHINE},
};

static struct valobj VOclu0[] = {
  {V_ID, 0, CLUSTER, NAME, CLU_ID, MR_CLUSTER},
};

static struct valobj VOlist0[] = {
  {V_ID, 0, LIST, NAME, LIST_ID, MR_LIST},
};

static struct valobj VOfilsys0[] = {
  {V_ID, 0, FILESYS, LABEL, FILSYS_ID, MR_FILESYS},
};

static struct valobj VOchar0[] = {
  {V_CHAR, 0},
};

static struct valobj VOsort01[] = {
  {V_SORT, 1},
  {V_SORT, 0},
};

static struct valobj VOuser0sort[] = {
  {V_ID, 0, USERS, LOGIN, USERS_ID, MR_USER},
  {V_SORT, 0},
};

static struct valobj VOfilsys0user1[] = {
  {V_LOCK, 0, FILESYS, 0, FILSYS_ID, MR_DEADLOCK},
  {V_ID, 0, FILESYS, LABEL, FILSYS_ID, MR_FILESYS},
  {V_ID, 1, USERS, LOGIN, USERS_ID, MR_USER},
};


/* Validations
 * Commonly used validation records defined here so that they may
 * be shared.
 */

static struct validate VDmach = { VOmach0, 1 };
static struct validate VDsort0= { VOsort0, 1 };
static struct validate VDwild0= { VOwild0, 1 };
static struct validate VDupwild0= { VOupwild0, 1 };
static struct validate VDsort2= { VOsort01,2 };
static struct validate VDwild2sort2 = { VOwild01sort01,4 };
static struct validate VDwild3sort1 = { VOwild012sort0,4 };
static struct validate VDsortf = { 
    VOsort0,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    followup_fix_modby,
};

static struct validate VDwildsortf = { 
    VOwild0sort,
    2,
    0,
    0,
    0,
    0,
    0,
    0,
    followup_fix_modby,
};

static struct validate VDwild2sortf = { 
    VOwild01sort01,
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    followup_fix_modby,
};

static struct validate VDupwildsortf = { 
    VOupwild0sort,
    2,
    0,
    0,
    0,
    0,
    0,
    0,
    followup_fix_modby,
};



/* Query data */

static char *galo_fields[] = {
  LOGIN, UID, SHELL, LAST, FIRST, MIDDLE,
};

static char *gual_fields[] = {
  LOGIN,
  LOGIN, UID, SHELL, LAST, FIRST, MIDDLE, STATUS,
  MIT_ID, CLASS, COMMENTS, SIGNATURE, SECURE, MOD1, MOD2, MOD3,
};
 
static char *gubl_fields[] = {
  LOGIN,
  LOGIN, UID, SHELL, LAST, FIRST, MIDDLE, STATUS, 
  MIT_ID, CLASS, MOD1, MOD2, MOD3,
};

static struct validate gubl_validate =	
{
  VOwild0sort,
  2,
  0,
  0,
  0,
  0,
  access_login,
  0,
  followup_guax,
};

static char *guau_fields[] = {
    UID,
    LOGIN, UID, SHELL, LAST, FIRST, MIDDLE, STATUS,
    MIT_ID, CLASS, COMMENTS, SIGNATURE, SECURE, MOD1, MOD2, MOD3,
};
 
static char *guan_fields[] = {
    FIRST, LAST,
    LOGIN, UID, SHELL, LAST, FIRST, MIDDLE, STATUS,
    MIT_ID, CLASS, COMMENTS, SIGNATURE, SECURE, MOD1, MOD2, MOD3,
};
 
static char *guac_fields[] = {
    CLASS,
    LOGIN, UID, SHELL, LAST, FIRST, MIDDLE, STATUS,
    MIT_ID, CLASS, COMMENTS, SIGNATURE, SECURE, MOD1, MOD2, MOD3,
};
 
static char *guam_fields[] = {
    MIT_ID,
    LOGIN, UID, SHELL, LAST, FIRST, MIDDLE, STATUS,
    MIT_ID, CLASS, COMMENTS, SIGNATURE, SECURE, MOD1, MOD2, MOD3,
};
 
static char *gubu_fields[] = {
    UID,
    LOGIN, UID, SHELL, LAST, FIRST, MIDDLE, STATUS, 
    MIT_ID, CLASS, MOD1, MOD2, MOD3,
};

static struct validate gubu_validate =	
{
  VOsort0,
  1,
  0,
  0,
  0,
  0,
  access_login,
  0,
  followup_fix_modby,
};

static char *gubn_fields[] = {
  FIRST, LAST,
  LOGIN, UID, SHELL, LAST, FIRST, MIDDLE, STATUS, 
  MIT_ID, CLASS, MOD1, MOD2, MOD3,
};

static struct validate gubn_validate =
{
    VOwild01sort0,
    3,
    0,
    0,
    0,
    0,
    0,
    0,
    followup_fix_modby,
};

static char *gubc_fields[] = {
  CLASS,
  LOGIN, UID, SHELL, LAST, FIRST, MIDDLE, STATUS, 
  MIT_ID, CLASS, MOD1, MOD2, MOD3,
};

static char *gubm_fields[] = {
  MIT_ID,
  LOGIN, UID, SHELL, LAST, FIRST, MIDDLE, STATUS, 
  MIT_ID, CLASS, MOD1, MOD2, MOD3,
};

static char *auac_fields[] = {
  LOGIN, UID, SHELL, LAST, FIRST, MIDDLE, STATUS, 
  MIT_ID, CLASS, COMMENTS, SIGNATURE, SECURE,
};

static struct valobj auac_valobj[] = {
  {V_LOCK, 0, USERS, 0, USERS_ID, MR_DEADLOCK},
  {V_CHAR, 0},
  {V_CHAR, 3},
  {V_CHAR, 4},
  {V_CHAR, 5},
  {V_CHAR, 7},
  {V_TYPE, 8, CLASS, 0, 0, MR_BAD_CLASS},
  {V_ID, 9, "strings", "string", "string_id", MR_NO_MATCH},
};

static struct validate auac_validate = {
  auac_valobj,
  8,
  LOGIN,
  "users.login = LEFT('%s',SIZE(users.login))",
  1,
  USERS_ID,
  0,
  setup_ausr,
  followup_ausr,
};
 
static struct validate ausr_validate = {
  auac_valobj,
  7,
  LOGIN,
  "users.login = LEFT('%s',SIZE(users.login))",  
  1,
  USERS_ID,
  0,
  setup_ausr,
  followup_ausr,
};

static char *rusr_fields[] = {
  UID, LOGIN, "fs_type"
};

static struct valobj rusr_valobj[] = {
  {V_LOCK, 0, USERS, 0, USERS_ID, MR_DEADLOCK},
  {V_LOCK, 0, LIST, 0, LIST_ID, MR_DEADLOCK},
  {V_LOCK, 0, FILESYS, 0, FILSYS_ID, MR_DEADLOCK},
  {V_LOCK, 0, "nfsphys", 0, "nfsphys_id", MR_DEADLOCK},
};

static struct validate rusr_validate = {
  rusr_valobj,
  4,
  0,
  0,
  0,
  0,
  0,
  register_user,
  0,
};

static char *uuac_fields[] = {
  LOGIN,
  "newlogin", UID, SHELL, LAST, FIRST, MIDDLE, STATUS, 
  MIT_ID, CLASS, COMMENTS, SIGNATURE, SECURE,
};

static struct valobj uuac_valobj[] = {
    {V_LOCK, 0, USERS, 0, USERS_ID, MR_DEADLOCK},
    {V_ID, 0, USERS, LOGIN, USERS_ID, MR_USER},
    {V_RENAME, 1, USERS, LOGIN, USERS_ID, MR_NOT_UNIQUE},
    {V_CHAR, 4},
    {V_CHAR, 5},
    {V_CHAR, 6},
    {V_CHAR, 8},
    {V_TYPE, 9, CLASS, 0, 0, MR_BAD_CLASS},
    {V_ID, 10, "strings", "string", "string_id", MR_NO_MATCH},
};

static struct validate uuac_validate = {
    uuac_valobj,
    9,
    0,
    0,
    0,
    USERS_ID,
    0,
    setup_ausr,
    followup_uuac,
};
 
static struct validate uusr_validate = {
  uuac_valobj,
  8,
  0,
  0,
  0,
  USERS_ID,
  0,
  setup_ausr,
  set_modtime_by_id,
};

static char *uush_fields[] = {
  LOGIN,
  SHELL,
};

static struct validate uush_validate = {
  VOuser0lock,
  2,
  0,
  0,
  0,
  USERS_ID,
  access_user,
  0,
  set_modtime_by_id,
};

static char *uust_fields[] = {
  LOGIN,
  STATUS,
};

static char *uuss_fields[] = {
  LOGIN,
  SECURE,
};
 
static struct validate uust_validate = {
  VOuser0lock,
  2,
  0,
  0,
  0,
  USERS_ID,
  0,
  0,
  set_modtime_by_id,
};

static char *dusr_fields[] = {
  LOGIN,
};

static struct validate dusr_validate = {
  VOuser0lock,
  2,
  0,
  0,
  0,
  0,
  0,
  setup_dusr,
  0,
};

static char *dubu_fields[] = {
  UID,
};

static struct valobj dubu_valobj[] = {
    {V_LOCK, 0, USERS, 0, USERS_ID, MR_DEADLOCK},
    {V_ID, 0, USERS, UID, USERS_ID, MR_USER}
};

static struct validate dubu_validate = {
  dubu_valobj,
  2,
  0,
  0,
  0,
  0,
  0,
  setup_dusr,
  0,
};

static char *gkum_fields[] = { LOGIN, "kerberos",
			       LOGIN, "kerberos" };

static char *akum_fields[] = { LOGIN, "kerberos" };

static struct valobj akum_valobj[] =
{
    {V_ID, 0, USERS, LOGIN, USERS_ID, MR_USER},
    {V_ID, 1, "strings", "string", "string_id", MR_NO_MATCH},
};

static struct validate akum_validate =
{
    akum_valobj,
    1,
    USERS_ID,
    "krbmap.users_id = %d or krbmap.string_id = %d",
    2,
    USERS_ID,
    access_user,
    setup_akum,
    0
};

static struct validate dkum_validate =
{
    akum_valobj,
    2,
    USERS_ID,
    "krbmap.users_id = %d and krbmap.string_id = %d",
    2,
    USERS_ID,
    access_user,
    0,
    0
};

static char *gfbl_fields[] = {
  LOGIN,
  LOGIN, "fullname", "nickname", "home_addr",
  "home_phone", "office_addr", "office_phone", "department", 
  "affiliation", MOD1, MOD2, MOD3,
};

static struct validate gfbl_validate = {
  VOuser0,
  1,
  0,
  0,
  0,
  0,
  access_user,
  0,
  followup_fix_modby,
};

static char *ufbl_fields[] = {
  LOGIN,
  "fullname", "nickname", "home_addr",
  "home_phone", "office_addr", "office_phone", "department", 
  "affiliation",
};

static struct validate ufbl_validate = {
  VOuser0lock,
  2,
  0,
  0,
  0,
  0,
  access_user,
  0,
  set_finger_modtime,
};

static char *gpob_fields[] = {
  LOGIN,
  LOGIN, TYPE, "box", MOD1, MOD2, MOD3,
};

static struct validate gpob_validate = {
  VOuser0,
  1,
  "potype",
  "users.potype != 'NONE' and users.users_id=%d",
  1,
  0,
  access_user,
  0,
  followup_gpob,
};

static char *gpox_fields[] = {
  LOGIN, TYPE, "box",
};

static struct validate gpox_validate = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  followup_gpob,
};

static char *spob_fields[] = {
  LOGIN, TYPE, "box",
};

static struct valobj spob_valobj[] = {
  {V_LOCK, 0, USERS, 0, USERS_ID, MR_DEADLOCK},
  {V_ID, 0, USERS, LOGIN, USERS_ID, MR_USER},
  {V_TYPE, 1, "pobox", 0, 0, MR_TYPE},
};

static struct validate spob_validate =	/* SET_POBOX */
{
  spob_valobj,
  3,
  0,
  0,
  0,
  0,
  access_user,
  0,
  set_pobox,
};

static struct validate spop_validate =	/* SET_POBOX_POP */
{
  VOuser0lock,
  2,
  0,
  0,
  0,
  0,
  access_user,
  setup_spop,
  set_pobox_modtime,
};

static struct validate dpob_validate =	/* DELETE_POBOX */
{
  VOuser0lock,
  2,
  0,
  0,
  0,
  0,
  access_user,
  setup_dpob,
  set_pobox_modtime,
};

static char *gmac_fields[] = {
  NAME,
  NAME, TYPE, MOD1, MOD2, MOD3,
};

static char *amac_fields[] = {
  NAME, TYPE
};

static struct valobj amac_valobj[] = {
  {V_CHAR, 0},
  {V_LOCK, 0, MACHINE, 0, MACH_ID, MR_DEADLOCK},
  {V_TYPE, 1, "mac_type", 0, 0, MR_TYPE},
};

static struct validate amac_validate = {
  amac_valobj,
  3,
  NAME,
  "machine.name = UPPERCASE(LEFT('%s',SIZE(machine.name)))",
  1,
  MACH_ID,
  0,
  prefetch_value,
  set_uppercase_modtime,
};

static char *umac_fields[] = {
  NAME,
  "newname", TYPE,
};

static struct valobj umac_valobj[] = {
  {V_LOCK, 0, MACHINE, 0, MACH_ID, MR_DEADLOCK},
  {V_ID, 0, MACHINE, NAME, MACH_ID, MR_MACHINE},
  {V_RENAME, 1, MACHINE, NAME, MACH_ID, MR_NOT_UNIQUE},
  {V_TYPE, 2, "mac_type", 0, 0, MR_TYPE},
};

static struct validate umac_validate = {
  umac_valobj,
  4,
  0,
  0,
  0,
  MACH_ID,
  0,
  0,
  set_modtime_by_id,
};

static char *dmac_fields[] = {
  NAME,
};

static struct valobj dmac_valobj[] = {
  {V_LOCK, 0, MACHINE, 0, MACH_ID, MR_DEADLOCK},
  {V_ID, 0, MACHINE, NAME, MACH_ID, MR_MACHINE},
};

static struct validate dmac_validate = {
  dmac_valobj,
  2,
  0,
  0,
  0,
  0,
  0,
  setup_dmac,
  0,
};

static char *gclu_fields[] = {
  NAME,
  NAME, DESC, LOCATION, MOD1, MOD2, MOD3, 
};

static char *aclu_fields[] = {
  NAME, DESC, LOCATION,
};
  
static struct valobj aclu_valobj[] = {
  {V_LOCK, 0, CLUSTER, 0, CLU_ID, MR_DEADLOCK},
  {V_CHAR, 0}
};

static struct validate aclu_validate =	
{
  aclu_valobj,
  2,
  NAME,
  "cluster.name = LEFT('%s',SIZE(cluster.name))",
  1,
  CLU_ID,
  0,
  prefetch_value,
  set_modtime,
};

static char *uclu_fields[] = {
  NAME,
  "newname", DESC, LOCATION,
};

static struct valobj uclu_valobj[] = {
  {V_LOCK, 0, CLUSTER, 0, CLU_ID, MR_DEADLOCK},
  {V_ID, 0, CLUSTER, NAME, CLU_ID, MR_CLUSTER},
  {V_RENAME, 1, CLUSTER, NAME, CLU_ID, MR_NOT_UNIQUE},
};

static struct validate uclu_validate = {
  uclu_valobj,
  3,
  0,
  0,
  0,
  CLU_ID,
  0,
  0,
  set_modtime_by_id,
};

static char *dclu_fields[] = {
  NAME,
};

static struct validate dclu_validate = {
    VOclu0,
    1,
    0,
    0,
    0,
    0,
    0,
    setup_dclu,
    0,
};

static char *gmcm_fields[] = {
  MACHINE, CLUSTER,
  MACHINE, CLUSTER,
};

static struct valobj gmcm_valobj[] =
{
  {V_UPWILD, 0},
  {V_WILD, 1},
  {V_SORT, 0},
  {V_SORT, 1},
};

static struct validate gmcm_validate = { gmcm_valobj, 4 };

static struct valobj amtc_valobj[] =	/* ADD_MACHINE_TO_CLUSTER */
{					/* DELETE_MACHINE_FROM_CLUSTER */
  {V_ID, 0, MACHINE, NAME, MACH_ID, MR_MACHINE},
  {V_ID, 1, CLUSTER, NAME, CLU_ID, MR_CLUSTER},
};

static struct validate amtc_validate = /* for amtc and dmfc */
{
  amtc_valobj,
  2,
  MACH_ID,
  "mcmap.mach_id = %d and mcmap.clu_id = %d",
  2,
  0,
  0,
  0,
  set_mach_modtime_by_id,
};

static char *gcld_fields[] = {
  CLUSTER, "serv_label",
  CLUSTER, "serv_label", "serv_cluster",
};

static char *acld_fields[] = {
  CLUSTER, "serv_label", "serv_cluster",
};

static struct valobj acld_valobj[] =
{
  {V_ID, 0, CLUSTER, NAME, CLU_ID, MR_CLUSTER},
  {V_CHAR, 1},
  {V_CHAR, 2},
};

static struct validate acld_validate =	
{					
  acld_valobj,
  3,
  CLU_ID,
  "svc.clu_id = %d AND svc.serv_label = '%s' AND svc.serv_cluster = LEFT('%s',SIZE(svc.serv_cluster))",
  3,
  0,
  0,
  0,
  set_cluster_modtime_by_id,
};

static struct valobj dcld_valobj[] =
{
  {V_ID, 0, CLUSTER, NAME, CLU_ID, MR_CLUSTER},
  {V_CHAR, 2},
};

static struct validate dcld_validate =	
{					
  dcld_valobj,
  2,
  CLU_ID,
  "svc.clu_id = %d AND svc.serv_label = '%s' AND svc.serv_cluster = '%s'",
  3,
  0,
  0,
  0,
  set_cluster_modtime_by_id,
};

static char *gsin_fields[] = {
  SERVICE,
  SERVICE, "update_int", "target_file", "script", "dfgen", "dfcheck",
  TYPE, "enable", "inprogress", "harderror", "errmsg",
  ACE_TYPE, ACE_NAME, MOD1, MOD2, MOD3,
};

static struct valobj gsin_valobj[] =
{
  { V_UPWILD, 0 },
};

static struct validate gsin_validate = 
{
    gsin_valobj,
    1,
    0,
    0,
    0,
    0,
    access_service,
    0,
    followup_glin,
};

static char *qgsv_fields[] = {
    "enable", "inprogress", "harderror",
    "service",
};

static struct valobj qgsv_valobj[] = {
  {V_TYPE, 0, "boolean", 0, 0, MR_TYPE},
  {V_TYPE, 1, "boolean", 0, 0, MR_TYPE},
  {V_TYPE, 2, "boolean", 0, 0, MR_TYPE},
};

static struct validate qgsv_validate = {
    qgsv_valobj,
    3,
    0,
    0,
    0,
    0,
    0,
    0,
    qualified_get_server,
};

static char *asin_fields[] = {
  SERVICE, "update_int", "target_file", "script", TYPE, "enable",
  ACE_TYPE, ACE_NAME,
};

static struct valobj asin_valobj[] = {
  {V_LOCK, 0, MACHINE, 0, MACH_ID, MR_DEADLOCK},
  {V_CHAR, 0},
  {V_TYPE, 4, "service", 0, 0, MR_TYPE},
  {V_TYPE, 6, ACE_TYPE, 0, 0, MR_ACE},
  {V_TYPEDATA, 7, 0, 0, 0, MR_ACE},
};

static struct validate asin_validate =	/* for asin, usin */
{
  asin_valobj,
  5,
  NAME,
  "servers.name = UPPERCASE(LEFT('%s',SIZE(servers.name)))",
  1,
  0,
  0,
  0,
  set_uppercase_modtime,
};

static struct validate rsve_validate = {
  asin_valobj,
  1,
  NAME,
  "services.name = uppercase('%s')",
  1,
  0,
  access_service,
  0,
  set_uppercase_modtime,
};

static char *ssif_fields[] = {
  SERVICE, "dfgen", "dfcheck", "inprogress", "harderror", "errmsg",
};

static struct validate ssif_validate = {
  asin_valobj,
  1,
  NAME,
  "services.name = uppercase('%s')",
  1,
  0,
  0,
  0,
  0,
};

static char *dsin_fields[] = {
  SERVICE,
};

static struct validate dsin_validate = {
  asin_valobj,
  1,
  NAME,
  "services.name = uppercase('%s')",
  1,
  0,
  0,
  setup_dsin,
  0,
};

static char *gshi_fields[] = {
  SERVICE, MACHINE,
  SERVICE, MACHINE, "enable", "override", "success", "inprogress",
  "hosterror", "hosterrormsg", "ltt", "lts", "value1", "value2",
  "value3", MOD1, MOD2, MOD3, 
};

static struct valobj gshi_valobj[] = {
    { V_UPWILD, 0 },
    { V_UPWILD, 1 },
};

static struct validate gshi_validate = {
  gshi_valobj,
  2,
  0,
  0,
  0,
  0,
  access_service,
  0,
  followup_fix_modby,
};

static char *qgsh_fields[] = {
    "service", "enable", "override", "success", "inprogress", "hosterror",
    "service", MACHINE,
};

static struct valobj qgsh_valobj[] = {
  {V_TYPE, 1, "boolean", 0, 0, MR_TYPE},
  {V_TYPE, 2, "boolean", 0, 0, MR_TYPE},
  {V_TYPE, 3, "boolean", 0, 0, MR_TYPE},
  {V_TYPE, 4, "boolean", 0, 0, MR_TYPE},
  {V_TYPE, 5, "boolean", 0, 0, MR_TYPE},
};

static struct validate qgsh_validate = {
    qgsh_valobj,
    5,
    0,
    0,
    0,
    0,
    0,
    0,
    qualified_get_serverhost,
};

static char *ashi_fields[] = {
  SERVICE, MACHINE, "enable", "value1", "value2", "value3",
};

static struct valobj ashi_valobj[] = {
  {V_LOCK, 0, MACHINE, 0, MACH_ID, MR_DEADLOCK},
  {V_NAME, 0, "servers", NAME, 0, MR_SERVICE},
  {V_ID, 1, MACHINE, NAME, MACH_ID, MR_MACHINE},
};

static struct validate ashi_validate = /* ashi & ushi */
{
  ashi_valobj,
  3,
  SERVICE,
  "serverhosts.service = UPPERCASE(LEFT('%s',SIZE(serverhosts.service))) and serverhosts.mach_id = %d",
  2,
  0,
  access_service,
  0,
  set_serverhost_modtime,
};

static struct validate rshe_validate =	
{
  ashi_valobj,
  3,
  SERVICE,
  "serverhosts.service = uppercase('%s') and serverhosts.mach_id = %d",
  2,
  0,
  access_service,
  0,
  set_serverhost_modtime,
};

static struct validate ssho_validate =	
{
  ashi_valobj,
  3,
  SERVICE,
  "serverhosts.service = uppercase('%s') and serverhosts.mach_id = %d",
  2,
  0,
  access_service,
  0,
  trigger_dcm,
};

static char *sshi_fields[] = {
    "service", MACHINE, "override", "success", "inprogress",
    "hosterror", "errmsg", "ltt", "lts"
};

static struct valobj sshi_valobj[] = {
  {V_NAME, 0, "servers", NAME, 0, MR_SERVICE},
  {V_ID, 1, MACHINE, NAME, MACH_ID, MR_MACHINE},  /* Was this ok to add? */
};

static struct validate sshi_validate =	
{
  sshi_valobj,
  2,
  0,
  0,
  0,
  0,
  0,
  setup_sshi,
  0,
};

static char *dshi_fields[] = {
  SERVICE, MACHINE,
};

static struct validate dshi_validate =	
{
  ashi_valobj,
  3,
  SERVICE,
  "serverhosts.service = uppercase('%s') and serverhosts.mach_id = %d",
  2,
  0,
  access_service,
  setup_dshi,
  0,
};

static char *gsha_fields[] = {
  MACHINE,
  MACHINE, ACE_TYPE, ACE_NAME, MOD1, MOD2, MOD3,
};

static struct validate gsha_validate =	
{
  VOupwild0,
  1,
  0,
  0,
  0,
  0,
  0,
  0,
  followup_gsha,
};

static char *asha_fields[] = {
  MACHINE, ACE_TYPE, ACE_NAME,
};

static struct valobj asha_valobj[] = {
  {V_ID, 0, MACHINE, NAME, MACH_ID, MR_MACHINE},
  {V_TYPE, 1, ACE_TYPE, 0, 0, MR_ACE},
  {V_TYPEDATA, 2, 0, 0, 0, MR_ACE},
};

static struct validate asha_validate =	
{
  asha_valobj,
  3,
  MACH_ID,
  "hostaccess.mach_id = %d",
  1,
  MACH_ID,
  0,
  0,
  set_modtime_by_id,
};

static char *gslo_fields[] = {
  "server",
  "server", MACHINE,
};

static char *gfsl_fields[] = {
  LABEL,
  LABEL, TYPE, MACHINE, NAME, "mount", "access", COMMENTS, "owner", "owners",
  "create", "lockertype", MOD1, MOD2, MOD3,
};

static char *gfsm_fields[] = {
  MACHINE,
  LABEL, TYPE, MACHINE, NAME, "mount", "access", COMMENTS, "owner", "owners",
  "create", "lockertype", MOD1, MOD2, MOD3,
};

static struct validate gfsm_validate = {
  VOmach0,
  1,
  0,
  0,
  0,
  0,
  0,
  0,
  followup_fix_modby,
};

static char *gfsn_fields[] = {
  MACHINE, "parition",
  LABEL, TYPE, MACHINE, NAME, "mount", "access", COMMENTS, "owner", "owners",
  "create", "lockertype", MOD1, MOD2, MOD3,
};

static struct valobj gfsn_valobj[] =
{
  {V_ID, 0, MACHINE, NAME, MACH_ID, MR_MACHINE},
  {V_WILD, 1},
};

static struct validate gfsn_validate = {
  gfsn_valobj,
  2,
  0,
  0,
  0,
  0,
  0,
  0,
  followup_fix_modby,
};

static char *gfsp_fields[] = {
  "path",
  LABEL, TYPE, MACHINE, NAME, "mount", "access", COMMENTS, "owner", "owners",
  "create", "lockertype", MOD1, MOD2, MOD3,
};
 
static char *gfsg_fields[] = {
  LIST,
  LABEL, TYPE, MACHINE, NAME, "mount", "access", COMMENTS, "owner", "owners",
  "create", "lockertype", MOD1, MOD2, MOD3,
};

static struct validate gfsg_validate = {
  VOlist0,
  1,
  0,
  0,
  0,
  0,
  access_list,
  0,
  followup_fix_modby,
};

static char *afil_fields[] = {
  LABEL, TYPE, MACHINE, NAME, "mount", "access", COMMENTS, "owner", "owners",
  "create", "lockertype",
};

static struct valobj afil_valobj[] = {
  {V_CHAR, 0},
  {V_LOCK, 0, FILESYS, 0, FILSYS_ID, MR_DEADLOCK},
  {V_TYPE, 1, FILESYS, 0, 0, MR_FSTYPE},
  {V_ID, 2, MACHINE, NAME, MACH_ID, MR_MACHINE},
  {V_CHAR, 3},
  {V_ID, 7, USERS, LOGIN, USERS_ID, MR_USER},
  {V_ID, 8, LIST, NAME, LIST_ID, MR_LIST},
  {V_TYPE, 10, "lockertype", 0, 0, MR_TYPE}
};

static struct validate afil_validate = {
  afil_valobj,
  8,
  LABEL,
  "filesys.label = LEFT('%s',SIZE(filesys.label))",
  1,
  FILSYS_ID,
  0,
  setup_afil,
  set_filesys_modtime,
};

static char *ufil_fields[] = {
  LABEL, "newlabel", TYPE, MACHINE, NAME, "mount", "access", COMMENTS,
  "owner", "owners", "create", "lockertype",
};

static struct valobj ufil_valobj[] = {
  {V_LOCK, 0, FILESYS, 0, FILSYS_ID, MR_DEADLOCK},
  {V_ID, 0, FILESYS, LABEL, FILSYS_ID, MR_FILESYS},
  {V_RENAME, 1, FILESYS, LABEL, FILSYS_ID, MR_NOT_UNIQUE},
  {V_TYPE, 2, FILESYS, 0, 0, MR_FSTYPE},
  {V_ID, 3, MACHINE, NAME, MACH_ID, MR_MACHINE},
  {V_CHAR, 4},
  {V_ID, 8, USERS, LOGIN, USERS_ID, MR_USER},
  {V_ID, 9, LIST, NAME, LIST_ID, MR_LIST},
  {V_TYPE, 11, "lockertype", 0, 0, MR_TYPE}
};

static struct validate ufil_validate = {
  ufil_valobj,
  9,
  LABEL,
  "filesys.filsys_id = %d",
  1,
  FILSYS_ID,
  0,
  setup_ufil,
  set_filesys_modtime,
};

static char *dfil_fields[] = {
  LABEL,
};

static struct valobj dfil_valobj[] = {
  {V_LOCK, 0, FILESYS, 0, FILSYS_ID, MR_DEADLOCK},
  {V_ID, 0, FILESYS, LABEL, FILSYS_ID, MR_FILESYS},
};

static struct validate dfil_validate = {
  dfil_valobj,
  2,
  "label",
  "filesys.filsys_id = %d",
  1,
  0,
  0,
  setup_dfil,
  0,
};

static char *gfgm_fields[] = {
    "fsgroup", FILESYS, "sortkey"
};

static struct valobj gfgm_valobj[] = {
    {V_ID, 0, FILESYS, LABEL, FILSYS_ID, MR_FILESYS},
    {V_SORT, 1},
    {V_SORT, 0},
};

static struct validate gfgm_validate = {
    gfgm_valobj,
    3,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

static struct valobj aftg_valobj[] = {
    {V_LOCK, 0, FILESYS, 0, FILSYS_ID, MR_DEADLOCK},
    {V_ID, 0, FILESYS, LABEL, FILSYS_ID, MR_FILESYS},
    {V_ID, 1, FILESYS, LABEL, FILSYS_ID, MR_FILESYS},
};

static struct validate aftg_validate = {
    aftg_valobj,
    3,
    "group_id",
    "fsgroup.group_id = %d and fsgroup.filsys_id = %d",
    2,
    0,
    0,
    0,
    0,
};

static char *ganf_fields[] = {
  MACHINE, DIR, DEVICE, STATUS, "allocated", "size", MOD1, MOD2, MOD3,
};

static char *gnfp_fields[] = {
  MACHINE, DIR,
  MACHINE, DIR, DEVICE, STATUS, "allocated", "size", MOD1, MOD2, MOD3,
};

static struct valobj gnfp_valobj[] = {
  {V_ID, 0, MACHINE, NAME, MACH_ID, MR_MACHINE},
  {V_WILD, 1},
};

static struct validate gnfp_validate = {
  gnfp_valobj,
  2,
  0,
  0,
  0,
  0,
  0,
  0,
  followup_fix_modby,
};

static struct valobj anfp_valobj[] = {
  {V_ID, 0, MACHINE, NAME, MACH_ID, MR_MACHINE},
  {V_CHAR, 1},
};

static struct validate anfp_validate = {
  anfp_valobj,
  2,
  DIR,
  "nfsphys.mach_id = %d and nfsphys.dir = LEFT('%s',SIZE(nfsphys.dir))",
  2,
  "nfsphys_id",
  0,
  prefetch_value,
  set_nfsphys_modtime,
};

static struct validate unfp_validate = {
  anfp_valobj,
  2,
  DIR,
  "nfsphys.mach_id = %d and nfsphys.dir = '%s'",
  2,
  "nfsphys_id",
  0,
  0,
  set_nfsphys_modtime,
};

static char *ajnf_fields[] = {
  MACHINE, DIR, "adjustment",
};

static char *dnfp_fields[] = {
  MACHINE, DIR,
};

static struct validate dnfp_validate = {
  VOmach0,
  1,
  DIR,
  "nfsphys.mach_id = %d and nfsphys.dir = '%s'",
  2,
  "nfsphys_id",
  0,
  setup_dnfp,
  set_nfsphys_modtime,
};

static char *gqot_fields[] = {
  FILESYS, TYPE, NAME,
  FILESYS, TYPE, NAME, QUOTA, DIR, MACHINE, MOD1, MOD2, MOD3,
};

static struct valobj gqot_valobj[] = {
  {V_TYPE, 1, QUOTA_TYPE, 0, 0, MR_TYPE},
  {V_TYPEDATA, 2, 0, 0, 0, MR_ACE},
  {V_SORT, 0, 0, 0, 0, 0},
};

static struct validate gqot_validate = {
  gqot_valobj,
  3,
  0,
  0,
  0,
  0,
  0,
  0,
  followup_gqot,
};

static char *gqbf_fields[] = {
  FILESYS,
  FILESYS, TYPE, NAME, QUOTA, DIR, MACHINE, MOD1, MOD2, MOD3,
};

static struct validate gqbf_validate = {
  VOsort0,
  1,
  0,
  0,
  0,
  0,
  access_filesys,
  0,
  followup_gqot,
};

static char *aqot_fields[] = {
  FILESYS, TYPE, NAME, QUOTA,
};

static struct valobj aqot_valobj[] = {
  {V_LOCK, 0, FILESYS, 0, FILSYS_ID, MR_DEADLOCK},
  {V_ID, 0, FILESYS, LABEL, FILSYS_ID, MR_FILESYS},
  {V_TYPE, 1, QUOTA_TYPE, 0, 0, MR_TYPE},
  {V_TYPEDATA, 2, 0, 0, 0, MR_ACE},
};

static struct validate aqot_validate = {
  aqot_valobj,
  4,
  FILSYS_ID,
  "quota.filsys_id = %d and quota.type = '%s' and quota.entity_id = %d",
  3,
  0,
  0,
  prefetch_filesys,
  followup_aqot,
};

static struct validate uqot_validate = {
  aqot_valobj,
  4,
  FILSYS_ID,
  "quota.filsys_id = %d and quota.type = '%s' and quota.entity_id = %d",
  3,
  0,
  0,
  setup_dqot,
  followup_aqot,
};

static struct validate dqot_validate = {
  aqot_valobj,
  4,
  FILSYS_ID,
  "quota.filsys_id = %d and quota.type = '%s' and quota.entity_id = %d",
  3,
  0,
  0,
  setup_dqot,
  0,
};

static char *gnfq_fields[] = {
  FILESYS, LOGIN,
  FILESYS, LOGIN, QUOTA, DIR, MACHINE, MOD1, MOD2, MOD3,
};

static struct validate gnfq_validate = {
  VOwild0sort,
  2,
  0,
  0,
  0,
  0,
  access_filesys,
  0,
  followup_gqot,
};

static char *gnqp_fields[] = {
  MACHINE, DIR,
  FILESYS, LOGIN, QUOTA, DIR, MACHINE, MOD1, MOD2, MOD3,
};

static struct valobj gnqp_valobj[] = {
  {V_ID, 0, MACHINE, NAME, MACH_ID, MR_MACHINE},
  {V_WILD, 1},
};

static struct validate gnqp_validate = { gnqp_valobj, 2, };

static char *anfq_fields[] = {
  FILESYS, LOGIN, QUOTA,
};

static struct validate anfq_validate = {
  VOfilsys0user1,
  3,
  FILSYS_ID,
  "quota.filsys_id = %d and quota.type = 'USER' and quota.entity_id = %d",
  2,
  0,
  0,
  prefetch_filesys,
  followup_aqot,
};

static struct validate unfq_validate = {
  VOfilsys0user1,
  3,
  FILSYS_ID,
  "quota.filsys_id = %d and quota.type = 'USER' and quota.entity_id = %d",
  2,
  0,
  0,
  setup_dqot,
  followup_aqot,
};

static struct validate dnfq_validate = {
  VOfilsys0user1,
  3,
  FILSYS_ID,
  "quota.filsys_id = %d and quota.entity_id = %d",
  2,
  0,
  0,
  setup_dqot,
  0,
};

static char *glin_fields[] = {
  NAME,
  NAME, "active", "publicflg", "hidden", "maillist", "grouplist", "gid",
  ACE_TYPE, ACE_NAME, DESC, MOD1, MOD2, MOD3,
};

static struct validate glin_validate = {
  0,
  0,
  0,
  0,
  0,
  0,
  access_vis_list_by_name,
  0,
  get_list_info,
};

static char *alis_fields[] = {
  NAME, "active", "publicflg", "hidden", "maillist", "grouplist", "gid",
  ACE_TYPE, ACE_NAME, DESC,
};

static struct valobj alis_valobj[] = {
  {V_LOCK, 0, LIST, 0, LIST_ID, MR_DEADLOCK},
  {V_CHAR, 0},
  {V_TYPE, 7, ACE_TYPE, 0, 0, MR_ACE},
  {V_TYPEDATA, 8, 0, 0, LIST_ID, MR_ACE},
};

static struct validate alis_validate = {
  alis_valobj,
  4,
  NAME,
  "list.name = LEFT('%s',SIZE(list.name))",
  1,
  LIST_ID,
  0,
  setup_alis,
  set_modtime,
};

static char *ulis_fields[] = {
  NAME,
  "newname", "active", "publicflg", "hidden", "maillist", "grouplist", "gid",
  ACE_TYPE, ACE_NAME, DESC,
};

static struct valobj ulis_valobj[] = {
  {V_LOCK, 0, LIST, 0, LIST_ID, MR_DEADLOCK},
  {V_ID, 0, LIST, NAME, LIST_ID, MR_LIST},
  {V_RENAME, 1, LIST, NAME, LIST_ID, MR_NOT_UNIQUE},
  {V_TYPE, 8, ACE_TYPE, 0, 0, MR_ACE},
  {V_TYPEDATA, 9, 0, 0, LIST_ID, MR_ACE},
};

static struct validate ulis_validate = {
  ulis_valobj,
  5,
  NAME,
  "list.list_id = %d",
  1,
  LIST_ID,
  access_list,
  setup_alis,
  set_modtime_by_id,
};

static char *dlis_fields[] = {
  NAME,
};

static struct validate dlis_validate = {
  VOlist0,
  1,
  NAME,
  "list.list_id = %d",
  1,
  0,
  access_list,
  setup_dlis,
  0,
};

static char *amtl_fields[] = {
  "list_name", "member_type", "member_name",
};

static struct valobj amtl_valobj[] = {
  {V_LOCK, 0, LIST, 0, LIST_ID, MR_DEADLOCK},
  {V_ID, 0, LIST, NAME, LIST_ID, MR_LIST},
  {V_TYPE, 1, "member", 0, 0, MR_TYPE},
  {V_TYPEDATA, 2, 0, 0, 0, MR_NO_MATCH},
};

static struct validate amtl_validate =
{
  amtl_valobj,
  4,
  0,
  0,
  0,
  0,
  access_list,
  0,
  add_member_to_list,
};

static struct validate dmfl_validate =
{
  amtl_valobj,
  4,
  0,
  0,
  0,
  0,
  access_list,
  0,
  delete_member_from_list,
};

static char *gaus_fields[] = {
    ACE_TYPE, ACE_NAME,
    "use_type", "use_name",
};

static struct valobj gaus_valobj[] = {
  {V_TYPE, 0, "gaus", 0, 0, MR_TYPE},
  {V_TYPEDATA, 1, 0, 0, 0, MR_NO_MATCH},
};

static struct validate gaus_validate = {
    gaus_valobj,
    2,
    0,
    0,
    0,
    0,
    access_member,
    0,
    get_ace_use,
};

static char *qgli_fields[] = {
    "active", "publicflg", "hidden", "maillist", "grouplist",
    "list",
};

static struct valobj qgli_valobj[] = {
  {V_TYPE, 0, "boolean", 0, 0, MR_TYPE},
  {V_TYPE, 1, "boolean", 0, 0, MR_TYPE},
  {V_TYPE, 2, "boolean", 0, 0, MR_TYPE},
  {V_TYPE, 3, "boolean", 0, 0, MR_TYPE},
  {V_TYPE, 4, "boolean", 0, 0, MR_TYPE},
};

static struct validate qgli_validate = {
    qgli_valobj,
    5,
    0,
    0,
    0,
    0,
    access_qgli,
    0,
    qualified_get_lists,
};

static char *gmol_fields[] = {
  "list_name",
  "member_type", "member_name",
};

static struct validate gmol_validate = {
  VOlist0,
  1,
  0,
  0,
  0,
  0,
  access_visible_list,
  0,
  get_members_of_list,
};

static struct validate geml_validate = {
  VOlist0,
  1,
  0,
  0,
  0,
  0,
  access_visible_list,
  0,
  get_end_members_of_list,
};
 
static char *glom_fields[] = {
  "member_type", "member_name",
  "list_name", "active", "publicflg", "hidden", "maillist", "grouplist",
};

static struct valobj glom_valobj[] = {
  {V_TYPE, 0, "rmember", 0, 0, MR_TYPE},
  {V_TYPEDATA, 1, 0, 0, 0, MR_NO_MATCH},
  {V_SORT, 0},
};

static struct validate glom_validate = {
  glom_valobj,
  3,
  0,
  0,
  0,
  0,
  access_member,
  0,
  get_lists_of_member,
};

static char *cmol_fields[] = {
  "list_name",
  "count",
};

static struct validate cmol_validate = {
  VOlist0,
  1,
  0,
  0,
  0,
  0,
  access_visible_list,
  0,
  count_members_of_list,
};

static char *gzcl_fields[] = {
    CLASS,
    CLASS, "xmt_type", "xmt_name", "sub_type", "sub_name",
    "iws_type", "iws_name", "iui_type", "iui_name", MOD1, MOD2, MOD3, 
};

static struct validate gzcl_validate = {
  VOwild0sort,
  2,
  0,
  0,
  0,
  0,
  0,
  0,
  followup_gzcl,
};

static char *azcl_fields[] = {
    CLASS, "xmt_type", "xmt_name", "sub_type", "sub_name",
    "iws_type", "iws_name", "iui_type", "iui_name",
};

static struct valobj azcl_valobj[] = {
  {V_CHAR, 0},
  {V_TYPE, 1, ACE_TYPE, 0, 0, MR_ACE},
  {V_TYPEDATA, 2, 0, 0, LIST_ID, MR_ACE},
  {V_TYPE, 3, ACE_TYPE, 0, 0, MR_ACE},
  {V_TYPEDATA, 4, 0, 0, LIST_ID, MR_ACE},
  {V_TYPE, 5, ACE_TYPE, 0, 0, MR_ACE},
  {V_TYPEDATA, 6, 0, 0, LIST_ID, MR_ACE},
  {V_TYPE, 7, ACE_TYPE, 0, 0, MR_ACE},
  {V_TYPEDATA, 8, 0, 0, LIST_ID, MR_ACE},
};

static struct validate azcl_validate = {
  azcl_valobj,
  9,
  CLASS,
  "zephyr.class = LEFT('%s',SIZE(zephyr.class))",
  1,
  0,
  0,
  0,
  set_zephyr_modtime,
};

static char *uzcl_fields[] = {
    CLASS, "newclass", "xmt_type", "xmt_name", "sub_type", "sub_name",
    "iws_type", "iws_name", "iui_type", "iui_name",
};

static struct valobj uzcl_valobj[] = {
  {V_RENAME, 1, "zephyr", CLASS, 0, MR_NOT_UNIQUE},
  {V_TYPE, 2, ACE_TYPE, 0, 0, MR_ACE},
  {V_TYPEDATA, 3, 0, 0, LIST_ID, MR_ACE},
  {V_TYPE, 4, ACE_TYPE, 0, 0, MR_ACE},
  {V_TYPEDATA, 5, 0, 0, LIST_ID, MR_ACE},
  {V_TYPE, 6, ACE_TYPE, 0, 0, MR_ACE},
  {V_TYPEDATA, 7, 0, 0, LIST_ID, MR_ACE},
  {V_TYPE, 8, ACE_TYPE, 0, 0, MR_ACE},
  {V_TYPEDATA, 9, 0, 0, LIST_ID, MR_ACE},
};

static struct validate uzcl_validate = {
  uzcl_valobj,
  9,
  CLASS,
  "zephyr.class = '%s'",
  1,
  0,
  0,
  0,
  set_zephyr_modtime,
};

static struct validate dzcl_validate = {
  0,
  0,
  CLASS,
  "zephyr.class = '%s'",
  1,
  0,
  0,
  0,
  0,
};

static char *gsvc_fields[] = {
  SERVICE,
  SERVICE, "protocol", "port", DESC, MOD1, MOD2, MOD3
};

static char *asvc_fields[] = {
  SERVICE, "protocol", "port", DESC
};

static struct validate asvc_validate = {
  VOchar0,
  1,
  NAME,
  "services.name = LEFT('%s',SIZE(services.name))",
  1,
  0,
  0,
  0,
  set_modtime,
};

static char *gpce_fields[] = {
    "printer",
    "printer", "spooling_host", "spool_directory", "rprinter",
    "quotaserver", "authenticate", "price", COMMENTS,
    MOD1, MOD2, MOD3,
};

static struct validate gpce_validate = {
  VOwild0sort,
  2,
  0,
  0,
  0,
  0,
  0,
  0,
  followup_gpce,
};

static char *apce_fields[] = {
    "printer", "spooling_host", "spool_directory", "rprinter",
    "quotaserver", "authenticate", "price", COMMENTS,
    MOD1, MOD2, MOD3,
};

static struct valobj apce_valobj[] = {
    {V_CHAR, 0},
    {V_ID, 1, MACHINE, NAME, MACH_ID, MR_MACHINE},
    {V_ID, 4, MACHINE, NAME, MACH_ID, MR_MACHINE},
};

static struct validate apce_validate = {
  apce_valobj,
  3,
  NAME,
  "printcap.name = LEFT('%s',SIZE(printcap.name))",
  1,
  0,
  0,
  0,
  set_modtime,
};

static struct validate dpce_validate = {
  0,
  0,
  NAME,
  "printcap.name = '%s'",
  1,
  0,
  0,
  0,
  0,
};

static char *gpcp_fields[] = {
    "printer",
    "printer", "spooling_host", "spool_directory", "rprinter", COMMENTS,
    MOD1, MOD2, MOD3,
};

static char *gpdm_fields[] = {
    NAME,
    NAME, "rpcnum", "host", MOD1, MOD2, MOD3,
};

static char *apdm_fields[] = {
    NAME, "rpcnum", "host"
};

static struct valobj apdm_valobj[] = {
    {V_CHAR, 0},
    {V_ID, 2, MACHINE, NAME, MACH_ID, MR_MACHINE},
};

static struct validate apdm_validate = {
  apdm_valobj,
  2,
  NAME,
  "palladium.name = LEFT('%s',SIZE(palladium.name))",
  1,
  0,
  0,
  0,
  set_modtime,
};

static struct validate dpdm_validate = {
  0,
  0,
  NAME,
  "palladium.name = '%s'",
  1,
  0,
  0,
  0,
  0,
};

static char *gali_fields[] = {
  NAME, TYPE, "trans",
  NAME, TYPE, "trans",
};

static char *aali_fields[] = {
  NAME, TYPE, "trans",
};

static struct valobj aali_valobj[] = {
  {V_CHAR, 0},
  {V_TYPE, 1, "alias", 0, 0, MR_TYPE},
  {V_CHAR, 2},
};

static struct validate aali_validate = {
  aali_valobj,
  3,
  "trans",
  "alias.name = LEFT('%s',SIZE(alias.name)) and alias.type = '%s' and alias.trans = LEFT('%s',SIZE(alias.trans))",
  3,
  0,
  0,
  0,
  0,
};

static struct validate dali_validate =	/* DELETE_ALIAS */
{
  0,
  0,
  "trans",
  "alias.name = '%s' and alias.type = '%s' and alias.trans = '%s'",
  3,
  0,
  0,
  0,
  0,
};

static char *gval_fields[] = {
  NAME, "value",
};

static struct valobj gval_valobj[] = {
  {V_NAME, 0, "numvalues", NAME, 0, MR_NO_MATCH},
};

static struct validate gval_validate = {
  gval_valobj,
  1,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
};

static char *aval_fields[] = {
  NAME, "value",
};

static struct validate aval_validate =	/* for aval, uval, and dval */
{
  VOchar0,
  1,
  NAME,
  "numvalues.name = LEFT('%s',SIZE(numvalues.name))",  /* LEFT() only needed for aval */
  1,
  0,
  0,
  0,
  0,
};

static char *dval_fields[] = {
  NAME,
};

static char *gats_fields[] = {
  "table_name", "appends", "updates", "deletes", MOD1, MOD2, MOD3,
};



/* Generalized Query Definitions */

/* Note: For any query which uses prefetch_value, the vcnt should be
 * one less than the number of %-format specifiers in the tlist.
 */

struct query Queries2[] = {
  {
    /* Q_GALO - GET_ALL_LOGINS */
    "get_all_logins",
    "galo",
    RETRIEVE,
    "u",
    USERS,
    "CHAR(login), CHAR(uid), shell, CHAR(last), CHAR(first), middle FROM users",
    galo_fields,
    6,
    "users_id != 0",
    0,
    &VDsort0,
  },

  {
    /* Q_GAAL - GET_ALL_ACTIVE_LOGINS */
    "get_all_active_logins",
    "gaal",
    RETRIEVE,
    "u",
    USERS,
    "CHAR(login), CHAR(uid), shell, CHAR(last), CHAR(first), middle FROM users",
    galo_fields,
    6,
    "status = 1",
    0,
    &VDsort0,
  },

  {
      /* Q_GUAL - GET_USER_ACCOUNT_BY_LOGIN */
      "get_user_account_by_login",
      "gual",
      RETRIEVE,
      "u",
      USERS,
      "CHAR(u.login), CHAR(u.uid), u.shell, CHAR(u.last), CHAR(u.first), u.middle, CHAR(u.status), CHAR(u.clearid), u.type, str.string, CHAR(u.signature), CHAR(u.secure), CHAR(u.modtime), CHAR(u.modby), u.modwith FROM users u, strings str",
      gual_fields,
      15,
      "u.login LIKE '%s' ESCAPE '*' AND u.users_id != 0 AND u.comment = str.string_id",
      1,
      &gubl_validate,
  },
 
  {
      /* Q_GUAU - GET_USER_ACCOUNT_BY_UID */
      "get_user_account_by_uid",
      "guau",
      RETRIEVE,
      "u",
      USERS,
      "CHAR(u.login), CHAR(u.uid), u.shell, CHAR(u.last), CHAR(u.first), u.middle, CHAR(u.status), CHAR(u.clearid), u.type, str.string, CHAR(u.signature), CHAR(u.secure), CHAR(u.modtime), CHAR(u.modby), u.modwith FROM users u, strings str",
      guau_fields,
      15,
      "u.uid = %s AND u.users_id != 0 AND u.comment = str.string_id",
      1,
      &gubu_validate,
  },
  
  {
      /* Q_GUAN - GET_USER_ACCOUNT_BY_NAME */
      "get_user_account_by_name",
      "guan",
      RETRIEVE,
      "u",
      USERS,
      "CHAR(u.login), CHAR(u.uid), u.shell, CHAR(u.last), CHAR(u.first), u.middle, CHAR(u.status), CHAR(u.clearid), u.type, str.string, CHAR(u.signature), CHAR(u.secure), CHAR(u.modtime), CHAR(u.modby), u.modwith FROM users u, strings str",
      guan_fields,
      15,
      "u.first LIKE '%s' ESCAPE '*' AND u.last LIKE '%s' ESCAPE '*' AND u.users_id != 0 and u.comment = str.string_id",
      2,
      &VDwild2sortf,
  },
 
  {
      /* Q_GUAC - GET_USER_ACCOUNT_BY_CLASS */
      "get_user_account_by_class",
      "guac",
      RETRIEVE,
      "u",
      USERS,
      "CHAR(u.login), CHAR(u.uid), u.shell, CHAR(u.last), CHAR(u.first), u.middle, CHAR(u.status), CHAR(u.clearid), u.type, str.string, CHAR(u.signature), CHAR(u.secure), CHAR(u.modtime), CHAR(u.modby), u.modwith FROM users u, strings str",
      guac_fields,
      15,
      "u.type = UPPERCASE('%s') AND u.users_id != 0 AND u.comment = str.string_id",
      1,
      &VDsortf,
  },
 
  {
      /* Q_GUAM - GET_USER_ACCOUNT_BY_MITID */
      "get_user_account_by_id",
      "guai",
      RETRIEVE,
      "u",
      USERS,
      "CHAR(u.login), CHAR(u.uid), u.shell, CHAR(u.last), CHAR(u.first), u.middle, CHAR(u.status), CHAR(u.clearid), u.type, str.string, CHAR(u.signature), CHAR(u.secure), CHAR(u.modtime), CHAR(u.modby), u.modwith FROM users u, strings str",
      guam_fields,
      15,
      "u.clearid LIKE '%s' ESCAPE '*' AND u.users_id != 0 AND u.comment = str.string_id",
      1,
      &VDwildsortf,
  },
 
  {
    /* Q_GUBL - GET_USER_BY_LOGIN */
    "get_user_by_login",
    "gubl",
    RETRIEVE,
    "u",
    USERS,  
    "CHAR(login), CHAR(uid), shell, CHAR(last), CHAR(first), middle, CHAR(status), CHAR(clearid), type, CHAR(modtime), CHAR(modby), modwith FROM users",
    gubl_fields,
    12,
    "login LIKE '%s' ESCAPE '*' AND users_id != 0",
    1,
    &gubl_validate,
  },

  {
    /* Q_GUBU - GET_USER_BY_UID */
    "get_user_by_uid",
    "gubu",
    RETRIEVE,
    "u",
    USERS,
    "CHAR(login), CHAR(uid), shell, CHAR(last), CHAR(first), middle, CHAR(status), CHAR(clearid), type, CHAR(modtime), CHAR(modby), modwith FROM users",
    gubu_fields,
    12,
    "uid = %s AND users_id != 0",
    1,
    &gubu_validate,
  },

  {
    /* Q_GUBN - GET_USER_BY_NAME */
    "get_user_by_name",
    "gubn",
    RETRIEVE,
    "u",
    USERS,
    "CHAR(login), CHAR(uid), shell, CHAR(last), CHAR(first), middle, CHAR(status), CHAR(clearid), type, CHAR(modtime), CHAR(modby), modwith FROM users",
    gubn_fields,
    12,
    "first LIKE '%s' ESCAPE '*' AND last LIKE '%s' ESCAPE '*' AND users_id != 0",
    2,
    &gubn_validate,
  },

  {
    /* Q_GUBC - GET_USER_BY_CLASS */
    "get_user_by_class",
    "gubc",
    RETRIEVE,
    "u",
    USERS,
    "CHAR(login), CHAR(uid), shell, CHAR(last), CHAR(first), middle, CHAR(status), CHAR(clearid), type, CHAR(modtime), CHAR(modby), modwith FROM users",
    gubc_fields,
    12,
    "type = uppercase('%s') AND users_id != 0",
    1,
    &VDsortf,
  },

  {
    /* Q_GUBM - GET_USER_BY_MITID */
    "get_user_by_mitid",
    "gubm",
    RETRIEVE,
    "u",
    USERS,
    "CHAR(login), CHAR(uid), shell, CHAR(last), CHAR(first), middle, CHAR(status), CHAR(clearid), type, CHAR(modtime), CHAR(modby), modwith FROM users",
    gubm_fields,
    12,
    "clearid LIKE '%s' ESCAPE '*' AND users_id != 0",
    1,
    &VDwildsortf,
  },

  {
    /* Q_AUAC - ADD_USER_ACCOUNT */  /* uses prefetch_value() for users_id */
    "add_user_account",
    "auac",
    APPEND,
    "u",
    USERS,
    "INTO users (login, uid, shell, last, first, middle, status, clearid, type, comment, signature, secure, users_id) VALUES ('%s', %s, '%s', '%s', '%s', '%s', %s, '%s', '%s', %d, '%s', %s, %s)",
    auac_fields,
    12,
    (char *)0,
    0,
    &auac_validate,
  },
 
  {
    /* Q_AUSR - ADD_USER */  /* uses prefetch_value() for users_id */
    "add_user",
    "ausr",
    APPEND,
    "u",
    USERS,
    "INTO users (login, uid, shell, last, first, middle, status, clearid, type, comment, signature, secure, users_id) VALUES ('%s', %s, '%s', '%s', '%s', '%s', %s, '%s', '%s', 0, '', 0, %s)",
    auac_fields,
    9,
    0,
    0,
    &ausr_validate,
  },

  {
    /* Q_RUSR - REGISTER_USER */
    "register_user",
    "rusr",
    APPEND,
    0,
    0,
    0,
    rusr_fields,
    3,
    0,
    0,
    &rusr_validate,
  },

  {
    /* Q_UUAC - UPDATE_USER_ACCOUNT */
    "update_user_account",
    "uuac",
    UPDATE,
    "u",
    USERS,
    "users SET login = '%s', uid = %s, shell = '%s', last = '%s', first = '%s', middle = '%s', status = %s, clearid = '%s', type = '%s', comment = %d, signature = '%s', secure = %s",
    uuac_fields,
    12,
    "users_id = %d",
    1,
    &uuac_validate,
  },
 
  {
    /* Q_UUSR - UPDATE_USER */
    "update_user",
    "uusr",
    UPDATE,
    "u",
    USERS,
    "users SET login = '%s', uid = %s, shell = '%s', last = '%s', first = '%s', middle = '%s', status = %s, clearid = '%s',  type = '%s'",
    uuac_fields,
    9,
    "users_id = %d",
    1,
    &uusr_validate,
  },

  {
    /* Q_UUSH - UPDATE_USER_SHELL */
    "update_user_shell",
    "uush",
    UPDATE,
    "u",
    USERS,
    "users SET shell = '%s'",  
    uush_fields,
    1,
    "users_id = %d",
    1,
    &uush_validate,
  },

  {
    /* Q_UUST - UPDATE_USER_STATUS */
    "update_user_status",
    "uust",
    UPDATE,
    "u",
    USERS,
    "users SET status = %s",
    uust_fields,
    1,
    "users_id = %d",
    1,
    &uust_validate,
  },

  {
    /* Q_UUSS - UPDATE_USER_SECURITY_STATUS */
    "update_user_security_status",
    "uuss",
    UPDATE,
    "u",
    USERS,
    "secure = %s",
    uuss_fields,
    1,
    "users_id = %d",
    1,
    &uust_validate,
  },
 
  {
    /* Q_DUSR - DELETE_USER */ 
    "delete_user",
    "dusr",
    DELETE,
    "u",
    USERS,
    (char *)0,
    dusr_fields,
    0,
    "users_id = %d",
    1,
    &dusr_validate,
  },

  {
    /* Q_DUBU - DELETE_USER_BY_UID */
    "delete_user_by_uid",
    "dubu",
    DELETE,
    "u",
    USERS,
    (char *)0,
    dubu_fields,
    0,
    "users_id = %d",
    1,
    &dubu_validate,
  },

  {
    /* Q_GKUM - GET_KERBEROS_USER_MAP */ 
    "get_kerberos_user_map",
    "gkum",
    RETRIEVE,
    "k",
    "krbmap",
    "CHAR(u.login), CHAR(s.string) FROM krbmap k, users u, strings s",
    gkum_fields,
    2,
    "u.login LIKE '%s' ESCAPE '*' AND s.string LIKE '%s' ESCAPE '*' AND k.users_id = u.users_id AND k.string_id = s.string_id",
    2,
    &VDwild2sort2,
  },

  {
    /* Q_AKUM - ADD_KERBEROS_USER_MAP */
    "add_kerberos_user_map",
    "akum",
    APPEND,
    "k",
    "krbmap",
    "INTO krbmap (users_id, string_id) VALUES (%d, %d)",
    akum_fields,
    2,
    0,
    0,
    &akum_validate,
  },

  {
    /* Q_DKUM - DELETE_KERBEROS_USER_MAP */
    "delete_kerberos_user_map",
    "dkum",
    DELETE,
    "k",
    "krbmap",
    0,
    akum_fields,
    0,
    "users_id = %d AND string_id = %d",
    2,
    &dkum_validate,
  },

  {
    /* Q_GFBL - GET_FINGER_BY_LOGIN */
    "get_finger_by_login",
    "gfbl",
    RETRIEVE,
    "u",
    USERS,
    "CHAR(login), fullname, nickname, home_addr, home_phone, office_addr, office_phone, department, affiliation, CHAR(fmodtime), CHAR(fmodby), fmodwith FROM users",
    gfbl_fields,
    12,
    "users_id = %d",
    1,
    &gfbl_validate,
  },

  {
    /* Q_UFBL - UPDATE_FINGER_BY_LOGIN */
    "update_finger_by_login",
    "ufbl",
    UPDATE,
    "u",
    USERS,
    "users SET fullname= '%s',nickname= '%s',home_addr= '%s',home_phone= '%s',office_addr= '%s',office_phone= '%s',department= '%s',affiliation= '%s'",
    ufbl_fields,
    8,
    "users_id = %d",
    1,
    &ufbl_validate,
  },

  {
    /* Q_GPOB - GET_POBOX */
    "get_pobox",
    "gpob",
    RETRIEVE,
    "u",
    USERS,
    "CHAR(login), potype, CHAR(pop_id) + ':' + CHAR(box_id), CHAR(pmodtime), CHAR(pmodby), pmodwith FROM users",
    gpob_fields,
    6,
    "users_id = %d",
    1,
    &gpob_validate,
  },

  {
    /* Q_GAPO - GET_ALL_POBOXES */
    "get_all_poboxes",
    "gapo",
    RETRIEVE,
    "u",
    USERS,
    "CHAR(login), potype, CHAR(pop_id) + ':' + CHAR(box_id) FROM users",
    gpox_fields,
    3,
    "potype != 'NONE'",
    0,
    &gpox_validate,
  },
      
  {
    /* Q_GPOP - GET_POBOXES_POP */
    "get_poboxes_pop",
    "gpop",
    RETRIEVE,
    "u",
    USERS,
    "CHAR(login), potype, CHAR(pop_id) + ':' + CHAR(box_id) FROM users",
    gpox_fields,
    3,
    "potype = 'POP'",
    0,
    &gpox_validate
  },

  {
    /* Q_GPOF - GET_POBOXES_SMTP */
    "get_poboxes_smtp",
    "gpos",
    RETRIEVE,
    "u",
    USERS,
    "CHAR(login), potype, CHAR(pop_id) + ':' + CHAR(box_id) FROM users",
    gpox_fields,
    3,
    "potype = 'SMTP'",
    0,
    &gpox_validate
  },

  {
    /* Q_SPOB - SET_POBOX */
    "set_pobox",
    "spob",
    UPDATE,
    0,
    USERS,
    0,
    spob_fields,
    3,
    (char *)0,
    0,
    &spob_validate,
  },

  {
    /* Q_SPOP - SET_POBOX_POP */
    "set_pobox_pop",
    "spop",
    UPDATE,
    "u",
    USERS,
    "users SET potype = 'POP'",
    spob_fields,
    0,
    "users_id = %d",
    1,
    &spop_validate,
  },

  {
    /* Q_DPOB - DELETE_POBOX */
    "delete_pobox",
    "dpob",
    UPDATE,
    "u",
    USERS,
    "users SET potype = 'NONE'",
    spob_fields,
    0,
    "users_id = %d",
    1,
    &dpob_validate,
  },

  {
    /* Q_GMAC - GET_MACHINE */
    "get_machine",
    "gmac",
    RETRIEVE,
    "m",
    MACHINE,
    "CHAR(name), type, CHAR(modtime), CHAR(modby), modwith FROM machine",
    gmac_fields,
    5,
    "name LIKE '%s' ESCAPE '*' AND mach_id != 0", 
    1,
    &VDupwildsortf,
  },

  {
    /* Q_AMAC - ADD_MACHINE */ /* uses prefetch_value() for mach_id */
    "add_machine",
    "amac",
    APPEND,
    "m",
    MACHINE,
    "INTO machine (name, type, mach_id) VALUES (uppercase('%s'),'%s',%s)",
    amac_fields,
    2,
    0,
    0,
    &amac_validate,
  },

  {
    /* Q_UMAC - UPDATE_MACHINE */
    "update_machine",
    "umac",
    UPDATE,
    "m",
    MACHINE,
    "machine SET name = uppercase('%s'), type = '%s'",
    umac_fields,
    2,
    "mach_id = %d",
    1,
    &umac_validate,
  },

  {
    /* Q_DMAC - DELETE_MACHINE */
    "delete_machine",
    "dmac",
    DELETE,
    "m",
    MACHINE,
    (char *)0,
    dmac_fields,
    0,
    "mach_id = %d",
    1,
    &dmac_validate,
  },

  {
    /* Q_GCLU - GET_CLUSTER */
    "get_cluster",
    "gclu",
    RETRIEVE,
    "c",
    CLUSTER,
    "CHAR(name), description, location, CHAR(modtime), CHAR(modby), modwith FROM cluster",
    gclu_fields,
    6,
    "name LIKE '%s' ESCAPE '*' AND clu_id != 0",
    1,
    &VDwildsortf,
  },

  {
    /* Q_ACLU - ADD_CLUSTER */ /* uses prefetch_value() for clu_id */
    "add_cluster",
    "aclu",
    APPEND,
    "c",
    CLUSTER,
    "INTO cluster (name, description, location, clu_id) VALUES ('%s','%s','%s',%s)",
    aclu_fields,
    3,
    0,
    0,
    &aclu_validate,
  },

  {
    /* Q_UCLU - UPDATE_CLUSTER */
    "update_cluster",
    "uclu",
    UPDATE,
    "c",
    CLUSTER,
    "cluster SET name = '%s', description = '%s', location = '%s'",
    uclu_fields,
    3,
    "clu_id = %d",
    1,
    &uclu_validate,
  },

  {
    /* Q_DCLU - DELETE_CLUSTER */
    "delete_cluster",
    "dclu",
    DELETE,
    "c",
    CLUSTER,
    (char *)0,
    dclu_fields,
    0,
    "clu_id = %d",
    1,
    &dclu_validate,
  },

  {
    /* Q_GMCM - GET_MACHINE_TO_CLUSTER_MAP */ 
    "get_machine_to_cluster_map",
    "gmcm",
    RETRIEVE,
    "map",
    "mcmap",
    "CHAR(m.name), CHAR(c.name) FROM machine m, cluster c, mcmap map",
    gmcm_fields,
    2,
    "m.name LIKE '%s' ESCAPE '*' AND c.name LIKE '%s' ESCAPE '*' AND map.clu_id = c.clu_id AND map.mach_id = m.mach_id",
    2,
    &gmcm_validate, 
  },
                                           
  {
    /* Q_AMTC - ADD_MACHINE_TO_CLUSTER */
    "add_machine_to_cluster",
    "amtc",
    APPEND,
    "m",
    "mcmap",
    "INTO mcmap (mach_id, clu_id) VALUES (%d, %d)",
    gmcm_fields,
    2,
    0,
    0,
    &amtc_validate,
  },

  {
    /* Q_DMFC - DELETE_MACHINE_FROM_CLUSTER */
    "delete_machine_from_cluster",
    "dmfc",
    DELETE,
    "m",
    "mcmap",
    0,
    gmcm_fields,
    0,
    "mach_id = %d AND clu_id = %d",
    2,
    &amtc_validate,
  },

  {
    /* Q_GCLD - GET_CLUSTER_DATA */
    "get_cluster_data",
    "gcld",
    RETRIEVE,
    "s",
    "svc",
    "CHAR(c.name), CHAR(s.serv_label), s.serv_cluster FROM svc s, cluster c",
    gcld_fields,
    3,
    "c.clu_id = s.clu_id AND c.name LIKE '%s' ESCAPE '*' AND s.serv_label LIKE '%s' ESCAPE '*'",
    2,
    &VDwild2sort2,
  },

  {
    /* Q_ACLD - ADD_CLUSTER_DATA */
    "add_cluster_data",
    "acld",
    APPEND,
    "s",
    "svc",
    "INTO svc (clu_id, serv_label, serv_cluster) VALUES (%d,'%s','%s')",
    acld_fields,
    3,
    (char *)0,
    0,
    &acld_validate,
  },

  {
    /* Q_DCLD - DELETE_CLUSTER_DATA */
    "delete_cluster_data",
    "dcld",
    DELETE,
    "s",
    "svc",
    (char *)0,
    acld_fields,
    0,
    "clu_id = %d AND serv_label = '%s' AND serv_cluster = '%s'",   
    3,
    &dcld_validate,
  },

  {
    /* Q_GSIN - GET_SERVER_INFO */
    "get_server_info",
    "gsin",
    RETRIEVE,
    "s",
    "servers",
    "CHAR(name), CHAR(update_int), target_file, script, CHAR(dfgen), CHAR(dfcheck), type, CHAR(enable), CHAR(inprogress), CHAR(harderror), errmsg, acl_type, CHAR(acl_id), CHAR(modtime), CHAR(modby), modwith FROM servers",
    gsin_fields,
    16,
    "name LIKE '%s' ESCAPE '*'",
    1,
    &gsin_validate,
  },

  {
    /* Q_QGSV - QUALIFIED_GET_SERVER */
    "qualified_get_server",
    "qgsv",
    RETRIEVE,
    0,
    "servers",
    0,
    qgsv_fields,
    1,
    0,
    3,
    &qgsv_validate,
  },

  {
    /* Q_ASIN - ADD_SERVER_INFO */
    "add_server_info",
    "asin",
    APPEND,
    "s",
    "servers",
    "INTO servers (name, update_int, target_file, script, type, enable, acl_type, acl_id) VALUES (uppercase('%s'),%s,'%s','%s','%s',%s,'%s',%d)",
    asin_fields,
    8,
    (char *)0,
    0,
    &asin_validate,
  },

  {
    /* Q_USIN - UPDATE_SERVER_INFO */
    "update_server_info",
    "usin",
    UPDATE,
    "s",
    "servers",
    "servers SET update_int = %s, target_file = '%s', script = '%s', type = '%s', enable = %s, acl_type = '%s', acl_id = %d",
    asin_fields,
    7,
    "name = uppercase('%s')",
    1,
    &asin_validate,
  },

  {
    /* Q_RSVE - RESET_SERVER_ERROR */
    "reset_server_error",
    "rsve",
    UPDATE,
    "s",
    "servers",
    "servers SET harderror = 0, dfcheck = dfgen",
    dsin_fields,
    0,
    "name = uppercase('%s')",
    1,
    &rsve_validate,
  },

  {
    /* Q_SSIF - SET_SERVER_INTERNAL_FLAGS */
    "set_server_internal_flags",
    "ssif",
    UPDATE,
    "s",
    "servers",
    "servers SET dfgen = %s, dfcheck = %s, inprogress = %s, harderror = %s, errmsg = '%s'",
    ssif_fields,
    5,
    "name = uppercase('%s')",
    1,
    &ssif_validate,
  },

  {
    /* Q_DSIN - DELETE_SERVER_INFO */
    "delete_server_info",
    "dsin",
    DELETE,
    "s",
    "servers",
    (char *)0,
    dsin_fields,
    0,
    "name = uppercase('%s')",
    1,
    &dsin_validate,
  },

  {
    /* Q_GSHI - GET_SERVER_HOST_INFO */ 
    "get_server_host_info",
    "gshi",
    RETRIEVE,
    "sh",
    "serverhosts",
    "CHAR(sh.service), CHAR(m.name), CHAR(sh.enable), CHAR(sh.override), CHAR(sh.success), CHAR(sh.inprogress), CHAR(sh.hosterror), sh.hosterrmsg, CHAR(sh.ltt), CHAR(sh.lts), CHAR(sh.value1), CHAR(sh.value2), sh.value3, CHAR(sh.modtime), CHAR(sh.modby), sh.modwith FROM serverhosts sh, machine m",
    gshi_fields,
    16,
    "sh.service LIKE '%s' ESCAPE '*' AND m.name LIKE '%s' ESCAPE '*' AND m.mach_id = sh.mach_id",
    2,
    &gshi_validate,
  },

  {
    /* Q_QGSV - QUALIFIED_GET_SERVER_HOST */
    "qualified_get_server_host",
    "qgsh",
    RETRIEVE,
    0,
    "serverhosts",
    0,
    qgsh_fields,
    2,
    0,
    6,
    &qgsh_validate,
  },

  {
    /* Q_ASHI - ADD_SERVER_HOST_INFO */
    "add_server_host_info",
    "ashi",
    APPEND,
    "sh",
    "serverhosts",
    "INTO serverhosts (service, mach_id, enable, value1, value2, value3) VALUES (uppercase('%s'), %d, %s, %s, %s, '%s')",
    ashi_fields,
    6,
    (char *)0,
    0,
    &ashi_validate,
  },

  {
    /* Q_USHI - UPDATE_SERVER_HOST_INFO */
    "update_server_host_info",
    "ushi",
    UPDATE,
    "sh",
    "serverhosts",
    "serverhosts SET enable = %s, value1 = %s, value2 = %s, value3 = '%s'",
    ashi_fields,
    4,
    "service = uppercase('%s') AND mach_id = %d",
    2,
    &ashi_validate,
  },

  {
    /* Q_RSHE - RESET_SERVER_HOST_ERROR */
    "reset_server_host_error",
    "rshe",
    UPDATE,
    "sh",
    "serverhosts",
    "serverhosts SET hosterror = 0",
    dshi_fields,
    0,
    "service = uppercase('%s') AND mach_id = %d",
    2,
    &rshe_validate,
  },

  {
    /* Q_SSHO - SET_SERVER_HOST_OVERRIDE */
    "set_server_host_override",
    "ssho",
    UPDATE,
    "sh",
    "serverhosts",
    "serverhosts SET override = 1",
    dshi_fields,
    0,
    "service = uppercase('%s') AND mach_id = %d",
    2,
    &ssho_validate,
  },

  {
    /* Q_SSHI - SET_SERVER_HOST_INTERNAL */
    "set_server_host_internal",
    "sshi",
    UPDATE,
    "s",
    "serverhosts",
    "serverhosts SET override = %s, success = %s, inprogress = %s, hosterror = %s, hosterrmsg = '%s', ltt = %s, lts = %s",
    sshi_fields,
    7,
    "service = uppercase('%s') AND mach_id = %d", 
    2,
    &sshi_validate,
  },

  {
    /* Q_DSHI - DELETE_SERVER_HOST_INFO */
    "delete_server_host_info",
    "dshi",
    DELETE,
    "sh",
    "serverhosts",
    (char *)0,
    dshi_fields,
    0,
    "service = uppercase('%s') AND mach_id = %d",
    2,
    &dshi_validate,
  },

  {
    /* Q_GSHA - GET_SERVER_HOST_ACCESS */ 
    "get_server_host_access",
    "gsha",
    RETRIEVE,
    "ha",
    "hostaccess",
    "CHAR(m.name), ha.acl_type, CHAR(ha.acl_id), CHAR(ha.modtime), CHAR(ha.modby), ha.modwith FROM hostaccess ha, machine m",
    gsha_fields,
    6,
    "m.name LIKE '%s' ESCAPE '*' AND ha.mach_id = m.mach_id",
    1,
    &gsha_validate,
  },

  {
    /* Q_ASHA - ADD_SERVER_HOST_ACCESS */
    "add_server_host_access",
    "asha",
    APPEND,
    "ha",
    "hostaccess",
    "INTO hostaccess (mach_id, acl_type, acl_id) VALUES (%d,'%s',%d)",
    asha_fields,
    3,
    0,
    0,
    &asha_validate,
  },

  {
    /* Q_USHA - UPDATE_SERVER_HOST_ACCESS */
    "update_server_host_access",
    "usha",
    UPDATE,
    "ha",
    "hostaccess",
    "hostaccess SET acl_type = '%s', acl_id = %d",
    asha_fields,
    2,
    "mach_id = %d",
    1,
    &asha_validate,
  },

  {
    /* Q_DSHA - DELETE_SERVER_HOST_ACCESS */
    "delete_server_host_access",
    "dsha",
    DELETE,
    "ha",
    "hostaccess",
    0,
    asha_fields,
    0,
    "mach_id = %d",
    1,
    &VDmach,
  },

  {
    /* Q_GSLO - GET_SERVER_LOCATIONS */ 
    "get_server_locations",
    "gslo",
    RETRIEVE,
    "sh",
    "serverhosts",
    "CHAR(sh.service), CHAR(m.name) FROM serverhosts sh, machine m",
    gslo_fields,
    2,
    "sh.service LIKE '%s' ESCAPE '*' AND sh.mach_id = m.mach_id",
    1,
    &VDupwild0,
  },

  {
    /* Q_GFSL - GET_FILESYS_BY_LABEL */ 
    "get_filesys_by_label",
    "gfsl",
    RETRIEVE,
    "fs",
    FILESYS,
    "CHAR(fs.label), fs.type, CHAR(m.name), CHAR(fs.name), fs.mount, fs.access, fs.comments, CHAR(u.login), CHAR(l.name), CHAR(fs.createflg), fs.lockertype, CHAR(fs.modtime), CHAR(fs.modby), fs.modwith FROM filesys fs, machine m, users u, list l",
    gfsl_fields,
    14,
    "fs.label LIKE '%s' ESCAPE '*' AND fs.mach_id = m.mach_id AND fs.owner = u.users_id AND fs.owners = l.list_id",
    1,
    &VDwildsortf,
  },

  {
    /* Q_GFSM - GET_FILESYS_BY_MACHINE */ 
    "get_filesys_by_machine",
    "gfsm",
    RETRIEVE,
    "fs",
    FILESYS,
    "CHAR(fs.label), fs.type, CHAR(m.name), CHAR(fs.name), fs.mount, fs.access, fs.comments, CHAR(u.login), CHAR(l.name), CHAR(fs.createflg), fs.lockertype, CHAR(fs.modtime), CHAR(fs.modby), fs.modwith FROM filesys fs, machine m, users u, list l",
    gfsm_fields,
    14,
    "fs.mach_id = %d AND m.mach_id = fs.mach_id AND fs.owner = u.users_id AND fs.owners = l.list_id",
    1,
    &gfsm_validate,
  },

  {
    /* Q_GFSN - GET_FILESYS_BY_NFSPHYS */ 
    "get_filesys_by_nfsphys",
    "gfsn",
    RETRIEVE,
    "fs",
    FILESYS,
    "CHAR(fs.label), fs.type, CHAR(m.name), CHAR(fs.name), fs.mount, fs.access, fs.comments, CHAR(u.login), CHAR(l.name), CHAR(fs.createflg), fs.lockertype, CHAR(fs.modtime), CHAR(fs.modby), fs.modwith FROM filesys fs, machine m, users u, list l, nfsphys np",
    gfsn_fields,
    14,
    "fs.mach_id = %d AND m.mach_id = fs.mach_id AND fs.owner = u.users_id AND fs.owners = l.list_id AND np.nfsphys_id = fs.phys_id AND np.dir LIKE '%s' ESCAPE '*' AND fs.type = 'NFS'", 
    2,
    &gfsn_validate,
  },

  {
    /* Q_GFSG - GET_FILESYS_BY_GROUP */ 
    "get_filesys_by_group",
    "gfsg",
    RETRIEVE,
    "fs",
    FILESYS,
    "CHAR(fs.label), fs.type, CHAR(m.name), CHAR(fs.name), fs.mount, fs.access, fs.comments, CHAR(u.login), CHAR(l.name), CHAR(fs.createflg), fs.lockertype, CHAR(fs.modtime), CHAR(fs.modby), fs.modwith FROM filesys fs, machine m, users u, list l",
    gfsg_fields,
    14,
    "fs.owners = %d AND m.mach_id = fs.mach_id AND fs.owner = u.users_id AND fs.owners = l.list_id",
    1,
    &gfsg_validate,
  },

  {
    /* Q_GFSP - GET_FILESYS_BY_PATH */
    "get_filesys_by_path",
    "gfsp",
    RETRIEVE,
    "fs",
    FILESYS,
    "CHAR(fs.label), fs.type, CHAR(m.name), CHAR(fs.name), fs.mount, fs.access, fs.comments, CHAR(u.login), CHAR(l.name), CHAR(fs.createflg), fs.lockertype, CHAR(fs.modtime), CHAR(fs.modby), fs.modwith FROM filesys fs, machine m, users u, list l",
    gfsp_fields,
    14,
    "fs.name LIKE '%s' ESCAPE '*' AND m.mach_id = fs.mach_id AND fs.owner = u.users_id AND fs.owners = list_id",
    1,
    &VDwildsortf,
  },
 
  {
    /* Q_AFIL - ADD_FILESYS */ /* uses prefetch_value() for filsys_id */
    "add_filesys",
    "afil",
    APPEND,
    "fs",
    FILESYS,
    "INTO filesys (label, type, mach_id, name, mount, access, comments, owner, owners, createflg, lockertype, filsys_id) VALUES ('%s','%s',%d,'%s','%s','%s','%s',%d,%d,%s,'%s',%s)",
    afil_fields,
    11,
    0,
    0,
    &afil_validate,
  },

  {
    /* Q_UFIL - UPDATE_FILESYS */
    "update_filesys",
    "ufil",
    UPDATE,
    "fs",
    FILESYS,
    "filesys SET label = '%s', type = '%s', mach_id = %d, name = '%s', mount = '%s', access = '%s', comments = '%s', owner = %d, owners = %d, createflg = %s, lockertype = '%s'",
    ufil_fields,
    11,
    "filsys_id = %d",
    1,
    &ufil_validate,      
  },

  {
    /* Q_DFIL - DELETE_FILESYS */
    "delete_filesys",
    "dfil",
    DELETE,
    "fs",
    FILESYS,
    (char *)0,
    dfil_fields,
    0,
    "filsys_id = %d",
    1,
    &dfil_validate,
  },

  {
    /* Q_GFGM - GET_FSGROUP_MEMBERS */ 
    "get_fsgroup_members",
    "gfgm",
    RETRIEVE,
    "fg",
    "fsgroup",
    "CHAR(fs.label), fg.key FROM fsgroup fg, filesys fs",
    gfgm_fields,
    2,
    "fg.group_id = %d AND fs.filsys_id = fg.filsys_id",
    1,
    &gfgm_validate,
  },

  {
    /* Q_AFTG - ADD_FILESYS_TO_FSGROUP */
    "add_filesys_to_fsgroup",
    "aftg",
    APPEND,
    "fg",
    "fsgroup",
    "INTO fsgroup (group_id,filsys_id,key) VALUES (%d, %d, '%s')",
    gfgm_fields,
    3,
    (char *)0,
    0,
    &aftg_validate,
  },

  {
    /* Q_RFFG - REMOVE_FILESYS_FROM_FSGROUP */
    "remove_filesys_from_fsgroup",
    "rffg",
    DELETE,
    "fg",
    "fsgroup",
    (char *)0,
    gfgm_fields,
    0,
    "group_id = %d AND filsys_id = %d",
    2,
    &aftg_validate,
  },

  {
    /* Q_GANF - GET_ALL_NFSPHYS */ 
    "get_all_nfsphys",
    "ganf",
    RETRIEVE,
    "np",
    "nfsphys",
    "CHAR(m.name), CHAR(np.dir), np.device, CHAR(np.status), CHAR(np.allocated), CHAR(np.partsize), CHAR(np.modtime), CHAR(np.modby), np.modwith FROM nfsphys np, machine m",
    ganf_fields,
    9,
    "m.mach_id = np.mach_id",
    0,
    &VDsortf,
  },

  {
    /* Q_GNFP - GET_NFSPHYS */ 
    "get_nfsphys",
    "gnfp",
    RETRIEVE,
    "np",
    "nfsphys",
    "CHAR(m.name), CHAR(np.dir), np.device, CHAR(np.status), CHAR(np.allocated), CHAR(np.partsize), CHAR(np.modtime), CHAR(np.modby), np.modwith FROM nfsphys np, machine m",
    gnfp_fields,
    9,
    "np.mach_id = %d AND np.dir LIKE '%s' ESCAPE '*' AND m.mach_id = np.mach_id", 
    2,
    &gnfp_validate,
  },

  {
    /* Q_ANFP - ADD_NFSPHYS */ /* uses prefetch_value() for nfsphys_id */
    "add_nfsphys",
    "anfp",
    APPEND,
    "np",
    "nfsphys",
    "INTO nfsphys (mach_id, dir, device, status, allocated, partsize, nfsphys_id) VALUES (%d, '%s', '%s', %s, %s, %s, %s)",
    ganf_fields,
    6,
    0,
    0,
    &anfp_validate,
  },

  {
    /* Q_UNFP - UPDATE_NFSPHYS */
    "update_nfsphys",
    "unfp",
    UPDATE,
    "np",
    "nfsphys",
    "nfsphys SET device = '%s', status = '%s', allocated = %s, partsize = %s",
    gnfp_fields,
    4,
    "mach_id = %d AND dir = '%s'", 
    2,
    &unfp_validate,
  },

  {
    /* Q_AJNF - ADJUST_NFSPHYS_ALLOCATION */
    "adjust_nfsphys_allocation",
    "ajnf",
    UPDATE,
    "np",
    "nfsphys",
    "nfsphys SET allocated = allocated + %s",
    ajnf_fields,
    1,
    "mach_id = %d AND dir = '%s'",
    2,
    &unfp_validate,
  },

  {
    /* Q_DNFP - DELETE_NFSPHYS */
    "delete_nfsphys",
    "dnfp",
    DELETE,
    "np",
    "nfsphys",
    (char *)0,
    dnfp_fields,
    0,
    "mach_id = %d AND dir = '%s'",
    2,
    &dnfp_validate,
  },

  {
    /* Q_GQOT - GET_QUOTA */ 
    "get_quota",
    "gqot",
    RETRIEVE,
    "q",
    QUOTA,
    "CHAR(fs.label), q.type, CHAR(q.entity_id), CHAR(q.quota), CHAR(q.phys_id), CHAR(m.name), CHAR(q.modtime), CHAR(q.modby), q.modwith FROM quota q, filesys fs, machine m",
    gqot_fields,
    9,
    "fs.label LIKE '%s' ESCAPE '*' AND q.type = '%s' AND q.entity_id = %d AND fs.filsys_id = q.filsys_id AND m.mach_id = fs.mach_id",
    3,
    &gqot_validate,
  },

  {
    /* Q_GQBF - GET_QUOTA_BY_FILESYS */ 
    "get_quota_by_filesys",
    "gqbf",
    RETRIEVE,
    "q",
    QUOTA,
    "CHAR(fs.label), q.type, CHAR(q.entity_id), CHAR(q.quota), CHAR(q.phys_id), CHAR(m.name), CHAR(q.modtime), CHAR(q.modby), q.modwith FROM quota q, filesys fs, machine m",
    gqbf_fields,
    9,
    "fs.label LIKE '%s' ESCAPE '*' AND fs.filsys_id = q.filsys_id AND m.mach_id = fs.mach_id",
    1,
    &gqbf_validate,
  },

  {
    /* Q_AQOT - ADD_QUOTA */ /* prefetch_filsys() gets last 2 values */
    "add_quota",
    "aqot",
    APPEND,
    "q",
    QUOTA,
    "INTO quota (type, entity_id, quota, filsys_id, phys_id) VALUES ('%s', %d, %s, %s, %s)",
    aqot_fields,
    3,
    (char *)0,
    0,
    &aqot_validate,
  },

  {
    /* Q_UQOT - UPDATE_QUOTA */
    "update_quota",
    "uqot",
    UPDATE,
    "q",
    QUOTA,
    "quota SET quota = %s",
    aqot_fields,
    1,
    "quota.filsys_id = %d AND quota.type = '%s' AND quota.entity_id = %d",
    3,
    &uqot_validate,
  },

  {
    /* Q_DQOT - DELETE_QUOTA */
    "delete_quota",
    "dqot",
    DELETE,
    "q",
    QUOTA,
    (char *)0,
    aqot_fields,
    0,
    "quota.filsys_id = %d AND quota.type = '%s' AND quota.entity_id = %d",
    3,
    &dqot_validate,
  },

  {
    /* Q_GNFQ - GET_NFS_QUOTAS */ 
    "get_nfs_quota",
    "gnfq",
    RETRIEVE,
    "q",
    QUOTA,
    "CHAR(fs.label), CHAR(u.login), CHAR(q.quota), CHAR(q.phys_id), CHAR(m.name), CHAR(q.modtime), CHAR(q.modby), q.modwith FROM quota q, filesys fs, users u, machine m",
    gnfq_fields,
    8,
    "fs.label LIKE '%s' ESCAPE '*' AND q.type = 'USER' AND q.entity_id = u.users_id AND fs.filsys_id = q.filsys_id AND m.mach_id = fs.mach_id AND u.login = '%s'",
    2,
    &gnfq_validate,
  },

  {
    /* Q_GNQP - GET_NFS_QUOTAS_BY_PARTITION */ 
    "get_nfs_quotas_by_partition",
    "gnqp",
    RETRIEVE,
    "q",
    QUOTA,
    "CHAR(fs.label), CHAR(u.login), CHAR(q.quota), CHAR(np.dir), CHAR(m.name) FROM quota q, filesys fs, users u, nfsphys np, machine m",
    gnqp_fields,
    5,
    "np.mach_id = %d AND np.dir LIKE '%s' ESCAPE '*' AND q.phys_id = np.nfsphys_id AND fs.filsys_id = q.filsys_id AND q.type = 'USER' AND u.users_id = q.entity_id AND m.mach_id = np.mach_id",
    2,
    &gnqp_validate,
  },

  {
    /* Q_ANFQ - ADD_NFS_QUOTA */ /* prefetch_filsys() gets last 2 values */
    "add_nfs_quota",
    "anfq",
    APPEND,
    "q",
    QUOTA,
    "INTO quota (type, entity_id, quota, filsys_id, phys_id) VALUES ('USER', %d, %s, %s, %s)",
    anfq_fields,
    2,
    (char *)0,
    0,
    &anfq_validate,
  },

  {
    /* Q_UNFQ - UPDATE_NFS_QUOTA */
    "update_nfs_quota",
    "unfq",
    UPDATE,
    "q",
    QUOTA,
    "quota SET quota = %s",
    anfq_fields,
    1,
    "filsys_id = %d AND type = 'USER' AND entity_id = %d",
    2,
    &unfq_validate,
  },

  {
    /* Q_DNFQ - DELETE_NFS_QUOTA */
    "delete_nfs_quota",
    "dnfq",
    DELETE,
    "q",
    QUOTA,
    (char *)0,
    anfq_fields,
    0,
    "filsys_id = %d AND type = 'USER' AND entity_id = %d",
    2,
    &dnfq_validate,
  },

  {
    /* Q_GLIN - GET_LIST_INFO */
    "get_list_info",
    "glin",
    RETRIEVE,
    0,
    LIST,
    0,
    glin_fields,
    13,
    0,
    1,              
    &glin_validate, 
  },
    
  {
    /* Q_EXLN - EXPAND_LIST_NAMES */
    "expand_list_names",
    "exln",
    RETRIEVE,
    "l",
    LIST,
    "CHAR(name) FROM list",
    glin_fields,
    1,
    "name LIKE '%s' ESCAPE '*' AND list_id != 0",
    1,
    &VDwild0,
  },
    
  {
    /* Q_ALIS - ADD_LIST */ /* uses prefetch_value() for list_id */
    "add_list",
    "alis",
    APPEND,
    "l",
    LIST, 
    "INTO list (name, active, publicflg, hidden, maillist, grouplist, gid, acl_type, acl_id, description, list_id) VALUES ('%s',%s,%s,%s,%s,%s,%s,'%s',%d,'%s',%s)",
    alis_fields,
    10,
    0,
    0,
    &alis_validate,
  },

  {
    /* Q_ULIS - UPDATE_LIST */
    "update_list",
    "ulis",
    UPDATE,
    "l",
    LIST,
    "list SET name='%s', active=%s, publicflg=%s, hidden=%s, maillist=%s, grouplist=%s, gid=%s, acl_type='%s', acl_id=%d, description='%s'",
    ulis_fields,
    10,
    "list_id = %d",
    1,
    &ulis_validate,
  },

  {
    /* Q_DLIS - DELETE_LIST */
    "delete_list",
    "dlis",
    DELETE,
    "l",
    LIST,
    (char *)0,
    dlis_fields,
    0,
    "list_id = %d",
    1,
    &dlis_validate,
  },

  {
    /* Q_AMTL - ADD_MEMBER_TO_LIST */
    "add_member_to_list",
    "amtl",
    APPEND,
    0,
    "imembers",
    0,
    amtl_fields,
    3,
    (char *)0,
    0,
    &amtl_validate,
  },

  {
    /* Q_DMFL - DELETE_MEMBER_FROM_LIST */
    "delete_member_from_list",
    "dmfl",
    DELETE,
    0,
    "imembers",
    (char *)0,
    amtl_fields,
    0,
    0,
    3,
    &dmfl_validate,
  },

  {
    /* Q_GAUS - GET_ACE_USE */
    "get_ace_use",
    "gaus",
    RETRIEVE,
    0,
    0,
    0,
    gaus_fields,
    2,
    0,
    2,
    &gaus_validate,
  },

  {
    /* Q_QGLI - QUALIFIED_GET_LISTS */
    "qualified_get_lists",
    "qgli",
    RETRIEVE,
    0,
    LIST,
    0,
    qgli_fields,
    1,
    0,
    5,
    &qgli_validate,
  },

  {
    /* Q_GMOL - GET_MEMBERS_OF_LIST */
    "get_members_of_list",
    "gmol",
    RETRIEVE,
    (char *)0,
    "imembers",
    (char *)0,
    gmol_fields,
    2,
    (char *)0,
    1,
    &gmol_validate,
  },

  {
    /* Q_GEML - GET_END_MEMBERS_OF_LIST */
    "get_end_members_of_list",
    "geml",
    RETRIEVE,
    (char *)0,
    "imembers",
    (char *)0,
    gmol_fields,
    0,
    (char *)0,
    1,
    &geml_validate,
  },
 
  {
    /* Q_GLOM - GET_LISTS_OF_MEMBER */
    "get_lists_of_member",
    "glom",
    RETRIEVE,
    0,
    "imembers",
    0,
    glom_fields,
    6,
    0,
    2,
    &glom_validate,
  },

  {
    /* Q_CMOL - COUNT_MEMBERS_OF_LIST */
    "count_members_of_list",
    "cmol",
    RETRIEVE,
    0,
    "imembers",
    0,
    cmol_fields,
    1,
    0,
    1,
    &cmol_validate,
  },

  {
    /* Q_GZCL - GET_ZEPHYR_CLASS */
    "get_zephyr_class",
    "gzcl",
    RETRIEVE,
    "z",
    "zephyr",
    "CHAR(class), xmt_type, CHAR(xmt_id),sub_type, CHAR(sub_id),iws_type, CHAR(iws_id),iui_type, CHAR(iui_id), CHAR(modtime), CHAR(modby), modwith FROM zephyr",
    gzcl_fields,
    12,
    "class LIKE '%s' ESCAPE '*'",
    1,
    &gzcl_validate,
  },    

  {
    /* Q_AZCL - ADD_ZEPHYR_CLASS */
    "add_zephyr_class",
    "azcl",
    APPEND,
    "z",
    "zephyr",
    "INTO zephyr (class, xmt_type, xmt_id, sub_type, sub_id, iws_type, iws_id, iui_type, iui_id) VALUES ('%s','%s',%d,'%s',%d,'%s',%d,'%s',%d)",
    azcl_fields,
    9,
    0,
    0,
    &azcl_validate,
  },    

  {
    /* Q_UZCL - UPDATE_ZEPHYR_CLASS */
    "update_zephyr_class",
    "uzcl",
    UPDATE,
    "z",
    "zephyr",
    "zephyr SET class = '%s', xmt_type = '%d', xmt_id = %d, sub_type = '%s', sub_id = %d, iws_type = '%s', iws_id = %d, iui_type = '%s', iui_id = %d",
    uzcl_fields,
    9,
    "class = '%s'",
    1,
    &uzcl_validate,
  },    

  {
    /* Q_DZCL - DELETE_ZEPHYR_CLASS */
    "delete_zephyr_class",
    "dzcl",
    DELETE,
    "z",
    "zephyr",
    0,
    uzcl_fields,
    0,
    "class = '%s'",
    1,
    &dzcl_validate,
  },    

  {
    /* Q_GSVC - GET_SERVICE */
    "get_service",
    "gsvc",
    RETRIEVE,
    "s",
    "services",
    "CHAR(name), protocol, CHAR(port), description, CHAR(modtime), CHAR(modby), modwith FROM services",
    gsvc_fields,
    7,
    "name LIKE '%s' ESCAPE '*'",
    1,
    &VDwildsortf,
  },

  {
    /* Q_ASVC - ADD_SERVICE */
    "add_service",
    "asvc",
    APPEND,
    "s",
    "services",
    "INTO services (name, protocol, port, description) VALUES ('%s','%s',%s,'%s')",
    asvc_fields,
    4,
    (char *)0,
    0,
    &asvc_validate,
  },

  {
    /* Q_DSVC - DELETE_SERVICE */
    "delete_service",
    "dsvc",
    DELETE,
    "s",
    "services",
    0,
    asvc_fields,
    0,
    "name = '%s'",
    1,
    &asvc_validate,
  },

  {
    /* Q_GPCE - GET_PRINTCAP_ENTRY */ 
    "get_printcap_entry",
    "gpce",
    RETRIEVE,
    "p",
    "printcap",
    "CHAR(p.name), CHAR(m.name), p.dir, p.rp, CHAR(p.quotaserver), CHAR(p.auth), CHAR(p.price), p.comments, CHAR(p.modtime), CHAR(p.modby), p.modwith FROM printcap p, machine m",
    gpce_fields,
    11,
    "p.name LIKE '%s' ESCAPE '*' AND m.mach_id = p.mach_id",
    1,
    &gpce_validate,
  },

  {
    /* Q_APCE - ADD_PRINTCAP_ENTRY */
    "add_printcap_entry",
    "apce",
    APPEND,
    "p",
    "printcap",
    "INTO printcap (name, mach_id, dir, rp, quotaserver, auth, price, comments) VALUES ('%s',%d,'%s','%s',%d,%s,%s,'%s')",
    apce_fields,
    8,
    0,
    0,
    &apce_validate,
  },

  {
    /* Q_DPCE - DELETE_PRINTCAP_ENTRY */
    "delete_printcap_entry",
    "dpce",
    DELETE,
    "p",
    "printcap",
    0,
    apce_fields,
    0,
    "name = '%s'",
    1,
    &dpce_validate,
  },

  {
    /* Q_GPCP - GET_PRINTCAP */
    "get_printcap",
    "gpcp",
    RETRIEVE,
    "p",
    "printcap",
    "CHAR(p.name), CHAR(m.name), p.dir, p.rp, p.comments, CHAR(p.modtime), CHAR(p.modby), p.modwith FROM printcap p, machine m",
    gpcp_fields,
    8,
    "p.name LIKE '%s' ESCAPE '*' AND m.mach_id = p.mach_id",
    1,
    &VDwildsortf,
  },

  {
    /* Q_DPCP - DELETE_PRINTCAP */
    "delete_printcap",
    "dpcp",
    DELETE,
    "p",
    "printcap",
    0,
    apce_fields,
    0,
    "name = '%s'",
    1,
    &dpce_validate,
  },

  {
    /* Q_GPDM - GET_PALLADIUM */
    "get_palladium",
    "gpdm",
    RETRIEVE,
    "p",
    "palladium",
    "CHAR(p.name), CHAR(p.ident), CHAR(m.name), CHAR(p.modtime), CHAR(p.modby), p.modwith FROM palladium p, machine m",
    gpdm_fields,
    6,
    "p.name LIKE '%s' ESCAPE '*' AND m.mach_id = p.mach_id",
    1,
    &VDwildsortf,
  },

  {
    /* Q_APDM - ADD_PALLADIUM */
    "add_palladium",
    "apdm",
    APPEND,
    "p",
    "palladium",
    "INTO palladium (name, ident, mach_id) VALUES ('%s',%s,%d)",
    apdm_fields,
    3,
    0,
    0,
    &apdm_validate,
  },

  {
    /* Q_DPDM - DELETE_PALLADIUM */
    "delete_palladium",
    "dpdm",
    DELETE,
    "p",
    "palladium",
    0,
    apdm_fields,
    0,
    "name = '%s'",
    1,
    &dpdm_validate,
  },

  {
    /* Q_GALI - GET_ALIAS */
    "get_alias",
    "gali",
    RETRIEVE,
    "a",
    "alias",
    "CHAR(name), CHAR(type), CHAR(trans) FROM alias",
    gali_fields,
    3,
    "name LIKE '%s' ESCAPE '*' AND type LIKE '%s' ESCAPE '*' AND trans LIKE '%s' ESCAPE '*'",
    3,
    &VDwild3sort1,
  },

  {
    /* Q_AALI - ADD_ALIAS */
    "add_alias",
    "aali",
    APPEND,
    "a",
    "alias",
    "INTO alias (name, type, trans) VALUES ('%s', '%s', '%s')",
    aali_fields,
    3,
    (char *)0,
    0,
    &aali_validate,
  },

  {
    /* Q_DALI - DELETE_ALIAS */
    "delete_alias",
    "dali",
    DELETE,
    "a",
    "alias",
    (char *)0,
    aali_fields,
    0,
    "name = '%s' AND type = '%s' AND  trans = '%s'", 
    3,
    &dali_validate,
  },

  {
    /* Q_GVAL - GET_VALUE */
    "get_value",
    "gval",
    RETRIEVE,
    "v",
    "numvalues",
    "CHAR(value) FROM numvalues",
    gval_fields,
    1,
    "name = '%s'",
    1,
    &gval_validate,
  },

  {
    /* Q_AVAL - ADD_VALUE */
    "add_value",
    "aval",
    APPEND,
    "v",
    "numvalues",
    "INTO numvalues (name, value) VALUES ('%s', %s)",
    aval_fields,
    2,
    (char *)0,
    0,
    &aval_validate,
  },

  {
    /* Q_UVAL - UPDATE_VALUE */
    "update_value",
    "uval",
    UPDATE,
    "v",
    "numvalues",
    "numvalues SET value = %s",
    aval_fields,
    1,
    "name = '%s'",
    1,
    &aval_validate,
  },

  {
    /* Q_DVAL - DELETE_VALUE */
    "delete_value",
    "dval",
    DELETE,
    "v",
    "numvalues",
    (char *)0,
    dval_fields,
    0,
    "name = '%s'",
    1,
    &aval_validate,
  },

  {
    /* Q_GATS - GET_ALL_TABLE_STATS */
    "get_all_table_stats",
    "gats",
    RETRIEVE,
    "tbs",
    "tblstats",
    "table_name, CHAR(appends), CHAR(updates), CHAR(deletes), CHAR(modtime) FROM tblstats",
    gats_fields,
    5,
    (char *)0,
    0,
    0,
  },

};

int QueryCount2 = (sizeof Queries2 / sizeof (struct query));