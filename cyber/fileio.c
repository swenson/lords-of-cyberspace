#include "header.h"
#include <globals.h>
FILE *fp;
#define GUMBY1 CITY_NAME_SIZE + 1 + 1 + (MAX_CITY_X * MAX_CITY_Y)

struct flddef cityFDA[] = {
  {
    CVTFLD_CHAR,
    GUMBY1,
    fldoff(city_struct, name),
    NULL
  },
  {
    CVTFLD_SHORT,
    2,
    fldoff(city_struct, invisible),
    NULL
  },
  {
    CVTFLD_END,
    0,
    0,
    NULL
  }
};

struct flddef memoryFDA[] = {
  {
    CVTFLD_CHAR,
    NAME_SIZE + 2,
    fldoff(memory_struct, name),
    NULL
  },
  {
    CVTFLD_SHORT,
    3,
    fldoff(memory_struct, index),
    NULL
  },
  {
    CVTFLD_CHAR,
    2,
    fldoff(memory_struct, size),
    NULL
  },
  {
    CVTFLD_SHORT,
    2,
    fldoff(memory_struct, load),
    NULL
  },
  {
    CVTFLD_CHAR,
    2 + MAX_SUBS,
    fldoff(memory_struct, packed),
    NULL
  },
  {
    CVTFLD_END,
    0,
    0,
    NULL
  }
};

#define GUMBY2 9+(2*MAX_SYS_X*MAX_SYS_Y)+481+PASSWORD_SIZE

struct flddef systemFDA[] = {
  {
    CVTFLD_CHAR,
    SYSTEM_NAME_SIZE + 10,
    fldoff(system_struct, Name),
    NULL
  },
  {
    CVTFLD_SHORT,
    4,
    fldoff(system_struct, net_x),
    NULL
  },
  {
    CVTFLD_LONG,
    2,
    fldoff(system_struct, rebuild_time),
    NULL
  },
  {
    CVTFLD_CHAR,
    GUMBY2,
    fldoff(system_struct, permanant),
    NULL
  },
  {
    CVTFLD_LONG,
    MAX_CPUS + 1,
    fldoff(system_struct, crash_time),
    NULL
  },
  {
    CVTFLD_STRUCT,
    MAX_MUS * MU_CAPACITY,
    fldoff(system_struct, memory),
    memoryFDA
  },
  {
    CVTFLD_END,
    0,
    0,
    NULL
  }
};

