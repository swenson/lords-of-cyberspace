#include <header.h>
#include <globals.h>

/****************************************************************************
	return system name of runner
*/
char * EXPORT Runner_System_Name(short s, short i) {
  short   n;

  if (SYS_PROGRAM.sys_x < 0 || SYS_PROGRAM.sys_y < 0) {
    return (world->city[SYS_PROGRAM.net_x][SYS_PROGRAM.net_y].name);
  }

  for (n = 0; n != max_systems; ++n) {
    if
    (
      Sys[n]->in_use &&
      Sys[n]->net_x == SYS_PROGRAM.net_x &&
      Sys[n]->net_y == SYS_PROGRAM.net_y &&
      Sys[n]->city_x == SYS_PROGRAM.city_x &&
      Sys[n]->city_y == SYS_PROGRAM.city_y
    ) {
      return (Sys[n]->Name);
    }
  }

  return ("None");
}
/****************************************************************************
	Cashier locks the programs I can't afford to buy
*/
void Cashier_Lock(short s) {
  short   g;
  short   m;
  short   i;
  long    min_balance = 0;

  if (s >= max_guys && Sys[s]->type == VENDOR) {
    for (g = 0; g != max_guys; ++g) {
      if
      (
        Port[g]->active &&
        Port[g]->access_level == SYSTEM_VIEW &&
        Port[g]->sys_num == s
      ) {
        if (Port[g]->code[0] && Port[g]->balance > min_balance) {
          min_balance = Port[g]->balance;
        }
      }
    }

    for (m = 0; m != Sys[s]->mus; ++m) {
      for (i = 0; i != MU_CAPACITY; ++i) {
        if
        (
          Sys[s]->memory[m][i].type == PROGRAM &&
          !Sys[s]->memory[m][i].running
        ) {
          if (Sys[s]->memory[m][i].value <= min_balance) {
            Sys[s]->memory[m][i].lock = 0;
          } else {
            Sys[s]->memory[m][i].lock = VENDOR_LOCK;
          }
        }
      }
    }
  }
}
/****************************************************************************
	send a message to the audit trail
*/
void NOTIFY(txt)
char    *txt;
{
  short   oldusn;
  oldusn = usrnum;
  usrnum = -1;
  shocst(txt, "%s", txt);
  usrnum = oldusn;
}
/*****************************************************************************
   gets user logged in
*/
short Greet_User(void) {
  short   i;
  short   j;
  short   created;

  if (STATE == NEWGUY_PROMPT) {
    created = Newguy(1);

    if (!created) {
      EXIT0;
    }

    i = -1;
  } else {
    i = Read_My_Rec(my_record);

    if (i < 0) {
      printf("Couldn't find rec, and glob->ibm[%d] = %d\n", usrnum, glob->ibm[usrnum]);

      if (glob->ibm[usrnum] == -1) {
        prfmsg(MSG187, glob->theguy);
        STATE = IBM_PROMPT;
        EXIT0;
      }

      created = Newguy(0);
      printf("Created = %d\n", created);

      if (!created) {
        EXIT0;
      }
    }
  }

  printf("Sign me on\n");
  Sign_Me_On();
  printf("Out of sign me on with myp = %d\n", myp);

  if (myp == -1) {
    EXIT0;
  }

  memcpy(MY_DECK, a_system, SYSTEM_SIZE);
  prf("%c%s", CTRL_S, CLEAR_SCREEN);
  Clear_Net_Display(CLEAR_ALL);
  prfmsg
  (
    MSG188A,
    me.alias,
    rank[His_Rank(me.score, me.won)].title,
    His_Rank(me.score, me.won)
  );
  New_Mail_Check();
  prfmsg(MG188B, VERSION);

  if (i < 0) {
    for (j = 0; j != CODE_SIZE - 1; ++j) {
      me.internet.code[j] = RANDOM(26) - 1 + 'A';
      me.internet.code[j + 1] = 0;
    }

    me.internet.credit_rating  = 10;
    me.internet.owed           = 0;

    for (j = 0; j != CODE_SIZE - 1; ++j) {
      me.bank.code[j] = RANDOM(26) - 1 + 'A';
      me.bank.code[j + 1] = 0;
    }

    Write_My_Rec(NO_MATCH);

    if (glob->sysopkey[0] && haskey(glob->sysopkey)) {
      me.perm[SET_PERM] = TRUE;
    }
  }

  Payday_Check((i >= 0));
  prfmsg(MSG189);
  New_FreeNet();
  New_Gang_Mail();

  if (me.invite) {
    prfmsg(MSG752);
  }

  STATE = MAIN_PROMPT;
  Update_Location();

  if (MY_PORT->autojack) {
    Jack_In();
  }

  EXIT0;
}
/****************************************************************************
	return system number where a runner is running
*/
short Runner_System_Number(short s, short i) {
  short   n;

  for (n = 0; n != max_systems; ++n) {
    if
    (
      Sys[n]->in_use &&
      Sys[n]->net_x == SYS_PROGRAM.net_x &&
      Sys[n]->net_y == SYS_PROGRAM.net_y &&
      Sys[n]->city_x == SYS_PROGRAM.city_x &&
      Sys[n]->city_y == SYS_PROGRAM.city_y
    ) {
      return (n);
    }
  }

  return (-1);
}
/**************************************************************************
	find what's in the specified direction
*/
short Find_City_Space(char *where, short *x, short *y, short spaces) {
  short   i;
  short   g;
  short   dir;
  short   dummy_s, dummy_c;
  struct  cpu_struct *runner;

  if (sameto("N", where)) {
    dir = NORTH;
  } else if (sameto("S", where)) {
    dir = SOUTH;
  } else if (sameto("E", where)) {
    dir = EAST;
  } else if (sameto("W", where)) {
    dir = WEST;
  } else {
    return (-1);
  }

  if (MY_PORT->access_level != CITY_VIEW) {
    return (-1);
  }

  *x = MY_PORT->city_x;
  *y = MY_PORT->city_y;

  for (i = 0; i != spaces; ++i) {
    if (dir == NORTH) {
      --*y;
    } else if (dir == SOUTH) {
      ++*y;
    } else if (dir == EAST) {
      ++*x;
    } else if (dir == WEST) {
      --*x;
    }

    if (*y < 0 || *y >= MAX_CITY_Y || *x >= MAX_CITY_X || *x < 0) {
      return (-1);
    }

    if (MY_CITY.type[*x][*y]) {
      return (MY_CITY.type[*x][*y]);
    }

    runner = Find_Program
             (
               MY_PORT->net_x, MY_PORT->net_y,
               *x, *y,
               -1, -1,
               &dummy_s, &dummy_c
             );

    if (runner != NULL) {
      return (CONST);
    }

    for (g = 0; g != max_guys; ++g) {
      if
      (
        Port[g]->active &&
        Port[g]->access_level != IDLE &&
        Port[g]->net_x == MY_PORT->net_x &&
        Port[g]->net_y == MY_PORT->net_y &&
        Port[g]->city_x == *x &&
        Port[g]->city_y == *y
      ) {
        return (A_PLAYER);
      }
    }
  }

  return (0);
}
/**************************************************************************
	find what's in the specified direction
*/
short Find_System_Space(char *where, short *x, short *y, short spaces) {
  short   i;
  short   dir;
  short   dummy_s, dummy_c;
  struct  cpu_struct *runner;

  if (sameto("N", where)) {
    dir = NORTH;
  } else if (sameto("S", where)) {
    dir = SOUTH;
  } else if (sameto("E", where)) {
    dir = EAST;
  } else if (sameto("W", where)) {
    dir = WEST;
  } else {
    return (-1);
  }

  if (MY_PORT->access_level != SYSTEM_VIEW) {
    return (-1);
  }

  *x = MY_PORT->sys_x;
  *y = MY_PORT->sys_y;

  for (i = 0; i != spaces; ++i) {
    if (dir == NORTH) {
      --*y;
    } else if (dir == SOUTH) {
      ++*y;
    } else if (dir == EAST) {
      ++*x;
    } else if (dir == WEST) {
      --*x;
    }

    if (*y < 0 || *y >= MAX_SYS_Y || *x >= MAX_SYS_X || *x < 0) {
      return (-1);
    }

    runner = Find_Program
             (
               MY_PORT->net_x, MY_PORT->net_y,
               MY_PORT->city_x, MY_PORT->city_y,
               *x, *y,
               &dummy_s, &dummy_c
             );

    if (runner != NULL) {
      return (CONST);
    } else if (MY_SYS->node_type[*x][*y]) {
      return (MY_SYS->node_type[*x][*y]);
    }
  }

  return (0);
}
/****************************************************************************
	reset location display values (to force a redisplay)
*/
void Reset_Location_Values(void) {
  MY_PORT->old_net_x      = -2;
  MY_PORT->old_net_y      = -2;
  MY_PORT->old_city_x     = -2;
  MY_PORT->old_city_y     = -2;
  MY_PORT->old_gang_score = -1;
  MY_PORT->old_walks      = -1;
  MY_PORT->old_ld_moves   = -1;
  MY_PORT->old_damage     = -1;
  MY_PORT->old_score      = -1;
  MY_PORT->old_rez        = -1;
  MY_PORT->old_online     = -1;
}
/****************************************************************************
	terminate a long (ansi-imbedded) string at the 79th "real" character
*/
short Terminate_String(char *msg) {
  short   i;
  short   so_far = 0;
  strcpy(glob->buf, msg);

  for (i = 0; i < strlen(glob->buf); ++i) {
    if (glob->buf[i] == 0) {
      EXIT0;
    } else if (glob->buf[i] != 27) {
      ++so_far;

      if (so_far == 79) {
        glob->buf[i] = 0;
        EXIT0;
      }
    } else {
      do {
        ++i;
      } while (glob->buf[i] != 'm' && glob->buf[0] != 0);
    }
  }

  EXIT0;
}
/****************************************************************************
	player scored some points
*/
short Scored(short g, short score, short gang_score) {
  short   old_port = myp;
  short   old_rank = His_Rank(Port[g]->guy.score, Port[g]->guy.won);
  short   new_rank;

  if (tourney->phase == PREGAME || tourney->phase == POSTGAME) {
    EXIT0;
  }

  if (score < 0) {
    if (Port[g]->guy.score <= 0) {
      EXIT0;
    }

    if (Port[g]->guy.score < abs(score)) {
      score = (short) - (Port[g]->guy.score);
    }
  }

  Port[g]->guy.score += score;
  Port[g]->guy.gang_pts += gang_score;
  Port[g]->guy.gang_pts_today += gang_score;
  Port[g]->gang_pts += gang_score;
  myp = g;
  Update_Location();
  myp = old_port;

  if (score == -1) {
    strcpy(str, getmsg(MSSG610A));
  } else if (score < -1) {
    sprintf(str, getmsg(MSSG611A), abs(score));
  } else if (score == 1) {
    strcpy(str, getmsg(MSSG610));
  } else {
    sprintf(str, getmsg(MSSG611), score);
  }

  Send(str, g, FALSE);
  new_rank = His_Rank(Port[g]->guy.score, Port[g]->guy.won);

  if (new_rank > old_rank) {
    sprintf
    (
      str,
      getmsg(MSSG612),
      Port[g]->guy.alias,
      rank[new_rank].title,
      new_rank
    );
    Send(str, g, FALSE);

    if (!Port[g]->invisible) {
      sprintf
      (
        str,
        getmsg(MSSG652),
        Port[g]->guy.alias,
        new_rank,
        rank[new_rank].title
      );
      Global_Message(str, g);
    }
  }

  EXIT0;
}
/****************************************************************************
	player just won the game
*/
short Endgame(void) {
  short   rnk;
  me.won = TRUE;
  me.score += glob->egamscor;
  rnk = His_Rank(me.score, me.won);
  Update_Location();
  sprintf
  (
    str,
    getmsg(MSG716),
    me.alias,
    rank[rnk].title
  );
  Send(str, myp, TRUE);
  sprintf
  (
    str,
    getmsg(MSG717),
    me.alias,
    rank[rnk].title
  );
  Global_Message(str, myp);
  EXIT0;
}
/****************************************************************************
	see if a program is running at the specified system location
*/
struct cpu_struct * EXPORT Find_Program
(
  short   net_x,
  short   net_y,
  short   city_x,
  short   city_y,
  short   sys_x,
  short   sys_y,
  short   *sys_num,
  short   *cpu_num
) {
  short   s;
  short   i;

  for (s = 0; s != max_systems; ++s) {
    if (Sys[s]->in_use) {
      for (i = 0; i != Sys[s]->cpus; ++i) {
        if
        (
          SYS_PROGRAM.m_index >= 0 &&
          SYS_PROGRAM.net_x == net_x &&
          SYS_PROGRAM.net_y == net_y &&
          SYS_PROGRAM.city_x == city_x &&
          SYS_PROGRAM.city_y == city_y &&
          SYS_PROGRAM.sys_x == sys_x &&
          SYS_PROGRAM.sys_y == sys_y
        ) {
          *sys_num = s;
          *cpu_num = i;
          return (&SYS_PROGRAM);
        }
      }
    }
  }

  return (NULL);
}
/****************************************************************************
	program has arrived, see if any Sentries or Guardians are around to
	report it
*/
short Sentry_Check(short s, short i) {
  short   g;
  short   p;
  short   is_local;

  if (Hydra_Has(CLOAK, s, i) || Hydra_Has(MUFFLER, s, i)) {
    EXIT0;
  }

  for (g = 0; g != max_guys; ++g) {
    if (Sys[g]->in_use && g != s) {
      for (p = 0; p != Sys[g]->cpus; ++p) {
        if
        (
          GUY_PROGRAM.m_index >= 0 &&
          GUY_PROGRAM.net_x == SYS_PROGRAM.net_x &&
          GUY_PROGRAM.net_y == SYS_PROGRAM.net_y
        ) {
          is_local = FALSE;

          if (GUY_PROGRAM.sys_x < 0 && SYS_PROGRAM.sys_x < 0) {
            is_local = TRUE;
          } else if
          (
            GUY_PROGRAM.sys_x >= 0 &&
            SYS_PROGRAM.sys_x >= 0 &&
            GUY_PROGRAM.city_x == SYS_PROGRAM.city_x &&
            GUY_PROGRAM.city_y == SYS_PROGRAM.city_y
          ) {
            is_local = TRUE;
          }

          if
          (
            is_local &&
            (
              GUY_SOURCE.index == SENTRY ||
              GUY_SOURCE.index == GUARDIAN
            )
          ) {
            sprintf
            (
              str,
              getmsg(MSG47),
              GUY_SOURCE.name,
              SYS_SOURCE.name,
              Runner_System_Name(s, i)
            );
            Send(str, g, FALSE);
          }
        }
      }
    }
  }

  EXIT0;
}
/***************************************************************************
	add random subs to a Hydra
*/
void   Configure_Cop_Hydra(hydra)
struct memory_struct *hydra;
{
  short   d;
  short   n;
  short   j;
  short   ok;
  short   max_subs = MAX_SUBS;

