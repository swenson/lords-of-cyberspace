/*
	int     type;
	char    name[SYS_TYPE_NAME_SIZE];
	int     cpus;
	int     mus;
	int     speed;
	int     defense;
	int     power;
*/

struct sys_type_struct System[] = {
  /*  Type Index      Name          CPUS      MUS    SPEED    WALL    PWR  */
  {BBS,           "BBS",          1,       3,      1,      1,      2},
  {LEV1,          "Level 1",      2,       4,      2,      2,      3},
  {LEV2,          "Level 2",      2,       5,      3,      3,      4},
  {LEV3,          "Level 3",      3,       6,      4,      4,      5},
  {CRIMINAL,      "Criminal",     3,       7,      6,      5,      6},
  {VENDOR,        "Vendor",       4,      12,      8,      6,      7},
  {LEV4,          "Level 4",      4,       8,     10,      7,      8},
  {LEV5,          "Level 5",      5,       9,     12,      8,      9},
  {HUB,           "Net Hub",      5,      10,     14,      9,     10},
  {WORLDBANK,     "Worldbank",    5,      11,     16,     10,     11},
  {FILE_SERVER,   "File Server",  6,      12,     18,     11,     12},
  {WEB_SERVER,    "Web Server",   6,      12,     20,     12,     13},
  {LEV6,          "Level 6",      6,      12,     22,     13,     14},
  {LEV7,          "Level 7",      6,      12,     24,     14,     15},
  {COP_SYS,       "The EYE",      8,      12,     26,     15,     16},
  {CYBERIA_SYS,   CYBERIA,        8,      12,     28,     16,     17},
  {ALIEN_SYS,     ALIEN_CITY,     8,      12,     30,     20,     20},
  {END_SYS,       VALHALLA,       8,      12,     35,     25,     25}
};