/*****************************************************************************
	display list of all users
*/
short EXPORT Who(void) {
  short   n;
  short   lines = 0;
  FILE    *fp;
  char    last_on[DATE_SIZE];

  if (STATE == MAIN_PROMPT) {
    MY_PORT->misc = 0;
  } else {
    if (margc && sameas(margv[0], ANS_NO)) {
      STATE = MAIN_PROMPT;
      EXIT0;
    }
  }

  fp = fopen(BRIEF_FILE, READB);
  fseek(fp, (long) MY_PORT->misc * BRIEF_SIZE, SEEK_SET);
  Clear_Display_Window();

  do {
    n = fread(&glob->brief, BRIEF_SIZE, 1, fp);
    ++MY_PORT->misc;

    if
    (
      n == 1 &&
      !sameas(glob->brief.userid, OPEN_RECORD) &&
      !sameto(A_GANG, glob->brief.userid)
    ) {
      CONV_DATE(last_on, glob->brief.last_on);
      sprintf
      (
        str,
        getmsg(M719),
        glob->brief.alias,
        Brief_Userid(glob->brief.userid),
        rank[His_Rank(glob->brief.score, glob->brief.won)].title,
        His_Rank(glob->brief.score, glob->brief.won),
        last_on
      );
      prf(str);
      ++lines;
    }
  } while (n == 1 && lines < 9);

  fclose(fp);

  if (n == 1) {
    Prompt(getmsg(MSG243), WHO_PROMPT);
  } else {
    STATE = MAIN_PROMPT;
  }

  EXIT0;
}
/****************************************************************************
	re-create the brief user file
*/
void EXPORT Create_Brief(void) {
  short   n;
  FILE    *fp;
  FILE    *fp_brief;
  unlink(BRIEF_FILE);
  Create_New(BRIEF_FILE);
  fp = fopen(USER_FILE, READB);
  fp_brief = fopen(BRIEF_FILE, APPENDB);

  do {
    n = fread(a_human, HUMAN_SIZE, 1, fp);

    if (n == 1) {
      Set_Brief(a_human, &glob->brief);
      fwrite(&glob->brief, BRIEF_SIZE, 1, fp_brief);
    }
  } while (n == 1);

  fclose(fp);
  fclose(fp_brief);
}
/****************************************************************************
	add a system to the perm system report
*/
short Report_Add_System(short net_x, short net_y, short x, short y) {
  short   i;
  short   m;
  short   s;
  short   done;
  short   max_str;
  short   min_str;
  FILE    *fp;
  fp = fopen(REPORT_FILE, APPENDA);

  if (fp == NULL) {
    NOTIFY("LoC: REPORT FILE FAILURE");
    EXIT0;
  }

  fprintf
  (
    fp,
    "City  : %s (%d/%d @ %d/%d)\n",
    world->city[net_x][net_y].name,
    net_x, net_y,
    x, y
  );

  if (a_system->rebuild_time) {
    fprintf(fp, "Regen : %ld hours\n", a_system->rebuild_time / 3600L);
  } else {
    fprintf(fp, "Regen : Never\n");
  }

  for (i = done = 0; i != SYS_TYPES && !done; ++i) {
    if (System[i].type == a_system->type) {
      fprintf(fp, "Type  : %s\n", System[i].name);
      done = TRUE;
    }
  }

  if (!done) {
    fprintf(fp, "Type  : Unknown\n");
  }

  fprintf(fp, "CPUs  : %3d", a_system->cpus);

  for (i = done = 0; i != SYS_TYPES && !done; ++i) {
    if (System[i].cpus >= a_system->cpus) {
      fprintf(fp, " (Equivalent: %2d %s)\n", i, System[i].name);
      done = TRUE;
    }
  }

  if (!done) {
    fprintf(fp, " (Equivalent: Off the Scale)\n");
  }

  fprintf(fp, "Speed : %3d", a_system->speed);

  for (i = done = 0; i != SYS_TYPES && !done; ++i) {
    if (System[i].speed >= a_system->speed) {
      fprintf(fp, " (Equivalent: %2d %s)\n", i, System[i].name);
      done = TRUE;
    }
  }

  if (!done) {
    fprintf(fp, " (Equivalent: Off the Scale)\n");
  }

  fprintf(fp, "Wall  : %3d", a_system->wall);

  for (i = done = 0; i != SYS_TYPES && !done; ++i) {
    if (System[i].defense >= a_system->wall) {
      fprintf(fp, " (Equivalent: %2d %s)\n", i, System[i].name);
      done = TRUE;
    }
  }

  if (!done) {
    fprintf(fp, " (Equivalent: Off the Scale)\n");
  }

  fprintf(fp, "Gate  : %3d", a_system->gate);

  for (i = done = 0; i != SYS_TYPES && !done; ++i) {
    if (System[i].defense >= a_system->gate) {
      fprintf(fp, " (Equivalent: %2d %s)\n", i, System[i].name);
      done = TRUE;
    }
  }

  if (!done) {
    fprintf(fp, " (Equivalent: Off the Scale)\n");
  }

  fprintf(fp, "Memory: %3d", a_system->mus);

  for (i = done = 0; i != SYS_TYPES && !done; ++i) {
    if (System[i].mus >= a_system->mus && System[i].type != VENDOR) {
      fprintf(fp, " (Equivalent: %2d %s)\n", i, System[i].name);
      done = TRUE;
    }
  }

  if (!done) {
    fprintf(fp, " (Equivalent: Off the Scale)\n");
  }

  for (m = 0; m != a_system->mus; ++m) {
    for (i = 0; i != MU_CAPACITY; ++i) {
      if (a_system->memory[m][i].type) {
        fprintf
        (
          fp,
          "            %2d/%d %3d %5d %2d %-10s",
          m + 1,
          i + 1,
          a_system->memory[m][i].lock,
          a_system->memory[m][i].value,
          a_system->memory[m][i].strength,
          a_system->memory[m][i].name
        );

        if (a_system->memory[m][i].type != PROGRAM) {
          fprintf(fp, "\n");
        } else {
          for (s = done = 0; s != SYS_TYPES && !done; ++s) {
            if (System[s].type == LEV1) {
              min_str = 1;
              max_str = 3;
            } else if (System[s].type == LEV2) {
              min_str = 1;
              max_str = 4;
            } else {
              min_str = (System[s].power / 2) + 1;
              max_str = (System[s].power / 2) * 3;
            }

            if
            (
              a_system->memory[m][i].strength >= min_str &&
              a_system->memory[m][i].strength <= max_str
            ) {
              fprintf
              (
                fp,
                " (Equivalent: %2d %s)\n",
                s,
                System[s].name
              );
              done = TRUE;
            }
          }

          if (!done) {
            fprintf(fp, " (Equivalent: Off the Scale)\n");
          }
        }

        if
        (
          a_system->memory[m][i].type == PROGRAM &&
          a_system->memory[m][i].class == HYDRA
        ) {
          fprintf(fp, "                              ");

          for (s = 0; s != MAX_SUBS; ++s) {
            if (a_system->memory[m][i].sub[s]) {
              fprintf
              (
                fp,
                "%s ",
                Programs[a_system->memory[m][i].sub[s]].Name
              );
            }
          }

          fprintf(fp, "\n");
        }
      }
    }
  }

  for (y = 0; y != MAX_SYS_Y; ++y) {
    for (x = 0; x != MAX_SYS_X; ++x) {
      if (a_system->node_type[x][y] == 0) {
        fprintf(fp, ".");
      } else if (a_system->node_type[x][y] == MEMORY) {
        fprintf(fp, "M");
      } else if (a_system->node_type[x][y] == CPU) {
        fprintf(fp, "C");
      } else if (a_system->node_type[x][y] == DATA_WALL) {
        fprintf(fp, "#");
      } else if (a_system->node_type[x][y] == CLOSED_GATE) {
        fprintf(fp, "+");
      } else if (a_system->node_type[x][y] == PHONE) {
        fprintf(fp, "P");
      }
    }

    fprintf(fp, "\n");
  }

  fprintf(fp, "\n");
  fclose(fp);
  EXIT0;
}
/****************************************************************************
	write a permanant system report
*/
short Perm_Report(void) {
  short   net_x;
  short   net_y;
  short   x;
  short   y;
  short   done = FALSE;
  short   did_one = FALSE;
  FILE    *fp;
  net_x = MY_PORT->nx;
  net_y = MY_PORT->ny;
  x = MY_PORT->cx;
  y = MY_PORT->cy;

  if
  (
    world->city[net_x][net_y].type[x][y] &&
    world->city[net_x][net_y].type[x][y] != PHONE &&
    world->city[net_x][net_y].type[x][y] != CYBERDECK &&
    world->city[net_x][net_y].type[x][y] != DELETED_NODE
  ) {
    did_one = TRUE;
    Set_System_Path(x, y, net_x, net_y);
    fp = fopen(glob->path, READB);

    if (fp != NULL) {
      fread(a_system, SYSTEM_SIZE, 1, fp);
      fclose(fp);

      if (a_system->permanant) {
        sprintf
        (
          str,
          getmsg(MSG907),
          a_system->Name,
          net_x, net_y,
          x, y
        );
        Send(str, myp, FALSE);
        Set_Perm_Path(x, y, net_x, net_y);
        fp = fopen(glob->path, READB);
        fread(a_system, SYSTEM_SIZE, 1, fp);
        fclose(fp);
        Report_Add_System(net_x, net_y, x, y);
      }
    }
  }

  ++MY_PORT->cy;

  if (MY_PORT->cy == MAX_CITY_Y) {
    MY_PORT->cy = 0;
    ++MY_PORT->cx;

    if (MY_PORT->cx == MAX_CITY_X) {
      MY_PORT->cx = 0;
      ++MY_PORT->ny;

      if (MY_PORT->ny == MAX_NET_Y) {
        MY_PORT->ny = 0;
        ++MY_PORT->nx;

        if (MY_PORT->nx == MAX_NET_X) {
          done = TRUE;
        }
      }
    }
  }

  if (!done) {
    if (!did_one) {
      return (FALSE);
    }

    btuinj(usrnum, CYCLE);
    return (TRUE);
  }

  Clear_Display_Window();
  prfmsg(MSG906, REPORT_FILE);
  MY_PORT->reporting = FALSE;
  STATE = MAIN_PROMPT;
  prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
  prf("%s%s", MY_PORT->prompt, CLEAR_RIGHT);
  btumil(usrnum, 80 - MY_PORT->prompt_len - MY_PORT->prompt_x);
  My_Outprf(usrnum);
  return (TRUE);
}
/****************************************************************************
	init the non-static Cyber data files
*/
void Init_Data_Files(void) {
  Create_New(FREENET_FILE);
  Create_New(USER_FILE);
  Create_New(DECK_FILE);
  Create_New(DAY_FILE);
  Create_New(GANG_FILE);
  Create_New(CUSTOM_FILE);
  Create_New(TOURNEY_FILE);
  Create_New(WINNER_FILE);
  Create_Brief();
}
/****************************************************************************
	remove inactive characters
*/
void Clear_Old_Characters(void) {
  short   cur_days = cofdat(today());
  short   index = -1;
  short   n;
  short   done = FALSE;
  FILE    *fp;
  FILE    *fp_brief;
  fp = fopen(USER_FILE, READB);
  fp_brief = fopen(BRIEF_FILE, READB);

  do {
    ++index;
    fseek(fp_brief, (long) index * BRIEF_SIZE, SEEK_SET);
    n = fread(&glob->brief, BRIEF_SIZE, 1, fp_brief);

    if (n == 0) {
      done = TRUE;
    } else if
    (
      !sameas(glob->brief.userid, OPEN_RECORD) &&
      !sameto(A_GANG, glob->brief.userid)
    ) {
      if (cur_days - cofdat(glob->brief.last_on) >= glob->inactdys) {
        strcpy(glob->brief.userid, OPEN_RECORD);
        fseek(fp_brief, (long) index * BRIEF_SIZE, SEEK_SET);
        fwrite(&glob->brief, BRIEF_SIZE, 1, fp_brief);
        strcpy(a_human->userid, OPEN_RECORD);
        fseek(fp, (long) index * HUMAN_SIZE, SEEK_SET);
        fwrite(a_human, HUMAN_SIZE, 1, fp);

        if (a_human->gang) {
          fclose(fp);
          fclose(fp_brief);
          Dead_Gangster(a_human->gang, index, a_human->rank);
          fp = fopen(USER_FILE, READB);
          fp_brief = fopen(BRIEF_FILE, READB);
        }
      }
    }
  } while (!done);

  fclose(fp);
  fclose(fp_brief);
}
/****************************************************************************
	set up the global network grid
*/
short Init_World(void) {
  short   i;
  short   net_x, net_y;
  short   custom_ok;
  FILE    *sfp;
  short   x, y;
  FILE    *fp;
  FILE    *new_fp;
  char    buf[4991];
  unlink(CITY_FILE);
  Create_New(CITY_FILE);
  fp = fopen("muicyber/data/cities.dat", READB);

  if (fp == NULL) {
    perror("Error opening file muicyber/data/cities.dat");
    exit(-1);
  }

  new_fp = fopen(CITY_FILE, APPENDB);

  if (new_fp == NULL) {
    perror("Error opening file for append muicyber/data/cities.dat");
    exit(-1);
  }

  for (net_x = 0; net_x != MAX_NET_X; ++net_x) {
    for (net_y = 0; net_y != MAX_NET_Y; ++net_y) {
      fread(buf, 309, 1, fp);
      cvtData
      (
        buf,
        a_city,
        309,
        cityFDA,
        CVTPACKED,
        CVTSERVER,
        CHAN_NUL
      );
      fwrite(a_city, CITY_SIZE, 1, new_fp);

      for (x = 0; x != MAX_CITY_X; ++x) {
        for (y = 0; y != MAX_CITY_Y; ++y) {
          if (a_city->type[x][y] && a_city->type[x][y] != PHONE) {
            sprintf
            (
              glob->path,
              "muicyber/data/systems/%d@%dx%d@.%df",
              net_x,
              net_y,
              x,
              y
            );
            sfp = fopen(glob->path, READB);

            if (sfp == NULL) {
              char sysbuff[1024];
              sprintf(sysbuff, "ERROR opening system file %s", glob->path);
              perror(sysbuff);
              exit(-1);
            } else {
              fread(buf, 4991, 1, sfp);
              fclose(sfp);
              cvtData
              (
                buf,
                a_system,
                4991,
                systemFDA,
                CVTPACKED,
                CVTSERVER,
                CHAN_NUL
              );
              sprintf
              (
                glob->path,
                "muicyber/data/systems/%d@%dx%d@.%df",
                net_x,
                net_y,
                x,
                y
              );
              unlink(glob->path);
              sfp = fopen(glob->path, APPENDB);
              fwrite(a_system, SYSTEM_SIZE, 1, sfp);
              fclose(sfp);

              if (a_system->permanant) {
                sprintf
                (
                  glob->path,
                  "muicyber/data/systems/%d@%dx%d@.%dd",
                  net_x,
                  net_y,
                  x,
                  y
                );
                sfp = fopen(glob->path, READB);

                if (sfp == NULL) {
                  perror("ERROR");
                  exit(-1);
                } else {
                  fread(buf, 4991, 1, sfp);
                  fclose(sfp);
                  cvtData
                  (
                    buf,
                    a_system,
                    4991,
                    systemFDA,
                    CVTPACKED,
                    CVTSERVER,
                    CHAN_NUL
                  );
                  sprintf
                  (
                    glob->path,
                    "muicyber/data/systems/%d@%dx%d@.%dd",
                    net_x,
                    net_y,
                    x,
                    y
                  );
                  unlink(glob->path);
                  sfp = fopen(glob->path, APPENDB);
                  fwrite(a_system, SYSTEM_SIZE, 1, sfp);
                  fclose(sfp);
                }
              }
            }
          }
        }
      }
    }
  }

  fclose(new_fp);
  fclose(fp);
  i = Read_City(0, 0, &world->city[0][0]);

  if (i == 0) {
    unlink(CITY_FILE);
    Create_New(CITY_FILE);
    Init_Cities();
  } else {
    printf("Cities already initialized; just reading\n");
  }

  for (net_x = 0; net_x != MAX_NET_X; ++net_x) {
    for (net_y = 0; net_y != MAX_NET_Y; ++net_y) {
      Read_City(net_x, net_y, &world->city[net_x][net_y]);
    }
  }

  printf("Reading custom city\n");
  custom_ok = Read_Custom_City(0, 0, a_city);

  if (!custom_ok) {
    printf("Initializing custom cities\n");
    Init_Custom_Cities();
  }

  if (glob->world_edit != 2) {
    printf("Reading custom cities\n");
    Read_Custom_Cities(-1, -1);
  }

  EXIT0;
}
/****************************************************************************
	pull in the customized city data
*/
short Read_Custom_Cities(short world_x, short world_y) {
  short   i;
  short   net_x, net_y;
  short   x, y;
  short   start_x, start_y;
  short   end_x, end_y;
  FILE    *fp;

  if (world_x == -1) {
    start_x = 0;
    end_x = MAX_NET_X;
    start_y = 0;
    end_y = MAX_NET_Y;
  } else {
    start_x = world_x;
    end_x = world_x + 1;
    start_y = world_y;
    end_y = world_y + 1;
  }

  for (net_x = start_x; net_x != end_x; ++net_x) {
    for (net_y = start_y; net_y != end_y; ++net_y) {
      Read_Custom_City(net_x, net_y, a_city);
      strcpy(world->city[net_x][net_y].name, a_city->name);
      world->city[net_x][net_y].security = a_city->security;
      world->city[net_x][net_y].landmass = a_city->landmass;
      world->city[net_x][net_y].invisible = a_city->invisible;

      for (x = 0; x != MAX_CITY_X; ++x) {
        for (y = 0; y != MAX_CITY_Y; ++y) {
          if (a_city->type[x][y] == DELETED_NODE) {
            if (world->city[net_x][net_y].type[x][y]) {
              world->city[net_x][net_y].type[x][y] = 0;
            } else {
              a_city->type[x][y] = 0;
              Write_Custom_City(net_x, net_y, a_city);
            }
          } else if (a_city->type[x][y] == PHONE) {
            world->city[net_x][net_y].type[x][y] = PHONE;
          } else if (a_city->type[x][y]) {
            Set_Custom_Path(x, y, net_x, net_y);
            fp = fopen(glob->path, READB);

            if (fp == NULL) {
              a_city->type[x][y] = 0;
              Write_Custom_City(net_x, net_y, a_city);
            } else {
              i = fread(a_system, SYSTEM_SIZE, 1, fp);
              fclose(fp);

              if (i != 1) {
                a_city->type[x][y] = 0;
                Write_Custom_City(net_x, net_y, a_city);
              } else {
                Set_System_Path(x, y, net_x, net_y);
                unlink(glob->path);
                fp = fopen(glob->path, APPENDB);
                fwrite(a_system, SYSTEM_SIZE, 1, fp);
                fclose(fp);

                if (a_system->permanant) {
                  Set_Custom_Perm_Path(x, y, net_x, net_y);
                  fp = fopen(glob->path, READB);
                  fread(a_system, SYSTEM_SIZE, 1, fp);
                  fclose(fp);
                  Set_Perm_Path(x, y, net_x, net_y);
                  unlink(glob->path);
                  fp = fopen(glob->path, APPENDB);
                  fwrite(a_system, SYSTEM_SIZE, 1, fp);
                  fclose(fp);
                }

                world->city[net_x][net_y].type[x][y] =
                  a_city->type[x][y];
              }
            }
          }
        }
      }
    }
  }

  EXIT0;
}
/****************************************************************************
	init the city file
*/
void Init_Cities(void) {
  short   i;
  short   x, y;
  short   net_x, net_y;
  short   found;
  FILE    *fp;
  NOTIFY("LoC: Initializing Cities");

  for (net_x = 0; net_x != MAX_NET_X; ++net_x) {
    for (net_y = 0; net_y != MAX_NET_Y; ++net_y) {
      memset( (char *) a_city, 0, CITY_SIZE);

      for (i = 1, found = FALSE; i != NUM_CITIES; ++i) {
        if (City_Def[i].x == net_x && City_Def[i].y == net_y) {
          found = TRUE;
          strcpy(a_city->name, City_Def[i].name);
          a_city->security = City_Def[i].security;
          a_city->landmass = City_Def[i].landmass;
          a_city->invisible = FALSE;
          x = RANDOM(MAX_CITY_X - 1);
          y = RANDOM(MAX_CITY_Y - 1);
          a_city->type[x][y] = PHONE;
        }
      }

      if (!found) {
        strcpy(a_city->name, FRONTIER);
        a_city->invisible = FALSE;
      }

      fp = fopen(CITY_FILE, APPENDB);
      fwrite(a_city, CITY_SIZE, 1, fp);
      fclose(fp);
    }
  }
}
/****************************************************************************
	init the custom city file
*/
void Init_Custom_Cities(void) {
  short   net_x, net_y;
  FILE    *fp;

  for (net_x = 0; net_x != MAX_NET_X; ++net_x) {
    for (net_y = 0; net_y != MAX_NET_Y; ++net_y) {
      memset( (char *) a_city, 0, CITY_SIZE);
      strcpy(a_city->name, world->city[net_x][net_y].name);
      a_city->security = world->city[net_x][net_y].security;
      a_city->landmass = world->city[net_x][net_y].landmass;
      a_city->invisible = world->city[net_x][net_y].invisible;
      fp = fopen(CUSTOM_FILE, APPENDB);
      fwrite(a_city, CITY_SIZE, 1, fp);
      fclose(fp);
    }
  }
}
/****************************************************************************
	displays help for the specified program
*/
short Program_Help(char *word) {
  short   displaying = FALSE;
  short   done = FALSE;
  char    *read_result;
  char    txt[LINE_LEN];
  FILE    *fp;
  sprintf(str, "%s/proghelp.txt", HELP_PATH);
  fp = fopen(str, READA);

  if (fp == NULL) {
    prfmsg(MSG567);
    EXIT0;
  }

  do {
    read_result = fgets(txt, 1000, fp);

    if (read_result == NULL) {
      done = TRUE;
    } else {
      if (txt[0] == '*') {
        if (displaying) {
          displaying = FALSE;
          done = TRUE;
        } else if (sameto(word, &txt[1])) {
          displaying = TRUE;
        }
      }

      if (displaying) {
        prfmsg(M568, txt, CLEAR_RIGHT);
      }
    }
  } while (!done);

  fclose(fp);
  EXIT0;
}
/****************************************************************************
	during cleanup, process everyone's internet bill
*/
short Internet_Billing(void) {
  short   n;
  short   index = -1;
  short   written;
  FILE    *fp;
  fp = fopen(USER_FILE, READB);

  do {
    ++index;
    fseek(fp, (long) index * HUMAN_SIZE, SEEK_SET);
    n = fread(a_human, HUMAN_SIZE, 1, fp);

    if (n == 1 && !sameas(a_human->userid, OPEN_RECORD)) {
      a_human->invite = 0;

      if (a_human->gang_pts_today && a_human->gang) {
        Read_Gang(a_human->gang);
        ++gang->scorers;
        Write_Gang(a_human->gang);
      }

      a_human->gang_pts_today = 0;
      written = FALSE;

      if
      (
        a_human->internet.owed <= a_human->bank.balance ||
        a_human->perm[FREE_PERM]
      ) {
        if (!a_human->perm[FREE_PERM] && a_human->internet.owed) {
          Add_Bank_Record
          (
            a_human,
            -(a_human->internet.owed),
            -1, -1,
            "AUTOMATED",
            "Internet Bill"
          );
          written = TRUE;
        }

        if (a_human->internet.credit_rating < 10) {
          ++a_human->internet.credit_rating;
        }

        if (a_human->internet.owed) {
          Add_Internet_Record
          (
            a_human,
            -(a_human->internet.owed),
            -1, -1,
            "AUTOMATED",
            "Bill Paid"
          );
          written = TRUE;
        }
      } else {
        if (a_human->internet.credit_rating) {
          --a_human->internet.credit_rating;
        }

        if (a_human->bank.balance) {
          Add_Internet_Record
          (
            a_human,
            -(a_human->bank.balance),
            -1, -1,
            "AUTOMATED",
            "Bill Paid"
          );
          Add_Bank_Record
          (
            a_human,
            -(a_human->bank.balance),
            -1, -1,
            "AUTOMATED",
            "Internet Bill"
          );
          written = TRUE;
        } else {
          Add_Internet_Record
          (
            a_human,
            0L,
            -1, -1,
            "AUTOMATED",
            "Delinquent!"
          );
          written = TRUE;
        }
      }

      if (!written) {
        Add_Internet_Record
        (
          a_human,
          0L,
          -1, -1,
          "AUTOMATED",
          "No Billing Today"
        );
      }
    }
  } while (n == 1);

  fclose(fp);
  EXIT0;
}
/****************************************************************************
	displays the contents of a text file
*/
short Dump_File(char *file) {
  short   i;
  short   done = FALSE;
  char    *read_result;
  char    txt[LINE_LEN];

  if (STATE != DUMP_FILE_PROMPT) {
    /*for (i = 0; i != strlen(file); ++i)
    {
    	file[i] = toupper(file[i]);
    }*/
    MY_PORT->fp = fopen(file, READA);

    if (MY_PORT->fp == NULL) {
      prfmsg(MSG567);
      EXIT0;
    }
  } else if (margc && sameas(margv[0], ANS_NO)) {
    fclose(MY_PORT->fp);
    STATE = MAIN_PROMPT;
    EXIT0;
  } else if (margc == 0) {
    Clear_Display_Window();
  }

  for (i = DISP_Y; i != ASYNCH_Y && !done; ++i) {
    read_result = fgets(txt, 1000, MY_PORT->fp);

    if (read_result != NULL) {
      prfmsg(M568, txt, CLEAR_RIGHT);
    } else {
      fclose(MY_PORT->fp);
      done = TRUE;
      STATE = MAIN_PROMPT;
    }
  }

  if (!done) {
    Prompt(getmsg(MSG243), DUMP_FILE_PROMPT);
  } else {
    prf(NL);
  }

  EXIT0;
}
/****************************************************************************
	print who was on
*/
short Dayfile(void) {
  FILE    *fp;
  short   i;
  short   n;
  short   done = FALSE;
  struct  day_struct day_entry;
  fp = fopen(DAY_FILE, READB);

  if (fp == NULL) {
    NOTIFY("LoC: DAYFILE OPEN FAILED!");
    EXIT0;
  }

  prf("%c%s", CTRL_S, CLEAR_SCREEN);
  prfmsg(MSG9);

  for (i = 0; i != MAX_LEDGER && !done; ++i) {
    n = fread(&day_entry, DAY_SIZE, 1, fp);

    if (n != 1 || day_entry.port >= max_guys || day_entry.port < 0) {
      done = TRUE;
    } else {
      sprintf
      (
        str,
        getmsg(MSG10),
        day_entry.port + 1,
        day_entry.dateon,
        day_entry.timeon,
        day_entry.secs / 60,
        day_entry.userid
      );
      prf(str);
    }
  }

  fclose(fp);
  MY_PORT->prompt_y = ASYNCH_Y - 1;
  Prompt(getmsg(MSG207), ANY_KEY_PROMPT);
  EXIT0;
}
/*****************************************************************************
	put new entry in dayfile
*/
short Day_Out(void) {
  short   i;
  long    lsk;
  FILE    *fp;
  struct  day_struct  day_entry;

  if (MY_PORT->secs_on < 60 || MY_PORT->invisible) {
    EXIT0;
  }

  fp = fopen(DAY_FILE, READB);

  if (fp == NULL) {
    NOTIFY("LoC: DAYFILE OPEN FAILED!");
    EXIT0;
  }

  for (i = MAX_LEDGER - 2; i > -1; --i) {
    lsk = (long) i * DAY_SIZE;
    fseek(fp, lsk, SEEK_SET);
    fread(&day_entry, DAY_SIZE, 1, fp);
    lsk = (long) (i + 1) * DAY_SIZE;
    fseek(fp, lsk, SEEK_SET);
    fwrite(&day_entry, DAY_SIZE, 1, fp);
  }

  day_entry.port = myp;
  day_entry.secs = MY_PORT->secs_on;
  TODAY(day_entry.dateon);
  CONV_TIME(day_entry.timeon, MY_PORT->time_on);
  strcpy(day_entry.userid, me.userid);
  lsk = 0;
  fseek(fp, lsk, SEEK_SET);
  fwrite(&day_entry, DAY_SIZE, 1, fp);
  fclose(fp);
  EXIT0;
}
/****************************************************************************
	display the contents of a document
*/
void   Display_Doc(file)
struct memory_struct *file;
{
  if (file->class == USELESS_DOC) {
    prfmsg(MSG615);
  } else if (file->class == BUSINESS_DOC) {
    prfmsg(MSG616);
  } else if (file->class == SECRET_DOC) {
    prfmsg(MSG617);
  } else if (file->class == ENDGAME_DOC) {
    prfmsg(MSG715);
  } else if (file->class == TOP_SECRET_DOC) {
    prfmsg(MSG618);
  } else if (file->class == BANK_DOC) {
    Read_Human(file->index, a_human);

    if (!sameas(a_human->userid, OPEN_RECORD)) {
      prfmsg(MSG647, &file->name[9], a_human->alias);
    } else {
      file->class = USELESS_DOC;
      prfmsg(MSG615);
    }
  } else if (file->class == INTERNET_DOC) {
    Read_Human(file->index, a_human);

    if (!sameas(a_human->userid, OPEN_RECORD)) {
      prfmsg(MSG648, &file->name[9], a_human->alias);
    } else {
      file->class = USELESS_DOC;
      prfmsg(MSG615);
    }
  } else if (file->class == GANG_DOC) {
    Read_Human(file->index, a_human);

    if
    (
      !sameas(a_human->userid, OPEN_RECORD) &&
      sameto(A_GANG, a_human->userid)
    ) {
      prfmsg(MSG828, &file->name[9], a_human->alias);
    } else {
      file->class = USELESS_DOC;
      prfmsg(MSG615);
    }
  } else if (file->class == PASSWORD_DOC) {
    prfmsg(MSG674, &file->name[9]);
  }
}
/****************************************************************************
	create a data file
*/
void Create_New(path)
char *path;
{
  FILE    *fp;
  fp = fopen(path, "a+b");

  if (fp == NULL) {
    char buff[1024];
    sprintf(buff, "Error creating file %s a+b", path);
    perror(buff);
    exit(-1);
  }

  fclose(fp);
}
/****************************************************************************
	write the specified city out to the city file
*/
short Write_City(short net_x, short net_y, struct city_struct *some_city) {
  long    index = -1;
  short   done = FALSE;
  short   x, y;
  FILE    *fp;
  fp = fopen(CITY_FILE, READB);

  if (fp == NULL) {
    NOTIFY("LoC: CITY file open failed!");
    EXIT0;
  }

  for (x = 0; x != MAX_CITY_X && !done; ++x) {
    for (y = 0; y != MAX_CITY_Y && !done; ++y) {
      ++index;

      if (x == net_x && y == net_y) {
        done = TRUE;
      }
    }
  }

  if (!done) {
    sprintf(str, "LoC: Unknown Net Loc %d/%d", net_x, net_y);
    NOTIFY(str);
    EXIT0;
  }

  fseek(fp, (long) index * CITY_SIZE, SEEK_SET);
  fwrite(some_city, CITY_SIZE, 1, fp);
  fclose(fp);
  EXIT0;
}
/****************************************************************************
	write the specified city out to the custom city file
*/
short Write_Custom_City(short net_x, short net_y, struct city_struct *some_city) {
  long    index = -1;
  short   done = FALSE;
  short   x, y;
  FILE    *fp;
  fp = fopen(CUSTOM_FILE, READB);

  if (fp == NULL) {
    NOTIFY("LoC: CUSTOM file open failed!");
    EXIT0;
  }

  for (x = 0; x != MAX_CITY_X && !done; ++x) {
    for (y = 0; y != MAX_CITY_Y && !done; ++y) {
      ++index;

      if (x == net_x && y == net_y) {
        done = TRUE;
      }
    }
  }

  if (!done) {
    sprintf(str, "LoC: Unknown Net Loc %d/%d", net_x, net_y);
    NOTIFY(str);
    EXIT0;
  }

  fseek(fp, (long) index * CITY_SIZE, SEEK_SET);
  fwrite(some_city, CITY_SIZE, 1, fp);
  fclose(fp);
  EXIT0;
}
/****************************************************************************
	read the specified city from the customized city file
*/
short Read_Custom_City(short net_x, short net_y, struct city_struct *some_city) {
  short   n;
  long    index = -1;
  short   done = FALSE;
  short   x, y;
  FILE    *fp;
  fp = fopen(CUSTOM_FILE, READB);

  if (fp == NULL) {
    NOTIFY("LoC: CUSTOM file open failed!");
    EXIT0;
  }

  for (x = 0; x != MAX_CITY_X && !done; ++x) {
    for (y = 0; y != MAX_CITY_Y && !done; ++y) {
      ++index;

      if (x == net_x && y == net_y) {
        done = TRUE;
      }
    }
  }

  if (!done) {
    sprintf(str, "LoC: Unknown Net Loc %d/%d", net_x, net_y);
    NOTIFY(str);
    EXIT0;
  }

  fseek(fp, (long) index * CITY_SIZE, SEEK_SET);
  n = fread(some_city, CITY_SIZE, 1, fp);
  fclose(fp);

  if (n != 1) {
    EXIT0;
  }

  return (TRUE);
}
/****************************************************************************
	read the specified city from the city file
*/
short Read_City(short net_x, short net_y, struct city_struct *some_city) {
  short   n;
  long    index = -1;
  short   done = FALSE;
  short   x, y;
  FILE    *fp;
  fp = fopen(CITY_FILE, READB);

  if (fp == NULL) {
    NOTIFY("LoC: CITY file open failed!");
    EXIT0;
  }

  for (x = 0; x != MAX_CITY_X && !done; ++x) {
    for (y = 0; y != MAX_CITY_Y && !done; ++y) {
      ++index;

      if (x == net_x && y == net_y) {
        done = TRUE;
      }
    }
  }

  if (!done) {
    sprintf(str, "LoC: Unknown Net Loc %d/%d", net_x, net_y);
    NOTIFY(str);
    EXIT0;
  }

  fseek(fp, (long) index * CITY_SIZE, SEEK_SET);
  n = fread(some_city, CITY_SIZE, 1, fp);
  fclose(fp);

  if (n != 1) {
    sprintf(str, "LoC: Net Read on %d/%d failed", net_x, net_y);
    NOTIFY(str);
    EXIT0;
  }

  return (TRUE);
}
/****************************************************************************
	see if there is an available system slot for me
*/
short System_Move_Check(short net_x, short net_y, short city_x, short city_y) {
  short   i;

  if (world->city[net_x][net_y].type[city_x][city_y] == END_SYS) {
    if
    (
      !me.perm[GAMEOP_PERM] &&
      His_Rank(me.score, me.won) < NUM_RANKS - 2
    ) {
      Clear_Display_Window();
      prfmsg(MSG558);
      return (FALSE);
    }

    if (!System_Is_Running(myp, KEY1)) {
      Clear_Display_Window();
      prfmsg(MSG714, Programs[KEY1].Name);
      prfmsg(MSG558);
      return (FALSE);
    }

    if (!System_Is_Running(myp, KEY2)) {
      Clear_Display_Window();
      prfmsg(MSG714, Programs[KEY2].Name);
      prfmsg(MSG558);
      return (FALSE);
    }

    if (!System_Is_Running(myp, KEY3)) {
      Clear_Display_Window();
      prfmsg(MSG714, Programs[KEY3].Name);
      prfmsg(MSG558);
      return (FALSE);
    }
  }

  for (i = 0; i != max_systems; ++i) {
    if (Sys[i]->in_use) {
      if (Sys[i]->net_x == net_x && Sys[i]->net_y == net_y) {
        if (Sys[i]->city_x == city_x && Sys[i]->city_y == city_y) {
          return (TRUE);
        }
      }
    } else if (i < max_guys && Port[i]->active) {
      if (Sys[i]->net_x == net_x && Sys[i]->net_y == net_y) {
        if (Sys[i]->city_x == city_x && Sys[i]->city_y == city_y) {
          return (TRUE);
        }
      }
    } else if (i >= max_guys && i < glob->cop_sys) {
      return (TRUE);
    }
  }

  EXIT0;
}
/****************************************************************************
	user is moving into a system
*/
void Move_To_System(short net_x, short net_y, short city_x, short city_y) {
  long    elapsed;
  short   found = FALSE;
  short   i;
  short   regen = FALSE;
  short   avail = -1;
  FILE    *fp;

  for (i = 0; i != max_systems && !found; ++i) {
    if (Sys[i]->in_use) {
      if
      (
        Sys[i]->net_x == net_x &&
        Sys[i]->net_y == net_y &&
        Sys[i]->city_x == city_x &&
        Sys[i]->city_y == city_y
      ) {
        MY_PORT->sys_num = i;
        found = TRUE;
      }
    } else if
    (
      i < max_guys &&
      Port[i]->active &&
      (i == myp || Port[i]->access_level != IDLE)
    ) {
      if
      (
        Sys[i]->net_x == net_x &&
        Sys[i]->net_y == net_y &&
        Sys[i]->city_x == city_x &&
        Sys[i]->city_y == city_y
      ) {
        MY_PORT->sys_num = i;
        found = TRUE;
      }
    } else if (i >= max_guys && i < glob->cop_sys) {
      avail = i;
    }
  }

  if (!found) {
    if (avail == -1) {
      catastro("LoC: Fail #5");
    }

    MY_PORT->sys_num = avail;
    glob->blob[MY_PORT->sys_num] = 0;

    if (MY_NODE == END_SYS) {
      Set_Valhalla_Path(city_x, city_y);
      fp = fopen(glob->path, READB);

      if (fp == NULL) {
        regen = TRUE;
        Create_New(glob->path);
      } else {
        i = fread(MY_SYS, SYSTEM_SIZE, 1, fp);
        fclose(fp);

        if (i != 1) {
          regen = TRUE;
        } else if (time(0) > MY_SYS->rebuild_time) {
          regen = TRUE;
        }
      }

      if (regen) {
        memset( (char *) MY_SYS, 0, SYSTEM_SIZE);
        strcpy(MY_SYS->Name, VALHALLA);
        MY_SYS->type = END_SYS;

        if (city_x != glob->end_x || city_y != glob->end_y) {
          Init_System(MY_SYS, END_SYS);
        } else {
          Init_System(MY_SYS, ENDGAME_SYS);
        }

        MY_SYS->rebuild_time = time(0) + VALHALLA_REBUILD;
      }
    } else if (MY_NODE == ALIEN_SYS) {
      memset( (char *) MY_SYS, 0, SYSTEM_SIZE);
      strcpy(MY_SYS->Name, MY_CITY.name);
      MY_SYS->type = ALIEN_SYS;
      Init_System(MY_SYS, ALIEN_SYS);
    } else if (MY_NODE == CYBERIA_SYS) {
      memset( (char *) MY_SYS, 0, SYSTEM_SIZE);
      strcpy(MY_SYS->Name, MY_CITY.name);
      MY_SYS->type = CYBERIA_SYS;
      Init_System(MY_SYS, CYBERIA_SYS);
    } else {
      Set_System_Path(city_x, city_y, net_x, net_y);
      fp = fopen(glob->path, READB);

      if (fp == NULL) {
        NOTIFY("LoC: System OPEN failure!");
        catastro
        (
          "LoC: Fail #1: %s n=%d/%d c=%d/%d s=%d",
          glob->path,
          net_x, net_y,
          city_x, city_y,
          MY_NODE
        );
      }

      i = fread(MY_SYS, SYSTEM_SIZE, 1, fp);
      fclose(fp);

      if (i != 1) {
        NOTIFY("LoC: System READ failure!");
        catastro
        (
          "LoC: Fail #2: n=%d/%d c=%d/%d s=%d",
          net_x, net_y,
          city_x, city_y,
          MY_NODE
        );
      }

      if (MY_SYS->permanant) {
        Set_Perm_Path(city_x, city_y, net_x, net_y);
        fp = fopen(glob->path, READB);

        if (fp == NULL) {
          NOTIFY("LoC: Perm System OPEN failure!");
          catastro
          (
            "LoC: Fail #3: n=%d/%d c=%d/%d s=%d",
            net_x, net_y,
            city_x, city_y,
            MY_NODE
          );
        }

        i = fread(a_system, SYSTEM_SIZE, 1, fp);
        fclose(fp);

        if (i != 1) {
          NOTIFY("LoC: Perm System READ failure!");
          catastro
          (
            "LoC: Fail #4: n=%d/%d c=%d/%d s=%d",
            net_x, net_y,
            city_x, city_y,
            MY_NODE
          );
        }

        if
        (
          (a_system->rebuild_time &&
           time(0) > MY_SYS->rebuild_time + a_system->rebuild_time) ||
          MY_SYS->last_write < tourney->phase_start
        ) {
          memcpy(MY_SYS, a_system, SYSTEM_SIZE);
          MY_SYS->rebuild_time = time(0);
        }

        MY_SYS->type = MY_NODE;
        MY_SYS->password[0] = 0;
      } else if
      (
        MY_SYS->rebuild_time < time(0) ||
        MY_SYS->cpus == 0 ||
        MY_SYS->last_write < tourney->phase_start
      ) {
        Init_System(MY_SYS, MY_NODE);
        MY_SYS->rebuild_time = time(0) + (glob->tmpdur * 60);
      }
    }

    if
    (
      !MY_SYS->permanant &&
      MY_NODE == BBS &&
      time(0) - MY_SYS->last_write > 30
    ) {
      Free_BBS_Stuff(MY_SYS);
    }

    elapsed = time(0) - MY_SYS->last_write;

    for (i = 0; i != MY_SYS->cpus; ++i) {
      if (elapsed > MY_SYS->crash_time[i]) {
        MY_SYS->crash_time[i] = 0;
      } else {
        MY_SYS->crash_time[i] -= elapsed;
      }
    }

    for (i = 0; i != MAX_CPUS + 1; ++i) {
      Proc[MY_PORT->sys_num]->cpu[i].m_index = -1;
    }

    Proc[MY_PORT->sys_num]->file_server = 0;
    Proc[MY_PORT->sys_num]->moled = FALSE;
    Proc[MY_PORT->sys_num]->infected = FALSE;
    Proc[MY_PORT->sys_num]->torped = FALSE;
    MY_MISC_CITY.camoflaged[MY_SYS->city_x][MY_SYS->city_y] = FALSE;
  }

  if (command != JACK) {
    sprintf(str, getmsg(MSG55), me.alias, MY_SYS->Name);
    City_Message(str, MY_SYS->net_x, MY_SYS->net_y, myp);
  }

  Proc[MY_PORT->sys_num]->enemy[myp] = FALSE;
  MY_PORT->access_level = SYSTEM_VIEW;
  MY_PORT->gx = -1;
  MY_PORT->gy = -1;
  MY_PORT->wx = -1;
  MY_PORT->wy = -1;
  MY_PORT->px = -1;
  MY_PORT->py = -1;
  MY_SYS->in_use = TRUE;
  MY_SYS->net_x = net_x;
  MY_SYS->net_y = net_y;
  MY_SYS->city_x = city_x;
  MY_SYS->city_y = city_y;
}
/****************************************************************************
	user is leaving system, clean it up after him
*/
void Leaving_System(short jacking_out) {
  short   s, i;
  short   x, y;
  short   g;
  short   in_use = FALSE;
  FILE    *fp;
  short   old_sx = MY_PORT->sys_x;
  short   old_sy = MY_PORT->sys_y;
  MY_PORT->code[0] = 0;
  Proc[MY_PORT->sys_num]->enemy[myp] = FALSE;

  if (MY_PORT->sys_x != -1) {
    MY_SYS->node_type[MY_PORT->sys_x][MY_PORT->sys_y] = 0;
  }

  if (MY_PORT->gx != -1) {
    if (MY_SYS->type != BBS) {
      MY_SYS->node_type[MY_PORT->gx][MY_PORT->gy] = CLOSED_GATE;
    } else {
      MY_SYS->node_type[MY_PORT->gx][MY_PORT->gy] = OPENED_GATE;
    }

    MY_PORT->gx = -1;
    MY_PORT->gy = -1;
  }

  if (MY_PORT->wx != -1) {
    MY_SYS->node_type[MY_PORT->wx][MY_PORT->wy] = DEAD_WALL;
    MY_SYS->breached = TRUE;
    MY_PORT->wx = -1;
    MY_PORT->wy = -1;
  }

  if (MY_PORT->px != -1) {
    MY_SYS->node_type[MY_PORT->px][MY_PORT->py] = PHONE;
    MY_PORT->px = -1;
    MY_PORT->py = -1;
  }

  if (MY_PORT->sys_num == myp) {
    if (jacking_out) {
      in_use = FALSE;
    } else {
      in_use = TRUE;
    }
  } else if (MY_PORT->sys_num < max_guys) {
    in_use = TRUE;
  } else if (MY_SYS->type == COP_SYS) {
    in_use = TRUE;
  } else {
    for (g = 0; g != max_guys; ++g) {
      if
      (
        g != myp &&
        Port[g]->active &&
        Port[g]->sys_num == MY_PORT->sys_num
      ) {
        in_use = TRUE;
      }
    }

    for (s = 0; s != max_systems; ++s) {
      if (s != MY_PORT->sys_num && Sys[s]->in_use) {
        for (i = 0; i != Sys[s]->cpus; ++i) {
          if
          (
            SYS_PROGRAM.m_index >= 0 &&
            SYS_PROGRAM.net_x == MY_PORT->net_x &&
            SYS_PROGRAM.net_y == MY_PORT->net_y &&
            SYS_PROGRAM.city_x == MY_PORT->city_x &&
            SYS_PROGRAM.city_y == MY_PORT->city_y &&
            SYS_PROGRAM.sys_x != -1 &&
            SYS_PROGRAM.sys_y != -1
          ) {
            in_use = TRUE;
          }
        }
      }
    }

    s = MY_PORT->sys_num;

    if (s >= max_guys) {
      for (i = 0; i != Sys[s]->cpus; ++i) {
        if
        (
          SYS_PROGRAM.m_index >= 0 &&
          (
            SYS_SOURCE.class == HYDRA ||
            SYS_SOURCE.index == SURGE ||
            SYS_SOURCE.index == ICEBOLT ||
            SYS_SOURCE.index == HERCULES ||
            SYS_SOURCE.index == INFILTRATE ||
            SYS_SOURCE.index == HANDCUFF ||
            SYS_SOURCE.index == FIREBOLT ||
            SYS_SOURCE.index == BANSHEE ||
            SYS_SOURCE.index == JOLT ||
            SYS_SOURCE.index == MIMIC ||
            SYS_SOURCE.index == BRAINDRAIN ||
            SYS_SOURCE.index == LOBOTOMY ||
            SYS_SOURCE.index == LIGHTNING ||
            SYS_SOURCE.index == PUNTER
          )
        ) {
          in_use = TRUE;
        }
      }
    }
  }

  MY_PORT->access_level = CITY_VIEW;
  File_Server_Check(MY_PORT->sys_num);
  MY_SYS->in_use = in_use;

  if
  (
    !in_use &&
    MY_SYS->type != ALIEN_SYS &&
    MY_SYS->type != CYBERIA_SYS
  ) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      for (x = 0; x != MAX_SYS_X; ++x) {
        if
        (
          MY_SYS->type != BBS &&
          MY_SYS->node_type[x][y] == OPENED_GATE
        ) {
          MY_SYS->node_type[x][y] = CLOSED_GATE;
        }
      }
    }

    if (MY_PORT->sys_num >= max_guys) {
      MY_SYS->last_write = time(0);

      if (MY_SYS->type != END_SYS) {
        Set_System_Path
        (
          MY_PORT->city_x,
          MY_PORT->city_y,
          MY_PORT->net_x,
          MY_PORT->net_y
        );
      } else {
        Set_Valhalla_Path(MY_PORT->city_x, MY_PORT->city_y);
      }

      fp = fopen(glob->path, READB);
      fwrite(MY_SYS, SYSTEM_SIZE, 1, fp);
      fclose(fp);
    }
  }

  MY_PORT->sys_num = -1;
  Update_Location();

  if (old_sx != -1) {
    Quick_Update
    (
      SYSTEM_VIEW,
      MY_PORT->net_x, MY_PORT->net_y,
      MY_PORT->city_x, MY_PORT->city_y,
      old_sx, old_sy,
      0, 0
    );
  }
}
/****************************************************************************
	see if non-local programs are gone. if so, close down system
*/
short Close_System_Check(short s, short option) {
  short   i;
  short   x, y;
  short   g, p;
  FILE    *fp;

  if (s < max_guys || Sys[s]->type == COP_SYS || Sys[s]->type == CON_SYS) {
    EXIT0;
  }

  for (g = 0; g != max_guys; ++g) {
    if
    (
      !glob->shutdown &&
      Port[g]->active &&
      Port[g]->sys_num == s &&
      Port[g]->access_level != IDLE
    ) {
      EXIT0;
    }
  }

  for (g = 0; g != max_systems; ++g) {
    if (g != s && Sys[g]->in_use) {
      for (p = 0; p != Sys[g]->cpus; ++p) {
        if
        (
          !glob->shutdown &&
          GUY_PROGRAM.m_index >= 0 &&
          GUY_PROGRAM.net_x == Sys[s]->net_x &&
          GUY_PROGRAM.net_y == Sys[s]->net_y &&
          GUY_PROGRAM.city_x == Sys[s]->city_x &&
          GUY_PROGRAM.city_y == Sys[s]->city_y &&
          GUY_PROGRAM.sys_x != -1 &&
          GUY_PROGRAM.sys_y != -1
        ) {
          EXIT0;
        }
      }
    }
  }

  for (i = 0; i != Sys[s]->cpus; ++i) {
    if
    (
      !glob->shutdown &&
      SYS_PROGRAM.m_index >= 0 &&
      (
        SYS_SOURCE.class == HYDRA ||
        SYS_SOURCE.index == SURGE ||
        SYS_SOURCE.index == ICEBOLT ||
        SYS_SOURCE.index == HERCULES ||
        SYS_SOURCE.index == INFILTRATE ||
        SYS_SOURCE.index == HANDCUFF ||
        SYS_SOURCE.index == FIREBOLT ||
        SYS_SOURCE.index == BANSHEE ||
        SYS_SOURCE.index == JOLT ||
        SYS_SOURCE.index == MIMIC ||
        SYS_SOURCE.index == BRAINDRAIN ||
        SYS_SOURCE.index == LOBOTOMY ||
        SYS_SOURCE.index == LIGHTNING ||
        SYS_SOURCE.index == PUNTER
      )
    ) {
      EXIT0;
    }
  }

  if (option) {
    return (TRUE);
  }

  Stop_All_Programs(s);
  Sys[s]->in_use = FALSE;

  if (Sys[s]->type == ALIEN_SYS || Sys[s]->type == CYBERIA_SYS) {
    EXIT0;
  }

  for (y = 0; y != MAX_SYS_Y; ++y) {
    for (x = 0; x != MAX_SYS_X; ++x) {
      if
      (
        Sys[s]->type != BBS &&
        Sys[s]->node_type[x][y] == OPENED_GATE
      ) {
        Sys[s]->node_type[x][y] = CLOSED_GATE;
      }
    }
  }

  Sys[s]->last_write = time(0);

  if (Sys[s]->type != END_SYS) {
    Set_System_Path
    (
      Sys[s]->city_x,
      Sys[s]->city_y,
      Sys[s]->net_x,
      Sys[s]->net_y
    );
  } else {
    Set_Valhalla_Path(Sys[s]->city_x, Sys[s]->city_y);
  }

  fp = fopen(glob->path, READB);

  if (fp != NULL) {
    fwrite(Sys[s], SYSTEM_SIZE, 1, fp);
    fclose(fp);
  }

  EXIT0;
}
/*****************************************************************************
	find my user record
*/
short Read_My_Rec(struct human_struct *my_record) {
  short   i;
  short   n;
  short   index = -1;
  long    elapsed;
  FILE    *fp;
  fp = fopen(BRIEF_FILE, READB);

  do {
    n = fread(&glob->brief, BRIEF_SIZE, 1, fp);
    ++index;
  } while (n == 1 && !sameas(glob->brief.userid, usaptr->userid));

  fclose(fp);

  if (n == 0) {
    index = -1;
  } else {
    fp = fopen(USER_FILE, READB);
    fseek(fp, (long) index * HUMAN_SIZE, SEEK_SET);
    fread(my_record, HUMAN_SIZE, 1, fp);
    fclose(fp);
    fp = fopen(DECK_FILE, READB);
    fseek(fp, (long) index * SYSTEM_SIZE, SEEK_SET);
    fread(a_system, SYSTEM_SIZE, 1, fp);
    fclose(fp);
    a_system->in_use = FALSE;
    elapsed = time(0) - a_system->last_write;

    for (i = 0; i != a_system->cpus; ++i) {
      if (elapsed > a_system->crash_time[i]) {
        a_system->crash_time[i] = 0;
      } else {
        a_system->crash_time[i] -= elapsed;
      }
    }

    a_system->type = CYBERDECK;
  }

  return (index);
}
/*****************************************************************************
	display list of top users
*/
void Top_Nine(short option) {
  short   i;
  long    ltemp;
  short   temp;
  long    lowest;
  short   low_index;
  short   n;
  short   done;
  short   index = -1;
  short   indexes[9];
  long    score[9];
  FILE    *fp;
  fp = fopen(BRIEF_FILE, READB);

  for (n = 0; n != 9; ++n) {
    indexes[n] = -1;
    score[n] = -1;
    glob->winners = 0;
  }

  do {
    n = fread(&glob->brief, BRIEF_SIZE, 1, fp);
    ++index;

    if
    (
      n == 1 &&
      !sameas(glob->brief.userid, OPEN_RECORD) &&
      !sameto(A_GANG, glob->brief.userid) &&
      !glob->brief.perm[GAMEOP_PERM]
    ) {
      if (tourney->type == ENDGAME_TOURNEY) {
        if (glob->brief.won) {
          ++glob->winners;
        }
      } else if (tourney->type == SCORE_TOURNEY) {
        if (glob->brief.score >= tourney->score) {
          ++glob->winners;
        }
      }

      for (i = done = 0; i != 9 && !done; ++i) {
        if (indexes[i] == -1) {
          indexes[i] = index;
          score[i] = glob->brief.score;
          done = TRUE;
        }
      }

      if (!done) {
        lowest = -1;

        for (i = 0; i != 9; ++i) {
          if (score[i] < lowest || lowest == -1) {
            lowest = score[i];
            low_index = i;
          }
        }

        if (glob->brief.score > lowest) {
          score[low_index] = glob->brief.score;
          indexes[low_index] = glob->brief.index;
        }
      }
    }
  } while (n == 1);

  for (i = 0; i != 9; ++i) {
    for (n = 0; n != 9; ++n) {
      if (score[n] < score[i]) {
        ltemp = score[n];
        score[n] = score[i];
        score[i] = ltemp;
        temp = indexes[n];
        indexes[n] = indexes[i];
        indexes[i] = temp;
      }
    }
  }

  for (i = 0; i != 9; ++i) {
    if (score[i] != -1) {
      fseek(fp, (long) indexes[i] * BRIEF_SIZE, SEEK_SET);
      fread(&glob->brief, BRIEF_SIZE, 1, fp);

      if (option) {
        sprintf
        (
          str,
          getmsg(MSSG569),
          i + 1,
          glob->brief.alias,
          rank[His_Rank(glob->brief.score, glob->brief.won)].title,
          His_Rank(glob->brief.score, glob->brief.won),
          score[i]
        );
        prf(str);
      }

      if (i == 0) {
        sprintf
        (
          glob->topguy,
          "%s the %s",
          glob->brief.alias,
          rank[His_Rank(glob->brief.score, glob->brief.won)].title
        );
        strcpy(glob->winner, glob->brief.userid);
      }
    }
  }

  fclose(fp);
}
/*****************************************************************************
	find a user record for the given alias
*/
short Find_Alias(struct human_struct *someone, char *alias) {
  short   i;
  short   n;
  short   index = -1;
  short   found = FALSE;
  FILE    *fp;

  for (i = 0; i != max_guys; ++i) {
    if
    (
      Port[i]->active &&
      sameas(Port[i]->guy.alias, alias)
    ) {
      memcpy(someone, &Port[i]->guy, HUMAN_SIZE);
      return (someone->index);
    }
  }

  fp = fopen(BRIEF_FILE, READB);

  do {
    n = fread(&glob->brief, BRIEF_SIZE, 1, fp);

    if
    (
      !sameas(glob->brief.userid, OPEN_RECORD) &&
      sameas(glob->brief.alias, alias)
    ) {
      found = TRUE;
    }

    ++index;
  } while (n == 1 && !found);

  fclose(fp);

  if (n == 0) {
    index = -1;
  } else {
    fp = fopen(USER_FILE, READB);
    fseek(fp, (long) index * HUMAN_SIZE, SEEK_SET);
    fread(someone, HUMAN_SIZE, 1, fp);
    fclose(fp);
  }

  return (index);
}
/*****************************************************************************
	find a user record for the given internet code
*/
short Find_Internet_User(struct human_struct *someone, char *internet_code) {
  short   i;
  short   n;
  short   index = -1;
  short   found = FALSE;
  FILE    *fp;

  for (i = 0; i != max_guys; ++i) {
    if
    (
      Port[i]->active &&
      sameas(Port[i]->guy.internet.code, internet_code)
    ) {
      memcpy(someone, &Port[i]->guy, HUMAN_SIZE);
      return (someone->index);
    }
  }

  fp = fopen(BRIEF_FILE, READB);

  do {
    n = fread(&glob->brief, BRIEF_SIZE, 1, fp);

    if
    (
      !sameas(glob->brief.userid, OPEN_RECORD) &&
      sameas(glob->brief.internet_code, internet_code)
    ) {
      found = TRUE;
    }

    ++index;
  } while (n == 1 && !found);

  fclose(fp);

  if (n == 0) {
    index = -1;
  } else {
    fp = fopen(USER_FILE, READB);
    fseek(fp, (long) index * HUMAN_SIZE, SEEK_SET);
    fread(someone, HUMAN_SIZE, 1, fp);
    fclose(fp);
  }

  return (index);
}
/*****************************************************************************
	find a user record for the given bank code
*/
short Find_Bank_User(struct human_struct *someone, char *bank_code) {
  short   i;
  short   n;
  short   index = -1;
  short   found = FALSE;
  FILE    *fp;

  for (i = 0; i != max_guys; ++i) {
    if
    (
      Port[i]->active &&
      sameas(Port[i]->guy.bank.code, bank_code)
    ) {
      memcpy(someone, &Port[i]->guy, HUMAN_SIZE);
      return (someone->index);
    }
  }

  fp = fopen(BRIEF_FILE, READB);

  do {
    n = fread(&glob->brief, BRIEF_SIZE, 1, fp);

    if
    (
      !sameas(glob->brief.userid, OPEN_RECORD) &&
      sameas(glob->brief.bank_code, bank_code)
    ) {
      found = TRUE;
    }

    ++index;
  } while (n == 1 && !found);

  fclose(fp);

  if (n == 0) {
    index = -1;
  } else {
    fp = fopen(USER_FILE, READB);
    fseek(fp, (long) index * HUMAN_SIZE, SEEK_SET);
    fread(someone, HUMAN_SIZE, 1, fp);
    fclose(fp);
  }

  return (index);
}
/*****************************************************************************
	find a random non-sysop user record (for hacking purposes)
*/
short Find_Random_User(char *bank_code, char *internet_code) {
  short   i;
  short   n;
  short   ok;
  short   index = 0;
  FILE    *fp;
  fp = fopen(BRIEF_FILE, READB);

  do {
    n = fread(&glob->brief, BRIEF_SIZE, 1, fp);

    if (!sameas(glob->brief.userid, OPEN_RECORD)) {
      for (i = 0, ok = TRUE; i != NUM_PERMS && ok; ++i) {
        if (glob->brief.perm[i]) {
          ok = FALSE;
        }
      }

      if (ok && His_Rank(glob->brief.score, glob->brief.won) == 1) {
        ok = FALSE;
      }

      if (ok) {
        glob->random_user[index] = glob->brief.index;
        ++index;
      }
    }
  } while (n == 1 && index != MAX_RANDOM_USERS);

  fclose(fp);

  if (index == 0) {
    return (-1);
  }

  fp = fopen(USER_FILE, READB);
  n = glob->random_user[RANDOM(index) - 1];
  fseek(fp, (long) n * HUMAN_SIZE, SEEK_SET);
  fread(a_human, HUMAN_SIZE, 1, fp);
  fclose(fp);
  strcpy(bank_code, a_human->bank.code);
  strcpy(internet_code, a_human->internet.code);
  return (n);
}
/*****************************************************************************
	update some other user, online and in file
*/
short Write_User(struct human_struct *someone) {
  short   i;
  FILE    *fp;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && sameas(Port[i]->guy.userid, someone->userid)) {
      memcpy(&Port[i]->guy, someone, HUMAN_SIZE);
      EXIT0;
    }
  }

  fp = fopen(USER_FILE, READB);
  fseek(fp, (long) someone->index * HUMAN_SIZE, SEEK_SET);
  fwrite(someone, HUMAN_SIZE, 1, fp);
  fclose(fp);
  Set_Brief(someone, &glob->brief);
  fp = fopen(BRIEF_FILE, READB);
  fseek(fp, (long) glob->brief.index * BRIEF_SIZE, SEEK_SET);
  fwrite(&glob->brief, BRIEF_SIZE, 1, fp);
  fclose(fp);
  EXIT0;
}
/*****************************************************************************
	set the brief character fields
*/
void   Set_Brief(human, brief)
struct human_struct *human;
struct brief_human_struct *brief;
{
  short   i;
  brief->index          = human->index;
  brief->outlaw         = human->outlaw;
  brief->last_on        = human->last_on;
  brief->score          = human->score;
  brief->won            = human->won;
  brief->gang           = human->gang;
  brief->rank           = human->rank;
  brief->uploads        = human->uploads;
  brief->gang_bank      = human->gang_bank;
  brief->gang_pts       = human->gang_pts;
  brief->gang_pts_today = human->gang_pts_today;
  strcpy(brief->internet_code, human->internet.code);
  strcpy(brief->bank_code,     human->bank.code);
  strcpy(brief->alias,         human->alias);
  strcpy(brief->userid,        human->userid);

