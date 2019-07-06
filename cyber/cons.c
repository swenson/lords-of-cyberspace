#include <header.h>
#include <globals.h>

/****************************************************************************
    init the end-game city
*/
void EXPORT Init_Valhalla(void) {
  short   net_x, net_y;
  short   x, y;
  short   ok;

  do {
    ok = TRUE;

    do {
      net_x = RANDOM(MAX_NET_X) - 1;
      net_y = RANDOM(MAX_NET_Y) - 1;
    } while (!sameas(world->city[net_x][net_y].name, FRONTIER));

    for (x = 0; x != MAX_CITY_X && ok; ++x) {
      for (y = 0; y != MAX_CITY_Y && ok; ++y) {
        if (world->city[net_x][net_y].type[x][y] != 0) {
          ok = FALSE;
        }
      }
    }
  } while (!ok);

  strcpy(world->city[net_x][net_y].name, VALHALLA);
  glob->valhalla_x = net_x;
  glob->valhalla_y = net_y;
  y = 4;
  x = RANDOM(20) + 5;
  glob->end_x = x;
  glob->end_y = y;

  for (x = 2; x != MAX_CITY_X - 2; ++x) {
    for (y = 2; y != MAX_CITY_Y - 2; ++y) {
      if
      (
        RANDOM(4) == 1 ||
        (x == glob->end_x && y == glob->end_y) ||
        x == 2 || y == 2 ||
        x == MAX_CITY_X - 3 || y == MAX_CITY_Y - 3
      ) {
        world->city[net_x][net_y].type[x][y] = END_SYS;
      }
    }
  }

  /*sprintf
  (
    str,
    "LoC: Today's Endgame: %d/%d %d/%d",
    net_x,
    net_y,
    glob->end_x,
    glob->end_y
  );
  NOTIFY(str);*/
}
/****************************************************************************
    Alien Nation appears in frontier
*/
short Aliens(void) {
  short   ok = TRUE;
  short   net_x;
  short   net_y;
  short   x;
  short   y;
  short   s;
  short   i;
  short   g;
  --glob->alien_timer;

  if (glob->alien_timer > 0) {
    EXIT0;
  }

  if (glob->alien_x == -1) {
    do {
      net_x = RANDOM(MAX_NET_X) - 1;
      net_y = RANDOM(MAX_NET_Y) - 1;
    } while (!sameas(world->city[net_x][net_y].name, FRONTIER));

    for (x = 0; x != MAX_CITY_X && ok; ++x) {
      for (y = 0; y != MAX_CITY_Y && ok; ++y) {
        if (world->city[net_x][net_y].type[x][y] != 0) {
          ok = FALSE;
        }
      }
    }

    if (ok) {
      for (s = 0; s != max_systems && ok; ++s) {
        if (Sys[s]->in_use) {
          for (i = 0; i != Sys[s]->cpus && ok; ++i) {
            if
            (
              SYS_PROGRAM.m_index >= 0 &&
              SYS_PROGRAM.net_x == net_x &&
              SYS_PROGRAM.net_y == net_y
            ) {
              ok = FALSE;
            }
          }
        }
      }
    }

    if (ok) {
      for (g = 0; g != max_guys && ok; ++g) {
        if
        (
          Port[g]->active &&
          Port[g]->access_level != IDLE &&
          Port[g]->net_x == net_x &&
          Port[g]->net_y == net_y
        ) {
          ok = FALSE;
        }
      }
    }

    if (!ok) {
      glob->alien_timer = ALIEN_TIME3;
      EXIT0;
    } else {
      glob->alien_timer = ALIEN_TIME2;
    }

    strcpy(world->city[net_x][net_y].name, ALIEN_CITY);

    for (x = 0; x != MAX_CITY_X; ++x) {
      for (y = 0; y != MAX_CITY_Y; ++y) {
        if (RANDOM(3) == 1) {
          world->city[net_x][net_y].type[x][y] = ALIEN_SYS;
        }
      }
    }

    glob->alien_x = net_x;
    glob->alien_y = net_y;
    EXIT0;
  }

  for (s = 0; s != max_systems && ok; ++s) {
    if
    (
      Sys[s]->in_use &&
      Sys[s]->net_x == glob->alien_x &&
      Sys[s]->net_y == glob->alien_y
    ) {
      ok = FALSE;
    }
  }

  if (ok) {
    for (g = 0; g != max_guys && ok; ++g) {
      if
      (
        Port[g]->active &&
        Port[g]->access_level != IDLE &&
        Port[g]->net_x == glob->alien_x &&
        Port[g]->net_y == glob->alien_y
      ) {
        ok = FALSE;
      }
    }
  }

  if (!ok) {
    glob->alien_timer = ALIEN_TIME3;
    EXIT0;
  }

  glob->alien_timer = ALIEN_TIME1;
  strcpy(world->city[glob->alien_x][glob->alien_y].name, FRONTIER);

  for (x = 0; x != MAX_CITY_X; ++x) {
    for (y = 0; y != MAX_CITY_Y; ++y) {
      world->city[glob->alien_x][glob->alien_y].type[x][y] = 0;
    }
  }

  glob->alien_x = -1;
  glob->alien_y = -1;
  EXIT0;
}
/****************************************************************************
    Cyberia appears in frontier
*/
short Cyberia(void) {
  short   ok = TRUE;
  short   net_x;
  short   net_y;
  short   x;
  short   y;
  short   s;
  short   i;
  short   g;
  --glob->cyberia_timer;

  if (glob->cyberia_timer > 0) {
    EXIT0;
  }

  if (glob->cyberia_x == -1) {
    do {
      net_x = RANDOM(MAX_NET_X) - 1;
      net_y = RANDOM(MAX_NET_Y) - 1;
    } while (!sameas(world->city[net_x][net_y].name, FRONTIER));

    for (x = 0; x != MAX_CITY_X && ok; ++x) {
      for (y = 0; y != MAX_CITY_Y && ok; ++y) {
        if (world->city[net_x][net_y].type[x][y] != 0) {
          ok = FALSE;
        }
      }
    }

    if (ok) {
      for (s = 0; s != max_systems && ok; ++s) {
        if (Sys[s]->in_use) {
          for (i = 0; i != Sys[s]->cpus && ok; ++i) {
            if
            (
              SYS_PROGRAM.m_index >= 0 &&
              SYS_PROGRAM.net_x == net_x &&
              SYS_PROGRAM.net_y == net_y
            ) {
              ok = FALSE;
            }
          }
        }
      }
    }

    if (ok) {
      for (g = 0; g != max_guys && ok; ++g) {
        if
        (
          Port[g]->active &&
          Port[g]->access_level != IDLE &&
          Port[g]->net_x == net_x &&
          Port[g]->net_y == net_y
        ) {
          ok = FALSE;
        }
      }
    }

    if (!ok) {
      glob->cyberia_timer = CYBERIA_TIME3;
      EXIT0;
    } else {
      glob->cyberia_timer = CYBERIA_TIME2;
    }

    strcpy(world->city[net_x][net_y].name, CYBERIA);

    for (x = 0; x != MAX_CITY_X; ++x) {
      for (y = 0; y != MAX_CITY_Y; ++y) {
        if (RANDOM(3) == 1) {
          world->city[net_x][net_y].type[x][y] = CYBERIA_SYS;
        }
      }
    }

    glob->cyberia_x = net_x;
    glob->cyberia_y = net_y;
    EXIT0;
  }

  for (s = 0; s != max_systems && ok; ++s) {
    if
    (
      Sys[s]->in_use &&
      Sys[s]->net_x == glob->cyberia_x &&
      Sys[s]->net_y == glob->cyberia_y
    ) {
      ok = FALSE;
    }
  }

  if (ok) {
    for (g = 0; g != max_guys && ok; ++g) {
      if
      (
        Port[g]->active &&
        Port[g]->access_level != IDLE &&
        Port[g]->net_x == glob->cyberia_x &&
        Port[g]->net_y == glob->cyberia_y
      ) {
        ok = FALSE;
      }
    }
  }

  if (!ok) {
    glob->cyberia_timer = CYBERIA_TIME3;
    EXIT0;
  }

  glob->cyberia_timer = CYBERIA_TIME1;
  strcpy(world->city[glob->cyberia_x][glob->cyberia_y].name, FRONTIER);

  for (x = 0; x != MAX_CITY_X; ++x) {
    for (y = 0; y != MAX_CITY_Y; ++y) {
      world->city[glob->cyberia_x][glob->cyberia_y].type[x][y] = 0;
    }
  }

  glob->cyberia_x = -1;
  glob->cyberia_y = -1;
  EXIT0;
}
/****************************************************************************
    see if a Con kicks off a new hydra
*/
short Con_Actions(short s) {
  short   m;
  short   i;
  short   p;
  short   done;
  short   avail = -1;

  for (m = 0; m != Sys[s]->mus; ++m) {
    for (i = 0; i != MU_CAPACITY; ++i) {
      if
      (
        Sys[s]->memory[m][i].type == PROGRAM &&
        !Sys[s]->memory[m][i].running
      ) {
        Remove_From_Memory(s, m, i);
      }
    }
  }

  for (p = 0; p != Sys[s]->cpus; ++p) {
    if (Proc[s]->cpu[p].m_index < 0 && !Sys[s]->crash_time[p]) {
      avail = p;
    }
  }

  if (avail < 0) {
    EXIT0;
  }

  for (m = done = 0; m != Sys[s]->mus && !done; ++m) {
    for (i = 0; i != MU_CAPACITY && !done; ++i) {
      if (Sys[s]->memory[m][i].type == 0) {
        done = TRUE;
        p = con_programs[RANDOM(CON_PROGRAMS) - 1].index;
        Sys[s]->memory[m][i].index = p;
        Sys[s]->memory[m][i].type = PROGRAM;
        Sys[s]->memory[m][i].lock = FALSE;
        Sys[s]->memory[m][i].packed = FALSE;
        Sys[s]->memory[m][i].running = FALSE;
        Sys[s]->memory[m][i].value = Programs[p].value;
        Sys[s]->memory[m][i].class = Programs[p].class;
        Sys[s]->memory[m][i].strength = Programs[p].strength;
        Sys[s]->memory[m][i].size = 1;
        Sys[s]->memory[m][i].load = Programs[p].load;
        strcpy(Sys[s]->memory[m][i].name, Programs[p].Name);
        Configure_Con_Hydra(&Sys[s]->memory[m][i]);
        Run_System_Program(s, p);
      }
    }
  }

  EXIT0;
}
/****************************************************************************
    init the computer operated background systems (cons)
*/
void Init_Cons(void) {
  short   s;
  short   i;
  short   m;

  for (s = max_systems - CONS; s != max_systems; ++s) {
    Sys[s]->mus = MAX_MUS;
    Sys[s]->cpus = MAX_CPUS;

    for (i = 0; i != Sys[s]->cpus; ++i) {
      Proc[s]->cpu[i].m_index = -1;
      Sys[s]->crash_time[i] = 0;
    }

    for (m = 0; m != Sys[s]->mus; ++m) {
      for (i = 0; i != MU_CAPACITY; ++i) {
        Sys[s]->memory[m][i].type = 0;
      }
    }

    sprintf(Sys[s]->Name, "Unknown%d", s);
    Sys[s]->in_use = TRUE;
    Sys[s]->speed = CON_SYS;
    Sys[s]->wall = 0; /* originally? 32000; */
    Sys[s]->gate = 0; /* originally? 32000; */
    Sys[s]->type = CON_SYS;
    Sys[s]->net_x = -1;
    Sys[s]->net_y = -1;
    Sys[s]->city_x = -1;
    Sys[s]->city_y = -1;
  }
}
/****************************************************************************
    see if the netcops send out a robocop
*/
short Cop_Actions(void) {
  short   m;
  short   i;
  short   p;
  short   done;
  short   found;
  short   cop_x, cop_y;
  short   avail = -1;
  long    last_net_crime;
  short   s = glob->cop_sys;

  for (i = 0; i != Sys[s]->cpus; ++i) {
    if
    (
      SYS_PROGRAM.m_index >= 0 &&
      SYS_PROGRAM.target_index >= 0
    ) {
      for (myp = found = 0; myp != max_guys; ++myp) {
        if
        (
          MY_PORT->active &&
          MY_PORT->access_level != IDLE &&
          me.index == SYS_PROGRAM.target_index &&
          time(0) - me.last_net_crime < glob->copmins1 * 60
        ) {
          found = TRUE;
        }
      }

      if (!found) {
        if (SYS_PROGRAM.cop_x < 0) {
          Program_Stops(s, i);
        } else {
          SYS_PROGRAM.target_index = -1;
        }
      }
    }
  }

  for (i = 0; i != Sys[s]->cpus; ++i) {
    if
    (
      SYS_PROGRAM.m_index >= 0 &&
      SYS_PROGRAM.cop_x >= 0 &&
      SYS_PROGRAM.target_index < 0
    ) {
      cop_x = SYS_PROGRAM.cop_x;
      cop_y = SYS_PROGRAM.cop_y;
      last_net_crime = misc_world->city[cop_x][cop_y].last_net_crime;

      if (time(0) - last_net_crime > glob->copmins1 * 60) {
        Program_Stops(s, i);
      }
    }
  }

  for (i = 0; i != Sys[s]->cpus && avail < 0; ++i) {
    if (SYS_PROGRAM.m_index < 0 && !Sys[s]->crash_time[i]) {
      avail = i;
    }
  }

  if (avail < 0) {
    EXIT0;
  }

  for (m = done = 0; m != Sys[s]->mus && !done; ++m) {
    i = 0;

    if (Sys[s]->memory[m][i].type == 0) {
      done = TRUE;
      p = ROBOCOP;
      Sys[s]->memory[m][i].index = p;
      Sys[s]->memory[m][i].type = PROGRAM;
      Sys[s]->memory[m][i].lock = FALSE;
      Sys[s]->memory[m][i].packed = FALSE;
      Sys[s]->memory[m][i].running = FALSE;
      Sys[s]->memory[m][i].value = 1;
      Sys[s]->memory[m][i].class = Programs[p].class;
      Sys[s]->memory[m][i].strength = Programs[p].strength;
      Sys[s]->memory[m][i].strength += RANDOM(COP_STR);
      Sys[s]->memory[m][i].size = MU_CAPACITY;
      Sys[s]->memory[m][i].load = Programs[p].load;
      strcpy(Sys[s]->memory[m][i].name, Programs[p].Name);
      Configure_Cop_Hydra(&Sys[s]->memory[m][i]);
      Run_System_Program(s, p);
    } else if
    (
      Sys[s]->memory[m][i].type == PROGRAM &&
      Sys[s]->memory[m][i].index == ROBOCOP
    ) {
      if (!Sys[s]->memory[m][i].running) {
        done = TRUE;
        p = ROBOCOP;
        Run_System_Program(s, p);
      }
    } else {
      Remove_From_Memory(s, m, i);
    }
  }

  EXIT0;
}
/****************************************************************************
    see if the netcops take off after someone
*/
void Cop_Scan(void) {
  short   i;
  short   done;
  short   s = glob->cop_sys;

  for (myp = 0; myp != max_guys; ++myp) {
    if
    (
      MY_PORT->active &&
      MY_PORT->access_level != IDLE &&
      me.wanted &&
      time(0) - me.last_net_crime < glob->copmins1 * 60 &&
      RANDOM(60) == 1
    ) {
      for (i = 0; i != Sys[s]->cpus; ++i) {
        if
        (
          SYS_PROGRAM.m_index >= 0 &&
          SYS_PROGRAM.target_index < 0 &&
          SYS_PROGRAM.cop_x < 0
        ) {
          SYS_PROGRAM.target_index = me.index;
        }
      }
    }
  }

  for (i = done = 0; i != Sys[s]->cpus && !done; ++i) {
    if
    (
      SYS_PROGRAM.m_index >= 0 &&
      SYS_PROGRAM.target_index < 0 &&
      SYS_PROGRAM.net_x == SYS_PROGRAM.cop_x &&
      SYS_PROGRAM.net_y == SYS_PROGRAM.cop_y
    ) {
      for (myp = 0; myp != max_guys && !done; ++myp) {
        if
        (
          MY_PORT->active &&
          time(0) - me.last_net_crime < glob->copmins1 * 60 &&
          MY_PORT->net_x == SYS_PROGRAM.net_x &&
          MY_PORT->net_y == SYS_PROGRAM.net_y
        ) {
          SYS_PROGRAM.target_index = me.index;
          done = TRUE;
        }
      }
    }
  }
}
/****************************************************************************
    init the computer operated background netcops system
*/
void Init_Cops(void) {
  short   s = glob->cop_sys;
  short   g;
  short   i;
  short   x, y;
  short   dummy_s;
  short   dummy_c;
  short   done;
  struct  cpu_struct *runner;
  Init_System(Sys[s], COP_SYS);

  for (i = 0; i != MAX_CPUS + 1; ++i) {
    Proc[s]->cpu[i].m_index = -1;
  }

  strcpy(Sys[s]->Name, "The EYE");
  Sys[s]->in_use = TRUE;
  printf("Selecting city\n");

  do {
    do {
      x = RANDOM(MAX_NET_X) - 1;
      y = RANDOM(MAX_NET_Y) - 1;
      printf("x = %d, y = %d, name %s, valhalla %d %d, alien %d %d, cyberia %d %d\n", x, y,
             world->city[x][y].name, glob->valhalla_x, glob->valhalla_y, glob->alien_x, glob->alien_y,
             glob->cyberia_x, glob->cyberia_y);
    } while
    (
      !sameas(world->city[x][y].name, FRONTIER) ||
      (x == glob->valhalla_x && y == glob->valhalla_y) ||
      (x == glob->alien_x && y == glob->alien_y) ||
      (x == glob->cyberia_x && y == glob->cyberia_y)
    );

    printf("Found net %d %d\n", x, y);
    Sys[s]->net_x = x;
    Sys[s]->net_y = y;
    x = RANDOM(MAX_CITY_X) - 1;
    y = RANDOM(MAX_CITY_Y) - 1;

    if (world->city[Sys[s]->net_x][Sys[s]->net_y].type[x][y] == 0) {
      done = TRUE;
      runner = Find_Program
               (
                 Sys[s]->net_x, Sys[s]->net_y,
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
            Port[g]->net_x == Sys[s]->net_x &&
            Port[g]->net_y == Sys[s]->net_y &&
            Port[g]->city_x == x &&
            Port[g]->city_y == y
          ) {
            done = FALSE;
          }
        }
      }
    }
  } while (!done);

  printf("Found city\n");
  Sys[s]->city_x = x;
  Sys[s]->city_y = y;
  world->city[Sys[s]->net_x][Sys[s]->net_y].type[x][y] = COP_SYS;
  glob->cop_hq_x = Sys[s]->net_x;
  glob->cop_hq_y = Sys[s]->net_y;
}
/****************************************************************************
    move robocop towards it's assigned city
*/
short Cop_Seek_Remote_City(short s, short i) {
  short   g;
  short   dir;
  short   nailed;
  short   dummy_s, dummy_c;
  short   blocked = FALSE;
  short   city_x;
  short   city_y;
  short   seek_x = SYS_PROGRAM.seek_x;
  short   seek_y = SYS_PROGRAM.seek_y;
  short   cop_x = SYS_PROGRAM.cop_x;
  short   cop_y = SYS_PROGRAM.cop_y;
  short   net_x = SYS_PROGRAM.net_x;
  short   net_y = SYS_PROGRAM.net_y;
  short   old_net_x = SYS_PROGRAM.net_x;
  short   old_net_y = SYS_PROGRAM.net_y;
  struct  cpu_struct  *runner;

  if (net_y > cop_y) {
    dir = NORTH;

    if (seek_y != 0) {
      seek_y = -1;
    }
  } else if (net_y < cop_y) {
    dir = SOUTH;

    if (seek_y != MAX_SYS_Y - 1) {
      seek_y = -1;
    }
  } else if (net_x > cop_x) {
    dir = WEST;

    if (seek_x != 0) {
      seek_x = -1;
    }
  } else if (net_x < cop_x) {
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
  Update_Others
  (
    CITY_VIEW,
    net_x, net_y,
    SYS_PROGRAM.city_x, SYS_PROGRAM.city_y
  );
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
  EXIT0;
}
/****************************************************************************
    robocop moves to the city it was assigned to monitor
*/
short Cop_Seek_Remote(short s, short i) {
  if (SYS_PROGRAM.sys_x < 0 || SYS_PROGRAM.sys_y < 0) {
    if
    (
      SYS_PROGRAM.net_x == SYS_PROGRAM.cop_x &&
      SYS_PROGRAM.net_y == SYS_PROGRAM.cop_y
    ) {
      EXIT0;
    }

    Cop_Seek_Remote_City(s, i);
  } else {
    Seeker_Leaving_System(s, i);
  }

  EXIT0;
}
/****************************************************************************
    a con system hydra picks it's target
*/
short EXPORT Select_Con_Target(short s, short i) {
  short   g;
  short   lev;
  SYS_PROGRAM.target_index = -1;

  if (RANDOM(CON_ODDS) != 1) {
    EXIT0;
  }

  g = RANDOM(max_guys) - 1;

  if (Port[g]->active && Port[g]->access_level != IDLE) {
    lev = His_Rank(Port[g]->guy.score, Port[g]->guy.won);

    if (SYS_SOURCE.strength < lev) {
      SYS_PROGRAM.target_index = Port[g]->guy.index;
    }
  }

  EXIT0;
}
