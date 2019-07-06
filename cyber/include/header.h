/**********************************************************************
*       Lords of Cyberspace                                           *
*       Origin Date: 10/22/92                                         *
*       Last Mod:    11/01/97                                         *
*                                                                     *
*       Programmer:  Mark Peterson                                    *
*                                                                     *
*       Copyright (C) 1997 by Mark Peterson, All Rights Reserved      *
***********************************************************************/

#ifdef GCDOS
#define MSHORT      SHORT
#else
#ifdef GCWINNT
#define MSHORT      SHORT
#else
#define MBBS
#define GCMAXLONG   MAXLONG
#define GBOOL       int
#define MSHORT      int
#endif
#endif

#define VERSION     "2.13"

/* include files */

/*
#include <GCOMM.H>
#include <MAJORBBS.h>
#include <FILEXFER.h>

#ifdef MBBS
#include <MESSAGE.h>
#else
#include <gme.h>
#endif
*/

#include "fakebbs.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <muicyber.h>
#include <unistd.h>
#include <time.h>
/* #include <fsdbbs.h> */

/* Files */

#define REPORT_FILE         "MUICYBER.RPT"
#define GANG_FILE           "muicyber/data/gangs.dat"
#define USER_FILE           "muicyber/data/users.dat"
#define BRIEF_FILE          "muicyber/data/brief.dat"
#define DECK_FILE           "muicyber/data/decks.dat"
#define CITY_FILE           "muicyber/data/cityfile.dat"
#define CUSTOM_FILE         "muicyber/data/custom.dat"
#define DAY_FILE            "muicyber/data/dayfile.dat"
#define FREENET_FILE        "muicyber/data/freenet.dat"
#define TOURNEY_FILE        "muicyber/data/tourney.dat"
#define WINNER_FILE         "muicyber/data/winners.dat"
#define HELP_PATH           "muicyber/data/help"
#define VALHALLA_PATH       "muicyber/data/valhalla"
#define SYSTEM_PATH         "muicyber/data/systems"
#define CUSTOM_PATH         "muicyber/data/custom"
#define MAIL_PATH           "muicyber/data/mail/"
#define GANG_MAIL_FILE      "muicyber/data/gangmail/gang%d.dat"

/* Screen distribution defines */

#define PROMPT_Y            10
#define DISP_Y              11
#define NUM_PREV_MSGS       4
#define ASYNCH_Y            (24 - NUM_PREV_MSGS)

/* Sub States */