  for (i = 0; i != NUM_PERMS; ++i) {
    brief->perm[i] = human->perm[i];
  }
}
/*****************************************************************************
   writes my character out to user file
*/
void Write_My_Rec(short option) {
  short   n;
  short   found = FALSE;
  short   index = -1;
  FILE    *fp;
  fp = fopen(BRIEF_FILE, READB);
  if (fp == NULL) {
    fprintf(stderr, "Error opening brief file!\n");
    fflush(stderr);
    return;
  }

  if (option == NO_MATCH) {
    do {
      n = fread(&glob->brief, BRIEF_SIZE, 1, fp);

      if (n == 1) {
        if (sameas(glob->brief.userid, OPEN_RECORD)) {
          found = TRUE;
        }
      }

      ++index;
    } while (n == 1 && !found);

    me.index = index;
  } else {
    found = TRUE;
  }

  if (found) {
    if (option == SUICIDE) {
      strcpy(me.userid, OPEN_RECORD);
    }
    
    Set_Brief(&me, &glob->brief);
    fseek(fp, (long) glob->brief.index * BRIEF_SIZE, SEEK_SET);
    fwrite(&glob->brief, BRIEF_SIZE, 1, fp);
    fclose(fp);

    fp = fopen(USER_FILE, READB);
    if (fp == NULL) {
      fprintf(stderr, "Error opening user file\n");
      fflush(stderr);
      return;
    }
    fseek(fp, (long) me.index * HUMAN_SIZE, SEEK_SET);
    fwrite(&me, HUMAN_SIZE, 1, fp);
    fclose(fp);
    Write_Deck(me.index, Sys[myp]);
  } else {
    fclose(fp);
    fp = fopen(USER_FILE, APPENDB);
    fwrite(&me, HUMAN_SIZE, 1, fp);
    fclose(fp);
    Set_Brief(&me, &glob->brief);
    fp = fopen(BRIEF_FILE, APPENDB);
    fwrite(&glob->brief, BRIEF_SIZE, 1, fp);
    fclose(fp);
    fp = fopen(DECK_FILE, APPENDB);
    fwrite(Sys[myp], SYSTEM_SIZE, 1, fp);
    fclose(fp);
  }
}
/*****************************************************************************
   writes a character out to user file
*/
short Update_Human(struct human_struct *someone) {
  short   i;
  FILE    *fp;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && sameas(Port[i]->guy.userid, someone->userid)) {
      memcpy(&Port[i]->guy, someone, HUMAN_SIZE);
    }
  }

  fp = fopen(USER_FILE, READB);
  fseek(fp, (long) someone->index * HUMAN_SIZE, SEEK_SET);
  fwrite(someone, HUMAN_SIZE, 1, fp);
  fclose(fp);
  Set_Brief(someone, &glob->brief);
  fp = fopen(BRIEF_FILE, READB);
  fseek(fp, (long) glob->brief.index * BRIEF_SIZE, SEEK_SET);
  fwrite(&glob->brief, BRIEF_SIZE, 1, fp);
  fclose(fp);
  EXIT0;
}
/*****************************************************************************
   read the specified user record
*/
short Read_Human(short index, struct human_struct *someone) {
  short   i;
  short   n;
  FILE    *fp;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && Port[i]->guy.index == index) {
      memcpy(someone, &Port[i]->guy, HUMAN_SIZE);
      EXIT0;
    }
  }

  fp = fopen(USER_FILE, READB);
  fseek(fp, (long) index * HUMAN_SIZE, SEEK_SET);
  n = fread(someone, HUMAN_SIZE, 1, fp);
  fclose(fp);

  if (n != 1) {
    strcpy(a_human->alias, "Gumby");
  }

  EXIT0;
}
/*****************************************************************************
   writes my system out to the system file
*/
void EXPORT Write_Deck(short index, struct system_struct *deck) {
  FILE    *fp;
  deck->last_write = time(0);
  fp = fopen(DECK_FILE, READB);
  fseek(fp, (long) index * SYSTEM_SIZE, SEEK_SET);
  fwrite(deck, SYSTEM_SIZE, 1, fp);
  fclose(fp);
}
