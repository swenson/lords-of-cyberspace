#include <header.h>
#include <globals.h>

/***************************************************************************
    gang menu
*/
short EXPORT Gang_Menu(void) {
  short   ok;

  if (me.invite) {
    ok = Read_Gang(me.invite);

    if (ok && !sameas(gang->name, OPEN_RECORD)) {
      prfmsg(MSG753, me.invite, gang->name);
      Prompt(getmsg(MSG754), ACCEPT_PROMPT);
      EXIT0;
    }

    me.invite = 0;
  }

  if (me.gang) {
    prfmsg(MX731);
    Prompt(getmsg(MX733), GANG_PROMPT2);
  } else {
    prfmsg(MX730);
    Prompt(getmsg(MX732), GANG_PROMPT1);
  }

  EXIT0;
}
/***************************************************************************
    read a gang record
*/
short Read_Gang(short g) {
  short   n;
  FILE    *fp;

  if (g == 0) {
    EXIT0;
  }

  fp = fopen(GANG_FILE, READB);

  if (fp == NULL) {
    NOTIFY("LoC: Gang File Open Failure!");
    EXIT0;
  }

  fseek(fp, (long) g * GANG_SIZE, SEEK_SET);
  n = fread(gang, GANG_SIZE, 1, fp);
  fclose(fp);

  if (n != 1) {
    EXIT0;
  }

  return (TRUE);
}
/***************************************************************************
    write a gang record
*/
short Write_Gang(short g) {
  FILE    *fp;

  if (g == 0) {
    NOTIFY("LoC: Gang 0 Operation Attempted");
    EXIT0;
  }

  fp = fopen(GANG_FILE, READB);

  if (fp == NULL) {
    NOTIFY("LoC: Gang File Open Failure!");
    EXIT0;
  }

  fseek(fp, (long) g * GANG_SIZE, SEEK_SET);
  fwrite(gang, GANG_SIZE, 1, fp);
  fclose(fp);
  return (TRUE);
}
/*****************************************************************************
    check the gang forum
*/
short Read_Gang_Mail(void) {
  short   i;
  short   n;
  short   done = FALSE;
  FILE    *fp;
  sprintf(str, GANG_MAIL_FILE, me.gang);

  if (STATE == GANG_PROMPT2) {
    fp = fopen(str, READB);

    if (fp == NULL) {
      prfmsg(MSG836);
      STATE = MAIN_PROMPT;
      Gang_Menu();
      EXIT0;
    }

    n = fread(&glob->header, FREENET_SIZE, 1, fp);
    fclose(fp);

    if (n == 0) {
      prfmsg(MSG836);
      STATE = MAIN_PROMPT;
      Gang_Menu();
      EXIT0;
    }

    sprintf
    (
      str,
      getmsg(MSG708),
      glob->header.msg_num - glob->header.total + 1,
      glob->header.msg_num
    );
    Prompt(str, GANG_MAIL_PROMPT1);
    EXIT0;
  }

  if (STATE == GANG_MAIL_PROMPT1) {
    if (margc == 0) {
      MY_PORT->forum1 = me.gang_mail + 1;
    } else {
      MY_PORT->forum1 = LNUM(margv[0]);
    }

    fp = fopen(str, READB);
    fread(&glob->header, FREENET_SIZE, 1, fp);
    fclose(fp);

    if (MY_PORT->forum1 > glob->header.msg_num) {
      MY_PORT->forum1 = glob->header.msg_num;
    } else if (MY_PORT->forum1 < 1) {
      MY_PORT->forum1 = 1;
    }

    MY_PORT->forum2 = 1;
  } else if (sameas(margv[0], ANS_NO)) {
    Clear_Display_Window();
    STATE = MAIN_PROMPT;
    me.gang_mail = MY_PORT->forum1 - 1;
    Gang_Menu();
    EXIT0;
  }

  fp = fopen(str, READB);

  do {
    fseek(fp, MY_PORT->forum2 * (long) FREENET_SIZE, SEEK_SET);
    n = fread(&glob->dummy, FREENET_SIZE, 1, fp);

    if (n == 0) {
      Prompt(getmsg(MSG710), GANG_MAIL_PROMPT3);
      me.gang_mail = MY_PORT->forum1;
      fclose(fp);
      EXIT0;
    }

    if (glob->dummy.msg_num < MY_PORT->forum1) {
      ++MY_PORT->forum2;
    } else {
      Clear_Display_Window();
      MY_PORT->forum1 = glob->dummy.msg_num;
      sprintf
      (
        str,
        getmsg(MSG711),
        glob->dummy.msg_num,
        glob->dummy.from
      );
      prf(str);

      for (i = 0; i != MESSAGE_LEN; ++i) {
        if (glob->dummy.body[i][0]) {
          prfmsg(MSG704, glob->dummy.body[i]);
        }
      }

      fseek(fp, (MY_PORT->forum2 + 1L) * (long) FREENET_SIZE, SEEK_SET);
      n = fread(&glob->dummy, FREENET_SIZE, 1, fp);

      if (n == 0) {
        Prompt(getmsg(MSG710), GANG_MAIL_PROMPT3);
        me.gang_mail = MY_PORT->forum1;
        fclose(fp);
        EXIT0;
      }

      ++MY_PORT->forum2;
      fclose(fp);
      Prompt(getmsg(MSG705), GANG_MAIL_PROMPT2);
      EXIT0;
    }
  } while (!done);

  Prompt(getmsg(MSG710), GANG_MAIL_PROMPT3);
  me.gang_mail = MY_PORT->forum1;
  fclose(fp);
  EXIT0;
}
/*****************************************************************************
    post a message to a gang forum
*/
short Post_Gang(void) {
  char    today_date[DATE_SIZE];
  char    now_time[TIME_SIZE];
  struct  freenet_struct *msg;
  msg = (struct freenet_struct *) vdaptr;

  if (STATE == POST_GANG_PROMPT1 || STATE == POST_GANG_PROMPT2) {
    setmem(msg, FREENET_SIZE, 0);
    TODAY(today_date);
    NOW(now_time);

    if (me.gang == MY_PORT->gang) {
      sprintf
      (
        msg->from,
        getmsg(MSG706),
        me.alias,
        today_date,
        now_time
      );
    } else {
      sprintf
      (
        msg->from,
        getmsg(MSG835),
        me.alias,
        today_date,
        now_time
      );
    }

    MY_PORT->lines = 0;
    prf("\x1B[%d;1H", DISP_Y);
    prfmsg(MSG694);
    MY_PORT->prompt_y = DISP_Y + 1;
    sprintf(str, getmsg(MSG695), MY_PORT->lines + 1, MESSAGE_LEN);
    Prompt(str, GANG_BODY_PROMPT);
    EXIT0;
  }

  if (sameas(margv[0], "/c") || sameas(margv[0], "x")) {
    Clear_Display_Window();
    prfmsg(MSG696);
    STATE = MAIN_PROMPT;
    MY_PORT->prompt_y = PROMPT_Y;
    Gang_Menu();
    EXIT0;
  }

  if (!sameas(margv[0], "/s")) {
    rstrin();

    if (strlen(input) > LINE_SIZE - 1) {
      input[LINE_SIZE - 1] = 0;
    }

    strcpy(msg->body[MY_PORT->lines], input);
    ++MY_PORT->lines;
    ++MY_PORT->prompt_y;

    if (MY_PORT->lines < MESSAGE_LEN) {
      sprintf(str, getmsg(MSG695), MY_PORT->lines + 1, MESSAGE_LEN);
      Prompt(str, GANG_BODY_PROMPT);
      EXIT0;
    }
  }

  if (!MY_PORT->lines) {
    prfmsg(MSG697);
    Gang_Menu();
    EXIT0;
  }

  Put_Gang_Message(msg, MY_PORT->gang);
  Clear_Display_Window();
  STATE = MAIN_PROMPT;
  MY_PORT->prompt_y = PROMPT_Y;
  prfmsg(MSG698);
  Gang_Menu();
  EXIT0;
}
/****************************************************************************
    put a message in a gang forum
*/
short Put_Gang_Message(struct freenet_struct *msg, short g) {
  short   n;
  long    msg_num;
  FILE    *fp;
  sprintf(str, GANG_MAIL_FILE, g);
  fp = fopen(str, READB);

  if (fp == NULL) {
    Create_New(str);
    fp = fopen(str, READB);

    if (fp == NULL) {
      EXIT0;
    }
  }

  n = fread(&glob->header, FREENET_SIZE, 1, fp);

  if (n != 1) {
    fclose(fp);
    fp = fopen(str, APPENDB);
    setmem(&glob->header, FREENET_SIZE, 0);
    fwrite(&glob->header, FREENET_SIZE, 1, fp);
    fclose(fp);
    fp = fopen(str, READB);
    msg_num = 1;
  } else {
    fseek(fp, (long) FREENET_SIZE * (-1L), SEEK_END);
    fread(&glob->dummy, FREENET_SIZE, 1, fp);
    msg_num = glob->dummy.msg_num + 1;
  }

  msg->msg_num = msg_num;

  if (glob->header.total == FREENET_MAX) {
    for (n = 2; n <= FREENET_MAX; ++n) {
      fseek(fp, (long) n * (long) FREENET_SIZE, SEEK_SET);
      fread(&glob->dummy, FREENET_SIZE, 1, fp);
      fseek(fp, (long) (n - 1) * (long) FREENET_SIZE, SEEK_SET);
      fwrite(&glob->dummy, FREENET_SIZE, 1, fp);
    }

    fseek(fp, (long) FREENET_MAX * (long) FREENET_SIZE, SEEK_SET);
    fwrite(msg, FREENET_SIZE, 1, fp);
  } else {
    fclose(fp);
    fp = fopen(str, APPENDB);
    ++glob->header.total;
    fwrite(msg, FREENET_SIZE, 1, fp);
    fclose(fp);
    fp = fopen(str, READB);
  }

  glob->header.msg_num = msg_num;
  fseek(fp, 0L, SEEK_SET);
  fwrite(&glob->header, FREENET_SIZE, 1, fp);
  fclose(fp);
  EXIT0;
}
/*****************************************************************************
    inform user of unread gang messages
*/
short New_Gang_Mail(void) {
  short   n;
  FILE    *fp;

  if (me.gang == 0) {
    EXIT0;
  }

  sprintf(str, GANG_MAIL_FILE, me.gang);
  fp = fopen(str, READB);

  if (fp == NULL) {
    Create_New(str);
    me.gang_mail = 0;
    EXIT0;
  }

  n = fread(&glob->header, FREENET_SIZE, 1, fp);
  fclose(fp);

  if (n == 0) {
    me.gang_mail = 0;
    EXIT0;
  }

  if (glob->header.msg_num < me.gang_mail) {
    me.gang_mail = glob->header.msg_num;
    EXIT0;
  }

  if (glob->header.msg_num > me.gang_mail) {
    prfmsg(MSG832);
  }

  EXIT0;
}
/****************************************************************************
    during cleanup, pay off the gangs
*/
short Gang_Scoring(void) {
  short   i;
  short   index = -1;
  long    ltemp;
  short   temp;
  long    lowest;
  short   low_index;
  short   n;
  short   done;
  short   indexes[5];
  long    score[5];
  FILE    *fp;
  FILE    *ufp;
  fp = fopen(GANG_FILE, READB);

  for (n = 0; n != 5; ++n) {
    indexes[n] = -1;
    score[n] = -1;
  }

  do {
    ++index;
    fseek(fp, (long) index * GANG_SIZE, SEEK_SET);
    n = fread(gang, GANG_SIZE, 1, fp);

    if
    (
      n == 1 &&
      !sameas(gang->name, OPEN_RECORD) &&
      gang->pts_today
    ) {
      for (i = done = 0; i != 5 && !done; ++i) {
        if (indexes[i] == -1) {
          indexes[i] = gang->index;
          score[i] = gang->pts_today;
          done = TRUE;
        }
      }

      if (!done) {
        lowest = -1;

        for (i = 0; i != 5; ++i) {
          if (score[i] < lowest || lowest == -1) {
            lowest = score[i];
            low_index = i;
          }
        }

        if (gang->pts_today > lowest) {
          score[low_index] = gang->pts_today;
          indexes[low_index] = gang->index;
        }
      }
    }

    if (n == 1) {
      gang->pts_today = 0;
      fseek(fp, (long) index * GANG_SIZE, SEEK_SET);
      fwrite(gang, GANG_SIZE, 1, fp);
    }
  } while (n == 1);

  for (i = 0; i != 5; ++i) {
    for (n = 0; n != 5; ++n) {
      if (score[n] < score[i]) {
        ltemp = score[n];
        score[n] = score[i];
        score[i] = ltemp;
        temp = indexes[n];
        indexes[n] = indexes[i];
        indexes[i] = temp;
      }
    }
  }

  for (i = 0; i != 5; ++i) {
    if (score[i] > 0 && glob->gangbux[i]) {
      fseek(fp, (long) indexes[i] * GANG_SIZE, SEEK_SET);
      fread(gang, GANG_SIZE, 1, fp);

      if (gang->scorers) {
        ufp = fopen(USER_FILE, READB);
        fseek(ufp, (long) gang->ufile_index * HUMAN_SIZE, SEEK_SET);
        fread(a_human, HUMAN_SIZE, 1, ufp);
        fclose(ufp);

        if (i == 0) {
          strcpy(str, "1st Place Score");
        } else if (i == 1) {
          strcpy(str, "2nd Place Score");
        } else if (i == 2) {
          strcpy(str, "3rd Place Score");
        } else if (i == 3) {
          strcpy(str, "4th Place Score");
        } else if (i == 4) {
          strcpy(str, "5th Place Score");
        }

        Add_Bank_Record
        (
          a_human,
          (long) gang->scorers * (long) glob->gangbux[i],
          -1, -1,
          "AUTOMATED",
          str
        );
        gang->scorers = 0;
        fseek(fp, (long) indexes[i] * GANG_SIZE, SEEK_SET);
        fwrite(gang, GANG_SIZE, 1, fp);
      }
    }
  }

  fclose(fp);
  EXIT0;
}
/*****************************************************************************
    find a user record for the given gang file server code
*/
short Find_Gang_Server(struct human_struct *someone, char *internet_code) {
  short   n;
  short   g;
  short   index = -1;
  short   found = FALSE;
  FILE    *fp;
  fp = fopen(BRIEF_FILE, READB);

  do {
    n = fread(&glob->brief, BRIEF_SIZE, 1, fp);

    if
    (
      !sameas(glob->brief.userid, OPEN_RECORD) &&
      sameto(A_GANG, glob->brief.userid) &&
      sameas(glob->brief.internet_code, internet_code)
    ) {
      found = TRUE;
    }

    ++index;
  } while (n == 1 && !found);

  fclose(fp);

  if (n == 0) {
    index = -1;
  } else {
    fp = fopen(USER_FILE, READB);
    fseek(fp, (long) index * HUMAN_SIZE, SEEK_SET);
    fread(someone, HUMAN_SIZE, 1, fp);
    fclose(fp);
    g = INUM(&someone->userid[5]);
    Read_Gang(g);

    if (gang->locked[MY_PORT->net_x][MY_PORT->net_y]) {
      index = -1;
    }
  }

  return (index);
}
/*****************************************************************************
    find a random gang record (for hacking purposes)
*/
short Find_Random_Gang(char *bank_code, char *server_code) {
  short   n;
  short   index = 0;
  FILE    *fp;
  fp = fopen(BRIEF_FILE, READB);

  do {
    n = fread(&glob->brief, BRIEF_SIZE, 1, fp);

    if (sameto(A_GANG, glob->brief.userid)) {
      glob->random_user[index] = glob->brief.index;
      ++index;
    }
  } while (n == 1 && index != MAX_RANDOM_USERS);

  fclose(fp);

  if (index == 0) {
    return (-1);
  }

  fp = fopen(USER_FILE, READB);
  n = glob->random_user[RANDOM(index) - 1];
  fseek(fp, (long) n * HUMAN_SIZE, SEEK_SET);
  fread(a_human, HUMAN_SIZE, 1, fp);
  fclose(fp);
  strcpy(bank_code, a_human->bank.code);
  strcpy(server_code, a_human->internet.code);
  return (n);
}
/***************************************************************************
    gang services (non-member)
*/
short Gang_Services1(void) {
  short   i;
  short   n;
  short   x, y;
  short   done = FALSE;
  short   index = -1;
  FILE    *fp;
  Clear_Display_Window();

  if (sameas(margv[0], CANCEL)) {
    STATE = MAIN_PROMPT;
    EXIT0;
  }

  if (sameas(margv[0], "A")) {
    List_Gangs();
    EXIT0;
  }

  if (sameas(margv[0], "B")) {
    Prompt(getmsg(MSG740), MEMBERS_PROMPT);
    EXIT0;
  }

  if (sameas(margv[0], "C")) {
    fp = fopen(GANG_FILE, READB);

    if (fp == NULL) {
      catastro("LoC: Gang File Open Failure!");
    }

    do {
      n = fread(gang, GANG_SIZE, 1, fp);
      ++index;

      if (n == 1 && sameas(gang->name, OPEN_RECORD)) {
        done = TRUE;
      }
    } while (n == 1 && !done);

    fclose(fp);
    gang->members = 1;
    gang->newbie = 1;
    gang->pts = 0;
    gang->pts_today = 0;
    gang->scorers = 0;
    strcpy(gang->rank[0], "Leader");

    for (i = 0; i != MAX_GANG_PERMS; ++i) {
      gang->perm[0][i] = TRUE;
    }

    for (i = 1; i != GANG_RANKS; ++i) {
      strcpy(gang->rank[i], "Member");
      gang->perm[i][RENAME_PERM] = FALSE;
      gang->perm[i][WITHDRAW_PERM] = TRUE;
      gang->perm[i][SET_RANK_PERM] = FALSE;
      gang->perm[i][INVITE_PERM] = FALSE;
      gang->perm[i][KICK_PERM] = FALSE;
      gang->perm[i][CODE_PERM] = FALSE;
    }

    for (x = 0; x != MAX_NET_X; ++x) {
      for (y = 0; y != MAX_NET_Y; ++y) {
        gang->locked[x][y] = FALSE;
      }
    }

    if (done) {
      gang->index = index;
      gang->ufile_index = Init_Gang_Guy(index);
      sprintf(gang->name, "Gang #%d", index);
      fp = fopen(GANG_FILE, READB);
      fseek(fp, (long) index * GANG_SIZE, SEEK_SET);
      fwrite(gang, GANG_SIZE, 1, fp);
      fclose(fp);
    } else {
      fp = fopen(GANG_FILE, APPENDB);

      if (index == 0) {
        gang->index = index;
        sprintf(gang->name, "Gang #%d", index);
        fwrite(gang, GANG_SIZE, 1, fp);
        ++index;
      }

      gang->index = index;
      gang->ufile_index = Init_Gang_Guy(index);
      sprintf(gang->name, "Gang #%d", index);
      fwrite(gang, GANG_SIZE, 1, fp);
      fclose(fp);
    }

    me.gang = index;
    me.rank = 0;
    me.gang_bank = 0;
    me.uploads = 0;
    me.invite = 0;
    me.gang_pts = 0;
    me.gang_pts_today = 0;
    MY_PORT->gang_pts = 0;
    Write_My_Rec(EXISTS);
    sprintf(str, GANG_MAIL_FILE, me.gang);
    unlink(str);
    Create_New(str);
    prfmsg(MSG734);
  }

  if (sameas(margv[0], "D")) {
    Prompt(getmsg(MSG833), POST_GANG_PROMPT1);
    EXIT0;
  }

  Gang_Menu();
  EXIT0;
}
/***************************************************************************
    read my gang's data into the file server MU's
*/
short Upload_Gang_Data(short s, short g) {
  short   x, y;
  short   m;
  short   i;
  FILE    *fp;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && Port[i]->sys_num == s && Inside(i)) {
      return (FALSE);
    }
  }

  for (i = 0; i != max_systems; ++i) {
    if
    (
      Sys[i]->in_use &&
      Sys[i]->type == FILE_SERVER &&
      Proc[i]->file_server == g
    ) {
      return (FALSE);
    }
  }

  Set_System_Path
  (
    Sys[s]->city_x, Sys[s]->city_y,
    Sys[s]->net_x, Sys[s]->net_y
  );
  fp = fopen(glob->path, READB);
  fread(a_system, SYSTEM_SIZE, 1, fp);

  for (m = 0; m != MAX_MUS; ++m) {
    for (i = 0; i != MU_CAPACITY; ++i) {
      a_system->memory[m][i] = Sys[s]->memory[m][i];
    }
  }

  fseek(fp, 0L, SEEK_SET);
  fwrite(a_system, SYSTEM_SIZE, 1, fp);
  fclose(fp);
  fp = fopen(DECK_FILE, READB);
  fseek(fp, (long) g * SYSTEM_SIZE, SEEK_SET);
  fread(a_system, SYSTEM_SIZE, 1, fp);
  fclose(fp);

  for (m = 0; m != MAX_MUS; ++m) {
    for (i = 0; i != MU_CAPACITY; ++i) {
      Sys[s]->memory[m][i] = a_system->memory[m][i];
    }
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      if
      (
        Sys[s]->node_type[x][y] == MEMORY &&
        Sys[s]->node_index[x][y] >= a_system->mus
      ) {
        Sys[s]->node_type[x][y] = DEAD_MEMORY;
      }
    }
  }

  Update_Others
  (
    SYSTEM_VIEW,
    Sys[s]->net_x, Sys[s]->net_y,
    Sys[s]->city_x, Sys[s]->city_y
  );
  Proc[s]->file_server = g;
  return (TRUE);
}
/***************************************************************************
    see if I'm that last one out of a file server
*/
short File_Server_Check(short s) {
  short   g;

  if (Sys[s]->type != FILE_SERVER || Proc[s]->file_server == 0) {
    EXIT0;
  }

  for (g = 0; g != max_guys; ++g) {
    if (Port[g]->active && Port[g]->sys_num == s && Inside(g)) {
      EXIT0;
    }
  }

  Download_Gang_Data(s);
  EXIT0;
}
/***************************************************************************
    write my gang's data from the file server MU's
*/
short Download_Gang_Data(short s) {
  short   x, y;
  short   m;
  short   i;
  FILE    *fp;

  if (Sys[s]->type != FILE_SERVER || Proc[s]->file_server == 0) {
    EXIT0;
  }

  fp = fopen(DECK_FILE, READB);
  fseek(fp, (long) Proc[s]->file_server * SYSTEM_SIZE, SEEK_SET);
  fread(a_system, SYSTEM_SIZE, 1, fp);

  for (m = 0; m != MAX_MUS; ++m) {
    for (i = 0; i != MU_CAPACITY; ++i) {
      a_system->memory[m][i] = Sys[s]->memory[m][i];
    }
  }

  fseek(fp, (long) Proc[s]->file_server * SYSTEM_SIZE, SEEK_SET);
  fwrite(a_system, SYSTEM_SIZE, 1, fp);
  fclose(fp);
  Set_System_Path
  (
    Sys[s]->city_x, Sys[s]->city_y,
    Sys[s]->net_x, Sys[s]->net_y
  );
  fp = fopen(glob->path, READB);
  fread(a_system, SYSTEM_SIZE, 1, fp);
  fclose(fp);

  for (m = 0; m != MAX_MUS; ++m) {
    for (i = 0; i != MU_CAPACITY; ++i) {
      Sys[s]->memory[m][i] = a_system->memory[m][i];
    }
  }

  for (x = 0; x != MAX_SYS_X; ++x) {
    for (y = 0; y != MAX_SYS_Y; ++y) {
      if (Sys[s]->node_type[x][y] == DEAD_MEMORY) {
        Sys[s]->node_type[x][y] = MEMORY;
      } else if (Sys[s]->node_type[x][y] == DEAD_WALL) {
        Sys[s]->node_type[x][y] = DATA_WALL;
      } else if (Sys[s]->node_type[x][y] == OPENED_GATE) {
        Sys[s]->node_type[x][y] = CLOSED_GATE;
      }
    }
  }

  Update_Others
  (
    SYSTEM_VIEW,
    Sys[s]->net_x, Sys[s]->net_y,
    Sys[s]->city_x, Sys[s]->city_y
  );
  Proc[s]->file_server = 0;
  EXIT0;
}
/***************************************************************************
    gang services (member)
*/
short Gang_Services2(void) {
  FILE    *fp;
  Clear_Display_Window();

  if (sameas(margv[0], CANCEL)) {
    STATE = MAIN_PROMPT;
  } else if (sameas(margv[0], "A")) {
    List_Gangs();
  } else if (sameas(margv[0], "B")) {
    Prompt(getmsg(MSG741), MEMBERS_PROMPT);
  } else if (sameas(margv[0], "C")) {
    Prompt(getmsg(MSG736), RESIGN_PROMPT);
  } else if (sameas(margv[0], "D")) {
    if (MY_PORT->access_level != IDLE) {
      prfmsg(MSG184);
      Gang_Menu();
    } else {
      prf("%c%s", CTRL_S, CLEAR_SCREEN);
      Read_Gang(me.gang);
      fp = fopen(USER_FILE, READB);
      fseek(fp, (long) gang->ufile_index * HUMAN_SIZE, SEEK_SET);
      fread(a_human, HUMAN_SIZE, 1, fp);
      fclose(fp);
      fp = fopen(DECK_FILE, READB);
      fseek(fp, (long) gang->ufile_index * SYSTEM_SIZE, SEEK_SET);
      fread(a_system, SYSTEM_SIZE, 1, fp);
      fclose(fp);

      if (!gang->perm[me.rank][CODE_PERM]) {
        strcpy(a_human->bank.code, "<secret>");
      }

      prfmsg
      (
        MSG806,
        gang->name,
        gang->members,
        a_human->internet.code,
        a_system->mus,
        a_human->bank.code,
        l2as(a_human->bank.balance),
        l2as(me.gang_bank),
        gang->rank[me.rank],
        gang->rank[0],
        gang->rank[1],
        gang->rank[2],
        gang->rank[3],
        gang->rank[4],
        gang->rank[5]
      );
      MY_PORT->prompt_y = ASYNCH_Y - 1;
      Prompt(getmsg(MSG207), GANG_INFO_PROMPT);
    }
  } else if (sameas(margv[0], "E")) {
    if (MY_PORT->access_level != IDLE) {
      prfmsg(MSG184);
      Gang_Menu();
    } else {
      Clear_Display_Window();
      Gang_Bank();
    }
  } else if (sameas(margv[0], "F")) {
    Read_Gang(me.gang);

    if (!gang->perm[me.rank][SET_RANK_PERM]) {
      prfmsg(MSG772);
      Gang_Menu();
    } else {
      Prompt(getmsg(MSG773), SET_RANK_PROMPT1);
    }
  } else if (sameas(margv[0], "G")) {
    Read_Gang(me.gang);

    if (!gang->perm[me.rank][RENAME_PERM]) {
      prfmsg(MSG770);
      Gang_Menu();
    } else {
      Prompt(getmsg(MSG745), INVITE_PROMPT);
    }
  } else if (sameas(margv[0], "H")) {
    Read_Gang(me.gang);

    if (!gang->perm[me.rank][KICK_PERM]) {
      prfmsg(MSG771);
      Gang_Menu();
    } else {
      Prompt(getmsg(MSG755), KICK_PROMPT);
    }
  } else if (sameas(margv[0], "I")) {
    if (me.rank != 0) {
      prfmsg(MSG766);
      Gang_Menu();
    } else {
      Prompt(getmsg(MSG765), NEW_LEADER_PROMPT);
    }
  } else if (sameas(margv[0], "J")) {
    Read_Gang(me.gang);

    if (!gang->perm[me.rank][RENAME_PERM]) {
      prfmsg(MSG769);
      Gang_Menu();
    } else {
      Prompt(getmsg(MSG762), GANG_NAME_PROMPT);
    }
  } else if (sameas(margv[0], "K")) {
    prfmsg(MSG804, glob->memcost);
    Prompt(getmsg(MSG805), SERVER_PROMPT);
  } else if (sameas(margv[0], "L")) {
    if (me.rank != 0 && !gang->perm[me.rank][SET_RANK_PERM]) {
      prfmsg(MSG772);
      Gang_Menu();
    } else if (MY_PORT->access_level != IDLE) {
      prfmsg(MSG184);
      Gang_Menu();
    } else {
      Edit_Gang_Ranks();
    }
  } else if (sameas(margv[0], "M")) {
    Read_Gang_Mail();
  } else if (sameas(margv[0], "N")) {
    Prompt(getmsg(MSG834), POST_GANG_PROMPT2);
  } else {
    Gang_Menu();
  }

  EXIT0;
}
/***************************************************************************
    gang bank operations
*/
short Gang_Bank(void) {
  short   i;
  long    amt;
  FILE    *fp;

  if (STATE == GANG_LEDGER_PROMPT) {
    MY_PORT->prompt_y = PROMPT_Y;
    MY_PORT->prompt_x = 1;
    Redraw();
    prf(NL);
  } else {
    Clear_Display_Window();
  }

  if (me.gang == 0) {
    STATE = MAIN_PROMPT;
    EXIT0;
  } else if (STATE == GANG_BANK_PROMPT) {
    if (margc == 0 || sameas(margv[0], CANCEL)) {
      Gang_Menu();
      EXIT0;
    } else if (sameas(margv[0], "A")) {
      Prompt(getmsg(MSG781), GANG_DEPOSIT_PROMPT);
      EXIT0;
    } else if (sameas(margv[0], "B")) {
      Read_Gang(me.gang);

      if (!gang->perm[me.rank][WITHDRAW_PERM]) {
        prfmsg(MSG785);
      } else {
        Prompt(getmsg(MSG784), GANG_WITHDRAW_PROMPT);
        EXIT0;
      }
    } else if (sameas(margv[0], "C")) {
      Gang_Ledger();
      EXIT0;
    } else if (sameas(margv[0], "D")) {
      Read_Gang(me.gang);

      if (!gang->perm[me.rank][CODE_PERM]) {
        prfmsg(MSG788);
      } else {
        Prompt(getmsg(MSG789), GANG_CODE_PROMPT);
        EXIT0;
      }
    } else if (sameas(margv[0], "E")) {
      Read_Gang(me.gang);
      fp = fopen(USER_FILE, READB);
      fseek(fp, (long) gang->ufile_index * HUMAN_SIZE, SEEK_SET);
      fread(a_human, HUMAN_SIZE, 1, fp);
      fclose(fp);

      if (!gang->perm[me.rank][CODE_PERM]) {
        strcpy(a_human->bank.code, "<secret>");
      }

      sprintf
      (
        str,
        getmsg(MSG791),
        a_human->bank.balance,
        me.gang_bank,
        a_human->bank.code
      );
      prf(str);
      Prompt(getmsg(MSG207), GANG_SUMMARY_PROMPT);
      EXIT0;
    }
  } else if (STATE == GANG_CODE_PROMPT && margc && !sameas(margv[0], CANCEL)) {
    strcpy(str, margv[0]);
    str[CODE_SIZE - 1] = 0;
    i = Find_Bank_User(a_human, str);

    if (i >= 0) {
      prfmsg(MSG154, str);
    } else {
      Read_Gang(me.gang);
      fp = fopen(USER_FILE, READB);
      fseek(fp, (long) gang->ufile_index * HUMAN_SIZE, SEEK_SET);
      fread(a_human, HUMAN_SIZE, 1, fp);
      strcpy(a_human->bank.code, str);
      fseek(fp, (long) gang->ufile_index * HUMAN_SIZE, SEEK_SET);
      fwrite(a_human, HUMAN_SIZE, 1, fp);
      fclose(fp);
      Set_Brief(a_human, &glob->brief);
      fp = fopen(BRIEF_FILE, READB);
      fseek(fp, (long) gang->ufile_index * BRIEF_SIZE, SEEK_SET);
      fwrite(&glob->brief, BRIEF_SIZE, 1, fp);
      fclose(fp);
      prfmsg(MSG790, a_human->bank.code);
    }
  } else if (STATE == GANG_DEPOSIT_PROMPT && margc) {
    amt = LNUM(margv[0]);

    if (amt > me.bank.balance) {
      sprintf(str, getmsg(MSG782), me.bank.balance);
      prf(str);
    } else if (amt) {
      me.gang_bank += amt;
      Add_Bank_Record
      (
        &me,
        -(amt),
        me.start_x, me.start_y,
        me.alias,
        "Gang Deposit"
      );
      Read_Gang(me.gang);
      fp = fopen(USER_FILE, READB);
      fseek(fp, (long) gang->ufile_index * HUMAN_SIZE, SEEK_SET);
      fread(a_human, HUMAN_SIZE, 1, fp);
      fclose(fp);
      Add_Bank_Record
      (
        a_human,
        amt,
        me.start_x, me.start_y,
        me.alias,
        "Deposit"
      );
      sprintf(str, getmsg(MSG783), amt);
      prf(str);
    }
  } else if (STATE == GANG_WITHDRAW_PROMPT && margc) {
    Read_Gang(me.gang);
    fp = fopen(USER_FILE, READB);
    fseek(fp, (long) gang->ufile_index * HUMAN_SIZE, SEEK_SET);
    fread(a_human, HUMAN_SIZE, 1, fp);
    fclose(fp);
    amt = LNUM(margv[0]);

    if (amt > a_human->bank.balance) {
      sprintf(str, getmsg(MSG786), a_human->bank.balance);
      prf(str);
    } else if (amt) {
      me.gang_bank -= amt;
      Add_Bank_Record
      (
        &me,
        amt,
        me.start_x, me.start_y,
        me.alias,
        "Gang Withdrawl"
      );
      Add_Bank_Record
      (
        a_human,
        -(amt),
        me.start_x, me.start_y,
        me.alias,
        "Withdrawl"
      );
      sprintf(str, getmsg(MSG787), amt);
      prf(str);
    }
  }

  prfmsg(MSG779);
  Prompt(getmsg(MSG780), GANG_BANK_PROMPT);
  EXIT0;
}
/***************************************************************************
    gang file server operations
*/
short Gang_Server(void) {
  short   m;
  short   i;
  short   ok;
  FILE    *fp;
  Clear_Display_Window();

  if (me.gang == 0) {
    STATE = MAIN_PROMPT;
    EXIT0;
  }

  if (STATE == SERVER_PROMPT) {
    if (margc == 0 || sameas(margv[0], CANCEL)) {
      Gang_Menu();
      EXIT0;
    }

    if (sameas(margv[0], "A")) {
      Read_Gang(me.gang);

      if (!gang->perm[me.rank][CODE_PERM]) {
        prfmsg(MSG808);
      } else {
        Prompt(getmsg(MSG809), SERVER_CODE_PROMPT);
        EXIT0;
      }
    } else if (sameas(margv[0], "B")) {
      Read_Gang(me.gang);
      fp = fopen(DECK_FILE, READB);
      fseek(fp, (long) gang->ufile_index * SYSTEM_SIZE, SEEK_SET);
      fread(a_system, SYSTEM_SIZE, 1, fp);
      fclose(fp);
      fp = fopen(USER_FILE, READB);
      fseek(fp, (long) gang->ufile_index * HUMAN_SIZE, SEEK_SET);
      fread(a_human, HUMAN_SIZE, 1, fp);
      fclose(fp);

      if (a_system->mus == MAX_MUS) {
        prfmsg(MSG811, MAX_MUS);
      } else if
      (
        a_human->bank.balance < glob->memcost &&
        !me.perm[FREE_PERM]
      ) {
        prfmsg(MSG812);
      } else {
        m = a_system->mus;
        ok = Add_Memory(a_system, m);

        if (!ok) {
          prfmsg(MSG813);
        } else {
          for (i = 0; i != MU_CAPACITY; ++i) {
            a_system->memory[m][i].packed = FALSE;
            a_system->memory[m][i].lock = FALSE;
            a_system->memory[m][i].type = 0;
            a_system->memory[m][i].index = 0;
            a_system->memory[m][i].name[0] = 0;
          }

          ++a_system->mus;
          fp = fopen(DECK_FILE, READB);
          fseek(fp, (long) gang->ufile_index * SYSTEM_SIZE, SEEK_SET);
          fwrite(a_system, SYSTEM_SIZE, 1, fp);
          fclose(fp);

          if (!me.perm[FREE_PERM]) {
            Add_Bank_Record
            (
              a_human,
              (long) - (glob->memcost),
              me.start_x, me.start_y,
              me.alias,
              "Memory Purchase"
            );
          }

          prfmsg(MSG814, a_system->mus);
        }
      }
    } else if (sameas(margv[0], "C")) {
      if (!gang->perm[me.rank][CODE_PERM]) {
        prfmsg(MSG926);
      } else {
        World_View();
        i = Gang_Lock_List();
        sprintf(str, getmsg(MSG817), i);
        Prompt(str, GANG_LOCK_PROMPT1);
        EXIT0;
      }
    }
  } else if
  (
    STATE == SERVER_CODE_PROMPT &&
    margc &&
    !sameas(margv[0], CANCEL)
  ) {
    strcpy(str, margv[0]);
    str[CODE_SIZE - 1] = 0;
    i = Find_Internet_User(a_human, str);

    if (i >= 0) {
      prfmsg(MSG154, str);
    } else {
      Read_Gang(me.gang);
      fp = fopen(USER_FILE, READB);
      fseek(fp, (long) gang->ufile_index * HUMAN_SIZE, SEEK_SET);
      fread(a_human, HUMAN_SIZE, 1, fp);
      strcpy(a_human->internet.code, str);
      fseek(fp, (long) gang->ufile_index * HUMAN_SIZE, SEEK_SET);
      fwrite(a_human, HUMAN_SIZE, 1, fp);
      fclose(fp);
      Set_Brief(a_human, &glob->brief);
      fp = fopen(BRIEF_FILE, READB);
      fseek(fp, (long) gang->ufile_index * BRIEF_SIZE, SEEK_SET);
      fwrite(&glob->brief, BRIEF_SIZE, 1, fp);
      fclose(fp);
      prfmsg(MSSG810, a_human->internet.code);
    }
  } else if
  (
    STATE == GANG_LOCK_PROMPT1 &&
    (margc == 0 || sameas(margv[0], CANCEL))
  ) {
    if (MY_PORT->access_level == IDLE) {
      Clear_Net_Display(CLEAR_ALL);
    } else {
      Look("X");
    }
  } else if (STATE == GANG_LOCK_PROMPT1 && margc && !sameas(margv[0], CANCEL)) {
    i = INUM(margv[0]);
    ok = Gang_Lock_Find(i);

    if (ok) {
      Read_Gang(me.gang);

      if (gang->locked[MY_PORT->target_x][MY_PORT->target_y]) {
        gang->locked[MY_PORT->target_x][MY_PORT->target_y] = FALSE;
        prfmsg
        (
          MSG818,
          world->city[MY_PORT->target_x][MY_PORT->target_y].name
        );
      } else {
        gang->locked[MY_PORT->target_x][MY_PORT->target_y] = TRUE;
        prfmsg
        (
          MSG819,
          world->city[MY_PORT->target_x][MY_PORT->target_y].name
        );
      }

      Write_Gang(me.gang);
      i = Gang_Lock_List();
      sprintf(str, getmsg(MSG817), i);
      Prompt(str, GANG_LOCK_PROMPT1);
      EXIT0;
    }
  }

  prfmsg(MSG804, glob->memcost);
  Prompt(getmsg(MSG805), SERVER_PROMPT);
  EXIT0;
}
/***************************************************************************
    list gang file server restrictions
*/
short Gang_Lock_List(void) {
  short   x;
  short   y;
  short   n = 1;
  short   col;
  Read_Gang(me.gang);

  for (x = col = 0; x != MAX_NET_X; ++x) {
    for (y = 0; y != MAX_NET_Y; ++y) {
      if
      (
        sameas(world->city[x][y].name, FRONTIER) ||
        sameas(world->city[x][y].name, VALHALLA) ||
        sameas(world->city[x][y].name, ALIEN_CITY) ||
        sameas(world->city[x][y].name, CYBERIA) ||
        (world->city[x][y].invisible && !me.perm[GAMEOP_PERM])
      ) {
        if (gang->locked[x][y]) {
          gang->locked[x][y] = FALSE;
          Write_Gang(me.gang);
        }
      } else {
        if (gang->locked[x][y]) {
          prfmsg(MSG816, n, world->city[x][y].name);
        } else {
          prfmsg(MSG815, n, world->city[x][y].name);
        }

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
/***************************************************************************
    find the city I've selected to lock/unlock
*/
short Gang_Lock_Find(short city) {
  short   x;
  short   y;
  short   n = 1;
  Read_Gang(me.gang);

  for (x = 0; x != MAX_NET_X; ++x) {
    for (y = 0; y != MAX_NET_Y; ++y) {
      if
      (
        sameas(world->city[x][y].name, FRONTIER) ||
        sameas(world->city[x][y].name, VALHALLA) ||
        sameas(world->city[x][y].name, ALIEN_CITY) ||
        sameas(world->city[x][y].name, CYBERIA) ||
        (world->city[x][y].invisible && !me.perm[GAMEOP_PERM])
      ) {
        if (gang->locked[x][y]) {
          gang->locked[x][y] = FALSE;
          Write_Gang(me.gang);
        }
      } else {
        if (n == city) {
          MY_PORT->target_x = x;
          MY_PORT->target_y = y;
          return (TRUE);
        }

        ++n;
      }
    }
  }

  return (FALSE);
}
/***************************************************************************
    user is editing gang ranks
*/
short Edit_Gang_Ranks(void) {
  short   ok;
  ok = Read_Gang(me.gang);

  if (!ok) {
    STATE = MAIN_PROMPT;
    EXIT0;
  }

  sprintf
  (
    vdatmp,
    gang_edit_format,
    gang->rank[0], '\0',
    gang->newbie, '\0',
    gang->rank[1], '\0',
    y_or_n[gang->perm[1][RENAME_PERM]], '\0',
    y_or_n[gang->perm[1][WITHDRAW_PERM]], '\0',
    y_or_n[gang->perm[1][SET_RANK_PERM]], '\0',
    y_or_n[gang->perm[1][INVITE_PERM]], '\0',
    y_or_n[gang->perm[1][KICK_PERM]], '\0',
    y_or_n[gang->perm[1][CODE_PERM]], '\0',
    gang->rank[2], '\0',
    y_or_n[gang->perm[2][RENAME_PERM]], '\0',
    y_or_n[gang->perm[2][WITHDRAW_PERM]], '\0',
    y_or_n[gang->perm[2][SET_RANK_PERM]], '\0',
    y_or_n[gang->perm[2][INVITE_PERM]], '\0',
    y_or_n[gang->perm[2][KICK_PERM]], '\0',
    y_or_n[gang->perm[2][CODE_PERM]], '\0',
    gang->rank[3], '\0',
    y_or_n[gang->perm[3][RENAME_PERM]], '\0',
    y_or_n[gang->perm[3][WITHDRAW_PERM]], '\0',
    y_or_n[gang->perm[3][SET_RANK_PERM]], '\0',
    y_or_n[gang->perm[3][INVITE_PERM]], '\0',
    y_or_n[gang->perm[3][KICK_PERM]], '\0',
    y_or_n[gang->perm[3][CODE_PERM]], '\0',
    gang->rank[4], '\0',
    y_or_n[gang->perm[4][RENAME_PERM]], '\0',
    y_or_n[gang->perm[4][WITHDRAW_PERM]], '\0',
    y_or_n[gang->perm[4][SET_RANK_PERM]], '\0',
    y_or_n[gang->perm[4][INVITE_PERM]], '\0',
    y_or_n[gang->perm[4][KICK_PERM]], '\0',
    y_or_n[gang->perm[4][CODE_PERM]], '\0',
    gang->rank[5], '\0',
    y_or_n[gang->perm[5][RENAME_PERM]], '\0',
    y_or_n[gang->perm[5][WITHDRAW_PERM]], '\0',
    y_or_n[gang->perm[5][SET_RANK_PERM]], '\0',
    y_or_n[gang->perm[5][INVITE_PERM]], '\0',
    y_or_n[gang->perm[5][KICK_PERM]], '\0',
    y_or_n[gang->perm[5][CODE_PERM]], '\0'
  );
  clrprf();
  MY_PORT->editing = TRUE;
  STATE = EDTGANG;
  fsdroom(EDTGANG, gang_screen_spec, 1);
  fsdapr(vdaptr, vdasiz, vdatmp);
  fsdbkg(fsdrft());
  fsdego(vfyadn, Gang_Edit_Done);
  My_Outprf(MY_PORT->usrnum);
  EXIT0;
}
/***************************************************************************
    user is changing the gang name
*/
short Rename_Gang(void) {
  short   n;
  FILE    *fp;
  rstrin();
  strncpy(str, input, GANG_NAME_SIZE - 1);
  str[GANG_NAME_SIZE - 1] = 0;

  if (sameas(str, OPEN_RECORD)) {
    prfmsg(MSG764);
    Gang_Menu();
    EXIT0;
  }

  fp = fopen(GANG_FILE, READB);

  do {
    n = fread(gang, GANG_SIZE, 1, fp);

    if
    (
      n == 1 &&
      sameas(gang->name, str) &&
      gang->index != me.gang
    ) {
      fclose(fp);
      prfmsg(MSG764);
      Gang_Menu();
      EXIT0;
    }
  } while (n == 1);

  fseek(fp, (long) me.gang * GANG_SIZE, SEEK_SET);
  fread(gang, GANG_SIZE, 1, fp);
  strcpy(gang->name, str);
  fseek(fp, (long) me.gang * GANG_SIZE, SEEK_SET);
  fwrite(gang, GANG_SIZE, 1, fp);
  fclose(fp);
  prfmsg(MSG763, gang->name);
  Gang_Menu();
  EXIT0;
}
/***************************************************************************
    user has been invited into a gang
*/
short Accept(void) {
  short   ok;
  ok = Read_Gang(me.invite);

  if (!ok || sameas(gang->name, OPEN_RECORD)) {
    me.invite = 0;
    STATE = MAIN_PROMPT;
    EXIT0;
  }

  if (sameas(margv[0], ANS_YES)) {
    me.gang = me.invite;
    me.invite = 0;
    me.rank = gang->newbie;
    me.gang_bank = 0;
    me.uploads = 0;
    me.gang_pts = 0;
    me.gang_pts_today = 0;
    MY_PORT->gang_pts = 0;
    ++gang->members;
    Write_Gang(me.gang);
    Write_My_Rec(EXISTS);
    Gang_Menu();
  } else if (sameas(margv[0], ANS_NO)) {
    me.invite = 0;
    STATE = MAIN_PROMPT;
  } else {
    STATE = MAIN_PROMPT;
  }

  EXIT0;
}
/***************************************************************************
    user is inviting a new member into the gang
*/
short Invite(void) {
  short   i;

  if (margc == 0) {
    Gang_Menu();
    EXIT0;
  }

  Fix_Alias(MY_PORT->tstring);
  i = Find_Alias(a_human, MY_PORT->tstring);

  if (i < 0) {
    prfmsg(MSG176, MY_PORT->tstring);
    Gang_Menu();
    EXIT0;
  }

  if (a_human->index == me.index) {
    prfmsg(MSG751);
    Gang_Menu();
    EXIT0;
  }

  if (a_human->gang) {
    prfmsg(MSG746, a_human->alias);
    Gang_Menu();
    EXIT0;
  }

  if (a_human->invite == me.gang) {
    prfmsg(MSG750, a_human->alias);
    Gang_Menu();
    EXIT0;
  }

  if (a_human->invite) {
    prfmsg(MSG747, a_human->alias);
    Gang_Menu();
    EXIT0;
  }

  a_human->invite = me.gang;
  Update_Human(a_human);
  prfmsg(MSG749, a_human->alias);

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && sameas(Port[i]->guy.userid, a_human->userid)) {
      sprintf(str, getmsg(MSG748), me.alias);
      Send(str, i, TRUE);
      Gang_Menu();
      EXIT0;
    }
  }

  Gang_Menu();
  EXIT0;
}
/***************************************************************************
    user is kicking a member out of the gang
*/
short Kick_Out(void) {
  short   i;
  short   ok;

  if (margc == 0) {
    Gang_Menu();
    EXIT0;
  }

  Fix_Alias(MY_PORT->tstring);
  i = Find_Alias(a_human, MY_PORT->tstring);

  if (i < 0) {
    prfmsg(MSG176, MY_PORT->tstring);
    Gang_Menu();
    EXIT0;
  }

  if (a_human->index == me.index) {
    prfmsg(MSG751);
    Gang_Menu();
    EXIT0;
  }

  if (a_human->rank == 0) {
    prfmsg(MSG758);
    Gang_Menu();
    EXIT0;
  }

  if (a_human->gang == me.gang) {
    ok = Read_Gang(me.gang);

    if (ok) {
      --gang->members;
      Write_Gang(me.gang);
    }

    prfmsg(MSG756, a_human->alias);
    a_human->gang = 0;
    sprintf(str, getmsg(MSG757), me.alias);
  } else if (a_human->invite == me.gang) {
    prfmsg(MSG759, a_human->alias);
    a_human->invite = 0;
    sprintf(str, getmsg(MSG760), me.alias);
  } else {
    prfmsg(MSG761, a_human->alias);
    Gang_Menu();
    EXIT0;
  }

  Update_Human(a_human);

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && sameas(Port[i]->guy.userid, a_human->userid)) {
      Send(str, i, TRUE);
      Gang_Menu();
      EXIT0;
    }
  }

  Gang_Menu();
  EXIT0;
}
/***************************************************************************
    user is transferring gang leadership
*/
short New_Leader(void) {
  short   i;
  short   ok;

  if (margc == 0) {
    Gang_Menu();
    EXIT0;
  }

  Fix_Alias(MY_PORT->tstring);
  i = Find_Alias(a_human, MY_PORT->tstring);

  if (i < 0) {
    prfmsg(MSG176, MY_PORT->tstring);
    Gang_Menu();
    EXIT0;
  }

  if (a_human->index == me.index) {
    prfmsg(MSG751);
    Gang_Menu();
    EXIT0;
  }

  if (a_human->gang != me.gang) {
    prfmsg(MSG761, a_human->alias);
    Gang_Menu();
    EXIT0;
  }

  a_human->rank = 0;
  Update_Human(a_human);

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && sameas(Port[i]->guy.userid, a_human->userid)) {
      sprintf(str, getmsg(MSG767), me.alias);
      Send(str, i, TRUE);
    }
  }

  prfmsg(MSG768, a_human->alias);
  ok = Read_Gang(me.gang);

  if (ok) {
    me.rank = gang->newbie;
  }

  Write_My_Rec(EXISTS);
  Gang_Menu();
  EXIT0;
}
/***************************************************************************
    user is changing a member's rank
*/
short Set_Rank(void) {
  short   i;
  Clear_Display_Window();

  if (margc == 0 || me.gang == 0) {
    Gang_Menu();
    EXIT0;
  }

  if (STATE == SET_RANK_PROMPT1) {
    Fix_Alias(MY_PORT->tstring);
    i = Find_Alias(a_human, MY_PORT->tstring);

    if (i < 0) {
      prfmsg(MSG176, MY_PORT->tstring);
      Gang_Menu();
      EXIT0;
    }

    if (a_human->index == me.index) {
      prfmsg(MSG751);
      Gang_Menu();
      EXIT0;
    }

    if (a_human->gang != me.gang) {
      prfmsg(MSG761, a_human->alias);
      Gang_Menu();
      EXIT0;
    }

    if (a_human->rank == 0) {
      prfmsg(MSG774);
      Gang_Menu();
      EXIT0;
    }

    Read_Gang(me.gang);

    for (i = 1; i != GANG_RANKS; ++i) {
      prfmsg(MSG775, i, gang->rank[i]);
    }

    sprintf(str, getmsg(MSG776), a_human->alias, GANG_RANKS - 1);
    Prompt(str, SET_RANK_PROMPT2);
    EXIT0;
  }

  i = Find_Alias(a_human, MY_PORT->tstring);

  if (i < 0) {
    prfmsg(MSG176, MY_PORT->tstring);
    Gang_Menu();
    EXIT0;
  }

  i = INUM(margv[0]);

  if (i < 1 || i >= GANG_RANKS) {
    Gang_Menu();
    EXIT0;
  }

  a_human->rank = i;
  Update_Human(a_human);
  Read_Gang(me.gang);

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && sameas(Port[i]->guy.userid, a_human->userid)) {
      sprintf(str, getmsg(MSG777), me.alias, gang->rank[a_human->rank]);
      Send(str, i, TRUE);
    }
  }

  prfmsg(MSG778, a_human->alias, gang->rank[a_human->rank]);
  Gang_Menu();
  EXIT0;
}
/***************************************************************************
    user is quitting his gang
*/
short Resign(void) {
  short   ok;
  ok = Read_Gang(me.gang);

  if (!ok) {
    EXIT0;
  }

  if (me.rank == 0 && gang->members > 1) {
    prfmsg(MSG737);
    Gang_Menu();
    EXIT0;
  }

  --gang->members;

  if (gang->members == 0) {
    strcpy(gang->name, OPEN_RECORD);
  }

  Write_Gang(me.gang);
  me.gang = 0;
  Write_My_Rec(EXISTS);

  if (gang->members == 0) {
    prfmsg(MSG738);
    Write_Gang_Rec(SUICIDE);
  } else {
    prfmsg(MSG739);
  }

  STATE = MAIN_PROMPT;
  EXIT0;
}
/***************************************************************************
    gang member's character has been deleted
*/
short Dead_Gangster(short gang_num, short index, short rank) {
  short   n;
  short   ok;
  short   done = FALSE;
  FILE    *fp;
  ok = Read_Gang(gang_num);

  if (!ok) {
    EXIT0;
  }

  gang->members = 0;
  fp = fopen(BRIEF_FILE, READB);

  do {
    n = fread(&glob->brief, BRIEF_SIZE, 1, fp);

    if
    (
      n == 1 &&
      glob->brief.index != index &&
      glob->brief.gang == gang_num &&
      !sameas(glob->brief.userid, OPEN_RECORD) &&
      !sameto(A_GANG, glob->brief.userid)
    ) {
      ++gang->members;
    }
  } while (n == 1);

  fclose(fp);

  if (gang->members == 0) {
    strcpy(gang->name, OPEN_RECORD);
  }

  Write_Gang(gang_num);

  if (gang->members == 0) {
    Write_Gang_Rec(SUICIDE);
  } else if (rank == 0) {
    fp = fopen(BRIEF_FILE, READB);

    do {
      n = fread(&glob->brief, BRIEF_SIZE, 1, fp);

      if
      (
        n == 1 &&
        glob->brief.index != index &&
        glob->brief.gang == gang_num &&
        !sameas(glob->brief.userid, OPEN_RECORD) &&
        !sameto(A_GANG, glob->brief.userid)
      ) {
        done = TRUE;
        Read_Human(glob->brief.index, a_user);
        a_user->rank = 0;
        Update_Human(a_user);
      }
    } while (n == 1 && !done);

    fclose(fp);
  }

  EXIT0;
}
/***************************************************************************
    list members of a gang
*/
short Gang_Members(void) {
  short   lines = 0;
  short   i;
  short   n;
  short   ok;
  FILE    *fp;
  char    last_on[DATE_SIZE];
  char    bank[20];
  char    uploads[20];
  char    pts[20];

  if (STATE == MEMBERS_PROMPT) {
    if (!me.gang) {
      if (margc == 0) {
        Clear_Display_Window();
        Gang_Menu();
        EXIT0;
      }

      MY_PORT->target_type = INUM(margv[0]);
    } else {
      if (margc == 0) {
        MY_PORT->target_type = me.gang;
      } else {
        MY_PORT->target_type = INUM(margv[0]);
      }
    }

    if (MY_PORT->target_type <= 0) {
      prfmsg(MSG742, MY_PORT->target_type);
      Gang_Menu();
      EXIT0;
    }

    ok = Read_Gang(MY_PORT->target_type);

    if (!ok || sameas(gang->name, OPEN_RECORD)) {
      prfmsg(MSG742, MY_PORT->target_type);
      Gang_Menu();
      EXIT0;
    }

    MY_PORT->misc = 0;
  } else {
    if (margc && sameas(margv[0], ANS_NO)) {
      Clear_Display_Window();
      Gang_Menu();
      EXIT0;
    }
  }

  Read_Gang(MY_PORT->target_type);
  fp = fopen(BRIEF_FILE, READB);
  fseek(fp, (long) MY_PORT->misc * BRIEF_SIZE, SEEK_SET);
  Clear_Display_Window();

  do {
    n = fread(&glob->brief, BRIEF_SIZE, 1, fp);
    ++MY_PORT->misc;

    if
    (
      n == 1 &&
      glob->brief.gang == MY_PORT->target_type &&
      !sameas(glob->brief.userid, OPEN_RECORD)
    ) {
      CONV_DATE(last_on, glob->brief.last_on);
      sprintf
      (
        str,
        getmsg(M743),
        glob->brief.alias,
        gang->rank[glob->brief.rank],
        His_Rank(glob->brief.score, glob->brief.won),
        last_on
      );
      prf(str);
      ++lines;

      if (me.gang == glob->brief.gang) {
        for (i = 0; i != max_guys; ++i) {
          if
          (
            Port[i]->active &&
            Port[i]->guy.index == glob->brief.index
          ) {
            glob->brief.gang_pts = Port[i]->guy.gang_pts;
            glob->brief.gang_pts_today = Port[i]->guy.gang_pts_today;
            glob->brief.gang_bank = Port[i]->guy.gang_bank;
            glob->brief.uploads = Port[i]->guy.uploads;
          }
        }

        sprintf(pts, "%ld", glob->brief.gang_pts);
        sprintf(bank, "$%ld", glob->brief.gang_bank);
        sprintf(uploads, "%ld", glob->brief.uploads);
        sprintf
        (
          str,
          getmsg(MSG830),
          bank,
          uploads,
          pts,
          glob->brief.gang_pts_today
        );
        prf(str);
        ++lines;
      }
    }
  } while (n == 1 && lines < 8);

  fclose(fp);

  if (n != 1) {
    Prompt(getmsg(MSG207), NOMORE_MBR_PROMPT);
  } else {
    Prompt(getmsg(MSG243), MORE_MBR_PROMPT);
  }

  EXIT0;
}
/****************************************************************************
    finished entering gang rank data
*/
void Gang_Edit_Done(MSHORT save) {
  short   i;
  usrptr->state = glob->module_id;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && Port[i]->usrnum == usrnum) {
      myp = i;
    }
  }

  MY_PORT->editing = FALSE;
  setmbk(glob->cfg);

  if (save && me.gang && me.rank == 0) {
    Read_Gang(me.gang);
    fsdfxt(0, gang->rank[0], RANK_NAME_SIZE);
    fsdfxt(1, str, 2);
    gang->newbie = atoi(str);

    if (gang->newbie < 1 || gang->newbie >= GANG_RANKS) {
      gang->newbie = 1;
    }

    fsdfxt(2, gang->rank[1], RANK_NAME_SIZE);
    fsdfxt(3, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[1][RENAME_PERM] = TRUE;
    } else {
      gang->perm[1][RENAME_PERM] = FALSE;
    }

    fsdfxt(4, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[1][WITHDRAW_PERM] = TRUE;
    } else {
      gang->perm[1][WITHDRAW_PERM] = FALSE;
    }

    fsdfxt(5, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[1][SET_RANK_PERM] = TRUE;
    } else {
      gang->perm[1][SET_RANK_PERM] = FALSE;
    }

    fsdfxt(6, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[1][INVITE_PERM] = TRUE;
    } else {
      gang->perm[1][INVITE_PERM] = FALSE;
    }

    fsdfxt(7, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[1][KICK_PERM] = TRUE;
    } else {
      gang->perm[1][KICK_PERM] = FALSE;
    }

    fsdfxt(8, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[1][CODE_PERM] = TRUE;
    } else {
      gang->perm[1][CODE_PERM] = FALSE;
    }

    fsdfxt(9, gang->rank[2], RANK_NAME_SIZE);
    fsdfxt(10, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[2][RENAME_PERM] = TRUE;
    } else {
      gang->perm[2][RENAME_PERM] = FALSE;
    }

    fsdfxt(11, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[2][WITHDRAW_PERM] = TRUE;
    } else {
      gang->perm[2][WITHDRAW_PERM] = FALSE;
    }

    fsdfxt(12, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[2][SET_RANK_PERM] = TRUE;
    } else {
      gang->perm[2][SET_RANK_PERM] = FALSE;
    }

    fsdfxt(13, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[2][INVITE_PERM] = TRUE;
    } else {
      gang->perm[2][INVITE_PERM] = FALSE;
    }

    fsdfxt(14, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[2][KICK_PERM] = TRUE;
    } else {
      gang->perm[2][KICK_PERM] = FALSE;
    }

    fsdfxt(15, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[2][CODE_PERM] = TRUE;
    } else {
      gang->perm[2][CODE_PERM] = FALSE;
    }

    fsdfxt(16, gang->rank[3], RANK_NAME_SIZE);
    fsdfxt(17, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[3][RENAME_PERM] = TRUE;
    } else {
      gang->perm[3][RENAME_PERM] = FALSE;
    }

    fsdfxt(18, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[3][WITHDRAW_PERM] = TRUE;
    } else {
      gang->perm[3][WITHDRAW_PERM] = FALSE;
    }

    fsdfxt(19, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[3][SET_RANK_PERM] = TRUE;
    } else {
      gang->perm[3][SET_RANK_PERM] = FALSE;
    }

    fsdfxt(20, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[3][INVITE_PERM] = TRUE;
    } else {
      gang->perm[3][INVITE_PERM] = FALSE;
    }

    fsdfxt(21, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[3][KICK_PERM] = TRUE;
    } else {
      gang->perm[3][KICK_PERM] = FALSE;
    }

    fsdfxt(22, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[3][CODE_PERM] = TRUE;
    } else {
      gang->perm[3][CODE_PERM] = FALSE;
    }

    fsdfxt(23, gang->rank[4], RANK_NAME_SIZE);
    fsdfxt(24, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[4][RENAME_PERM] = TRUE;
    } else {
      gang->perm[4][RENAME_PERM] = FALSE;
    }

    fsdfxt(25, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[4][WITHDRAW_PERM] = TRUE;
    } else {
      gang->perm[4][WITHDRAW_PERM] = FALSE;
    }

    fsdfxt(26, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[4][SET_RANK_PERM] = TRUE;
    } else {
      gang->perm[4][SET_RANK_PERM] = FALSE;
    }

    fsdfxt(27, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[4][INVITE_PERM] = TRUE;
    } else {
      gang->perm[4][INVITE_PERM] = FALSE;
    }

    fsdfxt(28, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[4][KICK_PERM] = TRUE;
    } else {
      gang->perm[4][KICK_PERM] = FALSE;
    }

    fsdfxt(29, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[4][CODE_PERM] = TRUE;
    } else {
      gang->perm[4][CODE_PERM] = FALSE;
    }

    fsdfxt(30, gang->rank[5], RANK_NAME_SIZE);
    fsdfxt(31, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[5][RENAME_PERM] = TRUE;
    } else {
      gang->perm[5][RENAME_PERM] = FALSE;
    }

    fsdfxt(32, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[5][WITHDRAW_PERM] = TRUE;
    } else {
      gang->perm[5][WITHDRAW_PERM] = FALSE;
    }

    fsdfxt(33, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[5][SET_RANK_PERM] = TRUE;
    } else {
      gang->perm[5][SET_RANK_PERM] = FALSE;
    }

    fsdfxt(34, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[5][INVITE_PERM] = TRUE;
    } else {
      gang->perm[5][INVITE_PERM] = FALSE;
    }

    fsdfxt(35, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[5][KICK_PERM] = TRUE;
    } else {
      gang->perm[5][KICK_PERM] = FALSE;
    }

    fsdfxt(36, str, 2);

    if (sameas(str, ANS_YES)) {
      gang->perm[5][CODE_PERM] = TRUE;
    } else {
      gang->perm[5][CODE_PERM] = FALSE;
    }

    Write_Gang(me.gang);
  }

  Redraw();
  prf(NL);
  Gang_Menu();
  prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
  prf("%s%s", MY_PORT->prompt, CLEAR_RIGHT);
  My_Outprf(MY_PORT->usrnum);
  btumil(usrnum, 79 - MY_PORT->prompt_len);
  rstmbk();
}
/*****************************************************************************
    initialize a bogus player/deck for a gang to use
*/
short Init_Gang_Guy(short g) {
  short   i;
  short   index;
  memset((char *) a_human, 0, HUMAN_SIZE);
  sprintf(a_human->alias, "%s%d", A_GANG, g);
  sprintf(a_human->userid, "%s%d", A_GANG, g);

  for (i = 0; i != CODE_SIZE - 1; ++i) {
    a_human->bank.code[i] = RANDOM(26) - 1 + 'A';
    a_human->bank.code[i + 1] = 0;
  }

  for (i = 0; i != CODE_SIZE - 1; ++i) {
    a_human->internet.code[i] = RANDOM(26) - 1 + 'A';
    a_human->internet.code[i + 1] = 0;
  }

  Init_System(a_system, GANGDECK);
  strcpy(a_system->Name, "Gangsters");
  index = Write_Gang_Rec(NO_MATCH);
  return (index);
}
/*****************************************************************************
   writes gang character out to user file
*/
short Write_Gang_Rec(short option) {
  short   n;
  short   found = FALSE;
  short   index = -1;
  FILE    *fp;
  fp = fopen(BRIEF_FILE, READB);

  if (option == NO_MATCH) {
    do {
      n = fread(&glob->brief, BRIEF_SIZE, 1, fp);

      if (n == 1) {
        if (sameas(glob->brief.userid, OPEN_RECORD)) {
          found = TRUE;
        }
      }

      ++index;
    } while (n == 1 && !found);

    a_human->index = index;
  } else {
    found = TRUE;
    a_human->index = gang->ufile_index;
    fseek(fp, (long) gang->ufile_index * BRIEF_SIZE, SEEK_SET);
    fread(&glob->brief, BRIEF_SIZE, 1, fp);
  }

  if (found) {
    if (option == SUICIDE) {
      strcpy(a_human->userid, OPEN_RECORD);
      strcpy(a_human->alias, OPEN_RECORD);
    }

    Set_Brief(a_human, &glob->brief);
    fseek(fp, (long) glob->brief.index * BRIEF_SIZE, SEEK_SET);
    fwrite(&glob->brief, BRIEF_SIZE, 1, fp);
    fclose(fp);
    fp = fopen(USER_FILE, READB);
    fseek(fp, (long) a_human->index * HUMAN_SIZE, SEEK_SET);
    fwrite(a_human, HUMAN_SIZE, 1, fp);
    fclose(fp);
    Write_Deck(a_human->index, a_system);
  } else {
    fclose(fp);
    fp = fopen(USER_FILE, APPENDB);
    fwrite(a_human, HUMAN_SIZE, 1, fp);
    fclose(fp);
    Set_Brief(a_human, &glob->brief);
    fp = fopen(BRIEF_FILE, APPENDB);
    fwrite(&glob->brief, BRIEF_SIZE, 1, fp);
    fclose(fp);
    fp = fopen(DECK_FILE, APPENDB);
    fwrite(a_system, SYSTEM_SIZE, 1, fp);
    fclose(fp);
  }

  return (index);
}
/****************************************************************************
    display the gang bank account ledger
*/
void Gang_Ledger(void) {
  short   i;
  short   net_x, net_y;
  char    ledger_date[DATE_SIZE];
  char    ledger_time[TIME_SIZE];
  char    city_name[CITY_NAME_SIZE];
  FILE    *fp;
  Read_Gang(me.gang);
  fp = fopen(USER_FILE, READB);
  fseek(fp, (long) gang->ufile_index * HUMAN_SIZE, SEEK_SET);
  fread(a_human, HUMAN_SIZE, 1, fp);
  fclose(fp);
  prf("%c%s", CTRL_S, CLEAR_SCREEN);
  prfmsg(MSG205);

  for (i = 0; i != MAX_LEDGER; ++i) {
    if (a_human->bank.ledger[i].action[0]) {
      CONV_DATE(ledger_date, a_human->bank.ledger[i].date);
      CONV_TIME(ledger_time, a_human->bank.ledger[i].time);
      net_x = a_human->bank.ledger[i].net_x;
      net_y = a_human->bank.ledger[i].net_y;
      city_name[0] = 0;

      if (net_x >= 0 && net_y >= 0) {
        strcpy
        (
          city_name,
          world->city[net_x][net_y].name
        );
      }

      sprintf
      (
        str,
        getmsg(MSG206),
        ledger_date,
        ledger_time,
        a_human->bank.ledger[i].amount,
        a_human->bank.ledger[i].balance,
        city_name,
        a_human->bank.ledger[i].alias,
        a_human->bank.ledger[i].action
      );
      prf(str);
    }
  }

  MY_PORT->prompt_y = ASYNCH_Y - 1;
  Prompt(getmsg(MSG207), GANG_LEDGER_PROMPT);
}
/***************************************************************************
    list current gangs
*/
short EXPORT List_Gangs(void) {
  short   i;
  short   l = 0;
  short   n;
  FILE    *fp;

  if (STATE == GANG_PROMPT1 || STATE == GANG_PROMPT2) {
    MY_PORT->misc = 1;
  } else if (margc && sameas(margv[0], ANS_NO)) {
    Clear_Display_Window();
    Gang_Menu();
    EXIT0;
  }

  fp = fopen(GANG_FILE, READB);

  if (fp == NULL) {
    STATE = MAIN_PROMPT;
    NOTIFY("LoC: Gang File Failure!");
    EXIT0;
  }

  fseek(fp, (long) MY_PORT->misc * GANG_SIZE, SEEK_SET);
  Clear_Display_Window();

  do {
    n = fread(gang, GANG_SIZE, 1, fp);

    if (n == 1 && !sameas(gang->name, OPEN_RECORD)) {
      for (i = 0; i != max_guys; ++i) {
        if (Port[i]->active && Port[i]->guy.gang == gang->index) {
          gang->pts += Port[i]->gang_pts;
          gang->pts_today += Port[i]->gang_pts;
        }
      }

      sprintf
      (
        str,
        getmsg(M735),
        MY_PORT->misc,
        gang->name,
        gang->members,
        gang->pts,
        gang->pts_today
      );
      prf(str);
      ++l;
    }

    ++MY_PORT->misc;
  } while (n == 1 && l < 9);

  fclose(fp);

  if (n != 1) {
    Prompt(getmsg(MSG207), NOMORE_GANG_PROMPT);
  } else {
    Prompt(getmsg(MSG243), MORE_GANG_PROMPT);
  }

  EXIT0;
}