#define DOWNLOADING          1
#define MAIN_PROMPT          1001
#define X_PROMPT             1002
#define JACKIN_PROMPT        1004
#define STREET_PROMPT        1005
#define SUICIDE_PROMPT       1006
#define LD_PROMPT            1007
#define Y_PROMPT             1008
#define PICK_CITY_PROMPT1    1009
#define X2_PROMPT            1010
#define Y2_PROMPT            1011
#define NODE_TYPE_PROMPT     1014
#define SYSTEM_NAME_PROMPT   1016
#define PERM_NODE_PROMPT     1017
#define MOVE_PROMPT          1018
#define ERASE_PROMPT         1019
#define EDIT_HUMAN_PROMPT    1020
#define LD_TYPE_PROMPT       1021
#define LD_CODE_PROMPT       1022
#define GO_VENDOR_PROMPT     1023
#define LOCK_PROMPT          1024
#define BUY_PROMPT           1029
#define BUY_CPU_PROMPT1      1030
#define BUY_CPU_PROMPT2      1031
#define BUY_CPU_PROMPT3      1032
#define DOCTOR_PROMPT        1035
#define TACTICAL_PROMPT      1037
#define HYDRA_PROMPT         1038
#define DESIGN_PROMPT        1041
#define DESIGN_X1_PROMPT     1042
#define DESIGN_Y1_PROMPT     1043
#define DESIGN_X2_PROMPT     1044
#define DESIGN_Y2_PROMPT     1045
#define SYS_NAME_PROMPT      1046
#define TRANSFER1_PROMPT     1047
#define TRANSFER2_PROMPT     1048
#define RELOCATE_PROMPT      1049
#define FLEE_PROMPT          1050
#define ANY_KEY_PROMPT       1051
#define READ_PROMPT          1052
#define FENCE_PROMPT         1053
#define SERVICES_PROMPT      1054
#define CHANGE_BANK_PROMPT   1055
#define CHANGE_NET_PROMPT    1056
#define CHANGE_ALIAS_PROMPT  1057
#define NUM_MOVES_PROMPT     1058
#define EYE_PROMPT           1059
#define LEDGER_PROMPT        1060
#define STORE_PROMPT2        1062
#define STORE_PROMPT3        1063
#define RETRIEVE_PROMPT1     1064
#define RETRIEVE_PROMPT2     1065
#define RETRIEVE_PROMPT3     1066
#define STOP_PROMPT          1067
#define DUMP_FILE_PROMPT     1068
#define MEMORY_PROMPT        1069
#define MEMORY_PROMPT2       1070
#define MOVE_PROMPT2         1071
#define PROGRAMS_PROMPT      1072
#define DOCS_PROMPT          1073
#define WHERE_PROMPT         1074
#define STREET_CODE_PROMPT   1075
#define TARGET_GUY_PROMPT    1076
#define DIRECTION_PROMPT     1077
#define TARGET_CONST_PROMPT  1078
#define ZIP_PROMPT           1079
#define LOCKER_PROMPT        1080
#define NEWGUY_PROMPT        1081
#define RUN_PROMPT           1082
#define CONFIGURE_PROMPT     1083
#define INSTALL_PROMPT       1084
#define IMPROVE_PROMPT       1085
#define IMPROVE_PROMPT2      1086
#define IBM_PROMPT           1087
#define CITY_OPTION_PROMPT   1088
#define CITY_NAME_PROMPT     1089
#define SECURITY_PROMPT      1090
#define LANDMASS_PROMPT      1091
#define SYS_LD_PROMPT        1092
#define SYS_LDMOVES_PROMPT   1093
#define EDIT_SYSTEM_PROMPT   1094
#define SYS_OPTION_PROMPT    1095
#define DESIGN_PERM          1096
#define PDESIGN_PROMPT       1097
#define PDESIGN_X1_PROMPT    1098
#define PDESIGN_Y1_PROMPT    1099
#define PDESIGN_X2_PROMPT    1100
#define PDESIGN_Y2_PROMPT    1101
#define PMEMORY_PROMPT       1102
#define PMEMORY_PROMPT2      1103
#define PERASE_PROMPT        1104
#define PCHANGE_PROMPT       1105
#define PADD_PROMPT          1106
#define TRANSFER0_PROMPT     1107
#define OUTLAW_PROMPT        1108
#define PASSWORD_PROMPT      1109
#define MOVE_CPU_PROMPT      1110
#define MOVE_MEM_PROMPT      1111
#define MIMIC_PROMPT         1112
#define MAIL_PROMPT          1113
#define WRITE_MAIL_PROMPT    1114
#define GET_MSG_BODY_PROMPT  1115
#define ERASE_MSG_PROMPT     1116
#define NEXT_MSG_PROMPT      1117
#define FREE_BODY_PROMPT     1118
#define FREENET_PROMPT1      1119
#define FREENET_PROMPT2      1120
#define FREENET_PROMPT3      1121
#define WHO_PROMPT           1122
#define MISSION_PROMPT       1123
#define GANG_PROMPT1         1124
#define GANG_PROMPT2         1125
#define MORE_GANG_PROMPT     1126
#define NOMORE_GANG_PROMPT   1127
#define RESIGN_PROMPT        1128
#define MEMBERS_PROMPT       1129
#define MORE_MBR_PROMPT      1130
#define NOMORE_MBR_PROMPT    1131
#define PICK_CITY_PROMPT2    1132
#define INVITE_PROMPT        1133
#define ACCEPT_PROMPT        1134
#define KICK_PROMPT          1135
#define GANG_NAME_PROMPT     1136
#define NEW_LEADER_PROMPT    1137
#define SET_RANK_PROMPT1     1138
#define SET_RANK_PROMPT2     1139
#define GANG_BANK_PROMPT     1140
#define GANG_DEPOSIT_PROMPT  1141
#define GANG_LEDGER_PROMPT   1142
#define GANG_WITHDRAW_PROMPT 1143
#define GANG_CODE_PROMPT     1144
#define GANG_SUMMARY_PROMPT  1145
#define SERVER_PROMPT        1146
#define GANG_INFO_PROMPT     1147
#define SERVER_CODE_PROMPT   1148
#define GANG_LOCK_PROMPT1    1149
#define GO_SERVER_PROMPT     1150
#define POST_GANG_PROMPT1    1151
#define POST_GANG_PROMPT2    1152
#define GANG_BODY_PROMPT     1153
#define GANG_MAIL_PROMPT1    1154
#define GANG_MAIL_PROMPT2    1155
#define GANG_MAIL_PROMPT3    1156
#define SYS_PW_PROMPT        1157
#define ERASE_DOC_PROMPT     1158
#define RETRIEVE_PROMPT      1159
#define CHIP_ERASE_PROMPT    1160
#define TOURNEY_PROMPT       1161
#define TSTATUS_PROMPT       1162
#define PHASE_PROMPT1        1163
#define PHASE_PROMPT2        1164
#define TOURNEY_EDIT_PROMPT1 1165
#define TOURNEY_EDIT_PROMPT2 1166
#define TOURNEY_EDIT_PROMPT3 1167
#define TOURNEY_EDIT_PROMPT4 1168
#define RESET_PROMPT         1169
#define WINNERS_PROMPT       1170
#define COMBAT_PROMPT        1171
#define GANG_COUNT1          1172
#define GANG_COUNT2          1173
#define SELL_PROMPT          1174
#define READ_DOC_PROMPT      1175

/* Color Constants */

#define CLEAR_SCREEN            "\x1B[2J"
#define CLEAR_RIGHT             "\x1B[K"

#define bblack                  "\x1B[40m"
#define bred                    "\x1B[41m"
#define bgreen                  "\x1B[42m"
#define byellow                 "\x1B[43m"
#define bblue                   "\x1B[44m"
#define bmagenta                "\x1B[45m"
#define bcyan                   "\x1B[46m"
#define bwhite                  "\x1B[47m"

#define black                   "\x1B[30m"
#define red                     "\x1B[31m"
#define green                   "\x1B[32m"
#define yellow                  "\x1B[33m"
#define blue                    "\x1B[34m"
#define magenta                 "\x1B[35m"
#define cyan                    "\x1B[36m"
#define white                   "\x1B[37m"

#define normal                  "\x1B[0m"
#define bold                    "\x1B[0m\x1B[1m"

