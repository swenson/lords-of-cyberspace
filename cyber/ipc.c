#include <header.h>
#include <globals.h>

/*****************************************************************************
	send a message to everyone not jacked-in
*/
void My_Outprf(short user_num) {
  if (prfbuf[0]) {
    if (user_num != -1) {
      outprf(user_num);
    }

    clrprf();
  }
}
/*****************************************************************************
	send a message to everyone not jacked-in
*/
void EXPORT Idle_Message(char *msg_txt) {
  short g;

  for (g = 0; g != max_guys; ++g) {
    Port[g]->msg_flag = FALSE;

    if
    (
      Port[g]->active &&
      !Port[g]->guy.option[BUSY_OPT] &&
      g != myp &&
      !Local_Gablink(g)
    ) {
      sprintf(msg_text, glob->m570, me.alias, msg_txt);
      Send(msg_text, g, FALSE);
      Port[g]->msg_flag = TRUE;
    }
  }

  prf(glob->m571);

  for (g = 0; g != max_guys; ++g) {
    if
    (
      g != myp &&
      Port[g]->active &&
      !Port[g]->msg_flag &&
      Port[g]->monitoring
    ) {
      sprintf(msg_text, glob->m572, me.alias, msg_txt);
      Send(msg_text, g, FALSE);
    }
  }
}
/*****************************************************************************
	send a gab message
*/
void Gab_Message(msg_txt)
char *msg_txt;
{
  short   g;
  short   is_local;

  for (g = 0; g != max_guys; ++g) {
    Port[g]->msg_flag = FALSE;

    if (Port[g]->active && g != myp && !Port[g]->guy.option[BUSY_OPT]) {
      is_local = Local_Gabber(g);

      if (is_local) {
        sprintf(msg_text, glob->m573, me.alias, msg_txt);
        Send(msg_text, g, FALSE);
        Port[g]->msg_flag = TRUE;
      }
    }
  }

  prfmsg(MSG574);

  for (g = 0; g != max_guys; ++g) {
    if
    (
      g != myp &&
      Port[g]->active &&
      !Port[g]->msg_flag &&
      Port[g]->monitoring
    ) {
      sprintf(msg_text, glob->m575, me.alias, msg_txt);
      Send(msg_text, g, FALSE);
    }
  }
}
/*****************************************************************************
	see if the specified port is local for gabbing purposes
*/
short Local_Gabber(short g) {
  short   is_local = FALSE;

  if
  (
    Port[g]->access_level == MY_PORT->access_level &&
    Port[g]->net_x == MY_PORT->net_x &&
    Port[g]->net_y == MY_PORT->net_y &&
    MY_PORT->access_level == CITY_VIEW
  ) {
    is_local = TRUE;
  } else if
  (
    Port[g]->access_level == MY_PORT->access_level &&
    MY_PORT->access_level == SYSTEM_VIEW &&
    Port[g]->net_x == MY_PORT->net_x &&
    Port[g]->net_y == MY_PORT->net_y &&
    Port[g]->city_x == MY_PORT->city_x &&
    Port[g]->city_y == MY_PORT->city_y
  ) {
    is_local = TRUE;
  }

  return (is_local);
}
/****************************************************************************
	send messages between users
*/
short Communicate(short option) {
  short   i;
  short   g = -1;
  short   text_start = 0;
  short   matches = 0;
  short   abs_match;

  if (margv[0][0] == glob->privkey[0]) {
    if (margv[0][1] == 0) {
      prfmsg(MSG576);
      EXIT0;
    }

    for (i = abs_match = 0; i != max_guys && !abs_match; ++i) {
      if (Port[i]->active && !Port[i]->invisible) {
        if (sameas(&margv[0][1], Port[i]->guy.alias)) {
          g = i;
          abs_match = TRUE;
          matches = 1;
        } else if (sameto(&margv[0][1], Port[i]->guy.alias)) {
          g = i;
          ++matches;
        }
      }
    }

    if (g >= 0 && option == TO_GAB) {
      if (!Local_Gabber(g)) {
        prfmsg(MSG578, Port[g]->guy.alias);
        EXIT0;
      }
    }

    if (g < 0) {
      prfmsg(MSG580, &margv[0][1]);
      EXIT0;
    }

    if (matches > 1) {
      prfmsg(MSG581, &margv[0][1]);
      EXIT0;
    }

    text_start = 1;
  }

  glob->txt[0] = 0;

  for (i = text_start; i < margc; ++i) {
    if
    (
      i == 0 &&
      me.option[CHAT_OPT] &&
      margv[0][0] == glob->chatkey[0]
    ) {
      strcat(glob->txt, &margv[i][1]);
    } else {
      strcat(glob->txt, margv[i]);
    }

    if (i + 1 != margc) {
      strcat(glob->txt, " ");
    }
  }

  if (glob->txt[0] == 0) {
    prfmsg(MSG582);
    EXIT0;
  }

  if (g == -1) {
    if (option == TO_GAB) {
      Gab_Message(glob->txt);
    } else if (option == TO_IDLE) {
      Idle_Message(glob->txt);
    }
  } else {
    prfmsg(MSG583, Port[g]->guy.alias);
    sprintf(str, glob->m584, me.alias, glob->txt);
    Send(str, g, TRUE);

    for (i = 0; i != max_guys; ++i) {
      if
      (
        i != myp &&
        i != g &&
        Port[i]->active &&
        Port[i]->monitoring
      ) {
        sprintf(str, glob->m585, me.alias, glob->txt);
        Send(str, i, FALSE);
      }
    }
  }

  EXIT0;
}
/****************************************************************************
	guy just became uncloaked, reveal him
*/
void Reveal_Guy(short g) {
  short   p;
  short   is_local;

  for (p = 0; p != max_guys; ++p) {
    if
    (
      g != p &&
      Port[p]->active &&
      Port[p]->access_level != IDLE &&
      Port[g]->access_level == Port[p]->access_level &&
      Visible(p, g)
    ) {
      is_local = FALSE;

      if
      (
        Port[g]->net_x == Port[p]->net_x &&
        Port[g]->net_y == Port[p]->net_y &&
        Port[g]->access_level == CITY_VIEW
      ) {
        is_local = TRUE;
      } else if
      (
        Port[g]->access_level == SYSTEM_VIEW &&
        Port[g]->net_x == Port[p]->net_x &&
        Port[g]->net_y == Port[p]->net_y &&
        Port[g]->city_x == Port[p]->city_x &&
        Port[g]->city_y == Port[p]->city_y
      ) {
        is_local = TRUE;
      }

      if (is_local) {
        sprintf(str, glob->m35, him.alias);
        Send(str, p, FALSE);
      }
    }
  }

  if (Port[g]->access_level == SYSTEM_VIEW) {
    Quick_Update
    (
      Port[g]->access_level,
      Port[g]->net_x, Port[g]->net_y,
      Port[g]->city_x, Port[g]->city_y,
      Port[g]->sys_x, Port[g]->sys_y,
      0, 0
    );
  } else if (Port[g]->access_level == CITY_VIEW) {
    Quick_Update
    (
      Port[g]->access_level,
      Port[g]->net_x, Port[g]->net_y,
      Port[g]->city_x, Port[g]->city_y,
      Port[g]->city_x, Port[g]->city_y,
      0, 0
    );
  }
}
/****************************************************************************
	guy just became cloaked, hide him
*/
void Cloak_Guy(short g) {
  short   p;
  short   is_local;

  for (p = 0; p != max_guys; ++p) {
    if
    (
      g != p &&
      Port[p]->active &&
      Port[p]->access_level != IDLE &&
      Port[g]->access_level == Port[p]->access_level &&
      !Visible(p, g)
    ) {
      is_local = FALSE;

      if
      (
        Port[g]->net_x == Port[p]->net_x &&
        Port[g]->net_y == Port[p]->net_y &&
        Port[g]->access_level == CITY_VIEW
      ) {
        is_local = TRUE;
      } else if
      (
        Port[g]->access_level == SYSTEM_VIEW &&
        Port[g]->net_x == Port[p]->net_x &&
        Port[g]->net_y == Port[p]->net_y &&
        Port[g]->city_x == Port[p]->city_x &&
        Port[g]->city_y == Port[p]->city_y
      ) {
        is_local = TRUE;
      }

      if (is_local) {
        sprintf(str, glob->m22, him.alias);
        Send(str, p, FALSE);
      }
    }
  }

  if (Port[g]->access_level == SYSTEM_VIEW) {
    Quick_Update
    (
      Port[g]->access_level,
      Port[g]->net_x, Port[g]->net_y,
      Port[g]->city_x, Port[g]->city_y,
      Port[g]->sys_x, Port[g]->sys_y,
      0, 0
    );
  } else if (Port[g]->access_level == CITY_VIEW) {
    Quick_Update
    (
      Port[g]->access_level,
      Port[g]->net_x, Port[g]->net_y,
      Port[g]->city_x, Port[g]->city_y,
      Port[g]->city_x, Port[g]->city_y,
      0, 0
    );
  }
}
/****************************************************************************
	hydra just became cloaked, hide him
*/
void Cloak_Hydra(short s, short i) {
  short   p;
  short   access_level;
  short   hydra_here;

  if (SYS_PROGRAM.sys_x != -1 && SYS_PROGRAM.sys_y != -1) {
    access_level = SYSTEM_VIEW;
  } else {
    access_level = CITY_VIEW;
  }

  for (p = 0; p != max_guys; ++p) {
    if (s != p && Port[p]->active && Port[p]->access_level != IDLE) {
      hydra_here = FALSE;

      if
      (
        Port[p]->net_x == SYS_PROGRAM.net_x &&
        Port[p]->net_y == SYS_PROGRAM.net_y
      ) {
        if
        (
          access_level == SYSTEM_VIEW &&
          Port[p]->city_x == SYS_PROGRAM.city_x &&
          Port[p]->city_y == SYS_PROGRAM.city_y
        ) {
          hydra_here = TRUE;
        } else if
        (
          access_level == CITY_VIEW &&
          Port[p]->access_level == CITY_VIEW
        ) {
          hydra_here = TRUE;
        }
      }

      if (hydra_here && !Visible_Hydra(p, s, i)) {
        sprintf(str, glob->m22, SYS_SOURCE.name);
        Send(str, p, FALSE);
      }
    }
  }

  if (access_level == SYSTEM_VIEW) {
    Quick_Update
    (
      access_level,
      SYS_PROGRAM.net_x, SYS_PROGRAM.net_y,
      SYS_PROGRAM.city_x, SYS_PROGRAM.city_y,
      SYS_PROGRAM.sys_x, SYS_PROGRAM.sys_y,
      0, 0
    );
  } else if (access_level == CITY_VIEW) {
    Quick_Update
    (
      access_level,
      SYS_PROGRAM.net_x, SYS_PROGRAM.net_y,
      SYS_PROGRAM.city_x, SYS_PROGRAM.city_y,
      SYS_PROGRAM.city_x, SYS_PROGRAM.city_y,
      0, 0
    );
  }
}
/****************************************************************************
	send message to specified user(s)
*/
void System_Message(char *msg_txt, short s) {
  short   g;

  for (g = 0; g != max_guys; ++g) {
    if
    (
      Port[g]->active &&
      Port[g]->sys_num == s &&
      Port[g]->access_level == SYSTEM_VIEW
    ) {
      Send(msg_txt, g, FALSE);
    }
  }
}
/****************************************************************************
	send message to specified user(s)
*/
void System_Program_Message(char *msg_txt, short sys_num, short s, short i) {
  short   g;

  for (g = 0; g != max_guys; ++g) {
    if
    (
      Port[g]->active &&
      Port[g]->sys_num == sys_num &&
      Port[g]->access_level == SYSTEM_VIEW
    ) {
      if (Visible_Hydra(g, s, i)) {
        Send(msg_txt, g, FALSE);
      }
    }
  }
}
/****************************************************************************
	send message to specified user(s)
*/
void Cloakable_System_Message(char *msg_txt, short s, short p) {
  short   g;

  for (g = 0; g != max_guys; ++g) {
    if
    (
      g != p &&
      Port[g]->active &&
      Port[g]->sys_num == s &&
      Port[g]->access_level == SYSTEM_VIEW
    ) {
      if (Visible(g, p)) {
        Send(msg_txt, g, FALSE);
      }
    }
  }
}
/*****************************************************************************
	send a message to everyone in my city
*/
void City_Message(char *msg_txt, short x, short y, short p) {
  short   g;

  for (g = 0; g != max_guys; ++g) {
    if
    (
      g != p &&
      Port[g]->active &&
      Port[g]->net_x == x &&
      Port[g]->net_y == y &&
      Port[g]->access_level == CITY_VIEW
    ) {
      if (Visible(g, p)) {
        Send(msg_txt, g, FALSE);
      }
    }
  }
}
/*****************************************************************************
	send a message to everyone in a program's city
*/
void City_Program_Message(char *msg_txt, short x, short y, short s, short i) {
  short   g;

  for (g = 0; g != max_guys; ++g) {
    if
    (
      Port[g]->active &&
      Port[g]->net_x == x &&
      Port[g]->net_y == y &&
      Port[g]->access_level == CITY_VIEW
    ) {
      if (Visible_Hydra(g, s, i)) {
        Send(msg_txt, g, FALSE);
      }
    }
  }
}
/*****************************************************************************
	send a message to everyone
*/
void Global_Message(char *msg_txt, short p) {
  short   i;
  short   old_p = myp;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && i != p) {
      if (!Port[i]->guy.option[BUSY_OPT]) {
        Send(msg_txt, i, FALSE);
      }

      if
      (
        Port[i]->usrnum != -1 &&
#ifdef MBBS
        user[Port[i]->usrnum].substt == MAIN_PROMPT
#else
        usroff(Port[i]->usrnum)->substt == MAIN_PROMPT
#endif
      ) {
        myp = i;
        Update_Location();
        myp = old_p;
      }
    }
  }
}
/*******************************************************************************
	find a port for the user and sign them on
*/
short Sign_Me_On(void) {
  short   i;
  printf("Sign me on\n");
  myp = -1;

  if (usrptr->flags & INVISB) {
    for (i = max_guys - 1; i >= 0 && myp < 0; --i) {
      if (!Port[i]->active) {
        myp = i;
      }
    }
  } else {
    for (i = 0; i != max_guys && myp < 0; ++i) {
      if (!Port[i]->active) {
        myp = i;
      }
    }
  }

  printf("Found port = %d\n", myp);

  if (myp < 0) {
    EXIT0;
  }

  memcpy(&MY_PORT->guy, my_record, HUMAN_SIZE);
  MY_PORT->pinged         = FALSE;
  MY_PORT->redraw         = FALSE;
  MY_PORT->icebolt_time   = 0;
  MY_PORT->memory_bonk    = FALSE;
  MY_PORT->hung_up        = FALSE;
  MY_PORT->handcuff_time  = 0;
  MY_PORT->secs_on        = 0;
  MY_PORT->prompt[0]      = 0;
  MY_PORT->last_icon[0]   = 0;
  MY_PORT->last_x         = -1;
  MY_PORT->last_y         = -1;
  MY_PORT->back_x         = -1;
  MY_PORT->back_y         = -1;
  MY_PORT->autojack       = me.option[AUTOJACK_OPT];
  MY_PORT->code[0]        = 0;
  MY_PORT->gang_pts       = 0;
  MY_PORT->last_input[0]  = 0;
  MY_PORT->downloading    = FALSE;
  MY_PORT->editing        = FALSE;
  MY_PORT->cycling        = FALSE;
  MY_PORT->reporting      = FALSE;
  MY_PORT->unlocking      = FALSE;
  MY_PORT->prompt_y       = PROMPT_Y;
  MY_PORT->prompt_x       = 1;
  MY_PORT->usrnum         = usrnum;
  MY_PORT->active         = TRUE;
  MY_PORT->access_level   = IDLE;
  MY_PORT->link_type      = IDLE;
  MY_PORT->ld_time        = 0;
  MY_PORT->ld_moves       = 0;
  MY_PORT->walks[0]       = 0;
  MY_PORT->last_view      = GLOBAL_VIEW;
  MY_PORT->sys_num        = -1;
  MY_PORT->net_x          = 0;
  MY_PORT->net_y          = 0;
  MY_PORT->city_x         = -1;
  MY_PORT->city_y         = -1;
  MY_PORT->sys_x          = -1;
  MY_PORT->sys_y          = -1;
  MY_PORT->gx             = -1;
  MY_PORT->gy             = -1;
  MY_PORT->wx             = -1;
  MY_PORT->wy             = -1;
  MY_PORT->px             = -1;
  MY_PORT->py             = -1;
  MY_PORT->monitoring     = me.perm[MONITOR_PERM];

  for (i = 0; i != MAX_CPUS + 1; ++i) {
    MY_PROC->cpu[i].m_index = -1;
  }

  Reset_Location_Values();
  MY_PORT->time_on = now();

  if (usrptr->flags & INVISB) {
    MY_PORT->invisible = TRUE;
  } else {
    MY_PORT->invisible = FALSE;
    sprintf
    (
      str,
      getmsg(MSG590),
      myp + 1,
      me.userid,
      me.alias
    );
    Global_Message(str, myp);
  }

  MY_PORT->old_credit_rate = usrptr->crdrat;
  usrptr->crdrat += glob->cybccr;

  if (usrptr->crdrat < 0) {
    usrptr->crdrat = 0;
  }

  EXIT0;
}

