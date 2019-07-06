#include <header.h>
#include <globals.h>

/****************************************************************************
	user wants to make a retrograde movement
*/
short EXPORT Back_Up(void) {
  short   ldm = MY_PORT->ld_moves;
  short   city_x, city_y;
  short   net_x, net_y;
  short   sys_x = -1;
  short   sys_y = -1;
  short   old_nx = MY_PORT->net_x;
  short   old_ny = MY_PORT->net_y;
  short   old_cx = MY_PORT->city_x;
  short   old_cy = MY_PORT->city_y;
  short   old_sx = MY_PORT->sys_x;
  short   old_sy = MY_PORT->sys_y;
  short   old_access = MY_PORT->access_level;
  short   g;
  short   done;
  short   s, c;
  short   phone = TRUE;
  short   x, y;
  struct  cpu_struct *runner;

  if (MY_PORT->access_level == IDLE) {
    prfmsg(MSG276);
    EXIT0;
  }

  if (MY_PORT->walks[ldm] > 1) {
    net_x = MY_PORT->walked[ldm][MY_PORT->walks[ldm] - 2].x;
    net_y = MY_PORT->walked[ldm][MY_PORT->walks[ldm] - 2].y;
    city_x = MY_PORT->city_x;
    city_y = MY_PORT->city_y;

    do {
      done = TRUE;

      if (world->city[net_x][net_y].type[city_x][city_y]) {
        done = FALSE;
      } else {
        for (g = 0; g != max_guys; ++g) {
          if
          (
            g != myp &&
            Port[g]->active &&
            Port[g]->access_level == CITY_VIEW &&
            Port[g]->net_x == net_x &&
            Port[g]->net_y == net_y &&
            Port[g]->city_x == city_x &&
            Port[g]->city_y == city_y
          ) {
            done = FALSE;
          }
        }

        if (done) {
          runner = Find_Program
                   (
                     net_x, net_y,
                     city_x, city_y,
                     -1, -1,
                     &s, &c
                   );

          if (runner != NULL) {
            done = FALSE;
          }
        }
      }

      if (!done) {
        city_x = RANDOM(MAX_CITY_X) - 1;
        city_y = RANDOM(MAX_CITY_Y) - 1;
      }
    } while (!done);
  } else if (MY_PORT->ld_moves > 0) {
    net_x = MY_PORT->ld_path[MY_PORT->ld_moves - 1].net_x;
    net_y = MY_PORT->ld_path[MY_PORT->ld_moves - 1].net_y;
    city_x = MY_PORT->ld_path[MY_PORT->ld_moves - 1].city_x;
    city_y = MY_PORT->ld_path[MY_PORT->ld_moves - 1].city_y;
    sys_x = MY_PORT->ld_path[MY_PORT->ld_moves - 1].sys_x;
    sys_y = MY_PORT->ld_path[MY_PORT->ld_moves - 1].sys_y;
  } else if (MY_PORT->access_level == SYSTEM_VIEW && MY_PORT->sys_num == myp) {
    prfmsg(MSG680);
    EXIT0;
  } else {
    phone  = FALSE;
    net_x  = MY_DECK->net_x;
    net_y  = MY_DECK->net_y;
    city_x = MY_DECK->city_x;
    city_y = MY_DECK->city_y;
    sys_y  = START_SYS_Y;
    sys_x  = START_SYS_X;
    runner = Find_Program
             (
               net_x, net_y,
               city_x, city_y,
               sys_x, sys_y,
               &s, &c
             );

    if (runner != NULL || MY_DECK->node_type[sys_x][sys_y]) {
      for (x = done = 0; x != MAX_SYS_X && !done; ++x) {
        for (y = 0; y != MAX_SYS_Y && !done; ++y) {
          runner = Find_Program
                   (
                     net_x, net_y,
                     city_x, city_y,
                     x, y,
                     &s, &c
                   );

          if (runner == NULL && MY_DECK->node_type[x][y] == 0) {
            sys_x = x;
            sys_y = y;
            done = TRUE;
          }
        }
      }
    }
  }

  for (g = 0; g != max_guys; ++g) {
    if (sys_x == -1 || sys_y == -1) {
      if
      (
        g != myp &&
        Port[g]->active &&
        Port[g]->access_level == CITY_VIEW &&
        Port[g]->net_x == net_x &&
        Port[g]->net_y == net_y &&
        Port[g]->city_x == city_x &&
        Port[g]->city_y == city_y
      ) {
        prfmsg(MSG414, Port[g]->guy.alias);
        EXIT0;
      }
    } else {
      if
      (
        g != myp &&
        Port[g]->active &&
        Port[g]->access_level == SYSTEM_VIEW &&
        Port[g]->net_x == net_x &&
        Port[g]->net_y == net_y &&
        Port[g]->city_x == city_x &&
        Port[g]->city_y == city_y &&
        Port[g]->sys_x == sys_x &&
        Port[g]->sys_y == sys_y
      ) {
        prfmsg(MSG414, Port[g]->guy.alias);
        EXIT0;
      }
    }

    runner = Find_Program
             (
               net_x, net_y,
               city_x, city_y,
               -1, -1,
               &s, &c
             );

    if (runner != NULL) {
      prfmsg(MSG414, THEIR_SOURCE.name);
      EXIT0;
    }
  }

  if (MY_PORT->walks[ldm] > 1) {
    --MY_PORT->walks[ldm];
  } else if (MY_PORT->ld_moves > 0) {
    --MY_PORT->ld_moves;
  }

  if (sys_x != -1 && sys_y != -1) {
    MY_PORT->back_x = sys_x;
    MY_PORT->back_y = sys_y;
  } else {
    MY_PORT->back_x = -1;
    MY_PORT->back_y = -1;
  }

  Move_Me_To(net_x, net_y, city_x, city_y, !JACK, BACK);
  Update_Location();

  if (old_access == SYSTEM_VIEW) {
    Quick_Update
    (
      old_access,
      old_nx, old_ny,
      old_cx, old_cy,
      old_sx, old_sy,
      0, 0
    );
  } else {
    Quick_Update
    (
      old_access,
      old_nx, old_ny,
      old_cx, old_cy,
      old_cx, old_cy,
      0, 0
    );
  }

  if (sys_x != -1 && sys_y != -1 && phone) {
    MY_PORT->px = sys_x;
    MY_PORT->py = sys_y;
  }

  EXIT0;
}
/****************************************************************************
	user wants to enter Cyberspace via a street link
*/
short Street_Jack_In(void) {
  short   i;

  if (STATE == MAIN_PROMPT || STATE == JACKIN_PROMPT) {
    if (me.perm[FREE_PERM]) {
      Prompt(getmsg(MSG592), STREET_PROMPT);
      EXIT0;
    }

    Prompt(getmsg(MSSG593), STREET_CODE_PROMPT);
    EXIT0;
  }

  i = Find_Bank_User(a_human, margv[0]);

  if (i < 0) {
    prfmsg(MSG179);
    STATE = MAIN_PROMPT;
    EXIT0;
  }

  if
  (
    i != me.index &&
    !sameto(A_GANG, a_human->alias) &&
    (
      His_Rank(me.score, me.won) < glob->pvprank ||
      His_Rank(a_human->score, a_human->won) < glob->pvprank
    )
  ) {
    prfmsg(MSG807);
    EXIT0;
  }

  if
  (
    i != me.index &&
    glob->outlaw &&
    (!me.outlaw || !a_human->outlaw)
  ) {
    prfmsg(MSG807);
    EXIT0;
  }

  if (a_human->bank.balance == 0) {
    prfmsg(MSG594);
    STATE = MAIN_PROMPT;
    EXIT0;
  }

  strcpy(str, margv[0]);
  str[CODE_SIZE - 1] = 0;
  strcpy(MY_PORT->code, str);
  sprintf(str, getmsg(MSG595), a_human->bank.balance);
  Prompt(str, STREET_PROMPT);
  EXIT0;
}
/****************************************************************************
	user wants to enter Cyberspace
*/
short Jack_In(void) {
  short   i;
  short   cpus;
  printf("Jacking in with playkey = %d, haskey = %d\n", glob->playkey[0], haskey(glob->playkey));

  if (glob->playkey[0] && !haskey(glob->playkey)) {
    MY_PORT->autojack = FALSE;
    Send(getmsg(MSG596), myp, TRUE);
    EXIT0;
  }

  if (!me.perm[GAMEOP_PERM] && glob->jacks && me.jacks >= glob->jacks) {
    Send(getmsg(MSG873), myp, TRUE);
    MY_PORT->autojack = FALSE;
    EXIT0;
  }

  if (!Phase_Check(1)) {
    MY_PORT->autojack = FALSE;
    EXIT0;
  }

  if (me.damage >= glob->dpts) {
    i = His_Rank(me.score, me.won);

    if (i > 1) {
      me.score = rank[i - 1].score;
      me.damage = 0;
      me.won = FALSE;
      Update_Location();
    }
  }

  if (me.damage >= glob->dpts) {
    MY_PORT->autojack = FALSE;
    Send(getmsg(MSG597), myp, TRUE);
    EXIT0;
  }

  if (MY_DECK->cpus == 0) {
    MY_PORT->autojack = FALSE;
    Send(getmsg(MSG599), myp, TRUE);
    EXIT0;
  }

  if (MY_DECK->mus == 0) {
    MY_PORT->autojack = FALSE;
    Send(getmsg(MSG600), myp, TRUE);
    EXIT0;
  }

  if (me.start_x < 0 || me.start_y < 0) {
    Send(getmsg(MSG601), myp, TRUE);
    Relocate(0);
    EXIT0;
  }

  for (i = cpus = 0; i != MY_DECK->cpus; ++i) {
    if (!MY_DECK->crash_time[i]) {
      ++cpus;
    }
  }

  if (cpus == 0) {
    MY_PORT->autojack = FALSE;
    Send(getmsg(MSG602), myp, TRUE);
    EXIT0;
  }

  if (!me.interface) {
    MY_PORT->autojack = FALSE;
    Send(getmsg(MSG603), myp, TRUE);
    EXIT0;
  }

  if (me.renting) {
    if (MY_PORT->autojack) {
      if (me.internet.credit_rating == 0) {
        MY_PORT->autojack = FALSE;
        Send(getmsg(MSG144), myp, TRUE);
        EXIT0;
      }

      MY_PORT->link_type = INTERNET_LINK;
      MY_PORT->ld_time = 0;
      Jacking_In();
      MY_PORT->autojack = FALSE;
      EXIT0;
    }

    Prompt(getmsg(MSG604), JACKIN_PROMPT);
  } else {
    if (MY_PORT->autojack) {
      MY_PORT->autojack = FALSE;
      EXIT0;
    }

    Street_Jack_In();
  }

  EXIT0;
}
/****************************************************************************
	user wants to enter Cyberspace
*/
short Jacking_In(void) {
  short   i;
  short   s, c;
  short   minutes;
  short   g;
  short   x, y;
  short   done;
  struct  cpu_struct *runner;

  if (STATE == STREET_PROMPT) {
    minutes = INUM(margv[0]);

    if (minutes <= 0) {
      STATE = MAIN_PROMPT;
      EXIT0;
    }

    if (!me.perm[FREE_PERM]) {
      g = Find_Bank_User(a_human, MY_PORT->code);

      if (g < 0) {
        prfmsg(MSG179);
        STATE = MAIN_PROMPT;
        EXIT0;
      }

      if (minutes > a_human->bank.balance) {
        STATE = MAIN_PROMPT;
        prfmsg(MSG605);
        EXIT0;
      }

      prfmsg(MSG606, minutes);
      Add_Bank_Record
      (
        a_human,
        -((long) minutes),
        me.start_x, me.start_y,
        me.alias,
        "Street Link"
      );
    }

    MY_PORT->link_type = STREET_LINK;
    me.internet.time_paid_for = minutes * 60;
    me.internet.warn1 = 0;
    me.internet.warn5 = 0;
  }

  MY_PORT->ld_time = 0;
  MY_PORT->walks[MY_PORT->ld_moves] = 1;
  MY_PORT->walked[MY_PORT->ld_moves][0].x = me.start_x;
  MY_PORT->walked[MY_PORT->ld_moves][0].y = me.start_y;
  MY_DECK->in_use = TRUE;
  MY_PORT->icebolt_time = 0;
  MY_PORT->handcuff_time = 0;
  MY_PORT->net_x = me.start_x;
  MY_PORT->net_y = me.start_y;
  MY_DECK->net_x = me.start_x;
  MY_DECK->net_y = me.start_y;
  MY_PORT->sys_x = -1;
  MY_PORT->sys_y = -1;
  MY_PORT->tstring[0] = 0;
  MY_PORT->rez = 100;

  do {
    do {
      x = RANDOM(MAX_CITY_X - 2);
      y = RANDOM(MAX_CITY_Y - 2);
    } while (MY_CITY.type[x][y] != 0);

    done = TRUE;
    runner = Find_Program
             (
               MY_PORT->net_x, MY_PORT->net_y,
               x, y,
               -1, -1,
               &s, &c
             );

    if (runner != NULL) {
      done = FALSE;
    } else {
      for (g = 0; g != max_guys; ++g) {
        if
        (
          Port[g]->active &&
          Port[g]->net_x == MY_PORT->net_x &&
          Port[g]->net_y == MY_PORT->net_y &&
          Port[g]->city_x == x &&
          Port[g]->city_y == y
        ) {
          done = FALSE;
        } else if
        (
          Port[g]->active &&
          Sys[g]->net_x == MY_PORT->net_x &&
          Sys[g]->net_y == MY_PORT->net_y &&
          Sys[g]->city_x == x &&
          Sys[g]->city_y == y
        ) {
          done = FALSE;
        }
      }
    }
  } while (!done);

  MY_PORT->city_x = x;
  MY_PORT->city_y = y;
  MY_DECK->city_x = x;
  MY_DECK->city_y = y;
  MY_CITY.type[MY_PORT->city_x][MY_PORT->city_y] = CYBERDECK;
  MY_MISC_CITY.camoflaged[MY_PORT->city_x][MY_PORT->city_y] = FALSE;
  sprintf(str, glob->m35, MY_DECK->Name);
  City_Message(str, MY_PORT->net_x, MY_PORT->net_y, myp);
  Quick_Update
  (
    CITY_VIEW,
    MY_PORT->net_x, MY_PORT->net_y,
    MY_PORT->city_x, MY_PORT->city_y,
    MY_PORT->city_x, MY_PORT->city_y,
    0, 0
  );
  Move_To_System
  (
    MY_PORT->net_x,
    MY_PORT->net_y,
    MY_PORT->city_x,
    MY_PORT->city_y
  );
  MY_PORT->sys_y = START_SYS_Y;
  MY_PORT->sys_x = START_SYS_X;
  MY_SYS->node_type[MY_PORT->sys_x][MY_PORT->sys_y] = A_PLAYER;
  System_View(-1, -1);
  STATE = MAIN_PROMPT;
  ++me.jacks;
  Write_My_Rec(EXISTS);

  if (glob->jacks && !me.perm[GAMEOP_PERM]) {
    prfmsg(MSG874, glob->jacks - me.jacks);
  }

  for (i = 0; i != max_guys; ++i) {
    MY_PORT->enemy[i] = -1;
    Port[i]->enemy[myp] = -1;
  }

  EXIT0;
}
/****************************************************************************
	user's position was forcibly changed, clean up some stuff
*/
void Force_Context(void) {
  if (!MY_PORT->hung_up) {
#ifdef MBBS
    user[MY_PORT->usrnum].substt = MAIN_PROMPT;
#else
    usroff(MY_PORT->usrnum)->substt = MAIN_PROMPT;
#endif
    strcpy(MY_PORT->prompt, glob->main_prompt);
    strcpy(glob->buf, MY_PORT->prompt);
    stpans(glob->buf);
    MY_PORT->prompt_len = strlen(glob->buf);
    MY_PORT->unlocking = FALSE;
  }
}
/****************************************************************************
	user wants to exit Cyberspace
*/
short Jack_Out(void) {
  short   old_nx;
  short   old_ny;
  short   old_cx;
  short   old_cy;
  short   old_access = MY_PORT->access_level;
  MY_PORT->icebolt_time = 0;
  MY_PORT->handcuff_time = 0;
  Force_Context();

  if (MY_PORT->access_level == IDLE) {
    EXIT0;
  }

  Stop_Programs_In(myp);
  Boot_Users_From(myp);
  old_nx = MY_PORT->net_x;
  old_ny = MY_PORT->net_y;
  old_cx = MY_PORT->city_x;
  old_cy = MY_PORT->city_y;
  Move_Me_To(0, 0, 0, 0, JACK, !BACK);
  MY_PORT->link_type = IDLE;
  MY_PORT->ld_moves = 0;
  MY_PORT->walks[MY_PORT->ld_moves] = 0;
  MY_PORT->ld_time = 0;
  me.internet.time_paid_for = 0;
  DECK_CITY.type[MY_DECK->city_x][MY_DECK->city_y] = 0;
  DECK_MISC_CITY.camoflaged[MY_DECK->city_x][MY_DECK->city_y] = FALSE;
  My_Outprf(MY_PORT->usrnum);
  Stop_All_Programs(myp);
  sprintf(str, glob->m22, MY_DECK->Name);
  City_Message(str, MY_DECK->net_x, MY_DECK->net_y, myp);
  MY_PORT->access_level = IDLE;
  MY_DECK->in_use = FALSE;
  Quick_Update
  (
    CITY_VIEW,
    MY_DECK->net_x, MY_DECK->net_y,
    MY_DECK->city_x, MY_DECK->city_y,
    MY_DECK->city_x, MY_DECK->city_y,
    0, 0
  );
  Clear_Net_Display(CLEAR_ALL);
  prfmsg(MSG607);
  Update_Location();
  My_Outprf(MY_PORT->usrnum);

  if
  (
    old_access == CITY_VIEW &&
    (
      MY_DECK->net_x != old_nx ||
      MY_DECK->net_y != old_ny ||
      MY_DECK->city_x != old_cx ||
      MY_DECK->city_y != old_cy
    )
  ) {
    Quick_Update
    (
      CITY_VIEW,
      old_nx, old_ny,
      old_cx, old_cy,
      old_cx, old_cy,
      0, 0
    );
  }

  EXIT0;
}
/****************************************************************************
	stop all my programs
*/
void Stop_All_Programs(short s) {
  short   i;

  for (i = 0; i != MAX_CPUS + 1; ++i) {
    if (Proc[s]->cpu[i].m_index >= 0) {
      Program_Stops(s, i);
    }
  }
}
/****************************************************************************
	punt users from a system that's about to leave the net
*/
void Boot_Users_From(short s) {
  short   g;
  short   done;
  short   x, y;
  short   dummy_s, dummy_c;
  short   old_port = myp;
  struct  cpu_struct *runner;

  for (myp = 0; myp != max_guys; ++myp) {
    if
    (
      MY_PORT->active &&
      MY_PORT->sys_num == s &&
      MY_PORT->access_level != IDLE &&
      (myp != old_port || s >= max_guys)
    ) {
      do {
        done = TRUE;

        do {
          x = RANDOM(MAX_CITY_X) - 1;
          y = RANDOM(MAX_CITY_Y) - 1;
        } while (MY_CITY.type[x][y] != 0);

        runner = Find_Program
                 (
                   MY_PORT->net_x, MY_PORT->net_y,
                   x, y,
                   -1, -1,
                   &dummy_s, &dummy_c
                 );

        if (runner != NULL) {
          done = FALSE;
        } else {
          for (g = 0; g != max_guys; ++g) {
            if
            (
              Port[g]->active &&
              Port[g]->access_level != IDLE &&
              Port[g]->net_x == MY_PORT->net_x &&
              Port[g]->net_y == MY_PORT->net_y &&
              Port[g]->city_x == x &&
              Port[g]->city_y == y
            ) {
              done = FALSE;
            }
          }
        }
      } while (!done);

      Move_Me_To
      (
        MY_PORT->net_x, MY_PORT->net_y,
        x, y,
        !JACK, !BACK
      );
      Force_Context();
      My_Outprf(MY_PORT->usrnum);
    }
  }

  myp = old_port;
}
/****************************************************************************
	stop programs running in a system thats about to leave the net
*/
void EXPORT Stop_Programs_In(short g) {
  short   s;
  short   i;

  for (s = 0; s != max_systems; ++s) {
    if (Sys[s]->in_use) {
      for (i = 0; i != Sys[s]->cpus; ++i) {
        if
        (
          SYS_PROGRAM.m_index >= 0 &&
          SYS_PROGRAM.net_x == Sys[g]->net_x &&
          SYS_PROGRAM.net_y == Sys[g]->net_y &&
          SYS_PROGRAM.city_x == Sys[g]->city_x &&
          SYS_PROGRAM.city_y == Sys[g]->city_y
        ) {
          Program_Stops(s, i);
        }
      }
    }
  }
}