#define CTRL_S                  19

/* Limits */

#define MAX_NET_X           18
#define MAX_NET_Y           9

#define MAX_CITY_X          32
#define MAX_CITY_Y          9

#define MAX_SYS_X           32
#define START_SYS_X         16
#define MAX_SYS_Y           9
#define START_SYS_Y         4

#define MAX_X               MAX_CITY_X
#define MAX_Y               MAX_CITY_Y

#define MAX_LD_LINKS        15
#define MAX_MOVE            4
#define NUM_CITIES          31
#define MAXGUYS             32
#define CONS                2
#define COPS                1
#define FLUFF_SYSTEMS       10
#define MAXSYSTEMS          77      /*((MAXGUYS * 2) + CONS + COPS + FLUFF)*/
#define MU_CAPACITY         8
#define MAX_BACKUP_CHIPS    8
#define NAME_SIZE           20
#define MAX_MUS             12
#define MAX_CPUS            8

#define LINE_LEN            140
#define LINE_SIZE           80
#define DATE_SIZE           6
#define TIME_SIZE           6

/* Access Levels */

#define IDLE                0
#define GLOBAL_VIEW         0
#define CITY_VIEW           1
#define SYSTEM_VIEW         2
#define EDIT_SYSTEM_VIEW    3
#define EDIT_CITY_VIEW      4

/* Node Types */

#define PHONE               1
#define BBS                 2
#define LEV1                3
#define LEV2                4
#define LEV3                5
#define CRIMINAL            6
#define VENDOR              7
#define LEV4                8
#define LEV5                9
#define HUB                 10
#define WORLDBANK           11
#define FILE_SERVER         12
#define WEB_SERVER          13
#define LEV6                14
#define DATA_WALL           15
#define OPENED_GATE         16
#define DEAD_WALL           17
#define CONST               18
#define A_PLAYER            19
#define ALIEN_SYS           20
#define ACTIVE_CPU          21
#define CRASHED_CPU         22
#define TACT_CONST          23
#define THE_PLAYER          24
#define MEMORY              25
#define CLOSED_GATE         26
#define CPU                 27
#define LEV7                28
#define CYBERIA_SYS         29
#define DELETED_NODE        30
#define GANGDECK            31
#define CYBERDECK           32
#define DEAD_MEMORY         33
#define END_SYS             34
#define ENDGAME_SYS         35
#define COP_SYS             36
#define CON_SYS             37

/* Link types */

#define INTERNET_LINK       1
#define STREET_LINK         2

/* Misc */

#define mp                  ((struct message *) vdatmp)

#define BLOB_FACTOR         3
#define STUCK_GUARD         10
#define FENCE_ZAP           3
#define VENDOR_LOCK         5
#define CASHIERS            3
#define CON_ODDS            120
#define STYMIE_TIME         (10 * 60)
#define CON_PROGRAMS        16
#define COP_CRASH           (15 * 60)
#define VALHALLA_CRASH      (15 * 60)
#define VALHALLA_REBUILD    (30 * 60)
#define PERM_CRASH          (long) glob->tmpdur * (4L * 60L)
#define QUICK_SPEED         6
#define GATECRASH_PW        4
#define HERCULES_BONUS      3
#define COP_STR             20
#define ERASE_MISSION       1
#define CRASH_MISSION       2
#define RETRIEVE_MISSION    3
#define MISSION_TIME        (60L * 15L)
#define MISSION_BUX         30
#define MISSION_SCORE       7

#define ALIEN_TIME1         (3600 * 2)
#define ALIEN_TIME2         (20 * 60)
#define ALIEN_TIME3         (5 * 60)

#define CYBERIA_TIME1       (60 * 30)
#define CYBERIA_TIME2       (15 * 60)
#define CYBERIA_TIME3       (5 * 60)

#define FREENET_MAX         50

#define RADAR_RATE          5
#define DEVIL_SUBS          2
#define FIEND_SUBS          3
#define SATAN_SUBS          4

#define NL                  "\r"
#define READB               "rb+"
#define APPENDB             "ab+"

#ifdef MBBS
#define READA               "r  "
#define APPENDA             "a  "
#else
#define READA               "r+"
#define APPENDA             "a+"
#endif

#define CANCEL              "X"
#define ANS_YES             "Y"
#define ANS_NO              "N"
#define OPEN_RECORD         "OPENRECORD"
#define FRONTIER            "Frontier"
#define VALHALLA            "Valhalla"
#define ALIEN_CITY          "Alien Nation"
#define CYBERIA             "Cyberia"
#define A_GANG              "GANG#"
#define CLEAR_ALL           1
#define CLEAR_SOME          2

#if     !defined(TRUE) || ((TRUE) != 1)
#define TRUE    1
#endif
#if     !defined(FALSE) || ((FALSE) != 0)
#define FALSE   0
#endif

#define STATE               usrptr->substt
#define LD_LINK             TRUE
#define WALKED_NET          FALSE

/* Communication Options */

#define TO_GAB              1
#define TO_IDLE             2

/* permission indexes */

#define SET_PERM            0
#define GAMEOP_PERM         1
#define MONITOR_PERM        2
#define FREE_PERM           3
#define CREATE_PERM         4
#define EDIT_USER_PERM      5
#define EDIT_WORLD_PERM     6
#define NUM_PERMS           8

/* option indexes */