  if (hydra->index == DEVIL) {
    max_subs = DEVIL_SUBS;
  } else if (hydra->index == FIEND) {
    max_subs = FIEND_SUBS;
  } else if (hydra->index == SATAN) {
    max_subs = SATAN_SUBS;
  }

  for (j = 0; j != MAX_SUBS; ++j) {
    hydra->sub[j] = 0;
  }

  for (d = 0; d != max_subs; ++d) {
    do {
      if (d != 0) {
        n = RANDOM(13);
      } else {
        n = RANDOM(2) + 11;
      }

      if (n == 1) {
        n = RADAR;
      } else if (n == 2) {
        n = CLOAK;
      } else if (n == 3) {
        n = MUFFLER;
      } else if (n == 4) {
        n = ARMOR;
      } else if (n == 5) {
        n = SHIELD;
      } else if (n == 6) {
        n = STATIK;
      } else if (n == 7) {
        n = GLUEGUN;
      } else if (n == 8) {
        n = TENTACLE;
      } else if (n == 9) {
        n = COBRA;
      } else if (n == 10) {
        n = PHASER;
      } else if (n == 11) {
        n = CATTLEPROD;
      } else if (n == 12) {
        n = CHAINSAW;
      } else if (n == 13) {
        n = SKULLDRILL;
      }

      for (j = 0, ok = TRUE; j != max_subs; ++j) {
        if (hydra->sub[j] == n) {
          ok = FALSE;
        }
      }
    } while (!ok);

    hydra->sub[d] = n;
  }
}
/***************************************************************************
	add random subs to a Hydra
*/
void   Configure_Con_Hydra(hydra)
struct memory_struct *hydra;
{
  short   d;
  short   n;
  short   j;
  short   ok;

  for (j = 0; j != MAX_SUBS; ++j) {
    hydra->sub[j] = 0;
  }

