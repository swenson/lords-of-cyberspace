#include <header.h>
#include <globals.h>

/****************************************************************************
	information on how to play LoC
*/
short EXPORT Help(void) {
  short   i;
  short   n;
  short   cmd;
  Clear_Display_Window();

  if (parm1[0] == 0) {
    prfmsg(MSG328);
  } else if (sameto(parm1, "commands")) {
    Help_Commands();
  } else if (sameto(parm1, "proglist")) {
    Help_Programs();
  } else if (sameto(parm1, "icons")) {
    Help_Icons();
  } else if (sameto(parm1, "moving")) {
    Dump_File("muicyber/.data/help/moving.txt");
  } else if (sameto(parm1, "chatting")) {
    Dump_File("muicyber/.data/help/chatting.txt");
  } else if (sameto(parm1, "general") || sameto(parm1, "version")) {
    if (MY_PORT->access_level != IDLE) {
      prfmsg(MSG613);
      EXIT0;
    }

    Help_Dump();
  } else {
    for (i = 1; i != NUM_PROGRAMS; ++i) {
      if (sameas(parm1, Programs[i].Name)) {
        Program_Help(parm1);
        EXIT0;
      }
    }

    for (i = n = 0; i < NUM_COMMANDS; ++i) {
      if (sameto(parm1, commands[i])) {
        ++n;
        cmd = i;

        if (sameas(parm1, commands[i])) {
          n = 1;
          i = NUM_COMMANDS;
        }
      }
    }

    if (n == 0) {
      prfmsg(MSG329, parm1);
    } else if (n > 1) {
      prfmsg(MSG185, parm1);
    } else {
      sprintf(glob->path, "%s/%s.txt", HELP_PATH, commands[cmd]);
      Dump_File(glob->path);
    }
  }

  EXIT0;
}
/****************************************************************************
	explain the various icons
*/
void Help_Icons(void) {
  if (me.option[IBM_OPT]) {
    prfmsg
    (
      ICONSCR1,
      glob->theguy,
      glob->badguy,
      glob->empty,
      glob->ldlink,
      glob->decksys,
      glob->bbssys,
      glob->aliensys,
      glob->endsys,
      glob->lev1sys,
      glob->lev2sys,
      glob->lev3sys,
      glob->lev4sys,
      glob->lev5sys,
      glob->lev6sys,
      glob->lev7sys,
      glob->banksys,
      glob->hubsys,
      glob->vendsys,
      glob->crimesys,
      glob->websys,
      glob->copsys,
      glob->gangsys,
      glob->datawall,
      glob->deadwall,
      glob->opengate,
      glob->clsdgate,
      glob->syscpui,
      glob->syscpuc,
      glob->sysmem,
      glob->sysmemx
    );
  } else {
    prfmsg
    (
      ICONSCR1,
      glob->atheguy,
      glob->abadguy,
      glob->aempty,
      glob->aldlink,
      glob->adecksys,
      glob->abbssys,
      glob->aufosys,
      glob->aendsys,
      glob->alev1sys,
      glob->alev2sys,
      glob->alev3sys,
      glob->alev4sys,
      glob->alev5sys,
      glob->alev6sys,
      glob->alev7sys,
      glob->abanksys,
      glob->ahubsys,
      glob->avendsys,
      glob->acrimsys,
      glob->awebsys,
      glob->acopsys,
      glob->agngsys,
      glob->adatawal,
      glob->adeadwal,
      glob->aopngate,
      glob->aclsgate,
      glob->asyscpui,
      glob->asyscpuc,
      glob->asysmem,
      glob->asysmemx
    );
  }
}
/****************************************************************************
	make a sorted command list along with a sorted index
*/
void Command_Sort(short *sorted_index) {
  short   i;
  short   n;
  short   t;
  char    cmd_sort[NUM_COMMANDS][COMMAND_SIZE];
  char    temp_cmd[COMMAND_SIZE];
  memset( (char *) cmd_sort, 0, sizeof(cmd_sort));

  for (i = 0; i != NUM_COMMANDS; ++i) {
    strcpy(cmd_sort[i], commands[i]);
    sorted_index[i] = i;
  }

  sorted_index[0] = 0;

  for (i = 0; i != NUM_COMMANDS; ++i) {
    for (n = 0; n != NUM_COMMANDS; ++n) {
      if (strcmp(cmd_sort[i], cmd_sort[n]) < 0) {
        strcpy(temp_cmd, cmd_sort[i]);
        strcpy(cmd_sort[i], cmd_sort[n]);
        strcpy(cmd_sort[n], temp_cmd);
        t = sorted_index[i];
        sorted_index[i] = sorted_index[n];
        sorted_index[n] = t;
      }
    }
  }
}
/****************************************************************************
	list all of the commands in alphabetical order
*/
void Help_Commands(void) {
  short   ok;
  short   special;
  short   i;
  short   l;
  short   sorted_index[NUM_COMMANDS];
  Command_Sort(sorted_index);

  for (i = l = 0; i != NUM_COMMANDS; ++i) {
    ok = TRUE;
    special = FALSE;

    if (sorted_index[i] == CREATE) {
      if (!me.perm[CREATE_PERM]) {
        ok = FALSE;
      } else {
        special = TRUE;
      }
    } else if (sorted_index[i] == TOURNEY) {
      if (me.perm[GAMEOP_PERM]) {
        special = TRUE;
      }
    } else if (sorted_index[i] == GO_OUTLAW && !glob->outlaw) {
      ok = FALSE;
    } else if (sorted_index[i] == PERMITS) {
      if (!me.perm[SET_PERM]) {
        ok = FALSE;
      } else {
        special = TRUE;
      }
    } else if (sorted_index[i] == EDIT) {
      if (!me.perm[EDIT_WORLD_PERM] && !me.perm[EDIT_USER_PERM]) {
        ok = FALSE;
      } else {
        special = TRUE;
      }
    }

    if (ok) {
      if (!special) {
        prfmsg(M330, commands[sorted_index[i]]);
      } else {
        prfmsg(MSG917, commands[sorted_index[i]]);
      }

      ++l;

      if (l == 7) {
        l = 0;
        prf(NL);
      }
    }
  }
}
/****************************************************************************
	list all of the programs in alphabetical order
*/
void Help_Programs(void) {
  short   i;
  short   l;
  short   sorted_index[NUM_PROGRAMS];
  Program_Sort(sorted_index);

  for (i = 1, l = 0; i != NUM_PROGRAMS; ++i) {
    if
    (
      Programs[sorted_index[i]].class != HYDRA ||
      Programs[sorted_index[i]].index == SATAN ||
      Programs[sorted_index[i]].index == FIEND ||
      Programs[sorted_index[i]].index == DEVIL
    ) {
      prfmsg(M331, Programs[sorted_index[i]].Name);
      ++l;

      if (l == 7) {
        l = 0;
        prf(NL);
      }
    }
  }
}
/****************************************************************************
	make a sorted program list along with a sorted index
*/
void Program_Sort(short *sorted_index) {
  short   i;
  short   n;
  short   t;
  char    pgm_sort[NUM_PROGRAMS][NAME_SIZE];
  char    temp_pgm[NAME_SIZE];
  memset( (char *) pgm_sort, 0, sizeof(pgm_sort));

  for (i = 1; i != NUM_PROGRAMS; ++i) {
    strcpy(pgm_sort[i], Programs[i].Name);
    sorted_index[i] = i;
  }

  sorted_index[0] = 0;

  for (i = 1; i != NUM_PROGRAMS; ++i) {
    for (n = 1; n != NUM_PROGRAMS; ++n) {
      if (strcmp(pgm_sort[i], pgm_sort[n]) < 0) {
        strcpy(temp_pgm, pgm_sort[i]);
        strcpy(pgm_sort[i], pgm_sort[n]);
        strcpy(pgm_sort[n], temp_pgm);
        t = sorted_index[i];
        sorted_index[i] = sorted_index[n];
        sorted_index[n] = t;
      }
    }
  }
}
/****************************************************************************
	use mbbs file download
*/
void Help_Dump(void) {
  /*if (ftgnew() == 0) {
    ftgsbm("");
    STATE = 0;
  } else {
    prf("%s", CLEAR_SCREEN);
    STATE = DOWNLOADING;

    if (sameto(parm1, "general")) {
      stzcpy(ftgptr->tagspc, "general.txt", TSLENG);
    } else {
      stzcpy(ftgptr->tagspc, "change.log", TSLENG);
    }

    ftgptr->tshndl = Download_Handler;
    ftgsbm("?");
    MY_PORT->downloading = TRUE;
  }*/
  prf("Check the link below, Ace.\n");
  My_Outprf(usrnum);
}
/****************************************************************************
	Download Handler
*/
int EXPORT Download_Handler(int tshcod) {
  int     rc = 0;

  switch (tshcod) {
  case TSHVIS:
    rc = 1;
    break;

  case TSHDSC:
    sprintf(tshmsg, "file %s", ftgptr->tagspc);
    break;

  case TSHBEG:
    sprintf(tshmsg, "muicyber/.data/help/%s", ftgptr->tagspc);
    strcpy(ftfscb->fname, ftgptr->tagspc);
    rc = 1;
    break;

  case TSHFIN:
    usrptr->state = glob->module_id;
    STATE = DOWNLOADING;
    setmbk(glob->cfg);
    prfmsg(MSG207);
    My_Outprf(usrnum);
    rstmbk();
    rc = 1;
    break;
  }

  return (rc);
}