#define IBM_OPT             0
#define BUSY_OPT            1
#define CHAT_OPT            2
#define AUTOJACK_OPT        3
#define NUM_OPTS            8

/* commands */

#define LIST_PROGRAMS   1
#define TIME            2
#define WHERE           3
#define DECK            4
#define PERMITS         5
#define EDIT            6
#define TOPTEN          7
#define CPUS            8
#define STOP            9
#define SUICIDE         10
#define DAYFILE         11
#define GO_OUTLAW       12
#define LOOK            13
#define SERVICES        14
#define MEMORY_SERVICES 15
#define OPTION_OPS      16
#define CHIPS           17
#define LIST_DOCS       18
#define DESIGN          19
#define JACK            20
#define BACK            21
#define FINANCES        22
#define CREATE          23
#define HELP            24
#define REDRAW_SCREEN   25
#define NORTH           26
#define SOUTH           27
#define EAST            28
#define WEST            29
#define RANK            30
#define MAIL            31
#define WHO             32
#define MISSION         33
#define GANG            34
#define TOURNEY         35
#define NUM_COMMANDS    36

/* Macros */

#define MY_SOURCE        MY_DECK->memory[MY_PROGRAM.m_index][MY_PROGRAM.p_index]
#define MY_PROGRAM       MY_PROC->cpu[i]
#define SYS_SOURCE       Sys[s]->memory[SYS_PROGRAM.m_index][SYS_PROGRAM.p_index]
#define SYS_PROGRAM      Proc[s]->cpu[i]
#define SEEK_SOURCE      Sys[sys_num]->memory[SEEK_PROGRAM.m_index][SEEK_PROGRAM.p_index]
#define SEEK_PROGRAM     Proc[sys_num]->cpu[cpu_num]
#define GUY_SOURCE       Sys[g]->memory[GUY_PROGRAM.m_index][GUY_PROGRAM.p_index]
#define GUY_PROGRAM      Proc[g]->cpu[p]
#define SYS_CONST_CITY   world->city[SYS_PROGRAM.net_x][SYS_PROGRAM.net_y]
#define MY_CONST_CITY    world->city[MY_PROGRAM.net_x][MY_PROGRAM.net_y].name
#define THEIR_SOURCE     Sys[s]->memory[runner->m_index][runner->p_index]
#define NOW(x) strncpy(x, nctime(now()), TIME_SIZE - 1); x[TIME_SIZE - 1]=0
#define TODAY(x) strncpy(x, ncdate(today()), DATE_SIZE - 1); x[DATE_SIZE - 1]=0
#define CONV_DATE(x, y) strncpy(x, ncdate(y), DATE_SIZE - 1); x[DATE_SIZE-1]=0
#define CONV_TIME(x, y) strncpy(x, nctime(y), TIME_SIZE - 1); x[TIME_SIZE-1]=0
#define INUM(x)             abs(atoi(x))
#define LNUM(x)             labs(atol(x))
#define EXIT0               return (0)
#define MY_PORT             Port[myp]
#define DECK_CITY           world->city[MY_DECK->net_x][MY_DECK->net_y]
#define DECK_MISC_CITY      misc_world->city[MY_DECK->net_x][MY_DECK->net_y]
#define MY_CITY             world->city[MY_PORT->net_x][MY_PORT->net_y]
#define MY_MISC_CITY        misc_world->city[MY_PORT->net_x][MY_PORT->net_y]
#define MY_NODE             MY_CITY.type[MY_PORT->city_x][MY_PORT->city_y]
#define HIS_CITY            world->city[Port[g]->net_x][Port[g]->net_y]
#define HIS_NODE            HIS_CITY.type[Sys[g]->city_x][Sys[g]->city_y]
#define SYS_CITY            world->city[Sys[g]->net_x][Sys[g]->net_y]
#define SYS_NODE            SYS_CITY.type[Sys[g]->city_x][Sys[g]->city_y]
#define DEAD_CITY           world->city[dead->sys[p].net_x][dead->sys[p].net_y]
#define DEAD_NODE           DEAD_CITY.type[dead->sys[p].city_x][dead->sys[p].city_y]
#define MY_SYS              Sys[MY_PORT->sys_num]
#define MY_DECK             Sys[myp]
#define MY_PROC             Proc[myp]
#define me                  MY_PORT->guy
#define him                 Port[g]->guy
#define HIS                 Port[g]->sys_num
#define ON_OFF(x) if (!me.perm[x]) prfmsg(MSG270, x); else prfmsg(MSG271, x);
#define OFF_ON(x) if (!me.option[x]) prfmsg(MSG270, x + 1); else prfmsg(MSG271, x + 1);

#define GANG_SIZE           (sizeof(struct gang_struct))
#define TOURNEY_SIZE        (sizeof(struct tourney_struct))
#define WINNERS_SIZE        (sizeof(struct winners_struct))
#define MEMORY_ELEMENT_SIZE (sizeof(struct memory_struct))
#define POST_SIZE           (sizeof(struct post_struct))
#define FREENET_SIZE        (sizeof(struct freenet_struct))
#define DAY_SIZE            (sizeof(struct day_struct))
#define CRASHED_SIZE        (sizeof(struct crashed_struct))
#define WORLD_SIZE          (sizeof(struct world_struct))
#define MISC_WORLD_SIZE     (sizeof(struct misc_world_struct))
#define GLOBALS_SIZE        (sizeof(struct globals_struct))
#define TRACKING_SIZE       (sizeof(struct tracking_struct))
#define SYSTEM_SIZE         (sizeof(struct system_struct))
#define PROCESSOR_SIZE      (sizeof(struct processor_struct))
#define CITY_SIZE           (sizeof(struct city_struct))
#define PORT_SIZE           (sizeof(struct port_struct))
#define HUMAN_SIZE          (sizeof(struct human_struct))
#define BRIEF_SIZE          (sizeof(struct brief_human_struct))

