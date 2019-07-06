#include <header.h>
#include <programs.h>
#include <vended.h>
#include <systypes.h>

/* global variables */

struct module cybermod = {
  "\0",
  cyber_login,
  cyber_input,
  cyber_status,
  cyber_asynch,
  NULL,
  cyber_hup,
  cyber_clean,
  cyber_delete,
  cyber_shutdown
  };

struct  gang_struct             *gang;
struct  world_struct            *world;
struct  misc_world_struct       *misc_world;
struct  tracking_struct         *tracking;
struct  port_struct             *Port[MAXGUYS];
struct  system_struct           *Sys[MAXSYSTEMS];
struct  processor_struct        *Proc[MAXSYSTEMS];
struct  city_struct             *a_city;
struct  tourney_struct          *tourney;
struct  human_struct            *a_human;
struct  human_struct            *a_user;
struct  human_struct            *my_record;
struct  system_struct           *a_system;
struct  globals_struct          *glob;
struct  crashed_struct          *dead;

char    str[500];
char    parm1[LINE_SIZE];
char    parm2[LINE_SIZE];
char    parm3[LINE_SIZE];
char    msg_text[256];
char    temp_userid[UIDSIZ];

char    *commands[] = {
  "X",
  "Programs",  "Clock",    "Where",     "Deck",     "Permits",
  "Edit",      "Top",      "CPUs",      "Stop",     "Suicide",
  "Dayfile",   "Outlaw",   "Look",      "Services", "Memory",
  "Option",    "Chips",    "Docs",      "Design",   "Jack",
  "Back",      "Finances", "Create",    "Help",     "Redraw",
  "North",     "South",    "East",      "West",     "Rank",
  "Mail",      "Who",      "Mission",   "Gang",     "Tourney"
};

char    *program_class[] = {
  "",
  "Utility",
  "Defense",
  "Agent",
  "Weapon",
  "Anti-System",
  "Anti-Program",
  "ICE",
  "Hydra",
  "End-Game Key"
};

char    *document_class[] = {
  "",
  "Useless",
  "Business",
  "Secret",
  "Top Secret",
  "Internet Code",
  "Bank Code",
  "Gate Code",
  "End-Game!",
  "Gang Code"
};

short   myp;
short   command;
short   max_guys = 0;
short   max_systems = 0;

char    *y_or_n[] = {ANS_NO, ANS_YES};

char human_screen_spec[] = "\
ALIAS \
USERID \
SET(ALT=N ALT=Y MULTICHOICE) \
GAMEOP(ALT=N ALT=Y MULTICHOICE) \
START_X \
MONITOR(ALT=N ALT=Y MULTICHOICE) \
START_Y \
FREE(ALT=N ALT=Y MULTICHOICE) \
DAMAGE \
CREATE(ALT=N ALT=Y MULTICHOICE) \
SCORE \
USEREDIT(ALT=N ALT=Y MULTICHOICE) \
NETCODE \
WEDIT(ALT=N ALT=Y MULTICHOICE) \
BANKCODE \
BANKBAL \
NETBAL \
RATING \
JOB(ALT=N ALT=Y MULTICHOICE) \
RENT(ALT=N ALT=Y MULTICHOICE) \
OUTLAW(ALT=N ALT=Y MULTICHOICE) \
GANG \
GANGRANK \
DONE(ALT=SAVE ALT=QUIT MULTICHOICE)";

char human_edit_format[] =
  "ALIAS=%s%c"
  "USERID=%s%c"
  "SET=%s%c"
  "GAMEOP=%s%c"
  "START_X=%d%c"
  "MONITOR=%s%c"
  "START_Y=%d%c"
  "FREE=%s%c"
  "DAMAGE=%d%c"
  "CREATE=%s%c"
  "SCORE=%ld%c"
  "USEREDIT=%s%c"
  "NETCODE=%s%c"
  "WEDIT=%s%c"
  "BANKCODE=%s%c"
  "BANKBAL=%ld%c"
  "NETBAL=%ld%c"
  "RATING=%d%c"
  "JOB=%s%c"
  "RENT=%s%c"
  "OUTLAW=%s%c"
  "GANG=%d%c"
  "GANGRANK=%d%c";

char gang_edit_format[] =
  "RANK0=%s%c"
  "NEWBIE=%d%c"
  "RANK1=%s%c"
  "PERM10=%s%c"
  "PERM11=%s%c"
  "PERM12=%s%c"
  "PERM13=%s%c"
  "PERM14=%s%c"
  "PERM15=%s%c"

  "RANK2=%s%c"
  "PERM20=%s%c"
  "PERM21=%s%c"
  "PERM22=%s%c"
  "PERM23=%s%c"
  "PERM24=%s%c"
  "PERM25=%s%c"

  "RANK3=%s%c"
  "PERM30=%s%c"
  "PERM31=%s%c"
  "PERM32=%s%c"
  "PERM33=%s%c"
  "PERM34=%s%c"
  "PERM35=%s%c"

  "RANK4=%s%c"
  "PERM40=%s%c"
  "PERM41=%s%c"
  "PERM42=%s%c"
  "PERM43=%s%c"
  "PERM44=%s%c"
  "PERM45=%s%c"

  "RANK5=%s%c"
  "PERM50=%s%c"
  "PERM51=%s%c"
  "PERM52=%s%c"
  "PERM53=%s%c"
  "PERM54=%s%c"
  "PERM55=%s%c";

char gang_screen_spec[] = "\
RANK0 \
NEWBIE \
RANK1 \
PERM10(ALT=N ALT=Y MULTICHOICE) \
PERM11(ALT=N ALT=Y MULTICHOICE) \
PERM12(ALT=N ALT=Y MULTICHOICE) \
PERM13(ALT=N ALT=Y MULTICHOICE) \
PERM14(ALT=N ALT=Y MULTICHOICE) \
PERM15(ALT=N ALT=Y MULTICHOICE) \
RANK2 \
PERM20(ALT=N ALT=Y MULTICHOICE) \
PERM21(ALT=N ALT=Y MULTICHOICE) \
PERM22(ALT=N ALT=Y MULTICHOICE) \
PERM23(ALT=N ALT=Y MULTICHOICE) \
PERM24(ALT=N ALT=Y MULTICHOICE) \
PERM25(ALT=N ALT=Y MULTICHOICE) \
RANK3 \
PERM30(ALT=N ALT=Y MULTICHOICE) \
PERM31(ALT=N ALT=Y MULTICHOICE) \
PERM32(ALT=N ALT=Y MULTICHOICE) \
PERM33(ALT=N ALT=Y MULTICHOICE) \
PERM34(ALT=N ALT=Y MULTICHOICE) \
PERM35(ALT=N ALT=Y MULTICHOICE) \
RANK4 \
PERM40(ALT=N ALT=Y MULTICHOICE) \
PERM41(ALT=N ALT=Y MULTICHOICE) \
PERM42(ALT=N ALT=Y MULTICHOICE) \
PERM43(ALT=N ALT=Y MULTICHOICE) \
PERM44(ALT=N ALT=Y MULTICHOICE) \
PERM45(ALT=N ALT=Y MULTICHOICE) \
RANK5 \
PERM50(ALT=N ALT=Y MULTICHOICE) \
PERM51(ALT=N ALT=Y MULTICHOICE) \
PERM52(ALT=N ALT=Y MULTICHOICE) \
PERM53(ALT=N ALT=Y MULTICHOICE) \
PERM54(ALT=N ALT=Y MULTICHOICE) \
PERM55(ALT=N ALT=Y MULTICHOICE) \
DONE(ALT=SAVE ALT=QUIT MULTICHOICE)";

char tourney_edit_format[] =
  "POSTGAME=%d%c"
  "CREDS0=%ld%c"
  "PRIZE0=%s%c"
  "CREDS1=%ld%c"
  "PRIZE1=%s%c"
  "CREDS2=%ld%c"
  "PRIZE2=%s%c"
  "CREDS3=%ld%c"
  "PRIZE3=%s%c"
  "CREDS4=%ld%c"
  "PRIZE4=%s%c"
  "CREDS5=%ld%c"
  "PRIZE5=%s%c"
  "CREDS6=%ld%c"
  "PRIZE6=%s%c"
  "CREDS7=%ld%c"
  "PRIZE7=%s%c"
  "CREDS8=%ld%c"
  "PRIZE8=%s%c";

char tourney_screen_spec[] = "\
POSTGAME \
CREDS0 \
PRIZE0 \
CREDS1 \
PRIZE1 \
CREDS2 \
PRIZE2 \
CREDS3 \
PRIZE3 \
CREDS4 \
PRIZE4 \
CREDS5 \
PRIZE5 \
CREDS6 \
PRIZE6 \
CREDS7 \
PRIZE7 \
CREDS8 \
PRIZE8 \
DONE(ALT=SAVE ALT=QUIT MULTICHOICE)";

char sys_screen_spec[] = "\
SPEED \
WALL \
GATE \
REBUILD \
DONE(ALT=SAVE ALT=QUIT MULTICHOICE)";

char sys_edit_format[] =
  "SPEED=%d%c"
  "WALL=%d%c"
  "GATE=%d%c"
  "REBUILD=%d%c";

char doc_screen_spec[] = "\
CLASS \
VALUE \
LOCK \
NAME \
PACKED \
SIZE \
DONE(ALT=SAVE ALT=QUIT MULTICHOICE)";

char doc_edit_format[] =
  "CLASS=%d%c"
  "VALUE=%d%c"
  "LOCK=%d%c"
  "NAME=%s%c"
  "PACKED=%s%c"
  "SIZE=%d%c";

char prog_screen_spec[] = "\
INDEX \
SUB1 \
LOCK \
SUB2 \
PACKED \
SUB3 \
STRENGTH \
SUB4 \
VALUE \
DONE(ALT=SAVE ALT=QUIT MULTICHOICE)";

char prog_edit_format[] =
  "INDEX=%d%c"
  "SUB1=%d%c"
  "LOCK=%d%c"
  "SUB2=%d%c"
  "PACKED=%s%c"
  "SUB3=%d%c"
  "STRENGTH=%d%c"
  "SUB4=%d%c"
  "VALUE=%d%c";

#include <ranks.h>
#include <world.h>

