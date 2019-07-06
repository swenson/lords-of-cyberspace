#include <header.h>
#include <globals.h>

/****************************************************************************
	tell me how powerful I am
*/
void EXPORT Display_My_Rank(void) {
  short   rnk = His_Rank(me.score, me.won);

  if (me.won) {
    sprintf
    (
      str,
      getmsg(MSSG673B),
      me.score,
      NUM_RANKS - 1,
      rank[NUM_RANKS - 1].title
    );
  } else if (rnk == NUM_RANKS - 2) {
    sprintf
    (
      str,
      getmsg(MSSG673A),
      me.score,
      rnk,
      rank[rnk].title,
      rnk + 1,
      rank[rnk + 1].title
    );
  } else {
    sprintf
    (
      str,
      getmsg(MSSG672),
      me.score,
      rnk,
      rank[rnk].title,
      rank[rnk + 1].score - me.score,
      rnk + 1,
      rank[rnk + 1].title
    );
  }

  prf(str);
}
/****************************************************************************
	tell me what my current criminal mission is (if any)
*/
short Current_Mission(void) {
  long    elapsed = time(0) - me.mission_time;
  Clear_Display_Window();

  if (me.mission_type == 0) {
    prfmsg(MSG721);
    EXIT0;
  }

  if (me.mission_type == ERASE_MISSION) {
    prfmsg
    (
      MSG722,
      world->city[me.mission_x][me.mission_y].name,
      me.mission_system
    );
  } else if (me.mission_type == CRASH_MISSION) {
    prfmsg
    (
      MSG723,
      world->city[me.mission_x][me.mission_y].name,
      me.mission_system
    );
  } else if (me.mission_type == RETRIEVE_MISSION) {
    prfmsg
    (
      MSG897,
      world->city[me.mission_x][me.mission_y].name,
      me.mission_system,
      me.mission_doc
    );
  }

  prfmsg
  (
    MSSG728,
    me.mission_score * MISSION_BUX,
    me.mission_score * MISSION_SCORE
  );

  if (elapsed > MISSION_TIME) {
    prfmsg(MSG726);
  } else if (MISSION_TIME - elapsed >= 120) {
    sprintf(str, getmsg(MSG724), (MISSION_TIME - elapsed) / 60);
    prf(str);
  } else {
    sprintf(str, getmsg(MSG725), MISSION_TIME - elapsed);
    prf(str);
  }

  EXIT0;
}
/****************************************************************************
	tell what players and/or programs are in the city I entered
*/
void City_Who(void) {
  short   x, y;
  short   g;
  short   s, c;
  short   guy[MAXGUYS];
  short   visible;
  short   cleared = FALSE;
  struct  cpu_struct *runner;
  str[0] = 0;

  for (g = 0; g != max_guys; ++g) {
    guy[g] = FALSE;

    if (g != myp) {
      if
      (
        Port[g]->active &&
        Port[g]->net_x == MY_PORT->net_x &&
        Port[g]->net_y == MY_PORT->net_y &&
        Port[g]->access_level != IDLE
      ) {
        guy[g] = Visible(myp, g);
      }
    }
  }

  for (y = 0; y != MAX_CITY_Y; ++y) {
    for (x = 0; x != MAX_CITY_X; ++x) {
      runner = Find_Program
               (
                 MY_PORT->net_x, MY_PORT->net_y,
                 x, y,
                 -1, -1,
                 &s, &c
               );

      if (runner != NULL) {
        visible = Visible_Hydra(myp, s, c);

        if (visible) {
          if (str[0]) {
            if (!cleared) {
              Clear_Display_Window();
              cleared = TRUE;
            }

            prfmsg(MSG192, str);
          }

          strcpy(str, THEIR_SOURCE.name);
        }
      } else if (MY_CITY.type[x][y] == 0 || MY_CITY.type[x][y] == PHONE) {
        for (g = 0; g < max_guys; ++g) {
          if
          (
            guy[g] &&
            Port[g]->city_x == x &&
            Port[g]->city_y == y
          ) {
            if (str[0]) {
              if (!cleared) {
                Clear_Display_Window();
                cleared = TRUE;
              }

              prfmsg(MSG192, str);
            }

            strcpy(str, Port[g]->guy.alias);
          }
        }
      }
    }
  }

  if (str[0]) {
    if (!cleared) {
      Clear_Display_Window();
      prfmsg(MSG193, str);
    } else {
      prfmsg(MSSG193, str);
    }
  }
}
/****************************************************************************
	tell what players and/or programs are in the system I entered
*/
void System_Who(void) {
  short   x;
  short   y;
  short   s, c;
  short   g;
  short   visible;
  short   cleared = FALSE;
  struct  cpu_struct *runner;
  str[0] = 0;

  for (y = 0; y != MAX_SYS_Y; ++y) {
    for (x = 0; x != MAX_SYS_X; ++x) {
      runner = Find_Program
               (
                 MY_PORT->net_x, MY_PORT->net_y,
                 MY_PORT->city_x, MY_PORT->city_y,
                 x, y,
                 &s, &c
               );

      if (runner != NULL) {
        visible = Visible_Hydra(myp, s, c);

        if (visible) {
          if (str[0]) {
            if (!cleared) {
              Clear_Display_Window();
              cleared = TRUE;
            }

            prfmsg(MSG192, str);
          }

          strcpy(str, THEIR_SOURCE.name);
        }
      } else if (MY_SYS->node_type[x][y] == A_PLAYER) {
        if (x != MY_PORT->sys_x || y != MY_PORT->sys_y) {
          for (g = 0; g != max_guys; ++g) {
            if
            (
              Port[g]->active &&
              Port[g]->sys_num == MY_PORT->sys_num &&
              Port[g]->sys_x == x &&
              Port[g]->sys_y == y
            ) {
              if (Visible(myp, g)) {
                if (str[0]) {
                  if (!cleared) {
                    Clear_Display_Window();
                    cleared = TRUE;
                  }

                  prfmsg(MSG192, str);
                }

                strcpy(str, Port[g]->guy.alias);
              }
            }
          }
        }
      }
    }
  }

  if (str[0]) {
    if (!cleared) {
      Clear_Display_Window();
      prfmsg(MSG193, str);
    } else {
      prfmsg(MSSG193, str);
    }
  }
}
/****************************************************************************
	clear out the "display" window
*/
void EXPORT Clear_Display_Window(void) {
  short   i;

  for (i = DISP_Y; i != ASYNCH_Y; ++i) {
    prf("\x1B[%d;1H%s", i, CLEAR_RIGHT);
  }

  prf("\x1B[%d;1H", DISP_Y);
}
/****************************************************************************
	display stuff relative to offline services
*/
void Service_Display(void) {
  char    payday[DATE_SIZE];
  prf("\x1B[1;38H");

  if (me.start_x < 0 || me.start_y < 0) {
    prfmsg(MSG194, CLEAR_RIGHT);
  } else {
    prfmsg
    (
      MSG195,
      world->city[me.start_x][me.start_y].name,
      CLEAR_RIGHT
    );
  }

  prf("\x1B[2;38H");
  prfmsg(MSG196, glob->dpts - me.damage, glob->dpts);

  if (me.damage) {
    prfmsg(MSG196A, me.damage * glob->hospcost);
  }

  prf("%s", CLEAR_RIGHT);
  prf("\x1B[3;38H");
  prfmsg(MSG197);
  sprintf(str, getmsg(MSG198), me.bank.balance, CLEAR_RIGHT);
  prf(str);
  prf("\x1B[4;38H");
  prfmsg(MSG199, me.bank.code, CLEAR_RIGHT);
  prf("\x1B[5;38H");
  prfmsg(MSG200, me.internet.code, CLEAR_RIGHT);
  prf("\x1B[6;38H");

  if (me.renting) {
    CONV_DATE(payday, me.rentdue);

    if (me.start_x >= 0 && me.start_y >= 0) {
      prfmsg(MSG201, Rent(), payday, CLEAR_RIGHT);
    } else {
      prfmsg(MSG202, payday, CLEAR_RIGHT);
    }
  } else {
    prfmsg(MSG203, CLEAR_RIGHT);
  }

  prf("\x1B[7;38H");
  prfmsg(MSG204, me.alias, CLEAR_RIGHT);
  prf("\x1B[8;38H%s", CLEAR_RIGHT);
  prf("\x1B[9;38H%s", CLEAR_RIGHT);
  prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
  prf("%s%s", MY_PORT->prompt, CLEAR_RIGHT);
  My_Outprf(MY_PORT->usrnum);
  Reset_Location_Values();
}
/****************************************************************************
	display my bank account ledger
*/
void Bank_Records(void) {
  short   i;
  short   net_x, net_y;
  char    ledger_date[DATE_SIZE];
  char    ledger_time[TIME_SIZE];
  char    city_name[CITY_NAME_SIZE];
  prf("%c%s", CTRL_S, CLEAR_SCREEN);
  prfmsg(MSG205);

  for (i = 0; i != MAX_LEDGER; ++i) {
    if (me.bank.ledger[i].action[0]) {
      CONV_DATE(ledger_date, me.bank.ledger[i].date);
      CONV_TIME(ledger_time, me.bank.ledger[i].time);
      net_x = me.bank.ledger[i].net_x;
      net_y = me.bank.ledger[i].net_y;
      city_name[0] = 0;

      if (net_x >= 0 && net_y >= 0) {
        strcpy
        (
          city_name,
          world->city[net_x][net_y].name
        );
      }

      sprintf
      (
        str,
        getmsg(MSG206),
        ledger_date,
        ledger_time,
        me.bank.ledger[i].amount,
        me.bank.ledger[i].balance,
        city_name,
        me.bank.ledger[i].alias,
        me.bank.ledger[i].action
      );
      prf(str);
    }
  }

  MY_PORT->prompt_y = ASYNCH_Y - 1;
  Prompt(getmsg(MSG207), LEDGER_PROMPT);
}
/****************************************************************************
	display my internet account ledger
*/
void Internet_Records(void) {
  short   i;
  short   net_x, net_y;
  char    ledger_date[DATE_SIZE];
  char    ledger_time[TIME_SIZE];
  char    city_name[CITY_NAME_SIZE];
  prf("%c%s", CTRL_S, CLEAR_SCREEN);
  prfmsg(MSG205);

  for (i = 0; i != MAX_LEDGER; ++i) {
    if (me.internet.ledger[i].action[0]) {
      CONV_DATE(ledger_date, me.internet.ledger[i].date);
      CONV_TIME(ledger_time, me.internet.ledger[i].time);
      net_x = me.internet.ledger[i].net_x;
      net_y = me.internet.ledger[i].net_y;
      city_name[0] = 0;

      if (net_x >= 0 && net_y >= 0) {
        strcpy
        (
          city_name,
          world->city[net_x][net_y].name
        );
      }

      sprintf
      (
        str,
        getmsg(MSG206),
        ledger_date,
        ledger_time,
        me.internet.ledger[i].amount,
        me.internet.ledger[i].balance,
        city_name,
        me.internet.ledger[i].alias,
        me.internet.ledger[i].action
      );
      prf(str);
    }
  }

  MY_PORT->prompt_y = ASYNCH_Y - 1;
  Prompt(getmsg(MSG207), LEDGER_PROMPT);
}
/****************************************************************************
	display my deck's stats
*/
void Display_Deck(void) {
  if (MY_DECK->password[0]) {
    prfmsg(M221, MY_DECK->password);
  } else {
    prfmsg(M221, "<NOT SET>");
  }

  prfmsg(MSG208);

  if (me.interface) {
    prfmsg(MSG209);
  } else {
    prfmsg(MSG210);
  }

  prfmsg(M220, MY_DECK->cpus, MY_DECK->speed);
  prfmsg(MSG222, MY_DECK->mus);
  prfmsg(MSG223, MY_DECK->wall);
  prfmsg(MSG224, MY_DECK->gate);
  prfmsg(MSG225);

  if (me.chip_reader) {
    prfmsg(MSG209);
  } else {
    prfmsg(MSG210);
  }

  prfmsg(MSG226, me.backup_chips);
}
/****************************************************************************
	display my internet/world bank status
*/
void Finances(void) {
  short   rnk = His_Rank(me.score, me.won);
  char    payday[DATE_SIZE];
  prfmsg(MSG227, me.internet.credit_rating);
  sprintf(str, getmsg(MSG228), me.internet.owed);
  prf(str);
  prfmsg(MSG229, me.internet.code);
  prfmsg(MSG230);
  sprintf(str, getmsg(MSG231), me.bank.balance);
  prf(str);
  prfmsg(MSG232, me.bank.code);

  if (me.employed) {
    CONV_DATE(payday, me.payday);
    prfmsg(MSG233, rank[rnk].salary, payday);
  } else {
    prfmsg(MSG234);
  }

  if (me.renting) {
    CONV_DATE(payday, me.rentdue);

    if (me.start_x >= 0 && me.start_y >= 0) {
      prfmsg(MSG235, Rent(), payday);
    } else {
      prfmsg(MSG236, payday);
    }
  } else {
    prfmsg(MSG237);
  }
}
/****************************************************************************
	display current time and time left on
*/
void Display_Time(void) {
  char    today_date[DATE_SIZE];
  char    today_time[TIME_SIZE];
  TODAY(today_date);
  NOW(today_time);
  printf("Today date %s %s\n", today_date, today_time);
  /*
  prfmsg(MSG238, today_date, today_time);

  if (MY_PORT->link_type == STREET_LINK)
  {
  	sprintf(str, getmsg(MSG239), me.internet.time_paid_for / 60);
  	prf("%s", str);
  }*/
}
/****************************************************************************
	prints user options
*/
void List_My_Options(void) {
  OFF_ON(IBM_OPT)
  prfmsg(MSG241);
  OFF_ON(BUSY_OPT)
  prfmsg(MSG632);
  OFF_ON(CHAT_OPT)
  prfmsg(MSG653);
  OFF_ON(AUTOJACK_OPT)
  prfmsg(MSG919);
}
/****************************************************************************
	print who is on
*/
short Where(short option) {
  short   g;
  short   super;
  char    city_name[CITY_NAME_SIZE];
  char    system_name[SYSTEM_NAME_SIZE];
  char    outlaw;
  char    gang_num[3];
  short   l = DISP_Y;
  Clear_Display_Window();
  prfmsg(M242);
  ++l;

  if (option == 0) {
    MY_PORT->misc = 0;
  }

  for (g = MY_PORT->misc; g != max_guys; ++g) {
    if (Port[g]->active) {
      if (l == ASYNCH_Y) {
        MY_PORT->misc = g;
        Prompt(getmsg(MSG243), WHERE_PROMPT);
        EXIT0;
      }

      super = FALSE;

      if (Port[g]->guy.outlaw && glob->outlaw) {
        outlaw = '*';
      } else {
        outlaw = ' ';
      }

      if (Port[g]->guy.gang) {
        sprintf(gang_num, "%d", Port[g]->guy.gang);
      } else {
        gang_num[0] = 0;
      }

      if (g == myp || me.perm[GAMEOP_PERM]) {
        super = TRUE;
      } else if (Port[g]->access_level != IDLE) {
        if (Traced(g)) {
          super = TRUE;
        }
      }

      strcpy(city_name, "None");
      strcpy(system_name, "None");

      if (Port[g]->link_type != IDLE) {
        if (super) {
          strcpy(city_name, HIS_CITY.name);
        } else {
          strcpy(city_name, "Unknown");
        }

        if (Port[g]->access_level == SYSTEM_VIEW) {
          if (super) {
            strcpy(system_name, Sys[HIS]->Name);
          } else {
            strcpy(system_name, "Unknown");
          }
        }
      }

      if (!Port[g]->invisible) {
        prfmsg
        (
          M244,
          g + 1,
          Brief_Userid(Port[g]->guy.userid),
          city_name,
          system_name,
          His_Rank(Port[g]->guy.score, Port[g]->guy.won),
          gang_num,
          outlaw,
          Port[g]->guy.alias
        );
      } else if (g == myp || me.perm[GAMEOP_PERM]) {
        prfmsg
        (
          M244I,
          g + 1,
          Brief_Userid(Port[g]->guy.userid),
          city_name,
          system_name,
          His_Rank(Port[g]->guy.score, Port[g]->guy.won),
          gang_num,
          outlaw,
          Port[g]->guy.alias
        );
      }

      ++l;
    }
  }

  STATE = MAIN_PROMPT;
  EXIT0;
}
/****************************************************************************
	prints users limits
*/
short List_My_Perms(void) {
  ON_OFF(SET_PERM)
  prfmsg(MSG245);

  if (!me.perm[SET_PERM]) {
    EXIT0;
  }

  ON_OFF(GAMEOP_PERM)
  prfmsg(MSG246);
  ON_OFF(MONITOR_PERM)
  prfmsg(MSG247);
  ON_OFF(FREE_PERM)
  prfmsg(MSG248);
  ON_OFF(CREATE_PERM)
  prfmsg(MSG249);
  ON_OFF(EDIT_USER_PERM)
  prfmsg(MSG250);
  ON_OFF(EDIT_WORLD_PERM)
  prfmsg(MSG251);
  EXIT0;
}
/****************************************************************************
	update location
*/
void Update_Location(void) {
  short   i;
  short   online;
  short   moves;
  short   rank;
  short   reprompt = FALSE;

  if (MY_PORT->link_type != IDLE) {
    if
    (
      MY_PORT->old_net_x != MY_PORT->net_x ||
      MY_PORT->old_net_y != MY_PORT->net_y
    ) {
      reprompt = TRUE;
      MY_PORT->old_net_x = MY_PORT->net_x;
      MY_PORT->old_net_y = MY_PORT->net_y;
      prf("\x1B[1;38H");
      prfmsg
      (
        MSG253,
        MY_PORT->net_x,
        MY_PORT->net_y,
        MY_CITY.name,
        CLEAR_RIGHT
      );
    }
  } else if (me.start_x < 0 || me.start_y < 0) {
    prf("\x1B[1;38H");
    prfmsg(MSG254, CLEAR_RIGHT);
    reprompt = TRUE;
    MY_PORT->old_net_x = -2;
    MY_PORT->old_net_y = -2;
  } else {
    reprompt = TRUE;
    MY_PORT->old_net_x = -2;
    MY_PORT->old_net_y = -2;
    prf("\x1B[1;38H");
    prfmsg
    (
      MSG255,
      world->city[me.start_x][me.start_y].name,
      CLEAR_RIGHT
    );
  }

  if (MY_PORT->access_level != IDLE) {
    if
    (
      MY_PORT->old_city_x != MY_PORT->city_x ||
      MY_PORT->old_city_y != MY_PORT->city_y
    ) {
      reprompt = TRUE;
      MY_PORT->old_city_x = MY_PORT->city_x;
      MY_PORT->old_city_y = MY_PORT->city_y;

      if (MY_PORT->access_level == SYSTEM_VIEW) {
        prf("\x1B[2;38H");
        prfmsg(MSG256, MY_SYS->Name, CLEAR_RIGHT);
      } else {
        prf("\x1B[2;38H");
        prfmsg(MSG257, CLEAR_RIGHT);
      }
    }
  } else {
    prf("\x1B[2;38H");
    prfmsg(MSG257, CLEAR_RIGHT);
    reprompt = TRUE;
    MY_PORT->old_city_x = -2;
    MY_PORT->old_city_y = -2;
  }

  moves = MY_PORT->walks[MY_PORT->ld_moves] - 1;

  if (moves < 0) {
    moves = 0;
  }

  if (MY_PORT->old_walks != moves) {
    reprompt = TRUE;
    MY_PORT->old_walks = moves;
    prf("\x1B[3;38H");
    prfmsg(MSSG261, moves, MAX_MOVE - 1, CLEAR_RIGHT);
  }

  moves = MY_PORT->ld_moves;

  if (moves < 0) {
    moves = 0;
  }

  if (MY_PORT->old_ld_moves != moves) {
    reprompt = TRUE;
    MY_PORT->old_ld_moves = moves;
    prf("\x1B[4;38H");
    prfmsg(MSSG262, moves, MAX_LD_LINKS - 1, CLEAR_RIGHT);
  }

  if (MY_PORT->old_damage != me.damage) {
    reprompt = TRUE;
    MY_PORT->old_damage = me.damage;
    prf("\x1B[5;38H");
    prfmsg(MSG263, glob->dpts - me.damage, glob->dpts, CLEAR_RIGHT);
  }

  if (MY_PORT->old_score != me.score) {
    reprompt = TRUE;
    MY_PORT->old_score = me.score;
    prf("\x1B[6;38H");
    rank = His_Rank(me.score, me.won);
    sprintf
    (
      str,
      getmsg(M264A),
      me.score,
      rank,
      CLEAR_RIGHT
    );
    prf(str);

    if (rank >= 2) {
      me.newbie = FALSE;
    }
  }

  if (MY_PORT->access_level == IDLE) {
    if (MY_PORT->old_rez != -1000) {
      reprompt = TRUE;
      MY_PORT->old_rez = -1000;
      prf("\x1B[7;38H");
      prfmsg(MSG265, CLEAR_RIGHT);
    }
  } else {
    if (MY_PORT->old_rez != MY_PORT->rez) {
      reprompt = TRUE;
      MY_PORT->old_rez = MY_PORT->rez;
      prf("\x1B[7;38H");
      prfmsg(MSG266, MY_PORT->rez, CLEAR_RIGHT);
    }
  }

  for (i = online = 0; i != max_guys; ++i) {
    if (Port[i]->active) {
      if (!Port[i]->invisible) {
        ++online;
      } else if (i == myp || me.perm[GAMEOP_PERM]) {
        ++online;
      }
    }
  }

  if (MY_PORT->old_online != online) {
    MY_PORT->old_online = online;
    reprompt = TRUE;
    prf("\x1B[8;38H");
    prfmsg(MSG712, (int) online, CLEAR_RIGHT);
  }

  if (me.gang && MY_PORT->old_gang_score != me.gang_pts) {
    reprompt = TRUE;
    MY_PORT->old_gang_score = me.gang_pts;
    prf("\x1B[9;38H");
    sprintf
    (
      str,
      getmsg(M264B),
      me.gang_pts,
      me.gang_pts_today,
      CLEAR_RIGHT
    );
    prf(str);
  }

  if (reprompt) {
    prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
    prf("%s%s", MY_PORT->prompt, CLEAR_RIGHT);
    My_Outprf(MY_PORT->usrnum);
  }
}
/****************************************************************************
	show where I am
*/
short EXPORT Look(char *option) {
  if (MY_PORT->access_level == IDLE) {
    if (me.renting) {
      prfmsg(MSG267);
    } else if (me.start_x >= 0 && me.start_y >= 0) {
      prfmsg(MSG268, world->city[me.start_x][me.start_y].name);
    }

    prfmsg(MSG269);
    EXIT0;
  }

  if (MY_PORT->access_level == CITY_VIEW) {
    if (sameas(option, "g")) {
      World_View();
      EXIT0;
    }

    City_View(-1, -1);

    if (command == LOOK) {
      City_Who();
      command = 0;
    }
  } else {
    if (sameas(option, "g")) {
      World_View();
      EXIT0;
    }

    if (sameas(option, "c")) {
      City_View(-1, -1);

      if (command == LOOK) {
        City_Who();
        command = 0;
      }

      EXIT0;
    }

    System_View(-1, -1);

    if (command == LOOK) {
      System_Who();
      command = 0;
    }
  }

  EXIT0;
}