int Setup_Signoff(void) {
  int i;
  for (i = 0, myp = -1; i != max_guys && myp < 0; ++i) {
    if
    (
      !glob->reset &&
      Port[i]->active &&
      Port[i]->hung_up &&
      sameas(Port[i]->guy.userid, usaptr->userid)
    ) {
      myp = i;
    }
  }
  
  if (myp == -1) {
    for (i = 0, myp = -1; i != max_guys && myp < 0; ++i) {
      if
      (
        Port[i]->active &&
        sameas(Port[i]->guy.userid, usaptr->userid)
      ) {
        myp = i;
      }
    }    
  }  
  return myp;
}

/******************************************************************************
	user is leaving, release his port
*/
void Sign_Me_Off(void) {
  Stop_All_Programs(myp);
  Write_My_Rec(EXISTS);
  Day_Out();
  MY_PORT->active = FALSE;

  if (!MY_PORT->invisible) {
    sprintf
    (
      str,
      getmsg(MSG591),
      myp + 1,
      me.userid,
      me.alias
    );
    Global_Message(str, myp);
  }

  //prf("\x1B[1;%dr", usaptr->scnfse);
  usrptr->crdrat = MY_PORT->old_credit_rate;
  myp = -1;
}
/****************************************************************************
	send text to other port
*/
short EXPORT Send(char *msg, short p, short beep) {
  short   bottom;

  if (p >= max_guys) {
    EXIT0;
  }

  if
  (
    Port[p]->editing ||
    Port[p]->downloading ||
    Port[p]->usrnum == -1 ||
#ifdef MBBS
    user[Port[p]->usrnum].substt == WINNERS_PROMPT
#else
    usroff(Port[p]->usrnum)->substt == WINNERS_PROMPT
#endif
  ) {
    EXIT0;
  }

  if (!glob->server) {
    My_Outprf(MY_PORT->usrnum);
  }

  strcpy(glob->buf, msg);
  stpans(glob->buf);

  if (beep) {
    prf("\7");
  }

  othuap = uacoff(Port[p]->usrnum);
  bottom = othuap->scnfse;

  if (bottom > 25) {
    bottom = 25;
  } else if (bottom < 24) {
    bottom = 24;
  }

  prf("\x1B[%d;%dr", ASYNCH_Y, bottom);

  if (strlen(glob->buf) < 79) {
    prf("\x1B[%d;1H\r%s", ASYNCH_Y + NUM_PREV_MSGS, msg);
  } else {
    Terminate_String(msg);
    prf("\x1B[%d;1H\r%s", ASYNCH_Y + NUM_PREV_MSGS, glob->buf);
  }

  prf("\x1B[1;%dr", bottom);
  prf("\x1B[%d;%dH", Port[p]->prompt_y, Port[p]->prompt_x);
  prf("%s%s", Port[p]->prompt, CLEAR_RIGHT);
  My_Outprf(Port[p]->usrnum);
  EXIT0;
}
