#include <header.h>
#include <globals.h>

/****************************************************************************
	god-like users may edit cyberspace
*/
short EXPORT Edit(void) {
  if (margc == 1) {
    prfmsg(M332);
    EXIT0;
  }

  if (sameto(margv[1], "world")) {
    if (!me.perm[EDIT_WORLD_PERM] || !glob->world_edit) {
      prfmsg(MSG274);
    } else if (!glob->world_edit) {
      prfmsg(MSG831);
    } else {
      Edit_World();
    }

    EXIT0;
  }

  if (sameto(margv[1], "player")) {
    if (!me.perm[EDIT_USER_PERM]) {
      prfmsg(MSG274);
    } else {
      Edit_Player();
    }

    EXIT0;
  }

  if (sameto(margv[1], "gang")) {
    if (!me.perm[EDIT_USER_PERM]) {
      prfmsg(MSG274);
    } else {
      Prompt(getmsg(MSG904), GANG_COUNT1);
    }

    EXIT0;
  }

  if (sameto(margv[1], "report")) {
    if (!me.perm[EDIT_USER_PERM]) {
      prfmsg(MSG274);
    } else {
      unlink(REPORT_FILE);
      MY_PORT->nx = 0;
      MY_PORT->ny = 0;
      MY_PORT->cx = 0;
      MY_PORT->cy = 0;
      MY_PORT->reporting = TRUE;
      btuinj(usrnum, CYCLE);
    }

    EXIT0;
  }

  prfmsg(M332);
  EXIT0;
}
/****************************************************************************
	user wants to re-design their deck configuration
*/
short EXPORT Design_Deck(void) {
  short   x, y;
  struct  edit_struct *e;
  e = (struct edit_struct *) vdaptr;

  if (STATE == MAIN_PROMPT) {
    Display_A_System(MY_DECK);
    prf("\x1B[1;45H");
    prfmsg(MSG333);
    prf("\x1B[2;45H");
    prfmsg(MSG334);
    prf("\x1B[3;45H");
    prfmsg(MSG335);
    prf("\x1B[4;45H");
    prfmsg(MSG336);
    prf("\x1B[5;45H");
    prfmsg(MSG337A);
    prf("\x1B[6;45H");
    prfmsg(MSG337B);
    prf("\x1B[7;45H");
    prfmsg(MSSG338);
    prf("\x1B[8;45H");
    prfmsg(MSG838);
    prf("\x1B[9;45H");
    prfmsg(MSG901);
    MY_PORT->prompt_y = 10;
    MY_PORT->prompt_x = 45;
    STATE = DESIGN_PROMPT;
  } else if (STATE == DESIGN_PROMPT) {
    e->option = INUM(margv[0]);

    if (e->option < 1 || e->option > 9) {
      EXIT0;
    }

    if (e->option == 7) {
      Prompt(getmsg(MSG339), SYS_NAME_PROMPT);
    } else if (e->option == 8) {
      Prompt(getmsg(M839), SYS_PW_PROMPT);
    } else if (e->option == 6) {
      sprintf
      (
        str,
        getmsg(MSG684),
        'A' + MY_DECK->mus - 1
      );
      Prompt(str, MOVE_MEM_PROMPT);
      MY_PORT->last_view = 0;
      Display_A_System(MY_DECK);
    } else if (e->option == 5) {
      sprintf
      (
        str,
        getmsg(MSG683),
        'A' + MY_DECK->cpus - 1
      );
      Prompt(str, MOVE_CPU_PROMPT);
      MY_PORT->last_view = 0;
      Display_A_System(MY_DECK);
    } else if (e->option == 9) {
      if (!me.phone) {
        prfmsg(MSG902);
      } else {
        sprintf(str, getmsg(MSG340), MAX_SYS_X - 1);
        Prompt(str, DESIGN_X1_PROMPT);
      }
    } else {
      sprintf(str, getmsg(MSG340), MAX_SYS_X - 1);
      Prompt(str, DESIGN_X1_PROMPT);
    }
  } else if (STATE == MOVE_MEM_PROMPT) {
    if (margc == 0 || sameas(margv[0], CANCEL)) {
      MY_PORT->last_view = 0;
      STATE = MAIN_PROMPT;
      Design_Deck();
      EXIT0;
    } else {
      e->m = toupper(margv[0][0]) - 'A' + 1;

      if (e->m < 1 || e->m > MY_DECK->mus) {
        MY_PORT->last_view = 0;
        STATE = MAIN_PROMPT;
        Design_Deck();
        EXIT0;
      } else {
        --e->m;

        for (x = 0; x != MAX_SYS_X; ++x) {
          for (y = 0; y != MAX_SYS_Y; ++y) {
            if
            (
              MY_DECK->node_type[x][y] == MEMORY &&
              MY_DECK->node_index[x][y] == e->m
            ) {
              e->x1 = x;
              e->y1 = y;
            }
          }
        }

        sprintf(str, getmsg(MSG349), MAX_SYS_X - 1);
        Prompt(str, DESIGN_X2_PROMPT);
      }
    }
  } else if (STATE == MOVE_CPU_PROMPT) {
    if (margc == 0 || sameas(margv[0], CANCEL)) {
      MY_PORT->last_view = 0;
      STATE = MAIN_PROMPT;
      Design_Deck();
      EXIT0;
    } else {
      e->m = toupper(margv[0][0]) - 'A' + 1;

      if (e->m < 1 || e->m > MY_DECK->cpus) {
        MY_PORT->last_view = 0;
        STATE = MAIN_PROMPT;
        Design_Deck();
        EXIT0;
      } else {
        --e->m;

        for (x = 0; x != MAX_SYS_X; ++x) {
          for (y = 0; y != MAX_SYS_Y; ++y) {
            if
            (
              MY_DECK->node_type[x][y] == CPU &&
              MY_DECK->node_index[x][y] == e->m
            ) {
              e->x1 = x;
              e->y1 = y;
            }
          }
        }

        sprintf(str, getmsg(MSG349), MAX_SYS_X - 1);
        Prompt(str, DESIGN_X2_PROMPT);
      }
    }
  } else if (STATE == SYS_NAME_PROMPT) {
    if (margc) {
      rstrin();
      strncpy(MY_DECK->Name, input, SYSTEM_NAME_SIZE);
      MY_DECK->Name[SYSTEM_NAME_SIZE - 1] = 0;
      prfmsg(MSG341, MY_DECK->Name);
    }

    STATE = DESIGN_PROMPT;
  } else if (STATE == SYS_PW_PROMPT) {
    if (margc) {
      rstrin();

      if (sameas(input, CANCEL)) {
        MY_DECK->password[0] = 0;
        prfmsg(MSG840, "<NOT SET>");
      } else {
        strncpy(MY_DECK->password, input, PASSWORD_SIZE);
        MY_DECK->password[PASSWORD_SIZE - 1] = 0;
        prfmsg(MSG840, MY_DECK->password);
      }
    }

    STATE = DESIGN_PROMPT;
  } else if (STATE == DESIGN_X1_PROMPT) {
    if (margc == 0) {
      STATE = DESIGN_PROMPT;
    } else {
      e->x1 = INUM(margv[0]);

      if (e->x1 < 0 || e->x1 >= MAX_SYS_X) {
        STATE = DESIGN_PROMPT;
      } else {
        sprintf(str, getmsg(MSG342), MAX_SYS_Y - 1);
        Prompt(str, DESIGN_Y1_PROMPT);
      }
    }
  } else if (STATE == DESIGN_Y1_PROMPT) {
    Clear_Display_Window();

    if (margc == 0) {
      STATE = DESIGN_PROMPT;
    } else {
      e->y1 = INUM(margv[0]);

      if (e->y1 < 0 || e->y1 >= MAX_SYS_Y) {
        STATE = DESIGN_PROMPT;
      } else {
        if (e->option == 1) {
          if (MY_DECK->node_type[e->x1][e->y1]) {
            prfmsg(MSG343, e->x1, e->y1);
          } else if (e->x1 == START_SYS_X && e->y1 == START_SYS_Y) {
            prfmsg(MSG344, e->x1, e->y1);
          } else if
          (
            e->x1 == 0 ||
            e->y1 == 0 ||
            e->x1 == MAX_SYS_X - 1 ||
            e->y1 == MAX_SYS_Y - 1
          ) {
            prfmsg(MSG344, e->x1, e->y1);
          } else {
            MY_DECK->node_type[e->x1][e->y1] = DATA_WALL;
            Display_A_System(MY_DECK);
          }

          STATE = DESIGN_PROMPT;
        } else if (e->option == 2) {
          if
          (
            MY_DECK->node_type[e->x1][e->y1] != DATA_WALL &&
            MY_DECK->node_type[e->x1][e->y1] != DEAD_WALL
          ) {
            prfmsg(MSG345, e->x1, e->y1);
          } else {
            MY_DECK->node_type[e->x1][e->y1] = 0;
            Display_A_System(MY_DECK);
          }

          STATE = DESIGN_PROMPT;
        } else if (e->option == 3) {
          if (MY_DECK->node_type[e->x1][e->y1]) {
            prfmsg(MSG343, e->x1, e->y1);
          } else if (e->x1 == START_SYS_X && e->y1 == START_SYS_Y) {
            prfmsg(MSG344, e->x1, e->y1);
          } else if
          (
            e->x1 == 0 ||
            e->y1 == 0 ||
            e->x1 == MAX_SYS_X - 1 ||
            e->y1 == MAX_SYS_Y - 1
          ) {
            prfmsg(MSG344, e->x1, e->y1);
          } else {
            MY_DECK->node_type[e->x1][e->y1] = CLOSED_GATE;
            Display_A_System(MY_DECK);
          }

          STATE = DESIGN_PROMPT;
        } else if (e->option == 4) {
          if
          (
            MY_DECK->node_type[e->x1][e->y1] != CLOSED_GATE &&
            MY_DECK->node_type[e->x1][e->y1] != OPENED_GATE
          ) {
            prfmsg(MSG347, e->x1, e->y1);
          } else {
            MY_DECK->node_type[e->x1][e->y1] = 0;
            Display_A_System(MY_DECK);
          }

          STATE = DESIGN_PROMPT;
        } else if (e->option == 9) {
          if (MY_DECK->node_type[e->x1][e->y1]) {
            prfmsg(MSG343, e->x1, e->y1);
          } else if (e->x1 == START_SYS_X && e->y1 == START_SYS_Y) {
            prfmsg(MSG344, e->x1, e->y1);
          } else {
            for (x = 0; x != MAX_SYS_X; ++x) {
              for (y = 0; y != MAX_SYS_Y; ++y) {
                if (MY_DECK->node_type[x][y] == PHONE) {
                  MY_DECK->node_type[x][y] = 0;
                }
              }
            }

            MY_DECK->node_type[e->x1][e->y1] = PHONE;
            Display_A_System(MY_DECK);
          }

          STATE = DESIGN_PROMPT;
        }
      }
    }
  } else if (STATE == DESIGN_X2_PROMPT) {
    if (margc == 0) {
      MY_PORT->last_view = 0;
      STATE = MAIN_PROMPT;
      Design_Deck();
      EXIT0;
    } else {
      e->x2 = INUM(margv[0]);

      if (e->x2 < 0 || e->x2 >= MAX_SYS_X) {
        MY_PORT->last_view = 0;
        STATE = MAIN_PROMPT;
        Design_Deck();
        EXIT0;
      } else {
        sprintf(str, getmsg(MSG350), MAX_SYS_Y - 1);
        Prompt(str, DESIGN_Y2_PROMPT);
      }
    }
  } else if (STATE == DESIGN_Y2_PROMPT) {
    if (margc) {
      Clear_Display_Window();
      e->y2 = INUM(margv[0]);

      if (e->y2 >= 0 && e->y2 < MAX_SYS_Y) {
        if (MY_DECK->node_type[e->x2][e->y2]) {
          prfmsg(MSG346, e->x2, e->y2);
        } else if (e->x2 == START_SYS_X && e->y2 == START_SYS_Y) {
          prfmsg(MSG344, e->x2, e->y2);
        } else if
        (
          e->x2 == 0 ||
          e->y2 == 0 ||
          e->x2 == MAX_SYS_X - 1 ||
          e->y2 == MAX_SYS_Y - 1
        ) {
          prfmsg(MSG344, e->x2, e->y2);
        } else {
          MY_DECK->node_type[e->x2][e->y2] = MY_DECK->node_type[e->x1][e->y1];
          MY_DECK->node_index[e->x2][e->y2] = MY_DECK->node_index[e->x1][e->y1];
          MY_DECK->node_type[e->x1][e->y1] = 0;
          MY_DECK->node_index[e->x1][e->y1] = 0;
        }
      }
    }

    MY_PORT->last_view = 0;
    STATE = MAIN_PROMPT;
    Design_Deck();
    EXIT0;
  }

  if (STATE == DESIGN_PROMPT) {
    Prompt(getmsg(MY351), DESIGN_PROMPT);
  }

  EXIT0;
}
/****************************************************************************
	edit a permanant system
*/
short EXPORT Design_Perm(void) {
  short   m;
  short   i;
  short   s;
  short   x, y;
  FILE   *fp;
  struct edit_struct *e;
  e = (struct edit_struct *) vdaptr;

  if (STATE == DESIGN_PERM) {
    Display_A_System(&e->sys);
    prf("\x1B[1;45H");
    prfmsg(MSG625);
    prf("\x1B[2;45H");
    prfmsg(MSG626);
    prf("\x1B[3;45H");
    prfmsg(MSG627);
    prf("\x1B[4;45H");
    prfmsg(MSG628);
    prf("\x1B[5;45H");
    prfmsg(MSG629);
    prf("\x1B[6;45H");
    prfmsg(MS630);
    prf("\x1B[7;45H");
    prfmsg(MSG853);
    MY_PORT->prompt_y = 8;
    MY_PORT->prompt_x = 45;
    STATE = PDESIGN_PROMPT;
  } else if (STATE == PDESIGN_PROMPT) {
    Clear_Display_Window();

    if (sameas(margv[0], CANCEL)) {
      if (glob->world_edit == 2) {
        Set_Perm_Path(e->x, e->y, e->net_x, e->net_y);
        fp = fopen(glob->path, READB);
        fwrite(&e->sys, SYSTEM_SIZE, 1, fp);
        fclose(fp);
        Set_System_Path(e->x, e->y, e->net_x, e->net_y);
        fp = fopen(glob->path, READB);
        fwrite(&e->sys, SYSTEM_SIZE, 1, fp);
        fclose(fp);
      } else {
        Set_Custom_Perm_Path(e->x, e->y, e->net_x, e->net_y);
        fp = fopen(glob->path, READB);
        fwrite(&e->sys, SYSTEM_SIZE, 1, fp);
        fclose(fp);
        Set_Custom_Path(e->x, e->y, e->net_x, e->net_y);
        fp = fopen(glob->path, READB);
        fwrite(&e->sys, SYSTEM_SIZE, 1, fp);
        fclose(fp);
      }

      STATE = -2;
      EXIT0;
    }

    e->option = INUM(margv[0]);

    if (e->option < 1 || e->option > 14) {
      EXIT0;
    }

    if (e->option == 1) {
      if (e->sys.cpus >= MAX_CPUS) {
        prfmsg(MSG633, MAX_CPUS);
        EXIT0;
      }

      sprintf(str, getmsg(MSG340), MAX_SYS_X - 1);
      Prompt(str, PDESIGN_X1_PROMPT);
    } else if (e->option == 2) {
      if (e->sys.cpus == 1) {
        prfmsg(MSG634);
        EXIT0;
      }

      sprintf(str, getmsg(MSG340), MAX_SYS_X - 1);
      Prompt(str, PDESIGN_X1_PROMPT);
    } else if (e->option == 3) {
      if (e->sys.mus >= MAX_MUS) {
        prfmsg(MSG636, MAX_MUS);
        EXIT0;
      }

      sprintf(str, getmsg(MSG340), MAX_SYS_X - 1);
      Prompt(str, PDESIGN_X1_PROMPT);
    } else if (e->option == 4) {
      if (e->sys.mus == 1) {
        prfmsg(MSG637);
        EXIT0;
      }

      sprintf(str, getmsg(MSG340), MAX_SYS_X - 1);
      Prompt(str, PDESIGN_X1_PROMPT);
    } else if
    (
      e->option == 5 ||
      e->option == 6 ||
      e->option == 7 ||
      e->option == 8 ||
      e->option == 9 ||
      e->option == 13 ||
      e->option == 14
    ) {
      sprintf(str, getmsg(MSG340), MAX_SYS_X - 1);
      Prompt(str, PDESIGN_X1_PROMPT);
    } else if (e->option == 10) {
      Perm_Memory(&e->sys);
      sprintf(str, getmsg(MSG640), glob->dirs);
      Prompt(str, PMEMORY_PROMPT);
      MY_PORT->last_view = 0;
      Display_A_System(&e->sys);
      EXIT0;
    } else if (e->option == 11) {
      Perm_FSE();
      EXIT0;
    } else if (e->option == 12) {
      e->sys.mus = 1;
      e->sys.cpus = 1;

      for (x = 0; x != MAX_SYS_X; ++x) {
        for (y = 0; y != MAX_SYS_Y; ++y) {
          if
          (
            e->sys.node_type[x][y] == DATA_WALL ||
            e->sys.node_type[x][y] == OPENED_GATE ||
            e->sys.node_type[x][y] == CLOSED_GATE
          ) {
            e->sys.node_type[x][y] = 0;
          } else if
          (
            e->sys.node_type[x][y] == MEMORY &&
            e->sys.node_index[x][y] >= 1
          ) {
            e->sys.node_type[x][y] = 0;
          } else if
          (
            e->sys.node_type[x][y] == CPU &&
            e->sys.node_index[x][y] >= 1
          ) {
            e->sys.node_type[x][y] = 0;
          }
        }
      }

      Display_A_System(&e->sys);
      EXIT0;
    }
  } else if (STATE == PMEMORY_PROMPT) {
    if (sameas(margv[0], CANCEL)) {
      STATE = DESIGN_PERM;
      MY_PORT->last_view = 0;
      Display_A_System(&e->sys);
      Design_Perm();
    } else {
      e->m = toupper(margv[0][0]) - 'A' + 1;

      if (e->m < 1 || e->m > e->sys.mus) {
        EXIT0;
      }

      --e->m;
      Memory_Contents(e->sys.memory[e->m], 1);
      Prompt(getmsg(MSG641), PMEMORY_PROMPT2);
    }

    EXIT0;
  } else if (STATE == PMEMORY_PROMPT2) {
    if (sameas(margv[0], CANCEL)) {
      Perm_Memory(&e->sys);
      sprintf(str, getmsg(MSG640), glob->dirs);
      Prompt(str, PMEMORY_PROMPT);
      MY_PORT->last_view = 0;
      Display_A_System(&e->sys);
      EXIT0;
    } else if (sameas(margv[0], "A")) {
      i = Memory_Usage(e->sys.memory[e->m]);

      if (i >= MU_CAPACITY) {
        EXIT0;
      }

      Prompt(getmsg(MSG646), PADD_PROMPT);
    } else if (sameas(margv[0], "E")) {
      e->files = Memory_Contents(e->sys.memory[e->m], 1);

      if (e->files == 0) {
        EXIT0;
      }

      sprintf(str, getmsg(MSG642), e->files);
      Prompt(str, PERASE_PROMPT);
    } else if (sameas(margv[0], "C")) {
      e->files = Memory_Contents(e->sys.memory[e->m], 1);

      if (e->files == 0) {
        EXIT0;
      }

      sprintf(str, getmsg(MSG643), e->files);
      Prompt(str, PCHANGE_PROMPT);
    }

    EXIT0;
  } else if (STATE == PERASE_PROMPT) {
    if (!sameas(margv[0], CANCEL)) {
      i = Num_Files(e->sys.memory[e->m]);
      e->p = INUM(margv[0]);

      if (e->p > 0 && e->p <= i) {
        --e->p;
        e->sys.memory[e->m][e->p].type = 0;

        if (e->p != MU_CAPACITY - 1) {
          for (i = e->p + 1; i != MU_CAPACITY; ++i) {
            e->sys.memory[e->m][i - 1] = e->sys.memory[e->m][i];
            e->sys.memory[e->m][i].type = 0;
          }
        }
      }
    }

    Memory_Contents(e->sys.memory[e->m], 1);
    Prompt(getmsg(MSG641), PMEMORY_PROMPT2);
  } else if (STATE == PCHANGE_PROMPT) {
    if (!sameas(margv[0], CANCEL)) {
      i = Num_Files(e->sys.memory[e->m]);
      e->p = INUM(margv[0]);

      if (e->p > 0 && e->p <= i) {
        --e->p;

        if (e->sys.memory[e->m][e->p].type == PROGRAM) {
          Perm_Prog_Edit_FSE();
          EXIT0;
        } else {
          Perm_Doc_Edit_FSE();
          EXIT0;
        }
      }
    }

    Memory_Contents(e->sys.memory[e->m], 1);
    Prompt(getmsg(MSG641), PMEMORY_PROMPT2);
  } else if (STATE == PADD_PROMPT) {
    e->p = -1;

    for (i = 0; i != MU_CAPACITY && e->p < 0; ++i) {
      if (e->sys.memory[e->m][i].type == 0) {
        e->p = i;
      }
    }

    if (sameas(margv[0], "P")) {
      e->sys.memory[e->m][e->p].type = PROGRAM;
      e->sys.memory[e->m][e->p].index = 1;
      e->sys.memory[e->m][e->p].lock = 0;
      e->sys.memory[e->m][e->p].packed = FALSE;
      e->sys.memory[e->m][e->p].vended = FALSE;
      e->sys.memory[e->m][e->p].class = Programs[1].class;
      e->sys.memory[e->m][e->p].size = Programs[1].size;
      e->sys.memory[e->m][e->p].load = Programs[1].load;
      e->sys.memory[e->m][e->p].value = Programs[1].value;
      e->sys.memory[e->m][e->p].strength = Programs[1].strength;

      for (i = 0; i != MAX_SUBS; ++i) {
        e->sys.memory[e->m][e->p].sub[i] = 0;
      }

      strcpy(e->sys.memory[e->m][e->p].name, Programs[1].Name);
      Perm_Prog_Add_FSE();
      EXIT0;
    } else if (sameas(margv[0], "D")) {
      e->sys.memory[e->m][e->p].type = DOCUMENT;
      e->sys.memory[e->m][e->p].class = 1;
      e->sys.memory[e->m][e->p].lock = 0;
      e->sys.memory[e->m][e->p].value = 0;
      e->sys.memory[e->m][e->p].packed = FALSE;
      e->sys.memory[e->m][e->p].vended = FALSE;
      e->sys.memory[e->m][e->p].size = 1;
      strcpy(e->sys.memory[e->m][e->p].name, "NONAME.DOC");
      Perm_Doc_Add_FSE();
      EXIT0;
    } else {
      Memory_Contents(e->sys.memory[e->m], 1);
      Prompt(getmsg(MSG641), PMEMORY_PROMPT2);
    }
  } else if (STATE == PDESIGN_X1_PROMPT) {
    if (margc == 0) {
      STATE = PDESIGN_PROMPT;
    } else {
      e->x1 = INUM(margv[0]);

      if (e->x1 < 0 || e->x1 >= MAX_SYS_X) {
        STATE = PDESIGN_PROMPT;
      } else {
        sprintf(str, getmsg(MSG342), MAX_SYS_Y - 1);
        Prompt(str, PDESIGN_Y1_PROMPT);
      }
    }
  } else if (STATE == PDESIGN_Y1_PROMPT) {
    if (margc == 0) {
      STATE = PDESIGN_PROMPT;
    } else {
      Clear_Display_Window();
      e->y1 = INUM(margv[0]);

      if (e->y1 < 0 || e->y1 >= MAX_SYS_Y) {
        STATE = PDESIGN_PROMPT;
      } else {
        if (e->option == 1) {
          if (e->sys.node_type[e->x1][e->y1]) {
            prfmsg(MSG343, e->x1, e->y1);
          } else {
            e->sys.node_type[e->x1][e->y1] = CPU;
            e->sys.node_index[e->x1][e->y1] = e->sys.cpus;
            ++e->sys.cpus;
            Display_A_System(&e->sys);
          }

          STATE = PDESIGN_PROMPT;
        } else if (e->option == 2) {
          if (e->sys.node_type[e->x1][e->y1] != CPU) {
            prfmsg(MSG635, e->x1, e->y1);
          } else {
            Remove_CPU(&e->sys, e->sys.node_index[e->x1][e->y1]);
            --e->sys.cpus;
            Display_A_System(&e->sys);
          }

          STATE = PDESIGN_PROMPT;
        } else if (e->option == 3) {
          if (e->sys.node_type[e->x1][e->y1]) {
            prfmsg(MSG343, e->x1, e->y1);
          } else {
            m = e->sys.mus;

            for (i = 0; i != MU_CAPACITY; ++i) {
              e->sys.memory[m][i].name[0] = 0;
              e->sys.memory[m][i].type = 0;
              e->sys.memory[m][i].class = 0;
              e->sys.memory[m][i].index = 0;
              e->sys.memory[m][i].value = 0;
              e->sys.memory[m][i].strength = 0;
              e->sys.memory[m][i].size = 0;
              e->sys.memory[m][i].load = 0;
              e->sys.memory[m][i].lock = 0;
              e->sys.memory[m][i].packed = 0;
              e->sys.memory[m][i].vended = 0;
              e->sys.memory[m][i].running = 0;

              for (s = 0; s != MAX_SUBS; ++s) {
                e->sys.memory[m][i].sub[s] = 0;
              }
            }

            e->sys.node_type[e->x1][e->y1] = MEMORY;
            e->sys.node_index[e->x1][e->y1] = e->sys.mus;
            ++e->sys.mus;
            Display_A_System(&e->sys);
          }

          STATE = PDESIGN_PROMPT;
        } else if (e->option == 4) {
          if (e->sys.node_type[e->x1][e->y1] != MEMORY) {
            prfmsg(MSG638, e->x1, e->y1);
          } else {
            Remove_Memory
            (
              &e->sys,
              e->sys.node_index[e->x1][e->y1]
            );
            --e->sys.mus;
            Display_A_System(&e->sys);
          }

          STATE = PDESIGN_PROMPT;
        } else if (e->option == 5) {
          if (e->sys.node_type[e->x1][e->y1]) {
            prfmsg(MSG343, e->x1, e->y1);
          } else {
            e->sys.node_type[e->x1][e->y1] = DATA_WALL;
            Display_A_System(&e->sys);
          }

          STATE = PDESIGN_PROMPT;
        } else if (e->option == 6) {
          if (e->sys.node_type[e->x1][e->y1] != DATA_WALL) {
            prfmsg(MSG345, e->x1, e->y1);
          } else {
            e->sys.node_type[e->x1][e->y1] = 0;
            Display_A_System(&e->sys);
          }

          STATE = PDESIGN_PROMPT;
        } else if (e->option == 7) {
          if (e->sys.node_type[e->x1][e->y1]) {
            prfmsg(MSG343, e->x1, e->y1);
          } else {
            e->sys.node_type[e->x1][e->y1] = CLOSED_GATE;
            Display_A_System(&e->sys);
          }

          STATE = PDESIGN_PROMPT;
        } else if (e->option == 8) {
          if
          (
            e->sys.node_type[e->x1][e->y1] != CLOSED_GATE &&
            e->sys.node_type[e->x1][e->y1] != OPENED_GATE
          ) {
            prfmsg(MSG347, e->x1, e->y1);
          } else {
            e->sys.node_type[e->x1][e->y1] = 0;
            Display_A_System(&e->sys);
          }

          STATE = PDESIGN_PROMPT;
        } else if (e->option == 9) {
          if
          (
            e->sys.node_type[e->x1][e->y1] != CPU &&
            e->sys.node_type[e->x1][e->y1] != MEMORY
          ) {
            prfmsg(MSG348, e->x1, e->y1);
            STATE = PDESIGN_PROMPT;
          } else {
            sprintf(str, getmsg(MSG349), MAX_SYS_X - 1);
            Prompt(str, PDESIGN_X2_PROMPT);
          }
        } else if (e->option == 13) {
          if (e->sys.node_type[e->x1][e->y1]) {
            prfmsg(MSG343, e->x1, e->y1);
          } else {
            e->sys.node_type[e->x1][e->y1] = PHONE;
            Display_A_System(&e->sys);
          }

          STATE = PDESIGN_PROMPT;
        } else if (e->option == 14) {
          if (e->sys.node_type[e->x1][e->y1] != PHONE) {
            prfmsg(MSG854, e->x1, e->y1);
          } else {
            e->sys.node_type[e->x1][e->y1] = 0;
            Display_A_System(&e->sys);
          }

          STATE = PDESIGN_PROMPT;
        }
      }
    }
  } else if (STATE == PDESIGN_X2_PROMPT) {
    if (margc == 0) {
      STATE = PDESIGN_PROMPT;
    } else {
      e->x2 = INUM(margv[0]);

      if (e->x2 < 0 || e->x2 >= MAX_SYS_X) {
        STATE = PDESIGN_PROMPT;
      } else {
        sprintf(str, getmsg(MSG350), MAX_SYS_Y - 1);
        Prompt(str, PDESIGN_Y2_PROMPT);
      }
    }
  } else if (STATE == PDESIGN_Y2_PROMPT) {
    if (margc == 0) {
      STATE = PDESIGN_PROMPT;
    } else {
      Clear_Display_Window();
      e->y2 = INUM(margv[0]);

      if (e->y2 >= 0 && e->y2 < MAX_SYS_Y) {
        if (e->sys.node_type[e->x2][e->y2]) {
          prfmsg(MSG346, e->x2, e->y2);
        } else {
          e->sys.node_type[e->x2][e->y2] = e->sys.node_type[e->x1][e->y1];
          e->sys.node_index[e->x2][e->y2] = e->sys.node_index[e->x1][e->y1];
          e->sys.node_type[e->x1][e->y1] = 0;
          Display_A_System(&e->sys);
        }
      }

      STATE = PDESIGN_PROMPT;
    }
  }

  if (STATE == PDESIGN_PROMPT) {
    Prompt(getmsg(MX631), PDESIGN_PROMPT);
  }

  EXIT0;
}
/****************************************************************************
	edit a city
*/
short EXPORT Edit_World(void) {
  short   net_x, net_y;
  short   i;
  short   col;
  short   x, y;
  short   custom;
  short   file_found;
  FILE    *fp;
  struct  edit_struct *e;
  e = (struct edit_struct *) vdaptr;

  if (STATE == MAIN_PROMPT) {
    sprintf
    (
      str,
      getmsg(MSG903),
      glob->valhalla_x,
      glob->valhalla_y,
      glob->alien_x,
      glob->alien_y,
      glob->cyberia_x,
      glob->cyberia_y
    );
    Send(str, myp, FALSE);
    prf("\x1B[%d;1H", DISP_Y);

    for (net_x = col = 0; net_x != MAX_NET_X; ++net_x) {
      for (net_y = 0; net_y != MAX_NET_Y; ++net_y) {
        if
        (
          !sameas(world->city[net_x][net_y].name, FRONTIER) &&
          !sameas(world->city[net_x][net_y].name, VALHALLA) &&
          !sameas(world->city[net_x][net_y].name, ALIEN_CITY) &&
          !sameas(world->city[net_x][net_y].name, CYBERIA)
        ) {
          strcpy(str, world->city[net_x][net_y].name);
          str[5] = 0;
          prfmsg(MSG352, net_x, net_y, str);
          ++col;

          if (col == 6) {
            prf(NL);
            col = 0;
          }
        }
      }
    }

    World_View();
    Prompt(getmsg(MSG353), X_PROMPT);
    EXIT0;
  }

  if (STATE == X_PROMPT) {
    net_x = INUM(margv[0]);

    if (net_x >= MAX_NET_X) {
      STATE = MAIN_PROMPT;
      EXIT0;
    }

    e->net_x = net_x;
    Prompt(getmsg(MSG354), Y_PROMPT);
    EXIT0;
  }

  if (STATE == Y_PROMPT) {
    net_y = INUM(margv[0]);

    if (net_y >= MAX_NET_Y) {
      STATE = MAIN_PROMPT;
      EXIT0;
    }

    e->net_y = net_y;
    net_x = e->net_x;
    Read_City(e->net_x, e->net_y, &e->city);

    if (glob->world_edit == 2) {
      Read_City(e->net_x, e->net_y, &e->custom_city);
    } else {
      Read_Custom_City(e->net_x, e->net_y, &e->custom_city);
    }

    for (i = 0; i != MAX_CRASHED; ++i) {
      if
      (
        dead->sys[i].type &&
        dead->sys[i].net_x == e->net_x &&
        dead->sys[i].net_y == e->net_y
      ) {
        dead->sys[i].timer = 1;
      }
    }

    sprintf(str, getmsg(MSG355), world->city[net_x][net_y].name);
    Prompt(str, PICK_CITY_PROMPT1);
    EXIT0;
  }

  net_x = e->net_x;
  net_y = e->net_y;

  if (STATE == PICK_CITY_PROMPT1) {
    if (sameas(margv[0], ANS_NO)) {
      STATE = MAIN_PROMPT;
      EXIT0;
    }

    if (!sameas(margv[0], ANS_YES)) {
      EXIT0;
    }

    Display_Edit_City();
    Read_System_Names();
    EXIT0;
  } else if (STATE == PICK_CITY_PROMPT2) {
    prf("\x1B[1;38H");
    sprintf(str, getmsg(MSG373), e->custom_city.name, CLEAR_RIGHT);
    prf(str);

    if (e->custom_city.landmass == AFRICA) {
      strcpy(e->landmass, "Africa");
    } else if (e->custom_city.landmass == NAMERICA) {
      strcpy(e->landmass, "N America");
    } else if (e->custom_city.landmass == SAMERICA) {
      strcpy(e->landmass, "S America");
    } else if (e->custom_city.landmass == EUROPE) {
      strcpy(e->landmass, "Europe");
    } else if (e->custom_city.landmass == ASIA) {
      strcpy(e->landmass, "Asia");
    } else {
      strcpy(e->landmass, "None");
    }

    prf("\x1B[2;38H");
    sprintf(str, getmsg(MSG374), e->landmass, CLEAR_RIGHT);
    prf(str);
    prf("\x1B[3;38H");
    sprintf(str, getmsg(MSG375), e->custom_city.security, CLEAR_RIGHT);
    prf(str);
    prf("\x1B[4;38H");
    sprintf
    (
      str,
      getmsg(MSG920),
      y_or_n[e->custom_city.invisible],
      CLEAR_RIGHT
    );
    prf(str);
    prf("\x1B[5;38H");
    sprintf(str, getmsg(M376), CLEAR_RIGHT);
    prf(str);
    MY_PORT->prompt_y = 6;
    MY_PORT->prompt_x = 38;
    Prompt(getmsg(M377), CITY_OPTION_PROMPT);
    EXIT0;
  }

  if (STATE == CITY_OPTION_PROMPT) {
    if (sameas(margv[0], "1")) {
      MY_PORT->prompt_y = 6;
      MY_PORT->prompt_x = 38;
      Prompt(getmsg(MSG378), CITY_NAME_PROMPT);
      EXIT0;
    } else if (sameas(margv[0], "2")) {
      prf("\x1B[1;38H");
      prfmsg(MSG379, CLEAR_RIGHT);
      prf("\x1B[2;38H");
      prfmsg(MSG380, CLEAR_RIGHT);
      prf("\x1B[3;38H");
      prfmsg(MSG381, CLEAR_RIGHT);
      prf("\x1B[4;38H");
      prfmsg(MSG382, CLEAR_RIGHT);
      prf("\x1B[5;38H");
      prfmsg(MSG383, CLEAR_RIGHT);
      prf("\x1B[6;38H");
      prfmsg(MSG384, CLEAR_RIGHT);
      MY_PORT->prompt_y = 7;
      MY_PORT->prompt_x = 38;
      Prompt(getmsg(MSG385), LANDMASS_PROMPT);
      EXIT0;
    } else if (sameas(margv[0], "3")) {
      MY_PORT->prompt_y = 6;
      MY_PORT->prompt_x = 38;
      Prompt(getmsg(MSG386), SECURITY_PROMPT);
      EXIT0;
    } else if (sameas(margv[0], "4")) {
      if (e->custom_city.invisible) {
        e->custom_city.invisible = FALSE;
      } else {
        e->custom_city.invisible = TRUE;
      }

      STATE = -1;
    } else if (sameas(margv[0], "5")) {
      MY_PORT->prompt_y = 6;
      MY_PORT->prompt_x = 38;
      Prompt(getmsg(MSG353), X2_PROMPT);
      EXIT0;
    } else if (sameas(margv[0], "X")) {
      STATE = MAIN_PROMPT;
      EXIT0;
    }
  }

  if (STATE == CITY_NAME_PROMPT) {
    rstrin();
    input[CITY_NAME_SIZE - 1] = 0;
    strcpy(e->custom_city.name, input);
    STATE = -1;
  }

  if (STATE == SECURITY_PROMPT) {
    e->custom_city.security = atoi(margv[0]);
    STATE = -1;
  }

  if (STATE == LANDMASS_PROMPT) {
    e->custom_city.landmass = atoi(margv[0]);

    if (e->custom_city.landmass > LANDMASSES) {
      e->custom_city.landmass = 0;
    }

    STATE = -1;
  }

  if (STATE == X2_PROMPT) {
    x = INUM(margv[0]);
    e->x = x;

    if (x >= MAX_CITY_X) {
      STATE = MAIN_PROMPT;
      EXIT0;
    }

    ++MY_PORT->prompt_y;
    Prompt(getmsg(MSG354), Y2_PROMPT);
    EXIT0;
  }

  if (STATE == Y2_PROMPT) {
    y = INUM(margv[0]);

    if (y >= MAX_CITY_Y) {
      STATE = MAIN_PROMPT;
      EXIT0;
    }

    e->y = y;
    x = e->x;
    e->custom = FALSE;

    if (e->custom_city.type[x][y] && glob->world_edit != 2) {
      if (e->custom_city.type[x][y] == DELETED_NODE) {
        e->node_type = 0;
      } else {
        e->node_type = e->custom_city.type[x][y];
        e->custom = TRUE;
      }
    } else {
      e->node_type = e->city.type[x][y];
    }

    if (e->node_type == PHONE) {
      strcpy(e->system_name, "LD Link");
      e->permanant = FALSE;
    } else if (e->node_type == 0) {
      strcpy(e->system_name, "N/A");
      e->permanant = FALSE;
    } else {
      if (!e->custom) {
        Set_System_Path(x, y, net_x, net_y);
      } else {
        Set_Custom_Path(x, y, net_x, net_y);
      }

      fp = fopen(glob->path, READB);

      if (fp == NULL) {
        i = 0;
      } else {
        i = fread(a_system, SYSTEM_SIZE, 1, fp);
        fclose(fp);
      }

      if (i != 1) {
        a_system->Name[0] = 0;
        a_system->permanant = FALSE;
      }

      strcpy(e->system_name, a_system->Name);
      e->permanant = a_system->permanant;
    }

    STATE = EDIT_SYSTEM_PROMPT;
  }

  if (STATE == EDIT_SYSTEM_PROMPT) {
    prf("\x1B[1;38H");

    if (e->node_type == PHONE) {
      prfmsg(M619, "L/D Phone Link", CLEAR_RIGHT);
    } else if (e->node_type == 0) {
      prfmsg(M619, "Nothing", CLEAR_RIGHT);
    } else {
      for (i = 0; i != SYS_TYPES; ++i) {
        if (System[i].type == e->node_type) {
          prfmsg(M619, System[i].name, CLEAR_RIGHT);
        }
      }
    }

    prf("\x1B[2;38H");
    prfmsg(MSG620, e->system_name, CLEAR_RIGHT);
    prf("\x1B[3;38H");
    prfmsg(MSG621, y_or_n[e->permanant], CLEAR_RIGHT);
    prf("\x1B[4;38H");
    prfmsg(MSG622, CLEAR_RIGHT);
    MY_PORT->prompt_y = 5;
    MY_PORT->prompt_x = 38;
    Prompt(getmsg(MSG623), SYS_OPTION_PROMPT);
    EXIT0;
  }

  x = e->x;
  y = e->y;

  if (STATE == SYS_OPTION_PROMPT) {
    if (sameas(margv[0], "1")) {
      Clear_Display_Window();
      prf(NL);
      prfmsg(M389);
      Prompt(getmsg(M390), NODE_TYPE_PROMPT);
      EXIT0;
    } else if (sameas(margv[0], "2")) {
      if (e->node_type != 0 && e->node_type != PHONE) {
        Prompt(getmsg(MSG392), SYSTEM_NAME_PROMPT);
      }

      EXIT0;
    } else if (sameas(margv[0], "3")) {
      if (e->node_type != 0 && e->node_type != PHONE) {
        Prompt(getmsg(MSG393), PERM_NODE_PROMPT);
      }

      EXIT0;
    } else if (sameas(margv[0], CANCEL) || sameas(margv[0], "4")) {
      STATE = -1;
      e->custom_city.type[x][y] = e->node_type;

      if (e->node_type == PHONE || e->node_type == 0) {
        if (glob->world_edit == 2) {
          Set_System_Path(x, y, net_x, net_y);
          unlink(glob->path);
          Set_Perm_Path(x, y, net_x, net_y);
          unlink(glob->path);
        } else {
          Set_Custom_Path(x, y, net_x, net_y);
          unlink(glob->path);
          Set_Custom_Perm_Path(x, y, net_x, net_y);
          unlink(glob->path);

          if (e->node_type == 0) {
            e->custom_city.type[x][y] = DELETED_NODE;
          }
        }
      } else {
        if (glob->world_edit == 2) {
          Set_System_Path(x, y, net_x, net_y);
          custom = FALSE;
        } else {
          Set_Custom_Path(x, y, net_x, net_y);
          custom = TRUE;
        }

        fp = fopen(glob->path, READB);

        if (fp != NULL) {
          file_found = TRUE;
        } else {
          file_found = FALSE;

          if (glob->world_edit != 2) {
            Set_System_Path(x, y, net_x, net_y);
            custom = FALSE;
            fp = fopen(glob->path, READB);

            if (fp != NULL) {
              file_found = TRUE;
            }
          }
        }

        if (!file_found) {
          memset( (char *) a_system, 0, SYSTEM_SIZE);
          strcpy(a_system->Name, e->system_name);
          a_system->permanant = e->permanant;
          Init_System(a_system, e->node_type);

          if (glob->world_edit == 2) {
            Set_System_Path(x, y, net_x, net_y);
          } else {
            Set_Custom_Path(x, y, net_x, net_y);
          }

          Create_New(glob->path);
          fp = fopen(glob->path, APPENDB);
          fwrite(a_system, SYSTEM_SIZE, 1, fp);
          fclose(fp);

          if (glob->world_edit == 2) {
            Set_Perm_Path(x, y, net_x, net_y);
          } else {
            Set_Custom_Perm_Path(x, y, net_x, net_y);
          }

          if (e->permanant) {
            Create_New(glob->path);
            fp = fopen(glob->path, APPENDB);
            fwrite(a_system, SYSTEM_SIZE, 1, fp);
            fclose(fp);
          } else {
            unlink(glob->path);
          }
        } else {
          fread(a_system, SYSTEM_SIZE, 1, fp);
          strcpy(a_system->Name, e->system_name);
          a_system->permanant = e->permanant;
          fclose(fp);

          if (!custom && glob->world_edit != 2) {
            Set_Custom_Path(x, y, net_x, net_y);
            Create_New(glob->path);
            custom = TRUE;
            fp = fopen(glob->path, APPENDB);
            fwrite(a_system, SYSTEM_SIZE, 1, fp);
            fclose(fp);
          } else {
            fp = fopen(glob->path, READB);
            fwrite(a_system, SYSTEM_SIZE, 1, fp);
            fclose(fp);
          }

          if (!custom) {
            Set_Perm_Path(x, y, net_x, net_y);
          } else {
            Set_Custom_Perm_Path(x, y, net_x, net_y);
          }

          if (e->permanant) {
            fp = fopen(glob->path, READB);

            if (fp == NULL) {
              Create_New(glob->path);
              fp = fopen(glob->path, APPENDB);
              fwrite(a_system, SYSTEM_SIZE, 1, fp);
              fclose(fp);
            } else {
              fread(a_system, SYSTEM_SIZE, 1, fp);
              strcpy(a_system->Name, e->system_name);
              a_system->permanant = e->permanant;
              fclose(fp);
              fp = fopen(glob->path, READB);
              fwrite(a_system, SYSTEM_SIZE, 1, fp);
              fclose(fp);
            }
          } else {
            unlink(glob->path);
          }
        }
      }

      if (sameas(margv[0], "4")) {
        if
        (
          e->node_type == PHONE ||
          e->node_type == 0 ||
          !e->permanant
        ) {
          STATE = SYS_OPTION_PROMPT;
          EXIT0;
        } else {
          memcpy(&e->sys, a_system, SYSTEM_SIZE);
          STATE = DESIGN_PERM;
          Design_Perm();
          EXIT0;
        }
      }
    } else {
      EXIT0;
    }
  }

  if (STATE == NODE_TYPE_PROMPT) {
    i = INUM(margv[0]);

    if (i == 0) {
      i = 0;
    } else if (i == 1) {
      i = PHONE;
    } else if (i == 2) {
      i = BBS;
    } else if (i == 3) {
      i = LEV1;
    } else if (i == 4) {
      i = LEV2;
    } else if (i == 5) {
      i = LEV3;
    } else if (i == 6) {
      i = CRIMINAL;
    } else if (i == 7) {
      i = VENDOR;
    } else if (i == 8) {
      i = LEV4;
    } else if (i == 9) {
      i = LEV5;
    } else if (i == 10) {
      i = HUB;
    } else if (i == 11) {
      i = WORLDBANK;
    } else if (i == 12) {
      i = FILE_SERVER;
    } else if (i == 13) {
      i = WEB_SERVER;
    } else if (i == 14) {
      i = LEV6;
    } else if (i == 15) {
      i = LEV7;
    } else {
      i = e->node_type;
    }

    e->node_type = i;

    if (e->node_type == PHONE) {
      strcpy(e->system_name, "LD Link");
      e->permanant = FALSE;
    } else if (e->node_type == 0) {
      strcpy(e->system_name, "N/A");
      e->permanant = FALSE;
    }

    STATE = -2;
    EXIT0;
  }

  if (STATE == SYSTEM_NAME_PROMPT) {
    rstrin();
    input[SYSTEM_NAME_SIZE - 1] = 0;
    strcpy(e->system_name, input);
    STATE = -2;
    EXIT0;
  }

  if (STATE == PERM_NODE_PROMPT) {
    if (sameas(margv[0], ANS_YES)) {
      e->permanant = TRUE;
      STATE = -2;
    } else if (sameas(margv[0], ANS_NO)) {
      e->permanant = FALSE;
      STATE = -2;
    }

    EXIT0;
  }

  if (glob->world_edit == 2) {
    Write_City(net_x, net_y, &e->custom_city);
    memcpy(&e->city, &e->custom_city, CITY_SIZE);
  } else {
    Write_Custom_City(net_x, net_y, &e->custom_city);
    strcpy(e->city.name, e->custom_city.name);
    e->city.landmass = e->custom_city.landmass;
    e->city.security = e->custom_city.security;
  }

  for (x = 0; x != MAX_CITY_X; ++x) {
    for (y = 0; y != MAX_CITY_Y; ++y) {
      if
      (
        world->city[net_x][net_y].type[x][y] == CYBERDECK &&
        e->city.type[x][y] == 0
      ) {
        e->city.type[x][y] = CYBERDECK;
      }
    }
  }

  if (glob->world_edit != 2) {
    Read_City(net_x, net_y, &world->city[net_x][net_y]);
    Read_Custom_Cities(net_x, net_y);

    for (x = 0; x != MAX_CITY_X; ++x) {
      for (y = 0; y != MAX_CITY_Y; ++y) {
        if
        (
          e->city.type[x][y] == CYBERDECK &&
          world->city[net_x][net_y].type[x][y] == 0
        ) {
          world->city[net_x][net_y].type[x][y] = CYBERDECK;
        }
      }
    }
  } else {
    memcpy(&world->city[net_x][net_y], &e->city, CITY_SIZE);
  }

  Update_Others(CITY_VIEW, net_x, net_y, 0, 0);
  STATE = -1;
  EXIT0;
}
/****************************************************************************
	display the city being edited
*/
void EXPORT Display_Edit_City(void) {
  short   i;
  short   x;
  short   y;
  short   node;
  struct  edit_struct *e;
  e = (struct edit_struct *) vdaptr;
  Clear_Display_Window();
  MY_PORT->last_view = EDIT_CITY_VIEW;
  prf("\x1B[1;1H");
  prf("  ");

  for (x = i = 0; x != MAX_CITY_X; ++x) {
    prfmsg(MSG356, i);
    ++i;

    if (i == 10) {
      i = 0;
    }
  }

  prfmsg(MSG357, CLEAR_RIGHT);

  for (y = 0; y != MAX_CITY_Y; ++y) {
    prfmsg(MSG358, y);

    for (x = 0; x != MAX_CITY_X; ++x) {
      if (e->custom_city.type[x][y] == DELETED_NODE) {
        node = DELETED_NODE;
      } else if (e->custom_city.type[x][y]) {
        node = e->custom_city.type[x][y];
      } else if (e->city.type[x][y]) {
        node = e->city.type[x][y];
      } else {
        node = 0;
      }

      if (node == DELETED_NODE) {
        prfmsg(MSG359A);
      } else if (node == LEV1) {
        prfmsg(MSG359);
      } else if (node == LEV2) {
        prfmsg(MSG360);
      } else if (node == LEV3) {
        prfmsg(MSG361);
      } else if (node == PHONE) {
        prfmsg(MSG362);
      } else if (node == LEV4) {
        prfmsg(MSG363);
      } else if (node == LEV5) {
        prfmsg(MSG364);
      } else if (node == BBS) {
        prfmsg(MSG365);
      } else if (node == VENDOR) {
        prfmsg(MSG366);
      } else if (node == HUB) {
        prfmsg(MSG367);
      } else if (node == WORLDBANK) {
        prfmsg(M368);
      } else if (node == CRIMINAL) {
        prfmsg(MSG369);
      } else if (node == FILE_SERVER) {
        prfmsg(MSG798);
      } else if (node == WEB_SERVER) {
        prfmsg(MSG847);
      } else if (node == LEV6) {
        prfmsg(MSG848);
      } else if (node == LEV7) {
        prfmsg(MSG849);
      } else {
        prfmsg(MSG370);
      }
    }

    prf("%s\r", CLEAR_RIGHT);
  }

  prfmsg(MSG371);
}
/****************************************************************************
	god-like users can edit characters
*/
short EXPORT Edit_Player(void) {
  short   i;

  if (STATE == MAIN_PROMPT) {
    Prompt(getmsg(MSG394), EDIT_HUMAN_PROMPT);
    EXIT0;
  }

  i = INUM(margv[0]);

  if (i <= 0) {
    MY_PORT->recnum = Find_Alias(a_human, margv[0]);
  } else {
    MY_PORT->recnum = i;
    Read_Human(MY_PORT->recnum, a_human);
  }

  if (MY_PORT->recnum == -1) {
    prfmsg(MSG395, margv[0]);
    STATE = MAIN_PROMPT;
    EXIT0;
  }

  sprintf
  (
    vdatmp,
    human_edit_format,
    a_human->alias, '\0',
    a_human->userid, '\0',
    y_or_n[a_human->perm[SET_PERM]], '\0',
    y_or_n[a_human->perm[GAMEOP_PERM]], '\0',
    a_human->start_x, '\0',
    y_or_n[a_human->perm[MONITOR_PERM]], '\0',
    a_human->start_y, '\0',
    y_or_n[a_human->perm[FREE_PERM]], '\0',
    a_human->damage, '\0',
    y_or_n[a_human->perm[CREATE_PERM]], '\0',
    a_human->score, '\0',
    y_or_n[a_human->perm[EDIT_USER_PERM]], '\0',
    a_human->internet.code, '\0',
    y_or_n[a_human->perm[EDIT_WORLD_PERM]], '\0',
    a_human->bank.code, '\0',
    a_human->bank.balance, '\0',
    a_human->internet.owed, '\0',
    a_human->internet.credit_rating, '\0',
    y_or_n[a_human->employed], '\0',
    y_or_n[a_human->renting], '\0',
    y_or_n[a_human->outlaw], '\0',
    a_human->gang, '\0',
    a_human->rank, '\0'
  );
  clrprf();
  MY_PORT->editing = TRUE;
  STATE = USREDIT;
  fsdroom(USREDIT, human_screen_spec, 1);
  fsdapr(vdaptr, vdasiz, vdatmp);
  fsdbkg(fsdrft());
  fsdego(vfyadn, Player_Edit_Done);
  My_Outprf(MY_PORT->usrnum);
  EXIT0;
}
/****************************************************************************
	finished entering player data
*/
void EXPORT Player_Edit_Done(MSHORT save) {
  short   i;
  usrptr->state = glob->module_id;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && Port[i]->usrnum == usrnum) {
      myp = i;
    }
  }

  MY_PORT->editing = FALSE;
  setmbk(glob->cfg);

  if (save) {
    Read_Human(MY_PORT->recnum, a_human);
    fsdfxt(0, a_human->alias, UIDSIZ);
    fsdfxt(1, a_human->userid, UIDSIZ);
    fsdfxt(2, str, 2);

    if (sameas(str, ANS_YES)) {
      a_human->perm[SET_PERM] = TRUE;
    } else {
      a_human->perm[SET_PERM] = FALSE;
    }

    fsdfxt(3, str, 2);

    if (sameas(str, ANS_YES)) {
      a_human->perm[GAMEOP_PERM] = TRUE;
    } else {
      a_human->perm[GAMEOP_PERM] = FALSE;
    }

    fsdfxt(4, str, 4);
    a_human->start_x = atoi(str);
    fsdfxt(5, str, 2);

    if (sameas(str, ANS_YES)) {
      a_human->perm[MONITOR_PERM] = TRUE;
    } else {
      a_human->perm[MONITOR_PERM] = FALSE;
    }

    fsdfxt(6, str, 4);
    a_human->start_y = atoi(str);
    fsdfxt(7, str, 2);

    if (sameas(str, ANS_YES)) {
      a_human->perm[FREE_PERM] = TRUE;
    } else {
      a_human->perm[FREE_PERM] = FALSE;
    }

    fsdfxt(8, str, 6);
    a_human->damage = INUM(str);
    fsdfxt(9, str, 2);

    if (sameas(str, ANS_YES)) {
      a_human->perm[CREATE_PERM] = TRUE;
    } else {
      a_human->perm[CREATE_PERM] = FALSE;
    }

    fsdfxt(10, str, 10);
    a_human->score = LNUM(str);
    fsdfxt(11, str, 2);

    if (sameas(str, ANS_YES)) {
      a_human->perm[EDIT_USER_PERM] = TRUE;
    } else {
      a_human->perm[EDIT_USER_PERM] = FALSE;
    }

    fsdfxt(12, a_human->internet.code, CODE_SIZE);
    fsdfxt(13, str, 2);

    if (sameas(str, ANS_YES)) {
      a_human->perm[EDIT_WORLD_PERM] = TRUE;
    } else {
      a_human->perm[EDIT_WORLD_PERM] = FALSE;
    }

    fsdfxt(14, a_human->bank.code, CODE_SIZE);
    fsdfxt(15, str, 8);
    a_human->bank.balance = LNUM(str);
    fsdfxt(16, str, 8);
    a_human->internet.owed = LNUM(str);
    fsdfxt(17, str, 3);
    a_human->internet.credit_rating = INUM(str);
    fsdfxt(18, str, 2);

    if (sameas(str, ANS_YES)) {
      if (!a_human->employed) {
        a_human->payday = cofdat(today()) + 1;
        a_human->payday = datofc(a_human->payday);
      }

      a_human->employed = TRUE;
    } else {
      a_human->employed = FALSE;
    }

    fsdfxt(19, str, 2);

    if (sameas(str, ANS_YES)) {
      if (!a_human->renting) {
        a_human->rentdue = cofdat(today()) + glob->rentdays;
        a_human->rentdue = datofc(a_human->rentdue);
      }

      a_human->renting = TRUE;
    } else {
      a_human->renting = FALSE;
    }

    fsdfxt(20, str, 2);

    if (sameas(str, ANS_YES)) {
      a_human->outlaw = TRUE;
    } else {
      a_human->outlaw = FALSE;
    }

    fsdfxt(21, str, 4);
    a_human->gang = INUM(str);
    fsdfxt(22, str, 4);
    a_human->rank = INUM(str);

    if (a_human->rank < 0 || a_human->rank > 5) {
      a_human->rank = 1;
    }

    Write_User(a_human);
  }

  Redraw();
  prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
  prf("%s%s", MY_PORT->prompt, CLEAR_RIGHT);
  My_Outprf(MY_PORT->usrnum);
  btumil(usrnum, 79 - MY_PORT->prompt_len);
  rstmbk();
}
/****************************************************************************
	FSE for perm system params
*/
short EXPORT Perm_FSE(void) {
  struct edit_struct *e;
  e = (struct edit_struct *) vdaptr;
  sprintf
  (
    MY_PORT->fsetmp,
    sys_edit_format,
    e->sys.speed, '\0',
    e->sys.wall, '\0',
    e->sys.gate, '\0',
    (short) (e->sys.rebuild_time / 3600L), '\0'
  );
  clrprf();
  MY_PORT->editing = TRUE;
  STATE = M639;
  fsdroom(M639, sys_screen_spec, 1);
  fsdapr(MY_PORT->fse, FSE_SIZE, MY_PORT->fsetmp);
  fsdbkg(fsdrft());
  fsdego(vfyadn, Perm_Edit_Done);
  My_Outprf(MY_PORT->usrnum);
  EXIT0;
}
/****************************************************************************
	finished entering perm sys data
*/
void EXPORT Perm_Edit_Done(MSHORT save) {
  short   i;
  struct  edit_struct *e;
  e = (struct edit_struct *) vdaptr;
  usrptr->state = glob->module_id;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && Port[i]->usrnum == usrnum) {
      myp = i;
    }
  }

  MY_PORT->editing = FALSE;
  setmbk(glob->cfg);

  if (save) {
    fsdfxt(0, str, 3);
    e->sys.speed = atoi(str);
    fsdfxt(1, str, 4);

    if (atoi(str) > 100) {
      e->sys.wall = 100;
    } else {
      e->sys.wall = atoi(str);
    }

    fsdfxt(2, str, 4);

    if (atoi(str) > 100) {
      e->sys.gate = 100;
    } else {
      e->sys.gate = atoi(str);
    }

    fsdfxt(3, str, 5);
    e->sys.rebuild_time = atol(str);
    e->sys.rebuild_time = e->sys.rebuild_time * 3600L;
  }

  STATE = DESIGN_PERM;
  MY_PORT->last_view = 0;
  Design_Perm();
  prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
  prf("%s%s", MY_PORT->prompt, CLEAR_RIGHT);
  My_Outprf(MY_PORT->usrnum);
  btumil(usrnum, 79 - MY_PORT->prompt_len);
  rstmbk();
}
/****************************************************************************
	FSE for perm system docs
*/
short EXPORT Perm_Doc_Add_FSE(void) {
  struct edit_struct *e;
  e = (struct edit_struct *) vdaptr;
  sprintf
  (
    MY_PORT->fsetmp,
    doc_edit_format,
    e->sys.memory[e->m][e->p].class, '\0',
    e->sys.memory[e->m][e->p].value, '\0',
    e->sys.memory[e->m][e->p].lock, '\0',
    e->sys.memory[e->m][e->p].name, '\0',
    y_or_n[e->sys.memory[e->m][e->p].packed], '\0',
    e->sys.memory[e->m][e->p].size, '\0'
  );
  clrprf();
  MY_PORT->editing = TRUE;
  STATE = MSG644;
  fsdroom(MSG644, doc_screen_spec, 1);
  fsdapr(MY_PORT->fse, FSE_SIZE, MY_PORT->fsetmp);
  fsdbkg(fsdrft());
  fsdego(vfyadn, Perm_Doc_Add_Done);
  My_Outprf(MY_PORT->usrnum);
  EXIT0;
}
/****************************************************************************
	finished entering perm doc data
*/
void EXPORT Perm_Doc_Add_Done(MSHORT save) {
  short   i;
  struct  edit_struct *e;
  e = (struct edit_struct *) vdaptr;
  usrptr->state = glob->module_id;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && Port[i]->usrnum == usrnum) {
      myp = i;
    }
  }

  MY_PORT->editing = FALSE;
  setmbk(glob->cfg);

  if (!save) {
    e->sys.memory[e->m][e->p].type = 0;
  } else {
    fsdfxt(0, str, 2);
    e->sys.memory[e->m][e->p].class = atoi(str);

    if
    (
      e->sys.memory[e->m][e->p].class < 1 ||
      e->sys.memory[e->m][e->p].class > 4
    ) {
      e->sys.memory[e->m][e->p].class = 1;
    }

    fsdfxt(1, str, 6);
    e->sys.memory[e->m][e->p].value = atoi(str);

    if (e->sys.memory[e->m][e->p].value < 0) {
      e->sys.memory[e->m][e->p].value = 0;
    }

    fsdfxt(2, str, 3);
    e->sys.memory[e->m][e->p].lock = atoi(str);

    if (e->sys.memory[e->m][e->p].lock < 0) {
      e->sys.memory[e->m][e->p].lock = 0;
    }

    fsdfxt(3, e->sys.memory[e->m][e->p].name, 20);
    fsdfxt(4, str, 2);

    if (sameas(str, ANS_YES)) {
      e->sys.memory[e->m][e->p].packed = TRUE;
    } else {
      e->sys.memory[e->m][e->p].packed = FALSE;
    }

    fsdfxt(5, str, 2);
    e->sys.memory[e->m][e->p].size = atoi(str);

    if
    (
      e->sys.memory[e->m][e->p].size < 1 ||
      e->sys.memory[e->m][e->p].size > MU_CAPACITY
    ) {
      e->sys.memory[e->m][e->p].size = 1;
    }
  }

  MY_PORT->last_view = 0;
  Display_A_System(&e->sys);
  Memory_Contents(e->sys.memory[e->m], 1);
  Prompt(getmsg(MSG641), PMEMORY_PROMPT2);
  prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
  prf("%s%s", MY_PORT->prompt, CLEAR_RIGHT);
  My_Outprf(MY_PORT->usrnum);
  btumil(usrnum, 79 - MY_PORT->prompt_len);
  rstmbk();
}
/****************************************************************************
	FSE for perm system programs
*/
short EXPORT Perm_Prog_Add_FSE(void) {
  struct edit_struct *e;
  e = (struct edit_struct *) vdaptr;
  sprintf
  (
    MY_PORT->fsetmp,
    prog_edit_format,
    e->sys.memory[e->m][e->p].index, '\0',
    e->sys.memory[e->m][e->p].sub[0], '\0',
    e->sys.memory[e->m][e->p].lock, '\0',
    e->sys.memory[e->m][e->p].sub[1], '\0',
    y_or_n[e->sys.memory[e->m][e->p].packed], '\0',
    e->sys.memory[e->m][e->p].sub[2], '\0',
    e->sys.memory[e->m][e->p].strength, '\0',
    e->sys.memory[e->m][e->p].sub[3], '\0',
    e->sys.memory[e->m][e->p].value, '\0'
  );
  clrprf();
  MY_PORT->editing = TRUE;
  STATE = MY645;
  fsdroom(MY645, prog_screen_spec, 1);
  fsdapr(MY_PORT->fse, FSE_SIZE, MY_PORT->fsetmp);
  fsdbkg(fsdrft());
  fsdego(vfyadn, Perm_Prog_Add_Done);
  My_Outprf(MY_PORT->usrnum);
  EXIT0;
}
/****************************************************************************
	finished entering perm program data
*/
void EXPORT Perm_Prog_Add_Done(MSHORT save) {
  short   i;
  short   p_num;
  struct  edit_struct *e;
  e = (struct edit_struct *) vdaptr;
  usrptr->state = glob->module_id;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && Port[i]->usrnum == usrnum) {
      myp = i;
    }
  }

  MY_PORT->editing = FALSE;
  setmbk(glob->cfg);

  if (!save) {
    e->sys.memory[e->m][e->p].type = 0;
  } else {
    fsdfxt(0, str, 3);
    e->sys.memory[e->m][e->p].index = atoi(str);

    if
    (
      e->sys.memory[e->m][e->p].index < 1 ||
      e->sys.memory[e->m][e->p].index >= NUM_PROGRAMS
    ) {
      e->sys.memory[e->m][e->p].index = 1;
    }

    p_num = e->sys.memory[e->m][e->p].index;
    e->sys.memory[e->m][e->p].class = Programs[p_num].class;
    e->sys.memory[e->m][e->p].size = Programs[p_num].size;
    e->sys.memory[e->m][e->p].load = Programs[p_num].load;
    strcpy(e->sys.memory[e->m][e->p].name, Programs[p_num].Name);
    fsdfxt(1, str, 3);
    e->sys.memory[e->m][e->p].sub[0] = atoi(str);
    fsdfxt(3, str, 3);
    e->sys.memory[e->m][e->p].sub[1] = atoi(str);
    fsdfxt(5, str, 3);
    e->sys.memory[e->m][e->p].sub[2] = atoi(str);
    fsdfxt(7, str, 3);
    e->sys.memory[e->m][e->p].sub[3] = atoi(str);

    for (i = 0; i != MAX_SUBS; ++i) {
      p_num = e->sys.memory[e->m][e->p].sub[i];

      if (e->sys.memory[e->m][e->p].class != HYDRA) {
        e->sys.memory[e->m][e->p].sub[i] = 0;
      } else if
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
        e->sys.memory[e->m][e->p].sub[i] = 0;
      } else {
        ++e->sys.memory[e->m][e->p].size;
      }
    }

    fsdfxt(2, str, 3);
    e->sys.memory[e->m][e->p].lock = atoi(str);

    if (e->sys.memory[e->m][e->p].lock < 0) {
      e->sys.memory[e->m][e->p].lock = 0;
    }

    fsdfxt(4, str, 2);

    if (sameas(str, ANS_YES)) {
      e->sys.memory[e->m][e->p].packed = TRUE;
    } else {
      e->sys.memory[e->m][e->p].packed = FALSE;
    }

    fsdfxt(6, str, 3);
    e->sys.memory[e->m][e->p].strength = atoi(str);

    if (e->sys.memory[e->m][e->p].strength < 1) {
      e->sys.memory[e->m][e->p].strength = 1;
    } else if (e->sys.memory[e->m][e->p].strength >= 100) {
      e->sys.memory[e->m][e->p].strength = 99;
    }

    fsdfxt(8, str, 6);
    e->sys.memory[e->m][e->p].value = atoi(str);

    if (e->sys.memory[e->m][e->p].value < 0) {
      e->sys.memory[e->m][e->p].value = 0;
    }
  }

  prf("%c%s", CTRL_S, CLEAR_SCREEN);
  MY_PORT->last_view = 0;
  Display_A_System(&e->sys);
  Memory_Contents(e->sys.memory[e->m], 1);
  Prompt(getmsg(MSG641), PMEMORY_PROMPT2);
  prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
  prf("%s%s", MY_PORT->prompt, CLEAR_RIGHT);
  My_Outprf(MY_PORT->usrnum);
  btumil(usrnum, 79 - MY_PORT->prompt_len);
  rstmbk();
}
/****************************************************************************
	FSE for perm system docs
*/
short EXPORT Perm_Doc_Edit_FSE(void) {
  struct edit_struct *e;
  e = (struct edit_struct *) vdaptr;
  sprintf
  (
    MY_PORT->fsetmp,
    doc_edit_format,
    e->sys.memory[e->m][e->p].class, '\0',
    e->sys.memory[e->m][e->p].value, '\0',
    e->sys.memory[e->m][e->p].lock, '\0',
    e->sys.memory[e->m][e->p].name, '\0',
    y_or_n[e->sys.memory[e->m][e->p].packed], '\0',
    e->sys.memory[e->m][e->p].size, '\0'
  );
  clrprf();
  MY_PORT->editing = TRUE;
  STATE = MSG644;
  fsdroom(MSG644, doc_screen_spec, 1);
  fsdapr(MY_PORT->fse, FSE_SIZE, MY_PORT->fsetmp);
  fsdbkg(fsdrft());
  fsdego(vfyadn, Perm_Doc_Edit_Done);
  My_Outprf(MY_PORT->usrnum);
  EXIT0;
}
/****************************************************************************
	finished entering perm doc data
*/
void EXPORT Perm_Doc_Edit_Done(MSHORT save) {
  short   i;
  struct  edit_struct *e;
  e = (struct edit_struct *) vdaptr;
  usrptr->state = glob->module_id;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && Port[i]->usrnum == usrnum) {
      myp = i;
    }
  }

  MY_PORT->editing = FALSE;
  setmbk(glob->cfg);

  if (save) {
    fsdfxt(0, str, 2);
    e->sys.memory[e->m][e->p].class = atoi(str);

    if
    (
      e->sys.memory[e->m][e->p].class < 1 ||
      e->sys.memory[e->m][e->p].class > 4
    ) {
      e->sys.memory[e->m][e->p].class = 1;
    }

    fsdfxt(1, str, 6);
    e->sys.memory[e->m][e->p].value = atoi(str);

    if (e->sys.memory[e->m][e->p].value < 0) {
      e->sys.memory[e->m][e->p].value = 0;
    }

    fsdfxt(2, str, 3);
    e->sys.memory[e->m][e->p].lock = atoi(str);

    if (e->sys.memory[e->m][e->p].lock < 0) {
      e->sys.memory[e->m][e->p].lock = 0;
    }

    fsdfxt(3, e->sys.memory[e->m][e->p].name, 20);
    fsdfxt(4, str, 2);

    if (sameas(str, ANS_YES)) {
      e->sys.memory[e->m][e->p].packed = TRUE;
    } else {
      e->sys.memory[e->m][e->p].packed = FALSE;
    }

    fsdfxt(5, str, 2);
    e->sys.memory[e->m][e->p].size = atoi(str);

    if
    (
      e->sys.memory[e->m][e->p].size < 1 ||
      e->sys.memory[e->m][e->p].size > MU_CAPACITY
    ) {
      e->sys.memory[e->m][e->p].size = 1;
    }
  }

  MY_PORT->last_view = 0;
  Display_A_System(&e->sys);
  Memory_Contents(e->sys.memory[e->m], 1);
  Prompt(getmsg(MSG641), PMEMORY_PROMPT2);
  prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
  prf("%s%s", MY_PORT->prompt, CLEAR_RIGHT);
  My_Outprf(MY_PORT->usrnum);
  btumil(usrnum, 79 - MY_PORT->prompt_len);
  rstmbk();
}
/****************************************************************************
	FSE for perm system programs
*/
short EXPORT Perm_Prog_Edit_FSE(void) {
  struct edit_struct *e;
  e = (struct edit_struct *) vdaptr;
  sprintf
  (
    MY_PORT->fsetmp,
    prog_edit_format,
    e->sys.memory[e->m][e->p].index, '\0',
    e->sys.memory[e->m][e->p].sub[0], '\0',
    e->sys.memory[e->m][e->p].lock, '\0',
    e->sys.memory[e->m][e->p].sub[1], '\0',
    y_or_n[e->sys.memory[e->m][e->p].packed], '\0',
    e->sys.memory[e->m][e->p].sub[2], '\0',
    e->sys.memory[e->m][e->p].strength, '\0',
    e->sys.memory[e->m][e->p].sub[3], '\0',
    e->sys.memory[e->m][e->p].value, '\0'
  );
  clrprf();
  MY_PORT->editing = TRUE;
  STATE = MY645;
  fsdroom(MY645, prog_screen_spec, 1);
  fsdapr(MY_PORT->fse, FSE_SIZE, MY_PORT->fsetmp);
  fsdbkg(fsdrft());
  fsdego(vfyadn, Perm_Prog_Edit_Done);
  My_Outprf(MY_PORT->usrnum);
  EXIT0;
}
/****************************************************************************
	finished entering perm program data
*/
void EXPORT Perm_Prog_Edit_Done(MSHORT save) {
  short   i;
  short   p_num;
  struct  edit_struct *e;
  e = (struct edit_struct *) vdaptr;
  usrptr->state = glob->module_id;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && Port[i]->usrnum == usrnum) {
      myp = i;
    }
  }

  MY_PORT->editing = FALSE;
  setmbk(glob->cfg);

  if (save) {
    fsdfxt(0, str, 3);
    e->sys.memory[e->m][e->p].index = atoi(str);

    if
    (
      e->sys.memory[e->m][e->p].index < 1 ||
      e->sys.memory[e->m][e->p].index >= NUM_PROGRAMS
    ) {
      e->sys.memory[e->m][e->p].index = 1;
    }

    p_num = e->sys.memory[e->m][e->p].index;
    e->sys.memory[e->m][e->p].class = Programs[p_num].class;
    e->sys.memory[e->m][e->p].size = Programs[p_num].size;
    e->sys.memory[e->m][e->p].load = Programs[p_num].load;
    strcpy(e->sys.memory[e->m][e->p].name, Programs[p_num].Name);
    fsdfxt(1, str, 3);
    e->sys.memory[e->m][e->p].sub[0] = atoi(str);
    fsdfxt(3, str, 3);
    e->sys.memory[e->m][e->p].sub[1] = atoi(str);
    fsdfxt(5, str, 3);
    e->sys.memory[e->m][e->p].sub[2] = atoi(str);
    fsdfxt(7, str, 3);
    e->sys.memory[e->m][e->p].sub[3] = atoi(str);

    for (i = 0; i != MAX_SUBS; ++i) {
      p_num = e->sys.memory[e->m][e->p].sub[i];

      if (e->sys.memory[e->m][e->p].class != HYDRA) {
        e->sys.memory[e->m][e->p].sub[i] = 0;
      } else if
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
        e->sys.memory[e->m][e->p].sub[i] = 0;
      } else {
        ++e->sys.memory[e->m][e->p].size;
      }
    }

    fsdfxt(2, str, 3);
    e->sys.memory[e->m][e->p].lock = atoi(str);

    if (e->sys.memory[e->m][e->p].lock < 0) {
      e->sys.memory[e->m][e->p].lock = 0;
    }

    fsdfxt(4, str, 2);

    if (sameas(str, ANS_YES)) {
      e->sys.memory[e->m][e->p].packed = TRUE;
    } else {
      e->sys.memory[e->m][e->p].packed = FALSE;
    }

    fsdfxt(6, str, 3);
    e->sys.memory[e->m][e->p].strength = atoi(str);

    if (e->sys.memory[e->m][e->p].strength < 1) {
      e->sys.memory[e->m][e->p].strength = 1;
    } else if (e->sys.memory[e->m][e->p].strength >= 100) {
      e->sys.memory[e->m][e->p].strength = 99;
    }

    fsdfxt(8, str, 6);
    e->sys.memory[e->m][e->p].value = atoi(str);

    if (e->sys.memory[e->m][e->p].value < 0) {
      e->sys.memory[e->m][e->p].value = 0;
    }
  }

  prf("%c%s", CTRL_S, CLEAR_SCREEN);
  MY_PORT->last_view = 0;
  Display_A_System(&e->sys);
  Memory_Contents(e->sys.memory[e->m], 1);
  Prompt(getmsg(MSG641), PMEMORY_PROMPT2);
  prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
  prf("%s%s", MY_PORT->prompt, CLEAR_RIGHT);
  My_Outprf(MY_PORT->usrnum);
  btumil(usrnum, 79 - MY_PORT->prompt_len);
  rstmbk();
}
/****************************************************************************
	cycle reading the system names in a city
*/
void EXPORT Read_System_Names(void) {
  Clear_Display_Window();
  MY_PORT->misc = 0;
  MY_PORT->target_x = 0;
  MY_PORT->target_y = 0;
  MY_PORT->cycling = TRUE;
  btuinj(usrnum, CYCLE);
}
/****************************************************************************
	cycle reading the system names in a city
*/
short EXPORT Next_System_Name(void) {
  short   x = MY_PORT->target_x;
  short   y = MY_PORT->target_y;
  short   n;
  FILE    *fp;
  struct  edit_struct *e;
  e = (struct edit_struct *) vdaptr;

  if (e->city.type[x][y] || e->custom_city.type[x][y]) {
    if (e->custom_city.type[x][y] == DELETED_NODE) {
      strcpy(a_system->Name, "ERASED");
      n = 1;
    } else if
    (
      e->city.type[x][y] == CYBERDECK ||
      e->custom_city.type[x][y] == CYBERDECK
    ) {
      strcpy(a_system->Name, "USER");
      n = 1;
    } else if
    (
      e->city.type[x][y] == PHONE ||
      e->custom_city.type[x][y] == PHONE
    ) {
      strcpy(a_system->Name, "Phone");
      n = 1;
    } else if (e->custom_city.type[x][y]) {
      if (glob->world_edit == 1) {
        Set_Custom_Path(x, y, e->net_x, e->net_y);
      } else {
        Set_System_Path(x, y, e->net_x, e->net_y);
      }

      fp = fopen(glob->path, READB);

      if (fp == NULL) {
        n = 0;
        sprintf
        (
          str,
          "LoC: Bad City %d/%d %d/%d",
          e->net_x, e->net_y,
          x, y
        );
        NOTIFY(str);
      } else {
        n = fread(a_system, SYSTEM_SIZE, 1, fp);
        fclose(fp);
      }
    } else {
      Set_System_Path(x, y, e->net_x, e->net_y);
      fp = fopen(glob->path, READB);

      if (fp == NULL) {
        n = 0;
        sprintf
        (
          str,
          "LoC: Bad City %d/%d %d/%d",
          e->net_x, e->net_y,
          x, y
        );
        NOTIFY(str);
      } else {
        n = fread(a_system, SYSTEM_SIZE, 1, fp);
        fclose(fp);
      }
    }

    if (n == 1) {
      strcpy(str, a_system->Name);
    } else {
      strcpy(str, "BAD!");
    }

    str[6] = 0;
    prfmsg(MSG372, x, y, str);
    ++MY_PORT->misc;

    if (MY_PORT->misc == 5) {
      MY_PORT->misc = 0;
      prf(NL);
    }

    ++MY_PORT->target_x;
    My_Outprf(usrnum);
  }

  for (y = MY_PORT->target_y; y < MAX_CITY_Y; ++y) {
    for (x = MY_PORT->target_x; x < MAX_CITY_X; ++x) {
      if (e->city.type[x][y]) {
        MY_PORT->target_x = x;
        MY_PORT->target_y = y;
        btuinj(usrnum, CYCLE);
        EXIT0;
      }
    }

    MY_PORT->target_x = 0;
  }

  MY_PORT->cycling = FALSE;
  STATE = PICK_CITY_PROMPT2;
  Edit_World();
  prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
  prf("%s%s", MY_PORT->prompt, CLEAR_RIGHT);
  btumil(usrnum, 80 - MY_PORT->prompt_len - MY_PORT->prompt_x);
  My_Outprf(usrnum);
  EXIT0;
}



