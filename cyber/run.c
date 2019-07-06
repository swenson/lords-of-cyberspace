#include <header.h>
#include <globals.h>

/**************************************************************************
	see if I specified a program to run
*/
short EXPORT Ran_Program(short m, short p, short *cmd_matches, char *next) {
  short   strength = 0;
  short   m_index = -1;
  short   p_index = -1;
  short   p_num   = -1;
  short   class;
  short   running = -1;
  short   packed = -1;
  short   locked = -1;
  short   old_p;
  short   old_m;
  short   unique = TRUE;
  short   spaces;
  short   n;
  short   i;
  short   j;
  short   match = 0;

  if (m != -1 && p != -1) {
    STATE = MAIN_PROMPT;

    if (MY_DECK->memory[m][p].type != PROGRAM) {
      EXIT0;
    }

    if (MY_DECK->memory[m][p].running) {
      running = TRUE;
      old_m = m;
      old_p = p;
    } else if (MY_DECK->memory[m][p].lock) {
      locked = TRUE;
      old_m = m;
      old_p = p;
    } else if (MY_DECK->memory[m][p].packed) {
      packed = TRUE;
      old_m = m;
      old_p = p;
    }

    p_num = MY_DECK->memory[m][p].index;
    m_index = m;
    p_index = p;
  } else {
    for (i = 0; i != NUM_PROGRAMS; ++i) {
      glob->used[i] = 0;
    }

    for (j = 0; j != MY_DECK->mus; ++j) {
      for (i = 0; i != MU_CAPACITY; ++i) {
        if (MY_DECK->memory[j][i].type == PROGRAM) {
          n = MY_DECK->memory[j][i].index;

          if (sameas(MY_DECK->memory[j][i].name, margv[0])) {
            if (!glob->used[n]) {
              ++glob->used[n];
              ++*cmd_matches;

              if (next[0]) {
                if (*cmd_matches != 2) {
                  strcat(MY_PORT->unique, ",");
                }

                strcat(MY_PORT->unique, " ");
                strcat(MY_PORT->unique, next);
                next[0] = 0;
              }

              strcpy(next, MY_DECK->memory[j][i].name);
            }

            if (MY_DECK->memory[j][i].running) {
              if (running != FALSE) {
                p_num = n;
                running = TRUE;
                old_p = i;
                old_m = j;
              }
            } else if (MY_DECK->memory[j][i].lock) {
              if (locked != FALSE) {
                p_num = n;
                locked = TRUE;
                old_p = i;
                old_m = j;
              }
            } else if (MY_DECK->memory[j][i].packed) {
              if (packed != FALSE) {
                p_num = n;
                packed = TRUE;
                old_p = i;
                old_m = j;
              }
            } else if (MY_DECK->memory[j][i].strength > strength) {
              strength = MY_DECK->memory[j][i].strength;
              p_num = n;
              m_index = j;
              p_index = i;
              running = FALSE;
              packed = FALSE;
              locked = FALSE;
            }
          } else if (sameto(margv[0], MY_DECK->memory[j][i].name)) {
            if (!glob->used[n]) {
              ++glob->used[n];
              ++*cmd_matches;

              if (next[0]) {
                if (*cmd_matches != 2) {
                  strcat(MY_PORT->unique, ",");
                }

                strcat(MY_PORT->unique, " ");
                strcat(MY_PORT->unique, next);
                next[0] = 0;
              }

              strcpy(next, MY_DECK->memory[j][i].name);
            }

            if (MY_DECK->memory[j][i].running) {
              if (running != FALSE) {
                p_num = n;
                running = TRUE;
                old_p = i;
                old_m = j;
              }
            } else if (MY_DECK->memory[j][i].lock) {
              if (locked != FALSE) {
                p_num = n;
                locked = TRUE;
                old_p = i;
                old_m = j;
              }
            } else if (MY_DECK->memory[j][i].packed) {
              if (packed != FALSE) {
                p_num = n;
                packed = TRUE;
                old_p = i;
                old_m = j;
              }
            } else {
              ++match;

              if (match == 1) {
                p_num = n;
                m_index = j;
                p_index = i;
                strength = MY_DECK->memory[j][i].strength;
                locked = FALSE;
                packed = FALSE;
                running = FALSE;
              } else if (n != p_num) {
                unique = FALSE;
              } else if (MY_DECK->memory[j][i].strength > strength) {
                p_num = n;
                m_index = j;
                p_index = i;
                strength = MY_DECK->memory[j][i].strength;
                locked = FALSE;
                packed = FALSE;
                running = FALSE;
              }
            }
          }
        }
      }
    }

    if (*cmd_matches > 1) {
      return (FALSE);
    }
  }

  if (running == TRUE) {
    Clear_Display_Window();
    prfmsg(MSG80, MY_DECK->memory[old_m][old_p].name);
    return (TRUE);
  }

  if (locked == TRUE) {
    Clear_Display_Window();
    prfmsg(MSG82, MY_DECK->memory[old_m][old_p].name);
    return (TRUE);
  }

  if (packed == TRUE) {
    Clear_Display_Window();
    prfmsg(MSG83, MY_DECK->memory[old_m][old_p].name);
    return (TRUE);
  }

  if (p_index == -1 || !unique) {
    EXIT0;
  }

  Clear_Display_Window();
  MY_PORT->p_index     = p_index;
  MY_PORT->m_index     = m_index;
  MY_PORT->p_num       = p_num;
  MY_PORT->target_s    = -1;
  MY_PORT->target_c    = -1;
  MY_PORT->target_type = -1;
  MY_PORT->target_x    = -1;
  MY_PORT->target_y    = -1;
  class = MY_DECK->memory[m_index][p_index].class;

  if
  (
    p_num == GATECRASH ||
    p_num == VIRUS ||
    p_num == SNOUT ||
    p_num == PATRIOT ||
    p_num == MASON ||
    p_num == ANTIVIRUS ||
    p_num == CASHIER ||
    p_num == TRACE ||
    p_num == CAMOFLAGE ||
    p_num == FENCE ||
    p_num == CRASH ||
    p_num == BULLDOZER ||
    p_num == TORPEDO ||
    p_num == MOLE
  ) {
    if (MY_PORT->access_level != SYSTEM_VIEW) {
      prfmsg(MSG84, MY_DECK->memory[m_index][p_index].name);
      return (TRUE);
    }
  }

  if (MY_PORT->sys_num < max_guys && MY_PORT->sys_num != myp) {
    if
    (
      p_num == BULLDOZER ||
      p_num == MOLE ||
      p_num == GATECRASH ||
      p_num == CRASH ||
      p_num == BLOB ||
      p_num == VIRUS ||
      p_num == TORPEDO
    ) {
      if
      (
        !me.perm[GAMEOP_PERM] &&
        His_Rank(me.score, me.won) < glob->pvprank
      ) {
        prfmsg(M661, glob->pvprank);
        return (TRUE);
      }

      if
      (
        !me.perm[GAMEOP_PERM] &&
        His_Rank
        (
          Port[MY_PORT->sys_num]->guy.score,
          Port[MY_PORT->sys_num]->guy.won
        ) < glob->pvprank
      ) {
        prfmsg(M662, glob->pvprank);
        return (TRUE);
      }

      if (glob->outlaw && !me.perm[GAMEOP_PERM]) {
        if (!me.outlaw) {
          prfmsg(MSSG670);
          return (TRUE);
        }

        if (!Port[MY_PORT->sys_num]->guy.outlaw) {
          prfmsg(MSSG671);
          return (TRUE);
        }
      }
    }
  }

  if
  (
    p_num == CLOAK ||
    p_num == RADAR ||
    p_num == MUFFLER ||
    p_num == SHIELD ||
    p_num == ARMOR ||
    p_num == CHAINSAW ||
    p_num == SKULLDRILL ||
    p_num == TENTACLE ||
    p_num == CATTLEPROD ||
    p_num == GLUEGUN ||
    p_num == COBRA ||
    p_num == PHASER ||
    p_num == NETSPEED ||
    p_num == STATIK ||
    p_num == KEY1 ||
    p_num == KEY2 ||
    p_num == KEY3
  ) {
    Load_Program();
    return (TRUE);
  }

  if (p_num != UNLOCKER && p_num != LOCKER) {
    if (MY_PORT->access_level == IDLE) {
      prfmsg(MSG85, MY_DECK->memory[m_index][p_index].name);
      return (TRUE);
    }
  }

  if
  (
    class == HYDRA ||
    p_num == SURGE ||
    p_num == LIGHTNING ||
    p_num == PUNTER ||
    p_num == ICEBOLT ||
    p_num == INFILTRATE ||
    p_num == HANDCUFF ||
    p_num == FIREBOLT ||
    p_num == BANSHEE ||
    p_num == BRAINDRAIN ||
    p_num == LOBOTOMY ||
    p_num == JOLT
  ) {
    Prompt(getmsg(MSG86), TARGET_GUY_PROMPT);
    return (TRUE);
  }

  if (p_num == MIMIC) {
    Prompt(getmsg(MSG687), MIMIC_PROMPT);
    return (TRUE);
  }

  if (p_num == KNIGHT || p_num == ADDLE || p_num == HERCULES) {
    Prompt(getmsg(MSG87), TARGET_CONST_PROMPT);
    return (TRUE);
  }

  if
  (
    p_num == ANTIVIRUS ||
    p_num == BARRICADE ||
    p_num == BLOB ||
    p_num == BULLDOZER ||
    p_num == CASHIER ||
    p_num == CRASH ||
    p_num == TORPEDO ||
    p_num == FENCE ||
    p_num == GABLINK ||
    p_num == GATECRASH ||
    p_num == GUARDIAN ||
    p_num == SOLDIER ||
    p_num == MASON ||
    p_num == MOLE ||
    p_num == PROBE ||
    p_num == SENTRY ||
    p_num == SNOUT ||
    p_num == PATRIOT ||
    p_num == TRACE ||
    p_num == CAMOFLAGE ||
    p_num == VIRUS
  ) {
    if (margc > 1) {
      if
      (
        sameto(margv[1], "north") ||
        sameto(margv[1], "south") ||
        sameto(margv[1], "east") ||
        sameto(margv[1], "west")
      ) {
        if (p_num != PROBE) {
          spaces = 1;
        } else {
          spaces = MY_DECK->memory[MY_PORT->m_index][MY_PORT->p_index].strength;
        }

        if (MY_PORT->access_level == SYSTEM_VIEW) {
          MY_PORT->target_type = Find_System_Space
                                 (
                                   margv[1],
                                   &MY_PORT->target_x,
                                   &MY_PORT->target_y,
                                   spaces
                                 );
        } else {
          MY_PORT->target_type = Find_City_Space
                                 (
                                   margv[1],
                                   &MY_PORT->target_x,
                                   &MY_PORT->target_y,
                                   spaces
                                 );
        }

        Load_Program();
        return (TRUE);
      }
    }

    Prompt(getmsg(MSG88), DIRECTION_PROMPT);
    return (TRUE);
  }

  if (p_num == UNLOCKER || p_num == LOCKER) {
    Memory_Summary();
    sprintf(str, getmsg(MSG89), glob->dirs);
    Prompt(str, LOCKER_PROMPT);
    return (TRUE);
  }

  prfmsg
  (
    MSG90,
    MY_DECK->memory[MY_PORT->m_index][MY_PORT->p_index].name
  );
  return (TRUE);
}
/**************************************************************************
	a program stops, clean up after it
*/
short Program_Stops(short s, short i) {
  short   g;
  short   was_cloak = FALSE;
  short   was_radar = FALSE;
  short   in_system = FALSE;
  short   is_local;
  short   n = Runner_System_Number(s, i);

  if (SYS_PROGRAM.m_index < 0 || !SYS_SOURCE.running) {
    EXIT0;
  }

  if (SYS_PROGRAM.sys_x >= 0 && SYS_PROGRAM.sys_y >= 0) {
    in_system = TRUE;

    if (SYS_SOURCE.index == CAMOFLAGE) {
      if (misc_world->city[SYS_PROGRAM.net_x][SYS_PROGRAM.net_y].camoflaged[SYS_PROGRAM.city_x][SYS_PROGRAM.city_y]) {
        --misc_world->city[SYS_PROGRAM.net_x][SYS_PROGRAM.net_y].camoflaged[SYS_PROGRAM.city_x][SYS_PROGRAM.city_y];
      }

      Quick_Update
      (
        CITY_VIEW,
        SYS_PROGRAM.net_x, SYS_PROGRAM.net_y,
        SYS_PROGRAM.city_x, SYS_PROGRAM.city_y,
        SYS_PROGRAM.city_x, SYS_PROGRAM.city_y,
        0, 0
      );
    }
  }

  if (SYS_SOURCE.index == CLOAK) {
    was_cloak = TRUE;
  }

  if (SYS_SOURCE.index == RADAR) {
    was_radar = TRUE;
  }

  if (SYS_SOURCE.index == BLOB) {
    glob->blob[n] = FALSE;
  }

  sprintf(str, glob->m92, SYS_SOURCE.name);

  for (g = 0; g != max_guys; ++g) {
    is_local = FALSE;

    if
    (
      (g != myp || glob->server) &&
      Port[g]->active &&
      Port[g]->access_level != IDLE &&
      Port[g]->net_x == SYS_PROGRAM.net_x &&
      Port[g]->net_y == SYS_PROGRAM.net_y
    ) {
      if
      (
        in_system &&
        Port[g]->city_x == SYS_PROGRAM.city_x &&
        Port[g]->city_y == SYS_PROGRAM.city_y
      ) {
        is_local = TRUE;
      } else if (!in_system && Port[g]->access_level == CITY_VIEW) {
        is_local = TRUE;
      }
    }

    if (is_local) {
      if (SYS_SOURCE.class == HYDRA && Hydra_Has(CLOAK, s, i)) {
        if (Visible_Hydra(g, s, i)) {
          Send(str, g, FALSE);
        }
      } else {
        Send(str, g, FALSE);
      }
    } else if
    (
      g == s &&
      SYS_PROGRAM.net_x != -1 &&
      SYS_PROGRAM.net_y != -1 &&
      SYS_PROGRAM.city_x != -1 &&
      SYS_PROGRAM.city_y != -1
    ) {
      Send(str, g, FALSE);
    }
  }

  if (SYS_SOURCE.index == MIMIC) {
    strcpy(SYS_SOURCE.name, Programs[MIMIC].Name);
  }

  SYS_SOURCE.running = FALSE;
  SYS_PROGRAM.m_index = -1;

  if (SYS_PROGRAM.sys_x >= 0 && SYS_PROGRAM.sys_y >= 0) {
    Quick_Update
    (
      SYSTEM_VIEW,
      SYS_PROGRAM.net_x, SYS_PROGRAM.net_y,
      SYS_PROGRAM.city_x, SYS_PROGRAM.city_y,
      SYS_PROGRAM.sys_x, SYS_PROGRAM.sys_y,
      0, 0
    );
  } else if (SYS_PROGRAM.city_x >= 0 && SYS_PROGRAM.city_y >= 0) {
    Quick_Update
    (
      CITY_VIEW,
      SYS_PROGRAM.net_x, SYS_PROGRAM.net_y,
      SYS_PROGRAM.city_x, SYS_PROGRAM.city_y,
      SYS_PROGRAM.city_x, SYS_PROGRAM.city_y,
      0, 0
    );
  }

  if (was_cloak && !System_Is_Running(s, CLOAK)) {
    Reveal_Guy(s);
  } else if
  (
    was_radar &&
    !System_Is_Running(s, RADAR) &&
    Port[s]->access_level == CITY_VIEW &&
    myp == s
  ) {
    Look("X");
  }

  Close_System_Check(n, 0);
  EXIT0;
}
/**************************************************************************
	stop a program
*/
short Stop_Program(short option) {
  short   i;
  short   done;
  short   stopped = FALSE;

  for (i = done = 0; i != MY_DECK->cpus && !done; ++i) {
    if (MY_PROGRAM.m_index >= 0) {
      done = TRUE;
    }
  }

  if (!done) {
    prfmsg(MSG93);
    EXIT0;
  }

  if (option == MAIN_PROMPT) {
    if (parm1[0] == 0) {
      CPU_Status(myp);
      sprintf(str, getmsg(MSG94), MY_DECK->cpus);
      Prompt(str, STOP_PROMPT);
      EXIT0;
    }
  }

  i = INUM(parm1);

  if (i) {
    if (i > MY_DECK->cpus) {
      prfmsg(MSG95, i);
      EXIT0;
    }

    --i;

    if (MY_PROGRAM.m_index == -1) {
      prfmsg(MSG96, i + 1);
      EXIT0;
    }

    prfmsg(MSG97, MY_SOURCE.name);
    Program_Stops(myp, i);
  } else {
    for (i = 0; i != MY_DECK->cpus && !stopped; ++i) {
      if (MY_PROGRAM.m_index >= 0 && sameto(parm1, MY_SOURCE.name)) {
        stopped = TRUE;
        prfmsg(MSG97, MY_SOURCE.name);
        Program_Stops(myp, i);
      }
    }

    if (!stopped) {
      prfmsg(MSG98, parm1);
      EXIT0;
    }
  }

  EXIT0;
}
/**************************************************************************
	show the programs the user is currently running
*/
void CPU_Status(short s) {
  short   i;
  char    *city_name;
  char    *system_name;

  for (i = 0; i != Sys[s]->cpus; ++i) {
    if (Sys[s]->crash_time[i]) {
      sprintf(str, getmsg(MSG1C), i + 1, Sys[s]->crash_time[i]);
      prf(str);
    } else if (Proc[s]->cpu[i].m_index == -1) {
      prfmsg(MSG1B, i + 1);
    } else {
      prfmsg(MSG1A, i + 1, SYS_PROGRAM.rez, SYS_SOURCE.strength);

      if
      (
        SYS_SOURCE.class == HYDRA ||
        SYS_SOURCE.index == SENTRY ||
        SYS_SOURCE.index == CASHIER ||
        SYS_SOURCE.index == TRACE ||
        SYS_SOURCE.index == CAMOFLAGE ||
        SYS_SOURCE.index == GABLINK ||
        SYS_SOURCE.index == GUARDIAN ||
        SYS_SOURCE.index == SOLDIER ||
        SYS_SOURCE.index == BANSHEE ||
        SYS_SOURCE.index == BARRICADE ||
        SYS_SOURCE.index == FENCE ||
        SYS_SOURCE.index == BLOB
      ) {
        city_name = SYS_CONST_CITY.name;
        system_name = Runner_System_Name(s, i);

        if (SYS_PROGRAM.sys_x < 0 || SYS_PROGRAM.sys_y < 0) {
          prfmsg
          (
            MSG6,
            SYS_SOURCE.name,
            SYS_PROGRAM.loading,
            city_name,
            SYS_PROGRAM.net_x,
            SYS_PROGRAM.net_y
          );
        } else {
          prfmsg
          (
            MSG4,
            SYS_SOURCE.name,
            SYS_PROGRAM.loading,
            city_name,
            SYS_PROGRAM.net_x,
            SYS_PROGRAM.net_y,
            system_name
          );
        }
      } else {
        prfmsg(MSG5, SYS_SOURCE.name, SYS_PROGRAM.loading);
      }
    }

    prf(NL);
  }
}
/****************************************************************************
	a system executes a program
*/
short Run_System_Program(short s, short p_num) {
  short   avail = -1;
  short   abs_match = FALSE;
  short   n;
  short   i;
  short   j;
  short   m_index;
  short   p_index;
  short   tries = 0;

  do {
    ++tries;
    i = RANDOM(Sys[s]->cpus) - 1;

    if (SYS_PROGRAM.m_index == -1 && !Sys[s]->crash_time[i]) {
      avail = i;
    }
  } while (avail < 0 && tries != 10);

  if (avail < 0) {
    for (i = 0; i != Sys[s]->cpus && avail < 0; ++i) {
      if (SYS_PROGRAM.m_index == -1 && !Sys[s]->crash_time[i]) {
        avail = i;
      }
    }
  }

  if (avail < 0) {
    EXIT0;
  }

  for (j = 0; j != Sys[s]->mus && !abs_match; ++j) {
    for (i = 0; i != MU_CAPACITY && !abs_match; ++i) {
      if (Sys[s]->memory[j][i].type == PROGRAM) {
        n = Sys[s]->memory[j][i].index;

        if (n == p_num && !Sys[s]->memory[j][i].running) {
          abs_match = TRUE;
          m_index = j;
          p_index = i;
        }
      }
    }
  }

  if (!abs_match) {
    EXIT0;
  }

  if (Sys[s]->memory[m_index][p_index].packed) {
    Sys[s]->memory[m_index][p_index].packed = FALSE;

    if (Memory_Usage(Sys[s]->memory[m_index]) > MU_CAPACITY) {
      Sys[s]->memory[m_index][p_index].packed = TRUE;
      EXIT0;
    }
  }

  if (Sys[s]->memory[m_index][p_index].lock) {
    Sys[s]->memory[m_index][p_index].lock = FALSE;
  }

  if (Load_System_Program(s, avail, m_index, p_index)) {
    i = avail;
    Quick_Update
    (
      SYSTEM_VIEW,
      SYS_PROGRAM.net_x, SYS_PROGRAM.net_y,
      SYS_PROGRAM.city_x, SYS_PROGRAM.city_y,
      SYS_PROGRAM.sys_x, SYS_PROGRAM.sys_y,
      0, 0
    );
    return (TRUE);
  }

  EXIT0;
}
/****************************************************************************
	I just probed a program
*/
void Probed_Program(short g, short p) {
  short   x;

  if (GUY_SOURCE.index == MIMIC) {
    prfmsg(MSG688, GUY_SOURCE.name, GUY_SOURCE.strength);
  } else if (GUY_SOURCE.class != HYDRA) {
    prfmsg(MSG99, GUY_SOURCE.name, GUY_SOURCE.strength);
  } else {
    prfmsg(MSSG100, GUY_SOURCE.name, GUY_SOURCE.strength);

    for (x = 0; x != MAX_SUBS; ++x) {
      if (GUY_SOURCE.sub[x]) {
        prfmsg(MSG101, Programs[GUY_SOURCE.sub[x]].Name);
      }
    }
  }

  if (Sys[g]->net_x >= 0 && Sys[g]->net_y >= 0) {
    prfmsg
    (
      MSG844,
      Sys[g]->Name,
      world->city[Sys[g]->net_x][Sys[g]->net_y].name
    );
  } else {
    prfmsg(MSG845);
  }
}
/****************************************************************************
	I just probed a guy
*/
void Probed_Guy(short g) {
  short   p;
  prfmsg
  (
    MSG102,
    Port[g]->guy.alias,
    rank[His_Rank(Port[g]->guy.score, Port[g]->guy.won)].title
  );

  for (p = 0; p != Sys[g]->cpus; ++p) {
    if
    (
      GUY_PROGRAM.m_index >= 0 &&
      (
        GUY_SOURCE.index == CLOAK ||
        GUY_SOURCE.index == RADAR ||
        GUY_SOURCE.index == MUFFLER ||
        GUY_SOURCE.index == SHIELD ||
        GUY_SOURCE.index == ARMOR ||
        GUY_SOURCE.index == CHAINSAW ||
        GUY_SOURCE.index == SKULLDRILL ||
        GUY_SOURCE.index == TENTACLE ||
        GUY_SOURCE.index == CATTLEPROD ||
        GUY_SOURCE.index == GLUEGUN ||
        GUY_SOURCE.index == STATIK ||
        GUY_SOURCE.index == PHASER ||
        GUY_SOURCE.index == COBRA ||
        GUY_SOURCE.index == KEY1 ||
        GUY_SOURCE.index == KEY2 ||
        GUY_SOURCE.index == KEY3 ||
        GUY_SOURCE.index == NETSPEED
      )
    ) {
      prfmsg(MSG103, GUY_SOURCE.name, GUY_SOURCE.strength);
    }
  }

  if (Sys[g]->net_x >= 0 && Sys[g]->net_y >= 0) {
    prfmsg
    (
      MSG844,
      Sys[g]->Name,
      world->city[Sys[g]->net_x][Sys[g]->net_y].name
    );
  } else {
    prfmsg(MSG845);
  }

  sprintf(str, getmsg(MSG104), me.alias);
  Send(str, g, FALSE);
}
/****************************************************************************
	I just probed something
*/
short Probed(void) {
  short   i;
  short   g;
  short   p;
  short   n;
  FILE    *fp;
  short   x = MY_PORT->target_x;
  short   y = MY_PORT->target_y;
  short   s = MY_PORT->target_type;
  struct  cpu_struct *runner;

  if (s == -1) {
    prfmsg(MSG110);
    EXIT0;
  }

  if (MY_PORT->access_level == SYSTEM_VIEW) {
    if (MY_SYS->node_type[x][y] == CLOSED_GATE) {
      prfmsg(MSG105, MY_SYS->gate);
    } else if (MY_SYS->node_type[x][y] == PHONE) {
      prfmsg(MSG111, 0);
    } else if (MY_SYS->node_type[x][y] == OPENED_GATE) {
      prfmsg(MSG106, MY_SYS->gate);
    } else if (MY_SYS->node_type[x][y] == DEAD_MEMORY) {
      prfmsg(MSG826);
    } else if (MY_SYS->node_type[x][y] == MEMORY) {
      Memory_Contents(MY_SYS->memory[MY_SYS->node_index[x][y]], 1);
    } else if (MY_SYS->node_type[x][y] == CPU) {
      prfmsg(MSG107, MY_SYS->speed);
      s = MY_PORT->sys_num;
      i = MY_SYS->node_index[x][y];

      if (Proc[s]->cpu[i].m_index != -1) {
        prfmsg(MSG803, SYS_SOURCE.name, SYS_SOURCE.strength);
      }
    } else if (MY_SYS->node_type[x][y] == A_PLAYER) {
      for (g = 0; g < max_guys; ++g) {
        if
        (
          Port[g]->active &&
          Port[g]->sys_num == MY_PORT->sys_num &&
          Port[g]->sys_x == x &&
          Port[g]->sys_y == y
        ) {
          Probed_Guy(g);
        }
      }
    } else if (MY_SYS->node_type[x][y] == DATA_WALL) {
      prfmsg(MSG108, MY_SYS->wall);
    } else if (MY_SYS->node_type[x][y] == DEAD_WALL) {
      prfmsg(MSG109);
    }

    runner = Find_Program
             (
               MY_PORT->net_x, MY_PORT->net_y,
               MY_PORT->city_x, MY_PORT->city_y,
               x, y,
               &g, &p
             );

    if (runner != NULL) {
      Probed_Program(g, p);
    } else if (MY_SYS->node_type[x][y] == 0) {
      prfmsg(MSG110);
    }

    EXIT0;
  }

  for (g = 0; g != max_guys; ++g) {
    if
    (
      Port[g]->active &&
      Port[g]->access_level == CITY_VIEW &&
      Port[g]->net_x == MY_PORT->net_x &&
      Port[g]->net_y == MY_PORT->net_y &&
      Port[g]->city_x == x &&
      Port[g]->city_y == y
    ) {
      Probed_Guy(g);
      EXIT0;
    }
  }

  if (s == CONST) {
    runner = Find_Program
             (
               MY_PORT->net_x, MY_PORT->net_y,
               x, y,
               -1, -1,
               &g, &p
             );

    if (runner != NULL) {
      Probed_Program(g, p);
      EXIT0;
    }

    s = 0;
  }

  if (s == 0) {
    prfmsg(MSG110);
    EXIT0;
  }

  if (s == PHONE) {
    prfmsg(MSG111, MY_CITY.security);
    EXIT0;
  }

  if (s == CYBERDECK) {
    for (s = 0; s != max_guys; ++s) {
      if (Sys[s]->in_use || Port[s]->active) {
        if
        (
          Sys[s]->net_x == MY_PORT->net_x &&
          Sys[s]->net_y == MY_PORT->net_y
        ) {
          if (Sys[s]->city_x == x && Sys[s]->city_y == y) {
            prfmsg(MSG112, Sys[s]->Name);
          }
        }
      }
    }

    EXIT0;
  }

  if (s == COP_SYS) {
    prfmsg(MSG679);
    EXIT0;
  }

  if (s == ALIEN_SYS) {
    prfmsg(MSG923);
    EXIT0;
  }

  if (s == CYBERIA_SYS) {
    prfmsg(MSG923);
    EXIT0;
  }

  if (s == END_SYS) {
    prfmsg(MSG713);
    EXIT0;
  }

  if (s == BBS) {
    prfmsg(MSG113);
  } else if (s == LEV1) {
    prfmsg(MSG114);
  } else if (s == LEV2) {
    prfmsg(MSG115);
  } else if (s == LEV3) {
    prfmsg(MSG116);
  } else if (s == VENDOR) {
    prfmsg(MSG117);
  } else if (s == LEV4) {
    prfmsg(MSG118);
  } else if (s == LEV5) {
    prfmsg(MSG119);
  } else if (s == LEV6) {
    prfmsg(MSG851);
  } else if (s == LEV7) {
    prfmsg(MSG852);
  } else if (s == CRIMINAL) {
    prfmsg(MSG120);
  } else if (s == WORLDBANK) {
    prfmsg(MSG120B);
  } else if (s == WEB_SERVER) {
    prfmsg(MSG850);
  } else if (s == FILE_SERVER) {
    prfmsg(M797);
  } else if (s == HUB) {
    prfmsg(M120A);
  }

  Set_System_Path(x, y, MY_PORT->net_x, MY_PORT->net_y);
  fp = fopen(glob->path, READB);

  if (fp == NULL) {
    prf
    (
      "File Open Failed at %d/%d %d/%d\r",
      MY_PORT->net_x, MY_PORT->net_y,
      x, y
    );
  } else {
    n = fread(a_system, SYSTEM_SIZE, 1, fp);

    if (n != 1) {
      prf
      (
        "File Read Failed at %d/%d %d/%d\r",
        MY_PORT->net_x, MY_PORT->net_y,
        x, y
      );
    } else {
      prfmsg(MSG121, a_system->Name);
    }

    fclose(fp);
  }

  EXIT0;
}
/****************************************************************************
	load up and run a program
*/
short Load_Program(void) {
  short   s;
  short   g;
  short   j;
  short   i = -1;
  short   index;
  short   m_index = MY_PORT->m_index;
  short   p_index = MY_PORT->p_index;
  short   x = MY_PORT->target_x;
  short   y = MY_PORT->target_y;
  short   p = MY_DECK->memory[m_index][p_index].index;
  short   class = MY_DECK->memory[m_index][p_index].class;
  short   sys_num;
  short   cpu_num;
  STATE = MAIN_PROMPT;

  if
  (
    p != MY_PORT->p_num ||
    MY_DECK->memory[m_index][p_index].type != PROGRAM
  ) {
    EXIT0;
  }

  for (s = 0; s != MY_DECK->cpus && i < 0; ++s) {
    if (MY_PROC->cpu[s].m_index == -1 && !MY_DECK->crash_time[s]) {
      i = s;
    }
  }

  if (i < 0) {
    prfmsg(MSG122, MY_DECK->memory[m_index][p_index].name);
    EXIT0;
  }

  MY_PROGRAM.net_x = -1;
  MY_PROGRAM.net_y = -1;
  MY_PROGRAM.city_x = -1;
  MY_PROGRAM.city_y = -1;
  MY_PROGRAM.seek_x = -1;
  MY_PROGRAM.seek_y = -1;
  MY_PROGRAM.cop_x = -1;
  MY_PROGRAM.cop_y = -1;
  MY_PROGRAM.target_s = -1;
  MY_PROGRAM.target_m = -1;
  MY_PROGRAM.target_index = -1;
  MY_PROGRAM.rez = 100;
  MY_PROGRAM.sees_hydra_s = -1;
  MY_PROGRAM.sees_hydra_c = -1;

  for (j = 0; j != max_guys; ++j) {
    MY_PROGRAM.sees[j] = FALSE;
    MY_PROGRAM.enemy[j] = FALSE;
  }

  if (p == VIRUS) {
    if
    (
      MY_PORT->target_type != 0 &&
      MY_PORT->target_type != DEAD_WALL &&
      MY_PORT->target_type != OPENED_GATE
    ) {
      prfmsg(MSG123);
      EXIT0;
    }

    Proc[MY_PORT->sys_num]->enemy[myp] = TRUE;
    Proc[MY_PORT->sys_num]->infected = TRUE;
  } else if
  (
    class == HYDRA ||
    p == SURGE ||
    p == LIGHTNING ||
    p == PUNTER ||
    p == ICEBOLT ||
    p == INFILTRATE ||
    p == HANDCUFF ||
    p == FIREBOLT ||
    p == BANSHEE ||
    p == BRAINDRAIN ||
    p == LOBOTOMY ||
    p == JOLT ||
    p == MIMIC
  ) {
    if
    (
      MY_PORT->target_type != 0 &&
      MY_PORT->target_type != DEAD_WALL &&
      MY_PORT->target_type != OPENED_GATE
    ) {
      prfmsg(MSG123);
      EXIT0;
    }

    g = MY_PORT->target_s;
    MY_PROGRAM.target_index = Port[g]->guy.index;

    if (p == MIMIC) {
      strcpy(MY_DECK->memory[m_index][p_index].name, MY_PORT->mimic);
    } else if (g < max_guys && !me.perm[GAMEOP_PERM]) {
      if (His_Rank(me.score, me.won) < glob->pvprank) {
        prfmsg(M661, glob->pvprank);
        EXIT0;
      }

      if (His_Rank(Port[g]->guy.score, Port[g]->guy.won) < glob->pvprank) {
        prfmsg(M662, glob->pvprank);
        EXIT0;
      }

      if (glob->outlaw) {
        if (!me.outlaw) {
          prfmsg(MSSG670);
          EXIT0;
        }

        if (!Port[g]->guy.outlaw) {
          prfmsg(MSSG671);
          EXIT0;
        }
      }
    }
  } else if (p == KNIGHT || p == ADDLE || p == HERCULES) {
    if
    (
      MY_PORT->target_type != 0 &&
      MY_PORT->target_type != DEAD_WALL &&
      MY_PORT->target_type != OPENED_GATE
    ) {
      prfmsg(MSG123);
      EXIT0;
    }

    sys_num = g = MY_PORT->target_s;
    MY_PROGRAM.target_s = g;
    cpu_num = MY_PROGRAM.target_index = MY_PORT->target_c;

    if (p == HERCULES && SEEK_SOURCE.class != HYDRA) {
      prfmsg(MSG837, SEEK_SOURCE.name);
      EXIT0;
    }

    if (g < max_guys && !me.perm[GAMEOP_PERM]) {
      if (His_Rank(me.score, me.won) < glob->pvprank) {
        prfmsg(M661, glob->pvprank);
        EXIT0;
      }

      if (His_Rank(Port[g]->guy.score, Port[g]->guy.won) < glob->pvprank) {
        prfmsg(M662, glob->pvprank);
        EXIT0;
      }

      if (glob->outlaw) {
        if (!me.outlaw) {
          prfmsg(MSSG670);
          EXIT0;
        }

        if (!Port[g]->guy.outlaw) {
          prfmsg(MSSG671);
          EXIT0;
        }
      }
    }
  } else if
  (
    p == SENTRY ||
    p == GUARDIAN ||
    p == SOLDIER ||
    p == GABLINK ||
    p == CASHIER ||
    p == TRACE ||
    p == CAMOFLAGE ||
    p == FENCE
  ) {
    if
    (
      MY_PORT->target_type != 0 &&
      MY_PORT->target_type != DEAD_WALL &&
      MY_PORT->target_type != OPENED_GATE
    ) {
      prfmsg(MSG123);
      EXIT0;
    }

    if (p == TRACE) {
      MY_PROGRAM.target_s = MY_PORT->sys_num;
      MY_PROGRAM.target_m = 0;
    } else if (p == CAMOFLAGE) {
      ++MY_MISC_CITY.camoflaged[MY_SYS->city_x][MY_SYS->city_y];
      Quick_Update
      (
        CITY_VIEW,
        MY_PORT->net_x, MY_PORT->net_y,
        MY_PORT->city_x, MY_PORT->city_y,
        MY_PORT->city_x, MY_PORT->city_y,
        0, 0
      );
    }
  } else if (p == BARRICADE) {
    if
    (
      MY_PORT->target_type != 0 &&
      MY_PORT->target_type != DEAD_WALL &&
      MY_PORT->target_type != OPENED_GATE &&
      MY_PORT->target_type != PHONE
    ) {
      prfmsg(MSG123);
      EXIT0;
    }

    if (p == TRACE) {
      MY_PROGRAM.target_s = MY_PORT->sys_num;
      MY_PROGRAM.target_m = 0;
    }
  } else if (p == PROBE) {
    Probed();
    EXIT0;
  } else if (p == GATECRASH) {
    if (MY_PORT->target_type != CLOSED_GATE) {
      prfmsg(MSG124);
      EXIT0;
    }

    Proc[MY_PORT->sys_num]->enemy[myp] = TRUE;
  } else if (p == BULLDOZER || p == MOLE) {
    if (MY_PORT->target_type != DATA_WALL) {
      prfmsg(MSG125);
      EXIT0;
    }

    Proc[MY_PORT->sys_num]->enemy[myp] = TRUE;

    if (p == MOLE) {
      Proc[MY_PORT->sys_num]->moled = TRUE;
    }
  } else if (p == CRASH || p == BLOB) {
    if (MY_PORT->target_type != CPU) {
      prfmsg(MSG126);
      EXIT0;
    }

    if (p == CRASH) {
      index = MY_SYS->node_index[x][y];

      if (MY_DECK->crash_time[index]) {
        prfmsg(MSG127);
        EXIT0;
      }
    }

    Proc[MY_PORT->sys_num]->enemy[myp] = TRUE;
  } else if (p == TORPEDO) {
    if (MY_PORT->target_type != CPU && MY_PORT->target_type != MEMORY) {
      prfmsg(MSG658);
      EXIT0;
    }

    Proc[MY_PORT->sys_num]->enemy[myp] = TRUE;
    Proc[MY_PORT->sys_num]->torped = TRUE;
  }

  MY_PROGRAM.m_index = m_index;
  MY_PROGRAM.p_index = p_index;
  MY_PROGRAM.loading = Loading_Time(myp, m_index, p_index);
  MY_PROGRAM.tentacle_timer = 0;
  MY_SOURCE.running = TRUE;

  if
  (
    class == HYDRA ||
    p == SURGE ||
    p == ICEBOLT ||
    p == INFILTRATE ||
    p == HANDCUFF ||
    p == FIREBOLT ||
    p == BANSHEE ||
    p == BRAINDRAIN ||
    p == LOBOTOMY ||
    p == JOLT ||
    p == MIMIC ||
    p == PUNTER ||
    p == GUARDIAN ||
    p == SOLDIER ||
    p == SENTRY ||
    p == CASHIER ||
    p == TRACE ||
    p == CAMOFLAGE ||
    p == FENCE ||
    p == GABLINK ||
    p == BARRICADE ||
    p == LIGHTNING ||
    p == KNIGHT ||
    p == ADDLE ||
    p == HERCULES
  ) {
    if (MY_PORT->access_level == SYSTEM_VIEW) {
      MY_PROGRAM.sys_x = x;
      MY_PROGRAM.sys_y = y;
    } else {
      MY_PROGRAM.sys_x = -1;
      MY_PROGRAM.sys_y = -1;
    }
  } else {
    MY_PROGRAM.sys_x = x;
    MY_PROGRAM.sys_y = y;
  }

  if
  (
    p != CLOAK &&
    p != RADAR &&
    p != MUFFLER &&
    p != SHIELD &&
    p != ARMOR &&
    p != CHAINSAW &&
    p != SKULLDRILL &&
    p != TENTACLE &&
    p != CATTLEPROD &&
    p != GLUEGUN &&
    p != COBRA &&
    p != PHASER &&
    p != NETSPEED &&
    p != KEY1 &&
    p != KEY2 &&
    p != KEY3 &&
    p != STATIK
  ) {
    MY_PROGRAM.net_x = MY_PORT->net_x;
    MY_PROGRAM.net_y = MY_PORT->net_y;

    if
    (
      class == HYDRA ||
      p == KNIGHT ||
      p == HERCULES ||
      p == ADDLE ||
      p == SURGE ||
      p == ICEBOLT ||
      p == INFILTRATE ||
      p == HANDCUFF ||
      p == FIREBOLT ||
      p == BANSHEE ||
      p == BRAINDRAIN ||
      p == LOBOTOMY ||
      p == JOLT ||
      p == MIMIC ||
      p == PUNTER ||
      p == GUARDIAN ||
      p == SOLDIER ||
      p == SENTRY ||
      p == CASHIER ||
      p == TRACE ||
      p == CAMOFLAGE ||
      p == FENCE ||
      p == GABLINK ||
      p == BARRICADE ||
      p == LIGHTNING
    ) {
      if (MY_PORT->access_level == CITY_VIEW) {
        MY_PROGRAM.city_x = x;
        MY_PROGRAM.city_y = y;
      } else {
        MY_PROGRAM.city_x = MY_PORT->city_x;
        MY_PROGRAM.city_y = MY_PORT->city_y;
      }
    } else {
      MY_PROGRAM.city_x = MY_PORT->city_x;
      MY_PROGRAM.city_y = MY_PORT->city_y;
    }

    sprintf(str, glob->m130, MY_SOURCE.name);

    if (Hydra_Has(CLOAK, s, i)) {
      Send(str, myp, FALSE);
    } else {
      if (MY_PORT->access_level == SYSTEM_VIEW) {
        System_Message(str, MY_PORT->sys_num);
      } else {
        City_Message(str, MY_PORT->net_x, MY_PORT->net_y, -1);
      }
    }
  }

  if (p == CLOAK && MY_PORT->access_level != IDLE) {
    Cloak_Guy(myp);
  } else if (Hydra_Has(CLOAK, myp, i)) {
    Cloak_Hydra(myp, i);
  }

  prf("\x1B[%d;1H", DISP_Y);
  prfmsg
  (
    MSSG129,
    i + 1,
    MY_SOURCE.name,
    MY_SOURCE.strength,
    MY_PROGRAM.loading
  );

  if (MY_PORT->access_level == SYSTEM_VIEW && MY_PROGRAM.sys_x != -1) {
    Quick_Update
    (
      MY_PORT->access_level,
      MY_PORT->net_x, MY_PORT->net_y,
      MY_PORT->city_x, MY_PORT->city_y,
      MY_PROGRAM.sys_x, MY_PROGRAM.sys_y,
      0, 0
    );
  } else if (MY_PORT->access_level == CITY_VIEW && MY_PROGRAM.city_x != -1) {
    Quick_Update
    (
      MY_PORT->access_level,
      MY_PORT->net_x, MY_PORT->net_y,
      MY_PORT->city_x, MY_PORT->city_y,
      MY_PROGRAM.city_x, MY_PROGRAM.city_y,
      0, 0
    );
  }

  if (MY_PORT->access_level == CITY_VIEW && MY_SOURCE.index == RADAR) {
    Look("X");
  }

  Sentry_Check(myp, i);
  return (TRUE);
}
/****************************************************************************
	system loads up and runs a program
*/
short Load_System_Program(short s, short i, short m_index, short p_index) {
  short   j;
  short   g;
  short   done = FALSE;
  short   tries = 0;
  short   x = 0;
  short   y = 0;
  short   appears_here = TRUE;
  short   dummy_s, dummy_c;
  struct  cpu_struct *runner;

  if (Sys[s]->type == CON_SYS) {
    appears_here = FALSE;
  } else if
  (
    Sys[s]->type == COP_SYS &&
    Sys[s]->memory[m_index][p_index].index == ROBOCOP
  ) {
    appears_here = FALSE;
  }

  if (appears_here) {
    do {
      ++tries;

      if
      (
        RANDOM(7) != 1 ||
        Sys[s]->memory[m_index][p_index].index == FENCE ||
        Sys[s]->memory[m_index][p_index].index == CASHIER
      ) {
        x = RANDOM(MAX_SYS_X - 4) + 1;
        y = RANDOM(MAX_SYS_Y - 4) + 1;
      } else {
        if (RANDOM(3) == 1) {
          x = 0;
          y = RANDOM(MAX_SYS_Y) - 1;
        } else if (RANDOM(3) == 1) {
          x = MAX_SYS_X - 1;
          y = RANDOM(MAX_SYS_Y) - 1;
        } else if (RANDOM(3) == 1) {
          y = 0;
          x = RANDOM(MAX_SYS_X) - 1;
        } else {
          y = MAX_SYS_Y - 1;
          x = RANDOM(MAX_SYS_X) - 1;
        }
      }

      if (Sys[s]->node_type[x][y] == 0) {
        runner = Find_Program
                 (
                   Sys[s]->net_x, Sys[s]->net_y,
                   Sys[s]->city_x, Sys[s]->city_y,
                   x, y,
                   &dummy_s, &dummy_c
                 );

        if (runner == NULL) {
          done = TRUE;
        }
      }
    } while (!done && tries != 50);

    if (!done) {
      EXIT0;
    }
  }

  SYS_PROGRAM.seek_x = -1;
  SYS_PROGRAM.seek_y = -1;
  SYS_PROGRAM.cop_x = -1;
  SYS_PROGRAM.cop_y = -1;
  SYS_PROGRAM.target_s = -1;
  SYS_PROGRAM.target_m = -1;
  SYS_PROGRAM.m_index = m_index;
  SYS_PROGRAM.p_index = p_index;
  SYS_PROGRAM.rez = 100;
  SYS_SOURCE.running = TRUE;
  SYS_PROGRAM.loading = Loading_Time(s, m_index, p_index);
  SYS_PROGRAM.tentacle_timer = 0;

  if
  (
    appears_here &&
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
    )
  ) {
    do {
      g = RANDOM(max_guys) - 1;
    } while (!Proc[s]->enemy[g]);

    SYS_PROGRAM.target_index = Port[g]->guy.index;
  } else {
    SYS_PROGRAM.target_index = -1;
  }

  if
  (
    SYS_SOURCE.index == KNIGHT ||
    SYS_SOURCE.index == ADDLE ||
    SYS_SOURCE.index == HERCULES
  ) {
    Knight_Target(s, &SYS_PROGRAM.target_s, &SYS_PROGRAM.target_index);
  }

  for (j = 0; j != max_guys; ++j) {
    SYS_PROGRAM.sees[j] = FALSE;
    SYS_PROGRAM.enemy[j] = FALSE;
  }

  if (appears_here) {
    SYS_PROGRAM.net_x = Sys[s]->net_x;
    SYS_PROGRAM.net_y = Sys[s]->net_y;
    SYS_PROGRAM.city_x = Sys[s]->city_x;
    SYS_PROGRAM.city_y = Sys[s]->city_y;
    SYS_PROGRAM.sys_x = x;
    SYS_PROGRAM.sys_y = y;
    sprintf(str, glob->m130, SYS_SOURCE.name);

    if (!Hydra_Has(CLOAK, s, i)) {
      System_Message(str, s);
    }
  } else {
    SYS_PROGRAM.net_x = RANDOM(MAX_NET_X) - 1;
    SYS_PROGRAM.net_y = RANDOM(MAX_NET_Y) - 1;
    SYS_PROGRAM.city_x = Sys[s]->city_x;
    SYS_PROGRAM.city_y = Sys[s]->city_y;
    SYS_PROGRAM.sys_x = -1;
    SYS_PROGRAM.sys_y = -1;

    do {
      do {
        x = RANDOM(MAX_CITY_X - 2);
        y = RANDOM(MAX_CITY_Y - 2);
      } while (SYS_CONST_CITY.type[x][y] != 0);

      runner = Find_Program
               (
                 SYS_PROGRAM.net_x, SYS_PROGRAM.net_y,
                 x, y,
                 -1, -1,
                 &dummy_s, &dummy_c
               );

      if (runner == NULL) {
        for (g = 0, done = TRUE; g != max_guys; ++g) {
          if
          (
            Port[g]->active &&
            Port[g]->access_level != IDLE &&
            Port[g]->net_x == SYS_PROGRAM.net_x &&
            Port[g]->net_y == SYS_PROGRAM.net_y &&
            Port[g]->city_x == x &&
            Port[g]->city_y == y
          ) {
            done = FALSE;
          } else {
            SYS_PROGRAM.city_x = x;
            SYS_PROGRAM.city_y = y;
          }
        }
      }
    } while (!done);

    if (!Hydra_Has(CLOAK, s, i)) {
      sprintf(str, glob->m130, SYS_SOURCE.name);
      City_Message(str, SYS_PROGRAM.net_x, SYS_PROGRAM.net_y, -1);
    }
  }

  Sentry_Check(s, i);
  return (TRUE);
}
/**************************************************************************
	user wants to lock something in memory
*/
short EXPORT File_Utility(short option) {
  short   i;
  short   f;
  short   s;
  short   m;
  short   x, y;
  short   total;
  short   found;
  short   avail = -1;

  if (option == 0) {
    if (STATE == MEMORY_PROMPT2) {
      found = TRUE;
    } else {
      found = Find_Memory
              (
                margv[0],
                &MY_PORT->source_s,
                &MY_PORT->source_m
              );
    }

    if (!found) {
      STATE = MAIN_PROMPT;
      EXIT0;
    }

    s = MY_PORT->source_s;
    m = MY_PORT->source_m;
    total = Memory_Contents(Sys[s]->memory[m], 1);

    if (total == 0) {
      prfmsg(MSG131);
      STATE = MAIN_PROMPT;

      if (MY_PORT->unlocking) {
        MY_PORT->unlocking = FALSE;
        Memory_Services();
      }

      EXIT0;
    }

    if (MY_PORT->p_num == LOCKER) {
      sprintf(str, getmsg(MSG132), total);
    } else {
      sprintf(str, getmsg(MSG133), total);
    }

    Prompt(str, LOCK_PROMPT);
    EXIT0;
  }

  s = MY_PORT->source_s;
  m = MY_PORT->source_m;
  total = Num_Files(Sys[s]->memory[m]);
  f = INUM(margv[0]);

  if (f == 0 || f > total) {
    prfmsg(MSG134, total);
    STATE = MAIN_PROMPT;

    if (MY_PORT->unlocking) {
      MY_PORT->unlocking = FALSE;
      Memory_Services();
    }

    EXIT0;
  }

  --f;

  if (Sys[s]->memory[m][f].running) {
    prfmsg(MSG135, Sys[s]->memory[m][f].name);
    STATE = MAIN_PROMPT;

    if (MY_PORT->unlocking) {
      MY_PORT->unlocking = FALSE;
      Memory_Services();
    }

    EXIT0;
  }

  if (MY_PORT->p_num == LOCKER) {
    if (Sys[s]->memory[m][f].lock) {
      prfmsg(MSG136, Sys[s]->memory[m][f].name);
      STATE = MAIN_PROMPT;
      EXIT0;
    }
  } else {
    if (!Sys[s]->memory[m][f].lock) {
      prfmsg(MSG137, Sys[s]->memory[m][f].name);
      STATE = MAIN_PROMPT;

      if (MY_PORT->unlocking) {
        MY_PORT->unlocking = FALSE;
        Memory_Services();
      }

      EXIT0;
    }
  }

  for (i = 0; i != MY_DECK->cpus && avail < 0; ++i) {
    if (MY_PROGRAM.m_index == -1 && !MY_DECK->crash_time[i]) {
      avail = i;
    }
  }

  if (avail < 0) {
    prfmsg(MSG138);

    if (MY_PORT->unlocking) {
      MY_PORT->unlocking = FALSE;
      Memory_Services();
    }

    EXIT0;
  }

  i = avail;
  MY_PROGRAM.m_index = MY_PORT->m_index;
  MY_PROGRAM.p_index = MY_PORT->p_index;
  MY_PROGRAM.target_index = f;
  MY_PROGRAM.target_s = s;
  MY_PROGRAM.target_m = m;
  MY_PROGRAM.net_x = -1;
  MY_PROGRAM.net_y = -1;
  MY_PROGRAM.city_x = -1;
  MY_PROGRAM.city_y = -1;
  MY_PROGRAM.sys_x = -1;
  MY_PROGRAM.sys_y = -1;
  MY_PROGRAM.rez = 100;
  MY_SOURCE.running = TRUE;

  if (s != myp) {
    MY_PROGRAM.loading = Loading_Time
                         (
                           myp,
                           MY_PROGRAM.m_index,
                           MY_PROGRAM.p_index
                         );
  } else {
    MY_PROGRAM.loading = 0;
  }

  if (MY_PORT->access_level == SYSTEM_VIEW && MY_PORT->sys_num == s) {
    sprintf(str, glob->m130, MY_SOURCE.name);
    System_Message(str, MY_PORT->sys_num);
    MY_PROGRAM.net_x = MY_PORT->net_x;
    MY_PROGRAM.net_y = MY_PORT->net_y;
    MY_PROGRAM.city_x = MY_PORT->city_x;
    MY_PROGRAM.city_y = MY_PORT->city_y;

    for (x = 0; x != MAX_SYS_X; ++x) {
      for (y = 0; y != MAX_SYS_Y; ++y) {
        if
        (
          MY_SYS->node_type[x][y] == MEMORY &&
          MY_SYS->node_index[x][y] == m
        ) {
          MY_PROGRAM.sys_x = x;
          MY_PROGRAM.sys_y = y;
        }
      }
    }

    if (MY_PORT->access_level == SYSTEM_VIEW && MY_PROGRAM.sys_x != -1) {
      Quick_Update
      (
        MY_PORT->access_level,
        MY_PORT->net_x, MY_PORT->net_y,
        MY_PORT->city_x, MY_PORT->city_y,
        MY_PROGRAM.sys_x, MY_PROGRAM.sys_y,
        0, 0
      );
    }
  }

  Clear_Display_Window();
  prfmsg
  (
    MSSG129,
    avail + 1,
    MY_SOURCE.name,
    MY_SOURCE.strength,
    MY_PROGRAM.loading
  );

  if (!MY_PORT->unlocking) {
    STATE = MAIN_PROMPT;
  }

  if (MY_PORT->unlocking) {
    MY_PORT->unlocking = FALSE;
    Memory_Services();
  }

  Sentry_Check(myp, i);
  EXIT0;
}

