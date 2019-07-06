/*
	char    name[PROGRAM_NAME_SIZE];
	int     value;
	int     class;
	int     type;
	int     strength;
	int     memory_used;
	int     load;
*/

struct  program_struct Programs[] = {
  {"Nothing",           0,             0,               0, 0, 0, 0},

  {"Gablink",           5,       UTILITY,         GABLINK, 0, 2, 0},
  {"Probe",            10,       UTILITY,           PROBE, 0, 1, 0},
  {"Locker",           15,       UTILITY,          LOCKER, 0, 1, 5},
  {"Unlocker",         15,       UTILITY,        UNLOCKER, 0, 1, 10},
  {"Bulldozer",        20,       UTILITY,       BULLDOZER, 0, 3, 15},
  {"Mole",             20,       UTILITY,            MOLE, 0, 4, 40},
  {"GateCrash",        25,       UTILITY,       GATECRASH, 0, 1, 10},
  {"Muffler",          25,       UTILITY,         MUFFLER, 0, 2, 0},
  {"Camoflage",        25,       UTILITY,       CAMOFLAGE, 0, 1, 0},
  {"Cloak",            30,       UTILITY,           CLOAK, 0, 2, 0},
  {"Radar",            30,       UTILITY,           RADAR, 0, 2, 0},
  {"Trace",            35,       UTILITY,           TRACE, 0, 2, 10},
  {"NetSpeed",         50,       UTILITY,        NETSPEED, 2, 2, 0},

  {"Barricade",        40,       DEFENSE,       BARRICADE, 0, 1, 0},
  {"Mason",            40,       DEFENSE,           MASON, 0, 3, 10},
  {"Static",           45,       DEFENSE,          STATIK, 0, 1, 0},
  {"Shield",           50,       DEFENSE,          SHIELD, 0, 1, 0},
  {"Armor",            55,       DEFENSE,           ARMOR, 0, 1, 0},

  {"Cashier",          50,         AGENT,         CASHIER, 10, 1, 0},
  {"Fence",            50,         AGENT,           FENCE, 6, 1, 0},

  {"Phaser",           50,        WEAPON,          PHASER, 0, 1, 3},
  {"Tentacle",         50,        WEAPON,        TENTACLE, 0, 1, 3},
  {"GlueGun",          55,        WEAPON,         GLUEGUN, 0, 1, 3},
  {"CattleProd",       55,        WEAPON,      CATTLEPROD, 0, 1, 5},
  {"Cobra",            60,        WEAPON,           COBRA, 0, 1, 5},
  {"SkullDrill",       60,        WEAPON,      SKULLDRILL, 0, 1, 3},
  {"Chainsaw",         65,        WEAPON,        CHAINSAW, 0, 1, 3},

  {"Crash",            50,    ANTISYSTEM,           CRASH, 0, 2, 20},
  {"Blob",             50,    ANTISYSTEM,            BLOB, 0, 2,  5},
  {"Virus",            50,    ANTISYSTEM,           VIRUS, 0, 3, 20},
  {"Torpedo",          75,    ANTISYSTEM,         TORPEDO, 0, 4, 30},

  {"Hercules",         50,   ANTIPROGRAM,        HERCULES, 0, 3, 10},
  {"AntiVirus",        50,   ANTIPROGRAM,       ANTIVIRUS, 0, 3, 10},
  {"Snout",            50,   ANTIPROGRAM,           SNOUT, 0, 3, 10},
  {"Patriot",          50,   ANTIPROGRAM,         PATRIOT, 0, 3, 10},
  {"Addle",            75,   ANTIPROGRAM,           ADDLE, 0, 2, 10},
  {"Knight",          100,   ANTIPROGRAM,          KNIGHT, 0, 3, 10},

  {"Sentry",           50,           ICE,          SENTRY, 0, 3, 10},
  {"Mimic",            50,           ICE,           MIMIC, 0, 2, 10},
  {"Guardian",         55,           ICE,        GUARDIAN, 0, 4, 10},
  {"Punter",           60,           ICE,          PUNTER, 0, 2, 10},
  {"Handcuff",         65,           ICE,        HANDCUFF, 0, 2, 10},
  {"Icebolt",          70,           ICE,         ICEBOLT, 0, 2, 10},
  {"Surge",            75,           ICE,           SURGE, 0, 2, 20},
  {"Braindrain",       80,           ICE,      BRAINDRAIN, 0, 3, 20},
  {"Lobotomy",         85,           ICE,        LOBOTOMY, 0, 3, 20},
  {"Jolt",             90,           ICE,            JOLT, 0, 4, 10},
  {"Firebolt",        100,           ICE,        FIREBOLT, 0, 5, 20},
  {"Infiltrate",      100,           ICE,      INFILTRATE, 0, 3, 10},
  {"Lightning",       100,           ICE,       LIGHTNING, 0, 4, 20},
  {"Soldier",         110,           ICE,         SOLDIER, 0, 5, 5},
  {"Banshee",         125,           ICE,         BANSHEE, 0, 6, 20},

  {"Devil",           150,         HYDRA,           DEVIL, 0, 6, 20},
  {"Fiend",           175,         HYDRA,           FIEND, 0, 5, 20},
  {"Satan",           200,         HYDRA,           SATAN, 0, 4, 20},

  {"Imp",              50,         HYDRA,             IMP, 1, 1, 1},
  {"Robot",            50,         HYDRA,           ROBOT, 2, 1, 1},
  {"Rogue",            50,         HYDRA,           ROGUE, 3, 1, 1},
  {"NetWorm",          50,         HYDRA,         NETWORM, 4, 1, 1},
  {"Drone",            50,         HYDRA,           DRONE, 5, 1, 1},
  {"CyberDroid",       50,         HYDRA,      CYBERDROID, 6, 1, 1},
  {"Tron",             50,         HYDRA,            TRON, 7, 1, 1},
  {"Wraith",           50,         HYDRA,          WRAITH, 8, 1, 1},
  {"Ghost",            50,         HYDRA,           GHOST, 9, 1, 1},
  {"RoadWarrior",      50,         HYDRA,     ROADWARRIOR, 10, 1, 1},
  {"BlackKnight",      50,         HYDRA,     BLACKKNIGHT, 11, 1, 1},
  {"NetRunner",        50,         HYDRA,       NETRUNNER, 12, 1, 1},
  {"Flatline",         50,         HYDRA,        FLATLINE, 13, 1, 1},
  {"MegaDeath",        50,         HYDRA,       MEGADEATH, 14, 1, 1},
  {"CancerMan",        50,         HYDRA,       CANCERMAN, 15, 1, 1},
  {"WeirdScience",     50,         HYDRA,    WEIRDSCIENCE, 16, 1, 1},
  {"Dervish",          50,         HYDRA,         DERVISH, 20, 1, 1},
  {"Shreader",         50,         HYDRA,        SHREADER, 25, 1, 1},

  {"RoboCop",           1,         HYDRA,         ROBOCOP, 10, 1, 1},

  {"BlackKey",         50,        ENDKEY,            KEY1, 0, 1, 0},
  {"GreyKey",          50,        ENDKEY,            KEY2, 0, 1, 0},
  {"WhiteKey",         50,        ENDKEY,            KEY3, 0, 1, 0}
};

