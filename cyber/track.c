#include <header.h>
#include <globals.h>

/***************************************************************************
	see if I'm next to a target. If not, try to move to target
*/
short EXPORT Sys_Track
(
  short   s,
  struct  cpu_struct *runner,
  short   target_x,
  short   target_y,
  short   runner_s,
  short   *stuck
) {
  short   dir;
  short   next = 0;
  short   x, y;
  short   ok;
  short   old_x = runner->sys_x;
  short   old_y = runner->sys_y;
  *stuck = FALSE;
  x = abs(runner->sys_x - target_x);
  y = abs(runner->sys_y - target_y);

  if (x && y) {
  } else if (x > 1 || y > 1) {
  } else {
    return (TRUE);
  }

  dir = Find_System_Route
        (
          s,
          runner->sys_x, runner->sys_y,
          target_x, target_y,
          runner_s,
          &next,
          THEIR_SOURCE.index,
          THEIR_SOURCE.strength
        );

  if (dir == NORTH) {
    --runner->sys_y;
  } else if (dir == SOUTH) {
    ++runner->sys_y;
  } else if (dir == EAST) {
    ++runner->sys_x;
  } else if (dir == WEST) {
    --runner->sys_x;
  } else {
    *stuck = TRUE;
    EXIT0;
  }

  x = abs(runner->sys_x - target_x);
  y = abs(runner->sys_y - target_y);

  if (x && y) {
    ok = FALSE;
  } else if (x > 1 || y > 1) {
    ok = FALSE;
  } else {
    ok = TRUE;
  }

  if
  (
    !ok &&
    Sys[runner_s]->speed >= QUICK_SPEED &&
    next
  ) {
    if (next == NORTH) {
      --runner->sys_y;
    } else if (next == SOUTH) {
      ++runner->sys_y;
    } else if (next == EAST) {
      ++runner->sys_x;
    } else if (next == WEST) {
      --runner->sys_x;
    }
  }

  Quick_Update
  (
    SYSTEM_VIEW,
    runner->net_x, runner->net_y,
    runner->city_x, runner->city_y,
    old_x, old_y,
    runner->sys_x, runner->sys_y
  );
  x = abs(runner->sys_x - target_x);
  y = abs(runner->sys_y - target_y);

  if (x && y) {
    return (FALSE);
  }

  if (x > 1 || y > 1) {
    return (FALSE);
  }

  return (TRUE);
}
/***************************************************************************
	see if I'm next to a target. If not, try to move to target
*/
short EXPORT City_Track
(
  struct  cpu_struct *runner,
  short   target_x,
  short   target_y,
  short   s
) {
  short   dir;
  short   x, y;
  short   ok;
  short   next_dir = 0;
  short   old_x = runner->city_x;
  short   old_y = runner->city_y;
  x = abs(runner->city_x - target_x);
  y = abs(runner->city_y - target_y);

  if (x && y) {
  } else if (x > 1 || y > 1) {
  } else {
    return (TRUE);
  }

  dir = Find_City_Route
        (
          &world->city[runner->net_x][runner->net_y],
          runner->net_x, runner->net_y,
          runner->city_x, runner->city_y,
          target_x, target_y,
          &next_dir
        );

  if (dir == NORTH) {
    --runner->city_y;
  } else if (dir == SOUTH) {
    ++runner->city_y;
  } else if (dir == EAST) {
    ++runner->city_x;
  } else if (dir == WEST) {
    --runner->city_x;
  } else {
    runner->seek_x = -1;
    runner->seek_y = -1;
    EXIT0;
  }

  x = abs(runner->city_x - target_x);
  y = abs(runner->city_y - target_y);

  if (x && y) {
    ok = FALSE;
  } else if (x > 1 || y > 1) {
    ok = FALSE;
  } else {
    ok = TRUE;
  }

  if (!ok && next_dir && Sys[s]->speed >= QUICK_SPEED) {
    if (next_dir == NORTH) {
      --runner->city_y;
    } else if (next_dir == SOUTH) {
      ++runner->city_y;
    } else if (next_dir == EAST) {
      ++runner->city_x;
    } else if (next_dir == WEST) {
      --runner->city_x;
    }
  }

  Quick_Update
  (
    CITY_VIEW,
    runner->net_x, runner->net_y,
    runner->city_x, runner->city_y,
    old_x, old_y,
    runner->city_x, runner->city_y
  );
  x = abs(runner->city_x - target_x);
  y = abs(runner->city_y - target_y);

  if (x && y) {
    return (FALSE);
  }

  if (x > 1 || y > 1) {
    return (FALSE);
  }

  return (TRUE);
}
/**************************************************************************
	figure a path from point A to point B
*/
short EXPORT Find_System_Route
(
  short   s_num,
  short   from_x,
  short   from_y,
  short   to_x,
  short   to_y,
  short   runner_s,
  short   *next,
  short   index,
  short   strength
) {
  short   old_x, old_y;
  short   x, y;
  short   x1, y1;
  short   skip;
  short   count;
  short   dist;
  short   s;
  short   i;
  short   d;
  short   active = 1;
  short   done = FALSE;
  short   empty;

  if (from_y - 1 == to_y && from_x == to_x) {
    return (NORTH);
  }

  if (from_y + 1 == to_y && from_x == to_x) {
    return (SOUTH);
  }

  if (from_y == to_y && from_x + 1 == to_x) {
    return (EAST);
  }

  if (from_y == to_y && from_x - 1 == to_x) {
    return (WEST);
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      tracking->used_node[x][y].x = -1;
      tracking->used_node[x][y].y = -1;

      if (Sys[s_num]->node_type[x][y] == 0) {
        empty = TRUE;
      } else if
      (
        Sys[s_num]->node_type[x][y] == DEAD_WALL ||
        Sys[s_num]->node_type[x][y] == OPENED_GATE
      ) {
        empty = TRUE;
      } else if
      (
        Sys[s_num]->node_type[x][y] == CLOSED_GATE &&
        (
          s_num == runner_s ||
          Sys[runner_s]->type == COP_SYS ||
          Sys[runner_s]->type == CON_SYS
        )
      ) {
        empty = TRUE;
      } else if
      (
        Sys[s_num]->node_type[x][y] == CLOSED_GATE &&
        strength > Sys[s_num]->gate &&
        (
          index == DEVIL ||
          index == FIEND ||
          index == SATAN ||
          index == BANSHEE
        )
      ) {
        empty = TRUE;
      } else if
      (
        Sys[s_num]->node_type[x][y] == DATA_WALL &&
        s_num == runner_s &&
        (
          index == LIGHTNING ||
          index == FIREBOLT ||
          index == LOBOTOMY ||
          index == BANSHEE ||
          index == SOLDIER ||
          index == SATAN ||
          index == CASHIER ||
          index == FENCE ||
          index == MASON
        )
      ) {
        empty = TRUE;
      } else {
        empty = FALSE;
      }

      if (!empty) {
        tracking->used_node[x][y].x = -2;
        tracking->used_node[x][y].y = -2;
      }
    }
  }

  for (s = 0; s != max_systems; ++s) {
    if (Sys[s]->in_use) {
      for (i = 0; i != Sys[s]->cpus; ++i) {
        if
        (
          SYS_PROGRAM.m_index >= 0 &&
          SYS_PROGRAM.net_x == Sys[s_num]->net_x &&
          SYS_PROGRAM.net_y == Sys[s_num]->net_y &&
          SYS_PROGRAM.city_x == Sys[s_num]->city_x &&
          SYS_PROGRAM.city_y == Sys[s_num]->city_y
        ) {
          x = SYS_PROGRAM.sys_x;
          y = SYS_PROGRAM.sys_y;
          tracking->used_node[x][y].x = -2;
          tracking->used_node[x][y].y = -2;
        }
      }
    }
  }

  tracking->active_node[0].from_x = -2;
  tracking->active_node[0].from_y = -2;
  tracking->active_node[0].to_x = from_x;
  tracking->active_node[0].to_y = from_y;
  tracking->active_node[0].count = 0;
  tracking->active_node[0].dist = abs(from_x - to_x) + abs(from_y - to_y);
  tracking->used_node[from_x][from_y].x = -2;
  tracking->used_node[from_x][from_y].y = -2;

  do {
    x = tracking->active_node[active - 1].to_x;
    y = tracking->active_node[active - 1].to_y;
    count = tracking->active_node[active - 1].count;
    --active;
    x1 = x + 1;

    if (x1 < MAX_SYS_X) {
      if (x1 == to_x && y == to_y) {
        done = TRUE;
      } else if (tracking->used_node[x1][y].x == -1) {
        dist = abs(x1 - to_x) + abs(y - to_y);

        for (i = skip = index = 0; i != active && !skip; ++i) {
          d = tracking->active_node[i].count;
          d += tracking->active_node[i].dist;

          if
          (
            tracking->active_node[i].to_x == x &&
            tracking->active_node[i].to_y == y
          ) {
            skip = TRUE;
          } else if (count + dist + 1 < d) {
            index = i;
          }
        }

        if (!skip) {
          if (index) {
            ++index;
          }

          for (i = active; i > index; --i) {
            tracking->active_node[i] = tracking->active_node[i - 1];
          }

          tracking->active_node[index].from_x = x;
          tracking->active_node[index].from_y = y;
          tracking->active_node[index].to_x = x1;
          tracking->active_node[index].to_y = y;
          tracking->active_node[index].count = count + 1;
          tracking->active_node[index].dist = dist;
          tracking->used_node[x1][y].x = x;
          tracking->used_node[x1][y].y = y;
          ++active;
        }
      }
    }

    x1 = x - 1;

    if (x1 >= 0 && !done) {
      if (x1 == to_x && y == to_y) {
        done = TRUE;
      } else if (tracking->used_node[x1][y].x == -1) {
        dist = abs(x1 - to_x) + abs(y - to_y);

        for (i = skip = index = 0; i != active && !skip; ++i) {
          d = tracking->active_node[i].count;
          d += tracking->active_node[i].dist;

          if
          (
            tracking->active_node[i].to_x == x &&
            tracking->active_node[i].to_y == y
          ) {
            skip = TRUE;
          } else if (count + dist + 1 < d) {
            index = i;
          }
        }

        if (!skip) {
          if (index) {
            ++index;
          }

          for (i = active; i > index; --i) {
            tracking->active_node[i] = tracking->active_node[i - 1];
          }

          tracking->active_node[index].from_x = x;
          tracking->active_node[index].from_y = y;
          tracking->active_node[index].to_x = x1;
          tracking->active_node[index].to_y = y;
          tracking->active_node[index].count = count + 1;
          tracking->active_node[index].dist = dist;
          tracking->used_node[x1][y].x = x;
          tracking->used_node[x1][y].y = y;
          ++active;
        }
      }
    }

    y1 = y - 1;

    if (y1 >= 0 && !done) {
      if (x == to_x && y1 == to_y) {
        done = TRUE;
      } else if (tracking->used_node[x][y1].x == -1) {
        dist = abs(x - to_x) + abs(y1 - to_y);

        for (i = skip = index = 0; i != active && !skip; ++i) {
          d = tracking->active_node[i].count;
          d += tracking->active_node[i].dist;

          if
          (
            tracking->active_node[i].to_x == x &&
            tracking->active_node[i].to_y == y
          ) {
            skip = TRUE;
          } else if (count + dist + 1 < d) {
            index = i;
          }
        }

        if (!skip) {
          if (index) {
            ++index;
          }

          for (i = active; i > index; --i) {
            tracking->active_node[i] = tracking->active_node[i - 1];
          }

          tracking->active_node[index].from_x = x;
          tracking->active_node[index].from_y = y;
          tracking->active_node[index].to_x = x;
          tracking->active_node[index].to_y = y1;
          tracking->active_node[index].count = count + 1;
          tracking->active_node[index].dist = dist;
          tracking->used_node[x][y1].x = x;
          tracking->used_node[x][y1].y = y;
          ++active;
        }
      }
    }

    y1 = y + 1;

    if (y1 < MAX_SYS_Y && !done) {
      if (x == to_x && y1 == to_y) {
        done = TRUE;
      } else if (tracking->used_node[x][y1].x == -1) {
        dist = abs(x - to_x) + abs(y1 - to_y);

        for (i = skip = index = 0; i != active && !skip; ++i) {
          d = tracking->active_node[i].count;
          d += tracking->active_node[i].dist;

          if
          (
            tracking->active_node[i].to_x == x &&
            tracking->active_node[i].to_y == y
          ) {
            skip = TRUE;
          } else if (count + dist + 1 < d) {
            index = i;
          }
        }

        if (!skip) {
          if (index) {
            ++index;
          }

          for (i = active; i > index; --i) {
            tracking->active_node[i] = tracking->active_node[i - 1];
          }

          tracking->active_node[index].from_x = x;
          tracking->active_node[index].from_y = y;
          tracking->active_node[index].to_x = x;
          tracking->active_node[index].to_y = y1;
          tracking->active_node[index].count = count + 1;
          tracking->active_node[index].dist = dist;
          tracking->used_node[x][y1].x = x;
          tracking->used_node[x][y1].y = y;
          ++active;
        }
      }
    }

    if (active == 0 && !done) {
      EXIT0;
    }
  } while (!done);

  done = FALSE;
  *next = 0;

  do {
    if (from_y - 1 == y && from_x == x) {
      return (NORTH);
    }

    if (from_y + 1 == y && from_x == x) {
      return (SOUTH);
    }

    if (from_y == y && from_x + 1 == x) {
      return (EAST);
    }

    if (from_y == y && from_x - 1 == x) {
      return (WEST);
    }

    old_x = x;
    old_y = y;
    x = tracking->used_node[old_x][old_y].x;
    y = tracking->used_node[old_x][old_y].y;

    if (old_y - 1 == y && old_x == x) {
      *next = SOUTH;
    } else if (old_y + 1 == y && old_x == x) {
      *next = NORTH;
    } else if (old_y == y && old_x + 1 == x) {
      *next = WEST;
    } else if (old_y == y && old_x - 1 == x) {
      *next = EAST;
    }
  } while (!done);

  EXIT0;
}
/**************************************************************************
	figure a path from point A to point B
*/
short EXPORT Find_City_Route
(
  struct  city_struct *city,
  short   net_x,
  short   net_y,
  short   from_x,
  short   from_y,
  short   to_x,
  short   to_y,
  short   *next
) {
  short   s;
  short   g;
  short   i;
  short   old_x, old_y;
  short   x, y;
  short   x1, y1;
  short   skip;
  short   dist;
  short   active = 1;
  short   done = FALSE;
  short   count;
  short   index;
  short   d;

  if (from_y - 1 == to_y && from_x == to_x) {
    return (NORTH);
  }

  if (from_y + 1 == to_y && from_x == to_x) {
    return (SOUTH);
  }

  if (from_y == to_y && from_x + 1 == to_x) {
    return (EAST);
  }

  if (from_y == to_y && from_x - 1 == to_x) {
    return (WEST);
  }

  for (x = 0; x != MAX_CITY_X; ++x) {
    for (y = 0; y != MAX_CITY_Y; ++y) {
      if (city->type[x][y]) {
        tracking->used_node[x][y].x = -2;
        tracking->used_node[x][y].y = -2;
      } else {
        tracking->used_node[x][y].x = -1;
        tracking->used_node[x][y].y = -1;
      }
    }
  }

  for (s = 0; s != max_systems; ++s) {
    if (Sys[s]->in_use) {
      for (i = 0; i != Sys[s]->cpus; ++i) {
        if
        (
          SYS_PROGRAM.m_index >= 0 &&
          SYS_PROGRAM.net_x == net_x &&
          SYS_PROGRAM.net_y == net_y
        ) {
          x = SYS_PROGRAM.city_x;
          y = SYS_PROGRAM.city_y;
          tracking->used_node[x][y].x = -2;
          tracking->used_node[x][y].y = -2;
        }
      }
    }
  }

  for (g = 0; g != max_guys; ++g) {
    if
    (
      Port[g]->active &&
      Port[g]->access_level != IDLE &&
      Port[g]->net_x == net_x &&
      Port[g]->net_y == net_y
    ) {
      tracking->used_node[Port[g]->city_x][Port[g]->city_y].x = -2;
      tracking->used_node[Port[g]->city_x][Port[g]->city_y].y = -2;
    }
  }

  dist = abs(from_x - to_x) + abs(from_y - to_y);
  tracking->active_node[0].from_x = -2;
  tracking->active_node[0].from_y = -2;
  tracking->active_node[0].to_x = from_x;
  tracking->active_node[0].to_y = from_y;
  tracking->active_node[0].count = 0;
  tracking->active_node[0].dist = dist;
  tracking->used_node[from_x][from_y].x = -2;
  tracking->used_node[from_x][from_y].y = -2;

  do {
    x = tracking->active_node[active - 1].to_x;
    y = tracking->active_node[active - 1].to_y;
    count = tracking->active_node[active - 1].count;
    --active;
    x1 = x + 1;

    if (x1 < MAX_CITY_X) {
      if (x1 == to_x && y == to_y) {
        done = TRUE;
      } else if (tracking->used_node[x1][y].x == -1) {
        dist = abs(x1 - to_x) + abs(y - to_y);

        for (i = skip = index = 0; i != active && !skip; ++i) {
          d = tracking->active_node[i].count;
          d += tracking->active_node[i].dist;

          if
          (
            tracking->active_node[i].to_x == x &&
            tracking->active_node[i].to_y == y
          ) {
            skip = TRUE;
          } else if (count + dist + 1 < d) {
            index = i;
          }
        }

        if (!skip) {
          if (index) {
            ++index;
          }

          for (i = active; i > index; --i) {
            tracking->active_node[i] = tracking->active_node[i - 1];
          }

          tracking->active_node[index].from_x = x;
          tracking->active_node[index].from_y = y;
          tracking->active_node[index].to_x = x1;
          tracking->active_node[index].to_y = y;
          tracking->active_node[index].count = count + 1;
          tracking->active_node[index].dist = dist;
          tracking->used_node[x1][y].x = x;
          tracking->used_node[x1][y].y = y;
          ++active;
        }
      }
    }

    x1 = x - 1;

    if (x1 >= 0 && !done) {
      if (x1 == to_x && y == to_y) {
        done = TRUE;
      } else if (tracking->used_node[x1][y].x == -1) {
        dist = abs(x1 - to_x) + abs(y - to_y);

        for (i = skip = index = 0; i != active && !skip; ++i) {
          d = tracking->active_node[i].count;
          d += tracking->active_node[i].dist;

          if
          (
            tracking->active_node[i].to_x == x &&
            tracking->active_node[i].to_y == y
          ) {
            skip = TRUE;
          } else if (count + dist + 1 < d) {
            index = i;
          }
        }

        if (!skip) {
          if (index) {
            ++index;
          }

          for (i = active; i > index; --i) {
            tracking->active_node[i] = tracking->active_node[i - 1];
          }

          tracking->active_node[index].from_x = x;
          tracking->active_node[index].from_y = y;
          tracking->active_node[index].to_x = x1;
          tracking->active_node[index].to_y = y;
          tracking->active_node[index].count = count + 1;
          tracking->active_node[index].dist = dist;
          tracking->used_node[x1][y].x = x;
          tracking->used_node[x1][y].y = y;
          ++active;
        }
      }
    }

    y1 = y - 1;

    if (y1 >= 0 && !done) {
      if (x == to_x && y1 == to_y) {
        done = TRUE;
      } else if (tracking->used_node[x][y1].x == -1) {
        dist = abs(x - to_x) + abs(y1 - to_y);

        for (i = skip = index = 0; i != active && !skip; ++i) {
          d = tracking->active_node[i].count;
          d += tracking->active_node[i].dist;

          if
          (
            tracking->active_node[i].to_x == x &&
            tracking->active_node[i].to_y == y
          ) {
            skip = TRUE;
          } else if (count + dist + 1 < d) {
            index = i;
          }
        }

        if (!skip) {
          if (index) {
            ++index;
          }

          for (i = active; i > index; --i) {
            tracking->active_node[i] = tracking->active_node[i - 1];
          }

          tracking->active_node[index].from_x = x;
          tracking->active_node[index].from_y = y;
          tracking->active_node[index].to_x = x;
          tracking->active_node[index].to_y = y1;
          tracking->active_node[index].count = count + 1;
          tracking->active_node[index].dist = dist;
          tracking->used_node[x][y1].x = x;
          tracking->used_node[x][y1].y = y;
          ++active;
        }
      }
    }

    y1 = y + 1;

    if (y1 < MAX_CITY_Y && !done) {
      if (x == to_x && y1 == to_y) {
        done = TRUE;
      } else if (tracking->used_node[x][y1].x == -1) {
        dist = abs(x - to_x) + abs(y1 - to_y);

        for (i = skip = index = 0; i != active && !skip; ++i) {
          d = tracking->active_node[i].count;
          d += tracking->active_node[i].dist;

          if
          (
            tracking->active_node[i].to_x == x &&
            tracking->active_node[i].to_y == y
          ) {
            skip = TRUE;
          } else if (count + dist + 1 < d) {
            index = i;
          }
        }

        if (!skip) {
          if (index) {
            ++index;
          }

          for (i = active; i > index; --i) {
            tracking->active_node[i] = tracking->active_node[i - 1];
          }

          tracking->active_node[index].from_x = x;
          tracking->active_node[index].from_y = y;
          tracking->active_node[index].to_x = x;
          tracking->active_node[index].to_y = y1;
          tracking->active_node[index].count = count + 1;
          tracking->active_node[index].dist = dist;
          tracking->used_node[x][y1].x = x;
          tracking->used_node[x][y1].y = y;
          ++active;
        }
      }
    }

    if (active == 0 && !done) {
      EXIT0;
    }
  } while (!done);

  done = FALSE;
  *next = 0;

  do {
    if (from_y - 1 == y && from_x == x) {
      return (NORTH);
    }

    if (from_y + 1 == y && from_x == x) {
      return (SOUTH);
    }

    if (from_y == y && from_x + 1 == x) {
      return (EAST);
    }

    if (from_y == y && from_x - 1 == x) {
      return (WEST);
    }

    old_x = x;
    old_y = y;
    x = tracking->used_node[old_x][old_y].x;
    y = tracking->used_node[old_x][old_y].y;

    if (old_y - 1 == y && old_x == x) {
      *next = SOUTH;
    } else if (old_y + 1 == y && old_x == x) {
      *next = NORTH;
    } else if (old_y == y && old_x + 1 == x) {
      *next = WEST;
    } else if (old_y == y && old_x - 1 == x) {
      *next = EAST;
    }
  } while (!done);

  EXIT0;
}
