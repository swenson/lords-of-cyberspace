/* global variables */

extern struct   gang_struct             *gang;
extern struct   tourney_struct          *tourney;
extern struct   world_struct            *world;
extern struct   misc_world_struct       *misc_world;
extern struct   tracking_struct         *tracking;
extern struct   port_struct             *Port[MAXGUYS];
extern struct   system_struct           *Sys[MAXSYSTEMS];
extern struct   processor_struct        *Proc[MAXSYSTEMS];
extern struct   city_def_struct         City_Def[];
extern struct   rank_struct             rank[];
extern struct   program_struct          Programs[];
extern struct   sys_type_struct         System[];
extern struct   city_struct             *a_city;
extern struct   human_struct            *a_human;
extern struct   human_struct            *a_user;
extern struct   human_struct            *my_record;
extern struct   system_struct           *a_system;
extern struct   globals_struct          *glob;
extern struct   crashed_struct          *dead;

extern char     msg_text[256];
extern char     str[500];
extern char     parm1[LINE_SIZE];
extern char     parm2[LINE_SIZE];
extern char     parm3[LINE_SIZE];
extern char     temp_userid[UIDSIZ];

extern char     *commands[];
extern char     *y_or_n[];
extern char     *program_class[];
extern char     *document_class[];

extern short    myp;
extern short    command;
extern short    max_guys;
extern short    max_systems;

extern struct vended_struct vendor_programs[];
extern struct vended_struct gang_programs[];
extern struct vended_struct bbs_programs[];
extern struct vended_struct level1_programs[];
extern struct vended_struct level2_programs[];
extern struct vended_struct level3_programs[];
extern struct vended_struct level4_programs[];
extern struct vended_struct level5_programs[];
extern struct vended_struct level6_programs[];
extern struct vended_struct level7_programs[];
extern struct vended_struct hub_programs[];
extern struct vended_struct bank_programs[];
extern struct vended_struct end_programs[];
extern struct vended_struct crime_programs[];
extern struct vended_struct alien_programs[];
extern struct vended_struct web_programs[];
extern struct vended_struct cyberia_programs[];
extern struct vended_struct con_programs[];
extern struct vended_struct user_programs[];

extern char     human_screen_spec[];
extern char     human_edit_format[];
extern char     tourney_screen_spec[];
extern char     tourney_edit_format[];
extern char     gang_screen_spec[];
extern char     gang_edit_format[];
extern char     sys_screen_spec[];
extern char     sys_edit_format[];
extern char     doc_screen_spec[];
extern char     doc_edit_format[];
extern char     prog_screen_spec[];
extern char     prog_edit_format[];