/**************************************************************************
	initialization routine
*/
void EXPORT init__muicyber(void) {
  short   i;
  short   x;
  long    used = 0;
  sprintf(str, "LoC: Initializing %s", VERSION);
  NOTIFY(str);
  printf("LoC: system size %ld\n", SYSTEM_SIZE);
  stzcpy(cybermod.descrp, gmdnam("MUICYBER.MDF"), MNMSIZ);
  i = register_module(&cybermod);
  glob = (struct globals_struct *) alczer(GLOBALS_SIZE);
  used += GLOBALS_SIZE;
  glob->module_id = i;
  dead = (struct crashed_struct *) alczer(CRASHED_SIZE);
  used += CRASHED_SIZE;
  gang = (struct gang_struct *) alczer(GANG_SIZE);
  used += GANG_SIZE;
  tourney = (struct tourney_struct *) alczer(TOURNEY_SIZE);
  used += TOURNEY_SIZE;
  Get_Config();
  randomize();
  Sort_Programs();

  for (i = 0; i != max_guys; ++i) {
    Port[i] = (struct port_struct *) alczer(PORT_SIZE);
    used += PORT_SIZE;
  }

  a_city = (struct city_struct *) alczer(CITY_SIZE);
  used += CITY_SIZE;

  for (i = 0; i != max_systems; ++i) {
    Sys[i] = (struct system_struct *) alczer(SYSTEM_SIZE);
    used += SYSTEM_SIZE;
  }

  for (i = 0; i != max_systems; ++i) {
    Proc[i] = (struct processor_struct *) alczer(PROCESSOR_SIZE);
    used += PROCESSOR_SIZE;
  }

  a_system = (struct system_struct *) alczer(SYSTEM_SIZE);
  used += SYSTEM_SIZE;
  a_human = (struct human_struct *) alczer(HUMAN_SIZE);
  used += HUMAN_SIZE;
  a_user = (struct human_struct *) alczer(HUMAN_SIZE);
  used += HUMAN_SIZE;
  my_record = (struct human_struct *) alczer(HUMAN_SIZE);
  used += HUMAN_SIZE;
  tracking = (struct tracking_struct *) alczer(TRACKING_SIZE);
  used += TRACKING_SIZE;
  world = (struct world_struct *) alczer(WORLD_SIZE);
  used += WORLD_SIZE;
  misc_world = (struct misc_world_struct *) alczer(MISC_WORLD_SIZE);
  used += MISC_WORLD_SIZE;
  used = used / 1000;
  sprintf(str, "LoC: Allocated %ldK", used);
  NOTIFY(str);
  Init_Data_Files();
  Init_World();
  dclvda(sizeof(struct edit_struct));
  dclvda(POST_SIZE);
  dclvda(FREENET_SIZE);
  dclvda(fsdroom(USREDIT, human_screen_spec, 0));
  dclvda(fsdroom(EDTGANG, gang_screen_spec, 0));
  dclvda(fsdroom(MSG878, tourney_screen_spec, 0));
  glob->alien_timer = 1;
  glob->alien_x = -1;
  glob->alien_y = -1;
  glob->cyberia_x = -1;
  glob->cyberia_y = -1;
  glob->cyberia_timer = 1;
  NOTIFY("LoC: Initializing Valhalla");
  Init_Valhalla();
  NOTIFY("LoC: Initializing Cops");
  Init_Cops();
  NOTIFY("LoC: Initializing Cons");
  Init_Cons();
  NOTIFY("LoC: Initializing Tourney");
  Init_Tourney();

  for (x = 4, used = glob->rankfctr; x != NUM_RANKS; ++x) {
    rank[x].score = rank[x - 1].score + used;
    used += glob->rankfctr;
  }

  rtkick(1, Cyber_Server);
  NOTIFY("LoC: Done Initializing");
}
/**************************************************************************
	logon entry point
*/
GBOOL cyber_login(void) {
  short   index;
  long    dur;

  if (usrptr->flags & WSGCSU) {
    return (FALSE);
  }

  setmbk(glob->cfg);

  if (glob->winner[0] == 0) {
    Top_Nine(0);
  }

  if (tourney->credits[0] && tourney->prizes[0][0]) {
    sprintf
    (
      str,
      "%ld system credits and %s",
      tourney->credits[0],
      tourney->prizes[0]
    );
  } else if (tourney->credits[0]) {
    sprintf
    (
      str,
      "%ld system credits",
      tourney->credits[0]
    );
  } else if (tourney->prizes[0][0]) {
    strcpy(str, tourney->prizes[0]);
  } else {
    str[0] = 0;
  }

  if (tourney->phase == PREGAME) {
    if (glob->pre_blurb) {
      index = Read_My_Rec(a_human);

      if (index < 0) {
        if (str[0]) {
          prfmsg(TBLURB1A, str);
        } else {
          prfmsg(TBLURB1B);
        }

        My_Outprf(usrnum);
      }
    }
  } else if (tourney->phase == GAME) {
    if (glob->game_blurb) {
      index = Read_My_Rec(a_human);

      if (index < 0) {
        if (str[0]) {
          prfmsg(TBLURB2B, glob->topguy, str);
        } else {
          prfmsg(TBLURB2A, glob->topguy);
        }

        My_Outprf(usrnum);
      }
    }
  } else if (tourney->phase == POSTGAME) {
    if (glob->post_blurb) {
      dur = (tourney->phase_end - time(0)) / 3600L;

      if (dur > 1) {
        sprintf(glob->buf, "in %ld hours", dur);
      } else if (dur == 1) {
        sprintf(glob->buf, "in %ld hour", dur);
      } else {
        dur = (tourney->phase_end - time(0)) / 60L;

        if (dur > 1) {
          sprintf(glob->buf, "in %ld minutes", dur);
        } else if (dur == 1) {
          sprintf(glob->buf, "in %ld minute", dur);
        } else {
          dur = tourney->phase_end - time(0);

          if (dur < 0) {
            if (!tourney->results_tallied) {
              strcpy(glob->buf, "after system clean-up");
            } else {
              strcpy(glob->buf, "immediately");
            }
          } else {
            sprintf(glob->buf, "in %ld seconds", dur);
          }
        }
      }

      if (str[0]) {
        prfmsg(TBLURB3B, glob->winner, str, glob->buf);
      } else {
        prfmsg(TBLURB3A, glob->winner, glob->buf);
      }

      My_Outprf(usrnum);
    }
  }

  rstmbk();
  return (FALSE);
}
/**************************************************************************
	called after user enters a line and hits c/r
*/
GBOOL EXPORT cyber_input(void) {
  short   i;
  short   m, p;
  long    n;
  setmbk(glob->cfg);
  btuchi(usrnum, Stroke_Monitor);
  btuxnf(usrnum, 0, 19);

  for (i = 0, myp = -1; i != max_guys && myp < 0; ++i) {
    if
    (
      !glob->reset &&
      Port[i]->active &&
      Port[i]->hung_up &&
      sameas(Port[i]->guy.userid, usaptr->userid)
    ) {
      myp = i;
      MY_PORT->hung_up = FALSE;
      MY_PORT->usrnum = usrnum;
      MY_PORT->prompt_y = PROMPT_Y;
      MY_PORT->prompt_x = 1;
      prf("%c%s", CTRL_S, CLEAR_SCREEN);
      Redraw();
      Main_Prompt();
      margc = 0;
    } else if (Port[i]->active && Port[i]->usrnum == usrnum) {
      myp = i;

      if (usrptr->flags & INVISB) {
        MY_PORT->invisible = TRUE;
      } else {
        MY_PORT->invisible = FALSE;
      }
    }
  }

  if (glob->reset) {
    prfmsg(MSG887);
    My_Outprf(usrnum);

    if (myp != -1) {
      cyber_hup();
      myp = -1;
    }
  } else if (myp == -1) {
    /* swenson: disabled */
    /*if (glob->validus[0] == 0)
    {
    	prfmsg(MSG139);
    	btuchi(usrnum, NULL);
    	rstmbk();
    	usrptr->flags &= ~NOGLOB;
    	My_Outprf(usrnum);
    	return (0);
    }*/
    if (glob->lockey[0] && !haskey(glob->lockey)) {
      prfmsg(MSG140);
      btuchi(usrnum, NULL);
      rstmbk();
      usrptr->flags &= ~NOGLOB;
      My_Outprf(usrnum);
      return (0);
    }

    if (!(usaptr->ansifl & ANSON)) {
      prfmsg(MSG141);
      btuchi(usrnum, NULL);
      rstmbk();
      usrptr->flags &= ~NOGLOB;
      My_Outprf(usrnum);
      return (0);
    }

    if (STATE != NEWGUY_PROMPT) {
      if (STATE == IBM_PROMPT) {
        if (margc != 0) {
          if (sameto(margv[0], "YES")) {
            glob->ibm[usrnum] = TRUE;
          } else if (sameto(margv[0], "No")) {
            glob->ibm[usrnum] = FALSE;
          } else if (sameas(margv[0], CANCEL)) {
            btumil(usrnum, 0);
            rstrxf();
            My_Outprf(usrnum);
            btuchi(usrnum, NULL);
            rstmbk();
            usrptr->flags &= ~NOGLOB;
            return (0);
          }
        }
      } else {
        glob->ibm[usrnum] = -1;
      }

      Greet_User();

      if (STATE == NEWGUY_PROMPT || STATE == IBM_PROMPT) {
        rstmbk();
        My_Outprf(usrnum);
        return (TRUE);
      }
    } else {
      if (margc == 0 || sameas(margv[0], CANCEL)) {
        btumil(usrnum, 0);
        rstrxf();
        My_Outprf(usrnum);
        btuchi(usrnum, NULL);
        rstmbk();
        usrptr->flags &= ~NOGLOB;
        return (0);
      }

      Greet_User();

      if (STATE == NEWGUY_PROMPT) {
        rstmbk();
        My_Outprf(usrnum);
        return (TRUE);
      }
    }
  } else if (STATE == MAIN_PROMPT && MY_PORT->autojack) {
    Jack_In();
  } else if (STATE == RESET_PROMPT) {
    Clear_Display_Window();

    if (margc && sameas(margv[0], ANS_YES)) {
      prfmsg(MSG887);
      Sign_Me_Off();
      glob->reset = TRUE;
    } else {
      STATE = MAIN_PROMPT;
      Tourney_Services();
    }
  } else if (STATE == COMBAT_PROMPT) {
    if (margc && sameas(margv[0], ANS_YES)) {
      MY_PORT->enemy[MY_PORT->misc] = TRUE;
      prfmsg(MSG896);
    } else if (margc && sameas(margv[0], ANS_NO)) {
      MY_PORT->enemy[MY_PORT->misc] = FALSE;
    }

    STATE = MAIN_PROMPT;
  } else if (STATE == WINNERS_PROMPT) {
    Previous_Winners();
  } else if (STATE == DOWNLOADING) {
    MY_PORT->downloading = FALSE;
    MY_PORT->prompt_y = PROMPT_Y;
    MY_PORT->prompt_x = 1;
    Redraw();
    STATE = MAIN_PROMPT;
  } else if (STATE == MIMIC_PROMPT) {
    if (margc == 0) {
      STATE = MAIN_PROMPT;
    } else {
      strncpy(MY_PORT->mimic, margv[0], NAME_SIZE - 1);
      MY_PORT->mimic[NAME_SIZE - 1] = 0;
      Prompt(getmsg(MSG86), TARGET_GUY_PROMPT);
    }
  } else if (STATE == TARGET_GUY_PROMPT) {
    if (margc == 0) {
      STATE = MAIN_PROMPT;
    } else {
      MY_PORT->target_s = Find_Guy(margv[0]);

      if (MY_PORT->target_s == -1) {
        STATE = MAIN_PROMPT;
      } else {
        Prompt(getmsg(MSG88), DIRECTION_PROMPT);
      }
    }
  } else if (STATE == GANG_PROMPT1) {
    if (margc) {
      Gang_Services1();
    } else {
      Clear_Display_Window();
      STATE = MAIN_PROMPT;
    }
  } else if (STATE == NOMORE_GANG_PROMPT || STATE == NOMORE_MBR_PROMPT) {
    Clear_Display_Window();
    Gang_Menu();
  } else if (STATE == GANG_COUNT1) {
    if (margc == 0) {
      STATE = MAIN_PROMPT;
    } else {
      MY_PORT->misc = atoi(margv[0]);
      Prompt(getmsg(MSG905), GANG_COUNT2);
    }
  } else if (STATE == GANG_COUNT2) {
    if (margc) {
      i = atoi(margv[0]);

      if (i && Read_Gang(MY_PORT->misc)) {
        gang->members = i;
        Write_Gang(MY_PORT->misc);
      }
    }

    STATE = MAIN_PROMPT;
  } else if (STATE == TOURNEY_PROMPT) {
    Tourney_Services();
  } else if (STATE == PHASE_PROMPT1 || STATE == PHASE_PROMPT2) {
    Phase_Change();
  } else if
  (
    STATE == TOURNEY_EDIT_PROMPT1 ||
    STATE == TOURNEY_EDIT_PROMPT2 ||
    STATE == TOURNEY_EDIT_PROMPT3 ||
    STATE == TOURNEY_EDIT_PROMPT4
  ) {
    Tourney_Edit();

    if (MY_PORT->editing) {
      rstmbk();
      return (TRUE);
    }
  } else if (STATE == GANG_INFO_PROMPT) {
    MY_PORT->prompt_y = PROMPT_Y;
    MY_PORT->prompt_x = 1;
    Redraw();
    prf(NL);
    Gang_Menu();
  } else if (STATE == MORE_GANG_PROMPT) {
    List_Gangs();
  } else if (STATE == MEMBERS_PROMPT || STATE == MORE_MBR_PROMPT) {
    Gang_Members();
  } else if (STATE == RESIGN_PROMPT && margc) {
    if (sameas(margv[0], ANS_YES)) {
      Resign();
    } else {
      Gang_Menu();
    }
  } else if (STATE == INVITE_PROMPT) {
    Invite();
  } else if
  (
    STATE == GANG_BANK_PROMPT ||
    STATE == GANG_DEPOSIT_PROMPT ||
    STATE == GANG_WITHDRAW_PROMPT ||
    STATE == GANG_LEDGER_PROMPT ||
    STATE == GANG_CODE_PROMPT ||
    STATE == GANG_SUMMARY_PROMPT
  ) {
    Gang_Bank();
  } else if
  (
    STATE == SERVER_PROMPT ||
    STATE == SERVER_CODE_PROMPT ||
    STATE == GANG_LOCK_PROMPT1
  ) {
    Gang_Server();
  } else if (STATE == KICK_PROMPT) {
    Kick_Out();
  } else if (STATE == SET_RANK_PROMPT1 || STATE == SET_RANK_PROMPT2) {
    Set_Rank();
  } else if (STATE == NEW_LEADER_PROMPT) {
    New_Leader();
  } else if (STATE == GANG_NAME_PROMPT) {
    if (margc) {
      Rename_Gang();
    } else {
      Gang_Menu();
    }
  } else if (margc && STATE == ACCEPT_PROMPT) {
    Accept();
  } else if (STATE == GANG_PROMPT2) {
    if (margc) {
      Gang_Services2();

      if (MY_PORT->editing) {
        rstmbk();
        return (TRUE);
      }
    } else {
      Clear_Display_Window();
      STATE = MAIN_PROMPT;
    }
  } else if (STATE == TARGET_CONST_PROMPT) {
    if (margc == 0) {
      STATE = MAIN_PROMPT;
    } else {
      Find_Target_Program
      (
        margv[0],
        &MY_PORT->target_s,
        &MY_PORT->target_c
      );

      if (MY_PORT->target_s == -1) {
        if (STATE != TACTICAL_PROMPT) {
          STATE = MAIN_PROMPT;
        }
      } else {
        Prompt(getmsg(MSG88), DIRECTION_PROMPT);
      }
    }
  } else if (STATE == DIRECTION_PROMPT) {
    if (margc == 0) {
      STATE = MAIN_PROMPT;
    } else if
    (
      sameas(margv[0], ANS_NO) ||
      sameas(margv[0], "S") ||
      sameas(margv[0], "E") ||
      sameas(margv[0], "W")
    ) {
      if (MY_DECK->memory[MY_PORT->m_index][MY_PORT->p_index].index != PROBE) {
        i = 1;
      } else {
        i = MY_DECK->memory[MY_PORT->m_index][MY_PORT->p_index].strength;
      }

      if (MY_PORT->access_level == SYSTEM_VIEW) {
        MY_PORT->target_type = Find_System_Space
                               (
                                 margv[0],
                                 &MY_PORT->target_x,
                                 &MY_PORT->target_y,
                                 i
                               );
      } else {
        MY_PORT->target_type = Find_City_Space
                               (
                                 margv[0],
                                 &MY_PORT->target_x,
                                 &MY_PORT->target_y,
                                 i
                               );
      }

      Load_Program();
    } else {
      STATE = MAIN_PROMPT;
    }
  } else if (STATE == GET_MSG_BODY_PROMPT) {
    if (margc) {
      Post_Private();
    }
  } else if (STATE == WHO_PROMPT) {
    Who();
  } else if (STATE == FREE_BODY_PROMPT) {
    if (margc) {
      Post_FreeNet();
    }
  } else if (STATE == GANG_BODY_PROMPT) {
    if (margc) {
      Post_Gang();
    }
  } else if (STATE == POST_GANG_PROMPT1) {
    if (margc && !sameas(margv[0], CANCEL)) {
      i = INUM(margv[0]);

      if (Read_Gang(i)) {
        MY_PORT->gang = i;
        Post_Gang();
      } else {
        prfmsg(MSG742, i);
        Gang_Menu();
      }
    } else {
      Gang_Menu();
    }
  } else if (STATE == POST_GANG_PROMPT2) {
    if (margc) {
      if (sameas(margv[0], CANCEL)) {
        Gang_Menu();
      } else {
        i = INUM(margv[0]);

        if (Read_Gang(i)) {
          MY_PORT->gang = i;
          Post_Gang();
        } else {
          prfmsg(MSG742, i);
          Gang_Menu();
        }
      }
    } else {
      MY_PORT->gang = me.gang;
      Post_Gang();
    }
  } else if (STATE == NEXT_MSG_PROMPT || STATE == ERASE_MSG_PROMPT) {
    Check_Mail();
  } else if (STATE == FREENET_PROMPT1 || STATE == FREENET_PROMPT2) {
    Read_FreeNet();
  } else if (STATE == GANG_MAIL_PROMPT1 || STATE == GANG_MAIL_PROMPT2) {
    Read_Gang_Mail();
  } else if (STATE == FREENET_PROMPT3) {
    Clear_Display_Window();
    STATE = MAIN_PROMPT;
    Mail();
  } else if (STATE == GANG_MAIL_PROMPT3) {
    Clear_Display_Window();
    STATE = MAIN_PROMPT;
    Gang_Menu();
  } else if (STATE == MAIL_PROMPT) {
    if (margc == 0) {
      Clear_Display_Window();
      STATE = MAIN_PROMPT;
    } else {
      Mail();
    }
  } else if (STATE == WRITE_MAIL_PROMPT) {
    if (margc == 0) {
      STATE = MAIN_PROMPT;
      Mail();
    } else {
      Write_Email();
    }
  } else if (STATE == TACTICAL_PROMPT) {
    if (margc && MY_PORT->access_level != IDLE) {
      Run_Tactical(margv[0]);
    }

    if (STATE != DIRECTION_PROMPT) {
      STATE = MAIN_PROMPT;
    }

    MY_PORT->tstring[0] = 0;
    Look("X");
  } else if (STATE == HYDRA_PROMPT) {
    Hydra_Select();
    STATE = MAIN_PROMPT;
  } else if (STATE == DUMP_FILE_PROMPT) {
    Dump_File(NULL);
  } else if (STATE == MOVE_PROMPT) {
    Clear_Display_Window();

    if (margc == 0) {
      Memory_Services();
    } else {
      Memory_Move();

      if (STATE != MOVE_PROMPT2) {
        Memory_Services();
      }
    }
  } else if (STATE == MOVE_PROMPT2) {
    Clear_Display_Window();

    if (margc) {
      i = Find_Memory(margv[0], &MY_PORT->dest_s, &MY_PORT->dest_m);

      if (i) {
        Memory_Move();
      }
    }

    Memory_Services();
  } else if (STATE == ERASE_PROMPT) {
    Clear_Display_Window();

    if (margc) {
      Memory_Erase();
    }

    Memory_Services();
  } else if (STATE == ZIP_PROMPT) {
    Clear_Display_Window();

    if (margc) {
      Memory_Zip();
    }

    Memory_Services();
  } else if (STATE == FENCE_PROMPT) {
    if (margc == 0) {
      STATE = MAIN_PROMPT;
    } else {
      Fence_Check(1);
    }
  } else if (STATE == READ_PROMPT) {
    Clear_Display_Window();

    if (margc) {
      Memory_Read();
    }

    Memory_Services();
  } else if (STATE == EDIT_HUMAN_PROMPT) {
    if (margc == 0) {
      STATE = MAIN_PROMPT;
    } else {
      Edit_Player();

      if (MY_PORT->editing) {
        rstmbk();
        return (TRUE);
      }
    }
  } else if (STATE == LOCK_PROMPT) {
    Clear_Display_Window();

    if (margc == 0) {
      if (MY_PORT->unlocking) {
        MY_PORT->unlocking = FALSE;
        Memory_Services();
      } else {
        STATE = MAIN_PROMPT;
      }
    } else {
      File_Utility(1);
    }
  } else if (STATE == MISSION_PROMPT) {
    if (margc && sameas(margv[0], ANS_YES)) {
      New_Mission();
    }

    STATE = MAIN_PROMPT;
  } else if (STATE == BUY_PROMPT) {
    if (margc == 0 || sameas(margv[0], CANCEL)) {
      Clear_Display_Window();
      Offline_Services();
    } else {
      Buying();

      if (STATE != BUY_CPU_PROMPT1) {
        Buy_Hardware();
        Service_Display();
      }
    }
  } else if (STATE == SELL_PROMPT) {
    if (margc == 0 || sameas(margv[0], CANCEL)) {
      Clear_Display_Window();
      Offline_Services();
    } else {
      Selling();
      Sell_Hardware();
      Service_Display();
    }
  } else if (STATE == DOCTOR_PROMPT) {
    if (margc) {
      Hospital();
    }

    Offline_Services();
  } else if (STATE == BUY_CPU_PROMPT1) {
    if (margc == 0) {
      Buy_Hardware();
    } else {
      MY_PORT->misc = INUM(margv[0]);

      if (MY_PORT->misc == 0) {
        Buy_Hardware();
      } else if (MY_DECK->speed && MY_PORT->misc != MY_DECK->speed) {
        Prompt(getmsg(MSG142), BUY_CPU_PROMPT2);
      } else {
        Prompt(getmsg(MSG143), BUY_CPU_PROMPT3);
      }
    }
  } else if (STATE == BUY_CPU_PROMPT2) {
    if (margc != 0 && sameas(margv[0], ANS_YES)) {
      Prompt(getmsg(MSG143), BUY_CPU_PROMPT3);
    } else {
      Buy_Hardware();
    }
  } else if (STATE == BUY_CPU_PROMPT3) {
    if (margc != 0) {
      CPU_Bought();
    }

    Buy_Hardware();
    Service_Display();
  } else if
  (
    STATE == DESIGN_X1_PROMPT ||
    STATE == DESIGN_Y1_PROMPT ||
    STATE == DESIGN_X2_PROMPT ||
    STATE == DESIGN_Y2_PROMPT ||
    STATE == SYS_NAME_PROMPT ||
    STATE == MOVE_MEM_PROMPT ||
    STATE == MOVE_CPU_PROMPT ||
    STATE == SYS_PW_PROMPT
  ) {
    Design_Deck();
  } else if
  (
    STATE == PDESIGN_X1_PROMPT ||
    STATE == PDESIGN_Y1_PROMPT ||
    STATE == PDESIGN_X2_PROMPT ||
    STATE == PDESIGN_Y2_PROMPT ||
    STATE == PMEMORY_PROMPT ||
    STATE == PMEMORY_PROMPT2 ||
    STATE == PERASE_PROMPT ||
    STATE == PCHANGE_PROMPT ||
    STATE == PADD_PROMPT
  ) {
    Design_Perm();

    if (MY_PORT->editing) {
      rstmbk();
      return (TRUE);
    }
  } else if (STATE == DESIGN_PROMPT) {
    if (margc) {
      if (sameas(margv[0], CANCEL)) {
        MY_PORT->prompt_y = PROMPT_Y;
        MY_PORT->prompt_x = 1;
        Redraw();
        STATE = MAIN_PROMPT;
      } else {
        Design_Deck();
      }
    }
  } else if (STATE == PDESIGN_PROMPT) {
    if (margc) {
      Design_Perm();

      if (STATE == -2) {
        Display_Edit_City();
        STATE = EDIT_SYSTEM_PROMPT;
        Edit_World();
      } else if (MY_PORT->editing) {
        rstmbk();
        return (TRUE);
      }
    }
  } else if (STATE == LEDGER_PROMPT) {
    MY_PORT->prompt_y = PROMPT_Y;
    MY_PORT->prompt_x = 1;
    Redraw();
    prf(NL);
    Offline_Services();
  } else if (STATE == TSTATUS_PROMPT) {
    MY_PORT->prompt_y = PROMPT_Y;
    MY_PORT->prompt_x = 1;
    Redraw();
    prf(NL);
    Tourney_Services();
  } else if (STATE == ANY_KEY_PROMPT) {
    MY_PORT->prompt_y = PROMPT_Y;
    MY_PORT->prompt_x = 1;
    Redraw();
    STATE = MAIN_PROMPT;
  } else if (STATE == PROGRAMS_PROMPT) {
    if (margc != 0 && sameas(margv[0], CANCEL)) {
      STATE = MAIN_PROMPT;
    } else {
      List_Programs(1);
    }
  } else if (STATE == INSTALL_PROMPT) {
    if (margc != 0 && sameas(margv[0], CANCEL)) {
      STATE = MAIN_PROMPT;
    } else {
      Installable_Programs(1);
    }
  } else if (STATE == DOCS_PROMPT) {
    if (margc != 0 && sameas(margv[0], CANCEL)) {
      STATE = MAIN_PROMPT;
    } else {
      List_Documents(1);
    }
  } else if (STATE == WHERE_PROMPT) {
    if (margc != 0 && sameas(margv[0], ANS_NO)) {
      STATE = MAIN_PROMPT;
    } else {
      if (margc == 0) {
        Clear_Display_Window();
      }

      Where(1);
    }
  } else if (STATE == EYE_PROMPT) {
    Redraw();
    STATE = MAIN_PROMPT;
  } else if
  (
    STATE == X_PROMPT ||
    STATE == Y_PROMPT ||
    STATE == PICK_CITY_PROMPT1 ||
    STATE == PICK_CITY_PROMPT2 ||
    STATE == CITY_OPTION_PROMPT ||
    STATE == CITY_NAME_PROMPT ||
    STATE == SECURITY_PROMPT ||
    STATE == LANDMASS_PROMPT ||
    STATE == X2_PROMPT ||
    STATE == Y2_PROMPT ||
    STATE == NODE_TYPE_PROMPT ||
    STATE == SYSTEM_NAME_PROMPT ||
    STATE == PERM_NODE_PROMPT ||
    STATE == EDIT_SYSTEM_PROMPT ||
    STATE == SYS_OPTION_PROMPT
  ) {
    if (margc == 0) {
      STATE = MAIN_PROMPT;
    } else {
      Edit_World();

      if (STATE == -1) {
        STATE = PICK_CITY_PROMPT1;
        strcpy(margv[0], ANS_YES);
        Edit_World();
        My_Outprf(usrnum);
        rstmbk();
        return (TRUE);
      } else if (STATE == -2) {
        STATE = EDIT_SYSTEM_PROMPT;
        Edit_World();
      } else if (MY_PORT->cycling) {
        My_Outprf(usrnum);
        rstmbk();
        return (TRUE);
      }
    }

    if (STATE == MAIN_PROMPT) {
      Main_Prompt();
      Clear_Net_Display(CLEAR_ALL);

      if (MY_PORT->access_level != IDLE) {
        Look("X");
      } else {
        Update_Location();
      }
    }
  } else if (STATE == STREET_PROMPT) {
    if (margc == 0) {
      STATE = MAIN_PROMPT;
    } else {
      Jacking_In();
    }
  } else if (STATE == STREET_CODE_PROMPT) {
    if (margc == 0) {
      STATE = MAIN_PROMPT;
    } else {
      Street_Jack_In();
    }
  } else if (STATE == JACKIN_PROMPT) {
    if (margc == 0 || sameas(margv[0], CANCEL)) {
      STATE = MAIN_PROMPT;
    } else if (sameas(margv[0], "I")) {
      if (me.internet.credit_rating == 0) {
        STATE = MAIN_PROMPT;
        prfmsg(MSG144);
      } else {
        MY_PORT->link_type = INTERNET_LINK;
        MY_PORT->ld_time = 0;
        Jacking_In();
      }
    } else if (sameas(margv[0], "S")) {
      Street_Jack_In();
    } else {
      STATE = MAIN_PROMPT;
    }
  } else if (margc && STATE == LD_PROMPT) {
    if (sameas(margv[0], ANS_YES)) {
      if (No_Local_Internet()) {
        STATE = MAIN_PROMPT;
      } else {
        Prompt(getmsg(MSG145), NUM_MOVES_PROMPT);
      }
    } else if (sameas(margv[0], ANS_NO)) {
      command = MY_PORT->last_cmd;
      City_Move();
      STATE = MAIN_PROMPT;
    }
  } else if (margc && STATE == SYS_LD_PROMPT) {
    if (sameas(margv[0], ANS_YES)) {
      if (No_Local_Internet()) {
        STATE = MAIN_PROMPT;
      } else {
        Prompt(getmsg(MSG145), SYS_LDMOVES_PROMPT);
      }
    } else if (sameas(margv[0], ANS_NO)) {
      command = MY_PORT->last_cmd;
      System_Move();
      STATE = MAIN_PROMPT;
    }
  } else if (STATE == NUM_MOVES_PROMPT) {
    if (margc == 0) {
      STATE = MAIN_PROMPT;
    } else {
      MY_PORT->spaces = INUM(margv[0]);

      if (MY_PORT->spaces < 1) {
        prfmsg(MSG542);
        STATE = MAIN_PROMPT;
      } else if (MY_PORT->spaces > 5 && !me.perm[GAMEOP_PERM]) {
        prfmsg(MSG799);
        STATE = MAIN_PROMPT;
      } else if
      (
        MY_PORT->spaces == 5 &&
        His_Rank(me.score, me.won) < 21 &&
        !me.perm[GAMEOP_PERM]
      ) {
        prfmsg(MSSG800, 21);
        STATE = MAIN_PROMPT;
      } else if
      (
        MY_PORT->spaces == 4 &&
        His_Rank(me.score, me.won) < 11 &&
        !me.perm[GAMEOP_PERM]
      ) {
        prfmsg(MSSG800, 11);
        STATE = MAIN_PROMPT;
      } else if (!(Net_Move_Check(MY_PORT->spaces, MY_PORT->last_cmd))) {
        STATE = MAIN_PROMPT;
      } else {
        if (!me.perm[FREE_PERM]) {
          prfmsg(MSG146, MY_PORT->spaces * glob->ldcost);
        }

        Prompt(getmsg(MSG147), LD_TYPE_PROMPT);
      }
    }
  } else if (STATE == SYS_LDMOVES_PROMPT) {
    if (margc != 0) {
      MY_PORT->spaces = INUM(margv[0]);

      if (MY_PORT->spaces < 1) {
        prfmsg(MSG542);
      } else if (MY_PORT->spaces > 5 && !me.perm[GAMEOP_PERM]) {
        prfmsg(MSG799);
      } else if
      (
        MY_PORT->spaces == 5 &&
        His_Rank(me.score, me.won) < 21 &&
        !me.perm[GAMEOP_PERM]
      ) {
        prfmsg(MSSG800, 21);
      } else if
      (
        MY_PORT->spaces == 4 &&
        His_Rank(me.score, me.won) < 11 &&
        !me.perm[GAMEOP_PERM]
      ) {
        prfmsg(MSSG800, 11);
      } else if (Net_Move_Check(MY_PORT->spaces, MY_PORT->last_cmd)) {
        prfmsg(MSG148);
        command = MY_PORT->last_cmd;
        Net_Move(MY_PORT->spaces, LD_LINK);
      }
    }

    STATE = MAIN_PROMPT;
  } else if (margc && STATE == LD_TYPE_PROMPT) {
    if (sameas(margv[0], "L")) {
      Prompt(getmsg(MSG149), LD_CODE_PROMPT);
    } else if (sameas(margv[0], "C")) {
      Crash_LD_Link();
    }
  } else if (margc && STATE == LD_CODE_PROMPT) {
    i = Find_Internet_User(a_human, margv[0]);

    if (i < 0) {
      prfmsg(MSG150);
      STATE = MAIN_PROMPT;
    } else if
    (
      i != me.index &&
      (
        His_Rank(me.score, me.won) < glob->pvprank ||
        His_Rank(a_human->score, a_human->won) < glob->pvprank
      )
    ) {
      prfmsg(MSG807);
      STATE = MAIN_PROMPT;
    } else if
    (
      i != me.index &&
      glob->outlaw &&
      (!me.outlaw || !a_human->outlaw)
    ) {
      prfmsg(MSG807);
      STATE = MAIN_PROMPT;
    } else {
      if (a_human->internet.credit_rating == 0) {
        if (sameas(a_human->userid, me.userid)) {
          prfmsg(MSG144);
        } else {
          prfmsg(MSG151);
        }
      } else if (a_human->internet.owed >= glob->netdebt) {
        if (sameas(a_human->userid, me.userid)) {
          prfmsg(MSG144A);
        } else {
          prfmsg(MSG151A);
        }
      } else {
        if (!a_human->perm[FREE_PERM]) {
          Add_Internet_Record
          (
            a_human,
            (long) MY_PORT->spaces * (long) glob->ldcost,
            MY_PORT->net_x, MY_PORT->net_y,
            me.alias,
            "LD Jump"
          );

          if (sameas(a_human->userid, me.userid)) {
            prfmsg(MSG152, MY_PORT->spaces * glob->ldcost);
          } else {
            prfmsg(MSG153, MY_PORT->spaces * glob->ldcost);
          }
        }

        prfmsg(MSG148);
        command = MY_PORT->last_cmd;
        Net_Move(MY_PORT->spaces, LD_LINK);
      }

      STATE = MAIN_PROMPT;
    }
  } else if (STATE == RELOCATE_PROMPT) {
    short   go_service_menu = FALSE;

    if (me.start_x >= 0 && me.start_y >= 0) {
      go_service_menu = TRUE;
    }

    Clear_Net_Display(CLEAR_ALL);

    if (margc && !sameas(margv[0], CANCEL)) {
      Relocate(RELOCATE_PROMPT);
    } else {
      MY_PORT->autojack = FALSE;
    }

    if (go_service_menu) {
      Offline_Services();
    } else {
      STATE = MAIN_PROMPT;
    }
  } else if (STATE == FLEE_PROMPT) {
    if (margc && sameas(margv[0], ANS_YES)) {
      me.renting = FALSE;
      me.arrest_time = 0;
    }

    Offline_Services();
  } else if (STATE == CHANGE_ALIAS_PROMPT) {
    if (margc != 0) {
      Change_Alias();
    }

    Offline_Services();
  } else if (STATE == CHANGE_BANK_PROMPT) {
    if (margc != 0) {
      strcpy(str, margv[0]);
      str[CODE_SIZE - 1] = 0;
      i = Find_Bank_User(a_human, str);

      if (i >= 0) {
        prfmsg(MSG154, str);
      } else {
        strcpy(me.bank.code, str);
        Write_My_Rec(EXISTS);
      }
    }

    Offline_Services();
  } else if (STATE == CHANGE_NET_PROMPT) {
    if (margc != 0) {
      strcpy(str, margv[0]);
      str[CODE_SIZE - 1] = 0;
      i = Find_Internet_User(a_human, str);

      if (i >= 0) {
        if (!me.perm[GAMEOP_PERM]) {
          prfmsg(MSG154, str);
        } else {
          prfmsg(MSG154A, str, a_human->alias);
        }
      } else {
        strcpy(me.internet.code, str);
        Write_My_Rec(EXISTS);
      }
    }

    Offline_Services();
  } else if (STATE == SERVICES_PROMPT) {
    Clear_Display_Window();

    if (margc == 0 || sameas(margv[0], CANCEL)) {
      Update_Location();
      STATE = MAIN_PROMPT;
    } else if (sameas(margv[0], "A")) {
      Prompt(getmsg(MSG155), CHANGE_BANK_PROMPT);
    } else if (sameas(margv[0], "B")) {
      if
      (
        !me.perm[GAMEOP_PERM] &&
        His_Rank(me.score, me.won) < glob->xferrank
      ) {
        prfmsg(M156, glob->xferrank);
        Offline_Services();
      } else {
        Prompt(getmsg(MSG649), TRANSFER0_PROMPT);
      }
    } else if (sameas(margv[0], "C")) {
      Bank_Records();
    } else if (sameas(margv[0], "D")) {
      Prompt(getmsg(MSG158), CHANGE_NET_PROMPT);
    } else if (sameas(margv[0], "E")) {
      Internet_Records();
    } else if (sameas(margv[0], "F")) {
      Buy_Hardware();
    } else if (sameas(margv[0], "K")) {
      Sell_Hardware();
    } else if (sameas(margv[0], "G")) {
      if (!me.damage) {
        prfmsg(MSG159);
        Offline_Services();
      } else {
        Prompt(getmsg(MSG160), DOCTOR_PROMPT);
      }
    } else if (sameas(margv[0], "H")) {
      Relocate(0);
    } else if (sameas(margv[0], "I")) {
      Rent_Flat();
      Offline_Services();
    } else if (sameas(margv[0], "J")) {
      if (!me.renting) {
        prfmsg(MSG162);
        Offline_Services();
      } else {
        Prompt(getmsg(MSG163), FLEE_PROMPT);
      }
    } else if (sameas(margv[0], "L")) {
      if (time(0) < me.last_name_change) {
        me.last_name_change = 0;
      }

      n = time(0) - me.last_name_change;

      if (n < glob->namechng && !me.perm[GAMEOP_PERM]) {
        n = glob->namechng - n;

        if (n <= 120) {
          sprintf(str, getmsg(MSG794), n);
        } else if (n < 7200) {
          sprintf(str, getmsg(MSG795), n / 60);
        } else {
          sprintf(str, getmsg(MSG796), n / 3600);
        }

        prf(str);
        Offline_Services();
      } else {
        Prompt(getmsg(MSG164), CHANGE_ALIAS_PROMPT);
      }
    } else {
      Offline_Services();
    }
  } else if (STATE == RETRIEVE_PROMPT1) {
    if (margc == 0 || sameas(margv[0], CANCEL)) {
      STATE = MAIN_PROMPT;
    } else {
      i = Find_Chip(margv[0], &MY_PORT->source_m);

      if (i == 0) {
        Backup_Services();
      } else {
        i = Memory_Contents(me.backup[MY_PORT->source_m], 1);

        if (i == 0) {
          Backup_Services();
        } else {
          Prompt(getmsg(MSG855), RETRIEVE_PROMPT);
        }
      }
    }
  } else if (STATE == RETRIEVE_PROMPT) {
    Clear_Display_Window();

    if (margc == 0 || sameas(margv[0], CANCEL)) {
      MY_PORT->source_m = -1;
      Backup_Services();
    } else if (sameas(margv[0], "R")) {
      i = Memory_Contents(me.backup[MY_PORT->source_m], 1);

      if (i == 0) {
        Backup_Services();
      } else {
        sprintf(str, getmsg(MSG171), i);
        Prompt(str, RETRIEVE_PROMPT2);
      }
    } else if (sameas(margv[0], "E")) {
      i = Memory_Contents(me.backup[MY_PORT->source_m], 1);

      if (i == 0) {
        Backup_Services();
      } else {
        sprintf(str, getmsg(MSG856), i);
        Prompt(str, CHIP_ERASE_PROMPT);
      }
    } else {
      Backup_Services();
    }
  } else if (STATE == CHIP_ERASE_PROMPT) {
    Clear_Display_Window();

    if (margc) {
      m = Num_Files(me.backup[MY_PORT->source_m]);
      i = INUM(margv[0]);

      if (i == 0 || i > m) {
        prfmsg(MSG134, m);
      } else {
        --i;

        if (me.backup[MY_PORT->source_m][i].type == DOCUMENT) {
          Scored(myp, me.backup[MY_PORT->source_m][i].class, 1);
          prf("\x1B[%d;1H", DISP_Y);
        } else if (me.backup[MY_PORT->source_m][i].value) {
          Scored(myp, 1, 1);
          prf("\x1B[%d;1H", DISP_Y);
        }

        Remove_From_Backup(MY_PORT->source_m, i);
      }
    }

    Backup_Services();
  } else if (STATE == RETRIEVE_PROMPT2) {
    Clear_Display_Window();

    if (margc == 0) {
      Backup_Services();
    } else {
      if (!Memory_Retrieve(STATE)) {
        Backup_Services();
      } else {
        Memory_Summary();
        sprintf(str, getmsg(MSG172), glob->dirs);
        Prompt(str, RETRIEVE_PROMPT3);
      }
    }
  } else if (STATE == RETRIEVE_PROMPT3) {
    Clear_Display_Window();

    if (margc) {
      i = Find_Memory(margv[0], &MY_PORT->dest_s, &MY_PORT->dest_m);

      if (i) {
        Memory_Retrieve(STATE);
      }
    }

    Backup_Services();
  } else if (STATE == RUN_PROMPT) {
    if (margc == 0) {
      STATE = MAIN_PROMPT;
    } else {
      i = INUM(margv[0]);

      if (i < 1 || i > MY_PORT->progs) {
        STATE = MAIN_PROMPT;
      } else {
        --i;
        Ran_Program
        (
          MY_PORT->proglist[i].m_index,
          MY_PORT->proglist[i].p_index,
          0, (char *) NULL
        );
      }
    }
  } else if (STATE == ERASE_DOC_PROMPT) {
    Clear_Display_Window();

    if (margc) {
      i = INUM(margv[0]);

      if (i > 0 && i <= MY_PORT->progs) {
        --i;
        m = MY_PORT->proglist[i].m_index;
        p = MY_PORT->proglist[i].p_index;

        if (MY_DECK->memory[m][p].type == DOCUMENT) {
          if (MY_DECK->memory[m][p].lock) {
            prfmsg(MSG82, MY_DECK->memory[m][p].name);
          } else {
            Scored(myp, MY_DECK->memory[m][p].class, 1);
            Remove_From_Memory(myp, m, p);
          }
        }
      }
    }

    List_Documents(0);
  } else if (STATE == READ_DOC_PROMPT) {
    Clear_Display_Window();

    if (margc) {
      i = INUM(margv[0]);

      if (i >= 0 && i <= MY_PORT->progs) {
        --i;
        m = MY_PORT->proglist[i].m_index;
        p = MY_PORT->proglist[i].p_index;

        if (MY_DECK->memory[m][p].type == DOCUMENT) {
          if (MY_DECK->memory[m][p].lock) {
            prfmsg(MSG82, MY_DECK->memory[m][p].name);
          } else if (MY_DECK->memory[m][p].packed) {
            prfmsg(MSG83, MY_DECK->memory[m][p].name);
          } else {
            Display_Doc(&MY_DECK->memory[m][p]);
          }
        }
      }
    }

    List_Documents(0);
  } else if (STATE == IMPROVE_PROMPT) {
    Clear_Display_Window();

    if (margc == 0) {
      STATE = MAIN_PROMPT;
    } else {
      i = INUM(margv[0]);

      if (i < 1 || i > MY_PORT->progs) {
        STATE = MAIN_PROMPT;
      } else {
        --i;
        MY_PORT->m_index = MY_PORT->proglist[i].m_index;
        MY_PORT->p_index = MY_PORT->proglist[i].p_index;
        Improve_Program(0);
      }
    }
  } else if (STATE == IMPROVE_PROMPT2) {
    if (margc != 0 && sameas(margv[0], ANS_YES)) {
      Improve_Program(1);
    }

    STATE = MAIN_PROMPT;
  } else if (STATE == CONFIGURE_PROMPT) {
    Clear_Display_Window();

    if (margc == 0) {
      STATE = MAIN_PROMPT;
    } else {
      i = INUM(margv[0]);

      if (i < 1 || i > MY_PORT->progs) {
        STATE = MAIN_PROMPT;
      } else {
        --i;
        MY_PORT->m_index = MY_PORT->proglist[i].m_index;
        MY_PORT->p_index = MY_PORT->proglist[i].p_index;
        Configure_Program();
      }
    }
  } else if (STATE == STORE_PROMPT2) {
    Clear_Display_Window();

    if (margc == 0) {
      Memory_Services();
    } else {
      if (!Memory_Store(STATE)) {
        Memory_Services();
      } else {
        Backup_Summary();
        sprintf(str, getmsg(MSG174), me.backup_chips);
        Prompt(str, STORE_PROMPT3);
      }
    }
  } else if (STATE == STORE_PROMPT3) {
    Clear_Display_Window();

    if (margc) {
      i = Find_Chip(margv[0], &MY_PORT->dest_m);

      if (i) {
        Memory_Store(STORE_PROMPT3);
      }
    }

    Memory_Services();
  } else if (STATE == MEMORY_PROMPT) {
    if (margc == 0 || sameas(margv[0], CANCEL)) {
      STATE = MAIN_PROMPT;
    } else {
      i = Find_Memory(margv[0], &MY_PORT->source_s, &MY_PORT->source_m);

      if (i == 0) {
        MY_PORT->source_s = MY_PORT->source_m = -1;
        Memory_Services();
      } else {
        i = Memory_Contents
            (
              Sys[MY_PORT->source_s]->memory[MY_PORT->source_m],
              1
            );

        if (i == 0) {
          MY_PORT->source_s = MY_PORT->source_m = -1;
          Memory_Services();
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
        }
      }
    }
  } else if (STATE == LOCKER_PROMPT) {
    if (margc == 0 || sameas(margv[0], CANCEL)) {
      STATE = MAIN_PROMPT;
    } else {
      i = Find_Memory(margv[0], &MY_PORT->source_s, &MY_PORT->source_m);

      if (i == 0) {
        STATE = MAIN_PROMPT;
      } else {
        File_Utility(0);
      }
    }
  } else if (STATE == MEMORY_PROMPT2) {
    Clear_Display_Window();

    if (margc == 0) {
      MY_PORT->source_s = MY_PORT->source_m = -1;
      Memory_Services();
    } else if (sameas(margv[0], CANCEL)) {
      MY_PORT->source_s = MY_PORT->source_m = -1;
      Memory_Services();
    } else if (sameas(margv[0], "E")) {
      Memory_Erase();
    } else if (sameas(margv[0], "R")) {
      Memory_Read();
    } else if (sameas(margv[0], "M")) {
      Memory_Move();
    } else if (sameas(margv[0], "Z")) {
      Memory_Zip();
    } else if (sameas(margv[0], "U")) {
      Memory_Unlock();
    } else if (sameas(margv[0], "S")) {
      if (MY_PORT->access_level != IDLE) {
        prfmsg(MSG184);
        Memory_Services();
      } else if (!me.chip_reader) {
        prfmsg(MSG166);
        Memory_Services();
      } else if (!me.backup_chips) {
        prfmsg(MSG167);
        Memory_Services();
      } else {
        Memory_Store(MEMORY_PROMPT2);
      }
    } else {
      Memory_Services();
    }
  } else if (STATE == TRANSFER0_PROMPT) {
    if (margc == 0) {
      Offline_Services();
    } else {
      i = Find_Bank_User(a_human, margv[0]);

      if (i < 0) {
        prfmsg(MSG179);
        Offline_Services();
      } else if
      (
        !me.perm[GAMEOP_PERM] &&
        !sameto(A_GANG, a_human->alias) &&
        His_Rank(a_human->score, a_human->won) < glob->xferrank
      ) {
        prfmsg(M156, glob->xferrank);
        Offline_Services();
      } else if
      (
        i != me.index &&
        !sameto(A_GANG, a_human->alias) &&
        (
          His_Rank(me.score, me.won) < glob->pvprank ||
          His_Rank(a_human->score, a_human->won) < glob->pvprank
        )
      ) {
        prfmsg(MSG682);
        Offline_Services();
      } else if
      (
        i != me.index &&
        !sameto(A_GANG, a_human->alias) &&
        glob->outlaw &&
        (!me.outlaw || !a_human->outlaw)
      ) {
        prfmsg(MSG682);
        Offline_Services();
      } else {
        strcpy(str, margv[0]);
        str[CODE_SIZE - 1] = 0;
        strcpy(MY_PORT->code, str);
        Prompt(getmsg(MSG157), TRANSFER1_PROMPT);
      }
    }
  } else if (STATE == TRANSFER1_PROMPT) {
    if (margc == 0) {
      Offline_Services();
    } else {
      Fix_Alias(MY_PORT->tstring);
      i = Find_Alias(a_human, MY_PORT->tstring);

      if (i < 0) {
        prfmsg(MSG176, MY_PORT->tstring);
        Offline_Services();
      } else {
        Prompt(getmsg(MSG177), TRANSFER2_PROMPT);
      }
    }
  } else if (STATE == TRANSFER2_PROMPT) {
    if (margc != 0) {
      n = LNUM(margv[0]);
      i = Find_Bank_User(a_human, MY_PORT->code);

      if (i < 0) {
        prfmsg(MSG179);
      } else {
        if (a_human->bank.balance < n) {
          prfmsg(MSG178);
        } else {
          Add_Bank_Record
          (
            a_human,
            -(n),
            me.start_x, me.start_y,
            me.alias,
            "Transfer Out"
          );

          if (me.gang && sameto(A_GANG, a_human->alias)) {
            i = INUM(&a_human->userid[5]);

            if (i == me.gang) {
              me.gang_bank -= n;
            }
          }

          i = Find_Alias(a_human, MY_PORT->tstring);

          if (i >= 0) {
            Add_Bank_Record
            (
              a_human,
              n,
              me.start_x, me.start_y,
              me.alias,
              "Transfer In"
            );

            if (me.gang && sameto(A_GANG, a_human->alias)) {
              i = INUM(&a_human->userid[5]);

              if (i == me.gang) {
                me.gang_bank += n;
              }
            }
          }
        }
      }
    }

    Offline_Services();
  } else if (STATE == STOP_PROMPT) {
    if (margc) {
      strcpy(parm1, margv[0]);
      Clear_Display_Window();
      Stop_Program(STATE);
    }

    STATE = MAIN_PROMPT;
  } else if (STATE == GO_VENDOR_PROMPT) {
    Clear_Display_Window();

    if (margc) {
      i = Find_Bank_User(a_human, margv[0]);

      if (i < 0) {
        prfmsg(MSG179);
        MY_PORT->code[0] = 0;
      } else if
      (
        i != me.index &&
        !sameto(A_GANG, a_human->alias) &&
        (
          His_Rank(me.score, me.won) < glob->pvprank ||
          His_Rank(a_human->score, a_human->won) < glob->pvprank
        )
      ) {
        prfmsg(MSG807);
        MY_PORT->code[0] = 0;
      } else if
      (
        i != me.index &&
        glob->outlaw &&
        !sameto(A_GANG, a_human->alias) &&
        (!me.outlaw || !a_human->outlaw)
      ) {
        prfmsg(MSG807);
        MY_PORT->code[0] = 0;
      } else {
        strcpy(str, margv[0]);
        str[CODE_SIZE - 1] = 0;
        strcpy(MY_PORT->code, str);
        MY_PORT->balance = a_human->bank.balance;
        Cashier_Lock(MY_PORT->sys_num);
        MY_SYS->node_type[MY_PORT->pwx][MY_PORT->pwy] = OPENED_GATE;
        Quick_Update
        (
          SYSTEM_VIEW,
          MY_PORT->net_x, MY_PORT->net_y,
          MY_PORT->city_x, MY_PORT->city_y,
          MY_PORT->pwx, MY_PORT->pwy,
          0, 0
        );
        sprintf
        (
          str,
          getmsg(MSG181),
          me.alias,
          MY_PORT->code,
          MY_SYS->Name
        );
        Send(str, MY_PORT->sys_num, FALSE);
        command = MY_PORT->move_dir;
        System_Move();
        sprintf(str, getmsg(MSSG180), MY_SYS->Name);
        Send(str, myp, FALSE);
      }
    }

    STATE = MAIN_PROMPT;
  } else if (STATE == GO_SERVER_PROMPT) {
    Clear_Display_Window();

    if (margc) {
      i = Find_Gang_Server(a_human, margv[0]);

      if (i < 0) {
        prfmsg(MSG821);
        MY_PORT->code[0] = 0;
      } else if (System_Is_Running(MY_PORT->sys_num, ANYTHING)) {
        prfmsg(MSG822);
        MY_PORT->code[0] = 0;
      } else if
      (
        Proc[MY_PORT->sys_num]->file_server != i &&
        Proc[MY_PORT->sys_num]->file_server != 0
      ) {
        prfmsg(MSG822);
        MY_PORT->code[0] = 0;
      } else if
      (
        Proc[MY_PORT->sys_num]->file_server == 0 &&
        !Upload_Gang_Data(MY_PORT->sys_num, i)
      ) {
        prfmsg(MSG822);
        MY_PORT->code[0] = 0;
      } else {
        strcpy(str, margv[0]);
        str[CODE_SIZE - 1] = 0;
        strcpy(MY_PORT->code, str);
        MY_SYS->node_type[MY_PORT->pwx][MY_PORT->pwy] = OPENED_GATE;
        Quick_Update
        (
          SYSTEM_VIEW,
          MY_PORT->net_x, MY_PORT->net_y,
          MY_PORT->city_x, MY_PORT->city_y,
          MY_PORT->pwx, MY_PORT->pwy,
          0, 0
        );
        command = MY_PORT->move_dir;
        System_Move();
      }
    }

    STATE = MAIN_PROMPT;
  } else if (STATE == PASSWORD_PROMPT) {
    if (margc) {
      if
      (
        !me.perm[GAMEOP_PERM] &&
        (
          MY_SYS->password[0] == 0 ||
          !sameas(MY_SYS->password, margv[0])
        )
      ) {
        prfmsg(MSG676);
      } else {
        prfmsg(MSG677);
        MY_SYS->node_type[MY_PORT->pwx][MY_PORT->pwy] = OPENED_GATE;
        Quick_Update
        (
          SYSTEM_VIEW,
          MY_PORT->net_x, MY_PORT->net_y,
          MY_PORT->city_x, MY_PORT->city_y,
          MY_PORT->pwx, MY_PORT->pwy,
          0, 0
        );
      }
    }

    STATE = MAIN_PROMPT;
  } else if (margc && STATE == OUTLAW_PROMPT) {
    Outlaw(1);
    STATE = MAIN_PROMPT;
  } else if (margc && STATE == SUICIDE_PROMPT) {
    if (sameas(margv[0], ANS_YES)) {
      MY_PORT->active = FALSE;
      sprintf
      (
        str,
        getmsg(MSG182),
        myp + 1,
        me.userid,
        me.alias
      );
      Global_Message(str, myp);
      Erase_Mail(me.index);

      if (me.gang) {
        Dead_Gangster(me.gang, me.index, me.rank);
      }

      Write_My_Rec(SUICIDE);
      myp = -1;
    } else if (sameas(margv[0], ANS_NO)) {
      STATE = MAIN_PROMPT;
    }
  } else if (margc) {
    rstrin();

    if (sameas(input, glob->againkey)) {
      prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
      prfmsg(MSG183, MY_PORT->last_input, CLEAR_RIGHT);
      prf("\x1B[%d;1H", DISP_Y);
      strcpy(input, MY_PORT->last_input);
    } else {
      strncpy(MY_PORT->last_input, input, LINE_LEN - 1);
      MY_PORT->last_input[LINE_LEN - 1] = 0;
    }

    parsin();
    Unique_Check();

    if (command) {
      MY_PORT->last_cmd = command;
      Do_Command();

      if (MY_PORT->reporting) {
        My_Outprf(usrnum);
        rstmbk();
        return (TRUE);
      }
    }
  } else if (STATE == MAIN_PROMPT && margc == 0) {
    if (MY_PORT->pinged || usrptr->flags & INJOIP) {
      MY_PORT->pinged = FALSE;
    } else if (glob->quick_move[usrnum] > 0) {
      if (glob->quick_move[usrnum] == '8') {
        command = NORTH;
        MY_PORT->last_cmd = NORTH;
      } else if (glob->quick_move[usrnum] == '2') {
        command = SOUTH;
        MY_PORT->last_cmd = SOUTH;
      } else if (glob->quick_move[usrnum] == '6') {
        command = EAST;
        MY_PORT->last_cmd = EAST;
      } else {
        command = WEST;
        MY_PORT->last_cmd = WEST;
      }

      Moving();
      glob->quick_move[usrnum] = FALSE;
    } else {
      Redraw();
    }
  }

  if (myp != -1) {
    if (MY_PORT->downloading) {
      btumil(usrnum, 0);
      rstrxf();
      My_Outprf(usrnum);
      rstmbk();
      return (1);
    }

    if (STATE == MAIN_PROMPT) {
      MY_PORT->prompt_y = PROMPT_Y;
      MY_PORT->prompt_x = 1;
      Main_Prompt();
      glob->quick_move[usrnum] = FALSE;
    }

    prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
    prf("%s%s", MY_PORT->prompt, CLEAR_RIGHT);
    My_Outprf(usrnum);

    if (STATE == MAIN_PROMPT) {
      btumil(usrnum, 60);
    } else {
      btumil(usrnum, 80 - MY_PORT->prompt_len - MY_PORT->prompt_x);
    }

    Inside(myp);

    if (MY_PORT->access_level == IDLE && me.damage >= glob->dpts) {
      i = His_Rank(me.score, me.won);

      if (i > 1) {
        me.score = rank[i - 1].score;
        me.damage = 0;
        me.won = FALSE;
        Update_Location();
      }
    }

    if
    (
      MY_PORT->access_level == SYSTEM_VIEW &&
      MY_PORT->sys_num == myp &&
      MY_PORT->rez != 100
    ) {
      MY_PORT->rez = 100;
      Update_Location();
    }

    rstmbk();
    return (1);
  } else {
    prf("\x1B[24;1H");
    btumil(usrnum, 0);
    rstrxf();
    My_Outprf(usrnum);
    btuchi(usrnum, NULL);
    rstmbk();
    usrptr->flags &= ~NOGLOB;
    return (0);
  }
}
/***************************************************************************
	see if the command entered is unique.
*/
short EXPORT Unique_Check(void) {
  short   i;
  short   n = 0;
  short   abs_match = 0;
  char    next[LINE_SIZE];
  command = 0;
  strcpy(MY_PORT->unique, "Do you mean");
  next[0] = 0;

  if (sameas(margv[0], "B")) {
    command = BACK;
    EXIT0;
  }

  if (sameas(margv[0], "N")) {
    command = NORTH;
    EXIT0;
  }

  if (sameas(margv[0], "S")) {
    command = SOUTH;
    EXIT0;
  }

  if (sameas(margv[0], "E")) {
    command = EAST;
    EXIT0;
  }

  if (sameas(margv[0], "W")) {
    command = WEST;
    EXIT0;
  }

  if (sameas(margv[0], "?")) {
    Parse_Input();
    Help();
    EXIT0;
  } else if
  (
    sameas(margv[0], "commands") ||
    sameas(margv[0], "proglist") ||
    sameas(margv[0], "icons") ||
    sameas(margv[0], "moving") ||
    sameas(margv[0], "chatting") ||
    sameas(margv[0], "general") ||
    sameas(margv[0], "version")
  ) {
    strcpy(parm1, margv[0]);
    Help();
    EXIT0;
  }

  if (margc == 1 && sameas(margv[0], CANCEL)) {
    Clear_Display_Window();

    if (MY_PORT->access_level != IDLE) {
      prfmsg(MSG184);
    } else {
      if (me.gang && MY_PORT->gang_pts) {
        Read_Gang(me.gang);
        gang->pts += MY_PORT->gang_pts;
        gang->pts_today += MY_PORT->gang_pts;
        Write_Gang(me.gang);
      }

      Sign_Me_Off();
    }

    EXIT0;
  }

  for (i = 1; i != NUM_COMMANDS && !abs_match; ++i) {
    if (sameto(margv[0], commands[i])) {
      ++n;
      command = i;

      if (next[0]) {
        if (n != 2) {
          strcat(MY_PORT->unique, ",");
        }

        strcat(MY_PORT->unique, " ");
        strcat(MY_PORT->unique, next);
        next[0] = 0;
      }

      strcpy(next, commands[i]);

      if (sameas(margv[0], commands[i])) {
        n = 1;
        abs_match = TRUE;
      }
    }
  }

  if (margc <= 2 && Ran_Program(-1, -1, &n, next)) {
    command = 0;
    EXIT0;
  }

  if (next[0]) {
    strcat(MY_PORT->unique, " or ");
    strcat(MY_PORT->unique, next);
    strcat(MY_PORT->unique, "?");
  }

  if (margc <= 2 && n > 1) {
    Clear_Display_Window();
    prfmsg(MSG720, MY_PORT->unique);
    command = 0;
    EXIT0;
  }

  if (n == 0 || (margc > 2 && command != CREATE)) {
    Clear_Display_Window();

    if (Local_Gablink(myp)) {
      Parse_Input();
      Communicate(TO_GAB);
      command = 0;
      EXIT0;
    }

    if
    (
      !me.option[CHAT_OPT] ||
      (me.option[CHAT_OPT] && input[0] == glob->chatkey[0]) ||
      (me.option[CHAT_OPT] && input[0] == glob->privkey[0])
    ) {
      Parse_Input();
      Communicate(TO_IDLE);
      command = 0;
      EXIT0;
    }

    if (n == 0) {
      prfmsg(MSG329, margv[0]);
    } else if (n > 1) {
      prfmsg(MSG720, MY_PORT->unique);
    } else {
      prfmsg(MSG542);
    }

    command = 0;
    EXIT0;
  }

  Parse_Input();
  EXIT0;
}
/**************************************************************************
	user has hung up
*/
void cyber_hup(void) {
  short   i;
  setmbk(glob->cfg);

  for (i = 0; i != max_guys; ++i) {
    if
    (
      Port[i]->active &&
      sameas(Port[i]->guy.userid, usaptr->userid)
    ) {
      Port[i]->hung_up = TRUE;
      Port[i]->hup_timer = 30;
      Port[i]->usrnum = -1;

      if (STATE == DUMP_FILE_PROMPT || STATE == NEXT_MSG_PROMPT) {
        fclose(Port[i]->fp);
      }
    }
  }

  rstmbk();
}
/**************************************************************************
	clean up routine, called once daily (generally 3:00 AM)
*/
void cyber_clean(void) {
  setmbk(glob->cfg);
  NOTIFY("LoC: Clean Up Started");
  glob->shutdown = TRUE;
  glob->server = TRUE;
  Port_Check();
  glob->server = FALSE;

  if (glob->inactdys) {
    NOTIFY("LoC: Clearing Inactive Chars");
    Clear_Old_Characters();
  }

  NOTIFY("LoC: Checking Tournament");
  Tourney_Check();
  NOTIFY("LoC: Internet Billing");
  Internet_Billing();
  NOTIFY("LoC: Gang Scoring");
  Gang_Scoring();
  NOTIFY("LoC: Clean Up Finished");
  rstmbk();
}
/**************************************************************************
	user is being deleted, remove his characters, etc.
*/
void cyber_delete(account)
char *account;
{
  short   n;
  short   found = FALSE;
  short   index = -1;
  FILE    *fp;
  fp = fopen(BRIEF_FILE, READB);

  do {
    n = fread(&glob->brief, BRIEF_SIZE, 1, fp);

    if (n == 1) {
      if (sameas(glob->brief.userid, account)) {
        found = TRUE;
      }
    }

    ++index;
  } while (n == 1 && !found);

  fclose(fp);

  if (found) {
    strcpy(glob->brief.userid, OPEN_RECORD);
    fp = fopen(BRIEF_FILE, READB);
    fseek(fp, (long) index * BRIEF_SIZE, SEEK_SET);
    fwrite(&glob->brief, BRIEF_SIZE, 1, fp);
    fclose(fp);
    fp = fopen(USER_FILE, READB);
    fseek(fp, (long) index * HUMAN_SIZE, SEEK_SET);
    fread(a_human, HUMAN_SIZE, 1, fp);
    strcpy(a_human->userid, OPEN_RECORD);
    fseek(fp, (long) index * HUMAN_SIZE, SEEK_SET);
    fwrite(a_human, HUMAN_SIZE, 1, fp);
    fclose(fp);
    Erase_Mail(index);

    if (glob->brief.gang) {
      Dead_Gangster(a_human->gang, a_human->index, a_human->rank);
    }
  }
}
/**************************************************************************
	system is shutting down, perform any cleanups
*/
void cyber_shutdown(void) {
  NOTIFY("LoC: Shutting Down");
  setmbk(glob->cfg);
  glob->shutdown = TRUE;
  glob->server = TRUE;
  Port_Check();
  glob->server = FALSE;
  rstmbk();
  clsmsg(glob->cfg);
  NOTIFY("LoC: Shut Down");
}
/***************************************************************************
	set the configurable options
*/
void Get_Config(void) {
  char    mask[11];
  char    activation[11];
  char    reg_num[11];
  char    *regnum;
  short   i;
  struct  demo_record_struct demo_record;
  short   cur_days = cofdat(today());
  short   demo_days;
  glob->server = FALSE;
  glob->reset = FALSE;
  glob->shutdown = FALSE;
  strcpy(glob->topguy, "the cyber-terrorists");
  glob->winner[0] = 0;
  glob->cfg = opnmsg("MUICYBER.MCV");
  glob->rankfctr = lngopt(RANKFCTR, 1, GCMAXLONG);
  glob->egamscor = lngopt(EGAMSCOR, 1, GCMAXLONG);
  glob->cybccr = numopt(LOCCCR, -32767, 32767);
  glob->jacks = numopt(TJACKS, -32767, 32767);
  glob->derez1 = numopt(DEREZ1, -32767, 32767);
  glob->gangbux[0] = numopt(GANG1BUX, -32767, 32767);
  glob->gangbux[1] = numopt(GANG2BUX, -32767, 32767);
  glob->gangbux[2] = numopt(GANG3BUX, -32767, 32767);
  glob->gangbux[3] = numopt(GANG4BUX, -32767, 32767);
  glob->gangbux[4] = numopt(GANG5BUX, -32767, 32767);
  glob->suihours = (long) numopt(SUIHOURS, -32767, 32767) * 3600L;
  glob->namechng = (long) numopt(NAMECHNG, -32767, 32767) * 3600L;
  glob->netdebt = numopt(NETDEBT, -32767, 32767);
  glob->tmpdur = numopt(TMPDUR, -32767, 32767);
  /*glob->maxuser = numopt(MAXUSER, -32767, 32767); */
  glob->maxuser = 32;
  glob->paydays = numopt(PAYDAYS, -32767, 32767);
  glob->rentdays = numopt(RENTDAYS, -32767, 32767);
  glob->minrent = numopt(MINRENT, -32767, 32767);
  glob->rentadd = numopt(RENTADD, -32767, 32767);
  glob->minair = numopt(MINAIR, -32767, 32767);
  glob->airadd = numopt(AIRADD, -32767, 32767);
  glob->memcost = numopt(MEMCOST, -32767, 32767);
  glob->cpucost = numopt(CPUCOST, -32767, 32767);
  glob->wallcost = numopt(WALLCOST, -32767, 32767);
  glob->gatecost = numopt(GATECOST, -32767, 32767);
  glob->facecost = numopt(FACECOST, -32767, 32767);
  glob->crcost = numopt(CRCOST, -32767, 32767);
  glob->ldpcost = numopt(LDPCOST, -32767, 32767);
  glob->chipcost = numopt(CHIPCOST, -32767, 32767);
  glob->ldcost = numopt(LDCOST, -32767, 32767);
  glob->netcost = numopt(NETCOST, -32767, 32767);
  glob->ldlcost = numopt(LDLCOST, -32767, 32767);
  glob->hospcost = numopt(HOSPCOST, -32767, 32767);
  glob->jackroll = numopt(JACKROLL, -32767, 32767);
  glob->jacksecs = numopt(JACKSECS, -32767, 32767);
  glob->dpts = numopt(FULLHPTS, -32767, 32767);
  glob->fate = numopt(RANDFATE, -32767, 32767);
  glob->virsecs = numopt(VIRSECS, -32767, 32767);
  glob->crshsecs = numopt(CRSHSECS, -32767, 32767);
  glob->copmins1 = numopt(COPMINS1, -32767, 32767);
  glob->copmins2 = numopt(COPMINS2, -32767, 32767);
  glob->addlesec = numopt(ADDLESEC, -32767, 32767);
  glob->prodsecs = numopt(PRODSECS, -32767, 32767);
  glob->gluesecs = numopt(GLUESECS, -32767, 32767);
  glob->zapodds = numopt(ZAPODDS, -32767, 32767);
  glob->statsecs = numopt(STATSECS, -32767, 32767);
  glob->boltsecs = numopt(BOLTSECS, -32767, 32767);
  glob->tentsecs = numopt(TENTSECS, -32767, 32767);
  glob->cuffsecs = numopt(CUFFSECS, -32767, 32767);
  glob->puntsecs = numopt(PUNTSECS, -32767, 32767);
  glob->inactdys = numopt(INACTDY, -32767, 32767);
  glob->pvprank = numopt(PVPRANK, -32767, 32767);
  glob->xferrank = numopt(XFERRANK, -32767, 32767);
  glob->topval = numopt(TOPVAL, -32767, 32767);
  glob->secval = numopt(SECVAL, -32767, 32767);
  glob->busval = numopt(BUSVAL, -32767, 32767);
  glob->phsrpowr = numopt(PHSRPOWR, -32767, 32767);
  glob->chaindmg = numopt(CHAINDMG, -32767, 32767);
  glob->firedmg = numopt(FIREDMG, -32767, 32767);
  glob->joltdmg = numopt(JOLTDMG, -32767, 32767);
  glob->soldmg = numopt(SOLDMG, -32767, 32767);
  glob->bandmg = numopt(BANDMG, -32767, 32767);
  glob->drilldmg = numopt(DRILLDMG, -32767, 32767);
  glob->stspeed = numopt(STSPEED, -32767, 32767);
  glob->stcpus = numopt(STCPUS, -32767, 32767);
  glob->stwall = numopt(STWALL, -32767, 32767);
  glob->stgate = numopt(STGATE, -32767, 32767);
  glob->stmem = numopt(STMEM, -32767, 32767);
  glob->stchips = numopt(STCHIPS, -32767, 32767);
  glob->streader = ynopt(STREADER);
  glob->outlaw = ynopt(OUTLAWS);
  glob->pre_blurb = ynopt(BLURB1);
  glob->game_blurb = ynopt(BLURB2);
  glob->post_blurb = ynopt(BLURB3);
  glob->autojack = ynopt(AUTOJACK);
  glob->againkey    = stgopt(AGAINKEY);
  glob->chatkey     = stgopt(CHATKEY);
  glob->privkey     = stgopt(PRIVKEY);
  glob->sysopkey    = stgopt(SYSOPKEY);
  glob->lockey      = stgopt(LOCKEY);
  glob->playkey     = stgopt(PLAYKEY);
  glob->validus     = stgopt(VALIDUS);
  glob->main_prompt = stgopt(MAINPR);
  glob->gab_prompt  = stgopt(MSG483);
  glob->empty       = stgopt(EMPTY);
  glob->aempty      = stgopt(AEMPTY);
  glob->gempty      = stgopt(GEMPTY);
  glob->agempty     = stgopt(AGEMPTY);
  glob->theguy      = stgopt(THEGUY);
  glob->badguy      = stgopt(BADGUY);
  glob->gtheguy     = stgopt(GTHEGUY);
  glob->datawall    = stgopt(DATAWALL);
  glob->deadwall    = stgopt(DEADWALL);
  glob->syscpui     = stgopt(SYSCPUI);
  glob->syscpuc     = stgopt(SYSCPUX);
  glob->sysmem      = stgopt(SYSMEM);
  glob->sysmemx     = stgopt(SYSMEMX);
  glob->bbssys      = stgopt(BBSSYS);
  glob->lev1sys     = stgopt(LEV1SYS);
  glob->lev2sys     = stgopt(LEV2SYS);
  glob->lev3sys     = stgopt(LEV3SYS);
  glob->lev5sys     = stgopt(LEV5SYS);
  glob->lev6sys     = stgopt(LEV6SYS);
  glob->lev7sys     = stgopt(LEV7SYS);
  glob->crimesys    = stgopt(CRIMESYS);
  glob->copsys      = stgopt(COPSYS);
  glob->decksys     = stgopt(DCKSYS);
  glob->lev4sys     = stgopt(LEV4SYS);
  glob->vendsys     = stgopt(VNDRSYS);
  glob->aliensys    = stgopt(UFOSYS);
  glob->ldlink      = stgopt(LDLINK);
  glob->opengate    = stgopt(OPENGATE);
  glob->clsdgate    = stgopt(CLSDGATE);
  glob->hubsys      = stgopt(HUBSYS);
  glob->banksys     = stgopt(BANKSYS);
  glob->endsys      = stgopt(ENDSYS);
  glob->gangsys     = stgopt(GANGSYS);
  glob->websys      = stgopt(WEBSYS);
  glob->aendsys     = stgopt(AENDSYS);
  glob->aufosys     = stgopt(AUFOSYS);
  glob->acrimsys    = stgopt(ACRIMSYS);
  glob->awebsys     = stgopt(AWEBSYS);
  glob->agngsys     = stgopt(AGANGSYS);
  glob->acopsys     = stgopt(ACOPSYS);
  glob->ahubsys     = stgopt(AHUBSYS);
  glob->abanksys    = stgopt(ABANKSYS);
  glob->atheguy     = stgopt(ATHEGUY);
  glob->abadguy     = stgopt(ABADGUY);
  glob->agtheguy    = stgopt(AGTHEGUY);
  glob->adatawal    = stgopt(ADATAWAL);
  glob->adeadwal    = stgopt(ADEADWAL);
  glob->asyscpui    = stgopt(ASYSCPUI);
  glob->asyscpuc    = stgopt(ASYSCPUX);
  glob->asysmem     = stgopt(ASYSMEM);
  glob->asysmemx    = stgopt(ASYSMEMX);
  glob->abbssys     = stgopt(ABBSSYS);
  glob->alev1sys    = stgopt(ALEV1SYS);
  glob->alev2sys    = stgopt(ALEV2SYS);
  glob->alev3sys    = stgopt(ALEV3SYS);
  glob->alev5sys    = stgopt(ALEV5SYS);
  glob->alev6sys    = stgopt(ALEV6SYS);
  glob->alev7sys    = stgopt(ALEV7SYS);
  glob->adecksys    = stgopt(ADCKSYS);
  glob->alev4sys    = stgopt(ALEV4SYS);
  glob->avendsys    = stgopt(AVNDSYS);
  glob->aldlink     = stgopt(ALDLINK);
  glob->aopngate    = stgopt(AOPNGATE);
  glob->aclsgate    = stgopt(ACLSGATE);
  glob->m53         = stgopt(MSSG53);
  glob->m54         = stgopt(MSSG54);
  glob->m35         = stgopt(MX35);
  glob->m91         = stgopt(MSSG91);
  glob->m92         = stgopt(MSG92);
  glob->m22         = stgopt(MX22);
  glob->m30         = stgopt(MSSG30);
  glob->m130        = stgopt(MSSG130);
  glob->m570        = stgopt(MSSG570);
  glob->m572        = stgopt(MSSG572);
  glob->m573        = stgopt(MSSG573);
  glob->m575        = stgopt(MSSG575);
  glob->m584        = stgopt(MSSG584);
  glob->m585        = stgopt(MSSG585);
  glob->m571        = stgopt(MSSG571);
  glob->m70a        = stgopt(MSSG70A);
  glob->m70b        = stgopt(MSSG70B);
  glob->m70c        = stgopt(MSSG70C);
  glob->m71         = stgopt(MSSG71);
  glob->ms71        = stgopt(MSG71);
  glob->m72         = stgopt(MSSG72);
  glob->m73         = stgopt(MSSG73);
  glob->ms73        = stgopt(MSG73);
  glob->m74         = stgopt(MSSG74);
  glob->m75         = stgopt(MSSG75);
  glob->m76         = stgopt(MSSG76);
  glob->m538        = stgopt(MSSG538);
  glob->m540        = stgopt(MSSG540);
  glob->m513        = stgopt(MSSG513);
  glob->m514        = stgopt(MSSG514);
  glob->m515        = stgopt(MSSG515);
  glob->m516A       = stgopt(M516A);
  glob->m516B       = stgopt(M516B);
  glob->m518        = stgopt(MSSG518);
  glob->m519        = stgopt(MSSG519);
  glob->world_edit = TRUE;
  sprintf(reg_num, "%ld", 74160129L);
  regnum = spr("%s", bturno);

  if (sameas(regnum, reg_num)) {
    glob->world_edit = 2;
  }

  reg_num[0]  = regnum[0];
  reg_num[1]  = regnum[1];
  reg_num[2]  = regnum[2];
  reg_num[3]  = regnum[2];
  reg_num[4]  = regnum[3];
  reg_num[5]  = regnum[4];
  reg_num[6]  = regnum[5];
  reg_num[7]  = regnum[5];
  reg_num[8]  = regnum[6];
  reg_num[9]  = regnum[7];
  reg_num[10] = 0;
  activation[10] = 0;
#ifdef MBBS
  sprintf(mask, "%ld", 1860164327L); /* 4 user */

  for (i = 0; i != strlen(mask); ++i) {
    activation[i] = 'A' + reg_num[i] - '0';
    activation[i] += mask[i] - '0';
  }

  /*if (sameas(activation, glob->validus))
  {*/
  max_guys = 32;
  /*}*/
  sprintf(mask, "%ld", 1304865279L); /* 8 user */
  /* swenson: just set the maximu number of users to 32 */
  /*
  for (i = 0; i != strlen(mask); ++i)
  {
  	activation[i] = 'A' + reg_num[i] - '0';
  	activation[i] += mask[i] - '0';
  }
  activation[10] = 0;

  if (sameas(activation, glob->validus))
  {
  	max_guys = 32;
  }*/
  sprintf(mask, "%ld", 1057496185L); /* 16 user */
  /*
  for (i = 0; i != strlen(mask); ++i)
  {
  	activation[i] = 'A' + reg_num[i] - '0';
  	activation[i] += mask[i] - '0';
  }

  if (sameas(activation, glob->validus))
  {
  	max_guys = 32;
  }
  */
  sprintf(mask, "%ld", 1867351207L); /* 24 user */
  /*
  for (i = 0; i != strlen(mask); ++i)
  {
  	activation[i] = 'A' + reg_num[i] - '0';
  	activation[i] += mask[i] - '0';
  }

  if (sameas(activation, glob->validus))
  {
  	max_guys = 32;
  }*/
#endif
#ifdef MBBS
  sprintf(mask, "%ld", 1193079235L); /* 32 user */
#else
  sprintf(mask, "%ld", 1496012538L);
#endif
  /* swenson: always activated */
  /*for (i = 0; i != strlen(mask); ++i)
  {
  	activation[i] = 'A' + reg_num[i] - '0';
  	activation[i] += mask[i] - '0';
  }

  if (sameas(activation, glob->validus))
  {
  	max_guys = 32;
  }*/
  regnum = spr("%s", bturno);

  for (i = 0; i != strlen(regnum); ++i) {
    if (regnum[i] != '0' && regnum[i] != '9') {
      ++regnum[i];
    }
  }

#if 0

  if
  (
    atol(regnum) == 50868736L ||  /* nonexistant? */
    atol(regnum) == 83833736L ||  /* gcomm pirate */
    atol(regnum) == 42869856L ||  /* gcomm pirate */
    atol(regnum) == 89093709L ||  /* gcomm pirate */
    atol(regnum) == 20430432L     /* pirate? 303-431-9894 */
  ) {
    max_guys = 0;
  }

  if (glob->validus[0] == 0 || max_guys == 0) {
    max_guys = 32;
    strcpy(glob->validus, "gcomm");
  }

#endif
  max_guys = 32;

  if (glob->maxuser > max_guys) {
    glob->maxuser = max_guys;
  }

  sprintf(str, "LoC: %d/%d Users", glob->maxuser, max_guys);
  NOTIFY(str);

  if (glob->maxuser < max_guys) {
    max_guys = glob->maxuser;
  }

  max_systems = (max_guys * 2) + CONS + COPS + FLUFF_SYSTEMS;
  glob->cop_sys = max_systems - CONS - COPS;

  if (sameas(glob->validus, "gcomm")) {
#ifdef MBBS
    setbtv(genbb);
#else
    dfaSetBlk(genbb);
#endif
    strcpy(demo_record.userid, glob->validus);
    stzcpy(demo_record.modnam, gmdnam("MUICYBER.MDF"), MNMSIZ);
#ifdef MBBS

    if (acqbtv(&demo_record, &demo_record, 0))
#else
    if (dfaAcqEQ(&demo_record, &demo_record, 0))
#endif
    {
      demo_days = cofdat(demo_record.install_date);

      if (cur_days - demo_days >= DEMO_DAYS || demo_days > cur_days) {
        NOTIFY("LoC: Deactivated!");
        NOTIFY("LoC: To register, call:");
        NOTIFY("LoC: Adept Communications");
        NOTIFY("LoC: 800-294-3044");
        glob->validus[0] = 0;
      } else {
        sprintf
        (
          str,
          "LoC: %d more days",
          DEMO_DAYS - (cur_days - demo_days)
        );
        NOTIFY(str);
      }
    } else {
      demo_record.install_date = today();
#ifdef MBBS
      invbtv(&demo_record, sizeof(demo_record));
#else
      dfaInsertV(&demo_record, sizeof(demo_record));
#endif
      NOTIFY("LoC: Demo Activated");
    }

#ifdef MBBS
    rstbtv();
#else
    dfaRstBlk();
#endif
  }
}
/****************************************************************************
	incoming asynch message
*/
GBOOL cyber_asynch(void) {
  short   i;
  short   p = -1;
  short   bottom;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && Port[i]->usrnum == othusn) {
      p = i;
    }
  }

  if (p != -1) {
    Port[p]->pinged = TRUE;
  }

  if
  (
    p == -1 ||
    Port[p]->editing ||
    Port[p]->cycling ||
    Port[p]->reporting ||
    Port[p]->downloading ||
    Port[p]->guy.option[BUSY_OPT] ||
    Port[p]->usrnum == -1 ||
#ifdef MBBS
    user[Port[p]->usrnum].substt == WINNERS_PROMPT
#else
    usroff(Port[p]->usrnum)->substt == WINNERS_PROMPT
#endif
  ) {
    return (FALSE);
  }

  bottom = othuap->scnfse;

  if (bottom > 25) {
    bottom = 25;
  }

  sprintf(str, "\x1B[%d;%dr", ASYNCH_Y, bottom);
  btuxmn(othusn, str);
  sprintf(str, "\x1B[%d;1H", ASYNCH_Y + NUM_PREV_MSGS);
  btuxmn(othusn, str);
  btuxmn(othusn, prfbuf);
  sprintf(str, "\x1B[1;%dr", bottom);
  btuxmn(othusn, str);
  sprintf
  (
    str,
    "\x1B[%d;%dH%s",
    Port[p]->prompt_y,
    Port[p]->prompt_x,
    Port[p]->prompt
  );
  btuxmn(othusn, str);
