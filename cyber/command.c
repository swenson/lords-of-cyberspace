#include <header.h>
#include <globals.h>

/***************************************************************************
	execute user's command
*/
void EXPORT Do_Command(void) {
  short   s;

  switch (command) {
  case GANG:
    Clear_Display_Window();
    Gang_Menu();
    break;

  case MISSION:
    Current_Mission();
    break;

  case WHO:
    Write_My_Rec(EXISTS);
    Who();
    break;

  case MAIL:
    Clear_Display_Window();
    Mail();
    break;

  case HELP:
    Help();
    break;

  case RANK:
    Clear_Display_Window();
    Display_My_Rank();
    break;

  case GO_OUTLAW:
    Outlaw(0);
    break;

  case REDRAW_SCREEN:
    Redraw();
    break;

  case TOPTEN:
    Write_My_Rec(EXISTS);
    Clear_Display_Window();
    Top_Nine(1);
    break;

  case LIST_PROGRAMS:
    List_Programs(0);
    break;

  case LIST_DOCS:
    Clear_Display_Window();
    List_Documents(0);
    break;

  case DAYFILE:
    if (MY_PORT->access_level != IDLE) {
      prfmsg(MSG184);
    } else {
      Dayfile();
    }

    break;

  case SERVICES:
    Clear_Display_Window();
    Offline_Services();
    break;

  case DESIGN:
    if (MY_PORT->access_level != IDLE) {
      Clear_Display_Window();
      prfmsg(MSG184);
    } else {
      Design_Deck();
    }

    break;

  case STOP:
    Clear_Display_Window();
    Stop_Program(STATE);
    break;

  case CREATE:
    Clear_Display_Window();
    Create_Program();
    break;

  case CPUS:
    Clear_Display_Window();

    if (margc > 1 && me.perm[GAMEOP_PERM]) {
      s = INUM(parm1);

      if (s < 0 || s >= max_systems) {
        prfmsg(MSG272, max_systems - 1);
      } else if (!Sys[s]->in_use) {
        prfmsg(MSG273, s);
      } else {
        if (Sys[s]->net_x >= 0 && Sys[s]->net_y >= 0) {
          prfmsg
          (
            MSG792,
            world->city[Sys[s]->net_x][Sys[s]->net_y].name
          );
        }

        prfmsg
        (
          MSG793,
          Sys[s]->net_x,
          Sys[s]->net_y,
          Sys[s]->Name,
          Sys[s]->city_x,
          Sys[s]->city_y
        );
        CPU_Status(s);
      }
    } else {
      CPU_Status(myp);
    }

    break;

  case TIME:
    Clear_Display_Window();
    Display_Time();
    break;

  case WHERE:
    Where(0);
    break;

  case PERMITS:
    Clear_Display_Window();

    if (margc == 1) {
      List_My_Perms();
    } else if
    (
      me.perm[SET_PERM] ||
      (glob->sysopkey[0] && haskey(glob->sysopkey))
    ) {
      Set_Perms();
    } else {
      prfmsg(MSG274);
    }

    break;

  case EDIT:
    Clear_Display_Window();

    if (!me.perm[EDIT_WORLD_PERM] && !me.perm[EDIT_USER_PERM]) {
      prfmsg(MSG274);
    } else if (MY_PORT->access_level != IDLE) {
      prfmsg(MSG184);
    } else {
      Edit();
    }

    break;

  case SUICIDE:
    Clear_Display_Window();

    if (MY_PORT->access_level != IDLE) {
      prfmsg(MSG184);
    } else {
      if
      (
        !me.perm[GAMEOP_PERM] &&
        glob->suihours &&
        time(0) - me.created < glob->suihours
      ) {
        prfmsg
        (
          MSG823,
          (short) ((glob->suihours - (time(0) - me.created)) / 60L)
        );
      } else {
        Prompt(getmsg(MSG275), SUICIDE_PROMPT);
      }
    }

    break;

  case OPTION_OPS:
    Clear_Display_Window();

    if (margc == 1) {
      List_My_Options();
    } else {
      Set_Options();
    }

    break;

  case LOOK:
    Clear_Display_Window();
    Look(parm1);
    break;

  case JACK:
    Clear_Display_Window();
    printf("MY_PORT->access_level = %d == IDLE = %d\n", MY_PORT->access_level,
           MY_PORT->access_level == IDLE);

    if (MY_PORT->access_level == IDLE) {
      printf("Calling jackin\n");
      Jack_In();
    } else if (MY_PORT->icebolt_time && !me.perm[GAMEOP_PERM]) {
      prfmsg(MSG277, MY_PORT->icebolt_time);
    } else if (MY_PORT->handcuff_time && !me.perm[GAMEOP_PERM]) {
      prfmsg(MSG278, MY_PORT->handcuff_time);
    } else if
    (
      MY_PORT->sys_num != myp &&
      RANDOM(glob->jackroll) == 1 &&
      !me.perm[GAMEOP_PERM]
    ) {
      prfmsg(MSG279);
      MY_PORT->handcuff_time = glob->jacksecs;
    } else {
      Jack_Out();
    }

    break;

  case FINANCES:
    Clear_Display_Window();
    Finances();
    break;

  case NORTH:
  case SOUTH:
  case EAST:
  case WEST:
    if (MY_PORT->access_level != IDLE) {
      Moving();
    } else {
      Clear_Display_Window();
      prfmsg(MSG276);
    }

    break;

  case BACK:
    Clear_Display_Window();

    if (MY_PORT->icebolt_time) {
      prfmsg(MSG277, MY_PORT->icebolt_time);
    } else {
      Back_Up();
    }

    break;

  case DECK:
    Clear_Display_Window();
    Display_Deck();
    break;

  case MEMORY_SERVICES:
    Clear_Display_Window();

    if (parm1[0]) {
      s = Find_Memory
          (
            parm1,
            &MY_PORT->source_s,
            &MY_PORT->source_m
          );

      if (s == 0) {
        MY_PORT->source_s = MY_PORT->source_m = -1;
      }
    } else {
      MY_PORT->source_s = MY_PORT->source_m = -1;
    }

    Memory_Services();
    break;

  case TOURNEY:
    Clear_Display_Window();

    if (MY_PORT->access_level != IDLE) {
      prfmsg(MSG184);
    } else {
      Tourney_Services();
    }

    break;

  case CHIPS:
    Clear_Display_Window();

    if (MY_PORT->access_level != IDLE) {
      prfmsg(MSG184);
    } else if (me.backup_chips == 0) {
      prfmsg(MSG541);
    } else if (!me.chip_reader) {
      prfmsg(MSG166);
    } else {
      if (parm1[0]) {
        s = Find_Chip(parm1, &MY_PORT->source_m);

        if (s == 0) {
          MY_PORT->source_m = -1;
        }
      } else {
        MY_PORT->source_m = -1;
      }

      Backup_Services();
    }

    break;
  }
}
/****************************************************************************
	user wants to change their alias
*/
short Change_Alias(void) {
  short   i;
  char    name[NAME_SIZE];
  Fix_Alias(name);

  for (i = 1; i != NUM_PROGRAMS; ++i) {
    if (sameas(name, Programs[i].Name)) {
      prfmsg(MSG280, Programs[i].Name);
      EXIT0;
    }
  }

  i = Find_Alias(a_human, name);

  if
  (
    i >= 0 ||
    sameas(name, "AUTOMATED") ||
    sameas(name, CANCEL) ||
    sameto(A_GANG, name)
  ) {
    prfmsg(MSG281, name);
    EXIT0;
  }

  strcpy(me.alias, name);
  me.last_name_change = time(0);
  Write_My_Rec(EXISTS);
  EXIT0;
}
/****************************************************************************
	list all the programs I have
*/
short List_Programs(short option) {
  short   m, p;
  short   s;
  short   subs;
  short   index = 0;
  short   l = DISP_Y;
  char    lock_str[10];
  char    pack_str[10];
  char    run_str[10];

  if (option == 0) {
    MY_PORT->m_index = 0;
    MY_PORT->p_index = 0;
  } else {
    if (margc == 0) {
      if (MY_PORT->m_index == -1) {
        STATE = MAIN_PROMPT;
        EXIT0;
      }
    } else if (sameas(margv[0], "R")) {
      sprintf(str, getmsg(MSG282), MY_PORT->progs);
      Prompt(str, RUN_PROMPT);
      EXIT0;
    } else if (sameas(margv[0], "I")) {
      sprintf(str, getmsg(MSG283), MY_PORT->progs);
      Prompt(str, IMPROVE_PROMPT);
      EXIT0;
    } else if (sameas(margv[0], "C")) {
      sprintf(str, getmsg(MSG284), MY_PORT->progs);
      Prompt(str, CONFIGURE_PROMPT);
      EXIT0;
    } else {
      if (MY_PORT->m_index == -1) {
        STATE = MAIN_PROMPT;
        EXIT0;
      }
    }
  }

  Clear_Display_Window();

  for (m = MY_PORT->m_index; m < MY_DECK->mus; ++m) {
    if (m != MY_PORT->m_index) {
      MY_PORT->p_index = 0;
    }

    for (p = MY_PORT->p_index; p < MU_CAPACITY; ++p) {
      if (MY_DECK->memory[m][p].type == PROGRAM) {
        if (l == ASYNCH_Y) {
          MY_PORT->m_index = m;
          MY_PORT->p_index = p;
          MY_PORT->progs = index;
          Prompt(getmsg(MSG285), PROGRAMS_PROMPT);
          EXIT0;
        }

        strcpy(lock_str, " ");
        strcpy(pack_str, " ");
        strcpy(run_str, " ");

        if (MY_DECK->memory[m][p].lock) {
          strcpy(lock_str, "L");
        }

        if (MY_DECK->memory[m][p].packed) {
          strcpy(pack_str, "P");
        }

        if (MY_DECK->memory[m][p].running) {
          strcpy(run_str, "R");
        }

        prfmsg
        (
          MSG286,
          index + 1,
          m + 1,
          p + 1,
          lock_str,
          pack_str,
          run_str,
          MY_DECK->memory[m][p].value,
          MY_DECK->memory[m][p].strength,
          File_Size(&MY_DECK->memory[m][p]),
          MY_DECK->memory[m][p].name,
          program_class[MY_DECK->memory[m][p].class]
        );

        if (index >= MAX_PROGLIST) {
          catastro("LoC: MAX_PROGLIST exceeded");
        }

        MY_PORT->proglist[index].m_index = m;
        MY_PORT->proglist[index].p_index = p;
        ++index;

        if (MY_DECK->memory[m][p].class == HYDRA) {
          for (s = subs = 0; s != MAX_SUBS; ++s) {
            if (MY_DECK->memory[m][p].sub[s]) {
              ++subs;
            }
          }

          prfmsg(MSG287, subs);
        }

        prf(NL);
        ++l;
      }
    }
  }

  if (l == DISP_Y && MY_PORT->m_index == 0 && MY_PORT->p_index == 0) {
    prfmsg(MSG289);
  } else {
    MY_PORT->m_index = -1;
    MY_PORT->p_index = -1;
    MY_PORT->progs = index;
    Prompt(getmsg(MSG290), PROGRAMS_PROMPT);
    EXIT0;
  }

  STATE = MAIN_PROMPT;
  EXIT0;
}
/****************************************************************************
	user wants to become an outlaw
*/
short Outlaw(short option) {
  if (option == 0) {
    if (!glob->outlaw) {
      prfmsg(MSG668);
      EXIT0;
    }

    if (me.outlaw) {
      prfmsg(MSG666);
      EXIT0;
    }

    prfmsg(MSG665);
    Prompt(getmsg(MSG669), OUTLAW_PROMPT);
    EXIT0;
  }

  if (!sameas(margv[0], "y")) {
    EXIT0;
  }

  me.outlaw = TRUE;
  prfmsg(MSG667);
  EXIT0;
}
/****************************************************************************
	list all the documents I have
*/
short List_Documents(short option) {
  short   m, p;
  short   l = DISP_Y;
  char    lock_str[10];
  char    pack_str[10];
  short   index = 0;

  if (option == 0) {
    MY_PORT->m_index = 0;
    MY_PORT->p_index = 0;
  } else {
    if (margc == 0) {
      if (MY_PORT->m_index == -1) {
        STATE = MAIN_PROMPT;
        EXIT0;
      }
    } else if (sameas(margv[0], "E")) {
      sprintf(str, getmsg(M842A), MY_PORT->progs);
      Prompt(str, ERASE_DOC_PROMPT);
      EXIT0;
    } else if (sameas(margv[0], "R")) {
      sprintf(str, getmsg(M842B), MY_PORT->progs);
      Prompt(str, READ_DOC_PROMPT);
      EXIT0;
    }

    Clear_Display_Window();
  }

  for (m = MY_PORT->m_index; m < MY_DECK->mus; ++m) {
    if (m != MY_PORT->m_index) {
      MY_PORT->p_index = 0;
    }

    for (p = MY_PORT->p_index; p < MU_CAPACITY; ++p) {
      if (MY_DECK->memory[m][p].type == DOCUMENT) {
        if (l == ASYNCH_Y - 1) {
          MY_PORT->m_index = m;
          MY_PORT->p_index = p;
          MY_PORT->progs = index;
          Prompt(getmsg(M841), DOCS_PROMPT);
          EXIT0;
        }

        strcpy(lock_str, " ");
        strcpy(pack_str, " ");

        if (MY_DECK->memory[m][p].lock) {
          strcpy(lock_str, "L");
        }

        if (MY_DECK->memory[m][p].packed) {
          strcpy(pack_str, "P");
        }

        if
        (
          (MY_DECK->memory[m][p].packed || MY_DECK->memory[m][p].lock) &&
          !me.perm[GAMEOP_PERM]
        ) {
          prfmsg
          (
            M291,
            index + 1,
            m + 1,
            p + 1,
            lock_str,
            pack_str,
            File_Size(&MY_DECK->memory[m][p]),
            MY_DECK->memory[m][p].name
          );
        } else {
          prfmsg
          (
            M292,
            index + 1,
            m + 1,
            p + 1,
            lock_str,
            pack_str,
            MY_DECK->memory[m][p].value,
            File_Size(&MY_DECK->memory[m][p]),
            MY_DECK->memory[m][p].name,
            document_class[MY_DECK->memory[m][p].class]
          );
        }

        ++l;

        if (index >= MAX_PROGLIST) {
          catastro("LoC: MAX_PROGLIST exceeded");
        }

        MY_PORT->proglist[index].m_index = m;
        MY_PORT->proglist[index].p_index = p;
        ++index;
      }
    }
  }

  if (l == DISP_Y - 1 && MY_PORT->m_index == 0 && MY_PORT->p_index == 0) {
    prfmsg(MSG293);
    STATE = MAIN_PROMPT;
  } else {
    MY_PORT->m_index = -1;
    MY_PORT->p_index = -1;
    MY_PORT->progs = index;
    Prompt(getmsg(M843), DOCS_PROMPT);
  }

  EXIT0;
}
/****************************************************************************
	pick an offline service
*/
short Offline_Services(void) {
  if (MY_PORT->access_level != IDLE) {
    prfmsg(MSG184);
    EXIT0;
  }

  prf("%s", CLEAR_RIGHT);
  prfmsg(MSG8Y);
  Prompt(getmsg(M294X), SERVICES_PROMPT);
  Service_Display();
  EXIT0;
}
/****************************************************************************
	pick a memory service
*/
short Memory_Services(void) {
  short   i;

  if (MY_DECK->mus == 0) {
    prfmsg(MSG295);
    STATE = MAIN_PROMPT;
    EXIT0;
  }

  prf("%s", CLEAR_RIGHT);

  if (MY_PORT->source_s >= 0 && MY_PORT->source_m >= 0) {
    i = Memory_Contents
        (
          Sys[MY_PORT->source_s]->memory[MY_PORT->source_m],
          0
        );

    if (i == 0) {
      MY_PORT->source_s = -1;
      MY_PORT->source_m = -1;
    } else {
      if
      (
        MY_PORT->access_level == IDLE &&
        me.chip_reader &&
        me.backup_chips
      ) {
        Prompt(getmsg(MSG175B), MEMORY_PROMPT2);
      } else {
        Prompt(getmsg(MSG175A), MEMORY_PROMPT2);
      }

      EXIT0;
    }
  }

  if (MY_PORT->memory_bonk) {
    MY_PORT->memory_bonk = FALSE;
    STATE = MAIN_PROMPT;
    EXIT0;
  }

  Memory_Summary();
  sprintf(str, getmsg(MSG89), glob->dirs);
  Prompt(str, MEMORY_PROMPT);
  EXIT0;
}
/****************************************************************************
	pick a backup service
*/
short Backup_Services(void) {
  short   i;
  prf("%s", CLEAR_RIGHT);

  if (MY_PORT->source_m >= 0) {
    i = Memory_Contents(me.backup[MY_PORT->source_m], 0);

    if (i == 0) {
      MY_PORT->source_m = -1;
    } else {
      Prompt(getmsg(MSG855), RETRIEVE_PROMPT);
      EXIT0;
    }
  }

  Backup_Summary();
  sprintf(str, getmsg(M170), me.backup_chips);
  Prompt(str, RETRIEVE_PROMPT1);
  EXIT0;
}
/****************************************************************************
	find someplace to live
*/
short Rent_Flat(void) {
  long    cost;

  if (me.renting) {
    prfmsg(MSG297);
    EXIT0;
  }

  cost = Rent() * 2;

  if (cost > me.bank.balance && !me.perm[FREE_PERM]) {
    sprintf(str, getmsg(MSG298), cost);
    prf(str);
    EXIT0;
  }

  me.renting = TRUE;
  me.rentdue = cofdat(today()) + glob->rentdays;
  me.rentdue = datofc(me.rentdue);

  if (!me.perm[FREE_PERM]) {
    Add_Bank_Record
    (
      &me,
      -(cost),
      me.start_x, me.start_y,
      me.alias,
      "Renting"
    );
  }

  EXIT0;
}
/****************************************************************************
	pick a city to relocate to
*/
short Relocate(short option) {
  short   net_x;
  short   net_y;
  short   start_x;
  short   start_y;
  short   n;
  long    cost = glob->minair;
  short   city_num;
  short   done = FALSE;
  short   mass1, mass2;

  if (option == 0) {
    World_View();
    n = Display_Cities();
    sprintf(str, getmsg(MSG299), n);
    Prompt(str, RELOCATE_PROMPT);
    EXIT0;
  }

  city_num = INUM(margv[0]);

  for (net_x = n = 0; net_x != MAX_NET_X && !done; ++net_x) {
    for (net_y = 0; net_y != MAX_NET_Y && !done; ++net_y) {
      if
      (
        !sameas(world->city[net_x][net_y].name, FRONTIER) &&
        !sameas(world->city[net_x][net_y].name, VALHALLA) &&
        !sameas(world->city[net_x][net_y].name, ALIEN_CITY) &&
        !sameas(world->city[net_x][net_y].name, CYBERIA) &&
        (!world->city[net_x][net_y].invisible || me.perm[GAMEOP_PERM])
      ) {
        ++n;

        if (n == city_num) {
          done = TRUE;
          start_x = net_x;
          start_y = net_y;
        }
      }
    }
  }

  if (!done) {
    EXIT0;
  }

  if (me.start_x >= 0 || me.start_y >= 0) {
    mass1 = world->city[start_x][start_y].landmass;
    mass2 = world->city[me.start_x][me.start_y].landmass;

    if (mass1 == SAMERICA) {
      if (mass2 == NAMERICA) {
        cost += glob->airadd;
      } else if (mass2 != SAMERICA) {
        cost += glob->airadd * 2;
      }
    } else if (mass1 == NAMERICA) {
      if (mass2 == SAMERICA) {
        cost += glob->airadd;
      } else if (mass2 != NAMERICA) {
        cost += glob->airadd * 2;
      }
    } else if (mass1 == AFRICA) {
      if (mass2 == EUROPE) {
        cost += glob->airadd;
      } else if (mass2 != AFRICA) {
        cost += glob->airadd * 2;
      }
    } else if (mass1 == ASIA) {
      if (mass2 != ASIA) {
        cost += glob->airadd * 2;
      }
    } else if (mass1 == EUROPE) {
      if (mass2 == AFRICA) {
        cost += glob->airadd;
      } else if (mass2 != EUROPE) {
        cost += glob->airadd * 2;
      }
    }

    sprintf
    (
      str,
      getmsg(MSG300),
      world->city[me.start_x][me.start_y].name,
      world->city[start_x][start_y].name,
      cost
    );
    prf(str);

    if (!me.perm[FREE_PERM]) {
      if (me.bank.balance < cost && !me.perm[FREE_PERM]) {
        prfmsg(MSG301);
        EXIT0;
      }

      Add_Bank_Record
      (
        &me,
        -(cost),
        me.start_x, me.start_y,
        me.alias,
        "Relocation"
      );
    }
  }

  if (me.start_x >= 0 || me.start_y >= 0) {
    prfmsg(MSG302);
    me.renting = FALSE;
    me.employed = FALSE;
    me.arrest_time = 0;
    me.start_x = start_x;
    me.start_y = start_y;
  } else {
    me.start_x = start_x;
    me.start_y = start_y;
    Update_Location();
    Jack_In();
  }

  EXIT0;
}
/****************************************************************************
	user is buying some cpus
*/
short CPU_Bought(void) {
  short   i;
  short   how_many;
  short   new_cpus;
  long    cost;
  long    sold = 0;
  short   new_total;
  how_many = INUM(margv[0]);

  if (how_many == 0) {
    EXIT0;
  }

  cost = (long) how_many * (long) MY_PORT->misc * (long) glob->cpucost;

  if (MY_PORT->misc != MY_DECK->speed) {
    sold = (long) MY_DECK->cpus * (long) MY_DECK->speed;
    sold = (sold * (long) glob->cpucost) / 2L;
  }

  if (me.bank.balance + sold < cost && !me.perm[FREE_PERM]) {
    prfmsg(MSG303);
    EXIT0;
  }

  if (MY_PORT->misc != MY_DECK->speed) {
    new_total = how_many;
  } else {
    new_total = MY_DECK->cpus + how_many;
  }

  if (new_total > MAX_CPUS) {
    prfmsg(MSG304, MAX_CPUS);
    EXIT0;
  }

  Add_Bank_Record
  (
    &me,
    sold,
    me.start_x, me.start_y,
    me.alias,
    "CPU Trade-In"
  );
  Stop_All_Programs(myp);
  MY_DECK->speed = MY_PORT->misc;

  if (new_total > MY_DECK->cpus) {
    new_cpus = new_total - MY_DECK->cpus;

    for (i = 0; i != new_cpus; ++i) {
      Add_CPU(MY_DECK, MY_DECK->cpus);
      MY_PROC->cpu[MY_DECK->cpus].m_index = -1;
      MY_DECK->crash_time[MY_DECK->cpus] = 0;
      ++MY_DECK->cpus;
    }
  } else if (new_total < MY_DECK->cpus) {
    for (i = MY_DECK->cpus - 1; i > new_total - 1; --i) {
      Remove_CPU(MY_DECK, i);
    }

    MY_DECK->cpus = new_total;
  }

  if (!me.perm[FREE_PERM]) {
    Add_Bank_Record
    (
      &me,
      -(cost),
      me.start_x, me.start_y,
      me.alias,
      "CPU Purchase"
    );
  }

  EXIT0;
}
/****************************************************************************
	user is buying something
*/
short Buying(void) {
  short   i;
  short   m;
  short   what = INUM(margv[0]);
  short   ok;
  long    cost;

  if (what < 1 || what > 8) {
    EXIT0;
  }

  Clear_Display_Window();

  if (what == 1) {
    cost = glob->facecost;

    if (me.interface) {
      prfmsg(MSG305);
      EXIT0;
    }

    if (me.bank.balance < cost && !me.perm[FREE_PERM]) {
      prfmsg(MSG303);
      EXIT0;
    }

    me.interface = TRUE;
  } else if (what == 2) {
    Prompt(getmsg(MSG306), BUY_CPU_PROMPT1);
    EXIT0;
  } else if (what == 3) {
    cost = glob->memcost;

    if (MY_DECK->mus == MAX_MUS) {
      prfmsg(MSG307, MAX_MUS);
      EXIT0;
    }

    if (MY_DECK->cpus == 0) {
      prfmsg(MSG624);
      EXIT0;
    }

    if (me.bank.balance < cost && !me.perm[FREE_PERM]) {
      prfmsg(MSG303);
      EXIT0;
    }

    Stop_All_Programs(myp);
    m = MY_DECK->mus;
    ok = Add_Memory(MY_DECK, m);

    if (!ok) {
      prfmsg(MSG650);
      EXIT0;
    }

    for (i = 0; i != MU_CAPACITY; ++i) {
      MY_DECK->memory[m][i].packed = FALSE;
      MY_DECK->memory[m][i].lock = FALSE;
      MY_DECK->memory[m][i].type = 0;
      MY_DECK->memory[m][i].index = 0;
      MY_DECK->memory[m][i].name[0] = 0;
    }

    ++MY_DECK->mus;
  } else if (what == 4) {
    cost = (long) glob->wallcost * (long) (MY_DECK->wall + 1);

    if (me.bank.balance < cost && !me.perm[FREE_PERM]) {
      prfmsg(MSG303);
      EXIT0;
    }

    ++MY_DECK->wall;
  } else if (what == 5) {
    cost = (long) glob->gatecost * (long) (MY_DECK->gate + 1);

    if (me.bank.balance < cost && !me.perm[FREE_PERM]) {
      prfmsg(MSG303);
      EXIT0;
    }

    ++MY_DECK->gate;
  } else if (what == 6) {
    cost = glob->crcost;

    if (me.chip_reader) {
      prfmsg(MSG308);
      EXIT0;
    }

    if (me.bank.balance < cost && !me.perm[FREE_PERM]) {
      prfmsg(MSG303);
      EXIT0;
    }

    me.chip_reader = TRUE;
  } else if (what == 7) {
    cost = glob->chipcost;

    if (me.backup_chips == MAX_BACKUP_CHIPS) {
      prfmsg(MSG309, MAX_BACKUP_CHIPS);
      EXIT0;
    }

    if (me.bank.balance < cost && !me.perm[FREE_PERM]) {
      prfmsg(MSG303);
      EXIT0;
    }

    m = me.backup_chips;

    for (i = 0; i != MU_CAPACITY; ++i) {
      me.backup[m][i].packed = FALSE;
      me.backup[m][i].lock = FALSE;
      me.backup[m][i].type = 0;
      me.backup[m][i].index = 0;
      me.backup[m][i].name[0] = 0;
    }

    ++me.backup_chips;
  } else if (what == 8) {
    cost = glob->ldpcost;

    if (me.phone) {
      prfmsg(MSG899);
      EXIT0;
    }

    if (me.bank.balance < cost && !me.perm[FREE_PERM]) {
      prfmsg(MSG303);
      EXIT0;
    }

    me.phone = TRUE;
    ok = Add_Phone(MY_DECK);

    if (!ok) {
      prfmsg(MSG900);
      EXIT0;
    }
  }

  if (!me.perm[FREE_PERM]) {
    Add_Bank_Record
    (
      &me,
      -(cost),
      me.start_x, me.start_y,
      me.alias,
      "Hardware Buy"
    );
  }

  EXIT0;
}
/****************************************************************************
	user is selling something
*/
short Selling(void) {
  short   i;
  short   m;
  short   what = INUM(margv[0]);
  short   n;
  short   x, y;
  long    cost;

  if (what < 1 || what > 7) {
    EXIT0;
  }

  Clear_Display_Window();

  if (what == 1) {
    cost = glob->ldpcost / 2;

    if (!me.phone) {
      prfmsg(MSG916);
      EXIT0;
    }

    me.phone = FALSE;

    for (x = 0; x != MAX_SYS_X; ++x) {
      for (y = 0; y != MAX_SYS_Y; ++y) {
        if (MY_DECK->node_type[x][y] == PHONE) {
          MY_DECK->node_type[x][y] = 0;
        }
      }
    }
  } else if (what == 2) {
    if (MY_DECK->cpus == 0) {
      prfmsg(MSG911);
      EXIT0;
    }

    cost = ((long) MY_DECK->speed * (long) glob->cpucost) / 2L;
    Stop_All_Programs(myp);
    Remove_CPU(MY_DECK, 0);
    --MY_DECK->cpus;
  } else if (what == 3) {
    for (m = 0, n = -1; m != MY_DECK->mus && n == -1; ++m) {
      n = m;

      for (i = 0; i != MU_CAPACITY; ++i) {
        if (MY_DECK->memory[m][i].type) {
          n = -1;
        }
      }
    }

    if (n == -1) {
      prfmsg(MSG910);
      EXIT0;
    }

    cost = glob->memcost / 2;
    Stop_All_Programs(myp);
    Remove_Memory(MY_DECK, n);
    --MY_DECK->mus;
  } else if (what == 4) {
    cost = ((long) glob->wallcost * (long) MY_DECK->wall) / 2L;

    if (MY_DECK->wall <= glob->stwall) {
      prfmsg(MSG912);
      EXIT0;
    }

    --MY_DECK->wall;
  } else if (what == 5) {
    cost = ((long) glob->gatecost * (long) MY_DECK->gate) / 2L;

    if (MY_DECK->gate <= glob->stgate) {
      prfmsg(MSG913);
      EXIT0;
    }

    --MY_DECK->gate;
  } else if (what == 6) {
    cost = glob->crcost / 2;

    if (!me.chip_reader) {
      prfmsg(MSG914);
      EXIT0;
    }

    me.chip_reader = FALSE;
  } else if (what == 7) {
    for (m = 0, n = -1; m != me.backup_chips && n == -1; ++m) {
      n = m;

      for (i = 0; i != MU_CAPACITY; ++i) {
        if (me.backup[m][i].type) {
          n = -1;
        }
      }
    }

    if (n == -1) {
      if (me.backup_chips && !me.chip_reader) {
        n = me.backup_chips - 1;
      } else {
        prfmsg(MSG915);
        EXIT0;
      }
    }

    for (m = n + 1; m < me.backup_chips; ++m) {
      for (i = 0; i != MU_CAPACITY; ++i) {
        me.backup[m - 1][i] = me.backup[m][i];
      }
    }

    cost = glob->chipcost / 2;
    --me.backup_chips;
  }

  Add_Bank_Record
  (
    &me,
    cost,
    me.start_x, me.start_y,
    me.alias,
    "Pawned Hardware"
  );
  EXIT0;
}
/****************************************************************************
	user wants to buy some hardware
*/
void Buy_Hardware(void) {
  char    dummy[25];
  sprintf(str, "$%d", glob->facecost);
  prfmsg(MSSG310, str);

  if (me.interface) {
    prfmsg(MSG311, CLEAR_RIGHT);
  } else {
    prfmsg(MSG312, CLEAR_RIGHT);
  }

  sprintf(str, "$%d X speed", glob->cpucost);
  sprintf
  (
    dummy,
    "$%ld",
    ((long) MY_DECK->speed * (long) MY_DECK->cpus * (long) glob->cpucost) / 2L
  );
  prfmsg
  (
    MSSG313,
    str,
    MY_DECK->cpus,
    MY_DECK->speed,
    dummy,
    CLEAR_RIGHT
  );
  sprintf(str, "$%d", glob->memcost);
  prfmsg(MSSG314, str, MY_DECK->mus, CLEAR_RIGHT);
  sprintf(str, "$%ld", (long) glob->wallcost * (long) (MY_DECK->wall + 1));
  prfmsg(MSSG315, str, MY_DECK->wall, CLEAR_RIGHT);
  sprintf(str, "$%ld", (long) glob->gatecost * (long) (MY_DECK->gate + 1));
  prfmsg(MSSG316, str, MY_DECK->gate, CLEAR_RIGHT);
  sprintf(str, "$%d", glob->crcost);
  prfmsg(MSSG317, str);

  if (me.chip_reader) {
    prfmsg(MSG311, CLEAR_RIGHT);
  } else {
    prfmsg(MSG312, CLEAR_RIGHT);
  }

  sprintf(str, "$%d", glob->chipcost);
  prfmsg(MX318, str, me.backup_chips, CLEAR_RIGHT);
  sprintf(str, "$%d", glob->ldpcost);
  prfmsg(MSG898, str);

  if (me.phone) {
    prfmsg(MSG311, CLEAR_RIGHT);
  } else {
    prfmsg(MSG312, CLEAR_RIGHT);
  }

  Prompt(getmsg(MX319), BUY_PROMPT);
}
/****************************************************************************
	user wants to sell some hardware
*/
void Sell_Hardware(void) {
  char    dummy[25];
  sprintf(str, "$%d", glob->ldpcost / 2);
  prfmsg(MSG921, str);

  if (me.phone) {
    prfmsg(MSG311, CLEAR_RIGHT);
  } else {
    prfmsg(MSG312, CLEAR_RIGHT);
  }

  sprintf(str, "$%d X speed", glob->cpucost / 2);
  sprintf
  (
    dummy,
    "$%ld",
    ((long) MY_DECK->speed * (long) glob->cpucost) / 2L
  );
  prfmsg
  (
    MSSG313,
    str,
    MY_DECK->cpus,
    MY_DECK->speed,
    dummy,
    CLEAR_RIGHT
  );
  sprintf(str, "$%d", glob->memcost / 2);
  prfmsg(MSSG314, str, MY_DECK->mus, CLEAR_RIGHT);
  sprintf(str, "$%ld", ((long) glob->wallcost * (long) MY_DECK->wall) / 2);
  prfmsg(MSSG315, str, MY_DECK->wall, CLEAR_RIGHT);
  sprintf(str, "$%ld", ((long) glob->gatecost * (long) MY_DECK->gate) / 2);
  prfmsg(MSSG316, str, MY_DECK->gate, CLEAR_RIGHT);
  sprintf(str, "$%d", glob->crcost / 2);
  prfmsg(MSSG317, str);

  if (me.chip_reader) {
    prfmsg(MSG311, CLEAR_RIGHT);
  } else {
    prfmsg(MSG312, CLEAR_RIGHT);
  }

  sprintf(str, "$%d", glob->chipcost / 2);
  prfmsg(MX318, str, me.backup_chips, CLEAR_RIGHT);
  Prompt(getmsg(M908), SELL_PROMPT);
}
/****************************************************************************
	user went to the hospital to get healed
*/
short Hospital(void) {
  long    cost;
  short   pts;
  pts = abs(atoi(margv[0]));

  if (pts < 1) {
    EXIT0;
  }

  if (pts > me.damage) {
    pts = me.damage;
  }

  cost = (long) glob->hospcost * (long) pts;

  if (cost > me.bank.balance && !me.perm[FREE_PERM]) {
    sprintf(str, getmsg(MSG320), cost);
    prf(str);
    EXIT0;
  }

  if (!me.perm[FREE_PERM]) {
    Add_Bank_Record
    (
      &me,
      -(cost),
      me.start_x, me.start_y,
      me.alias,
      "Medical Bill"
    );
  }

  me.damage -= pts;
  EXIT0;
}
/****************************************************************************
	sysop wants to create a program
*/
short Create_Program(void) {
  short   avail;
  short   m = -1;
  short   i;
  short   s;
  short   strength;
  short   value = 0;
  short   p = 0;

  if (!me.perm[CREATE_PERM]) {
    prfmsg(MSG274);
    EXIT0;
  }

  if (margc == 1) {
    prfmsg(MSG321);
    EXIT0;
  }

  for (i = 1; i != NUM_PROGRAMS; ++i) {
    if (sameto(parm1, Programs[i].Name)) {
      p = i;
    }
  }

  if (p == 0) {
    prfmsg(MSG322, parm1);
    EXIT0;
  }

  for (i = 0; i != MY_DECK->mus && m < 0; ++i) {
    avail = MU_CAPACITY - Memory_Usage(MY_DECK->memory[i]);

    if (avail >= Programs[p].size) {
      m = i;
    }
  }

  if (m == -1) {
    prfmsg(MSG323, Programs[p].Name);
    EXIT0;
  }

  prfmsg(MSG324, Programs[p].Name, m + 1);

  for (i = 0; i != MU_CAPACITY; ++i) {
    if (!MY_DECK->memory[m][i].type) {
      MY_DECK->memory[m][i].packed = FALSE;
      MY_DECK->memory[m][i].lock = FALSE;
      MY_DECK->memory[m][i].type = PROGRAM;
      MY_DECK->memory[m][i].index = p;
      MY_DECK->memory[m][i].class = Programs[p].class;
      MY_DECK->memory[m][i].size = Programs[p].size;
      MY_DECK->memory[m][i].load = Programs[p].load;
      MY_DECK->memory[m][i].vended = FALSE;
      strcpy(MY_DECK->memory[m][i].name, Programs[p].Name);

      for (s = 0; s != MAX_SUBS; ++s) {
        MY_DECK->memory[m][i].sub[s] = 0;
      }

      if (parm2[0]) {
        strength = INUM(parm2);

        if (strength > 99) {
          strength = 99;
        }
      } else {
        strength = Programs[p].strength;
      }

      if (strength > 0) {
        MY_DECK->memory[m][i].strength = strength;
      } else {
        MY_DECK->memory[m][i].strength = 1;
      }

      if (parm3[0]) {
        value = INUM(parm3);
      }

      if (value >= 0) {
        MY_DECK->memory[m][i].value = value;
      } else {
        MY_DECK->memory[m][i].value = Programs[p].value;
      }

      EXIT0;
    }
  }

  EXIT0;
}
/****************************************************************************
	toggle an option
*/
short Set_Options(void) {
  short   opt;
  opt = INUM(parm1);

  if (opt < 1 || opt > NUM_OPTS) {
    prfmsg(MSG325, opt);
    EXIT0;
  }

  --opt;

  if (!me.option[opt]) {
    me.option[opt] = TRUE;
  } else {
    me.option[opt] = FALSE;
  }

  if (opt == IBM_OPT) {
    Redraw();
    prf(NL);
  }

  List_My_Options();

  if (opt == CHAT_OPT && me.option[CHAT_OPT]) {
    prfmsg(MSG654, glob->chatkey, glob->chatkey);
  }

  EXIT0;
}
/****************************************************************************
	allows validated users to toggle their permissions
*/
void EXPORT Set_Perms(void) {
  short   perm;

  if (parm1[0]) {
    perm = INUM(parm1);

    if (perm > NUM_PERMS) {
      prfmsg(MSG326, perm);
    } else {
      me.perm[perm] = abs(me.perm[perm] - 1);

      if (glob->sysopkey[0] && haskey(glob->sysopkey)) {
        me.perm[SET_PERM] = TRUE;
      }

      List_My_Perms();

      if (perm == MONITOR_PERM) {
        MY_PORT->monitoring = me.perm[perm];
      }
    }
  } else {
    prfmsg(MSG327, NUM_PERMS);
  }
}
