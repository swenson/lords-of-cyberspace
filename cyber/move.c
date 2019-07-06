#include <header.h>
#include <globals.h>

/****************************************************************************
	place me in the specified coordinates, load the data
*/
short EXPORT Move_Me_To
(
  short   net_x,
  short   net_y,
  short   city_x,
  short   city_y,
  short   jackout,
  short   back
) {
  short   x;
  short   do_who = FALSE;
  short   left_system = FALSE;
  short   tries;
  short   old_cx = MY_PORT->city_x;
  short   old_cy = MY_PORT->city_y;
  short   old_sx = MY_PORT->sys_x;
  short   old_sy = MY_PORT->sys_y;
  short   dir;
  short   dummy_s, dummy_c;
  short   done;
  struct  cpu_struct *runner;

  if (command == NORTH) {
    dir = SOUTH;
  } else if (command == SOUTH) {
    dir = NORTH;
  } else if (command == EAST) {
    dir = WEST;
  } else {
    dir = EAST;
  }

  if (MY_PORT->access_level == SYSTEM_VIEW) {
    if
    (
      net_x >= 0 && net_x < MAX_NET_X &&
      net_y >= 0 && net_y < MAX_NET_Y &&
      city_x >= 0 && city_x < MAX_CITY_X &&
      city_y >= 0 && city_y < MAX_CITY_Y &&
      !jackout
    ) {
      if (world->city[net_x][net_y].type[city_x][city_y] == END_SYS) {
        if
        (
          !System_Move_Check
          (
            MY_PORT->net_x, MY_PORT->net_y,
            MY_PORT->city_x, MY_PORT->city_y
          )
        ) {
          EXIT0;
        }
      }
    }

    sprintf(str, glob->m54, me.alias);
    Cloakable_System_Message(str, MY_PORT->sys_num, myp);
    Leaving_System(jackout);
    left_system = TRUE;
  } else {
    if
    (
      net_x != MY_PORT->net_x ||
      net_y != MY_PORT->net_y ||
      jackout
    ) {
      sprintf(str, glob->m54, me.alias);
      City_Message(str, MY_PORT->net_x, MY_PORT->net_y, myp);
    }
  }

  if (jackout) {
    MY_PORT->access_level = IDLE;
    return (TRUE);
  }

  if (net_x != MY_PORT->net_x || net_y != MY_PORT->net_y || left_system) {
    do_who = TRUE;
    sprintf(str, glob->m53, me.alias);
    City_Message(str, net_x, net_y, myp);
  }

  MY_PORT->net_x = net_x;
  MY_PORT->net_y = net_y;
  MY_PORT->access_level = CITY_VIEW;

  if (!back) {
    MY_PORT->sys_x = -1;
    MY_PORT->sys_y = -1;
  }

  MY_PORT->city_x = city_x;
  MY_PORT->city_y = city_y;

  if
  (
    MY_CITY.type[MY_PORT->city_x][MY_PORT->city_y] != 0 &&
    MY_CITY.type[MY_PORT->city_x][MY_PORT->city_y] != PHONE
  ) {
    if
    (
      !System_Move_Check
      (
        MY_PORT->net_x, MY_PORT->net_y,
        MY_PORT->city_x, MY_PORT->city_y
      )
    ) {
      command = dir;
      Move_Me_To(net_x, net_y, old_cx, old_cy, jackout, back);
      return (FALSE);
    }

    tries = 0;
    Move_To_System
    (
      MY_PORT->net_x,
      MY_PORT->net_y,
      MY_PORT->city_x,
      MY_PORT->city_y
    );

    do {
      done = TRUE;

      if (back && MY_PORT->back_x >= 0) {
        MY_PORT->sys_x = MY_PORT->back_x;
        MY_PORT->sys_y = MY_PORT->back_y;
        MY_PORT->back_x = -1;
        MY_PORT->back_y = -1;
      } else if (command == EAST) {
        MY_PORT->sys_x = 0;
        MY_PORT->sys_y = RANDOM(MAX_SYS_Y) - 1;

        if (old_sy != -1) {
          MY_PORT->sys_y = old_sy;
        }
      } else if (command == WEST) {
        MY_PORT->sys_x = MAX_SYS_X - 1;
        MY_PORT->sys_y = RANDOM(MAX_SYS_Y) - 1;

        if (old_sy != -1) {
          MY_PORT->sys_y = old_sy;
        }
      } else if (command == NORTH) {
        MY_PORT->sys_x = RANDOM(MAX_SYS_X) - 1;
        MY_PORT->sys_y = MAX_SYS_Y - 1;

        if (old_sx != -1) {
          MY_PORT->sys_x = old_sx;
        }
      } else if (command == SOUTH) {
        MY_PORT->sys_x = RANDOM(MAX_SYS_X) - 1;
        MY_PORT->sys_y = 0;

        if (old_sx != -1) {
          MY_PORT->sys_x = old_sx;
        }
      }

      old_sx = -1;
      old_sy = -1;
      x = MY_SYS->node_type[MY_PORT->sys_x][MY_PORT->sys_y];

      if (x != 0) {
        if (x != PHONE) {
          done = FALSE;
        } else if (!back) {
          done = FALSE;
        }
      } else {
        runner = Find_Program
                 (
                   MY_PORT->net_x,  MY_PORT->net_y,
                   MY_PORT->city_x, MY_PORT->city_y,
                   MY_PORT->sys_x,  MY_PORT->sys_y,
                   &dummy_s, &dummy_c
                 );

        if (runner != NULL) {
          done = FALSE;
        }
      }

      ++tries;
    } while (tries < 15 && !done);

    if (tries >= 15) {
      Clear_Display_Window();
      prfmsg(MSG558);
      MY_PORT->sys_x = -1;
      MY_PORT->sys_y = -1;
      Leaving_System(!JACK);
      command = dir;
      Move_Me_To(net_x, net_y, old_cx, old_cy, jackout, back);
      return (FALSE);
    }

    Update_Location();
    sprintf(str, glob->m53, me.alias);
    Cloakable_System_Message(str, MY_PORT->sys_num, myp);
    MY_SYS->node_type[MY_PORT->sys_x][MY_PORT->sys_y] = A_PLAYER;
    Quick_Update
    (
      SYSTEM_VIEW,
      MY_PORT->net_x, MY_PORT->net_y,
      MY_PORT->city_x, MY_PORT->city_y,
      MY_PORT->sys_x, MY_PORT->sys_y,
      0, 0
    );
    Look("X");
    System_Who();

    if (left_system) {
      return (TRUE);
    }
  }

  Quick_Update
  (
    CITY_VIEW,
    MY_PORT->net_x, MY_PORT->net_y,
    MY_PORT->city_x, MY_PORT->city_y,
    old_cx, old_cy,
    MY_PORT->city_x, MY_PORT->city_y
  );

  if (left_system || do_who) {
    Look("X");
  }

  if (do_who) {
    City_Who();
  }

  return (TRUE);
}
/****************************************************************************
	user is moving
*/
short EXPORT Moving(void) {
  short   stopped;
  short   spaces = 1;

  if (!Phase_Check(1)) {
    Jack_Out();
    EXIT0;
  }

  if
  (
    margc > 1 &&
    MY_PORT->access_level == CITY_VIEW &&
    System_Is_Running(myp, NETSPEED) == 1
  ) {
    spaces = INUM(margv[1]);
  } else if (margc > 1 && me.perm[GAMEOP_PERM]) {
    spaces = INUM(margv[1]);
  }

  if (spaces < 1) {
    spaces = 1;
  } else if (spaces > 10) {
    spaces = 10;
  }

  if (MY_PORT->access_level == CITY_VIEW) {
    if
    (
      MY_NODE == PHONE &&
      MY_PORT->ld_moves + 1 != MAX_LD_LINKS &&
      !MY_PORT->icebolt_time
    ) {
      Clear_Display_Window();
      Prompt(getmsg(MSG559), LD_PROMPT);
    } else {
      if (MY_NODE == PHONE && !MY_PORT->icebolt_time) {
        Clear_Display_Window();
        prfmsg(MSG560);
      }

      do {
        stopped = City_Move();
        --spaces;
      } while (!stopped && spaces);
    }
  } else {
    if (MY_PORT->px != -1 && MY_PORT->ld_moves + 1 != MAX_LD_LINKS) {
      Clear_Display_Window();
      Prompt(getmsg(MSG559), SYS_LD_PROMPT);
    } else {
      if (MY_PORT->px != -1) {
        Clear_Display_Window();
        prfmsg(MSG560);
      }

      do {
        stopped = System_Move();
        --spaces;
      } while (spaces && !stopped);
    }
  }

  EXIT0;
}
/****************************************************************************
	see if the specified net jump is valid
*/
short Net_Move_Check(short spaces, short command) {
  short   net_y = MY_PORT->net_y;

  if (command == NORTH) {
    net_y -= spaces;
  } else if (command == SOUTH) {
    net_y += spaces;
  } else {
    return (TRUE);
  }

  if (net_y < 0 || net_y >= MAX_NET_Y) {
    Clear_Display_Window();
    prfmsg(MSG678A);
    EXIT0;
  }

  return (TRUE);
}
/****************************************************************************
	user is moving on the net level
*/
short EXPORT Net_Move(short spaces, short move_to_ld) {
  short   net_x, net_y;
  short   city_x, city_y;
  short   x, y;
  short   old_nx, old_ny;
  short   old_cx, old_cy;
  short   i;
  short   g;
  short   s, c;
  short   ok;
  short   found_ld = FALSE;
  struct  cpu_struct *runner;
  net_x = old_nx = MY_PORT->net_x;
  net_y = old_ny = MY_PORT->net_y;
  old_cx = MY_PORT->city_x;
  old_cy = MY_PORT->city_y;

  if (command == NORTH) {
    net_y -= spaces;
  } else if (command == SOUTH) {
    net_y += spaces;
  } else if (command == EAST) {
    net_x += spaces;
  } else if (command == WEST) {
    net_x -= spaces;
  }

  if (net_y < 0 || net_y >= MAX_NET_Y) {
    prfmsg(MSG678);
    EXIT0;
  }

  if (net_x >= MAX_NET_X) {
    net_x -= MAX_NET_X;
  } else if (net_x < 0) {
    net_x += MAX_NET_X;
  }

  if (move_to_ld) {
    MY_PORT->ld_path[MY_PORT->ld_moves].net_x = MY_PORT->net_x;
    MY_PORT->ld_path[MY_PORT->ld_moves].net_y = MY_PORT->net_y;
    MY_PORT->ld_path[MY_PORT->ld_moves].city_x = MY_PORT->city_x;
    MY_PORT->ld_path[MY_PORT->ld_moves].city_y = MY_PORT->city_y;

    if (MY_PORT->access_level == SYSTEM_VIEW) {
      MY_PORT->ld_path[MY_PORT->ld_moves].sys_x = MY_PORT->sys_x;
      MY_PORT->ld_path[MY_PORT->ld_moves].sys_y = MY_PORT->sys_y;
    } else {
      MY_PORT->ld_path[MY_PORT->ld_moves].sys_x = -1;
      MY_PORT->ld_path[MY_PORT->ld_moves].sys_y = -1;
    }

    ++MY_PORT->ld_moves;
    MY_PORT->walks[MY_PORT->ld_moves] = 1;
    MY_PORT->walked[MY_PORT->ld_moves][0].x = net_x;
    MY_PORT->walked[MY_PORT->ld_moves][0].y = net_y;
    city_x = 0;
    city_y = 0;

    for (x = 0; x != MAX_CITY_X; ++x) {
      for (y = 0; y != MAX_CITY_Y; ++y) {
        if (world->city[net_x][net_y].type[x][y] == PHONE) {
          city_x = x;
          city_y = y;
          found_ld = TRUE;
        }
      }
    }

    if (!found_ld) {
      do {
        ok = TRUE;
        city_x = RANDOM(MAX_CITY_X) - 1;
        city_y = RANDOM(MAX_CITY_Y) - 1;

        if (world->city[net_x][net_y].type[city_x][city_y]) {
          ok = FALSE;
        } else {
          for (g = 0; g != max_guys && ok; ++g) {
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
              ok = FALSE;
            }
          }

          if (ok) {
            runner = Find_Program
                     (
                       net_x, net_y,
                       city_x, city_y,
                       -1, -1,
                       &s, &c
                     );

            if (runner != NULL) {
              ok = FALSE;
            }
          }
        }
      } while (!ok);
    }
  } else {
    short   ldm = MY_PORT->ld_moves;

    for (i = 0; i < MY_PORT->walks[ldm]; ++i) {
      if
      (
        MY_PORT->walked[ldm][i].x == net_x &&
        MY_PORT->walked[ldm][i].y == net_y
      ) {
        MY_PORT->walks[ldm] = i;
      }
    }

    if (MY_PORT->walks[ldm] + 1 > MAX_MOVE) {
      Clear_Display_Window();
      prfmsg(MSG561);
      EXIT0;
    }

    if (command == NORTH) {
      city_x = MY_PORT->city_x;
      city_y = MAX_CITY_Y - 1;
    } else if (command == SOUTH) {
      city_x = MY_PORT->city_x;
      city_y = 0;
    } else if (command == EAST) {
      city_y = MY_PORT->city_y;
      city_x = 0;
    } else if (command == WEST) {
      city_y = MY_PORT->city_y;
      city_x = MAX_CITY_X - 1;
    }

    MY_PORT->walked[ldm][MY_PORT->walks[ldm]].x = net_x;
    MY_PORT->walked[ldm][MY_PORT->walks[ldm]].y = net_y;
    ++MY_PORT->walks[ldm];
  }

  runner = Find_Program
           (
             net_x, net_y,
             city_x, city_y,
             -1, -1,
             &s, &c
           );

  if (runner != NULL) {
    Clear_Display_Window();
    Ran_Into_Program(s, c);

    if (MY_PORT->walks[MY_PORT->ld_moves] > 1) {
      --MY_PORT->walks[MY_PORT->ld_moves];
    } else if (MY_PORT->ld_moves > 0) {
      --MY_PORT->ld_moves;
    }

    EXIT0;
  }

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
      Clear_Display_Window();
      Ran_Into_Guy(g);

      if (MY_PORT->walks[MY_PORT->ld_moves] > 1) {
        --MY_PORT->walks[MY_PORT->ld_moves];
      } else if (MY_PORT->ld_moves > 0) {
        --MY_PORT->ld_moves;
      }

      EXIT0;
    }
  }

  if (MY_PORT->icebolt_time) {
    Clear_Display_Window();
    prfmsg(MSG277, MY_PORT->icebolt_time);

    if (MY_PORT->walks[MY_PORT->ld_moves] > 1) {
      --MY_PORT->walks[MY_PORT->ld_moves];
    } else if (MY_PORT->ld_moves > 0) {
      --MY_PORT->ld_moves;
    }

    EXIT0;
  }

  Move_Me_To(net_x, net_y, city_x, city_y, !JACK, !BACK);
  Update_Location();
  Quick_Update
  (
    CITY_VIEW,
    old_nx, old_ny,
    old_cx, old_cy,
    old_cx, old_cy,
    0, 0
  );
  EXIT0;
}
/****************************************************************************
	user is moving on the city level
*/
short EXPORT City_Move(void) {
  short   x, y;
  short   s, c;
  short   g;
  short   stopped = TRUE;
  struct  cpu_struct *runner;
  x = MY_PORT->city_x;
  y = MY_PORT->city_y;

  if (command == NORTH) {
    --y;
  } else if (command == SOUTH) {
    ++y;
  } else if (command == EAST) {
    ++x;
  } else if (command == WEST) {
    --x;
  }

  if (y < 0) {
    command = NORTH;
    Net_Move(1, WALKED_NET);
    return (stopped);
  }

  if (y >= MAX_CITY_Y) {
    command = SOUTH;
    Net_Move(1, WALKED_NET);
    return (stopped);
  }

  if (x >= MAX_CITY_X) {
    command = EAST;
    Net_Move(1, WALKED_NET);
    return (stopped);
  }

  if (x < 0) {
    command = WEST;
    Net_Move(1, WALKED_NET);
    return (stopped);
  }

  for (g = 0; g != max_guys; ++g) {
    if
    (
      g != myp &&
      Port[g]->active &&
      Port[g]->access_level == CITY_VIEW &&
      Port[g]->net_x == MY_PORT->net_x &&
      Port[g]->net_y == MY_PORT->net_y &&
      Port[g]->city_x == x &&
      Port[g]->city_y == y
    ) {
      Clear_Display_Window();
      Ran_Into_Guy(g);
      return (stopped);
    }
  }

  runner = Find_Program
           (
             MY_PORT->net_x, MY_PORT->net_y,
             x, y,
             -1, -1,
             &s, &c
           );

  if (runner != NULL) {
    Clear_Display_Window();
    Ran_Into_Program(s, c);
    return (stopped);
  }

  if (MY_PORT->icebolt_time) {
    Clear_Display_Window();
    prfmsg(MSG277, MY_PORT->icebolt_time);
    return (stopped);
  }

  Move_Me_To(MY_PORT->net_x, MY_PORT->net_y, x, y, !JACK, !BACK);

  if (MY_PORT->access_level != CITY_VIEW) {
    return (stopped);
  }

  return (!stopped);
}
/****************************************************************************
	user is moving on the system level
*/
short EXPORT System_Move(void) {
  short   g;
  short   s, c;
  short   x, y;
  short   old_x = MY_PORT->sys_x;
  short   old_y = MY_PORT->sys_y;
  short   stopped = TRUE;
  short   off_gate = FALSE;
  struct  cpu_struct *runner;

  if (MY_PORT->access_level != SYSTEM_VIEW) {
    return (stopped);
  }

  x = MY_PORT->sys_x;
  y = MY_PORT->sys_y;

  if (command == NORTH) {
    --y;
  } else if (command == SOUTH) {
    ++y;
  } else if (command == EAST) {
    ++x;
  } else if (command == WEST) {
    --x;
  }

  if (y < 0) {
    command = NORTH;
    City_Move();
    return (stopped);
  }

  if (y >= MAX_SYS_Y) {
    command = SOUTH;
    City_Move();
    return (stopped);
  }

  if (x >= MAX_SYS_X) {
    command = EAST;
    City_Move();
    return (stopped);
  }

  if (x < 0) {
    command = WEST;
    City_Move();
    return (stopped);
  }

  runner = Find_Program
           (
             MY_PORT->net_x, MY_PORT->net_y,
             MY_PORT->city_x, MY_PORT->city_y,
             x, y,
             &s, &c
           );

  if (runner != NULL) {
    Clear_Display_Window();
    Ran_Into_Program(s, c);
    return (stopped);
  }

  if (MY_SYS->node_type[x][y] == MEMORY) {
    g = Memory_Contents(MY_SYS->memory[MY_SYS->node_index[x][y]], 1);

    if (g == 0) {
      MY_PORT->memory_bonk = FALSE;
    } else {
      MY_PORT->memory_bonk = TRUE;
      MY_PORT->source_s = MY_PORT->sys_num;
      MY_PORT->source_m = MY_SYS->node_index[x][y];
      Prompt(getmsg(MSG175A), MEMORY_PROMPT2);
    }

    return (stopped);
  }

  if (MY_PORT->icebolt_time) {
    Clear_Display_Window();
    prfmsg(MSG277, MY_PORT->icebolt_time);
    return (stopped);
  }

  if (MY_SYS->node_type[x][y] == 0) {
    MY_SYS->node_type[MY_PORT->sys_x][MY_PORT->sys_y] = 0;

    if (MY_PORT->gx != -1) {
      off_gate = TRUE;

      if (MY_NODE != BBS) {
        MY_SYS->node_type[MY_PORT->gx][MY_PORT->gy] = CLOSED_GATE;
      } else {
        MY_SYS->node_type[MY_PORT->gx][MY_PORT->gy] = OPENED_GATE;
      }

      Quick_Update
      (
        SYSTEM_VIEW,
        MY_PORT->net_x, MY_PORT->net_y,
        MY_PORT->city_x, MY_PORT->city_y,
        MY_PORT->gx, MY_PORT->gy,
        0, 0
      );
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

    MY_SYS->node_type[x][y] = A_PLAYER;
    MY_PORT->sys_x = x;
    MY_PORT->sys_y = y;
    Quick_Update
    (
      SYSTEM_VIEW,
      MY_PORT->net_x, MY_PORT->net_y,
      MY_PORT->city_x, MY_PORT->city_y,
      old_x, old_y,
      x, y
    );

    if
    (
      off_gate &&
      (MY_NODE == VENDOR || MY_NODE == FILE_SERVER) &&
      !Inside(myp)
    ) {
      MY_PORT->code[0] = 0;
    }

    File_Server_Check(MY_PORT->sys_num);
    Fence_Check(0);
    return (!stopped);
  }

  Clear_Display_Window();

  if
  (
    MY_SYS->node_type[x][y] == OPENED_GATE ||
    MY_SYS->node_type[x][y] == DEAD_WALL ||
    (
      MY_SYS->node_type[x][y] == CLOSED_GATE &&
      MY_SYS->type != VENDOR &&
      me.perm[GAMEOP_PERM]
    )
  ) {
    if (MY_SYS->node_type[x][y] == DEAD_WALL) {
      MY_PORT->code[0] = 0;
    }

    MY_SYS->node_type[MY_PORT->sys_x][MY_PORT->sys_y] = 0;

    if (MY_PORT->gx != -1) {
      if (MY_NODE != BBS) {
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

    if (MY_SYS->node_type[x][y] == DEAD_WALL) {
      MY_PORT->wx = x;
      MY_PORT->wy = y;
    } else {
      MY_PORT->gx = x;
      MY_PORT->gy = y;
    }

    MY_SYS->node_type[x][y] = A_PLAYER;
    MY_PORT->sys_x = x;
    MY_PORT->sys_y = y;
    Quick_Update
    (
      SYSTEM_VIEW,
      MY_PORT->net_x, MY_PORT->net_y,
      MY_PORT->city_x, MY_PORT->city_y,
      old_x, old_y,
      x, y
    );
  } else if (MY_SYS->node_type[x][y] == CLOSED_GATE) {
    MY_SYS->node_type[MY_PORT->sys_x][MY_PORT->sys_y] = 0;

    if (MY_PORT->sys_num == myp) {
      if (MY_PORT->gx != -1) {
        MY_SYS->node_type[MY_PORT->gx][MY_PORT->gy] = CLOSED_GATE;
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

      MY_SYS->node_type[x][y] = A_PLAYER;
      MY_PORT->sys_x = x;
      MY_PORT->sys_y = y;
      MY_PORT->gx = x;
      MY_PORT->gy = y;
      Quick_Update
      (
        SYSTEM_VIEW,
        MY_PORT->net_x, MY_PORT->net_y,
        MY_PORT->city_x, MY_PORT->city_y,
        old_x, old_y,
        x, y
      );
    } else if (System_Is_Running(MY_PORT->sys_num, CASHIER)) {
      if (Inside(myp)) {
        if (MY_PORT->code[0] == 0) {
          prfmsg(MSG562);
          return (stopped);
        }

        if (MY_PORT->gx != -1) {
          MY_SYS->node_type[MY_PORT->gx][MY_PORT->gy] = CLOSED_GATE;
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

        MY_PORT->gx = x;
        MY_PORT->gy = y;
        MY_SYS->node_type[x][y] = A_PLAYER;
        MY_PORT->sys_x = x;
        MY_PORT->sys_y = y;
        Quick_Update
        (
          SYSTEM_VIEW,
          MY_PORT->net_x, MY_PORT->net_y,
          MY_PORT->city_x, MY_PORT->city_y,
          old_x, old_y,
          x, y
        );
      } else if
      (
        System_Is_Running(myp, CLOAK) ||
        System_Is_Running(myp, MUFFLER)
      ) {
        MY_SYS->node_type[MY_PORT->sys_x][MY_PORT->sys_y] = A_PLAYER;
        prfmsg(MSG686);
      } else {
        MY_PORT->pwx = x;
        MY_PORT->pwy = y;
        MY_PORT->move_dir = command;
        MY_SYS->node_type[MY_PORT->sys_x][MY_PORT->sys_y] = A_PLAYER;
        Clear_Display_Window();
        Prompt(getmsg(MSG563), GO_VENDOR_PROMPT);
      }
    } else if
    (
      MY_NODE == FILE_SERVER &&
      !System_Is_Running(MY_PORT->sys_num, ANYTHING)
    ) {
      if (Inside(myp)) {
        if (MY_PORT->code[0] == 0) {
          prfmsg(MSG562);
          return (stopped);
        }

        if (MY_PORT->gx != -1) {
          MY_SYS->node_type[MY_PORT->gx][MY_PORT->gy] = CLOSED_GATE;
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

        MY_PORT->gx = x;
        MY_PORT->gy = y;
        MY_SYS->node_type[x][y] = A_PLAYER;
        MY_PORT->sys_x = x;
        MY_PORT->sys_y = y;
        Quick_Update
        (
          SYSTEM_VIEW,
          MY_PORT->net_x, MY_PORT->net_y,
          MY_PORT->city_x, MY_PORT->city_y,
          old_x, old_y,
          x, y
        );
      } else {
        MY_PORT->pwx = x;
        MY_PORT->pwy = y;
        MY_PORT->move_dir = command;
        MY_SYS->node_type[MY_PORT->sys_x][MY_PORT->sys_y] = A_PLAYER;
        Clear_Display_Window();
        Prompt(getmsg(MSG820), GO_SERVER_PROMPT);
      }
    } else if (MY_SYS->password[0] != 0) {
      MY_SYS->node_type[MY_PORT->sys_x][MY_PORT->sys_y] = A_PLAYER;
      MY_PORT->pwx = x;
      MY_PORT->pwy = y;
      Clear_Display_Window();

      if (!me.perm[GAMEOP_PERM]) {
        Prompt(getmsg(MSG675), PASSWORD_PROMPT);
      } else {
        sprintf(str, getmsg(MSG829), MY_SYS->password);
        Prompt(str, PASSWORD_PROMPT);
      }
    } else {
      MY_SYS->node_type[MY_PORT->sys_x][MY_PORT->sys_y] = A_PLAYER;
      prfmsg(MSG562);
    }

    return (stopped);
  } else if (MY_SYS->node_type[x][y] == CPU) {
    prfmsg(MSG564);
    return (stopped);
  } else if (MY_SYS->node_type[x][y] == DEAD_MEMORY) {
    prfmsg(MSG827);
    return (stopped);
  } else if (MY_SYS->node_type[x][y] == DATA_WALL) {
    prfmsg(MSG565);
    return (stopped);
  } else if (MY_SYS->node_type[x][y] == PHONE) {
    MY_PORT->px = x;
    MY_PORT->py = y;
    MY_SYS->node_type[MY_PORT->sys_x][MY_PORT->sys_y] = 0;

    if (MY_PORT->gx != -1) {
      MY_SYS->node_type[MY_PORT->gx][MY_PORT->gy] = CLOSED_GATE;
      MY_PORT->gx = -1;
      MY_PORT->gy = -1;
    }

    if (MY_PORT->wx != -1) {
      MY_SYS->node_type[MY_PORT->wx][MY_PORT->wy] = DEAD_WALL;
      MY_SYS->breached = TRUE;
      MY_PORT->wx = -1;
      MY_PORT->wy = -1;
    }

    MY_SYS->node_type[x][y] = A_PLAYER;
    MY_PORT->sys_x = x;
    MY_PORT->sys_y = y;
    Quick_Update
    (
      SYSTEM_VIEW,
      MY_PORT->net_x, MY_PORT->net_y,
      MY_PORT->city_x, MY_PORT->city_y,
      old_x, old_y,
      x, y
    );
  } else if (MY_SYS->node_type[x][y] == A_PLAYER) {
    for (g = 0; g < max_guys; ++g) {
      if
      (
        Port[g]->active &&
        Port[g]->sys_num == MY_PORT->sys_num &&
        Port[g]->sys_x == x &&
        Port[g]->sys_y == y
      ) {
        Clear_Display_Window();
        Ran_Into_Guy(g);
      }
    }
  } else {
    prfmsg(MSG566);
    return (stopped);
  }

  return (!stopped);
}