/* character file constants */

#define NO_MATCH    0
#define EXISTS      1

/* Program Types */

#define UTILITY         1
#define DEFENSE         2
#define AGENT           3
#define WEAPON          4
#define ANTISYSTEM      5
#define ANTIPROGRAM     6
#define ICE             7
#define HYDRA           8
#define ENDKEY          9

/* Programs */

#define ANYTHING        0
#define BULLDOZER       1
#define MOLE            2
#define UNLOCKER        3
#define GATECRASH       4
#define SENTRY          5
#define GUARDIAN        6
#define RADAR           7
#define SURGE           8
#define LIGHTNING       9
#define CRASH           10
#define PUNTER          11
#define BLOB            12
#define VIRUS           13
#define CLOAK           14
#define MUFFLER         15
#define SHIELD          16
#define DERVISH         17
#define ARMOR           18
#define STATIK          19
#define KNIGHT          20
#define HERCULES        21
#define SNOUT           22
#define ICEBOLT         23
#define FIREBOLT        24
#define JOLT            25
#define BRAINDRAIN      26
#define LOBOTOMY        27
#define BANSHEE         28
#define HANDCUFF        29
#define TORPEDO         30
#define ANTIVIRUS       31
#define LOCKER          32
#define ROBOCOP         33
#define DEVIL           34
#define FIEND           35
#define SATAN           36
#define CASHIER         37
#define PHASER          38
#define CHAINSAW        39
#define SKULLDRILL      40
#define TENTACLE        41
#define CATTLEPROD      42
#define GLUEGUN         43
#define COBRA           44
#define PROBE           45
#define BARRICADE       46
#define SOLDIER         47
#define GABLINK         48
#define MASON           49
#define FENCE           50
#define IMP             51
#define ROBOT           52
#define ROGUE           53
#define NETWORM         54
#define DRONE           55
#define CYBERDROID      56
#define TRON            57
#define WRAITH          58
#define GHOST           59
#define ROADWARRIOR     60
#define BLACKKNIGHT     61
#define NETRUNNER       62
#define FLATLINE        63
#define MEGADEATH       64
#define CANCERMAN       65
#define WEIRDSCIENCE    66
#define ADDLE           67
#define TRACE           68
#define MIMIC           69
#define KEY1            70
#define KEY2            71
#define SHREADER        72
#define KEY3            73
#define INFILTRATE      74
#define NETSPEED        75
#define PATRIOT         76
#define CAMOFLAGE       77
#define NUM_PROGRAMS    78

/* file types */

#define PROGRAM         1
#define DOCUMENT        3

#define USELESS_DOC     1
#define BUSINESS_DOC    2
#define SECRET_DOC      3
#define TOP_SECRET_DOC  4
#define INTERNET_DOC    5
#define BANK_DOC        6
#define PASSWORD_DOC    7
#define ENDGAME_DOC     8
#define GANG_DOC        9

/* Structure definitions */

#define GANG_NAME_SIZE  30
#define GANG_RANKS      6
#define RANK_NAME_SIZE  15

#define RENAME_PERM     0
#define WITHDRAW_PERM   1
#define SET_RANK_PERM   2
#define INVITE_PERM     3
#define KICK_PERM       4
#define CODE_PERM       5
#define MAX_GANG_PERMS  6

struct gang_struct {
  short   index;
  short   ufile_index;
  short   members;
  short   newbie;
  long    pts_today;
  long    pts;
  char    name[GANG_NAME_SIZE];
  char    rank[GANG_RANKS][RANK_NAME_SIZE];
  char    perm[GANG_RANKS][MAX_GANG_PERMS];
  char    locked[MAX_NET_X][MAX_NET_Y];
  short   unused[99];
  short   scorers;
};

#define MESSAGE_LEN     8

struct post_struct {
  char    from[LINE_LEN];
  char    body[MESSAGE_LEN][LINE_SIZE];
};

struct freenet_struct {
  long    total;
  long    msg_num;
  char    from[LINE_LEN];
  char    body[MESSAGE_LEN][LINE_LEN];
};

struct day_struct {
  short   port;
  char    dateon[DATE_SIZE];
  char    timeon[TIME_SIZE];
  long    secs;
  char    userid[UIDSIZ];
};

struct  program_struct {
  char    Name[NAME_SIZE];
  short   value;
  char    class;
  char    index;
  short   strength;
  char    size;
  char    load;
};

struct vended_struct {
  short   index;
  short   occurs;
  short   chance;
};

#define SYS_TYPE_NAME_SIZE  20
#define SYS_TYPES           18

struct sys_type_struct {
  short   type;
  char    name[SYS_TYPE_NAME_SIZE];
  short   cpus;
  short   mus;
  short   speed;
  short   defense;
  short   power;
};

#define MAX_SUBS    4

struct memory_struct {
  char    name[NAME_SIZE];
  char    type;       /* generic type - program, data, etc */
  char    class;
  short   index;