  for (d = 0; d != MAX_SUBS; ++d) {
    do {
      n = RANDOM(13);

      if (n == 1) {
        n = CLOAK;
      } else if (n == 2) {
        n = RADAR;
      } else if (n == 3) {
        n = MUFFLER;
      } else if (n == 4) {
        n = SHIELD;
      } else if (n == 5) {
        n = ARMOR;
      } else if (n == 6) {
        n = STATIK;
      } else if (n == 7) {
        n = CHAINSAW;
      } else if (n == 8) {
        n = SKULLDRILL;
      } else if (n == 9) {
        n = TENTACLE;
      } else if (n == 10) {
        n = CATTLEPROD;
      } else if (n == 11) {
        n = GLUEGUN;
      } else if (n == 12) {
        n = PHASER;
      } else if (n == 13) {
        n = COBRA;
      }

      for (j = 0, ok = TRUE; j != MAX_SUBS; ++j) {
        if (hydra->sub[j] == n) {
          ok = FALSE;
        }
      }
    } while (!ok);

    hydra->sub[d] = n;
  }
}
/***************************************************************************
	see if there is a local internet system
*/
short No_Local_Internet(void) {
  short   x, y;

  for (x = 0; x != MAX_CITY_X; ++x) {
    for (y = 0; y != MAX_CITY_Y; ++y) {
      if (MY_CITY.type[x][y] == HUB) {
        return (FALSE);
      }
    }
  }

  prfmsg(MSG465);
  return (TRUE);
}
/***************************************************************************
	long random numbers
*/
/*long LRAND(long n)
{
	short       x;
	long        y;
	long        val;
	long double ld;
	long double sq;

	if (n <= 0)
	{
		return (1L);
	}

	if (n <= 32767)
	{
		x = (short) n;
		val = RANDOM(x) + 1;
		return (val);
	}

	ld = n;
	sq = sqrtl(ld);

	x = (short) sq;
	val = RANDOM(x);
	y = (long) sq;
	val = val * y;
	y = RANDOM(x);
	val += y;
	++val;

	return (val);
}*/
/***************************************************************************
	my deck got destroyed/confiscated
*/
void Destroy_My_Deck(short confiscated) {
  short   cpu_index = 0;
  short   mem_index = 0;
  short   n;
  short   x, y;
  short   dead_cpu[MAX_CPUS + 1];
  short   dead_memory[MAX_MUS];

  for (n = 0; n != MAX_CPUS + 1; ++n) {
    dead_cpu[n] = FALSE;
  }

  for (n = 0; n != MAX_MUS; ++n) {
    dead_memory[n] = FALSE;
  }

  Stop_All_Programs(myp);

  for (n = MY_DECK->cpus - 1; n >= 0; --n) {
    if (confiscated || RANDOM(glob->zapodds) == 1) {
      dead_cpu[n] = TRUE;
      --MY_DECK->cpus;
    }
  }

  for (n = MY_DECK->mus - 1; n >= 0; --n) {
    if (confiscated || RANDOM(glob->zapodds) == 1) {
      dead_memory[n] = TRUE;
      --MY_DECK->mus;
    }
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      if
      (
        MY_DECK->node_type[x][y] == CPU &&
        dead_cpu[MY_DECK->node_index[x][y]]
      ) {
        MY_DECK->node_type[x][y] = 0;
        MY_DECK->node_index[x][y] = 0;
      } else if
      (
        MY_DECK->node_type[x][y] == MEMORY &&
        dead_memory[MY_DECK->node_index[x][y]]
      ) {
        MY_DECK->node_type[x][y] = 0;
        MY_DECK->node_index[x][y] = 0;
      }
    }
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      if (MY_DECK->node_type[x][y] == CPU) {
        MY_DECK->node_index[x][y] = cpu_index;
        ++cpu_index;
      } else if (MY_DECK->node_type[x][y] == MEMORY) {
        MY_DECK->node_index[x][y] = mem_index;
        ++mem_index;
      }
    }
  }

  if (confiscated || RANDOM(glob->zapodds) == 1) {
    me.interface = FALSE;
  }

  if (confiscated || RANDOM(glob->zapodds) == 1) {
    if (me.phone) {
      me.phone = FALSE;

      for (x = 0; x != MAX_SYS_X; ++x) {
        for (y = 0; y != MAX_SYS_Y; ++y) {
          if (MY_DECK->node_type[x][y] == PHONE) {
            MY_DECK->node_type[x][y] = 0;
          }
        }
      }
    }
  }

  if (MY_DECK->cpus == 0) {
    MY_DECK->speed = 0;
  }

  if (confiscated) {
    MY_DECK->wall = glob->stwall;
  } else if (RANDOM(glob->zapodds) == 1) {
    MY_DECK->wall -= RANDOM(MY_DECK->wall);

    if (MY_DECK->wall < glob->stwall) {
      MY_DECK->wall = glob->stwall;
    }
  }

  if (confiscated) {
    MY_DECK->gate = glob->stgate;
  } else if (RANDOM(glob->zapodds) == 1) {
    MY_DECK->gate -= RANDOM(MY_DECK->gate);

    if (MY_DECK->gate < glob->stgate) {
      MY_DECK->gate = glob->stgate;
    }
  }

  if (confiscated || RANDOM(glob->zapodds) == 1) {
    me.chip_reader = FALSE;
  }

  if (confiscated) {
    me.backup_chips = 0;
  }
}
/***************************************************************************
	see if there's a fence around
*/
short EXPORT Fence_Check(short option) {
  short   done = FALSE;
  short   x, y;
  short   score = 0;
  long    bux = 0;
  short   diff;
  short   s, i;
  short   m;
  struct  cpu_struct *runner;
  long    elapsed = time(0) - me.mission_time;

  if
  (
    MY_PORT->access_level != SYSTEM_VIEW ||
    MY_SYS->type != CRIMINAL ||
    Proc[MY_PORT->sys_num]->enemy[myp]
  ) {
    STATE = MAIN_PROMPT;
    EXIT0;
  }

  for (x = MY_PORT->sys_x - 1; x <= MY_PORT->sys_x + 1 && !done; ++x) {
    for (y = MY_PORT->sys_y - 1; y <= MY_PORT->sys_y + 1 && !done; ++y) {
      diff = abs(MY_PORT->sys_x - x);
      diff += abs(MY_PORT->sys_y - y);

      if
      (
        x >= 0 && x < MAX_SYS_X &&
        y >= 0 && y < MAX_SYS_Y &&
        diff == 1
      ) {
        runner = Find_Program
                 (
                   MY_PORT->net_x, MY_PORT->net_y,
                   MY_PORT->city_x, MY_PORT->city_y,
                   x, y,
                   &s, &i
                 );

        if (runner != NULL && SYS_SOURCE.index == FENCE) {
          done = TRUE;
        }
      }
    }
  }

  if (!done) {
    STATE = MAIN_PROMPT;
    EXIT0;
  }

  if (option == 0) {
    MY_PORT->source_m = 0;
    MY_PORT->p_num = 0;
  } else {
    if (sameas(margv[0], CANCEL)) {
      STATE = MAIN_PROMPT;
      EXIT0;
    } else if (sameas(margv[0], "A")) {
      for (m = MY_DECK->mus - 1; m >= 0; --m) {
        for (i = MU_CAPACITY - 1; i >= 0; --i) {
          if
          (
            me.mission_type == RETRIEVE_MISSION &&
            MY_DECK->memory[m][i].type == DOCUMENT &&
            sameas(me.mission_doc, MY_DECK->memory[m][i].name)
          ) {
            score += me.mission_score * MISSION_SCORE;
            bux += me.mission_score * MISSION_BUX;
            Remove_From_Memory(myp, m, i);
            me.mission_type = 0;
          } else if
          (
            MY_DECK->memory[m][i].type == DOCUMENT &&
            MY_DECK->memory[m][i].value
          ) {
            score += MY_DECK->memory[m][i].class - 1;
            bux += MY_DECK->memory[m][i].value;
            Remove_From_Memory(myp, m, i);
          }
        }
      }

      if (score) {
        Scored(myp, score, 0);
      }

      if (bux) {
        Add_Bank_Record
        (
          &me,
          bux,
          MY_PORT->net_x, MY_PORT->net_y,
          me.alias,
          "Fenced Docs"
        );
      }

      My_Outprf(MY_PORT->usrnum);
      prf(NL);
      sprintf(str, getmsg(MSG801), bux);
      prf(str);

      if
      (
        !me.mission_type ||
        elapsed > MISSION_TIME ||
        me.perm[GAMEOP_PERM]
      ) {
        Prompt(getmsg(MSG727), MISSION_PROMPT);
      } else {
        STATE = MAIN_PROMPT;
      }

      EXIT0;
    } else if (sameas(margv[0], ANS_YES)) {
      m = MY_PORT->source_m;
      i = MY_PORT->p_num;

      if
      (
        m < MY_DECK->mus &&
        MY_DECK->memory[m][i].type == DOCUMENT &&
        MY_DECK->memory[m][i].value
      ) {
        score = 0;
        bux = MY_DECK->memory[m][i].value;

        if
        (
          me.mission_type == RETRIEVE_MISSION &&
          MY_DECK->memory[m][i].type == DOCUMENT &&
          sameas(me.mission_doc, MY_DECK->memory[m][i].name)
        ) {
          score = me.mission_score * MISSION_SCORE;
          bux = me.mission_score * MISSION_BUX;
          me.mission_type = 0;
        } else if (MY_DECK->memory[m][i].class - 1 > 0) {
          score = MY_DECK->memory[m][i].class - 1;
        }

        if (score) {
          Scored(myp, score, 0);
        }

        Add_Bank_Record
        (
          &me,
          bux,
          MY_PORT->net_x, MY_PORT->net_y,
          me.alias,
          "Fenced Doc"
        );
        Remove_From_Memory(myp, m, i);
      }
    } else if (sameas(margv[0], ANS_NO)) {
      ++MY_PORT->p_num;

      if (MY_PORT->p_num >= MU_CAPACITY) {
        MY_PORT->p_num = 0;
        ++MY_PORT->source_m;

        if (MY_PORT->source_m >= MY_DECK->mus) {
          if
          (
            !me.mission_type ||
            elapsed > MISSION_TIME ||
            me.perm[GAMEOP_PERM]
          ) {
            Prompt(getmsg(MSG727), MISSION_PROMPT);
          } else {
            STATE = MAIN_PROMPT;
          }

          EXIT0;
        }
      }
    } else {
      EXIT0;
    }
  }

  for (m = MY_PORT->source_m; m < MY_DECK->mus; ++m) {
    for (i = MY_PORT->p_num; i < MU_CAPACITY; ++i) {
      MY_PORT->p_num = 0;

      if
      (
        MY_DECK->memory[m][i].type == DOCUMENT &&
        MY_DECK->memory[m][i].value
      ) {
        MY_PORT->source_m = m;
        MY_PORT->p_num = i;

        if
        (
          me.mission_type == RETRIEVE_MISSION &&
          MY_DECK->memory[m][i].type == DOCUMENT &&
          sameas(me.mission_doc, MY_DECK->memory[m][i].name)
        ) {
          bux = me.mission_score * MISSION_BUX;
        } else {
          bux = MY_DECK->memory[m][i].value;
        }

        sprintf
        (
          str,
          getmsg(MX466),
          THEIR_SOURCE.name,
          bux,
          MY_DECK->memory[m][i].name
        );
        Clear_Display_Window();
        Prompt(str, FENCE_PROMPT);
        EXIT0;
      }
    }
  }

  if (!me.mission_type || elapsed > MISSION_TIME || me.perm[GAMEOP_PERM]) {
    Prompt(getmsg(MSG727), MISSION_PROMPT);
  } else {
    STATE = MAIN_PROMPT;
  }

  EXIT0;
}
/***************************************************************************
	add a banking record to a user's ledger
*/
void Add_Bank_Record
(
  struct  human_struct *a_human,
  long    amount,
  short   net_x,
  short   net_y,
  char    *alias,
  char    *action
) {
  short   i;
  a_human->bank.balance += amount;

  if (a_human->bank.balance > 9999999L) {
    a_human->bank.balance = 9999999L;
  }

  for (i = 1; i != MAX_LEDGER; ++i) {
    a_human->bank.ledger[i - 1] = a_human->bank.ledger[i];
  }

  a_human->bank.ledger[MAX_LEDGER - 1].balance = a_human->bank.balance;
  a_human->bank.ledger[MAX_LEDGER - 1].amount  = amount;
  a_human->bank.ledger[MAX_LEDGER - 1].date    = today();
  a_human->bank.ledger[MAX_LEDGER - 1].time    = now();
  a_human->bank.ledger[MAX_LEDGER - 1].net_x   = net_x;
  a_human->bank.ledger[MAX_LEDGER - 1].net_y   = net_y;
  strcpy(a_human->bank.ledger[MAX_LEDGER - 1].alias, alias);
  strncpy
  (
    a_human->bank.ledger[MAX_LEDGER - 1].action,
    action,
    ACTION_SIZE - 1
  );
  a_human->bank.ledger[MAX_LEDGER - 1].action[ACTION_SIZE - 1] = 0;
  Write_User(a_human);
}
/***************************************************************************
	add an internet record to a user's ledger
*/
void Add_Internet_Record
(
  struct  human_struct *a_human,
  long    amount,
  short   net_x,
  short   net_y,
  char    *alias,
  char    *action
) {
  short   i;
  a_human->internet.owed += amount;

  if (a_human->internet.owed > 9999999L) {
    a_human->internet.owed = 9999999L;
  }

  for (i = 1; i != MAX_LEDGER; ++i) {
    a_human->internet.ledger[i - 1] = a_human->internet.ledger[i];
  }

  a_human->internet.ledger[MAX_LEDGER - 1].balance = a_human->internet.owed;
  a_human->internet.ledger[MAX_LEDGER - 1].amount  = amount;
  a_human->internet.ledger[MAX_LEDGER - 1].date    = today();
  a_human->internet.ledger[MAX_LEDGER - 1].time    = now();
  a_human->internet.ledger[MAX_LEDGER - 1].net_x   = net_x;
  a_human->internet.ledger[MAX_LEDGER - 1].net_y   = net_y;
  strcpy(a_human->internet.ledger[MAX_LEDGER - 1].alias, alias);
  strncpy
  (
    a_human->internet.ledger[MAX_LEDGER - 1].action,
    action,
    ACTION_SIZE - 1
  );
  a_human->internet.ledger[MAX_LEDGER - 1].action[ACTION_SIZE - 1] = 0;
  Write_User(a_human);
}
/***************************************************************************
	return my rent cost
*/
short Rent(void) {
  short   r = glob->minrent;

  if (world->city[me.start_x][me.start_y].landmass == NAMERICA) {
    r += glob->rentadd;
  } else if (world->city[me.start_x][me.start_y].landmass == EUROPE) {
    r += glob->rentadd * 2;
  } else if (world->city[me.start_x][me.start_y].landmass == SAMERICA) {
    r -= glob->rentadd;
  } else if (world->city[me.start_x][me.start_y].landmass == AFRICA) {
    r -= glob->rentadd;
  } else if (world->city[me.start_x][me.start_y].landmass == ASIA) {
    r += glob->rentadd * 3;
  }

  return (r);
}
/***************************************************************************
	assign a new criminal mission
*/
short New_Mission(void) {
  short   i;
  short   m;
  short   net_x;
  short   net_y;
  short   x, y;
  short   tries = 0;
  short   has_document = FALSE;
  short   done;
  FILE    *fp;
  short   n;

  do {
    net_x = RANDOM(MAX_NET_X) - 1;
    net_y = RANDOM(MAX_NET_Y) - 1;
  } while
  (
    sameas(world->city[net_x][net_y].name, FRONTIER) ||
    sameas(world->city[net_x][net_y].name, VALHALLA) ||
    sameas(world->city[net_x][net_y].name, ALIEN_CITY) ||
    sameas(world->city[net_x][net_y].name, CYBERIA) ||
    (world->city[net_x][net_y].invisible && !me.perm[GAMEOP_PERM])
  );

  do {
    x = RANDOM(MAX_CITY_X) - 1;
    y = RANDOM(MAX_CITY_Y) - 1;
    ++tries;

    if (tries > 100) {
      EXIT0;
    }

    if
    (
      world->city[net_x][net_y].type[x][y] == 0 ||
      world->city[net_x][net_y].type[x][y] == PHONE ||
      world->city[net_x][net_y].type[x][y] == CYBERDECK
    ) {
      done = FALSE;
    } else {
      done = TRUE;
    }
  } while (!done);

  Set_System_Path(x, y, net_x, net_y);
  fp = fopen(glob->path, READB);

  if (fp == NULL) {
    prf("File Open Failed at %d/%d %d/%d\r", net_x, net_y, x, y);
    EXIT0;
  }

  n = fread(a_system, SYSTEM_SIZE, 1, fp);
  fclose(fp);

  if (n != 1) {
    prf("File Read Failed at %d/%d %d/%d\r", net_x, net_y, x, y);
    EXIT0;
  }

  strcpy(me.mission_system, a_system->Name);

  for (i = 0; i != SYS_TYPES; ++i) {
    if (System[i].type == a_system->type) {
      me.mission_score = System[i].power;
    }
  }

  me.mission_x = net_x;
  me.mission_y = net_y;
  me.mission_time = time(0);
  me.mission_doc[0] = 0;

  if (!a_system->permanant && a_system->cpus) {
    for (m = 0; m != a_system->mus; ++m) {
      for (i = 0; i != MU_CAPACITY; ++i) {
        if (a_system->memory[m][i].type == DOCUMENT) {
          has_document = TRUE;

          if
          (
            RANDOM(10) == 1 &&
            a_system->memory[m][i].class != USELESS_DOC
          ) {
            strcpy(me.mission_doc, a_system->memory[m][i].name);
          }
        }
      }
    }
  }

  if (me.mission_doc[0]) {
    me.mission_type = RETRIEVE_MISSION;
    ++me.mission_score;
    fp = fopen(glob->path, READB);
    a_system->rebuild_time = time(0) + MISSION_TIME;
    a_system->last_write = time(0);

    for (i = 0; i != a_system->cpus; ++i) {
      a_system->crash_time[i] = 0;
    }

    fwrite(a_system, SYSTEM_SIZE, 1, fp);
    fclose(fp);
  } else if (has_document && RANDOM(2) == 1) {
    me.mission_type = ERASE_MISSION;
    fp = fopen(glob->path, READB);
    a_system->rebuild_time = time(0) + MISSION_TIME;
    a_system->last_write = time(0);

    for (i = 0; i != a_system->cpus; ++i) {
      a_system->crash_time[i] = 0;
    }

    fwrite(a_system, SYSTEM_SIZE, 1, fp);
    fclose(fp);
  } else {
    me.mission_type = CRASH_MISSION;
    me.mission_score += a_system->cpus;
  }

  Current_Mission();
  EXIT0;
}
/***************************************************************************
	display a list of cities
*/
short Display_Cities(void) {
  short   net_x;
  short   net_y;
  short   col;
  short   n = 1;

  for (net_x = col = 0; net_x != MAX_NET_X; ++net_x) {
    for (net_y = 0; net_y != MAX_NET_Y; ++net_y) {
      if
      (
        !sameas(world->city[net_x][net_y].name, FRONTIER) &&
        !sameas(world->city[net_x][net_y].name, VALHALLA) &&
        !sameas(world->city[net_x][net_y].name, ALIEN_CITY) &&
        !sameas(world->city[net_x][net_y].name, CYBERIA) &&
        (!world->city[net_x][net_y].invisible || me.perm[GAMEOP_PERM])
      ) {
        prfmsg(MSG467, n, world->city[net_x][net_y].name);
        ++col;
        ++n;

        if (col == 4) {
          prf(NL);
          col = 0;
        }
      }
    }
  }

  if (col) {
    prf(NL);
  }

  return (n - 1);
}
/****************************************************************************
	fix up a typed alias
*/
void Fix_Alias(name)
char *name;
{
  short   i;
  rstrin();
  strncpy(name, input, NAME_SIZE - 1);
  name[NAME_SIZE - 1] = 0;

  for (i = 0; i < strlen(name); ++i) {
    if (name[i] == ' ') {
      name[i] = '_';
    }
  }
}
/****************************************************************************
	shorten a userid
*/
char *Brief_Userid(userid)
char *userid;
{
  strcpy(temp_userid, userid);
  temp_userid[10] = 0;
  return (temp_userid);
}
/***************************************************************************
	see if there is a gablink around
*/
short Local_Gablink(short g) {
  short   s;
  short   i;
  short   n;
  short   is_local;

  for (s = 0; s != max_systems; ++s) {
    if (Sys[s]->in_use || s < max_guys) {
      for (i = 0; i != Sys[s]->cpus; ++i) {
        if
        (
          SYS_PROGRAM.m_index >= 0 &&
          SYS_SOURCE.index == GABLINK &&
          SYS_PROGRAM.net_x == Port[g]->net_x &&
          SYS_PROGRAM.net_y == Port[g]->net_y
        ) {
          is_local = FALSE;
          n = Runner_System_Number(s, i);

          if
          (
            (SYS_PROGRAM.sys_x == -1 || SYS_PROGRAM.sys_y == -1) &&
            Port[g]->access_level == CITY_VIEW
          ) {
            is_local = TRUE;
          } else if
          (
            SYS_PROGRAM.sys_x != -1 &&
            SYS_PROGRAM.sys_y != -1 &&
            Port[g]->access_level == SYSTEM_VIEW &&
            Port[g]->sys_num == n
          ) {
            is_local = TRUE;
          }

          if (is_local) {
            return (TRUE);
          }
        }
      }
    }
  }

  return (FALSE);
}
/***************************************************************************
	see if there are any targets for a knight around
*/
short Knight_Target(short s, short *sys_num, short *cpu_num) {
  short   g;
  short   p;

  for (g = 0; g != max_systems; ++g) {
    if ((Sys[g]->in_use || g < max_guys) && g != s) {
      for (p = 0; p != Sys[g]->cpus; ++p) {
        if
        (
          GUY_PROGRAM.m_index >= 0 &&
          GUY_PROGRAM.net_x == Sys[s]->net_x &&
          GUY_PROGRAM.net_y == Sys[s]->net_y &&
          GUY_PROGRAM.city_x == Sys[s]->city_x &&
          GUY_PROGRAM.city_y == Sys[s]->city_y &&
          GUY_PROGRAM.sys_x != -1 &&
          GUY_PROGRAM.sys_y != -1 &&
          (
            GUY_SOURCE.class == HYDRA ||
            GUY_SOURCE.index == BULLDOZER ||
            GUY_SOURCE.index == GATECRASH ||
            GUY_SOURCE.index == BLOB ||
            GUY_SOURCE.index == CRASH ||
            GUY_SOURCE.index == BARRICADE ||
            GUY_SOURCE.index == KNIGHT ||
            GUY_SOURCE.index == ADDLE ||
            GUY_SOURCE.index == TORPEDO ||
            GUY_SOURCE.index == HERCULES ||
            GUY_SOURCE.index == UNLOCKER ||
            GUY_SOURCE.index == GABLINK ||
            GUY_SOURCE.index == GUARDIAN ||
            GUY_SOURCE.index == SOLDIER ||
            GUY_SOURCE.index == MASON ||
            GUY_SOURCE.index == SENTRY ||
            GUY_SOURCE.index == ANTIVIRUS ||
            GUY_SOURCE.index == CASHIER ||
            GUY_SOURCE.index == FENCE ||
            GUY_SOURCE.index == SNOUT ||
            GUY_SOURCE.index == PATRIOT ||
            GUY_SOURCE.index == TRACE ||
            GUY_SOURCE.index == CAMOFLAGE
          )
        ) {
          *sys_num = g;
          *cpu_num = p;
          return (TRUE);
        }
      }
    }
  }

  return (FALSE);
}
/***************************************************************************
	see if a system is running the specified program
*/
short EXPORT System_Is_Running(short s, short p) {
  short   i;

  if (s < 0 || s >= max_systems) {
    return (FALSE);
  }

  for (i = 0; i != Sys[s]->cpus; ++i) {
    if (SYS_PROGRAM.m_index >= 0) {
      if (p == ANYTHING) {
        return (TRUE);
      }

      if (SYS_SOURCE.index == p) {
        if (SYS_PROGRAM.loading == 0) {
          return (TRUE);
        } else {
          return (2);
        }
      }
    }
  }

  return (FALSE);
}
/***************************************************************************
	see if I have a trace on a guy
*/
short EXPORT Traced(short g) {
  short   i;

  for (i = 0; i != MY_DECK->cpus; ++i) {
    if
    (
      MY_PROGRAM.m_index >= 0 &&
      MY_SOURCE.index == TRACE &&
      MY_PROGRAM.target_s == g &&
      !MY_PROGRAM.loading
    ) {
      return (TRUE);
    }
  }

  return (FALSE);
}
/****************************************************************************
	figure out loading for a program
*/
short EXPORT Loading_Time(short s, short m_index, short p_index) {
  short   t;
  t = Sys[s]->memory[m_index][p_index].load - Sys[s]->speed;

  if (t < 3) {
    t = 3;
  }

  return (t);
}
/****************************************************************************
	figure out what rank I am
*/
short EXPORT His_Rank(long score, short won) {
  short   i;

  if (!won) {
    for (i = 0; i != NUM_RANKS; ++i) {
      if (score < rank[i].score) {
        return (i - 1);
      }
    }

    return (NUM_RANKS - 2);
  }

  return (NUM_RANKS - 1);
}
/****************************************************************************
	sort the programs structure
*/
void EXPORT Sort_Programs(void) {
  short   s1;
  short   s2;
  struct  program_struct temp;

  for (s1 = 0; s1 != NUM_PROGRAMS; ++s1) {
    for (s2 = 0; s2 != NUM_PROGRAMS; ++s2) {
      if (Programs[s1].index < Programs[s2].index) {
        temp = Programs[s1];
        Programs[s1] = Programs[s2];
        Programs[s2] = temp;
      }
    }
  }
}
/***************************************************************************
	see if the named user is around
*/
short EXPORT Find_Guy(char *name) {
  short   matches = 0;
  short   abs_match = FALSE;
  short   index = -1;
  short   g;
  short   is_local;
  short   is_remote;
  short   remote_guy = FALSE;

  for (g = 0; g != max_guys && !abs_match; ++g) {
    is_local = FALSE;
    is_remote = FALSE;

    if
    (
      Port[g]->active &&
      Port[g]->access_level == MY_PORT->access_level &&
      Port[g]->net_x == MY_PORT->net_x &&
      Port[g]->net_y == MY_PORT->net_y &&
      MY_PORT->access_level == CITY_VIEW
    ) {
      is_local = TRUE;
    } else if
    (
      Port[g]->active &&
      Port[g]->access_level == MY_PORT->access_level &&
      MY_PORT->access_level == SYSTEM_VIEW &&
      Port[g]->net_x == MY_PORT->net_x &&
      Port[g]->net_y == MY_PORT->net_y &&
      Port[g]->city_x == MY_PORT->city_x &&
      Port[g]->city_y == MY_PORT->city_y
    ) {
      is_local = TRUE;
    } else if (Port[g]->active && Port[g]->access_level != IDLE) {
      is_remote = TRUE;
    }

    if (is_local) {
      if (sameas(him.alias, name)) {
        matches = 1;
        abs_match = TRUE;
        index = g;
      } else if (sameto(name, him.alias)) {
        ++matches;
        index = g;
      }
    } else if (is_remote && Traced(g)) {
      if (sameas(him.alias, name)) {
        matches = 1;
        abs_match = TRUE;
        index = g;
        remote_guy = TRUE;
      } else if (sameto(name, him.alias)) {
        ++matches;
        index = g;
        remote_guy = TRUE;
      }
    }
  }

  if (matches == 0) {
    prfmsg(MSG468, name);
    return (-1);
  }

  if (matches > 1) {
    prfmsg(MSG469, name);
    return (-1);
  }

  if (index == myp) {
    prfmsg(MSG470);
    return (-1);
  }

  if (!remote_guy && !Visible(myp, index)) {
    prfmsg(MSG468, name);
    return (-1);
  }

  return (index);
}
/***************************************************************************
	load "knight" style program to attack specified tactical target
*/
short EXPORT Run_Tactical(char *which) {
  short   x, y;
  short   s, c;
  short   visible;
  short   target = atoi(which);
  short   target_num = 0;
  struct  cpu_struct *runner;

  if (MY_PORT->access_level == CITY_VIEW) {
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

          if
          (
            s != myp &&
            visible &&
            sameto(MY_PORT->tstring, THEIR_SOURCE.name)
          ) {
            ++target_num;

            if (target_num == target) {
              MY_PORT->target_s = s;
              MY_PORT->target_c = c;
              Prompt(getmsg(MSG88), DIRECTION_PROMPT);
              EXIT0;
            }
          }
        }
      }
    }
  } else {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      for (x = 0; x != MAX_SYS_X; ++x) {
        runner = Find_Program
                 (
                   MY_PORT->net_x,
                   MY_PORT->net_y,
                   MY_PORT->city_x,
                   MY_PORT->city_y,
                   x,
                   y,
                   &s, &c
                 );

        if (runner != NULL) {
          visible = Visible_Hydra(myp, s, c);

          if
          (
            s != myp &&
            visible &&
            sameto(MY_PORT->tstring, THEIR_SOURCE.name)
          ) {
            ++target_num;

            if (target_num == target) {
              MY_PORT->target_s = s;
              MY_PORT->target_c = c;
              Prompt(getmsg(MSG88), DIRECTION_PROMPT);
              EXIT0;
            }
          }
        }
      }
    }
  }

  Clear_Display_Window();
  prfmsg(MSG471);
  EXIT0;
}
/***************************************************************************
	see if the named program is around
*/
short EXPORT Find_Target_Program(char *name, short *sys_index, short *cpu) {
  short   is_local;
  short   i;
  short   s;
  short   matches = 0;
  short   n;
  short   visible;

  for (s = 0; s != max_systems; ++s) {
    if ((Sys[s]->in_use || s < max_guys) && s != myp) {
      for (i = 0; i != Sys[s]->cpus; ++i) {
        if
        (
          SYS_PROGRAM.m_index >= 0 &&
          SYS_PROGRAM.net_x == MY_PORT->net_x &&
          SYS_PROGRAM.net_y == MY_PORT->net_y
        ) {
          is_local = FALSE;
          n = Runner_System_Number(s, i);

          if
          (
            (
              SYS_PROGRAM.sys_x == -1 ||
              SYS_PROGRAM.sys_y == -1
            ) &&
            MY_PORT->access_level == CITY_VIEW
          ) {
            is_local = TRUE;
          } else if
          (
            SYS_PROGRAM.sys_x != -1 &&
            SYS_PROGRAM.sys_y != -1 &&
            MY_PORT->access_level == SYSTEM_VIEW &&
            MY_PORT->sys_num == n
          ) {
            is_local = TRUE;
          }

          if (is_local && sameto(name, SYS_SOURCE.name)) {
            visible = Visible_Hydra(myp, s, i);

            if (visible) {
              ++matches;
              *sys_index = s;
              *cpu = i;
            }
          }
        }
      }
    }
  }

  if (matches == 0) {
    prfmsg(MSG472, name);
    *sys_index = -1;
    *cpu = -1;
  }

  if (matches > 1) {
    prfmsg(MSG473, name);
    *sys_index = -1;
    *cpu = -1;
    strncpy(MY_PORT->tstring, name, NAME_SIZE - 1);
    MY_PORT->tstring[NAME_SIZE - 1] = 0;
    Look("X");
    Prompt(getmsg(MSG474), TACTICAL_PROMPT);
  }

  EXIT0;
}
/***************************************************************************
	see if a hydra has the specified program as a subroutine
*/
short EXPORT Hydra_Has(short p, short s, short i) {
  short   r;

  if (SYS_SOURCE.class != HYDRA) {
    return (FALSE);
  }

  for (r = 0; r != MAX_SUBS; ++r) {
    if (SYS_SOURCE.sub[r] == p) {
      return (TRUE);
    }
  }

  return (FALSE);
}
/***************************************************************************
	see if a hydra is invisible
*/
short EXPORT Visible_Hydra(short g, short s, short i) {
  short   p;
  short   r;
  short   visible = TRUE;

  if (g == s) {
    return (TRUE);
  }

  if (SYS_SOURCE.class != HYDRA) {
    return (TRUE);
  }

  for (r = 0; r != MAX_SUBS; ++r) {
    if (SYS_SOURCE.sub[r] == CLOAK) {
      visible = FALSE;
    }
  }

  if (!visible) {
    for (p = 0; p != Sys[g]->cpus; ++p) {
      if
      (
        GUY_PROGRAM.m_index >= 0 &&
        GUY_SOURCE.index == RADAR &&
        GUY_PROGRAM.sees_hydra_s == s &&
        GUY_PROGRAM.sees_hydra_c == i
      ) {
        return (TRUE);
      }
    }
  }

  return (visible);
}
/***************************************************************************
	see if a guy is invisible
*/
short EXPORT Visible(short s, short g) {
  short   p;
  short   i;
  short   cloaked = FALSE;

  if (s < 0 || s >= max_systems || g < 0 || g >= max_systems) {
    return (TRUE);
  }

  for (p = 0; p != Sys[g]->cpus; ++p) {
    if
    (
      GUY_PROGRAM.m_index >= 0 &&
      GUY_SOURCE.index == CLOAK
    ) {
      cloaked = TRUE;
    }
  }

  if (!cloaked) {
    return (TRUE);
  }

  for (i = 0; i != Sys[s]->cpus; ++i) {
    if
    (
      SYS_PROGRAM.m_index >= 0 &&
      SYS_SOURCE.index == RADAR &&
      SYS_PROGRAM.sees[g]
    ) {
      return (TRUE);
    }
  }

  return (FALSE);
}
/***************************************************************************
	random numbers
*/
/*
short EXPORT RANDOM(short n)
{
	short   val;

	if (n <= 0)
	{
		return (1);
	}

	val = random(n) + 1;

	return (val);
}
*/
/****************************************************************************
	am I within a system's datawalls?
*/
short EXPORT Inside(short g) {
  short   x;
  short   y;

  if (Port[g]->access_level != SYSTEM_VIEW) {
    Port[g]->inside = FALSE;
    return (FALSE);
  }

  x = Port[g]->sys_x;
  y = Port[g]->sys_y;

  if
  (
    x == 0 ||
    y == 0 ||
    x == MAX_SYS_X - 1 ||
    y == MAX_SYS_Y - 1
  ) {
    Port[g]->inside = FALSE;
    return (FALSE);
  }

  Port[g]->inside = TRUE;
  return (TRUE);
}
/****************************************************************************
	is a program within a system's datawalls?
*/
short Program_Inside(short s, short i) {
  short   x, y;
  short   blocked = FALSE;
  short   n = Runner_System_Number(s, i);
  x = SYS_PROGRAM.sys_x;
  y = SYS_PROGRAM.sys_y;

  if (x < 0 && y < 0) {
    return (FALSE);
  }

  if (Sys[n]->node_type[x][y]) {
    return (TRUE);
  }

  while (x >= 0) {
    if
    (
      Sys[n]->node_type[x][y] == OPENED_GATE ||
      Sys[n]->node_type[x][y] == DATA_WALL ||
      Sys[n]->node_type[x][y] == CLOSED_GATE ||
      Sys[n]->node_type[x][y] == DEAD_WALL
    ) {
      blocked = TRUE;
    }

    --x;
  }

  if (!blocked) {
    return (FALSE);
  }

  blocked = FALSE;
  x = SYS_PROGRAM.sys_x;
  y = SYS_PROGRAM.sys_y;

  while (x <= MAX_SYS_X - 1) {
    if
    (
      Sys[n]->node_type[x][y] == OPENED_GATE ||
      Sys[n]->node_type[x][y] == DATA_WALL ||
      Sys[n]->node_type[x][y] == CLOSED_GATE ||
      Sys[n]->node_type[x][y] == DEAD_WALL
    ) {
      blocked = TRUE;
    }

    ++x;
  }

  if (!blocked) {
    return (FALSE);
  }

  blocked = FALSE;
  x = SYS_PROGRAM.sys_x;
  y = SYS_PROGRAM.sys_y;

  while (y >= 0) {
    if
    (
      Sys[n]->node_type[x][y] == OPENED_GATE ||
      Sys[n]->node_type[x][y] == DATA_WALL ||
      Sys[n]->node_type[x][y] == CLOSED_GATE ||
      Sys[n]->node_type[x][y] == DEAD_WALL
    ) {
      blocked = TRUE;
    }

    --y;
  }

  if (!blocked) {
    return (FALSE);
  }

  blocked = FALSE;
  x = SYS_PROGRAM.sys_x;
  y = SYS_PROGRAM.sys_y;

  while (y <= MAX_SYS_Y - 1) {
    if
    (
      Sys[n]->node_type[x][y] == OPENED_GATE ||
      Sys[n]->node_type[x][y] == DATA_WALL ||
      Sys[n]->node_type[x][y] == CLOSED_GATE ||
      Sys[n]->node_type[x][y] == DEAD_WALL
    ) {
      blocked = TRUE;
    }

    ++y;
  }

  if (!blocked) {
    return (FALSE);
  }

  return (TRUE);
}
/****************************************************************************
	try to crash a long distance link
*/
short Crash_LD_Link(void) {
  short   i;
  short   found;
  short   done;
  short   roll;
  short   s = glob->cop_sys;
  roll = RANDOM(10);

  if (roll >= MY_CITY.security + MY_PORT->spaces) {
    prfmsg(MSG475);
    prfmsg(MSG148);
    roll = RANDOM(10);
    sprintf(str, getmsg(MSG689), roll);
    Send(str, myp, TRUE);
    MY_PORT->rez -= roll;

    if (MY_PORT->rez <= 0) {
      strcpy(str, getmsg(MSG437));
      Send(str, myp, TRUE);
      Jack_Out();
    } else {
      command = MY_PORT->last_cmd;
      Net_Move(MY_PORT->spaces, LD_LINK);
      STATE = MAIN_PROMPT;
    }

    EXIT0;
  }

  roll = RANDOM(6);

  if (roll == 5 && MY_PORT->link_type == STREET_LINK) {
    roll = 1;
  }

  if (roll == 6) {
    Open_The_Eye();
    prfmsg(MSG476);
    prfmsg(MSG477);
    roll = RANDOM(6);

    if (roll <= 2) {
      roll = RANDOM(6) * 100;
      prfmsg(MSG478, roll);
      Add_Internet_Record
      (
        &me,
        (long) roll,
        MY_PORT->net_x, MY_PORT->net_y,
        me.alias,
        "Fine"
      );
    } else {
      prfmsg(MSG479);
      me.last_net_crime = time(0);

      if (roll == 6) {
        me.wanted = TRUE;
      } else {
        MY_MISC_CITY.last_net_crime = time(0);

        for (i = found = 0; i != Sys[s]->cpus && !found; ++i) {
          if
          (
            SYS_PROGRAM.m_index >= 0 &&
            SYS_PROGRAM.cop_x == MY_PORT->net_x &&
            SYS_PROGRAM.cop_y == MY_PORT->net_y
          ) {
            found = TRUE;
          }
        }

        if (!found) {
          for (i = done = 0; i != Sys[s]->cpus && !done; ++i) {
            if
            (
              SYS_PROGRAM.m_index >= 0 &&
              SYS_PROGRAM.target_index < 0 &&
              SYS_PROGRAM.cop_x < 0
            ) {
              SYS_PROGRAM.cop_x = MY_PORT->net_x;
              SYS_PROGRAM.cop_y = MY_PORT->net_y;
              done = TRUE;
            }
          }
        }
      }
    }

    Prompt(getmsg(MSG207), EYE_PROMPT);
  } else if (roll == 5) {
    prfmsg(MSG476);

    if (!me.arrest_time) {
      me.arrest_time = glob->copmins2 * 60;
      me.arrest_warn30 = FALSE;
      me.arrest_warn90 = FALSE;
    }

    prfmsg(MSG480, glob->ldcost * MY_PORT->spaces);
    Add_Internet_Record
    (
      &me,
      (long) (glob->ldcost) * (long) MY_PORT->spaces,
      MY_PORT->net_x, MY_PORT->net_y,
      me.alias,
      "Fine"
    );
    STATE = MAIN_PROMPT;
  } else {
    prfmsg(MSG476);
    prfmsg(MSG481, glob->ldcost * MY_PORT->spaces);
    Add_Internet_Record
    (
      &me,
      (long) (glob->ldcost) * (long) MY_PORT->spaces,
      MY_PORT->net_x, MY_PORT->net_y,
      me.alias,
      "Fine"
    );
    STATE = MAIN_PROMPT;
  }

  EXIT0;
}
/****************************************************************************
	set my non-Do? prompt
*/
void Main_Prompt(void) {
  STATE = MAIN_PROMPT;
  MY_PORT->prompt_y = PROMPT_Y;
  MY_PORT->prompt_x = 1;

  if (Local_Gablink(myp)) {
    strcpy(MY_PORT->prompt, glob->gab_prompt);
  } else {
    strcpy(MY_PORT->prompt, glob->main_prompt);
  }

  strcpy(glob->buf, MY_PORT->prompt);
  stpans(glob->buf);
  MY_PORT->prompt_len = strlen(glob->buf);
}
/****************************************************************************
	set my non-Do? prompt
*/
void EXPORT Prompt(char *prompt, short p) {
  strcpy(glob->buf, prompt);
  stpans(glob->buf);
  MY_PORT->prompt_len = strlen(glob->buf);
  STATE = p;
  sprintf(MY_PORT->prompt, "%s", prompt);
}
/****************************************************************************
	parse the input line for objects, targets, verbs, etc
*/
short Parse_Input(void) {
  parm1[0] = 0;
  parm2[0] = 0;
  parm3[0] = 0;

  if (margc == 1) {
    EXIT0;
  }

  strcpy(parm1, margv[1]);

  if (margc == 2) {
    EXIT0;
  }

  strcpy(parm2, margv[2]);

  if (margc == 3) {
    EXIT0;
  }

  strcpy(parm3, margv[3]);
  EXIT0;
}
/*****************************************************************************
	see if it's payday
*/
void Payday_Check(short exists) {
  long    expenses;
  short   nl = FALSE;
  short   new_day = FALSE;
  short   netcost = glob->netcost;

  if (today() != me.last_on) {
    if (exists && me.last_on) {
      me.newbie = FALSE;
    }

    new_day = TRUE;
    me.jacks = 0;

    if (me.phone) {
      netcost += glob->ldlcost;
    }

    if (tourney->phase == GAME) {
      Add_Internet_Record
      (
        &me,
        netcost,
        me.start_x, me.start_y,
        me.alias,
        "Daily Bill"
      );
    }

    me.last_on = today();

    if (me.damage) {
      prfmsg(MSG484);
      --me.damage;
      nl = TRUE;
    }

    if (!me.employed) {
      if (RANDOM(His_Rank(me.score, me.won) + 1) == 1) {
        prfmsg(MSG485);
      } else {
        prfmsg(MSG486);
        me.employed = TRUE;
        me.payday = 0;
      }

      nl = TRUE;
    }
  }

  if (today() >= me.payday && me.employed) {
    if (exists && tourney->phase == GAME) {
      prfmsg(MSG487, rank[His_Rank(me.score, me.won)].salary);
      nl = TRUE;
    }

    Add_Bank_Record
    (
      &me,
      rank[His_Rank(me.score, me.won)].salary,
      me.start_x, me.start_y,
      me.alias,
      "Payday"
    );
    me.payday = cofdat(today()) + glob->paydays;
    me.payday = datofc(me.payday);
  }

  if (me.renting && today() >= me.rentdue) {
    if (exists && tourney->phase == GAME) {
      if (Rent() > me.bank.balance && !me.perm[FREE_PERM]) {
        prfmsg(MSG488);
        prfmsg(MSG489);
        me.renting = FALSE;
      } else if (!me.perm[FREE_PERM]) {
        prfmsg(MSG490, Rent());
        Add_Bank_Record
        (
          &me,
          (long) - (Rent()),
          me.start_x, me.start_y,
          me.alias,
          "Rent Paid"
        );
      }

      nl = TRUE;
    }

    me.rentdue = cofdat(today()) + glob->rentdays;
    me.rentdue = datofc(me.rentdue);
  }

  if
  (
    new_day &&
    !me.perm[FREE_PERM] &&
    exists &&
    tourney->phase == GAME
  ) {
    expenses = Rent() / glob->rentdays;

    if (expenses > me.bank.balance) {
      expenses = me.bank.balance;
    }

    if (expenses) {
      Add_Bank_Record
      (
        &me,
        -(expenses),
        me.start_x, me.start_y,
        me.alias,
        "Living Expenses"
      );
    }
  }

  if (nl) {
    prf(NL);
  }
}
/*****************************************************************************
	initialize player stats
*/
short Newguy(short option) {
  short   i;
  short   bad_alias = FALSE;
  memset((char *) my_record, 0, HUMAN_SIZE);

  if (option == 0) {
    strncpy(my_record->alias, usaptr->userid, NAME_SIZE - 1);
    my_record->alias[NAME_SIZE - 1] = 0;

    for (i = 0; i < strlen(my_record->alias); ++i) {
      if (my_record->alias[i] == ' ') {
        my_record->alias[i] = '_';
      }
    }
  } else {
    Fix_Alias(my_record->alias);
  }

  for (i = 1; i != NUM_PROGRAMS; ++i) {
    if (sameas(my_record->alias, Programs[i].Name)) {
      bad_alias = TRUE;
    }
  }

  if (!bad_alias) {
    i = Find_Alias(a_human, my_record->alias);

    if
    (
      i >= 0 ||
      sameas(my_record->alias, "AUTOMATED") ||
      sameas(my_record->alias, CANCEL) ||
      sameto(A_GANG, my_record->alias)
    ) {
      bad_alias = TRUE;
    }
  }

  if (bad_alias) {
    prfmsg(MSG491, my_record->alias);
    prfmsg(MSG492);
    prfmsg(MSG493);
    STATE = NEWGUY_PROMPT;
    EXIT0;
  }

  strcpy(my_record->userid, usaptr->userid);
  my_record->created              = time(0);
  my_record->option[IBM_OPT]      = glob->ibm[usrnum];
  my_record->option[AUTOJACK_OPT] = glob->autojack;
  my_record->newbie               = TRUE;
  my_record->start_x              = -1;
  my_record->start_y              = -1;
  my_record->renting              = TRUE;
  my_record->employed             = TRUE;
  my_record->chip_reader          = glob->streader;
  my_record->interface            = TRUE;
  my_record->backup_chips         = glob->stchips;
  Init_System(a_system, CYBERDECK);
  strcpy(a_system->Name, "Cyberdeck");
  return (TRUE);
}
/****************************************************************************
	set a system path
*/
void Set_System_Path(short sys_x, short sys_y, short net_x, short net_y) {
  sprintf
  (
    glob->path,
    "%s/%d@%dx%d@.%df",
    SYSTEM_PATH,
    net_x,
    net_y,
    sys_x,
    sys_y
  );
}
/****************************************************************************
	set a valhalla system path
*/
void Set_Valhalla_Path(short sys_x, short sys_y) {
  sprintf(glob->path, "%s/%d@%d.dat", VALHALLA_PATH, sys_x, sys_y);
}
/****************************************************************************
	set a custom system path
*/
void Set_Custom_Path(short sys_x, short sys_y, short net_x, short net_y) {
  sprintf
  (
    glob->path,
    "%s/%d@%dx%d@.%df",
    CUSTOM_PATH,
    net_x,
    net_y,
    sys_x,
    sys_y
  );
}
/****************************************************************************
	set a permanant system definition path
*/
void Set_Perm_Path(short sys_x, short sys_y, short net_x, short net_y) {
  sprintf
  (
    glob->path,
    "%s/%d@%dx%d@.%dd",
    SYSTEM_PATH,
    net_x,
    net_y,
    sys_x,
    sys_y
  );
}
/****************************************************************************
	set a custom permanant system definition path
*/
void Set_Custom_Perm_Path(short sys_x, short sys_y, short net_x, short net_y) {
  sprintf
  (
    glob->path,
    "%s/%d@%dx%d@.%dd",
    CUSTOM_PATH,
    net_x,
    net_y,
    sys_x,
    sys_y
  );
}
/****************************************************************************
	pick a random spot in a system that's not the reserved spot
*/
void Random_Space(short *x, short *y) {
  do {
    *x = RANDOM(MAX_SYS_X - 2);
    *y = RANDOM(MAX_SYS_Y - 2);
  } while (*x == START_SYS_X && *y == START_SYS_Y);
}
/****************************************************************************
	initialize a system
*/
void Init_System(struct system_struct *sys, short sys_type) {
  short   roll;
  short   roll2;
  short   gate_type;
  short   m_num;
  short   p;
  short   m;
  short   x, y;
  short   i, j;
  short   done;
  short   progs_left;
  short   index;
  short   class;
  short   value;
  short   skip;
  short   bonus;
  short   size;
  short   num_gates = RANDOM(2) + 1;
  short   endgame = FALSE;
  short   has_password;
  short   auto_doc = TRUE;
  struct  sys_type_struct parm = System[0];

  if (sys_type == ENDGAME_SYS) {
    endgame = TRUE;
    sys_type = END_SYS;
  }

  for (i = 0; i != SYS_TYPES; ++i) {
    if (System[i].type == sys_type) {
      parm = System[i];
    }
  }

  for (i = 0; i != MAX_CPUS + 1; ++i) {
    sys->crash_time[i] = 0;
  }

  for (m = 0; m != MAX_MUS; ++m) {
    for (i = 0; i != MU_CAPACITY; ++i) {
      sys->memory[m][i].type = 0;
    }
  }

  sys->type = sys_type;
  sys->breached = FALSE;

  if (sys_type == BBS) {
    gate_type = OPENED_GATE;
  } else {
    gate_type = CLOSED_GATE;
  }

  if (sys_type == CYBERDECK) {
    sys->cpus  = glob->stcpus;
    sys->mus   = glob->stmem;
    sys->speed = glob->stspeed;
    sys->wall  = glob->stwall;
    sys->gate  = glob->stgate;
  } else if (sys_type == GANGDECK) {
    sys->cpus  = 0;
    sys->mus   = 0;
    sys->speed = 0;
    sys->wall  = 0;
    sys->gate  = 0;
  } else {
    sys->speed = parm.speed;
    sys->mus = parm.mus;
    sys->cpus = parm.cpus;
    sys->wall = parm.defense;
    sys->gate = parm.defense;

    while (RANDOM(2) == 1 && sys->wall > 1) {
      --sys->wall;
    }

    while (RANDOM(2) == 1 && sys->gate > 1) {
      --sys->gate;
    }
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      sys->node_type[x][y] = 0;
    }
  }

  if
  (
    sys->type != END_SYS &&
    sys->type != ALIEN_SYS &&
    sys->type != CYBERIA_SYS
  ) {
    for (x = 1, y = 1; x != MAX_SYS_X - 2; ++x) {
      sys->node_type[x][y] = DATA_WALL;
    }

    for (x = 1, y = MAX_SYS_Y - 2; x != MAX_SYS_X - 1; ++x) {
      sys->node_type[x][y] = DATA_WALL;
    }

    for (y = 1, x = 1; y != MAX_SYS_Y - 2; ++y) {
      sys->node_type[x][y] = DATA_WALL;
    }

    for (y = 1, x = MAX_SYS_X - 2; y != MAX_SYS_Y - 1; ++y) {
      sys->node_type[x][y] = DATA_WALL;
    }

    for (i = 0; i != num_gates; ++i) {
      x = RANDOM(MAX_SYS_X - 4) + 1;
      y = 1;
      sys->node_type[x][y] = gate_type;
      x = RANDOM(MAX_SYS_X - 4) + 1;
      y = MAX_SYS_Y - 2;
      sys->node_type[x][y] = gate_type;
      y = RANDOM(MAX_SYS_Y - 4) + 1;
      x = 1;
      sys->node_type[x][y] = gate_type;
      y = RANDOM(MAX_SYS_Y - 4) + 1;
      x = MAX_SYS_X - 2;
      sys->node_type[x][y] = gate_type;
    }
  } else {
    for (x = 0, y = (MAX_SYS_Y / 2); x != MAX_SYS_X; ++x) {
      if (RANDOM(2) != 1) {
        sys->node_type[x][y] = DATA_WALL;
      } else {
        sys->node_type[x][y] = gate_type;
      }
    }

    for (y = 0, x = (MAX_SYS_X / 2); y != MAX_SYS_Y; ++y) {
      if (RANDOM(2) != 1) {
        sys->node_type[x][y] = DATA_WALL;
      } else {
        sys->node_type[x][y] = gate_type;
      }
    }
  }

  if (sys_type == VENDOR) {
    for (x = START_SYS_X - 1; x != START_SYS_X + 2; ++x) {
      for (y = START_SYS_Y - 1; y != START_SYS_Y + 2; ++y) {
        if (x != START_SYS_X || y != START_SYS_Y) {
          sys->node_type[x][y] = DATA_WALL;
        }
      }
    }
  }

  for (i = 0; i < sys->cpus; ++i) {
    Add_CPU(sys, i);
  }

  if
  (
    RANDOM(10) == 1 &&
    (
      sys_type == LEV3 ||
      sys_type == CRIMINAL ||
      sys_type == LEV4 ||
      sys_type == LEV5 ||
      sys_type == LEV6 ||
      sys_type == LEV7 ||
      sys_type == WEB_SERVER ||
      sys_type == HUB ||
      sys_type == WORLDBANK ||
      sys_type == COP_SYS
    )
  ) {
    Add_Phone(sys);
  }

  if
  (
    !endgame &&
    sys_type != CYBERDECK &&
    sys_type != GANGDECK &&
    sys_type != BBS &&
    sys_type != VENDOR &&
    sys_type != COP_SYS &&
    sys_type != END_SYS &&
    sys_type != ALIEN_SYS &&
    sys_type != CYBERIA_SYS &&
    sys_type != FILE_SERVER &&
    !sys->permanant
  ) {
    for (j = 0; j != 4; ++j) {
      sys->password[j] = RANDOM(26) - 1 + 'A';
    }

    sys->password[4] = 0;
    has_password = FALSE;
  } else {
    sys->password[0] = 0;
    has_password = TRUE;
  }

  progs_left = 0;

  if (sys_type == CYBERDECK) {
    for (i = 0; user_programs[i].index != 0; ++i) {
      glob->vends[i] = user_programs[i];
      ++progs_left;
    }
  } else if (sys_type == VENDOR) {
    for (i = 0; vendor_programs[i].index != 0; ++i) {
      glob->vends[i] = vendor_programs[i];
      ++progs_left;
    }
  } else if (sys_type == FILE_SERVER) {
    for (i = 0; gang_programs[i].index != 0; ++i) {
      glob->vends[i] = gang_programs[i];
      ++progs_left;
    }
  } else if (sys_type == BBS) {
    for (i = 0; bbs_programs[i].index != 0; ++i) {
      glob->vends[i] = bbs_programs[i];
      ++progs_left;
    }
  } else if (sys_type == LEV1) {
    for (i = 0; level1_programs[i].index != 0; ++i) {
      glob->vends[i] = level1_programs[i];
      ++progs_left;
    }
  } else if (sys_type == LEV2) {
    for (i = 0; level2_programs[i].index != 0; ++i) {
      glob->vends[i] = level2_programs[i];
      ++progs_left;
    }
  } else if (sys_type == LEV3) {
    for (i = 0; level3_programs[i].index != 0; ++i) {
      glob->vends[i] = level3_programs[i];
      ++progs_left;
    }
  } else if (sys_type == LEV4) {
    for (i = 0; level4_programs[i].index != 0; ++i) {
      glob->vends[i] = level4_programs[i];
      ++progs_left;
    }
  } else if (sys_type == LEV5) {
    for (i = 0; level5_programs[i].index != 0; ++i) {
      glob->vends[i] = level5_programs[i];
      ++progs_left;
    }
  } else if (sys_type == LEV6) {
    for (i = 0; level6_programs[i].index != 0; ++i) {
      glob->vends[i] = level6_programs[i];
      ++progs_left;
    }
  } else if (sys_type == LEV7) {
    for (i = 0; level7_programs[i].index != 0; ++i) {
      glob->vends[i] = level7_programs[i];
      ++progs_left;
    }
  } else if (sys_type == WEB_SERVER) {
    for (i = 0; web_programs[i].index != 0; ++i) {
      glob->vends[i] = web_programs[i];
      ++progs_left;
    }
  } else if (sys_type == HUB) {
    for (i = 0; hub_programs[i].index != 0; ++i) {
      glob->vends[i] = hub_programs[i];
      ++progs_left;
    }
  } else if (sys_type == CRIMINAL) {
    for (i = 0; crime_programs[i].index != 0; ++i) {
      glob->vends[i] = crime_programs[i];
      ++progs_left;
    }
  } else if (sys_type == WORLDBANK) {
    for (i = 0; bank_programs[i].index != 0; ++i) {
      glob->vends[i] = bank_programs[i];
      ++progs_left;
    }
  } else if (sys_type == END_SYS) {
    for (i = 0; end_programs[i].index != 0; ++i) {
      glob->vends[i] = end_programs[i];
      ++progs_left;
    }
  } else if (sys_type == ALIEN_SYS) {
    for (i = 0; alien_programs[i].index != 0; ++i) {
      glob->vends[i] = alien_programs[i];
      ++progs_left;
    }
  } else if (sys_type == CYBERIA_SYS) {
    for (i = 0; cyberia_programs[i].index != 0; ++i) {
      glob->vends[i] = cyberia_programs[i];
      ++progs_left;
    }
  }

  for (i = 0; i < sys->mus; ++i) {
    done = Add_Memory(sys, i);

    if (i == 0 && sys->type == VENDOR) {
      for (m_num = 0; m_num != CASHIERS; ++m_num) {
        p = CASHIER;
        sys->memory[i][m_num].index = p;
        sys->memory[i][m_num].type = PROGRAM;
        sys->memory[i][m_num].lock = 0;
        sys->memory[i][m_num].packed = FALSE;
        sys->memory[i][m_num].vended = FALSE;
        sys->memory[i][m_num].class = Programs[p].class;
        sys->memory[i][m_num].size = Programs[p].size;
        sys->memory[i][m_num].load = Programs[p].load;
        strcpy(sys->memory[i][m_num].name, Programs[p].Name);
        sys->memory[i][m_num].value = Programs[p].value;
        sys->memory[i][m_num].strength = Programs[p].strength;
      }

      done = FALSE;
    }

    if (done) {
      for (m = 0; m != MU_CAPACITY; ++m) {
        sys->memory[i][m].type = 0;
      }

      m_num = 0;

      do {
        skip = FALSE;

        if (progs_left) {
          index = RANDOM(progs_left) - 1;
          p = glob->vends[index].index;

          if (RANDOM(100) > glob->vends[index].chance) {
            skip = TRUE;
            --glob->vends[index].occurs;

            if (glob->vends[index].occurs == 0) {
              if (index != progs_left - 1) {
                for (j = index; j != progs_left - 1; ++j) {
                  glob->vends[j] = glob->vends[j + 1];
                }
              }

              --progs_left;
            }
          }
        } else {
          p = 0;
        }

        m = Memory_Usage(sys->memory[i]);

        if (p && !skip) {
          if
          (
            Programs[p].class == HYDRA &&
            sys_type != VENDOR &&
            sys_type != BBS &&
            sys_type != LEV1 &&
            sys_type != LEV2
          ) {
            size = MU_CAPACITY;
          } else {
            size = Programs[p].size;
          }

          if (m + size <= MU_CAPACITY) {
            sys->memory[i][m_num].index = p;
            sys->memory[i][m_num].type = PROGRAM;

            if (sys->type == CYBERDECK || sys->type == VENDOR) {
              sys->memory[i][m_num].lock = 0;
            } else if (RANDOM(3) != 1) {
              sys->memory[i][m_num].lock = RANDOM(sys->type) - 1;
            }

            sys->memory[i][m_num].packed = FALSE;
            sys->memory[i][m_num].vended = FALSE;
            sys->memory[i][m_num].class = Programs[p].class;
            sys->memory[i][m_num].size = Programs[p].size;
            sys->memory[i][m_num].load = Programs[p].load;
            strcpy(sys->memory[i][m_num].name, Programs[p].Name);
            sys->memory[i][m_num].value = Programs[p].value;
            sys->memory[i][m_num].strength = Programs[p].strength;

            if (sys_type == CYBERDECK) {
              sys->memory[i][m_num].value = 0;
              sys->memory[i][m_num].strength = 1;
            } else {
              if (sys_type == LEV1) {
                bonus = RANDOM(2);

                if (RANDOM(4) == 1) {
                  ++bonus;
                }
              } else if (sys_type == LEV2) {
                bonus = RANDOM(3);

                if (RANDOM(2) == 1) {
                  ++bonus;
                }
              } else if
              (
                sys_type == ALIEN_SYS ||
                sys_type == CYBERIA_SYS
              ) {
                if (Programs[p].class != HYDRA) {
                  bonus = RANDOM((parm.power * 3) / 2);
                } else {
                  bonus = RANDOM(5);
                }
              } else {
                bonus = parm.power / 2;
                bonus += RANDOM(parm.power / 2);

                if (RANDOM(2) == 1) {
                  bonus += RANDOM(parm.power / 2);
                }
              }

              sys->memory[i][m_num].strength += bonus;
              value = sys->memory[i][m_num].value;
              value = value * bonus;
              value = value / 10;
              sys->memory[i][m_num].value += value;

              if (sys->memory[i][m_num].load) {
                sys->memory[i][m_num].load += bonus / 4;
              }
            }

            if
            (
              Programs[p].class == HYDRA &&
              sys_type != VENDOR &&
              sys_type != BBS &&
              sys_type != LEV1 &&
              sys_type != LEV2
            ) {
              Configure_Cop_Hydra(&sys->memory[i][m_num]);
              sys->memory[i][m_num].size = MU_CAPACITY;
            }

            ++m_num;
            m += Programs[p].size;
            --glob->vends[index].occurs;

            if (glob->vends[index].occurs == 0) {
              if (index != progs_left - 1) {
                for (j = index; j != progs_left - 1; ++j) {
                  glob->vends[j] = glob->vends[j + 1];
                }
              }

              --progs_left;
            }
          } else {
            p = 0;
          }
        }
      } while (m < MU_CAPACITY && p != 0 && progs_left);
    }
  }

  if (endgame) {
    for (i = done = 0; i != sys->mus && !done; ++i) {
      if (Memory_Usage(sys->memory[i]) + 1 <= MU_CAPACITY) {
        m_num = -1;

        for (m = 0; m != MU_CAPACITY && m_num < 0; ++m) {
          if (sys->memory[i][m].type == 0) {
            m_num = m;
          }
        }

        if (m_num >= 0) {
          sys->memory[i][m_num].lock = RANDOM(sys->type);
          sys->memory[i][m_num].type = DOCUMENT;
          sys->memory[i][m_num].class = ENDGAME_DOC;
          sys->memory[i][m_num].value = 1;
          strcpy(sys->memory[i][m_num].name, "DIVINE.DOC");
          sys->memory[i][m_num].packed = FALSE;
          sys->memory[i][m_num].vended = FALSE;
          sys->memory[i][m_num].size = 1;
          done = TRUE;
        }
      }
    }
  } else if
  (
    sys_type != CYBERDECK &&
    sys_type != GANGDECK &&
    sys_type != BBS &&
    sys_type != VENDOR &&
    sys_type != COP_SYS &&
    sys_type != END_SYS &&
    sys_type != ALIEN_SYS &&
    sys_type != CYBERIA_SYS &&
    sys_type != FILE_SERVER
  ) {
    for (i = 0; i < sys->mus; ++i) {
      do {
        m_num = -1;

        if (Memory_Usage(sys->memory[i]) + 1 <= MU_CAPACITY) {
          for (m = 0; m != MU_CAPACITY && m_num < 0; ++m) {
            if (sys->memory[i][m].type == 0) {
              m_num = m;
            }
          }
        }

        if (sys_type == WEB_SERVER) {
          roll2 = 1;
        } else {
          roll2 = RANDOM(2);
        }

        roll = RANDOM(100);

        if (!has_password && (roll / 10) * 10 == roll) {
          has_password = TRUE;
          class = PASSWORD_DOC;
          value = (sys->cpus * glob->topval) + RANDOM(glob->topval);

          for (j = 0; j != 4; ++j) {
            sys->memory[i][m_num].name[9 + j] = sys->password[j];
            sys->memory[i][m_num].name[9 + j + 1] = 0;
          }
        } else if (roll < sys->type) {
          class = TOP_SECRET_DOC;
          value = (sys->cpus * glob->topval) + RANDOM(glob->topval);

          if (sys->type == WORLDBANK && RANDOM(10) == 1) {
            sys->memory[i][m_num].index = Find_Random_User
                                          (
                                            glob->name,
                                            glob->name2
                                          );

            if (sys->memory[i][m_num].index >= 0) {
              class = BANK_DOC;
              strcpy
              (
                &sys->memory[i][m_num].name[9],
                glob->name
              );
            }
          } else if (sys->type == HUB && RANDOM(10) == 1) {
            sys->memory[i][m_num].index = Find_Random_User
                                          (
                                            glob->name,
                                            glob->name2
                                          );

            if (sys->memory[i][m_num].index >= 0) {
              class = INTERNET_DOC;
              strcpy
              (
                &sys->memory[i][m_num].name[9],
                glob->name2
              );
            }
          }
        } else if (roll < sys->type * 5) {
          class = SECRET_DOC;
          value = (sys->cpus * glob->secval) + RANDOM(glob->secval);
        } else if (roll < sys->type * 10) {
          class = BUSINESS_DOC;
          value = (sys->cpus * glob->busval) + RANDOM(glob->busval);
        } else {
          class = USELESS_DOC;
          value = 0;
        }

        if (m_num < 0 || (roll2 != 1 && !auto_doc)) {
          m_num = -1;
        } else {
          auto_doc = FALSE;

          if (RANDOM(4) != 1) {
            sys->memory[i][m_num].lock = RANDOM(sys->type) - 1;
          } else {
            sys->memory[i][m_num].lock = 0;
          }

          sys->memory[i][m_num].type = DOCUMENT;
          sys->memory[i][m_num].class = class;
          sys->memory[i][m_num].value = value;

          for (j = 0; j != 4; ++j) {
            sys->memory[i][m_num].name[j] = RANDOM(26) + 'A' - 1;
          }

          strcpy(&sys->memory[i][m_num].name[4], ".DOC");
          sys->memory[i][m_num].packed = FALSE;
          sys->memory[i][m_num].vended = FALSE;
          sys->memory[i][m_num].size = 1;
        }
      } while (m_num >= 0);
    }
  } else if (sys_type == FILE_SERVER) {
    m_num = -1;
    i = RANDOM(sys->mus) - 1;

    if (Memory_Usage(sys->memory[i]) + 1 <= MU_CAPACITY) {
      for (m = 0; m != MU_CAPACITY && m_num < 0; ++m) {
        if (sys->memory[i][m].type == 0) {
          m_num = m;
        }
      }

      if (m_num != -1 && RANDOM(10) == 1) {
        sys->memory[i][m_num].index = Find_Random_Gang
                                      (
                                        glob->name,
                                        glob->name2
                                      );

        if (sys->memory[i][m_num].index >= 0) {
          strcpy
          (
            &sys->memory[i][m_num].name[9],
            glob->name2
          );
          sys->memory[i][m_num].lock = RANDOM(sys->type) * 2;
          sys->memory[i][m_num].type = DOCUMENT;
          sys->memory[i][m_num].class = GANG_DOC;
          value = (sys->cpus * glob->topval) + RANDOM(glob->topval);
          sys->memory[i][m_num].value = value;

          for (j = 0; j != 4; ++j) {
            sys->memory[i][m_num].name[j] = RANDOM(26) + 'A' - 1;
          }

          strcpy(&sys->memory[i][m_num].name[4], ".DOC");
          sys->memory[i][m_num].packed = FALSE;
          sys->memory[i][m_num].vended = FALSE;
          sys->memory[i][m_num].size = 1;
        }
      }
    }
  }

  if
  (
    sys_type == END_SYS ||
    sys_type == ALIEN_SYS ||
    sys_type == CYBERIA_SYS
  ) {
    for (i = 0; i != 80; ++i) {
      x = RANDOM(MAX_SYS_X) - 1;
      y = RANDOM(MAX_SYS_Y) - 1;

      if (sys->node_type[x][y] == 0) {
        if (RANDOM(2) == 1) {
          sys->node_type[x][y] = DATA_WALL;
        } else {
          sys->node_type[x][y] = gate_type;
        }
      }
    }
  } else if
  (
    sys_type != VENDOR &&
    sys_type != CYBERDECK &&
    sys_type != GANGDECK &&
    sys_type != BBS
  ) {
    for (i = 0; i != 8; ++i) {
      x = RANDOM(MAX_SYS_X - 4) + 1;
      y = RANDOM(MAX_SYS_Y - 4) + 1;

      if (sys->node_type[x][y] == 0) {
        sys->node_type[x][y] = DATA_WALL;
      }
    }
  }
}
/****************************************************************************
	put some free stuff in a BBS
*/
void   Free_BBS_Stuff(sys)
struct system_struct *sys;
{
  short   m_num;
  short   p;
  short   m;
  short   i, j;
  short   progs_left;
  short   index;
  short   size;
  short   skip;
  progs_left = 0;

