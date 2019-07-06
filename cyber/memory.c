#include <header.h>
#include <globals.h>

/**************************************************************************
	run unlocker from the memory prompt
*/
short EXPORT Memory_Unlock(void) {
  short   found = FALSE;
  short   strength = 0;
  short   m_index = -1;
  short   p_index = -1;
  short   running = -1;
  short   locked = -1;
  short   packed = -1;
  short   old_p;
  short   old_m;
  short   i;
  short   j;

  for (j = 0; j != MY_DECK->mus; ++j) {
    for (i = 0; i != MU_CAPACITY; ++i) {
      if
      (
        MY_DECK->memory[j][i].type == PROGRAM &&
        MY_DECK->memory[j][i].index == UNLOCKER
      ) {
        found = TRUE;

        if (MY_DECK->memory[j][i].running) {
          if (running != FALSE) {
            running = TRUE;
            old_p = i;
            old_m = j;
          }
        } else if (MY_DECK->memory[j][i].lock) {
          if (locked != FALSE) {
            locked = TRUE;
            old_p = i;
            old_m = j;
          }
        } else if (MY_DECK->memory[j][i].packed) {
          if (packed != FALSE) {
            packed = TRUE;
            old_p = i;
            old_m = j;
          }
        } else if (MY_DECK->memory[j][i].strength > strength) {
          strength = MY_DECK->memory[j][i].strength;
          m_index = j;
          p_index = i;
          locked = FALSE;
          packed = FALSE;
          running = FALSE;
        }
      }
    }
  }

  if (!found) {
    prfmsg(MSG685);
    Memory_Services();
    EXIT0;
  }

  if (running == TRUE) {
    prfmsg(MSG80, MY_DECK->memory[old_m][old_p].name);
    Memory_Services();
    EXIT0;
  }

  if (locked == TRUE) {
    prfmsg(MSG82, MY_DECK->memory[old_m][old_p].name);
    Memory_Services();
    EXIT0;
  }

  if (packed == TRUE) {
    prfmsg(MSG83, MY_DECK->memory[old_m][old_p].name);
    Memory_Services();
    EXIT0;
  }

  MY_PORT->p_index     = p_index;
  MY_PORT->m_index     = m_index;
  MY_PORT->p_num       = UNLOCKER;
  MY_PORT->unlocking   = TRUE;
  File_Utility(0);
  EXIT0;
}
/**************************************************************************
	try to raise the strength of a program
*/
short Improve_Program(short option) {
  short   m = MY_PORT->m_index;
  short   p = MY_PORT->p_index;
  short   lev = His_Rank(me.score, me.won);
  short   strength = MY_DECK->memory[m][p].strength;
  short   roll;
  lev = lev / 2;

  if (lev == 0) {
    lev = 1;
  }

  STATE = MAIN_PROMPT;

  if (MY_DECK->memory[m][p].type != PROGRAM) {
    EXIT0;
  }

  if (MY_DECK->memory[m][p].running) {
    prfmsg(MSG135, MY_DECK->memory[m][p].name);
    EXIT0;
  }

  if (MY_DECK->memory[m][p].lock) {
    prfmsg(MSG82, MY_DECK->memory[m][p].name);
    EXIT0;
  }

  if (MY_DECK->memory[m][p].packed) {
    prfmsg(MSG83, MY_DECK->memory[m][p].name);
    EXIT0;
  }

  if (MY_DECK->memory[m][p].value == 0) {
    prfmsg(MSG681);
    EXIT0;
  }

  if (MY_DECK->memory[m][p].strength > lev) {
    prfmsg
    (
      MSG494,
      MY_DECK->memory[m][p].strength,
      MY_DECK->memory[m][p].name
    );
    EXIT0;
  }

  if (option == 0) {
    sprintf(str, getmsg(MSG495), MY_DECK->memory[m][p].name);
    Prompt(str, IMPROVE_PROMPT2);
    EXIT0;
  }

  roll = RANDOM(lev);

  if
  (
    roll < MY_DECK->memory[m][p].strength ||
    MY_DECK->memory[m][p].value == 0
  ) {
    prfmsg(MSG496);

    if (RANDOM(2) == 1 || MY_DECK->memory[m][p].strength <= 1) {
      prfmsg(MSG497, MY_DECK->memory[m][p].name);
      Remove_From_Memory(myp, m, p);
      Scored(myp, -(strength * 2), 0);
    } else {
      --MY_DECK->memory[m][p].strength;
      MY_DECK->memory[m][p].value -= 10;

      if (MY_DECK->memory[m][p].value < 0) {
        MY_DECK->memory[m][p].value = 0;
      }

      prfmsg
      (
        MSG498,
        MY_DECK->memory[m][p].strength,
        MY_DECK->memory[m][p].name
      );
      Scored(myp, -(strength), 0);
    }
  } else {
    prfmsg(MSG499);
    prfmsg
    (
      MSG498,
      MY_DECK->memory[m][p].strength + 1,
      MY_DECK->memory[m][p].name
    );
    Scored(myp, MY_DECK->memory[m][p].strength, 0);
    ++MY_DECK->memory[m][p].strength;
    MY_DECK->memory[m][p].value += 10;
  }

  EXIT0;
}
/****************************************************************************
	list all the hydra-installable programs I have
*/
short Installable_Programs(short option) {
  short   m, p;
  short   index = 0;
  short   l = DISP_Y;
  char    lock_str[10];
  char    pack_str[10];
  char    run_str[10];

  if (option == 0) {
    MY_PORT->m_index1 = 0;
    MY_PORT->p_index1 = 0;
  } else {
    if (margc == 0) {
      Clear_Display_Window();

      if (MY_PORT->m_index1 == -1) {
        STATE = MAIN_PROMPT;
        EXIT0;
      }
    } else if (margv[0][0] >= '0' && margv[0][0] <= '9') {
      p = INUM(margv[0]);

      if (p < 1 || p > MY_PORT->progs) {
        STATE = MAIN_PROMPT;
      } else {
        --p;
        MY_PORT->m_index1 = MY_PORT->proglist[p].m_index;
        MY_PORT->p_index1 = MY_PORT->proglist[p].p_index;
        Hydra_Select();
      }

      EXIT0;
    } else {
      if (MY_PORT->m_index1 == -1) {
        STATE = MAIN_PROMPT;
        EXIT0;
      }

      Clear_Display_Window();
    }
  }

  for (m = MY_PORT->m_index1; m < MY_DECK->mus; ++m) {
    if (m != MY_PORT->m_index1) {
      MY_PORT->p_index1 = 0;
    }

    for (p = MY_PORT->p_index1; p < MU_CAPACITY; ++p) {
      if
      (
        MY_DECK->memory[m][p].type == PROGRAM &&
        (MY_DECK->memory[m][p].index == CLOAK ||
         MY_DECK->memory[m][p].index == RADAR ||
         MY_DECK->memory[m][p].index == MUFFLER ||
         MY_DECK->memory[m][p].index == SHIELD ||
         MY_DECK->memory[m][p].index == ARMOR ||
         MY_DECK->memory[m][p].index == STATIK ||
         MY_DECK->memory[m][p].index == CHAINSAW ||
         MY_DECK->memory[m][p].index == SKULLDRILL ||
         MY_DECK->memory[m][p].index == TENTACLE ||
         MY_DECK->memory[m][p].index == CATTLEPROD ||
         MY_DECK->memory[m][p].index == GLUEGUN ||
         MY_DECK->memory[m][p].index == PHASER ||
         MY_DECK->memory[m][p].index == COBRA)
      ) {
        if (l == ASYNCH_Y) {
          MY_PORT->m_index1 = m;
          MY_PORT->p_index1 = p;
          MY_PORT->progs = index;
          sprintf(str, getmsg(MSG500), index);
          Prompt(str, INSTALL_PROMPT);
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
        prf(NL);

        if (index >= MAX_PROGLIST) {
          catastro("LoC: MAX_PROGLIST has been exceeded!");
        }

        MY_PORT->proglist[index].m_index = m;
        MY_PORT->proglist[index].p_index = p;
        ++index;
        ++l;
      }
    }
  }

  if (l == DISP_Y && MY_PORT->m_index1 == 0 && MY_PORT->p_index1 == 0) {
    prfmsg(MSG501);
  } else {
    MY_PORT->m_index1 = -1;
    MY_PORT->p_index1 = -1;
    MY_PORT->progs = index;
    sprintf(str, getmsg(MSG502), index);
    Prompt(str, INSTALL_PROMPT);
    EXIT0;
  }

  STATE = MAIN_PROMPT;
  EXIT0;
}
/*****************************************************************************
	change configuration on hydra
*/
short Configure_Program(void) {
  short   m_index = MY_PORT->m_index;
  short   p_index = MY_PORT->p_index;
  short   i;
  short   subs;
  short   max_subs;

  if (MY_DECK->memory[m_index][p_index].class != HYDRA) {
    prfmsg(MSG503);
    STATE = MAIN_PROMPT;
    EXIT0;
  }

  for (i = subs = 0; i != MAX_SUBS; ++i) {
    if (MY_DECK->memory[m_index][p_index].sub[i]) {
      ++subs;
    }
  }

  if (MY_DECK->memory[m_index][p_index].index == DEVIL) {
    max_subs = DEVIL_SUBS;
  } else if (MY_DECK->memory[m_index][p_index].index == FIEND) {
    max_subs = FIEND_SUBS;
  } else {
    max_subs = SATAN_SUBS;
  }

  if (subs >= max_subs) {
    prfmsg(MSG505, MY_DECK->memory[m_index][p_index].name);
    STATE = MAIN_PROMPT;
    EXIT0;
  }

  Installable_Programs(0);
  EXIT0;
}
/*****************************************************************************
	install a program into a hydra
*/
short Hydra_Select(void) {
  short   used;
  short   i;
  short   m_index = MY_PORT->m_index;
  short   p_index = MY_PORT->p_index;
  short   m = MY_PORT->m_index1;
  short   p = MY_PORT->p_index1;
  short   p_num = MY_DECK->memory[m][p].index;
  short   max_subs;
  Clear_Display_Window();
  STATE = MAIN_PROMPT;

  if
  (
    MY_DECK->memory[MY_PORT->m_index][MY_PORT->p_index].type != PROGRAM ||
    MY_DECK->memory[MY_PORT->m_index][MY_PORT->p_index].class != HYDRA
  ) {
    prfmsg(MSG507);
    EXIT0;
  }

  if (MY_DECK->memory[m][p].running) {
    prfmsg(MSG135, MY_DECK->memory[m][p].name);
    EXIT0;
  }

  if (MY_DECK->memory[m][p].lock) {
    prfmsg(MSG82, MY_DECK->memory[m][p].name);
    EXIT0;
  }

  if (MY_DECK->memory[m][p].packed) {
    prfmsg(MSG83, MY_DECK->memory[m][p].name);
    EXIT0;
  }

  if
  (
    p_num != CLOAK &&
    p_num != RADAR &&
    p_num != MUFFLER &&
    p_num != SHIELD &&
    p_num != ARMOR &&
    p_num != STATIK &&
    p_num != CHAINSAW &&
    p_num != SKULLDRILL &&
    p_num != TENTACLE &&
    p_num != CATTLEPROD &&
    p_num != GLUEGUN &&
    p_num != PHASER &&
    p_num != COBRA
  ) {
    prfmsg(MSG509, MY_DECK->memory[m][p].name);
    EXIT0;
  }

  if (m_index != m) {
    used = Memory_Usage(MY_DECK->memory[m_index]) + 1;

    if (used > MU_CAPACITY) {
      prfmsg
      (
        MSG510,
        MY_DECK->memory[m][p].name,
        MY_DECK->memory[m_index][p_index].name
      );
      EXIT0;
    }
  }

  if (MY_DECK->memory[m_index][p_index].index == DEVIL) {
    max_subs = DEVIL_SUBS;
  } else if (MY_DECK->memory[m_index][p_index].index == FIEND) {
    max_subs = FIEND_SUBS;
  } else {
    max_subs = SATAN_SUBS;
  }

  for (i = 0; i != max_subs; ++i) {
    if
    (
      MY_DECK->memory[m_index][p_index].sub[i] == p_num
    ) {
      prfmsg
      (
        MSG511,
        MY_DECK->memory[m_index][p_index].name,
        MY_DECK->memory[m][p].name
      );
      EXIT0;
    }

    if (MY_DECK->memory[m_index][p_index].sub[i] == 0) {
      MY_DECK->memory[m_index][p_index].sub[i] = p_num;
      ++MY_DECK->memory[m_index][p_index].size;
      prfmsg
      (
        MSG512,
        MY_DECK->memory[m_index][p_index].name,
        MY_DECK->memory[m][p].name
      );
      Remove_From_Memory(myp, m, p);
      EXIT0;
    }
  }

  prfmsg(MSG507);
  EXIT0;
}
/*****************************************************************************
	count up the memory used in a memory cell
*/
short Memory_Usage(struct memory_struct *memory) {
  short   i;
  short   used = 0;

  for (i = 0; i != MU_CAPACITY; ++i) {
    used += File_Size(&memory[i]);
  }

  return (used);
}
/*****************************************************************************
	count up the files in a memory cell
*/
short Num_Files(struct memory_struct *memory) {
  short   i;
  short   files = 0;

  for (i = 0; i != MU_CAPACITY; ++i) {
    if (memory[i].type) {
      ++files;
    }
  }

  return (files);
}

/**************************************************************************
	return the size of a file
*/
short File_Size(struct memory_struct *file) {
  short   size = 0;

  if (file->type == PROGRAM || file->type == DOCUMENT) {
    size = file->size;
  } else if (file->type == 0) {
    return (size);
  } else {
    size = 1;
  }

  if (file->packed) {
    size = size / 2;

    if (size == 0) {
      size = 1;
    }
  }

  return (size);
}
/**************************************************************************
	display the contents of some memory
*/
short EXPORT Memory_Contents(struct memory_struct *memory, short option) {
  short   i;
  short   s;
  short   total = 0;
  char    lock_str[10];
  char    pack_str[10];
  char    run_str[10];
  short   used = Memory_Usage(memory);

  if (option) {
    Clear_Display_Window();
    prf(glob->m513, MU_CAPACITY - used);
  }

  for (i = 0; i != MU_CAPACITY; ++i) {
    if (memory[i].type) {
      if (total == 0 && option) {
        prf(NL);
      }

      ++total;
    }

    strcpy(lock_str, " ");
    strcpy(pack_str, " ");
    strcpy(run_str, " ");

    if (memory[i].lock) {
      strcpy(lock_str, "L");
    }

    if (memory[i].packed) {
      strcpy(pack_str, "P");
    }

    if (memory[i].type == DOCUMENT) {
      if
      (
        !me.perm[GAMEOP_PERM] &&
        (memory[i].packed || memory[i].lock)
      ) {
        prf
        (
          glob->m514,
          i + 1,
          lock_str,
          pack_str,
          run_str,
          File_Size(&memory[i]),
          memory[i].name
        );
      } else {
        prf
        (
          glob->m515,
          i + 1,
          lock_str,
          pack_str,
          run_str,
          memory[i].value,
          File_Size(&memory[i]),
          memory[i].name,
          document_class[memory[i].class]
        );
      }
    } else if (memory[i].type == PROGRAM) {
      if (memory[i].running) {
        strcpy(run_str, "R");
      }

      if (!me.perm[GAMEOP_PERM] && memory[i].lock) {
        prf
        (
          glob->m516A,
          i + 1,
          lock_str,
          pack_str,
          run_str,
          File_Size(&memory[i]),
          memory[i].name,
          program_class[memory[i].class]
        );
      } else {
        prf
        (
          glob->m516B,
          i + 1,
          lock_str,
          pack_str,
          run_str,
          memory[i].value,
          memory[i].strength,
          File_Size(&memory[i]),
          memory[i].name,
          program_class[memory[i].class]
        );
      }

      prf(NL);

      if (memory[i].class == HYDRA) {
        for (s = 0; s != MAX_SUBS; ++s) {
          if (memory[i].sub[s]) {
            prf
            (
              glob->m518,
              Programs[memory[i].sub[s]].Name,
              program_class[Programs[memory[i].sub[s]].class]
            );
          }
        }
      }
    }
  }

  if (total == 0 && option) {
    prf(glob->m519);
  }

  return (total);
}
/**************************************************************************
	user wants to relocate something to a different memory unit
*/
short Memory_Move(void) {
  short   source_s = MY_PORT->source_s;
  short   source_m = MY_PORT->source_m;
  short   dest_s = MY_PORT->dest_s;
  short   dest_m = MY_PORT->dest_m;
  short   i;
  short   ok = TRUE;
  short   s;
  short   g;
  short   value;
  short   need_nl = FALSE;
  short   f = MY_PORT->m_index;
  short   sz;
  short   total;
  short   avail;
  short   go_cashier_lock = FALSE;
  s = source_s;

  if (s < max_guys && s != myp) {
    if
    (
      His_Rank(me.score, me.won) < glob->pvprank ||
      His_Rank(Port[s]->guy.score, Port[s]->guy.won) < glob->pvprank
    ) {
      prfmsg(MSG825);
      EXIT0;
    }

    if
    (
      glob->outlaw &&
      (!me.outlaw || !Port[s]->guy.outlaw)
    ) {
      prfmsg(MSG825);
      EXIT0;
    }
  }

  if (STATE == MEMORY_PROMPT2) {
    total = Memory_Contents(Sys[source_s]->memory[source_m], 1);
    sprintf(str, getmsg(MSG520), total);
    Prompt(str, MOVE_PROMPT);
    EXIT0;
  } else if (STATE == MOVE_PROMPT) {
    total = Num_Files(Sys[source_s]->memory[source_m]);
    f = INUM(margv[0]);

    if (f == 0 || f > total) {
      prfmsg(MSG134, total);
      EXIT0;
    }

    --f;

    if (Sys[source_s]->memory[source_m][f].lock) {
      prfmsg(MSG82, Sys[source_s]->memory[source_m][f].name);
      EXIT0;
    }

    if (Sys[source_s]->memory[source_m][f].running) {
      prfmsg(MSG135, Sys[source_s]->memory[source_m][f].name);
      EXIT0;
    }

    if
    (
      Sys[source_s]->memory[source_m][f].type == DOCUMENT &&
      Sys[source_s]->memory[source_m][f].class == ENDGAME_DOC
    ) {
      prfmsg(MSG718, Sys[source_s]->memory[source_m][f].name);
      EXIT0;
    }

    MY_PORT->m_index = f;
    Memory_Summary();
    sprintf(str, getmsg(MSG521), glob->dirs);
    Prompt(str, MOVE_PROMPT2);
    EXIT0;
  }

  if (Sys[source_s]->memory[source_m][f].lock) {
    prfmsg(MSG82, Sys[source_s]->memory[source_m][f].name);
    EXIT0;
  }

  if (Sys[source_s]->memory[source_m][f].running) {
    prfmsg(MSG135, Sys[source_s]->memory[source_m][f].name);
    EXIT0;
  }

  avail = MU_CAPACITY - Memory_Usage(Sys[dest_s]->memory[dest_m]);
  sz = File_Size(&Sys[source_s]->memory[source_m][f]);

  if (avail < sz) {
    prfmsg(MSG522);
    EXIT0;
  }

  value = Sys[source_s]->memory[source_m][f].value;

  if (source_s != myp && dest_s == myp) {
    if (Sys[source_s]->type == FILE_SERVER && Proc[source_s]->file_server) {
      --me.uploads;
    }
  } else if (source_s == myp && dest_s != myp) {
    if (Sys[dest_s]->type == FILE_SERVER && Proc[dest_s]->file_server) {
      ++me.uploads;
    }
  }

  if
  (
    dest_s == myp &&
    source_s != myp &&
    System_Is_Running(MY_PORT->sys_num, CASHIER)
  ) {
    i = Find_Bank_User(a_human, MY_PORT->code);

    if (i < 0) {
      ok = FALSE;
    } else {
      if (a_human->bank.balance < value) {
        value = (short) a_human->bank.balance;
        ok = FALSE;
      }

      Add_Bank_Record
      (
        a_human,
        -((long) value),
        MY_PORT->net_x, MY_PORT->net_y,
        me.alias,
        MY_SYS->Name
      );
      MY_PORT->balance -= value;
      go_cashier_lock = TRUE;

      if (me.gang && sameto(A_GANG, a_human->alias)) {
        g = atoi(&a_human->alias[5]);

        if (g == me.gang) {
          me.gang_bank -= value;
        }
      }

      prfmsg(MSG523, value, MY_PORT->code);
      need_nl = TRUE;

      if (MY_PORT->sys_num < max_guys) {
        Add_Bank_Record
        (
          &Port[MY_PORT->sys_num]->guy,
          (long) value,
          MY_PORT->net_x, MY_PORT->net_y,
          me.alias,
          MY_SYS->Name
        );
        sprintf
        (
          str,
          getmsg(MSG524),
          me.alias,
          Sys[source_s]->memory[source_m][f].name,
          value
        );
        Send(str, MY_PORT->sys_num, FALSE);
      }
    }
  } else if
  (
    dest_s != myp &&
    source_s == myp &&
    System_Is_Running(MY_PORT->sys_num, CASHIER)
  ) {
    if (Sys[source_s]->memory[source_m][f].type == DOCUMENT) {
      prfmsg(MSG925);
      EXIT0;
    }

    if (Sys[source_s]->memory[source_m][f].vended) {
      prfmsg(MSG918);
      EXIT0;
    }

    i = Find_Bank_User(a_human, MY_PORT->code);

    if (i < 0) {
      ok = FALSE;
    } else {
      value = value / 2;

      if (MY_PORT->sys_num < max_guys) {
        if (Port[MY_PORT->sys_num]->guy.bank.balance < value) {
          value = (short) Port[MY_PORT->sys_num]->guy.bank.balance;
        }
      }

      Add_Bank_Record
      (
        a_human,
        (long) value,
        MY_PORT->net_x, MY_PORT->net_y,
        me.alias,
        MY_SYS->Name
      );
      MY_PORT->balance += value;
      go_cashier_lock = TRUE;

      if (me.gang && sameto(A_GANG, a_human->alias)) {
        g = atoi(&a_human->alias[5]);

        if (g == me.gang) {
          me.gang_bank += (long) value;
        }
      }

      prfmsg
      (
        MSG525,
        value,
        MY_PORT->code
      );
      need_nl = TRUE;

      if (MY_PORT->sys_num < max_guys) {
        Add_Bank_Record
        (
          &Port[MY_PORT->sys_num]->guy,
          -((long) value),
          MY_PORT->net_x, MY_PORT->net_y,
          me.alias,
          MY_SYS->Name
        );
        sprintf
        (
          str,
          getmsg(MSG526),
          me.alias,
          Sys[source_s]->memory[source_m][f].name,
          value
        );
        Send(str, MY_PORT->sys_num, FALSE);
      }
    }
  }

  if
  (
    MY_PORT->access_level == SYSTEM_VIEW &&
    MY_SYS->type == VENDOR &&
    source_s != myp
  ) {
    Sys[source_s]->memory[source_m][f].vended = TRUE;
  }

  if (Sys[source_s]->memory[source_m][f].type == DOCUMENT) {
    Sys[source_s]->memory[source_m][f].vended = TRUE;
  }

  if (!ok) {
    prfmsg(MSG527);
    MY_PORT->code[0] = 0;
    me.last_net_crime = time(0);
    me.wanted = TRUE;
  } else if (need_nl) {
    prf(NL);
  }

  Add_To_Memory
  (
    Sys[dest_s]->memory[dest_m],
    &Sys[source_s]->memory[source_m][f]
  );
  Remove_From_Memory(source_s, source_m, f);

  if (go_cashier_lock) {
    Cashier_Lock(MY_PORT->sys_num);
  }

  EXIT0;
}
/**************************************************************************
	user wants to move something from memory to backup
*/
short Memory_Store(short option) {
  short   f = MY_PORT->dest_s;
  short   sz;
  short   total;
  short   avail;
  short   dest = MY_PORT->dest_m;
  short   source_s = MY_PORT->source_s;
  short   source_m = MY_PORT->source_m;

  if (option == MEMORY_PROMPT2) {
    total = Memory_Contents(Sys[source_s]->memory[source_m], 1);
    sprintf(str, getmsg(MSG173), total);
    Prompt(str, STORE_PROMPT2);
    EXIT0;
  } else if (option == STORE_PROMPT2) {
    total = Num_Files(Sys[source_s]->memory[source_m]);
    f = INUM(margv[0]);

    if (f == 0 || f > total) {
      prfmsg(MSG134, total);
      EXIT0;
    }

    --f;
    MY_PORT->dest_s = f;

    if (Sys[source_s]->memory[source_m][f].lock) {
      prfmsg(MSG82, Sys[source_s]->memory[source_m][f].name);
      EXIT0;
    }

    if (Sys[source_s]->memory[source_m][f].running) {
      prfmsg(MSG135, Sys[source_s]->memory[source_m][f].name);
      EXIT0;
    }

    return (TRUE);
  }

  avail = MU_CAPACITY - Memory_Usage(me.backup[dest]);
  sz = File_Size(&Sys[source_s]->memory[source_m][f]);

  if (avail < sz) {
    prfmsg(MSG535);
    EXIT0;
  }

  Add_To_Memory(me.backup[dest], &Sys[source_s]->memory[source_m][f]);
  Remove_From_Memory(source_s, source_m, f);
  EXIT0;
}
/**************************************************************************
	user wants to move something from backup to memory
*/
short Memory_Retrieve(short option) {
  short   f = MY_PORT->source_s;
  short   sz;
  short   total;
  short   avail;
  short   source_m = MY_PORT->source_m;
  short   dest_s = MY_PORT->dest_s;
  short   dest_m = MY_PORT->dest_m;

  if (option == RETRIEVE_PROMPT2) {
    total = Num_Files(me.backup[source_m]);
    f = INUM(margv[0]);

    if (f == 0 || f > total) {
      prfmsg(MSG134, total);
      EXIT0;
    }

    --f;
    MY_PORT->source_s = f;
    return (TRUE);
  }

  avail = MU_CAPACITY - Memory_Usage(Sys[dest_s]->memory[dest_m]);
  sz = File_Size(&me.backup[source_m][f]);

  if (avail < sz) {
    prfmsg(MSG536);
    EXIT0;
  }

  Add_To_Memory(Sys[dest_s]->memory[dest_m], &me.backup[source_m][f]);
  Remove_From_Backup(source_m, f);
  EXIT0;
}
/**************************************************************************
	add something to an MU
*/
short Add_To_Memory(struct memory_struct *memory, struct memory_struct *file) {
  short   i;

  for (i = 0; i != MU_CAPACITY; ++i) {
    if (!memory[i].type) {
      memcpy(&memory[i], file, MEMORY_ELEMENT_SIZE);
      EXIT0;
    }
  }

  prfmsg(MSG537);
  EXIT0;
}
/**************************************************************************
	remove everything from an MU
*/
void Erase_Memory(short s, short m) {
  short   i;

  while (Sys[s]->memory[m][0].type) {
    if (Sys[s]->memory[m][0].running) {
      for (i = 0; i != Sys[s]->cpus; ++i) {
        if
        (
          SYS_PROGRAM.m_index == m &&
          SYS_PROGRAM.p_index == 0
        ) {
          Program_Stops(s, i);
        }
      }
    }

    Remove_From_Memory(s, m, 0);
  }
}
/**************************************************************************
	remove something from an MU
*/
short Remove_From_Memory(short s, short m, short index) {
  short   i;
  short   g;
  short   p;
  Sys[s]->memory[m][index].type = 0;

  if (index != MU_CAPACITY - 1) {
    for (i = index + 1; i != MU_CAPACITY; ++i) {
      Sys[s]->memory[m][i - 1] = Sys[s]->memory[m][i];
      Sys[s]->memory[m][i].type = 0;
    }
  }

  for (i = 0; i != Sys[s]->cpus; ++i) {
    if (SYS_PROGRAM.m_index == m && SYS_PROGRAM.p_index > index) {
      --SYS_PROGRAM.p_index;
    }
  }

  for (g = 0; g != max_systems; ++g) {
    if (Sys[g]->in_use) {
      for (p = 0; p != Sys[g]->cpus; ++p) {
        if
        (
          GUY_PROGRAM.m_index >= 0 &&
          GUY_PROGRAM.net_x == Sys[s]->net_x &&
          GUY_PROGRAM.net_y == Sys[s]->net_y &&
          GUY_PROGRAM.city_x == Sys[s]->city_x &&
          GUY_PROGRAM.city_y == Sys[s]->city_y &&
          (
            GUY_SOURCE.index == UNLOCKER ||
            GUY_SOURCE.index == LOCKER
          ) &&
          GUY_PROGRAM.target_m == m &&
          GUY_PROGRAM.target_index > index
        ) {
          --GUY_PROGRAM.target_index;
        }
      }
    }
  }

  EXIT0;
}
/**************************************************************************
	remove something from a backup chip
*/
short Remove_From_Backup(short m, short index) {
  short   i;
  me.backup[m][index].type = 0;

  if (index == MU_CAPACITY - 1) {
    EXIT0;
  }

  for (i = index + 1; i != MU_CAPACITY; ++i) {
    me.backup[m][i - 1] = me.backup[m][i];
    me.backup[m][i].type = 0;
  }

  EXIT0;
}
/**************************************************************************
	summarize the deck's memory usage
*/
short Memory_Summary(void) {
  short   col = 0;
  short   m;
  short   i;
  short   programs;
  short   docs;
  short   total = 0;
  short   used;
  short   d;
  short   x, y;
  short   is_memory;
  char    dir;
  sprintf(glob->dirs, "1-%d", MY_DECK->mus);

  for (m = 0; m != MY_DECK->mus; ++m) {
    ++total;
    docs = 0;
    programs = 0;
    used = Memory_Usage(MY_DECK->memory[m]);

    for (i = 0; i != MU_CAPACITY; ++i) {
      if (MY_DECK->memory[m][i].type == PROGRAM) {
        ++programs;
      } else if (MY_DECK->memory[m][i].type == DOCUMENT) {
        ++docs;
      }
    }

    prf(glob->m538, m + 1, MU_CAPACITY - used, programs, docs);
    ++col;

    if (col == 2) {
      prf(NL);
      col = 0;
    }
  }

  if (col) {
    prf(NL);
    col = 0;
  }

  if (MY_PORT->access_level == SYSTEM_VIEW) {
    for (d = 0; d != 4; ++d) {
      x = MY_PORT->sys_x;
      y = MY_PORT->sys_y;

      if (d == 0) {
        dir = 'N';
        --y;
      } else if (d == 1) {
        dir = 'S';
        ++y;
      } else if (d == 2) {
        dir = 'E';
        ++x;
      } else {
        dir = 'W';
        --x;
      }

      is_memory = TRUE;

      if (y < 0 || y >= MAX_SYS_Y || x >= MAX_SYS_X || x < 0) {
        is_memory = FALSE;
      } else if (MY_SYS->node_type[x][y] != MEMORY) {
        is_memory = FALSE;
      }

      if (is_memory) {
        sprintf(str, ",%c", dir);
        strcat(glob->dirs, str);
        m = MY_SYS->node_index[x][y];
        ++total;
        docs = 0;
        programs = 0;
        used = Memory_Usage(MY_SYS->memory[m]);

        for (i = 0; i != MU_CAPACITY; ++i) {
          if (MY_SYS->memory[m][i].type == PROGRAM) {
            ++programs;
          } else if (MY_SYS->memory[m][i].type == DOCUMENT) {
            ++docs;
          }
        }

        prf
        (
          glob->m540,
          dir,
          MU_CAPACITY - used,
          programs,
          docs
        );
        ++col;

        if (col == 2) {
          prf(NL);
          col = 0;
        }
      }
    }
  }

  if (col) {
    prf(NL);
  }

  return (total);
}
/**************************************************************************
	summarize the permanant system's memory usage
*/
short Perm_Memory(struct system_struct *sys) {
  short   col = 0;
  short   m;
  short   i;
  short   programs;
  short   docs;
  short   total = 0;
  short   used;
  Clear_Display_Window();
  sprintf(glob->dirs, "A-%c", sys->mus + 'A' - 1);

  for (m = 0; m != sys->mus; ++m) {
    ++total;
    docs = 0;
    programs = 0;
    used = Memory_Usage(sys->memory[m]);

    for (i = 0; i != MU_CAPACITY; ++i) {
      if (sys->memory[m][i].type == PROGRAM) {
        ++programs;
      } else if (sys->memory[m][i].type == DOCUMENT) {
        ++docs;
      }
    }

    prf
    (
      glob->m540,
      m + 'A',
      MU_CAPACITY - used,
      programs,
      docs
    );
    ++col;

    if (col == 2) {
      prf(NL);
      col = 0;
    }
  }

  if (col) {
    prf(NL);
  }

  return (total);
}
/**************************************************************************
	summarize the deck's backup usage
*/
short Backup_Summary(void) {
  short   m;
  short   i;
  short   programs;
  short   docs;
  short   used;

  for (m = 0; m != me.backup_chips; ++m) {
    docs = 0;
    programs = 0;
    used = Memory_Usage(me.backup[m]);

    for (i = 0; i != MU_CAPACITY; ++i) {
      if (me.backup[m][i].type == PROGRAM) {
        ++programs;
      } else if (me.backup[m][i].type == DOCUMENT) {
        ++docs;
      }
    }

    prfmsg
    (
      MSG544,
      m + 1,
      MU_CAPACITY - used,
      programs,
      docs
    );
  }

  EXIT0;
}
/**************************************************************************
	find the specified memory
*/
short Find_Memory(char *where, short *s, short *m) {
  short   x, y;
  short   dir = 0;
  short   num = 0;
  short   is_memory = TRUE;

  if (sameto("N", where)) {
    dir = NORTH;
  } else if (sameto("S", where)) {
    dir = SOUTH;
  } else if (sameto("E", where)) {
    dir = EAST;
  } else if (sameto("W", where)) {
    dir = WEST;
  } else {
    num = INUM(where);

    if (num == 0) {
      Clear_Display_Window();
      prfmsg(MSG542);
      EXIT0;
    }
  }

  if (MY_PORT->access_level != SYSTEM_VIEW && dir) {
    Clear_Display_Window();
    prfmsg(MSG545);
    Print_Direction(dir);
    EXIT0;
  }

  if (dir) {
    x = MY_PORT->sys_x;
    y = MY_PORT->sys_y;

    if (dir == NORTH) {
      --y;
    } else if (dir == SOUTH) {
      ++y;
    } else if (dir == EAST) {
      ++x;
    } else if (dir == WEST) {
      --x;
    }

    if (y < 0 || y >= MAX_SYS_Y || x >= MAX_SYS_X || x < 0) {
      is_memory = FALSE;
    } else if (MY_SYS->node_type[x][y] != MEMORY) {
      is_memory = FALSE;
    }

    if (!is_memory) {
      Clear_Display_Window();
      prfmsg(MSG545);
      Print_Direction(dir);
      EXIT0;
    }

    *s = MY_PORT->sys_num;
    *m = MY_SYS->node_index[x][y];
    return (TRUE);
  }

  if (num > MY_DECK->mus) {
    Clear_Display_Window();
    prfmsg(MSG546, MY_DECK->mus);
    EXIT0;
  }

  --num;
  *s = myp;
  *m = num;
  return (TRUE);
}
/**************************************************************************
	find the specified backup chip
*/
short Find_Chip(char *where, short *m) {
  *m = INUM(where);

  if (*m == 0) {
    Clear_Display_Window();
    prfmsg(MSG542);
    EXIT0;
  }

  if (*m > me.backup_chips) {
    prfmsg(MSG548, me.backup_chips);
    EXIT0;
  }

  --*m;
  return (TRUE);
}
/**************************************************************************
	print a direction
*/
void Print_Direction(short dir) {
  if (dir == NORTH) {
    prfmsg(MSG549);
  } else if (dir == SOUTH) {
    prfmsg(MSG550);
  } else if (dir == EAST) {
    prfmsg(MSG551);
  } else if (dir == WEST) {
    prfmsg(MSG552);
  }
}
/**************************************************************************
	user wants to remove something from memory
*/
short Memory_Erase(void) {
  short   s = MY_PORT->source_s;
  short   m = MY_PORT->source_m;
  short   f;
  short   total = Num_Files(Sys[s]->memory[m]);

  if (s < max_guys && s != myp) {
    if
    (
      His_Rank(me.score, me.won) < glob->pvprank ||
      His_Rank(Port[s]->guy.score, Port[s]->guy.won) < glob->pvprank
    ) {
      prfmsg(MSG824);
      EXIT0;
    }

    if
    (
      glob->outlaw &&
      (!me.outlaw || !Port[s]->guy.outlaw)
    ) {
      prfmsg(MSG824);
      EXIT0;
    }
  }

  if (STATE == MEMORY_PROMPT2) {
    Memory_Contents(Sys[s]->memory[m], 1);
    sprintf(str, getmsg(MSG553), total);
    Prompt(str, ERASE_PROMPT);
    EXIT0;
  }

  f = INUM(margv[0]);

  if (f == 0 || f > total) {
    prfmsg(MSG134, total);
    EXIT0;
  }

  --f;

  if (Sys[s]->memory[m][f].lock) {
    prfmsg(MSG82, Sys[s]->memory[m][f].name);
    EXIT0;
  }

  if (Sys[s]->memory[m][f].running) {
    prfmsg(MSG135, Sys[s]->memory[m][f].name);
    EXIT0;
  }

  if (Sys[s]->memory[m][f].type == DOCUMENT) {
    if
    (
      me.mission_type == ERASE_MISSION &&
      me.mission_x == MY_PORT->net_x &&
      me.mission_y == MY_PORT->net_y &&
      MY_PORT->access_level == SYSTEM_VIEW &&
      sameas(me.mission_system, MY_SYS->Name) &&
      MY_SYS->type != CYBERDECK &&
      s != myp &&
      !Sys[s]->memory[m][f].vended
    ) {
      strcpy(str, getmsg(MSG729));
      Send(str, myp, TRUE);
      Scored(myp, me.mission_score * MISSION_SCORE, 1);
      Add_Bank_Record
      (
        &me,
        (long) (me.mission_score * MISSION_BUX),
        MY_PORT->net_x, MY_PORT->net_y,
        me.alias,
        "Mission"
      );
      me.mission_type = 0;
    } else if (Sys[s]->memory[m][f].class == ENDGAME_DOC && !me.won) {
      Endgame();
    } else {
      Scored(myp, Sys[s]->memory[m][f].class, 1);
    }

    prf("\x1B[%d;1H", DISP_Y);
  } else if (Sys[s]->memory[m][f].value) {
    Scored(myp, 1, 1);
    prf("\x1B[%d;1H", DISP_Y);
  }

  Remove_From_Memory(s, m, f);

  if (s != myp && Proc[s]->file_server == 0) {
    Proc[s]->enemy[myp] = TRUE;
  }

  if (s != myp && Sys[s]->type == FILE_SERVER && Proc[s]->file_server) {
    --me.uploads;
  }

  EXIT0;
}
/**************************************************************************
	user wants to zip/unzip something from memory
*/
short Memory_Zip(void) {
  short   s = MY_PORT->source_s;
  short   m = MY_PORT->source_m;
  short   f;
  short   used;
  short   total = Num_Files(Sys[s]->memory[m]);

  if (STATE == MEMORY_PROMPT2) {
    Memory_Contents(Sys[s]->memory[m], 1);
    sprintf(str, getmsg(MSG554), total);
    Prompt(str, ZIP_PROMPT);
    EXIT0;
  }

  f = INUM(margv[0]);

  if (f == 0 || f > total) {
    prfmsg(MSG134, total);
    EXIT0;
  }

  --f;

  if (Sys[s]->memory[m][f].lock) {
    prfmsg(MSG82, Sys[s]->memory[m][f].name);
    EXIT0;
  }

  if (Sys[s]->memory[m][f].running) {
    prfmsg(MSG135, Sys[s]->memory[m][f].name);
    EXIT0;
  }

  if (Sys[s]->memory[m][f].packed) {
    Sys[s]->memory[m][f].packed = FALSE;
    used = Memory_Usage(Sys[s]->memory[m]);

    if (used > MU_CAPACITY) {
      Sys[s]->memory[m][f].packed = TRUE;
      prfmsg(MSG651, Sys[s]->memory[m][f].name);
      EXIT0;
    }
  } else {
    Sys[s]->memory[m][f].packed = TRUE;
  }

  EXIT0;
}
/**************************************************************************
	user wants to examine some memory
*/
short EXPORT Memory_Read(void) {
  short   s = MY_PORT->source_s;
  short   m = MY_PORT->source_m;
  short   f;
  short   total;

  if (STATE == MEMORY_PROMPT2) {
    total = Memory_Contents(Sys[s]->memory[m], 1);
    sprintf(str, getmsg(MSG555), total);
    Prompt(str, READ_PROMPT);
    EXIT0;
  }

  total = Num_Files(Sys[s]->memory[m]);
  f = INUM(margv[0]);

  if (f == 0 || f > total) {
    prfmsg(MSG134, total);
    EXIT0;
  }

  --f;

  if (Sys[s]->memory[m][f].lock) {
    prfmsg(MSG82, Sys[s]->memory[m][f].name);
    EXIT0;
  }

  if (Sys[s]->memory[m][f].packed) {
    prfmsg(MSG83, Sys[s]->memory[m][f].name);
    EXIT0;
  }

  if (Sys[s]->memory[m][f].type == PROGRAM) {
    prfmsg(MSG556, Sys[s]->memory[m][f].name);
  } else if (Sys[s]->memory[m][f].type == DOCUMENT) {
    Display_Doc(&Sys[s]->memory[m][f]);
  } else {
    prfmsg(MSG557);
  }

  EXIT0;
}