  short   value;
  short   strength;
  char    size;
  char    vended;
  short   load;

  short   lock;
  char    packed;
  char    running;
  char    sub[MAX_SUBS];
} __attribute__((packed));;

#define CODE_SIZE   11
#define MAX_LEDGER  17
#define ACTION_SIZE 16

struct ledger_struct {
  long    amount;
  long    balance;
  short   date;
  short   time;
  short   net_x;
  short   net_y;
  char    alias[NAME_SIZE];
  char    action[ACTION_SIZE];
};

struct internet_struct {
  short   credit_rating;
  long    owed;
  char    code[CODE_SIZE];

  long    time_paid_for;
  char    warn1;
  char    warn5;
  struct  ledger_struct ledger[MAX_LEDGER];
};

struct bank_struct {
  char    code[CODE_SIZE];
  long    balance;
  struct  ledger_struct ledger[MAX_LEDGER];
};

struct cpu_struct {
  short   m_index;
  short   p_index;
  short   loading;
  short   tentacle_timer;
  short   sys_x,  sys_y;
  short   net_x,  net_y;
  short   city_x, city_y;
  short   seek_x, seek_y;
  short   cop_x, cop_y;
  short   target_index;
  short   target_s;
  short   target_m;
  char    sees[MAXGUYS];
  char    enemy[MAXGUYS];
  short   sees_hydra_s;
  short   sees_hydra_c;
  short   rez;
};

#define SYSTEM_NAME_SIZE  18
#define PASSWORD_SIZE     5

struct  system_struct {
  char    Name[SYSTEM_NAME_SIZE];
  char    in_use;
  char    mus;
  char    cpus;
  char    speed;
  char    wall;
  char    gate;
  char    type;
  char    breached;
  char    unused3;
  char    unused4;
  short   net_x, net_y;
  short   city_x, city_y;
  int    rebuild_time;
  int    last_write;
  char    permanant;
  char    unused1[8];
  char    node_type[MAX_SYS_X][MAX_SYS_Y];
  char    node_index[MAX_SYS_X][MAX_SYS_Y];
  char    unused2[481];
  char    password[PASSWORD_SIZE];
  int    crash_time[MAX_CPUS + 1];
  struct  memory_struct memory[MAX_MUS][MU_CAPACITY];
} __attribute__((packed));

struct  processor_struct {
  short   file_server;
  short   infected;
  short   moled;
  short   torped;
  char    enemy[MAXGUYS];
  struct  cpu_struct cpu[MAX_CPUS + 1];
};

struct  human_struct {
  short   index;
  short   outlaw;
  char    alias[NAME_SIZE];
  char    userid[UIDSIZ];
  short   last_on;
  char    renting;
  char    employed;
  char    newbie;
  short   payday;
  short   rentdue;
  short   start_x;
  short   start_y;
  short   damage;
  long    last_net_crime;
  short   arrest_time;
  char    arrest_warn30;
  char    arrest_warn90;
  short   wanted;
  long    score;

  short   unused[53];

  short   phone;
  char    mission_doc[NAME_SIZE];
  short   jacks;
  long    gang_mail;
  long    gang_pts_today;
  long    gang_pts;
  long    uploads;
  long    created;
  long    last_name_change;
  long    gang_bank;
  short   invite;
  short   gang;
  short   rank;
  short   mission_score;
  short   mission_x, mission_y;
  short   mission_type;
  long    mission_time;
  char    mission_system[SYSTEM_NAME_SIZE];

  short   won;
  long    freenet;
  char    perm[NUM_PERMS];
  char    option[NUM_OPTS];

  struct  internet_struct      internet;
  struct  bank_struct          bank;

  char    interface;
  char    chip_reader;
  char    nothing;

  /* this is only available when not jacked */

  char    backup_chips;
  struct  memory_struct backup[MAX_BACKUP_CHIPS][MU_CAPACITY];
};

struct  brief_human_struct {
  short   index;
  short   outlaw;
  char    alias[NAME_SIZE];
  char    userid[UIDSIZ];
  short   last_on;
  long    score;
  short   won;
  short   gang;
  long    gang_pts;
  long    gang_pts_today;
  long    uploads;
  long    gang_bank;
  short   rank;
  char    internet_code[CODE_SIZE];
  char    bank_code[CODE_SIZE];
  char    perm[NUM_PERMS];
};

struct  walk_path_struct {
  short   x, y;
};

struct  ld_path_struct {
  short   net_x, net_y;
  short   city_x, city_y;
  short   sys_x, sys_y;
};

struct program_list_struct {
  short   m_index;
  short   p_index;
};

#define MAX_PROGLIST    9
#define COMMAND_SIZE    10
#define FSE_SIZE        350