  for (i = 0; user_programs[i].index != 0; ++i) {
    glob->vends[i] = user_programs[i];
    ++progs_left;
  }

  for (i = 0; i != sys->mus; ++i) {
    for (m = 0, m_num = -1; m != MU_CAPACITY && m_num < 0; ++m) {
      if (sys->memory[i][m].type == 0) {
        m_num = m;
      }
    }

    if (m_num >= 0) {
      do {
        skip = FALSE;

        if (progs_left) {
          index = RANDOM(progs_left) - 1;
          p = glob->vends[index].index;

          if (RANDOM(2) == 1) {
            skip = TRUE;
            --glob->vends[index].occurs;

            if (glob->vends[index].occurs == 0) {
              if (index != progs_left - 1) {
                for (j = index; j != progs_left - 1; ++j) {
                  glob->vends[j] = glob->vends[j + 1];
                }
              }

              --progs_left;
            }
          }
        } else {
          p = 0;
        }

        m = Memory_Usage(sys->memory[i]);

        if (p && !skip) {
          size = Programs[p].size;

          if (m + size <= MU_CAPACITY) {
            sys->memory[i][m_num].index = p;
            sys->memory[i][m_num].type = PROGRAM;
            sys->memory[i][m_num].lock = 0;
            sys->memory[i][m_num].packed = FALSE;
            sys->memory[i][m_num].vended = FALSE;
            sys->memory[i][m_num].class = Programs[p].class;
            sys->memory[i][m_num].size = Programs[p].size;
            sys->memory[i][m_num].load = Programs[p].load;
            strcpy(sys->memory[i][m_num].name, Programs[p].Name);
            sys->memory[i][m_num].value = 0;
            sys->memory[i][m_num].strength = 1;

            if (RANDOM(10) == 1) {
              ++sys->memory[i][m_num].strength;
            }

            ++m_num;
            m += Programs[p].size;
            --glob->vends[index].occurs;

            if (glob->vends[index].occurs == 0) {
              if (index != progs_left - 1) {
                for (j = index; j != progs_left - 1; ++j) {
                  glob->vends[j] = glob->vends[j + 1];
                }
              }

              --progs_left;
            }
          } else {
            p = 0;
          }
        }
      } while (m < MU_CAPACITY && p != 0 && progs_left);
    }
  }
}
/****************************************************************************
	add a cpu to a system
*/
short Add_CPU(struct system_struct *sys, short i) {
  short   ok;
  short   x, y;
  short   cpu_x, cpu_y;
  short   tries = 0;

  do {
    ++tries;
    ok = TRUE;
    Random_Space(&cpu_x, &cpu_y);

    if (sys->node_type[cpu_x][cpu_y] != 0) {
      ok = FALSE;
    } else if
    (
      sys->node_type[cpu_x - 1][cpu_y] == OPENED_GATE ||
      sys->node_type[cpu_x + 1][cpu_y] == OPENED_GATE ||
      sys->node_type[cpu_x][cpu_y - 1] == OPENED_GATE ||
      sys->node_type[cpu_x][cpu_y + 1] == OPENED_GATE ||
      sys->node_type[cpu_x - 1][cpu_y] == CLOSED_GATE ||
      sys->node_type[cpu_x + 1][cpu_y] == CLOSED_GATE ||
      sys->node_type[cpu_x][cpu_y - 1] == CLOSED_GATE ||
      sys->node_type[cpu_x][cpu_y + 1] == CLOSED_GATE
    ) {
      ok = FALSE;
    }
  } while (!ok && tries < 1000);

  if (ok) {
    sys->node_type[cpu_x][cpu_y] = CPU;
    sys->node_index[cpu_x][cpu_y] = i;
    EXIT0;
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      if (sys->node_type[x][y] == 0) {
        sys->node_type[x][y] = CPU;
        sys->node_index[x][y] = i;
        sprintf(str, "CPU #%d at %d/%d", i, x, y);
        NOTIFY(str);
        EXIT0;
      }
    }
  }

  NOTIFY("LoC: Couldn't add CPU");
  EXIT0;
}
/****************************************************************************
	add a phone to a system
*/
short Add_Phone(struct system_struct *sys) {
  short   ok;
  short   x, y;
  short   phone_x, phone_y;
  short   done = FALSE;
  short   tries = 0;

  do {
    Random_Space(&phone_x, &phone_y);

    if (sys->node_type[phone_x][phone_y] == 0 && sys->type == CYBERDECK) {
      done = TRUE;
    } else if (sys->node_type[phone_x][phone_y] == 0) {
      for (x = phone_x, ok = FALSE; x >= 0; --x) {
        if (sys->node_type[x][phone_y]) {
          ok = TRUE;
        }
      }

      if
      (
        sys->node_type[phone_x - 1][phone_y] == OPENED_GATE ||
        sys->node_type[phone_x + 1][phone_y] == OPENED_GATE ||
        sys->node_type[phone_x][phone_y - 1] == OPENED_GATE ||
        sys->node_type[phone_x][phone_y + 1] == OPENED_GATE ||
        sys->node_type[phone_x - 1][phone_y] == CLOSED_GATE ||
        sys->node_type[phone_x + 1][phone_y] == CLOSED_GATE ||
        sys->node_type[phone_x][phone_y - 1] == CLOSED_GATE ||
        sys->node_type[phone_x][phone_y + 1] == CLOSED_GATE
      ) {
        ok = FALSE;
      }

      if (ok) {
        for (x = phone_x, ok = FALSE; x != MAX_SYS_X; ++x) {
          if (sys->node_type[x][phone_y]) {
            ok = TRUE;
          }
        }
      }

      if (ok) {
        for (y = phone_y, ok = FALSE; y != MAX_SYS_Y; ++y) {
          if (sys->node_type[phone_x][y]) {
            ok = TRUE;
          }
        }
      }

      if (ok) {
        for (y = phone_y, ok = FALSE; y >= 0; --y) {
          if (sys->node_type[phone_x][y]) {
            ok = TRUE;
          }
        }
      }

      if (ok) {
        done = TRUE;
      } else {
        ++tries;
      }
    } else {
      ++tries;
    }
  } while (!done && tries < 1000);

  if (done) {
    sys->node_type[phone_x][phone_y] = PHONE;
    return (TRUE);
  }

  return (FALSE);
}
/****************************************************************************
	remove a cpu from a system
*/
void Remove_CPU(struct system_struct *sys, short i) {
  short   x, y;

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      if (sys->node_type[x][y] == CPU && sys->node_index[x][y] == i) {
        sys->node_type[x][y] = 0;
      }
    }
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      if (sys->node_type[x][y] == CPU && sys->node_index[x][y] > i) {
        --sys->node_index[x][y];
      }
    }
  }

  for (x = i + 1; x < MAX_CPUS + 1; ++x) {
    sys->crash_time[x - 1] = sys->crash_time[x];
  }
}
/****************************************************************************
	remove a memory unit from a system
*/
void Remove_Memory(struct system_struct *sys, short mu_num) {
  short   x, y;
  short   i;
  short   index;

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      if
      (
        sys->node_type[x][y] == MEMORY &&
        sys->node_index[x][y] == mu_num
      ) {
        sys->node_type[x][y] = 0;
        sys->node_index[x][y] = 0;
      }
    }
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      if
      (
        sys->node_type[x][y] == MEMORY &&
        sys->node_index[x][y] > mu_num
      ) {
        --sys->node_index[x][y];
      }
    }
  }

  for (index = mu_num + 1; index < sys->mus; ++index) {
    for (i = 0; i != MU_CAPACITY; ++i) {
      sys->memory[index - 1][i] = sys->memory[index][i];
    }
  }
}
/****************************************************************************
	add a memory unit to a system
*/
short EXPORT Add_Memory(struct system_struct *sys, short i) {
  short   x, y;
  short   done = FALSE;
  short   tries = 0;

  do {
    ++tries;

    if (sys->type == VENDOR && i == 0 && tries == 1) {
      x = START_SYS_X;
      y = START_SYS_Y;
    } else {
      Random_Space(&x, &y);
    }

    if (sys->node_type[x][y] == 0) {
      if (sys->type == CYBERDECK) {
        done = TRUE;
      } else if (sys->type == VENDOR && i == 0 && tries == 1) {
        done = TRUE;
      } else if
      (
        sys->node_type[x - 1][y] != OPENED_GATE &&
        sys->node_type[x + 1][y] != OPENED_GATE &&
        sys->node_type[x][y - 1] != OPENED_GATE &&
        sys->node_type[x][y + 1] != OPENED_GATE &&
        sys->node_type[x - 1][y] != CLOSED_GATE &&
        sys->node_type[x + 1][y] != CLOSED_GATE &&
        sys->node_type[x][y - 1] != CLOSED_GATE &&
        sys->node_type[x][y + 1] != CLOSED_GATE &&
        sys->node_type[x - 1][y] != DATA_WALL &&
        sys->node_type[x + 1][y] != DATA_WALL &&
        sys->node_type[x][y - 1] != DATA_WALL &&
        sys->node_type[x][y + 1] != DATA_WALL
      ) {
        done = TRUE;
      }
    }
  } while (!done && tries < 5000);

  if (sys->node_type[x][y] == 0) {
    sys->node_type[x][y] = MEMORY;
    sys->node_index[x][y] = i;
    return (TRUE);
  }

  return (FALSE);
}
