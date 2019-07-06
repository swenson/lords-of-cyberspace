#include <header.h>
#include <globals.h>

/****************************************************************************
    I'm hammering on a wall
*/
short EXPORT Hammering(short i, short s) {
  short   x;
  short   y;
  short   ok = TRUE;
  x = SYS_PROGRAM.sys_x;
  y = SYS_PROGRAM.sys_y;

  if (Port[s]->access_level != SYSTEM_VIEW) {
    ok = FALSE;
  } else {
    x = abs(Port[s]->sys_x - x);
    y = abs(Port[s]->sys_y - y);

    if (x && y) {
      ok = FALSE;
    } else if (x > 1 || y > 1) {
      ok = FALSE;
    }
  }

  if (!ok) {
    sprintf(str, getmsg(MSG26), SYS_SOURCE.name);
    Send(str, s, FALSE);
    Program_Stops(s, i);
  }

  return (ok);
}
/****************************************************************************
    I just hammered a wall
*/
void Hammered(short i, short s) {
  short   p = SYS_SOURCE.index;
  short   ok;
  short   x;
  short   y;
  short   roll;
  short   n = Runner_System_Number(s, i);
  ok = Hammering(i, s);

  if (ok) {
    x = SYS_PROGRAM.sys_x;
    y = SYS_PROGRAM.sys_y;
    ok = FALSE;

    if (p == BULLDOZER) {
      roll = SYS_SOURCE.strength + RANDOM(glob->fate);

      if (roll > Sys[n]->wall + RANDOM(glob->fate)) {
        ok = TRUE;
      }
    } else if (p == MOLE) {
      if (Sys[n]->wall < 100) {
        ok = TRUE;
      }
    }

    if (!ok) {
      sprintf(str, glob->m30, SYS_SOURCE.name);
      Send(str, s, FALSE);
    } else {
      sprintf(str, getmsg(MSG31), SYS_SOURCE.name);
      Send(str, s, FALSE);
      Sys[n]->node_type[x][y] = DEAD_WALL;
      Sys[n]->breached = TRUE;
    }

    Program_Stops(s, i);
  }
}
/****************************************************************************
    I'm locking a file
*/
short Locking(short i, short s) {
  short   x;
  short   y;
  short   ok = TRUE;
  short   n = Runner_System_Number(s, i);
  x = SYS_PROGRAM.sys_x;
  y = SYS_PROGRAM.sys_y;

  if (x >= 0 && n != s) {
    if (Port[s]->access_level != SYSTEM_VIEW) {
      ok = FALSE;
    } else {
      x = abs(Port[s]->sys_x - x);
      y = abs(Port[s]->sys_y - y);

      if (x && y) {
        ok = FALSE;
      } else if (x > 1 || y > 1) {
        ok = FALSE;
      }
    }
  }

  if (!ok) {
    sprintf(str, getmsg(MSG27), SYS_SOURCE.name);
    Send(str, s, FALSE);
    Program_Stops(s, i);
  }

  return (ok);
}
/****************************************************************************
    I just locked a file
*/
void Locked(short i, short s) {
  short   ok;
  short   target_s = SYS_PROGRAM.target_s;
  short   m = SYS_PROGRAM.target_m;
  short   f = SYS_PROGRAM.target_index;
  ok = Locking(i, s);

  if (ok) {
    Sys[target_s]->memory[m][f].lock = SYS_SOURCE.strength;
    sprintf
    (
      str,
      getmsg(MSG32),
      SYS_SOURCE.name,
      Sys[target_s]->memory[m][f].name
    );
    Send(str, s, FALSE);
    Program_Stops(s, i);
  }
}
/****************************************************************************
    I'm unlocking a file
*/
short Unlocking(short i, short s) {
  short   x;
  short   y;
  short   ok = TRUE;
  short   f = SYS_PROGRAM.target_index;
  short   m = SYS_PROGRAM.target_m;
  short   target_s = SYS_PROGRAM.target_s;
  short   n = Runner_System_Number(s, i);
  x = SYS_PROGRAM.sys_x;
  y = SYS_PROGRAM.sys_y;

  if (x >= 0 && n != s) {
    if (Port[s]->access_level != SYSTEM_VIEW) {
      ok = FALSE;
    } else {
      x = abs(Port[s]->sys_x - x);
      y = abs(Port[s]->sys_y - y);

      if (x && y) {
        ok = FALSE;
      } else if (x > 1 || y > 1) {
        ok = FALSE;
      }
    }
  }

  if (ok) {
    if
    (
      Sys[target_s]->memory[m][f].type == 0 ||
      !Sys[target_s]->memory[m][f].lock
    ) {
      Program_Stops(s, i);
      return (FALSE);
    }
  }

  if (!ok) {
    sprintf(str, getmsg(MSG27), SYS_SOURCE.name);
    Send(str, s, FALSE);
    Program_Stops(s, i);
  }

  return (ok);
}
/****************************************************************************
    I just unlocked a file
*/
short Unlocked(short i, short s) {
  short   n = Runner_System_Number(s, i);
  short   f = SYS_PROGRAM.target_index;
  short   m = SYS_PROGRAM.target_m;
  short   target_s = SYS_PROGRAM.target_s;
  short   ok;
  short   my_roll;
  short   his_roll;
  ok = Unlocking(i, s);

  if (!ok) {
    EXIT0;
  }

  my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

  if (SYS_PROGRAM.sys_x >= 0) {
    his_roll = Sys[target_s]->memory[m][f].lock + RANDOM(glob->fate);
  } else {
    his_roll = 0;
  }

  if (my_roll <= his_roll && s != n) {
    sprintf(str, glob->m30, SYS_SOURCE.name);
    Send(str, s, FALSE);
  } else {
    Sys[target_s]->memory[m][f].lock = 0;

    if
    (
      s < max_guys &&
      Port[s]->usrnum != -1 &&
#ifdef MBBS
      user[Port[s]->usrnum].substt == MEMORY_PROMPT2 &&
#else
      usroff(Port[s]->usrnum)->substt == MEMORY_PROMPT2 &&
#endif
      Port[s]->source_s == target_s &&
      Port[s]->source_m == m
    ) {
      myp = s;
      Clear_Display_Window();
      Memory_Contents(Sys[target_s]->memory[m], 1);
    }

    sprintf
    (
      str,
      getmsg(MSG33),
      SYS_SOURCE.name,
      Sys[target_s]->memory[m][f].name
    );
    Send(str, s, FALSE);
  }

  Program_Stops(s, i);
  EXIT0;
}
/****************************************************************************
    I'm crashing through a gate
*/
short Gate_Crashing(short i, short s) {
  short   x;
  short   y;
  short   ok = TRUE;
  x = SYS_PROGRAM.sys_x;
  y = SYS_PROGRAM.sys_y;

  if (Port[s]->access_level != SYSTEM_VIEW) {
    ok = FALSE;
  } else {
    x = abs(Port[s]->sys_x - x);
    y = abs(Port[s]->sys_y - y);

    if (x && y) {
      ok = FALSE;
    } else if (x > 1 || y > 1) {
      ok = FALSE;
    }
  }

  if (!ok) {
    sprintf(str, getmsg(MSG28), SYS_SOURCE.name);
    Send(str, s, FALSE);
    Program_Stops(s, i);
  }

  return (ok);
}
/****************************************************************************
    I just crashed through a gate
*/
short Gate_Crashed(short i, short s) {
  short   n = Runner_System_Number(s, i);
  short   ok;
  short   my_roll;
  short   his_roll;
  short   x;
  short   y;
  ok = Gate_Crashing(i, s);

  if (!ok) {
    EXIT0;
  }

  x = SYS_PROGRAM.sys_x;
  y = SYS_PROGRAM.sys_y;
  my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);
  his_roll = Sys[n]->gate + RANDOM(glob->fate);

  if (my_roll <= his_roll && s != n) {
    sprintf(str, glob->m30, SYS_SOURCE.name);
    Send(str, s, FALSE);
  } else {
    sprintf(str, getmsg(MSG31), SYS_SOURCE.name);
    Send(str, s, FALSE);

    if (RANDOM(GATECRASH_PW) == 1 && Sys[n]->password[0]) {
      sprintf(str, getmsg(MSG846), SYS_SOURCE.name, Sys[n]->password);
      Send(str, s, TRUE);
    }

    Sys[n]->node_type[x][y] = OPENED_GATE;
  }

  Program_Stops(s, i);
  EXIT0;
}
/****************************************************************************
    I'm crashing a cpu
*/
short Crashing(short i, short s) {
  short   x;
  short   y;
  short   ok = TRUE;

  if (Port[s]->access_level != SYSTEM_VIEW) {
    ok = FALSE;
  } else {
    x = SYS_PROGRAM.sys_x;
    y = SYS_PROGRAM.sys_y;
    x = abs(Port[s]->sys_x - x);
    y = abs(Port[s]->sys_y - y);

    if (x && y) {
      ok = FALSE;
    } else if (x > 1 || y > 1) {
      ok = FALSE;
    }
  }

  if (!ok) {
    sprintf(str, getmsg(MSG29), SYS_SOURCE.name);
    Send(str, s, FALSE);
    Program_Stops(s, i);
  }

  return (ok);
}
/****************************************************************************
    I just crashed a cpu
*/
short Crashed(short i, short s) {
  short   ok;
  short   my_roll;
  short   his_roll;
  short   x;
  short   y;
  short   p;
  short   secs;
  short   score;
  short   crashed;
  short   g = Runner_System_Number(s, i);
  short   net_x = Sys[g]->net_x;
  short   net_y = Sys[g]->net_y;
  ok = Crashing(i, s);

  if (!ok) {
    EXIT0;
  }

  x = SYS_PROGRAM.sys_x;
  y = SYS_PROGRAM.sys_y;
  my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);
  his_roll = Sys[g]->speed + RANDOM(glob->fate);

  if (my_roll <= his_roll && !Port[s]->guy.perm[GAMEOP_PERM]) {
    sprintf(str, glob->m30, SYS_SOURCE.name);
    Send(str, s, FALSE);
    Program_Stops(s, i);
    EXIT0;
  }

  sprintf(str, getmsg(MSG31), SYS_SOURCE.name);
  Send(str, s, FALSE);
  p = Sys[g]->node_index[x][y];
  secs = glob->crshsecs * SYS_SOURCE.strength;
  Sys[g]->crash_time[p] = secs;
  sprintf(str, getmsg(MSG34), p + 1);
  Send(str, g, FALSE);

  if (g != s || i != p) {
    if (GUY_SOURCE.running) {
      Program_Stops(g, p);
    }
  }

  Program_Stops(s, i);
  crashed = Crash_Check(g, &score);

  if
  (
    crashed &&
    Port[s]->guy.mission_type == CRASH_MISSION &&
    Port[s]->guy.mission_x == net_x &&
    Port[s]->guy.mission_y == net_y &&
    sameas(Port[s]->guy.mission_system, Sys[g]->Name) &&
    Sys[g]->type != CYBERDECK
  ) {
    strcpy(str, getmsg(MSG729));
    Send(str, s, TRUE);
    Scored(s, Port[s]->guy.mission_score * MISSION_SCORE, 1);
    Add_Bank_Record
    (
      &Port[s]->guy,
      (long) (Port[s]->guy.mission_score * MISSION_BUX),
      net_x, net_y,
      Port[s]->guy.alias,
      "Mission"
    );
    Port[s]->guy.mission_type = 0;
  }

  EXIT0;
}
/****************************************************************************
    I'm torpedo'ing a cpu or MU
*/
short Torping(short i, short s) {
  short   x;
  short   y;
  short   ok = TRUE;
  short   g = Runner_System_Number(s, i);

  if (Port[s]->access_level != SYSTEM_VIEW) {
    ok = FALSE;
  } else {
    x = SYS_PROGRAM.sys_x;
    y = SYS_PROGRAM.sys_y;
    x = abs(Port[s]->sys_x - x);
    y = abs(Port[s]->sys_y - y);

    if (x && y) {
      ok = FALSE;
    } else if (x > 1 || y > 1) {
      ok = FALSE;
    }
  }

  x = SYS_PROGRAM.sys_x;
  y = SYS_PROGRAM.sys_y;

  if (Sys[g]->node_type[x][y] != CPU && Sys[g]->node_type[x][y] != MEMORY) {
    ok = FALSE;
  }

  if (!ok) {
    if (Sys[g]->node_type[x][y] == CPU) {
      sprintf(str, getmsg(MSG29), SYS_SOURCE.name);
      Send(str, s, FALSE);
    } else if (Sys[g]->node_type[x][y] == MEMORY) {
      sprintf(str, getmsg(MSG27), SYS_SOURCE.name);
      Send(str, s, FALSE);
    }

    Program_Stops(s, i);
  }

  return (ok);
}
/****************************************************************************
    I just torpedo'd a cpu or MU
*/
short Torped(short i, short s) {
  short   ok;
  short   my_roll;
  short   his_roll;
  short   x, y;
  short   p;
  short   f;
  short   crashed;
  short   score;
  short   gameop = FALSE;
  short   g = Runner_System_Number(s, i);
  short   net_x = Sys[g]->net_x;
  short   net_y = Sys[g]->net_y;
  ok = Torping(i, s);

  if (!ok) {
    EXIT0;
  }

  x = SYS_PROGRAM.sys_x;
  y = SYS_PROGRAM.sys_y;
  my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

  if (Sys[g]->type == FILE_SERVER) {
    my_roll = 0;
  } else if (s < max_guys && Port[s]->guy.perm[GAMEOP_PERM]) {
    gameop = TRUE;
  }

  his_roll = Sys[g]->speed + RANDOM(glob->fate);

  if (my_roll <= his_roll && !gameop) {
    sprintf(str, glob->m30, SYS_SOURCE.name);
    Send(str, s, FALSE);
    Program_Stops(s, i);
    EXIT0;
  }

  sprintf(str, getmsg(MSG31), SYS_SOURCE.name);
  Send(str, s, FALSE);
  p = Sys[g]->node_index[x][y];
  Stop_All_Programs(g);

  if (Sys[g]->node_type[x][y] == CPU) {
    sprintf(str, getmsg(MSG659), p + 1);
    Send(str, g, FALSE);
    Remove_CPU(Sys[g], p);
    --Sys[g]->cpus;
    Program_Stops(s, i);
    crashed = Crash_Check(g, &score);

    if (crashed) {
      if
      (
        Port[s]->guy.mission_type == CRASH_MISSION &&
        Port[s]->guy.mission_x == net_x &&
        Port[s]->guy.mission_y == net_y &&
        sameas(Port[s]->guy.mission_system, Sys[g]->Name) &&
        Sys[g]->type != CYBERDECK
      ) {
        strcpy(str, getmsg(MSG729));
        Send(str, s, TRUE);
        Scored(s, Port[s]->guy.mission_score * MISSION_SCORE, 1);
        Add_Bank_Record
        (
          &Port[s]->guy,
          (long) (Port[s]->guy.mission_score * MISSION_BUX),
          net_x, net_y,
          Port[s]->guy.alias,
          "Mission"
        );
        Port[s]->guy.mission_type = 0;
      } else if (score) {
        Scored(s, score, 1);
      }
    }

    EXIT0;
  }

  for (f = score = 0; f != MU_CAPACITY; ++f) {
    if (Sys[g]->memory[p][f].type == DOCUMENT) {
      score += Sys[g]->memory[p][f].class;
    } else if
    (
      Sys[g]->memory[p][f].type == PROGRAM &&
      Sys[g]->memory[p][f].value
    ) {
      ++score;
    }
  }

  sprintf(str, getmsg(MSG660), p + 1);
  Send(str, g, FALSE);
  Remove_Memory(Sys[g], p);
  --Sys[g]->mus;
  Program_Stops(s, i);

  if (score) {
    Scored(s, score, 1);
  }

  EXIT0;
}
/****************************************************************************
    Crash a system if all the CPUs are down
*/
short EXPORT Crash_Check(short g, short *score) {
  long    secs = 0;
  short   p;
  short   cop;
  short   endgame = FALSE;
  short   old_port = myp;
  *score = 0;

  for (p = 0; p != Sys[g]->cpus; ++p) {
    if (!Sys[g]->crash_time[p]) {
      return (FALSE);
    }

    if (Sys[g]->crash_time[p] > secs) {
      secs = Sys[g]->crash_time[p];
    }
  }

  if (g < max_guys) {
    myp = g;
    Jack_Out();
    myp = old_port;
    return (TRUE);
  }

  if (Sys[g]->type == ALIEN_SYS || Sys[g]->type == CYBERIA_SYS) {
    SYS_NODE = 0;
    Stop_Programs_In(g);
    Boot_Users_From(g);
    Stop_All_Programs(g);
    sprintf(str, glob->m22, Sys[g]->Name);
    City_Message(str, Sys[g]->net_x, Sys[g]->net_y, g);
    Quick_Update
    (
      CITY_VIEW,
      Sys[g]->net_x, Sys[g]->net_y,
      Sys[g]->city_x, Sys[g]->city_y,
      Sys[g]->city_x, Sys[g]->city_y,
      0, 0
    );
    Sys[g]->in_use = FALSE;
    *score = Sys[g]->type * 2;
    return (TRUE);
  }

  if (g == glob->cop_sys) {
    cop = TRUE;
  } else {
    cop = FALSE;

    if (Sys[g]->type == END_SYS) {
      endgame = TRUE;
    }
  }

  for (p = 0; p != MAX_CRASHED; ++p) {
    if (dead->sys[p].type == 0) {
      dead->sys[p].net_x = Sys[g]->net_x;
      dead->sys[p].net_y = Sys[g]->net_y;
      dead->sys[p].city_x = Sys[g]->city_x;
      dead->sys[p].city_y = Sys[g]->city_y;
      dead->sys[p].type = SYS_NODE;

      if (secs == 0) {
        *score = Sys[g]->type * 2;

        if (cop) {
          secs = COP_CRASH;
        } else if (endgame) {
          secs = VALHALLA_CRASH;
        } else if (!Sys[g]->permanant) {
          secs = Sys[g]->rebuild_time - time(0);
        } else {
          secs = PERM_CRASH;
        }
      }

      dead->sys[p].timer = secs;
      SYS_NODE = 0;
      Stop_Programs_In(g);
      Boot_Users_From(g);
      Stop_All_Programs(g);
      sprintf(str, glob->m22, Sys[g]->Name);
      City_Message(str, Sys[g]->net_x, Sys[g]->net_y, g);
      Quick_Update
      (
        CITY_VIEW,
        dead->sys[p].net_x, dead->sys[p].net_y,
        dead->sys[p].city_x, dead->sys[p].city_y,
        dead->sys[p].city_x, dead->sys[p].city_y,
        0, 0
      );

      if (cop) {
        Sys[g]->in_use = FALSE;
      }

      return (TRUE);
    }
  }

  NOTIFY("LoC: Too Many Crashed Systems");
  *score = 0;
  return (FALSE);
}