struct  port_struct {
  short   pinged;
  short   autojack;
  short   old_credit_rate;
  short   hup_timer;
  short   enemy[MAXGUYS];
  char    tstring[LINE_LEN];
  char    unique[160];
  char    mimic[NAME_SIZE];
  char    unlocking;
  short   time_on;
  short   gang;
  short   move_dir;
  short   hung_up;
  long    gang_pts;
  FILE    *fp;
  long    secs_on;
  short   memory_bonk;
  short   misc;
  short   rez;
  short   icebolt_time;
  short   handcuff_time;
  short   m_index, m_index1;
  short   p_index, p_index1;
  short   nx, ny;
  short   cx, cy;
  short   p_num;
  short   target_s;
  short   target_c;
  short   target_type;
  short   target_x;
  short   target_y;
  char    active;
  char    editing;
  char    cycling;
  char    reporting;
  char    invisible;
  char    msg_flag;
  char    last_icon[15];
  char    refreshed;
  char    inside;
  char    downloading;
  char    redraw;
  short   last_x, last_y;
  short   usrnum;
  short   lines;
  long    forum1, forum2;
  short   post_index;
  short   top[MAX_Y][MAX_X];
  char    last_view;
  short   source_s;
  short   source_m;
  short   dest_s;
  short   dest_m;
  short   spaces;
  short   recnum;
  short   last_cmd;
  char    prompt_x, prompt_y;
  short   prompt_len;
  char    monitoring;
  char    access_level;
  char    link_type;
  short   ld_moves;
  short   walks[MAX_LD_LINKS];
  short   sys_num;
  short   pwx, pwy;
  short   gx, gy;
  short   wx, wy;
  short   px, py;
  short   net_x, net_y;
  short   city_x, city_y;
  short   sys_x, sys_y;
  short   back_x, back_y;
  long    ld_time;

  short   old_net_x, old_net_y;
  short   old_city_x, old_city_y;
  short   old_walks;
  short   old_ld_moves;
  short   old_damage;
  long    old_score;
  long    old_gang_score;
  short   old_rez;
  short   old_online;

  char    prompt[LINE_LEN];
  char    code[CODE_SIZE];
  long    balance;
  char    last_input[LINE_LEN];
  char    fse[FSE_SIZE];
  char    fsetmp[FSE_SIZE];

  struct  ld_path_struct      ld_path[MAX_LD_LINKS];
  struct  walk_path_struct    walked[MAX_LD_LINKS][MAX_MOVE];
  struct  human_struct        guy;
  short   progs;
  struct  program_list_struct proglist[MAX_PROGLIST];
};

#define CITY_NAME_SIZE  15

struct city_struct {
  char    name[CITY_NAME_SIZE];
  char    security;
  char    landmass;
  char    type[MAX_CITY_X][MAX_CITY_Y];
  short   invisible;
  short   unused;
} __attribute__((packed));

struct misc_city_struct {
  char    camoflaged[MAX_CITY_X][MAX_CITY_Y];
  long    last_net_crime;
} __attribute__((packed));;

#define MAX_CRASHED 200

struct crashed_system_struct {
  short   net_x, net_y;
  short   city_x, city_y;
  long    timer;
  short   type;
} __attribute__((packed));;

struct crashed_struct {
  struct  crashed_system_struct sys[MAX_CRASHED];
};

struct  world_struct {
  struct  city_struct      city[MAX_NET_X][MAX_NET_Y];
} __attribute__((packed));

struct  misc_world_struct {
  struct  misc_city_struct city[MAX_NET_X][MAX_NET_Y];
} __attribute__((packed));;

#define AFRICA      1
#define NAMERICA    2
#define SAMERICA    3
#define EUROPE      4
#define ASIA        5
#define LANDMASSES  5

struct  city_def_struct { /* the initial cities */
  char    name[CITY_NAME_SIZE];
  char    x, y;
  char    security;
  char    landmass;
} __attribute__((packed));;

#define TITLE_SIZE  20
#define NUM_RANKS   31

struct rank_struct {
  char    title[TITLE_SIZE];
  long    score;
  short   salary;
};

#define DEMO_DAYS 14

struct demo_record_struct {
  char    userid[UIDSIZ];
  char    modnam[MNMSIZ];
  short   install_date;
};

struct edit_struct {
  char    system_name[SYSTEM_NAME_SIZE];
  char    landmass[20];
  short   node_type;
  short   permanant;
  short   custom;
  short   x, y;
  short   m, p, files;
  short   x1, y1;
  short   x2, y2;
  short   net_x, net_y;
  short   option;
  struct  city_struct     city;
  struct  city_struct     custom_city;
  struct  system_struct   sys;
};

struct  used_node_struct {
  short   x, y;
};

struct  active_node_struct {
  short   count;
  short   dist;
  short   from_x, from_y;
  short   to_x, to_y;
};

#define MAX_ACTIVE_NODES 100

struct  tracking_struct {
  struct used_node_struct    used_node[MAX_X][MAX_Y];
  struct active_node_struct  active_node[MAX_ACTIVE_NODES];
};

#define MAX_WINNERS 9

#define PREGAME     1
#define GAME        2
#define POSTGAME    3

#define ENDGAME_TOURNEY 1
#define SCORE_TOURNEY   2
#define TIMED_TOURNEY   3

struct tourney_struct {
  short   type;
  short   phase;
  short   winners;
  short   days;
  short   postgame_hours;
  short   results_tallied;
  long    score;
  long    phase_start;
  long    phase_end;
  long    credits[MAX_WINNERS];
  char    prizes[MAX_WINNERS][80];
};

struct winners_struct {
  char    date[15];
  char    alias[MAX_WINNERS][NAME_SIZE];
  char    userid[MAX_WINNERS][UIDSIZ];
  long    score[MAX_WINNERS];
  short   rank[MAX_WINNERS];
  char    prize[MAX_WINNERS][80];
};

#define MAX_RANDOM_USERS    500
#define MAX_VENDS           55