#ifdef MBBS
  user[othusn].flags |= INJOIP;
#else
  usroff(othusn)->flags |= INJOIP;
#endif
  return (TRUE);
}
/**************************************************************************
	intercept keystrokes
*/
char Stroke_Monitor(int chan, int c) {
  short   i;

  if (c == 8) {
    return (c);
  }

  if (c == 13 || c == 10) {
    glob->quick_move[chan] = FALSE;
    return (c);
  }

  if (c < ' ' || c > 127) {
    return (0);
  }

  if
  (
#ifdef MBBS
    user[chan].substt == MAIN_PROMPT &&
#else
    usroff(chan)->substt == MAIN_PROMPT &&
#endif
    (c == '8' || c == '2' || c == '6' || c == '4')
  ) {
    for (i = 0; i != max_guys; ++i) {
      if (Port[i]->active && Port[i]->usrnum == chan) {
        if
        (
          Port[i]->access_level != IDLE &&
          !glob->quick_move[chan]
        ) {
          glob->quick_move[chan] = c;
          return (13);
        }

        return (c);
      }
    }
  }

  glob->quick_move[chan] = -1;
  return (c);
}
/**************************************************************************
	status handler
*/
void EXPORT cyber_status(void) {
  short   i;
  short   done;

  for (i = 0, myp = -1; i != max_guys && myp < 0; ++i) {
    if (Port[i]->active && Port[i]->usrnum == usrnum) {
      myp = i;
    }
  }

  if (myp != -1 && status == CYCLE && MY_PORT->cycling) {
    setmbk(glob->cfg);
    Next_System_Name();
    rstmbk();
  } else if (myp != -1 && status == CYCLE && MY_PORT->reporting) {
    setmbk(glob->cfg);

    do {
      done = Perm_Report();
    } while (!done);

    rstmbk();
  } else {
    if (myp != -1) {
      MY_PORT->pinged = TRUE;
    }

    dfsthn();
  }
}

