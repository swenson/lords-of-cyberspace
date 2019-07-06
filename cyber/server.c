#include <header.h>
#include <globals.h>

/****************************************************************************/
void EXPORT Cyber_Server(void) {
  short   x, y;
  short   i;
  short   s;
  short   busy = FALSE;
  long    cur_tick;
  cur_tick = (long) hrtval();

  if (cur_tick - glob->last_tick < 10000) {
    busy = TRUE;
  }

  if (glob->reset) {
    Port_Check();
    setmbk(glob->cfg);
    glob->server = TRUE;
    Reset_Game();
    glob->server = FALSE;
    rstmbk();
  } else if (!busy) {
    setmbk(glob->cfg);
    glob->server = TRUE;
    Port_Check();
    Dead_Timers();
    Aliens();
    Cyberia();

    for (s = 0; s != max_systems; ++s) {
      glob->order[s] = s;
    }

    for (s = 0; s != max_systems; ++s) {
      x = RANDOM(max_systems) - 1;
      y = RANDOM(max_systems) - 1;
      i = glob->order[x];
      glob->order[x] = glob->order[y];
      glob->order[y] = i;
    }

    for (i = 0; i != max_systems; ++i) {
      s = glob->order[i];

      if (Sys[s]->in_use || (s < max_guys && Port[s]->active)) {
        Sys_Timers(s);

        if (Sys[s]->in_use) {
          Close_System_Check(s, 0);

          if (s >= max_guys) {
            Crash_Check(s, &x);
          }
        }
      }
    }

    for (s = max_systems - CONS; s != max_systems; ++s) {
      Con_Actions(s);
    }

    Cop_Actions();
    Cop_Scan();
    Server_Update();
    rstmbk();
    glob->server = FALSE;
  }

  rtkick(1, Cyber_Server);
  glob->last_tick = (long) hrtval();
}
/****************************************************************************
	check ports to see if anyone has fallen asleep
*/
void EXPORT Port_Check(void) {
  short   p;
  short   m;
  short   i;
  long    time_left;
  long    dmg;

  for (p = 0; p != max_guys; ++p) {
    myp = p;

    if (MY_PORT->active) {
      MY_PORT->redraw = FALSE;
      ++MY_PORT->secs_on;

      if (me.arrest_time) {
        --me.arrest_time;
        time_left = me.arrest_time;

        if (time_left <= 0) {
          strcpy(str, getmsg(MSG12));
          Send(str, myp, FALSE);
          Jack_Out();
          i = RANDOM(3);

          if (i == 1 || (me.damage == glob->dpts && me.score == 0)) {
            strcpy(str, getmsg(MSG13));
            Send(str, myp, FALSE);
            Destroy_My_Deck(TRUE);
          } else if (i == 2 || me.score == 0) {
            i = RANDOM(glob->dpts - me.damage);
            me.damage += i;
            sprintf(str, getmsg(MSG14), i);
            Send(str, myp, FALSE);

            if (me.damage >= glob->dpts) {
              strcpy(str, getmsg(MSG15));
              Send(str, myp, FALSE);
            }

            Update_Location();
          } else {
            dmg = LRAND(me.score);
            me.score -= dmg;
            sprintf(str, getmsg(MSSG16), dmg);
            Send(str, myp, FALSE);
            Update_Location();
          }
        } else if (time_left < 30) {
          if (!me.arrest_warn30) {
            strcpy(str, getmsg(MSG17));
            Send(str, myp, FALSE);
            me.arrest_warn30 = TRUE;
          }
        } else if (time_left < 90) {
          if (!me.arrest_warn90) {
            strcpy(str, getmsg(MSG18));
            Send(str, myp, FALSE);
            me.arrest_warn90 = TRUE;
          }
        }
      }

      if (time(0) - me.last_net_crime > glob->copmins1 * 60) {
        me.wanted = FALSE;
      }

      if (MY_PORT->icebolt_time) {
        --MY_PORT->icebolt_time;

        if (MY_PORT->icebolt_time == 0) {
          strcpy(str, getmsg(MSG19));
          Send(str, myp, FALSE);
        }
      }

      if (MY_PORT->handcuff_time) {
        --MY_PORT->handcuff_time;

        if (MY_PORT->handcuff_time == 0) {
          strcpy(str, getmsg(MSG20));
          Send(str, myp, FALSE);
        }
      }

      for (m = 0; m != MY_DECK->mus; ++m) {
        for (i = 0; i != MU_CAPACITY; ++i) {
          MY_DECK->memory[m][i].running = FALSE;
        }
      }

      for (i = 0; i != MY_DECK->cpus; ++i) {
        if (MY_PROGRAM.m_index >= 0) {
          MY_SOURCE.running = TRUE;
        }
      }

      if (MY_PORT->access_level != IDLE) {
        if (MY_PORT->link_type == STREET_LINK) {
          --me.internet.time_paid_for;
          time_left = me.internet.time_paid_for;

          if (time_left <= 0) {
            Jack_Out();
            strcpy(str, getmsg(MSG23));
            Send(str, myp, FALSE);
          } else if (time_left < 60) {
            if (!me.internet.warn1) {
              strcpy(str, getmsg(MSG24));
              Send(str, myp, FALSE);
              me.internet.warn1 = TRUE;
            }
          } else if (time_left < 60 * 5) {
            if (!me.internet.warn5) {
              strcpy(str, getmsg(MSG25));
              Send(str, myp, FALSE);
              me.internet.warn5 = TRUE;
            }
          }
        }
      }
    }
  }

  for (myp = 0; myp != max_guys; ++myp) {
    if (MY_PORT->active && MY_PORT->hung_up) {
      if (MY_PORT->access_level != IDLE) {
        if (me.perm[GAMEOP_PERM] || glob->shutdown || glob->reset) {
          Jack_Out();
        } else if
        (
          MY_PORT->icebolt_time == 0 &&
          MY_PORT->handcuff_time == 0
        ) {
          if (MY_PORT->sys_num == myp || MY_PORT->hup_timer == 0) {
            Jack_Out();
          } else {
            if (RANDOM(glob->jackroll) == 1) {
              MY_PORT->handcuff_time = glob->jacksecs;
            } else {
              Jack_Out();
            }
          }
        }
      }

      if (MY_PORT->hup_timer > 0) {
        --MY_PORT->hup_timer;
      }

      if (MY_PORT->access_level == IDLE || glob->reset) {
        Day_Out();
        Stop_All_Programs(myp);

        if (me.gang && MY_PORT->gang_pts) {
          Read_Gang(me.gang);
          gang->pts += MY_PORT->gang_pts;
          gang->pts_today += MY_PORT->gang_pts;
          Write_Gang(me.gang);
        }

        Write_My_Rec(EXISTS);
        MY_PORT->active = FALSE;

        if (!MY_PORT->invisible) {
          sprintf
          (
            str,
            getmsg(MSG21),
            myp + 1,
            MY_PORT->guy.userid,
            MY_PORT->guy.alias
          );
          Global_Message(str, myp);
        }
      }
    }
  }
}
/****************************************************************************
	advance crashed system timers.
*/
void EXPORT Dead_Timers(void) {
  short   s;
  short   c;
  short   g;
  short   p;
  short   cop = glob->cop_sys;
  FILE    *fp;
  struct  cpu_struct *runner;

  for (p = 0; p != MAX_CRASHED; ++p) {
    if (dead->sys[p].type) {
      --dead->sys[p].timer;

      if (dead->sys[p].type == COP_SYS && dead->sys[p].timer == 0) {
        dead->sys[p].type = 0;
        Init_Cops();
        Quick_Update
        (
          CITY_VIEW,
          Sys[cop]->net_x, Sys[cop]->net_y,
          Sys[cop]->city_x, Sys[cop]->city_y,
          Sys[cop]->city_x, Sys[cop]->city_y,
          0, 0
        );
        sprintf(str, glob->m35, Sys[cop]->Name);
        City_Message
        (
          str,
          Sys[cop]->net_x,
          Sys[cop]->net_y,
          -1
        );
      } else if (dead->sys[p].timer == 0) {
        runner = Find_Program
                 (
                   dead->sys[p].net_x, dead->sys[p].net_y,
                   dead->sys[p].city_x, dead->sys[p].city_y,
                   -1, -1,
                   &s, &c
                 );

        if (runner != NULL) {
          dead->sys[p].timer = 1;
        } else {
          for (g = 0; g != max_guys; ++g) {
            if
            (
              Port[g]->active &&
              Port[g]->access_level != IDLE &&
              Port[g]->net_x == dead->sys[p].net_x &&
              Port[g]->net_y == dead->sys[p].net_y &&
              Port[g]->city_x == dead->sys[p].city_x &&
              Port[g]->city_y == dead->sys[p].city_y
            ) {
              dead->sys[p].timer = 1;
            }
          }
        }

        if (dead->sys[p].timer == 0) {
          DEAD_NODE = dead->sys[p].type;
          dead->sys[p].type = 0;
          Quick_Update
          (
            CITY_VIEW,
            dead->sys[p].net_x, dead->sys[p].net_y,
            dead->sys[p].city_x, dead->sys[p].city_y,
            dead->sys[p].city_x, dead->sys[p].city_y,
            0, 0
          );

          if (DEAD_NODE != END_SYS) {
            Set_System_Path
            (
              dead->sys[p].city_x,
              dead->sys[p].city_y,
              dead->sys[p].net_x,
              dead->sys[p].net_y
            );
            fp = fopen(glob->path, READB);
            fread(a_system, SYSTEM_SIZE, 1, fp);
            fclose(fp);
          } else {
            strcpy(a_system->Name, VALHALLA);
          }

          sprintf(str, glob->m35, a_system->Name);
          City_Message
          (
            str,
            dead->sys[p].net_x,
            dead->sys[p].net_y,
            -1
          );
        }
      }
    }
  }
}
/****************************************************************************
	run a program if the system has it and it's not already running
*/
short EXPORT Use_Program(short s, short p) {
  short   m;
  short   i;

  for (m = 0; m != Sys[s]->mus; ++m) {
    for (i = 0; i != MU_CAPACITY; ++i) {
      if
      (
        Sys[s]->memory[m][i].type == PROGRAM &&
        Sys[s]->memory[m][i].index == p &&
        !Sys[s]->memory[m][i].running &&
        (RANDOM(3) == 1 || p == CASHIER || p == FENCE)
      ) {
        Run_System_Program(s, p);
        return (TRUE);
      }
    }
  }

  return (FALSE);
}
/****************************************************************************
	run a hydra if the system has one that's not already running
*/
short Use_Hydra(short s) {
  short   m;
  short   i;

  for (m = 0; m != Sys[s]->mus; ++m) {
    for (i = 0; i != MU_CAPACITY; ++i) {
      if
      (
        Sys[s]->memory[m][i].type == PROGRAM &&
        Sys[s]->memory[m][i].class == HYDRA &&
        Sys[s]->memory[m][i].sub[0] &&
        !Sys[s]->memory[m][i].running &&
        RANDOM(3) == 1
      ) {
        Run_System_Program(s, Sys[s]->memory[m][i].index);
        return (TRUE);
      }
    }
  }

  return (FALSE);
}
/****************************************************************************
	load up system programs (defense, etc)
*/
short EXPORT Sys_Actions(short s) {
  short   g, p;
  short   kt;
  short   ran = FALSE;
  short   enemies = FALSE;
  short   inside_enemies = FALSE;

  if (Sys[s]->cpus == 0) {
    EXIT0;
  }

  if (Sys[s]->type == FILE_SERVER && Proc[s]->file_server != 0) {
    EXIT0;
  }

  if (Sys[s]->type == BBS) {
    Use_Program(s, GABLINK);
    EXIT0;
  }

  if (Sys[s]->type == VENDOR) {
    ran = Use_Program(s, CASHIER);
  } else if (Sys[s]->type == CRIMINAL) {
    ran = Use_Program(s, FENCE);
  } else if
  (
    Sys[s]->type != COP_SYS &&
    Sys[s]->type != CON_SYS &&
    RANDOM(Sys[s]->type != 1)
  ) {
    ran = Use_Program(s, GUARDIAN);

    if (!ran) {
      ran = Use_Program(s, SOLDIER);
    }
  }

  if (Sys[s]->breached) {
    ran = Use_Program(s, MASON);
  }

  if (Proc[s]->infected) {
    ran = Use_Program(s, ANTIVIRUS);
  }

  if (Proc[s]->moled) {
    ran = Use_Program(s, SNOUT);
  }

  if (Proc[s]->torped) {
    ran = Use_Program(s, PATRIOT);
  }

  for (g = 0; g != max_guys; ++g) {
    if
    (
      !Port[g]->active ||
      Port[g]->access_level != SYSTEM_VIEW ||
      Port[g]->sys_num != s
    ) {
      Proc[s]->enemy[g] = FALSE;
    } else {
      if
      (
        Sys[s]->type == LEV3 ||
        Sys[s]->type == LEV4 ||
        Sys[s]->type == LEV5 ||
        Sys[s]->type == LEV6 ||
        Sys[s]->type == LEV7 ||
        Sys[s]->type == WEB_SERVER ||
        Sys[s]->type == HUB ||
        Sys[s]->type == WORLDBANK ||
        Sys[s]->type == END_SYS ||
        Sys[s]->type == ALIEN_SYS ||
        Sys[s]->type == CYBERIA_SYS
      ) {
        Proc[s]->enemy[g] = TRUE;
      } else if
      (
        Sys[s]->type == LEV1 ||
        Sys[s]->type == LEV2 ||
        Sys[s]->type == CRIMINAL
      ) {
        if (Port[g]->inside) {
          Proc[s]->enemy[g] = TRUE;
        }
      } else if (Sys[s]->type == VENDOR || Sys[s]->type == FILE_SERVER) {
        if (Port[g]->inside && Port[g]->code[0] == 0) {
          Proc[s]->enemy[g] = TRUE;
        }
      } else if (Sys[s]->type == COP_SYS) {
        Port[g]->guy.last_net_crime = time(0);
        Port[g]->guy.wanted = TRUE;
      }

      if (Proc[s]->enemy[g]) {
        enemies = TRUE;

        if
        (
          Port[g]->inside ||
          Sys[s]->type == END_SYS ||
          Sys[s]->type == ALIEN_SYS ||
          Sys[s]->type == CYBERIA_SYS
        ) {
          inside_enemies = TRUE;
        }
      }
    }

    if (Proc[s]->enemy[g]) {
      Port[g]->code[0] = 0;
    }
  }

  if (Sys[s]->type == COP_SYS) {
    EXIT0;
  }

  if (enemies) {
    kt = Knight_Target(s, &g, &p);

    if (!ran && kt) {
      ran = Use_Program(s, KNIGHT);
    }

    if (!ran && kt) {
      ran = Use_Program(s, ADDLE);
    }

    if (!ran) {
      ran = Use_Program(s, PUNTER);
    }

    if (!ran && inside_enemies) {
      ran = Use_Program(s, ICEBOLT);
    }

    if (!ran) {
      ran = Use_Program(s, HANDCUFF);
    }

    if (!ran) {
      ran = Use_Program(s, SURGE);
    }

    if (!ran && inside_enemies) {
      ran = Use_Program(s, LIGHTNING);
    }

    if (!ran) {
      ran = Use_Program(s, INFILTRATE);
    }

    if (!ran) {
      ran = Use_Hydra(s);
    }

    if (!ran) {
      ran = Use_Program(s, JOLT);
    }

    if (!ran && inside_enemies) {
      ran = Use_Program(s, FIREBOLT);
    }

    if (!ran) {
      ran = Use_Program(s, BRAINDRAIN);
    }

    if (!ran && inside_enemies) {
      ran = Use_Program(s, LOBOTOMY);
    }

    if (!ran && inside_enemies) {
      ran = Use_Program(s, BANSHEE);
    }

    if (!ran && inside_enemies) {
      ran = Use_Program(s, GUARDIAN);
    }

    if (!ran && inside_enemies) {
      Use_Program(s, SOLDIER);
    }
  }

  EXIT0;
}
/****************************************************************************
	see if a guy de-rezzes because of cloak/muffler
*/
short Cloak_Entropy(short s, short strength) {
  if (s >= max_guys) {
    EXIT0;
  }

  if (Port[s]->guy.perm[GAMEOP_PERM]) {
    EXIT0;
  }

  if (RANDOM(glob->derez1) != 1) {
    EXIT0;
  }

  myp = s;

  if (MY_PORT->access_level == IDLE) {
    EXIT0;
  }

  MY_PORT->rez -= RANDOM(strength);

  if (MY_PORT->rez <= 0) {
    strcpy(str, getmsg(MSG437));
    Send(str, s, TRUE);
    Jack_Out();
  } else {
    Update_Location();
  }

  EXIT0;
}
/****************************************************************************
	advance all system timers. Take appropriate action.
*/
short EXPORT Sys_Timers(short s) {
  short   m;
  short   i;
  short   t;

  for (m = 0; m != Sys[s]->mus; ++m) {
    for (i = 0; i != MU_CAPACITY; ++i) {
      Sys[s]->memory[m][i].running = FALSE;
    }
  }

  for (i = 0; i != Sys[s]->cpus; ++i) {
    if (SYS_PROGRAM.m_index >= 0) {
      SYS_SOURCE.running = TRUE;
    }

    if (Sys[s]->crash_time[i]) {
      if
      (
        Sys[s]->crash_time[i] < 0 ||
        Sys[s]->crash_time[i] > 32000
      ) {
        Sys[s]->crash_time[i] = 0;
      } else {
        --Sys[s]->crash_time[i];
      }

      if (Sys[s]->crash_time[i] == 0) {
        sprintf(str, getmsg(MSG46), i + 1);
        Send(str, s, FALSE);
        Update_Others
        (
          SYSTEM_VIEW,
          Sys[s]->net_x, Sys[s]->net_y,
          Sys[s]->city_x, Sys[s]->city_y
        );
      }
    }
  }

  if (s >= max_guys) {
    Sys_Actions(s);
  }

  for (i = 0; i != Sys[s]->cpus; ++i) {
    if (Sys[s]->in_use || (s < max_guys && Port[s]->active)) {
      /* ok */
    } else {
      EXIT0;
    }

    if (SYS_PROGRAM.m_index >= 0) {
      if (SYS_PROGRAM.loading) {
        --SYS_PROGRAM.loading;
      }

      if (SYS_PROGRAM.tentacle_timer) {
        --SYS_PROGRAM.tentacle_timer;
      }

      if
      (
        (
          SYS_SOURCE.class == HYDRA ||
          SYS_SOURCE.index == SURGE ||
          SYS_SOURCE.index == ICEBOLT ||
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
        ) &&
        !SYS_PROGRAM.loading
      ) {
        Seeker(s, i);
      } else if
      (
        !SYS_PROGRAM.loading &&
        (
          SYS_SOURCE.index == GUARDIAN ||
          SYS_SOURCE.index == CASHIER ||
          SYS_SOURCE.index == SOLDIER
        )
      ) {
        Guardian(s, i);
      } else if (!SYS_PROGRAM.loading && SYS_SOURCE.index == TRACE) {
        Trace(s, i);
      } else if (SYS_SOURCE.index == CLOAK || SYS_SOURCE.index == MUFFLER) {
        Cloak_Entropy(s, SYS_SOURCE.strength);
      } else if (!SYS_PROGRAM.loading && SYS_SOURCE.index == FENCE) {
        Fence(s, i);
      } else if
      (
        SYS_SOURCE.index == KNIGHT ||
        SYS_SOURCE.index == ADDLE ||
        SYS_SOURCE.index == HERCULES
      ) {
        Program_Seeker(s, i);
      } else if (SYS_SOURCE.index == RADAR && !SYS_PROGRAM.loading) {
        SYS_PROGRAM.loading = RADAR_RATE;
        Radar(s, i);
      } else if (SYS_SOURCE.index == SENTRY && !SYS_PROGRAM.loading) {
        Find_Enemies(s, i);
      } else if (SYS_SOURCE.index == VIRUS && !SYS_PROGRAM.loading) {
        Virus(s, i);
      } else if (SYS_SOURCE.index == MASON && !SYS_PROGRAM.loading) {
        Mason(s, i);
      } else if (SYS_SOURCE.index == SNOUT && !SYS_PROGRAM.loading) {
        Snout(s, i);
      } else if (SYS_SOURCE.index == PATRIOT && !SYS_PROGRAM.loading) {
        Patriot(s, i);
      } else if (SYS_SOURCE.index == ANTIVIRUS && !SYS_PROGRAM.loading) {
        Anti_Virus(s, i);
      } else if
      (
        SYS_SOURCE.index == MOLE ||
        SYS_SOURCE.index == BULLDOZER
      ) {
        if (SYS_PROGRAM.loading == 0) {
          Hammered(i, s);
        } else {
          Hammering(i, s);
        }
      } else if (SYS_SOURCE.index == UNLOCKER) {
        if (SYS_PROGRAM.loading == 0) {
          Unlocked(i, s);
        } else {
          Unlocking(i, s);
        }
      } else if (SYS_SOURCE.index == GATECRASH) {
        if (SYS_PROGRAM.loading == 0) {
          Gate_Crashed(i, s);
        } else {
          Gate_Crashing(i, s);
        }
      } else if (SYS_SOURCE.index == CRASH) {
        if (SYS_PROGRAM.loading == 0) {
          Crashed(i, s);
        } else {
          Crashing(i, s);
        }
      } else if (SYS_SOURCE.index == TORPEDO) {
        if (SYS_PROGRAM.loading == 0) {
          Torped(i, s);
        } else {
          Torping(i, s);
        }
      } else if (SYS_SOURCE.index == BLOB && !SYS_PROGRAM.loading) {
        t = (SYS_SOURCE.strength / BLOB_FACTOR);

        if (t == 0) {
          t = 1;
        }

        glob->blob[Runner_System_Number(s, i)] = t;
      } else if (SYS_SOURCE.index == LOCKER) {
        if (!SYS_PROGRAM.loading) {
          Locked(i, s);
        } else {
          Locking(i, s);
        }
      }

      if (!SYS_PROGRAM.loading && glob->blob[s]) {
        SYS_PROGRAM.loading = glob->blob[s];
      }
    }
  }

  EXIT0;
}
/****************************************************************************
	system program determines it's enemies
*/
void EXPORT Find_Enemies(short s, short i) {
  short   g;
  short   p;
  short   cloaked;
  short   guy_roll;
  short   my_roll;
  short   was_seen;
  short   in_system = FALSE;
  short   in_view;

  for (g = 0; g != max_guys; ++g) {
    was_seen = SYS_PROGRAM.sees[g];
    SYS_PROGRAM.enemy[g] = FALSE;
    SYS_PROGRAM.sees[g] = FALSE;
    in_view = FALSE;

    if (SYS_PROGRAM.sys_x >= 0 && SYS_PROGRAM.sys_y >= 0) {
      in_system = TRUE;

      if (Port[g]->access_level == SYSTEM_VIEW) {
        in_view = TRUE;
      }
    } else {
      if (Port[g]->access_level == CITY_VIEW) {
        in_view = TRUE;
      }
    }

    if
    (
      Port[g]->active &&
      in_view &&
      Port[g]->net_x == SYS_PROGRAM.net_x &&
      Port[g]->net_y == SYS_PROGRAM.net_y &&
      (!in_system ||
       (Port[g]->city_x == SYS_PROGRAM.city_x &&
        Port[g]->city_y == SYS_PROGRAM.city_y))
    ) {
      for (p = 0, cloaked = FALSE; p != Sys[g]->cpus; ++p) {
        if
        (
          GUY_PROGRAM.m_index >= 0 &&
          (GUY_SOURCE.index == CLOAK || GUY_SOURCE.index == MUFFLER)
        ) {
          guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
          my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

          if (guy_roll > my_roll && Hydra_Has(RADAR, s, i)) {
            guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
            my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);
          }

          if (guy_roll > my_roll) {
            cloaked = TRUE;
          }
        }
      }

      if (!cloaked) {
        SYS_PROGRAM.sees[g] = TRUE;

        if
        (
          !was_seen &&
          s < max_guys &&
          s != g &&
          (
            SYS_SOURCE.index == SENTRY ||
            SYS_SOURCE.index == GUARDIAN ||
            SYS_SOURCE.index == SOLDIER
          )
        ) {
          sprintf
          (
            str,
            getmsg(MSG47),
            SYS_SOURCE.name,
            him.alias,
            Runner_System_Name(s, i)
          );
          Send(str, s, FALSE);
        }
      }
    }

    if (SYS_PROGRAM.sees[g] && g != s) {
      if (SYS_SOURCE.index == CASHIER) {
        if (Port[g]->code[0] == 0 && Port[g]->inside) {
          SYS_PROGRAM.enemy[g] = TRUE;
        } else if
        (
          Port[g]->inside &&
          (System_Is_Running(g, CLOAK) ||
           System_Is_Running(g, MUFFLER))
        ) {
          SYS_PROGRAM.enemy[g] = TRUE;
        }
      } else if (SYS_SOURCE.index == FENCE) {
        if (Port[g]->inside || Proc[s]->enemy[g]) {
          SYS_PROGRAM.enemy[g] = TRUE;
        }
      } else {
        SYS_PROGRAM.enemy[g] = TRUE;
      }
    }

    if
    (
      (
        SYS_SOURCE.index == SENTRY ||
        SYS_SOURCE.index == GUARDIAN ||
        SYS_SOURCE.index == SOLDIER
      ) &&
      was_seen &&
      !SYS_PROGRAM.sees[g] &&
      s < max_guys &&
      s != g
    ) {
      sprintf
      (
        str,
        getmsg(MSG48),
        SYS_SOURCE.name,
        him.alias,
        Runner_System_Name(s, i)
      );
      Send(str, s, FALSE);
    }
  }
}
/****************************************************************************
	see if fence makes a move on a netrunner
*/
short EXPORT Fence(short s, short i) {
  short   j;
  short   nailed;
  short   stuck;
  short   x, y;
  short   dummy_s, dummy_c;
  short   anybody = FALSE;
  short   enemies = FALSE;
  short   n = Runner_System_Number(s, i);
  struct  cpu_struct *runner;
  Find_Enemies(s, i);

  for (myp = 0; myp != max_guys; ++myp) {
    if (MY_PORT->active && MY_PORT->sys_num == n) {
      anybody = TRUE;

      if (Proc[s]->enemy[myp]) {
        enemies = TRUE;
      }
    }

    if (SYS_PROGRAM.enemy[myp]) {
      nailed = Sys_Track
               (
                 s,
                 &SYS_PROGRAM,
                 MY_PORT->sys_x,
                 MY_PORT->sys_y,
                 s,
                 &stuck
               );

      if (stuck && RANDOM(10) == 1) {
        Program_Stops(s, i);
      }

      if (!nailed) {
        EXIT0;
      }

      nailed = Defense_Check(s, i, myp);

      if (!nailed) {
        EXIT0;
      }

      SYS_PROGRAM.enemy[myp] = FALSE;
      SYS_PROGRAM.sees[myp] = FALSE;
      sprintf
      (
        str,
        getmsg(MSG49),
        SYS_SOURCE.name,
        me.alias,
        Runner_System_Name(s, i)
      );
      Send(str, s, FALSE);
      sprintf(str, getmsg(MSG50), SYS_SOURCE.name);
      Send(str, myp, FALSE);
      Server_Update();

      if (s >= max_guys) {
        Program_Stops(s, i);
      }

      if (!me.perm[GAMEOP_PERM]) {
        Jack_Out();

        for (j = MY_DECK->mus - 1; j >= 0; --j) {
          if (RANDOM(FENCE_ZAP) == 1) {
            Remove_Memory(MY_DECK, j);
            --MY_DECK->mus;
          }
        }
      }

      EXIT0;
    }
  }

  if (Sys[s]->type != CRIMINAL || enemies) {
    EXIT0;
  }

  if (!anybody) {
    Program_Stops(s, i);
    EXIT0;
  }

  x = SYS_PROGRAM.sys_x;
  y = SYS_PROGRAM.sys_y;

  if
  (
    Sys[s]->node_type[x][y] == OPENED_GATE ||
    Sys[s]->node_type[x][y] == CLOSED_GATE
  ) {
    EXIT0;
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      if
      (
        Sys[s]->node_type[x][y] == OPENED_GATE ||
        Sys[s]->node_type[x][y] == CLOSED_GATE
      ) {
        runner = Find_Program
                 (
                   SYS_PROGRAM.net_x, SYS_PROGRAM.net_y,
                   SYS_PROGRAM.city_x, SYS_PROGRAM.city_y,
                   x, y,
                   &dummy_s, &dummy_c
                 );

        if (runner == NULL) {
          System_Track(s, i, x, y);
          EXIT0;
        }
      }
    }
  }

  EXIT0;
}
/****************************************************************************
	I just traced a player
*/
short EXPORT Trace(short s, short i) {
  short   g = SYS_PROGRAM.target_s;

  if (g >= max_guys) {
    sprintf(str, getmsg(MSG657), SYS_SOURCE.name);
    Send(str, s, FALSE);
    Program_Stops(s, i);
    EXIT0;
  }

  if (Port[g]->access_level == IDLE) {
    Program_Stops(s, i);
    EXIT0;
  }

  if (SYS_PROGRAM.target_m) {
    EXIT0;
  }

  SYS_PROGRAM.target_m = TRUE;

  if (Port[g]->access_level == CITY_VIEW) {
    sprintf
    (
      str,
      getmsg(MSG655),
      SYS_SOURCE.name,
      Port[g]->guy.alias,
      HIS_CITY.name
    );
  } else {
    sprintf
    (
      str,
      getmsg(MSG656),
      SYS_SOURCE.name,
      Port[g]->guy.alias,
      HIS_CITY.name,
      Sys[HIS]->Name
    );
  }

  Send(str, s, FALSE);
  EXIT0;
}
/****************************************************************************
	see if gaurdian makes a move on a netrunner
*/
short EXPORT Guardian(short s, short i) {
  short   n = Runner_System_Number(s, i);
  short   in_system = TRUE;
  short   anybody = FALSE;
  short   nailed;
  short   stuck;
  short   cashier;
  short   dmg;

  if (n < 0) {
    if (SYS_PROGRAM.sys_x >= 0 && SYS_PROGRAM.sys_y >= 0) {
      EXIT0;
    }

    in_system = FALSE;
  }

  Find_Enemies(s, i);

  for (myp = 0; myp != max_guys; ++myp) {
    if (MY_PORT->active && in_system && MY_PORT->sys_num == n) {
      anybody = TRUE;
    }

    if (SYS_PROGRAM.sees[myp]) {
      if (SYS_SOURCE.index == GUARDIAN || SYS_SOURCE.index == SOLDIER) {
        if (s < max_guys || Proc[s]->enemy[myp]) {
          SYS_PROGRAM.enemy[myp] = TRUE;
        } else {
          SYS_PROGRAM.enemy[myp] = FALSE;
        }
      } else {
        if (Proc[s]->enemy[myp]) {
          SYS_PROGRAM.enemy[myp] = TRUE;
        } else if (MY_PORT->code[0] == 0 && MY_PORT->inside) {
          SYS_PROGRAM.enemy[myp] = TRUE;
        } else if
        (
          MY_PORT->inside &&
          (System_Is_Running(myp, CLOAK) ||
           System_Is_Running(myp, MUFFLER))
        ) {
          SYS_PROGRAM.enemy[myp] = TRUE;
        } else {
          SYS_PROGRAM.enemy[myp] = FALSE;
        }
      }
    }

    if
    (
      s != myp &&
      SYS_PROGRAM.sees[myp] &&
      SYS_PROGRAM.enemy[myp]
    ) {
      if (in_system) {
        nailed = Sys_Track
                 (
                   n,
                   &SYS_PROGRAM,
                   MY_PORT->sys_x,
                   MY_PORT->sys_y,
                   s,
                   &stuck
                 );

        if (stuck && RANDOM(STUCK_GUARD) == 1) {
          Program_Stops(s, i);
        }
      } else {
        nailed = City_Track
                 (
                   &SYS_PROGRAM,
                   MY_PORT->city_x,
                   MY_PORT->city_y,
                   s
                 );
      }

      if (!nailed) {
        EXIT0;
      }

      nailed = Defense_Check(s, i, myp);

      if (nailed) {
        SYS_PROGRAM.enemy[myp] = FALSE;
        SYS_PROGRAM.sees[myp] = FALSE;

        if (SYS_SOURCE.index == SOLDIER) {
          sprintf
          (
            str,
            getmsg(MSG664),
            SYS_SOURCE.name,
            me.alias,
            Runner_System_Name(s, i)
          );
        } else {
          sprintf
          (
            str,
            getmsg(MSG49),
            SYS_SOURCE.name,
            me.alias,
            Runner_System_Name(s, i)
          );
        }

        Send(str, s, FALSE);

        if (SYS_SOURCE.index == GUARDIAN) {
          cashier = FALSE;
          sprintf
          (
            str,
            getmsg(MSG51),
            SYS_SOURCE.name
          );
          Send(str, myp, FALSE);
        } else if (SYS_SOURCE.index == SOLDIER) {
          cashier = FALSE;
          dmg = Armor_Check(s, i, myp, glob->soldmg);

          if (me.perm[GAMEOP_PERM]) {
            dmg = 0;
          }

          me.damage += dmg;
          sprintf(str, getmsg(MSG663), SYS_SOURCE.name, dmg);
          Send(str, myp, FALSE);

          if (me.damage >= glob->dpts) {
            strcpy(str, getmsg(MSG15));
            Send(str, myp, FALSE);
          }
        } else {
          cashier = TRUE;
          sprintf(str, getmsg(MSG52), SYS_SOURCE.name);
          Send(str, myp, FALSE);
        }

        Server_Update();

        if (s >= max_guys) {
          Program_Stops(s, i);
        }

        if (!me.perm[GAMEOP_PERM]) {
          Jack_Out();

          if (cashier) {
            for (n = 0; n != MY_DECK->mus; ++n) {
              Erase_Memory(myp, n);
            }
          }
        }

        EXIT0;
      }
    }
  }

  if (s >= max_guys && !anybody) {
    Program_Stops(s, i);
  }

  EXIT0;
}
/****************************************************************************
	seeker target not in this system, leave
*/
short EXPORT Seeker_Leaving_System(short s, short i) {
  short   g;
  short   dir;
  short   nailed;
  short   dummy_s, dummy_c;
  short   n = Runner_System_Number(s, i);
  short   city_x;
  short   city_y;
  short   stuck;
  short   seek_x = SYS_PROGRAM.seek_x;
  short   seek_y = SYS_PROGRAM.seek_y;
  short   net_x = SYS_PROGRAM.net_x;
  short   net_y = SYS_PROGRAM.net_y;
  short   old_city_x = SYS_PROGRAM.city_x;
  short   old_city_y = SYS_PROGRAM.city_y;
  struct  cpu_struct  *runner;

  if (seek_x == -1 || seek_y == -1) {
    dir = RANDOM(4);

    if (dir == 1) {
      if (SYS_PROGRAM.city_x != 0) {
        seek_x = 0;
        seek_y = RANDOM(MAX_SYS_Y) - 1;
      }
    } else if (dir == 2) {
      if (SYS_PROGRAM.city_x != MAX_CITY_X - 1) {
        seek_x = MAX_SYS_X - 1;
        seek_y = RANDOM(MAX_SYS_Y) - 1;
      }
    } else if (dir == 3) {
      if (SYS_PROGRAM.city_y != MAX_CITY_Y - 1) {
        seek_x = RANDOM(MAX_SYS_X) - 1;
        seek_y = MAX_SYS_Y - 1;
      }
    } else {
      if (SYS_PROGRAM.city_y != 0) {
        seek_x = RANDOM(MAX_SYS_X) - 1;
        seek_y = 0;
      }
    }
  }

  if (seek_x == -1 || seek_y == -1) {
    EXIT0;
  }

  nailed = Sys_Track
           (
             n,
             &SYS_PROGRAM,
             seek_x,
             seek_y,
             s,
             &stuck
           );

  if (!nailed) {
    SYS_PROGRAM.seek_x = seek_x;
    SYS_PROGRAM.seek_y = seek_y;
    EXIT0;
  }

  SYS_PROGRAM.seek_x = -1;
  SYS_PROGRAM.seek_y = -1;
  city_x = SYS_PROGRAM.city_x;
  city_y = SYS_PROGRAM.city_y;

  if (SYS_PROGRAM.sys_x == 0 && SYS_PROGRAM.city_x != 0) {
    --city_x;
  } else if
  (
    SYS_PROGRAM.sys_x == MAX_SYS_X - 1 &&
    SYS_PROGRAM.city_x != MAX_CITY_X - 1
  ) {
    ++city_x;
  } else if (SYS_PROGRAM.sys_y == 0 && SYS_PROGRAM.city_y != 0) {
    --city_y;
  } else if
  (
    SYS_PROGRAM.sys_y == MAX_SYS_Y - 1 &&
    SYS_PROGRAM.city_y != MAX_CITY_Y - 1
  ) {
    ++city_y;
  }

  runner = Find_Program
           (
             net_x, net_y,
             city_x, city_y,
             -1, -1,
             &dummy_s, &dummy_c
           );

  if (runner != NULL) {
    EXIT0;
  }

  for (g = 0; g != max_guys; ++g) {
    if
    (
      Port[g]->active &&
      Port[g]->access_level == CITY_VIEW &&
      Port[g]->net_x == net_x &&
      Port[g]->net_y == net_y &&
      Port[g]->city_x == city_x &&
      Port[g]->city_y == city_y
    ) {
      EXIT0;
    }
  }

  if
  (
    world->city[net_x][net_y].type[city_x][city_y] != 0 &&
    world->city[net_x][net_y].type[city_x][city_y] != PHONE
  ) {
    Jump_To_System(s, i, city_x, city_y);
    EXIT0;
  }

  SYS_PROGRAM.city_x = city_x;
  SYS_PROGRAM.city_y = city_y;
  SYS_PROGRAM.sys_x = -1;
  SYS_PROGRAM.sys_y = -1;
  sprintf(str, glob->m53, SYS_SOURCE.name);
  City_Program_Message(str, net_x, net_y, s, i);
  Update_Others(CITY_VIEW, net_x, net_y, city_x, city_y);
  sprintf(str, glob->m54, SYS_SOURCE.name);
  System_Program_Message(str, n, s, i);
  Update_Others(SYSTEM_VIEW, net_x, net_y, old_city_x, old_city_y);
  Close_System_Check(n, 0);
  Sentry_Check(s, i);
  EXIT0;
}
/****************************************************************************
	track him down - I'm not in his city
*/
short EXPORT Seek_In_Remote_City(short s, short i) {
  short   g;
  short   dir;
  short   nailed;
  short   dummy_s, dummy_c;
  short   blocked = FALSE;
  short   city_x;
  short   city_y;
  short   seek_x = SYS_PROGRAM.seek_x;
  short   seek_y = SYS_PROGRAM.seek_y;
  short   net_x = SYS_PROGRAM.net_x;
  short   net_y = SYS_PROGRAM.net_y;
  short   old_net_x = SYS_PROGRAM.net_x;
  short   old_net_y = SYS_PROGRAM.net_y;
  struct  cpu_struct  *runner;

  if (net_y > MY_PORT->net_y) {
    dir = NORTH;

    if (seek_y != 0) {
      seek_y = -1;
    }
  } else if (net_y < MY_PORT->net_y) {
    dir = SOUTH;

    if (seek_y != MAX_SYS_Y - 1) {
      seek_y = -1;
    }
  } else if (net_x > MY_PORT->net_x) {
    dir = WEST;

    if (seek_x != 0) {
      seek_x = -1;
    }
  } else if (net_x < MY_PORT->net_x) {
    dir = EAST;

    if (seek_x != MAX_SYS_X - 1) {
      seek_x = -1;
    }
  }

  if (seek_x == -1 || seek_y == -1) {
    if (dir == NORTH) {
      seek_x = RANDOM(MAX_CITY_X) - 1;
      seek_y = 0;
    } else if (dir == SOUTH) {
      seek_x = RANDOM(MAX_CITY_X) - 1;
      seek_y = MAX_CITY_Y - 1;
    } else if (dir == EAST) {
      seek_x = MAX_CITY_X - 1;
      seek_y = RANDOM(MAX_CITY_Y) - 1;
    } else {
      seek_x = 0;
      seek_y = RANDOM(MAX_CITY_Y) - 1;
    }
  }

  nailed = City_Track(&SYS_PROGRAM, seek_x, seek_y, s);

  if (!nailed) {
    SYS_PROGRAM.seek_x = seek_x;
    SYS_PROGRAM.seek_y = seek_y;

    if (Sys[s]->type == CON_SYS) {
      SYS_PROGRAM.loading = RANDOM(3);
    }

    EXIT0;
  }

  SYS_PROGRAM.seek_x = -1;
  SYS_PROGRAM.seek_y = -1;

  if (world->city[net_x][net_y].type[seek_x][seek_y]) {
    blocked = TRUE;
  } else {
    runner = Find_Program
             (
               net_x, net_y,
               seek_x,
               seek_y,
               -1, -1,
               &dummy_s, &dummy_c
             );

    if (runner != NULL) {
      blocked = TRUE;
    } else {
      for (g = 0; g != max_guys; ++g) {
        if
        (
          Port[g]->active &&
          Port[g]->access_level != IDLE &&
          Port[g]->net_x == net_x &&
          Port[g]->net_y == net_y &&
          Port[g]->city_x == seek_x &&
          Port[g]->city_y == seek_y
        ) {
          blocked = TRUE;
        }
      }
    }
  }

  if (blocked) {
    EXIT0;
  }

  SYS_PROGRAM.city_x = seek_x;
  SYS_PROGRAM.city_y = seek_y;
  city_x = SYS_PROGRAM.city_x;
  city_y = SYS_PROGRAM.city_y;

  if (dir == WEST) {
    --net_x;
    city_x = MAX_CITY_X - 1;
  } else if (dir == EAST) {
    ++net_x;
    city_x = 0;
  } else if (dir == NORTH) {
    --net_y;
    city_y = MAX_CITY_Y - 1;
  } else {
    ++net_y;
    city_y = 0;
  }

  if (net_y < 0) {
    net_y = 0;
  } else if (net_y >= MAX_NET_Y) {
    net_y = MAX_NET_Y - 1;
  }

  if (net_x >= MAX_NET_X) {
    net_x -= MAX_NET_X;
  } else if (net_x < 0) {
    net_x += MAX_NET_X;
  }

  if (world->city[net_x][net_y].type[city_x][city_y]) {
    EXIT0;
  }

  runner = Find_Program
           (
             net_x, net_y,
             city_x, city_y,
             -1, -1,
             &dummy_s, &dummy_c
           );

  if (runner != NULL) {
    EXIT0;
  }

  for (g = 0; g != max_guys; ++g) {
    if
    (
      Port[g]->active &&
      Port[g]->access_level == CITY_VIEW &&
      Port[g]->net_x == net_x &&
      Port[g]->net_y == net_y &&
      Port[g]->city_x == city_x &&
      Port[g]->city_y == city_y
    ) {
      EXIT0;
    }
  }

  SYS_PROGRAM.net_x = net_x;
  SYS_PROGRAM.net_y = net_y;
  SYS_PROGRAM.city_x = city_x;
  SYS_PROGRAM.city_y = city_y;
  sprintf(str, glob->m53, SYS_SOURCE.name);
  City_Program_Message(str, net_x, net_y, s, i);
  Update_Others(CITY_VIEW, net_x, net_y, city_x, city_y);
  sprintf(str, glob->m54, SYS_SOURCE.name);
  City_Program_Message(str, old_net_x, old_net_y, s, i);
  Update_Others(CITY_VIEW, old_net_x, old_net_y, city_x, city_y);
  Sentry_Check(s, i);
  EXIT0;
}
/****************************************************************************
	track him down - I'm in his city, he's in a system
*/
short EXPORT Seek_In_Local_System(short s, short i) {
  short   x, y;
  short   n;
  short   nailed;
  short   dir;
  short   old_city_x = SYS_PROGRAM.city_x;
  short   old_city_y = SYS_PROGRAM.city_y;
  short   tries = 0;
  short   done;
  short   dummy_s, dummy_c;
  struct  cpu_struct *runner;
  nailed = City_Track(&SYS_PROGRAM, MY_PORT->city_x, MY_PORT->city_y, s);

  if (!nailed) {
    EXIT0;
  }

  if (SYS_PROGRAM.city_x < MY_PORT->city_x) {
    dir = EAST;
  } else if (SYS_PROGRAM.city_x > MY_PORT->city_x) {
    dir = WEST;
  } else if (SYS_PROGRAM.city_y > MY_PORT->city_y) {
    dir = NORTH;
  } else {
    dir = SOUTH;
  }

  SYS_PROGRAM.city_x = MY_PORT->city_x;
  SYS_PROGRAM.city_y = MY_PORT->city_y;
  n = MY_PORT->sys_num;

  do {
    done = TRUE;

    if (dir == EAST) {
      x = 0;
      y = RANDOM(MAX_SYS_Y) - 1;
    } else if (dir == WEST) {
      x = MAX_SYS_X - 1;
      y = RANDOM(MAX_SYS_Y) - 1;
    } else if (dir == NORTH) {
      x = RANDOM(MAX_SYS_X) - 1;
      y = MAX_SYS_Y - 1;
    } else {
      x = RANDOM(MAX_SYS_X) - 1;
      y = 0;
    }

    if (Sys[n]->node_type[x][y] != 0) {
      done = FALSE;
    } else {
      runner = Find_Program
               (
                 SYS_PROGRAM.net_x, SYS_PROGRAM.net_y,
                 SYS_PROGRAM.city_x, SYS_PROGRAM.city_y,
                 x, y,
                 &dummy_s, &dummy_c
               );

      if (runner != NULL) {
        done = FALSE;
      }
    }

    ++tries;
  } while (tries < 15 && !done);

  if (!done) {
    SYS_PROGRAM.city_x = old_city_x;
    SYS_PROGRAM.city_y = old_city_y;
    EXIT0;
  }

  SYS_PROGRAM.sys_x = x;
  SYS_PROGRAM.sys_y = y;
  sprintf(str, glob->m53, SYS_SOURCE.name);
  System_Program_Message(str, n, s, i);
  Update_Others
  (
    SYSTEM_VIEW,
    MY_PORT->net_x, MY_PORT->net_y,
    MY_PORT->city_x, MY_PORT->city_y
  );
  sprintf(str, getmsg(MSG55), SYS_SOURCE.name, Sys[n]->Name);
  City_Program_Message(str, MY_PORT->net_x, MY_PORT->net_y, s, i);
  Update_Others
  (
    CITY_VIEW,
    MY_PORT->net_x, MY_PORT->net_y,
    MY_PORT->city_x, MY_PORT->city_y
  );
  Sentry_Check(s, i);
  EXIT0;
}
/****************************************************************************
	move a seeker from one system to an adjacent system
*/
short EXPORT Jump_To_System(short s, short i, short city_x, short city_y) {
  short   n = -1;
  short   old_n = Runner_System_Number(s, i);
  short   j;
  short   dir;
  short   old_city_x = SYS_PROGRAM.city_x;
  short   old_city_y = SYS_PROGRAM.city_y;
  short   old_sys_x = SYS_PROGRAM.sys_x;
  short   old_sys_y = SYS_PROGRAM.sys_y;
  short   tries = 0;
  short   dummy_s, dummy_c;
  struct  cpu_struct *runner;

  for (j = 0; j != max_systems; ++j) {
    if
    (
      Sys[j]->in_use &&
      Sys[j]->net_x == SYS_PROGRAM.net_x &&
      Sys[j]->net_y == SYS_PROGRAM.net_y &&
      Sys[j]->city_x == city_x &&
      Sys[j]->city_y == city_y
    ) {
      n = j;
    }
  }

  if (n < 0) {
    EXIT0;
  }

  if (SYS_PROGRAM.city_x < city_x) {
    dir = EAST;
  } else if (SYS_PROGRAM.city_x > city_x) {
    dir = WEST;
  } else if (SYS_PROGRAM.city_y > city_y) {
    dir = NORTH;
  } else {
    dir = SOUTH;
  }

  do {
    if (dir == EAST) {
      SYS_PROGRAM.sys_x = 0;
      SYS_PROGRAM.sys_y = RANDOM(MAX_SYS_Y) - 1;
    } else if (dir == WEST) {
      SYS_PROGRAM.sys_x = MAX_SYS_X - 1;
      SYS_PROGRAM.sys_y = RANDOM(MAX_SYS_Y) - 1;
    } else if (dir == NORTH) {
      SYS_PROGRAM.sys_x = RANDOM(MAX_SYS_X) - 1;
      SYS_PROGRAM.sys_y = MAX_SYS_Y - 1;
    } else {
      SYS_PROGRAM.sys_x = RANDOM(MAX_SYS_X) - 1;
      SYS_PROGRAM.sys_y = 0;
    }

    runner = Find_Program
             (
               SYS_PROGRAM.net_x, SYS_PROGRAM.net_y,
               city_x, city_y,
               SYS_PROGRAM.sys_x, SYS_PROGRAM.sys_y,
               &dummy_s, &dummy_c
             );
    ++tries;
  } while
  (
    tries < 15 &&
    (
      runner != NULL ||
      Sys[n]->node_type[SYS_PROGRAM.sys_x][SYS_PROGRAM.sys_y]
    )
  );

  if (tries >= 15) {
    SYS_PROGRAM.sys_x = old_sys_x;
    SYS_PROGRAM.sys_y = old_sys_y;
    EXIT0;
  }

  SYS_PROGRAM.city_x = city_x;
  SYS_PROGRAM.city_y = city_y;
  sprintf(str, glob->m53, SYS_SOURCE.name);
  System_Program_Message(str, n, s, i);
  Update_Others
  (
    SYSTEM_VIEW,
    SYS_PROGRAM.net_x, SYS_PROGRAM.net_y,
    SYS_PROGRAM.city_x, SYS_PROGRAM.city_y
  );
  sprintf(str, getmsg(MSG55), SYS_SOURCE.name, Sys[n]->Name);
  System_Program_Message(str, old_n, s, i);
  Update_Others
  (
    SYSTEM_VIEW,
    SYS_PROGRAM.net_x, SYS_PROGRAM.net_y,
    old_city_x, old_city_y
  );
  Sentry_Check(s, i);
  EXIT0;
}
/****************************************************************************
	anti-personnel programs try to find non-local netrunner
*/
short EXPORT Seek_Remote(short s, short i) {
  if (SYS_PROGRAM.sys_x < 0 || SYS_PROGRAM.sys_y < 0) {
    if
    (
      SYS_PROGRAM.net_x == MY_PORT->net_x &&
      SYS_PROGRAM.net_y == MY_PORT->net_y &&
      MY_PORT->access_level == CITY_VIEW
    ) {
      EXIT0;
    }

    if
    (
      SYS_PROGRAM.net_x == MY_PORT->net_x &&
      SYS_PROGRAM.net_y == MY_PORT->net_y &&
      MY_PORT->access_level == SYSTEM_VIEW
    ) {
      Seek_In_Local_System(s, i);
    } else {
      Seek_In_Remote_City(s, i);
    }
  } else {
    if
    (
      SYS_PROGRAM.net_x == MY_PORT->net_x &&
      SYS_PROGRAM.net_y == MY_PORT->net_y &&
      SYS_PROGRAM.city_x == MY_PORT->city_x &&
      SYS_PROGRAM.city_y == MY_PORT->city_y
    ) {
      EXIT0;
    }

    Seeker_Leaving_System(s, i);
  }

  EXIT0;
}
/****************************************************************************
	anti-personnel programs makes a move on a netrunner
*/
short EXPORT Seeker(short s, short i) {
  short   g;
  short   c;
  short   n;
  short   m;
  short   p;
  short   infiltrate_m;
  short   infiltrate_p;
  short   infiltrated;
  short   nailed;
  short   in_system = FALSE;
  short   dmg;
  short   stuck = FALSE;
  short   zapped = FALSE;
  short   booted = TRUE;

  if (SYS_PROGRAM.sys_x >= 0 && SYS_PROGRAM.sys_y >= 0) {
    n = Runner_System_Number(s, i);

    if (n < 0) {
      EXIT0;
    }

    in_system = TRUE;
  }

  Find_Enemies(s, i);
  myp = -1;

  for (g = 0; g != max_guys; ++g) {
    if
    (
      Port[g]->active &&
      Port[g]->access_level != IDLE &&
      Port[g]->guy.index == SYS_PROGRAM.target_index
    ) {
      myp = g;
    }
  }

  if (myp == -1) {
    if (Sys[s]->type == CON_SYS && !in_system) {
      Select_Con_Target(s, i);
    } else if (Sys[s]->type == COP_SYS) {
      if (SYS_PROGRAM.cop_x >= 0 && SYS_PROGRAM.cop_y >= 0) {
        Cop_Seek_Remote(s, i);
      }
    } else if (s >= max_guys) {
      Program_Stops(s, i);
    }

    EXIT0;
  }

  if (Sys[s]->type == CON_SYS && RANDOM(1000) == 1) {
    Select_Con_Target(s, i);
    EXIT0;
  }

  if (!SYS_PROGRAM.enemy[myp]) {
    if
    (
      s >= max_guys &&
      RANDOM(200) == 1 &&
      Sys[s]->type != COP_SYS &&
      Sys[s]->type != CON_SYS
    ) {
      Program_Stops(s, i);
    } else {
      Seek_Remote(s, i);
    }

    EXIT0;
  }

  if (in_system) {
    if
    (
      s >= max_guys &&
      !MY_PORT->inside &&
      RANDOM(200) == 1 &&
      (
        SYS_SOURCE.index == ICEBOLT ||
        SYS_SOURCE.index == LIGHTNING ||
        SYS_SOURCE.index == FIEND ||
        SYS_SOURCE.index == SATAN ||
        SYS_SOURCE.index == FIREBOLT ||
        SYS_SOURCE.index == LOBOTOMY ||
        SYS_SOURCE.index == BANSHEE
      )
    ) {
      Program_Stops(s, i);
      EXIT0;
    }

    nailed = Sys_Track
             (
               n,
               &SYS_PROGRAM,
               MY_PORT->sys_x,
               MY_PORT->sys_y,
               s,
               &stuck
             );
  } else {
    if
    (
      s >= max_guys &&
      RANDOM(20) == 1 &&
      (
        SYS_SOURCE.index != BANSHEE &&
        SYS_SOURCE.index != JOLT &&
        SYS_SOURCE.index != FIREBOLT &&
        SYS_SOURCE.class != HYDRA &&
        SYS_SOURCE.index != LOBOTOMY &&
        SYS_SOURCE.index != BRAINDRAIN
      )
    ) {
      Program_Stops(s, i);
      EXIT0;
    }

    nailed = City_Track(&SYS_PROGRAM, MY_PORT->city_x, MY_PORT->city_y, s);
  }

  if (!nailed) {
    if (SYS_SOURCE.index == BRAINDRAIN || SYS_SOURCE.index == LOBOTOMY) {
      SYS_PROGRAM.target_s = -1;
    }

    if (stuck && RANDOM(30) == 1 && Sys[s]->type != COP_SYS) {
      Program_Stops(s, i);
    }

    EXIT0;
  }

  if (SYS_SOURCE.class == HYDRA || SYS_SOURCE.index == MIMIC) {
    nailed = TRUE;
  } else {
    nailed = Defense_Check(s, i, myp);
  }

  if (nailed) {
    if (SYS_SOURCE.index == ICEBOLT) {
      sprintf
      (
        str,
        getmsg(MSG56),
        SYS_SOURCE.name,
        me.alias,
        Runner_System_Name(s, i)
      );
      Send(str, s, FALSE);
    } else if (SYS_SOURCE.index == INFILTRATE) {
      if (!me.perm[GAMEOP_PERM]) {
        infiltrated = FALSE;
      } else {
        infiltrated = TRUE;
      }

      for (m = 0; m != MY_DECK->mus && !infiltrated; ++m) {
        for (p = 0; p != MU_CAPACITY && !infiltrated; ++p) {
          if (MY_DECK->memory[m][p].type == PROGRAM) {
            infiltrated = TRUE;
            infiltrate_m = m;
            infiltrate_p = p;
            sprintf
            (
              str,
              getmsg(M57),
              SYS_SOURCE.name,
              me.alias,
              MY_DECK->memory[m][p].name,
              Runner_System_Name(s, i)
            );
            Send(str, s, FALSE);

            if (s < max_guys && MY_DECK->memory[m][p].value) {
              Scored(s, 1, 1);
            }
          }
        }
      }
    } else if (SYS_SOURCE.index == HANDCUFF) {
      sprintf
      (
        str,
        getmsg(MSG58),
        SYS_SOURCE.name,
        me.alias,
        Runner_System_Name(s, i)
      );
      Send(str, s, FALSE);
    } else if (SYS_SOURCE.index == BRAINDRAIN || SYS_SOURCE.index == LOBOTOMY) {
      if (SYS_PROGRAM.target_s == -1) {
        sprintf
        (
          str,
          getmsg(MSG59),
          SYS_SOURCE.name,
          me.alias,
          Runner_System_Name(s, i)
        );
        Send(str, s, FALSE);
        SYS_PROGRAM.target_s = 1;
      }
    } else if
    (
      SYS_SOURCE.index == FIREBOLT ||
      SYS_SOURCE.index == JOLT ||
      SYS_SOURCE.index == BANSHEE
    ) {
      sprintf
      (
        str,
        getmsg(MSG60),
        SYS_SOURCE.name,
        me.alias,
        Runner_System_Name(s, i)
      );
      Send(str, s, FALSE);
    } else if (SYS_SOURCE.class == HYDRA) {
      if (!me.perm[GAMEOP_PERM]) {
        Hydra_Combat(s, i, myp);
      }
    } else if (SYS_SOURCE.index != MIMIC) {
      sprintf
      (
        str,
        getmsg(MSG49),
        SYS_SOURCE.name,
        me.alias,
        Runner_System_Name(s, i)
      );
      Send(str, s, FALSE);
    }

    if (SYS_PROGRAM.m_index == -1) {
      EXIT0;
    }

    if (SYS_SOURCE.class == HYDRA) {
      booted = FALSE;
    } else if (SYS_SOURCE.index == LIGHTNING) {
      sprintf(str, getmsg(MSG61), SYS_SOURCE.name);
      Send(str, myp, FALSE);
      zapped = TRUE;
    } else if (SYS_SOURCE.index == PUNTER) {
      sprintf(str, getmsg(MSG62), SYS_SOURCE.name);
      Send(str, myp, FALSE);

      for (c = 0; c != MY_DECK->cpus && !me.perm[GAMEOP_PERM]; ++c) {
        MY_DECK->crash_time[c] = glob->puntsecs;
      }

      strcpy(str, getmsg(MSG63));
      Send(str, myp, FALSE);
    } else if (SYS_SOURCE.index == ICEBOLT) {
      sprintf(str, getmsg(MSG64), SYS_SOURCE.name);
      Send(str, myp, FALSE);

      if
      (
        MY_PORT->icebolt_time < glob->boltsecs &&
        !me.perm[GAMEOP_PERM]
      ) {
        MY_PORT->icebolt_time = glob->boltsecs;
      }

      booted = FALSE;
    } else if (SYS_SOURCE.index == HANDCUFF) {
      sprintf(str, getmsg(MSG65), SYS_SOURCE.name);
      Send(str, myp, FALSE);

      if
      (
        MY_PORT->handcuff_time < glob->cuffsecs &&
        !me.perm[GAMEOP_PERM]
      ) {
        MY_PORT->handcuff_time = glob->cuffsecs;
      }

      booted = FALSE;
    } else if (SYS_SOURCE.index == INFILTRATE) {
      booted = FALSE;

      if (infiltrated && !me.perm[GAMEOP_PERM]) {
        sprintf
        (
          str,
          getmsg(M66),
          SYS_SOURCE.name,
          MY_DECK->memory[infiltrate_m][infiltrate_p].name
        );
        Send(str, myp, FALSE);

        if (MY_DECK->memory[infiltrate_m][infiltrate_p].running) {
          for (p = 0; p != MY_DECK->cpus; ++p) {
            if
            (
              MY_PROC->cpu[p].m_index == infiltrate_m &&
              MY_PROC->cpu[p].p_index == infiltrate_p
            ) {
              Program_Stops(myp, p);
            }
          }
        }

        Remove_From_Memory(myp, infiltrate_m, infiltrate_p);
      }
    } else if
    (
      SYS_SOURCE.index == FIREBOLT ||
      SYS_SOURCE.index == BANSHEE ||
      SYS_SOURCE.index == JOLT
    ) {
      if (SYS_SOURCE.index == FIREBOLT) {
        dmg = RANDOM(glob->firedmg);
      } else if (SYS_SOURCE.index == BANSHEE) {
        dmg = RANDOM(glob->bandmg) + RANDOM(glob->bandmg);
      } else if (SYS_SOURCE.index == JOLT) {
        dmg = RANDOM(glob->joltdmg);
      }

      dmg = Armor_Check(s, i, myp, dmg);

      if (me.perm[GAMEOP_PERM]) {
        dmg = 0;
      }

      me.damage += dmg;
      sprintf(str, getmsg(MSG67), SYS_SOURCE.name, dmg);
      Send(str, myp, FALSE);

      if (me.damage >= glob->dpts) {
        strcpy(str, getmsg(MSG15));
        Send(str, myp, FALSE);
        booted = TRUE;
      } else {
        Update_Location();
        booted = FALSE;
      }
    } else if (SYS_SOURCE.index == MIMIC) {
      booted = FALSE;
    } else if
    (
      SYS_SOURCE.index == BRAINDRAIN ||
      SYS_SOURCE.index == LOBOTOMY
    ) {
      if (me.score) {
        if (SYS_SOURCE.index == LOBOTOMY) {
          dmg = SYS_SOURCE.strength * 2;
        } else {
          dmg = SYS_SOURCE.strength;
        }

        dmg = Armor_Check(s, i, myp, dmg);

        if (dmg && !me.perm[GAMEOP_PERM]) {
          me.score -= dmg;

          if (me.score < 0) {
            me.score = 0;
          }

          sprintf(str, getmsg(MSG68), SYS_SOURCE.name);
          Send(str, myp, FALSE);
          Update_Location();
        }
      }

      booted = FALSE;
    } else if (SYS_SOURCE.index == SURGE) {
      sprintf(str, getmsg(MSG69), SYS_SOURCE.name);
      Send(str, myp, FALSE);

      if (!me.perm[GAMEOP_PERM]) {
        me.interface = FALSE;
      }

      booted = TRUE;
    }

    if (SYS_SOURCE.index == BRAINDRAIN || SYS_SOURCE.index == LOBOTOMY) {
      if (me.score == 0) {
        Program_Stops(s, i);
      } else {
        SYS_PROGRAM.enemy[myp] = FALSE;
        SYS_PROGRAM.sees[myp] = FALSE;
      }
    } else if (SYS_SOURCE.class != HYDRA) {
      Server_Update();
      Program_Stops(s, i);
    }

    if (booted && !me.perm[GAMEOP_PERM]) {
      Jack_Out();
    }

    if (zapped && !me.perm[GAMEOP_PERM]) {
      Destroy_My_Deck(FALSE);
    }
  }

  EXIT0;
}
/****************************************************************************
	anti-ic programs makes a move on a program
*/
short EXPORT Program_Seeker(short s, short i) {
  short   sys_num;
  short   cpu_num;
  short   n;
  short   M;
  short   P;
  short   guy_roll;
  short   my_roll;
  short   nailed;
  short   in_system = FALSE;
  short   dmg;
  short   value;
  short   stuck;
  short   score = 0;
  short   m_num = SYS_PROGRAM.m_index;
  short   p_num = SYS_PROGRAM.p_index;
  short   strength;
  short   x, y;
  short   done;
  short   crashed;
  short   gang_score = 0;
  sys_num = SYS_PROGRAM.target_s;
  cpu_num = SYS_PROGRAM.target_index;
  M = SEEK_PROGRAM.m_index;

  if (M == -1) {
    Program_Stops(s, i);
    EXIT0;
  }

  if (SYS_PROGRAM.loading > 0) {
    EXIT0;
  }

  P = SEEK_PROGRAM.p_index;

  if (s < max_guys) {
    Proc[sys_num]->enemy[s] = TRUE;
  }

  if
  (
    SYS_PROGRAM.net_x != SEEK_PROGRAM.net_x ||
    SYS_PROGRAM.net_y != SEEK_PROGRAM.net_y
  ) {
    Program_Stops(s, i);
    EXIT0;
  }

  if (SYS_PROGRAM.sys_x >= 0 && SYS_PROGRAM.sys_y >= 0) {
    n = Runner_System_Number(s, i);

    if (n < 0) {
      Program_Stops(s, i);
      EXIT0;
    }

    if (SEEK_PROGRAM.sys_x < 0 || SEEK_PROGRAM.sys_y < 0) {
      Program_Stops(s, i);
      EXIT0;
    }

    in_system = TRUE;
  } else if (SEEK_PROGRAM.sys_x >= 0 || SEEK_PROGRAM.sys_y >= 0) {
    Program_Stops(s, i);
    EXIT0;
  }

  value = SEEK_SOURCE.value;

  if (Hydra_Has(CLOAK, sys_num, cpu_num)) {
    guy_roll = SEEK_SOURCE.strength + RANDOM(glob->fate);
    my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

    if (guy_roll > my_roll) {
      EXIT0;
    }
  }

  if (Hydra_Has(MUFFLER, sys_num, cpu_num)) {
    guy_roll = SEEK_SOURCE.strength + RANDOM(glob->fate);
    my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

    if (guy_roll > my_roll) {
      EXIT0;
    }
  }

  if (in_system) {
    nailed = Sys_Track
             (
               n,
               &SYS_PROGRAM,
               SEEK_PROGRAM.sys_x,
               SEEK_PROGRAM.sys_y,
               s,
               &stuck
             );
  } else {
    nailed = City_Track
             (
               &SYS_PROGRAM,
               SEEK_PROGRAM.city_x,
               SEEK_PROGRAM.city_y,
               s
             );
  }

  if (!nailed) {
    EXIT0;
  }

  Server_Update();
  guy_roll = SEEK_SOURCE.strength + RANDOM(glob->fate);
  my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

  if (SYS_SOURCE.index == HERCULES) {
    my_roll += HERCULES_BONUS;
  }

  if (guy_roll > my_roll) {
    if
    (
      guy_roll >= my_roll * 3 &&
      (SEEK_SOURCE.class == ICE || SEEK_SOURCE.class == HYDRA)
    ) {
      if (SYS_SOURCE.strength > 1) {
        sprintf
        (
          str,
          glob->m70b,
          SEEK_SOURCE.name,
          SYS_SOURCE.name
        );
      } else {
        sprintf
        (
          str,
          glob->m70c,
          SEEK_SOURCE.name,
          SYS_SOURCE.name
        );
      }

      --SYS_SOURCE.strength;
    } else {
      sprintf(str, glob->m70a, SYS_SOURCE.name, SEEK_SOURCE.name);
    }

    Send(str, s, FALSE);
    Send(str, sys_num, FALSE);
    strength = SYS_SOURCE.strength;
    Program_Stops(s, i);

    if (strength <= 0) {
      Remove_From_Memory(s, m_num, p_num);
    }

    EXIT0;
  }

  if (SYS_SOURCE.index == HERCULES) {
    nailed = TRUE;
  } else {
    nailed = Hydra_Defense_Check(s, i);
  }

  if (!nailed) {
    EXIT0;
  }

  if (SYS_SOURCE.index == ADDLE) {
    sprintf
    (
      str,
      glob->ms71,
      SYS_SOURCE.name,
      SEEK_SOURCE.name,
      Sys[sys_num]->Name
    );
    Send(str, s, FALSE);
    sprintf
    (
      str,
      glob->ms73,
      SEEK_SOURCE.name,
      SYS_SOURCE.name,
      Runner_System_Name(s, i),
      cpu_num + 1
    );
    Send(str, sys_num, FALSE);
    Program_Stops(sys_num, cpu_num);
    dmg = glob->addlesec * SYS_SOURCE.strength;
    Sys[sys_num]->crash_time[cpu_num] = dmg;
    Program_Stops(s, i);

    for (x = done = 0; x != MAX_SYS_X && !done; ++x) {
      for (y = 0; y != MAX_SYS_Y && !done; ++y) {
        if
        (
          Sys[sys_num]->node_type[x][y] == CPU &&
          Sys[sys_num]->node_index[x][y] == cpu_num
        ) {
          Quick_Update
          (
            SYSTEM_VIEW,
            Sys[sys_num]->net_x, Sys[sys_num]->net_y,
            Sys[sys_num]->city_x, Sys[sys_num]->city_y,
            x, y,
            0, 0
          );
          done = TRUE;
        }
      }
    }

    crashed = Crash_Check(sys_num, &score);

    if
    (
      s < max_guys &&
      crashed &&
      Port[s]->guy.mission_type == CRASH_MISSION &&
      Port[s]->guy.mission_x == Sys[sys_num]->net_x &&
      Port[s]->guy.mission_y == Sys[sys_num]->net_y &&
      sameas(Port[s]->guy.mission_system, Sys[sys_num]->Name) &&
      Sys[sys_num]->type != CYBERDECK
    ) {
      strcpy(str, getmsg(MSG729));
      Send(str, s, TRUE);
      Scored(s, Port[s]->guy.mission_score * MISSION_SCORE, 1);
      Add_Bank_Record
      (
        &Port[s]->guy,
        (long) (Port[s]->guy.mission_score * MISSION_BUX),
        Sys[sys_num]->net_x, Sys[sys_num]->net_y,
        Port[s]->guy.alias,
        "Mission"
      );
      Port[s]->guy.mission_type = 0;
    }

    EXIT0;
  }

  if (SYS_SOURCE.index == HERCULES) {
    dmg = SEEK_SOURCE.strength;
  } else {
    dmg = RANDOM(SYS_SOURCE.strength);
    dmg = Hydra_Armor_Check(s, i, dmg);
  }

  if (dmg > SEEK_SOURCE.strength) {
    dmg = SEEK_SOURCE.strength;
  }

  SEEK_SOURCE.strength -= dmg;

  if (SEEK_SOURCE.strength <= 0) {
    nailed = TRUE;
    sprintf
    (
      str,
      glob->m71,
      SYS_SOURCE.name,
      SEEK_SOURCE.name,
      Runner_System_Name(s, i)
    );
    Send(str, s, FALSE);
    sprintf
    (
      str,
      glob->m73,
      SEEK_SOURCE.name,
      SYS_SOURCE.name,
      Runner_System_Name(s, i)
    );
    Send(str, sys_num, FALSE);
  } else {
    nailed = FALSE;
    sprintf
    (
      str,
      glob->m72,
      SYS_SOURCE.name,
      SEEK_SOURCE.name,
      Runner_System_Name(s, i)
    );
    Send(str, s, FALSE);
    sprintf
    (
      str,
      glob->m74,
      SEEK_SOURCE.name,
      SYS_SOURCE.name,
      Runner_System_Name(s, i)
    );
    Send(str, sys_num, FALSE);
  }

  Program_Stops(sys_num, cpu_num);

  if (nailed) {
    Remove_From_Memory(sys_num, M, P);
    gang_score = 1;
  }

  if (s < max_guys && value && dmg) {
    Scored(s, dmg, gang_score);
  }

  Program_Stops(s, i);
  EXIT0;
}
/****************************************************************************
	see if one of my other radars already detects target
*/
short EXPORT Already_Detected(short s, short r, short g, short p) {
  short   i;

  for (i = 0; i != Sys[s]->cpus; ++i) {
    if
    (
      i != r &&
      SYS_PROGRAM.m_index >= 0 &&
      SYS_SOURCE.index == RADAR &&
      SYS_PROGRAM.sees_hydra_s == g &&
      SYS_PROGRAM.sees_hydra_c == p
    ) {
      return (TRUE);
    }
  }

  return (FALSE);
}
/****************************************************************************
	see if one of my other radars already detects target guy
*/
short EXPORT Guy_Already_Detected(short s, short r, short g) {
  short   i;

  for (i = 0; i != Sys[s]->cpus; ++i) {
    if
    (
      i != r &&
      SYS_PROGRAM.m_index >= 0 &&
      SYS_SOURCE.index == RADAR &&
      SYS_PROGRAM.sees[g]
    ) {
      return (TRUE);
    }
  }

  return (FALSE);
}
/****************************************************************************
	radar tries to detect cloaked netrunners
*/
short EXPORT Radar(short s, short i) {
  short   g;
  short   p;
  short   cloaked;
  short   guy_roll;
  short   my_roll;
  short   was_seen;
  short   in_system;
  short   done;
  short   is_local;
  short   do_update;

  if (Port[s]->access_level == IDLE) {
    EXIT0;
  }

  if (Port[s]->access_level == SYSTEM_VIEW) {
    in_system = TRUE;
  } else {
    in_system = FALSE;
  }

  for (g = 0; g != max_guys; ++g) {
    is_local = FALSE;
    was_seen = SYS_PROGRAM.sees[g];
    SYS_PROGRAM.sees[g] = FALSE;

    if
    (
      s != g &&
      Port[g]->active &&
      Port[g]->access_level == Port[s]->access_level &&
      Port[g]->net_x == Port[s]->net_x &&
      Port[g]->net_y == Port[s]->net_y
    ) {
      is_local = TRUE;

      if
      (
        Port[g]->access_level == SYSTEM_VIEW &&
        (Port[g]->city_x != Port[s]->city_x ||
         Port[g]->city_y != Port[s]->city_y)
      ) {
        is_local = FALSE;
      }
    }

    if (is_local) {
      for (p = 0, cloaked = FALSE; p != Sys[g]->cpus; ++p) {
        if
        (
          GUY_PROGRAM.m_index >= 0 &&
          GUY_SOURCE.index == CLOAK
        ) {
          guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
          my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

          if (guy_roll > my_roll) {
            cloaked = TRUE;
          }
        }
      }

      if (!cloaked) {
        SYS_PROGRAM.sees[g] = TRUE;

        if (!was_seen && !Guy_Already_Detected(s, i, g)) {
          sprintf(str, glob->m75, him.alias, SYS_SOURCE.name);
          Send(str, s, FALSE);
        }
      }
    }

    if (was_seen && !SYS_PROGRAM.sees[g]) {
      sprintf(str, glob->m76, SYS_SOURCE.name, him.alias);
      Send(str, s, FALSE);
    }

    if (SYS_PROGRAM.sees[g] != was_seen && s < max_guys) {
      if (in_system) {
        Quick_Update
        (
          Port[s]->access_level,
          Port[s]->net_x, Port[s]->net_y,
          Port[s]->city_x, Port[s]->city_y,
          Port[g]->sys_x, Port[g]->sys_y,
          0, 0
        );
      } else {
        Quick_Update
        (
          Port[s]->access_level,
          Port[s]->net_x, Port[s]->net_y,
          Port[s]->city_x, Port[s]->city_y,
          Port[g]->city_x, Port[g]->city_y,
          0, 0
        );
      }
    }
  }

  /* see if user's RADAR detects a CLOAKed HYDRA */

  for (g = done = 0; g != max_systems && !done; ++g) {
    if
    (
      g != s &&
      (
        Sys[g]->in_use ||
        (g < max_guys && Port[g]->access_level != IDLE)
      )
    ) {
      for (p = 0; p != Sys[g]->cpus && !done; ++p) {
        if
        (
          GUY_PROGRAM.m_index >= 0 &&
          GUY_SOURCE.class == HYDRA
        ) {
          is_local = FALSE;

          if
          (
            GUY_PROGRAM.net_x == Port[s]->net_x &&
            GUY_PROGRAM.net_y == Port[s]->net_y
          ) {
            if
            (
              in_system &&
              GUY_PROGRAM.city_x == Port[s]->city_x &&
              GUY_PROGRAM.city_y == Port[s]->city_y &&
              GUY_PROGRAM.sys_x != -1 &&
              GUY_PROGRAM.sys_y != -1
            ) {
              is_local = TRUE;
            } else if
            (
              !in_system &&
              GUY_PROGRAM.sys_x == -1 &&
              GUY_PROGRAM.sys_y == -1
            ) {
              is_local = TRUE;
            }
          }

          if (is_local) {
            if
            (
              Hydra_Has(CLOAK, g, p) &&
              !Already_Detected(s, i, g, p)
            ) {
              if
              (
                SYS_PROGRAM.sees_hydra_s == g &&
                SYS_PROGRAM.sees_hydra_c == p
              ) {
                was_seen = TRUE;
              } else {
                was_seen = FALSE;
              }

              guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
              my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

              if (guy_roll > my_roll) {
                cloaked = TRUE;
              } else {
                cloaked = FALSE;
              }

              do_update = FALSE;

              if (!cloaked) {
                SYS_PROGRAM.sees_hydra_s = g;
                SYS_PROGRAM.sees_hydra_c = p;

                if (!was_seen) {
                  sprintf
                  (
                    str,
                    glob->m75,
                    GUY_SOURCE.name,
                    SYS_SOURCE.name
                  );
                  Send(str, s, FALSE);
                  do_update = TRUE;
                }

                done = TRUE;
              } else if (was_seen && cloaked) {
                SYS_PROGRAM.sees_hydra_s = -1;
                SYS_PROGRAM.sees_hydra_c = -1;
                sprintf
                (
                  str,
                  glob->m76,
                  SYS_SOURCE.name,
                  GUY_SOURCE.name
                );
                Send(str, s, FALSE);
                do_update = TRUE;
              }

              if (do_update && s < max_guys) {
                if (in_system) {
                  Quick_Update
                  (
                    Port[s]->access_level,
                    Port[s]->net_x, Port[s]->net_y,
                    Port[s]->city_x, Port[s]->city_y,
                    GUY_PROGRAM.sys_x, GUY_PROGRAM.sys_y,
                    0, 0
                  );
                } else {
                  Quick_Update
                  (
                    Port[s]->access_level,
                    Port[s]->net_x, Port[s]->net_y,
                    Port[s]->city_x, Port[s]->city_y,
                    GUY_PROGRAM.city_x,
                    GUY_PROGRAM.city_y,
                    0, 0
                  );
                }
              }
            }
          }
        }
      }
    }
  }

  if (!done) {
    SYS_PROGRAM.sees_hydra_s = -1;
    SYS_PROGRAM.sees_hydra_c = -1;
  }

  EXIT0;
}
/***************************************************************************
	Virus tries to find a target
*/
short EXPORT Virus(short s, short i) {
  short   targets = FALSE;
  short   score;
  short   m;
  short   x, y;
  short   diff;
  short   g = Runner_System_Number(s, i);
  short   p;
  short   f;
  Proc[g]->infected = TRUE;

  for (x = SYS_PROGRAM.sys_x - 1; x <= SYS_PROGRAM.sys_x + 1; ++x) {
    for (y = SYS_PROGRAM.sys_y - 1; y <= SYS_PROGRAM.sys_y + 1; ++y) {
      diff = abs(SYS_PROGRAM.sys_x - x);
      diff += abs(SYS_PROGRAM.sys_y - y);

      if
      (
        x >= 0 && x < MAX_SYS_X &&
        y >= 0 && y < MAX_SYS_Y &&
        diff == 1
      ) {
        if (Sys[g]->node_type[x][y] == MEMORY) {
          m = Sys[g]->node_index[x][y];

          for (p = 0, f = -1; p != MU_CAPACITY && f < 0; ++p) {
            if
            (
              Sys[g]->memory[m][p].type &&
              !Sys[g]->memory[m][p].running
            ) {
              f = p;
            }
          }

          if (f >= 0) {
            sprintf
            (
              str,
              getmsg(MSG77),
              SYS_SOURCE.name,
              Sys[g]->memory[m][f].name
            );
            Send(str, s, FALSE);

            if (Sys[g]->memory[m][f].type == DOCUMENT) {
              score = Sys[g]->memory[m][f].class;
            } else if (Sys[g]->memory[m][f].value) {
              score = 1;
            } else {
              score = 0;
            }

            Remove_From_Memory(g, m, f);
            SYS_PROGRAM.loading = glob->virsecs;

            if (score && s < max_guys) {
              if
              (
                Port[s]->guy.mission_type == ERASE_MISSION &&
                Port[s]->guy.mission_x == Sys[g]->net_x &&
                Port[s]->guy.mission_y == Sys[g]->net_y &&
                sameas(Port[s]->guy.mission_system, Sys[g]->Name) &&
                Sys[g]->type != CYBERDECK
              ) {
                strcpy(str, getmsg(MSG729));
                Send(str, s, TRUE);
                Scored
                (
                  s,
                  Port[s]->guy.mission_score * MISSION_SCORE,
                  1
                );
                Add_Bank_Record
                (
                  &Port[s]->guy,
                  (long) (Port[s]->guy.mission_score * MISSION_BUX),
                  Sys[g]->net_x, Sys[g]->net_y,
                  Port[s]->guy.alias,
                  "Mission"
                );
                Port[s]->guy.mission_type = 0;
              } else {
                Scored(s, score, 1);
              }
            }

            EXIT0;
          }

          SYS_PROGRAM.seek_x = -1;
          SYS_PROGRAM.seek_y = -1;
        }
      }
    }
  }

  if (SYS_PROGRAM.seek_x != -1 && SYS_PROGRAM.seek_y != -1) {
    x = SYS_PROGRAM.seek_x;
    y = SYS_PROGRAM.seek_y;

    if (Sys[g]->node_type[x][y] == MEMORY) {
      m = Sys[g]->node_index[x][y];

      for (p = 0, f = -1; p != MU_CAPACITY && f < 0; ++p) {
        if
        (
          Sys[g]->memory[m][p].type &&
          !Sys[g]->memory[m][p].running
        ) {
          f = p;
        }
      }

      if (f >= 0) {
        System_Track(s, i, x, y);
        EXIT0;
      }
    }

    SYS_PROGRAM.seek_x = -1;
    SYS_PROGRAM.seek_y = -1;
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      if (Sys[g]->node_type[x][y] == MEMORY) {
        m = Sys[g]->node_index[x][y];

        for (p = 0, f = -1; p != MU_CAPACITY && f < 0; ++p) {
          if
          (
            Sys[g]->memory[m][p].type &&
            !Sys[g]->memory[m][p].running
          ) {
            f = p;
          }
        }

        if (f >= 0) {
          targets = TRUE;

          if (RANDOM(Sys[g]->mus) == 1) {
            SYS_PROGRAM.seek_x = x;
            SYS_PROGRAM.seek_y = y;
            System_Track(s, i, x, y);
            EXIT0;
          }
        }
      }
    }
  }

  if (!targets) {
    Program_Stops(s, i);
  }

  EXIT0;
}
/***************************************************************************
	Mason tries to find a dead wall
*/
short EXPORT Mason(short s, short i) {
  short   x, y;
  short   diff;
  short   n = Runner_System_Number(s, i);

  if (n < 0 || n >= max_systems) {
    Program_Stops(s, i);
    EXIT0;
  }

  for (x = SYS_PROGRAM.sys_x - 1; x <= SYS_PROGRAM.sys_x + 1; ++x) {
    for (y = SYS_PROGRAM.sys_y - 1; y <= SYS_PROGRAM.sys_y + 1; ++y) {
      diff = abs(SYS_PROGRAM.sys_x - x);
      diff += abs(SYS_PROGRAM.sys_y - y);

      if
      (
        x >= 0 && x < MAX_SYS_X &&
        y >= 0 && y < MAX_SYS_Y &&
        diff == 1
      ) {
        if (Sys[n]->node_type[x][y] == DEAD_WALL) {
          Sys[n]->node_type[x][y] = DATA_WALL;
          sprintf(str, getmsg(MSG78), SYS_SOURCE.name);
          Send(str, s, FALSE);
          Quick_Update
          (
            SYSTEM_VIEW,
            SYS_PROGRAM.net_x, SYS_PROGRAM.net_y,
            SYS_PROGRAM.city_x, SYS_PROGRAM.city_y,
            x, y,
            0, 0
          );
          EXIT0;
        }
      }
    }
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      if (Sys[n]->node_type[x][y] == DEAD_WALL) {
        System_Track(s, i, x, y);
        EXIT0;
      }
    }
  }

  Sys[n]->breached = FALSE;

  if (s >= max_guys) {
    Program_Stops(s, i);
  }

  EXIT0;
}
/***************************************************************************
	track in a system
*/
short EXPORT System_Track(short s, short i, short x, short y) {
  short   dir;
  short   next = 0;
  short   ok;
  short   x1, y1;
  short   n = Runner_System_Number(s, i);
  short   old_x = SYS_PROGRAM.sys_x;
  short   old_y = SYS_PROGRAM.sys_y;
  x1 = abs(SYS_PROGRAM.sys_x - x);
  y1 = abs(SYS_PROGRAM.sys_y - y);

  if (SYS_SOURCE.index != FENCE) {
    if ((x1 == 0 && y1 == 1) || (x1 == 1 && y1 == 0)) {
      EXIT0;
    }
  } else if (x1 == 0 && y1 == 0) {
    EXIT0;
  }

  dir = Find_System_Route
        (
          n,
          SYS_PROGRAM.sys_x, SYS_PROGRAM.sys_y,
          x, y,
          s,
          &next,
          SYS_SOURCE.index,
          SYS_SOURCE.strength
        );

  if (dir) {
    if (dir == NORTH) {
      --SYS_PROGRAM.sys_y;
    } else if (dir == SOUTH) {
      ++SYS_PROGRAM.sys_y;
    } else if (dir == EAST) {
      ++SYS_PROGRAM.sys_x;
    } else if (dir == WEST) {
      --SYS_PROGRAM.sys_x;
    }

    x1 = abs(SYS_PROGRAM.sys_x - x);
    y1 = abs(SYS_PROGRAM.sys_y - y);
    ok = FALSE;

    if (SYS_SOURCE.index != FENCE) {
      if ((x1 == 0 && y1 == 1) || (x1 == 1 && y1 == 0)) {
        ok = TRUE;
      }
    } else if (x1 == 0 && y1 == 0) {
      ok = TRUE;
    }

    if
    (
      !ok &&
      next &&
      Sys[s]->speed >= QUICK_SPEED
    ) {
      if (next == NORTH) {
        --SYS_PROGRAM.sys_y;
      } else if (next == SOUTH) {
        ++SYS_PROGRAM.sys_y;
      } else if (next == EAST) {
        ++SYS_PROGRAM.sys_x;
      } else if (next == WEST) {
        --SYS_PROGRAM.sys_x;
      }
    }

    x1 = SYS_PROGRAM.sys_x;
    y1 = SYS_PROGRAM.sys_y;

    if (SYS_SOURCE.index == FENCE) {
      if ((x1 == 0 && y1 == 1) || (x1 == 1 && y1 == 0)) {
        SYS_PROGRAM.sys_x = x;
        SYS_PROGRAM.sys_y = y;
      }
    }

    Quick_Update
    (
      SYSTEM_VIEW,
      SYS_PROGRAM.net_x, SYS_PROGRAM.net_y,
      SYS_PROGRAM.city_x, SYS_PROGRAM.city_y,
      old_x, old_y,
      SYS_PROGRAM.sys_x, SYS_PROGRAM.sys_y
    );
    EXIT0;
  }

  if (s < max_guys || RANDOM(20) != 1) {
    SYS_PROGRAM.seek_x = -1;
    SYS_PROGRAM.seek_y = -1;
    SYS_PROGRAM.loading = RANDOM(5);
  } else {
    Program_Stops(s, i);
  }

  EXIT0;
}
/***************************************************************************
	Snout tries to find a mole
*/
short EXPORT Snout(short s, short i) {
  short   g;
  short   p;
  short   x, y;
  short   diff;
  short   mole_m;
  short   mole_p;
  struct  cpu_struct *runner;

  for (x = SYS_PROGRAM.sys_x - 1; x <= SYS_PROGRAM.sys_x + 1; ++x) {
    for (y = SYS_PROGRAM.sys_y - 1; y <= SYS_PROGRAM.sys_y + 1; ++y) {
      diff = abs(SYS_PROGRAM.sys_x - x);
      diff += abs(SYS_PROGRAM.sys_y - y);

      if
      (
        x >= 0 && x < MAX_SYS_X &&
        y >= 0 && y < MAX_SYS_Y &&
        diff == 1
      ) {
        runner = Find_Program
                 (
                   SYS_PROGRAM.net_x,
                   SYS_PROGRAM.net_y,
                   SYS_PROGRAM.city_x,
                   SYS_PROGRAM.city_y,
                   x, y,
                   &g, &p
                 );

        if (runner != NULL && GUY_SOURCE.index == MOLE) {
          sprintf
          (
            str,
            getmsg(MSG79),
            SYS_SOURCE.name,
            GUY_SOURCE.name
          );
          Send(str, s, FALSE);
          Send(str, g, FALSE);
          mole_m = GUY_PROGRAM.m_index;
          mole_p = GUY_PROGRAM.p_index;
          Program_Stops(g, p);
          Remove_From_Memory(g, mole_m, mole_p);
          EXIT0;
        }
      }
    }
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      runner = Find_Program
               (
                 SYS_PROGRAM.net_x,
                 SYS_PROGRAM.net_y,
                 SYS_PROGRAM.city_x,
                 SYS_PROGRAM.city_y,
                 x, y,
                 &g, &p
               );

      if (runner != NULL && GUY_SOURCE.index == MOLE) {
        System_Track(s, i, x, y);
        EXIT0;
      }
    }
  }

  Proc[s]->moled = FALSE;

  if (s >= max_guys) {
    Program_Stops(s, i);
  }

  EXIT0;
}
/***************************************************************************
	Patriot tries to find a torpedo
*/
short Patriot(short s, short i) {
  short   g;
  short   p;
  short   x, y;
  short   diff;
  short   torp_m;
  short   torp_p;
  struct  cpu_struct *runner;

  for (x = SYS_PROGRAM.sys_x - 1; x <= SYS_PROGRAM.sys_x + 1; ++x) {
    for (y = SYS_PROGRAM.sys_y - 1; y <= SYS_PROGRAM.sys_y + 1; ++y) {
      diff = abs(SYS_PROGRAM.sys_x - x);
      diff += abs(SYS_PROGRAM.sys_y - y);

      if
      (
        x >= 0 && x < MAX_SYS_X &&
        y >= 0 && y < MAX_SYS_Y &&
        diff == 1
      ) {
        runner = Find_Program
                 (
                   SYS_PROGRAM.net_x,
                   SYS_PROGRAM.net_y,
                   SYS_PROGRAM.city_x,
                   SYS_PROGRAM.city_y,
                   x, y,
                   &g, &p
                 );

        if (runner != NULL && GUY_SOURCE.index == TORPEDO) {
          sprintf
          (
            str,
            getmsg(MSG79),
            SYS_SOURCE.name,
            GUY_SOURCE.name
          );
          Send(str, s, FALSE);
          Send(str, g, FALSE);
          torp_m = GUY_PROGRAM.m_index;
          torp_p = GUY_PROGRAM.p_index;
          Program_Stops(g, p);
          Remove_From_Memory(g, torp_m, torp_p);
          EXIT0;
        }
      }
    }
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      runner = Find_Program
               (
                 SYS_PROGRAM.net_x,
                 SYS_PROGRAM.net_y,
                 SYS_PROGRAM.city_x,
                 SYS_PROGRAM.city_y,
                 x, y,
                 &g, &p
               );

      if (runner != NULL && GUY_SOURCE.index == TORPEDO) {
        System_Track(s, i, x, y);
        EXIT0;
      }
    }
  }

  Proc[s]->torped = FALSE;

  if (s >= max_guys) {
    Program_Stops(s, i);
  }

  EXIT0;
}
/***************************************************************************
	AntiVirus tries to find a virus to destroy
*/
short EXPORT Anti_Virus(short s, short i) {
  short   g;
  short   p;
  short   x, y;
  short   diff;
  short   virus_m;
  short   virus_p;
  struct  cpu_struct *runner;

  for (x = SYS_PROGRAM.sys_x - 1; x <= SYS_PROGRAM.sys_x + 1; ++x) {
    for (y = SYS_PROGRAM.sys_y - 1; y <= SYS_PROGRAM.sys_y + 1; ++y) {
      diff = abs(SYS_PROGRAM.sys_x - x);
      diff += abs(SYS_PROGRAM.sys_y - y);

      if
      (
        x >= 0 && x < MAX_SYS_X &&
        y >= 0 && y < MAX_SYS_Y &&
        diff == 1
      ) {
        runner = Find_Program
                 (
                   SYS_PROGRAM.net_x,
                   SYS_PROGRAM.net_y,
                   SYS_PROGRAM.city_x,
                   SYS_PROGRAM.city_y,
                   x, y,
                   &g, &p
                 );

        if (runner != NULL && GUY_SOURCE.index == VIRUS) {
          sprintf
          (
            str,
            getmsg(MSG79),
            SYS_SOURCE.name,
            GUY_SOURCE.name
          );
          Send(str, s, FALSE);
          Send(str, g, FALSE);
          virus_m = GUY_PROGRAM.m_index;
          virus_p = GUY_PROGRAM.p_index;
          Program_Stops(g, p);
          Remove_From_Memory(g, virus_m, virus_p);
          EXIT0;
        }
      }
    }
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      runner = Find_Program
               (
                 SYS_PROGRAM.net_x,
                 SYS_PROGRAM.net_y,
                 SYS_PROGRAM.city_x,
                 SYS_PROGRAM.city_y,
                 x, y,
                 &g, &p
               );

      if (runner != NULL && GUY_SOURCE.index == VIRUS) {
        System_Track(s, i, x, y);
        EXIT0;
      }
    }
  }

  Proc[s]->infected = FALSE;

  if (s >= max_guys) {
    Program_Stops(s, i);
  }

  EXIT0;
}