struct globals_struct {
  char    txt[LINE_LEN];
  short   autojack;
  long    egamscor;
  char    topguy[80];
  char    winner[UIDSIZ];
  short   pre_blurb;
  short   game_blurb;
  short   post_blurb;
  short   reset;
  short   shutdown;
  short   jacks;
  short   winners;
  short   derez1;
  short   gangbux[5];
  long    suihours;
  long    namechng;
  long    rankfctr;
  long    last_tick;
  short   netdebt;
  short   cop_sys;
  short   used[NUM_PROGRAMS];
  short   cop_hq_x, cop_hq_y;
  short   valhalla_x, valhalla_y;
  short   alien_timer;
  short   alien_x, alien_y;
  short   cyberia_timer;
  short   cyberia_x, cyberia_y;
  short   end_x, end_y;
  short   maxuser;

#ifndef MBBS
  HMCVFILE cfg;
#else
  FILE    *cfg;
#endif

  short   module_id;
  short   world_edit;
  short   outlaw;
  short   inactdys;
  short   soldmg;
  short   pvprank;
  short   xferrank;
  short   joltdmg;
  short   firedmg;
  short   bandmg;
  short   zapodds;
  short   copmins1;
  short   copmins2;
  short   statsecs;
  short   gluesecs;
  short   boltsecs;
  short   tentsecs;
  short   cuffsecs;
  short   addlesec;
  short   chaindmg;
  short   drilldmg;
  short   phsrpowr;
  short   puntsecs;
  short   prodsecs;
  short   crshsecs;
  short   topval;
  short   secval;
  short   busval;
  short   virsecs;
  short   fate;
  short   dpts;
  short   jackroll;
  short   jacksecs;
  short   hospcost;
  short   netcost;
  short   ldlcost;
  short   ldcost;
  short   stspeed;
  short   stcpus;
  short   stwall;
  short   stgate;
  short   stmem;
  short   stchips;
  short   streader;
  short   memcost;
  short   cpucost;
  short   wallcost;
  short   gatecost;
  short   facecost;
  short   crcost;
  short   ldpcost;
  short   chipcost;
  short   minair;
  short   airadd;
  short   rentdays;
  short   minrent;
  short   rentadd;
  short   paydays;
  char    *againkey;
  char    *chatkey;
  char    *privkey;
  char    blob[MAXSYSTEMS];
  short   random_user[MAX_RANDOM_USERS];
  char    path[LINE_SIZE];
  char    name[LINE_SIZE];
  char    name2[LINE_SIZE];
  short   ibm[256];
  short   quick_move[256];
  short   order[MAXSYSTEMS];
  char    buf[300];
  char    dirs[LINE_SIZE];
  char    server;
  char    *validus;
  char    *sysopkey;
  char    *lockey;
  char    *playkey;

  char    *main_prompt;
  char    *gab_prompt;
  char    *empty;
  char    *aempty;
  char    *gempty;
  char    *agempty;
  char    *theguy;
  char    *badguy;
  char    *gtheguy;
  char    *syscpui;
  char    *syscpuc;
  char    *datawall;
  char    *deadwall;
  char    *sysmem;
  char    *sysmemx;
  char    *lev4sys;
  char    *lev5sys;
  char    *lev6sys;
  char    *lev7sys;
  char    *decksys;
  char    *websys;
  char    *bbssys;
  char    *lev1sys;
  char    *lev2sys;
  char    *lev3sys;
  char    *vendsys;
  char    *aliensys;
  char    *ldlink;
  char    *opengate;
  char    *clsdgate;
  char    *hubsys;
  char    *banksys;
  char    *crimesys;
  char    *copsys;
  char    *endsys;
  char    *gangsys;

  char    *atheguy;
  char    *abadguy;
  char    *agtheguy;
  char    *asyscpui;
  char    *asyscpuc;
  char    *adatawal;
  char    *adeadwal;
  char    *asysmem;
  char    *asysmemx;
  char    *alev4sys;
  char    *alev5sys;
  char    *alev6sys;
  char    *alev7sys;
  char    *adecksys;
  char    *abbssys;
  char    *alev1sys;
  char    *alev2sys;
  char    *alev3sys;
  char    *avendsys;
  char    *aufosys;
  char    *aldlink;
  char    *aopngate;
  char    *aclsgate;
  char    *ahubsys;
  char    *abanksys;
  char    *acrimsys;
  char    *awebsys;
  char    *agngsys;
  char    *acopsys;
  char    *aendsys;

  char    *m22;
  char    *m30;
  char    *m35;
  char    *m53;
  char    *m54;
  char    *m91;
  char    *m92;
  char    *m130;
  char    *m570;
  char    *m571;
  char    *m572;
  char    *m573;
  char    *m575;
  char    *m584;
  char    *m585;
  char    *m70a;
  char    *m70b;
  char    *m70c;
  char    *m71;
  char    *ms71;
  char    *m72;
  char    *m73;
  char    *ms73;
  char    *m74;
  char    *m75;
  char    *m76;
  char    *m538;
  char    *m540;
  char    *m513;
  char    *m514;
  char    *m515;
  char    *m516A;
  char    *m516B;
  char    *m518;
  char    *m519;

  short   cybccr;
  short   tmpdur;
  struct  vended_struct      vends[MAX_VENDS];
  struct  freenet_struct     header;
  struct  freenet_struct     dummy;
  struct  brief_human_struct brief;
  struct  winners_struct     fame;
};

#include <proto.h>
