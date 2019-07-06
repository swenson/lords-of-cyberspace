#include "header.h"
#include <globals.h>

/****************************************************************************
    e-mail and the freenet forum
*/
short EXPORT Mail(void) {
  if (STATE == MAIL_PROMPT) {
    Clear_Display_Window();

    if (sameas(margv[0], "1")) {
      Check_Mail();
      EXIT0;
    } else if (sameas(margv[0], "2")) {
      Write_Email();
      EXIT0;
    } else if (sameas(margv[0], "3")) {
      Read_FreeNet();
    } else if (sameas(margv[0], "4")) {
      Post_FreeNet();
    } else if (sameas(margv[0], CANCEL)) {
      STATE = MAIN_PROMPT;
      EXIT0;
    } else {
      STATE = MAIN_PROMPT;
    }
  }

  if (STATE == MAIN_PROMPT) {
    prfmsg(MSG690);
    Prompt(getmsg(MSG691), MAIL_PROMPT);
  }

  EXIT0;
}
/****************************************************************************
    write private e-mail to another player
*/
short Write_Email(void) {
  if (STATE == MAIL_PROMPT) {
    Prompt(getmsg(MSG692), WRITE_MAIL_PROMPT);
    EXIT0;
  }

  if (STATE == WRITE_MAIL_PROMPT) {
    Fix_Alias(MY_PORT->tstring);
    MY_PORT->post_index = Find_Alias(a_human, MY_PORT->tstring);

    if (MY_PORT->post_index < 0 || sameto(A_GANG, a_human->userid)) {
      prfmsg(MSG176, MY_PORT->tstring);
      STATE = MAIN_PROMPT;
      Mail();
      EXIT0;
    }

    Post_Private();
  }

  EXIT0;
}
/*****************************************************************************
    post a private e-mail message to someone's mailbox
*/
short Post_Private(void) {
  FILE    *fp;
  short   i;
  char    today_date[DATE_SIZE];
  char    now_time[TIME_SIZE];
  struct  post_struct *msg;
  msg = (struct post_struct *) vdaptr;

  if (STATE != GET_MSG_BODY_PROMPT) {
    setmem(msg, POST_SIZE, 0);
    TODAY(today_date);
    NOW(now_time);
    sprintf
    (
      msg->from,
      getmsg(MSSG693),
      me.alias,
      today_date,
      now_time
    );
    MY_PORT->lines = 0;
    prf("\x1B[%d;1H", DISP_Y);
    prfmsg(MSG694);
    MY_PORT->prompt_y = DISP_Y + 1;
    sprintf(str, getmsg(MSG695), MY_PORT->lines + 1, MESSAGE_LEN);
    Prompt(str, GET_MSG_BODY_PROMPT);
    EXIT0;
  }

  if (sameas(margv[0], "/c") || sameas(margv[0], "x")) {
    Clear_Display_Window();
    prfmsg(MSG696);
    STATE = MAIN_PROMPT;
    MY_PORT->prompt_y = PROMPT_Y;
    Mail();
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
      Prompt(str, GET_MSG_BODY_PROMPT);
      EXIT0;
    }
  }

  MY_PORT->prompt_y = PROMPT_Y;
  STATE = MAIN_PROMPT;
  Clear_Display_Window();

  if (!MY_PORT->lines) {
    prfmsg(MSG697);
    Mail();
    EXIT0;
  }

  sprintf(str, "%sM%d.DAT", MAIL_PATH, MY_PORT->post_index);
  Create_New(str);
  fp = fopen(str, APPENDB);

  if (fp == NULL) {
    prf("Mailbox failure!\r");
    prf("%s\r", str);
    NOTIFY("LoC: MAIL OPEN FAILED!");
    Mail();
    EXIT0;
  }

  fwrite(msg, POST_SIZE, 1, fp);
  fclose(fp);
  prfmsg(MSG698);

  if (me.index != MY_PORT->post_index) {
    for (i = 0; i != max_guys; ++i) {
      if (Port[i]->active && Port[i]->guy.index == MY_PORT->post_index) {
        sprintf(str, getmsg(MSG699), me.alias);
        Send(str, i, TRUE);
      }
    }
  }

  Mail();
  EXIT0;
}
/*****************************************************************************
    check user's mail
*/
short Check_Mail(void) {
  short   i;
  short   n;
  struct  post_struct msg;

  if (STATE == MAIL_PROMPT) {
    sprintf(str, "%sM%d.DAT", MAIL_PATH, me.index);
    MY_PORT->fp = fopen(str, READB);

    if (MY_PORT->fp == NULL) {
      Clear_Display_Window();
      prfmsg(MSG701);
      STATE = MAIN_PROMPT;
      Mail();
      EXIT0;
    }
  } else if (STATE == ERASE_MSG_PROMPT) {
    Clear_Display_Window();

    if (sameas(margv[0], ANS_YES)) {
      Erase_Mail(me.index);
      prfmsg(MSG702);
    }

    STATE = MAIN_PROMPT;
    Mail();
    EXIT0;
  } else if (STATE == NEXT_MSG_PROMPT) {
    if (sameas(margv[0], ANS_NO)) {
      Clear_Display_Window();
      fclose(MY_PORT->fp);
      STATE = MAIN_PROMPT;
      Mail();
      EXIT0;
    }
  }

  n = fread(&msg, POST_SIZE, 1, MY_PORT->fp);

  if (n == 0) {
    fclose(MY_PORT->fp);
    Prompt(getmsg(MSG703), ERASE_MSG_PROMPT);
    EXIT0;
  }

  Clear_Display_Window();
  prf("%s\r", msg.from);

  for (i = 0; i != MESSAGE_LEN; ++i) {
    if (msg.body[i][0]) {
      prfmsg(MSG704, msg.body[i]);
    }
  }

  n = fread(&msg, POST_SIZE, 1, MY_PORT->fp);

  if (n == 0) {
    fclose(MY_PORT->fp);
    Prompt(getmsg(MSG703), ERASE_MSG_PROMPT);
    EXIT0;
  }

  fseek(MY_PORT->fp, (long) POST_SIZE * (-1L), SEEK_CUR);
  Prompt(getmsg(MSG705), NEXT_MSG_PROMPT);
  EXIT0;
}
/****************************************************************************
    see if I have any personal mail
*/
void New_Mail_Check(void) {
  FILE    *fp;
  sprintf(str, "%sM%d.DAT", MAIL_PATH, me.index);
  fp = fopen(str, READB);

  if (fp != NULL) {
    prfmsg(MSG700);
    fclose(fp);
  } else {
    prf("\r");
  }
}
/****************************************************************************
    remove my mail file
*/
void Erase_Mail(short index) {
  sprintf(str, "%sM%d.DAT", MAIL_PATH, index);
  unlink(str);
}
/*****************************************************************************
    post a message to the FreeNet forum
*/
short Post_FreeNet(void) {
  char    today_date[DATE_SIZE];
  char    now_time[TIME_SIZE];
  struct  freenet_struct *msg;
  msg = (struct freenet_struct *) vdaptr;

  if (STATE == MAIL_PROMPT) {
    setmem(msg, FREENET_SIZE, 0);
    TODAY(today_date);
    NOW(now_time);
    sprintf
    (
      msg->from,
      getmsg(MSG706),
      me.alias,
      today_date,
      now_time
    );
    MY_PORT->lines = 0;
    prf("\x1B[%d;1H", DISP_Y);
    prfmsg(MSG694);
    MY_PORT->prompt_y = DISP_Y + 1;
    sprintf(str, getmsg(MSG695), MY_PORT->lines + 1, MESSAGE_LEN);
    Prompt(str, FREE_BODY_PROMPT);
    EXIT0;
  }

  if (sameas(margv[0], "/c") || sameas(margv[0], "x")) {
    Clear_Display_Window();
    prfmsg(MSG696);
    STATE = MAIN_PROMPT;
    MY_PORT->prompt_y = PROMPT_Y;
    Mail();
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
      Prompt(str, FREE_BODY_PROMPT);
      EXIT0;
    }
  }

  if (!MY_PORT->lines) {
    prfmsg(MSG697);
    Mail();
    EXIT0;
  }

  Put_FreeNet_Message(msg);
  Clear_Display_Window();
  STATE = MAIN_PROMPT;
  MY_PORT->prompt_y = PROMPT_Y;
  prfmsg(MSG698);
  Mail();
  EXIT0;
}
/****************************************************************************
    put a message in the freenet forum
*/
short Put_FreeNet_Message(struct freenet_struct *msg) {
  short   n;
  long    msg_num;
  FILE    *fp;
  fp = fopen(FREENET_FILE, READB);

  if (fp == NULL) {
    NOTIFY("LoC: FREENET OPEN FAILED!");
    EXIT0;
  }

  n = fread(&glob->header, FREENET_SIZE, 1, fp);

  if (n != 1) {
    fclose(fp);
    fp = fopen(FREENET_FILE, APPENDB);
    setmem(&glob->header, FREENET_SIZE, 0);
    fwrite(&glob->header, FREENET_SIZE, 1, fp);
    fclose(fp);
    fp = fopen(FREENET_FILE, READB);
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
    fp = fopen(FREENET_FILE, APPENDB);
    ++glob->header.total;
    fwrite(msg, FREENET_SIZE, 1, fp);
    fclose(fp);
    fp = fopen(FREENET_FILE, READB);
  }

  glob->header.msg_num = msg_num;
  fseek(fp, 0L, SEEK_SET);
  fwrite(&glob->header, FREENET_SIZE, 1, fp);
  fclose(fp);
  EXIT0;
}
/*****************************************************************************
    check the freenet forum
*/
short Read_FreeNet(void) {
  short   i;
  short   n;
  short   done = FALSE;
  FILE    *fp;

  if (STATE == MAIL_PROMPT) {
    fp = fopen(FREENET_FILE, READB);

    if (fp == NULL) {
      prfmsg(MSG709);
      STATE = MAIN_PROMPT;
      Mail();
      EXIT0;
    }

    n = fread(&glob->header, FREENET_SIZE, 1, fp);
    fclose(fp);

    if (n == 0) {
      prfmsg(MSG709);
      STATE = MAIN_PROMPT;
      Mail();
      EXIT0;
    }

    sprintf
    (
      str,
      getmsg(MSG708),
      glob->header.msg_num - glob->header.total + 1,
      glob->header.msg_num
    );
    Prompt(str, FREENET_PROMPT1);
    EXIT0;
  }

  if (STATE == FREENET_PROMPT1) {
    if (margc == 0) {
      MY_PORT->forum1 = me.freenet + 1;
    } else {
      MY_PORT->forum1 = LNUM(margv[0]);
    }

    fp = fopen(FREENET_FILE, READB);
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
    me.freenet = MY_PORT->forum1 - 1;
    Mail();
    EXIT0;
  }

  fp = fopen(FREENET_FILE, READB);

  do {
    fseek(fp, MY_PORT->forum2 * (long) FREENET_SIZE, SEEK_SET);
    n = fread(&glob->dummy, FREENET_SIZE, 1, fp);

    if (n == 0) {
      Prompt(getmsg(MSG710), FREENET_PROMPT3);
      me.freenet = MY_PORT->forum1;
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
        Prompt(getmsg(MSG710), FREENET_PROMPT3);
        me.freenet = MY_PORT->forum1;
        fclose(fp);
        EXIT0;
      }

      ++MY_PORT->forum2;
      fclose(fp);
      Prompt(getmsg(MSG705), FREENET_PROMPT2);
      EXIT0;
    }
  } while (!done);

  Prompt(getmsg(MSG710), FREENET_PROMPT3);
  me.freenet = MY_PORT->forum1;
  fclose(fp);
  EXIT0;
}
/*****************************************************************************
    inform user of unread freenet messages
*/
short EXPORT New_FreeNet(void) {
  short   n;
  FILE    *fp;
  fp = fopen(FREENET_FILE, READB);

  if (fp == NULL) {
    me.freenet = 0;
    EXIT0;
  }

  n = fread(&glob->header, FREENET_SIZE, 1, fp);
  fclose(fp);

  if (n == 0) {
    me.freenet = 0;
    EXIT0;
  }

  if (glob->header.msg_num < me.freenet) {
    me.freenet = glob->header.msg_num;
    EXIT0;
  }

  if (glob->header.msg_num > me.freenet) {
    prfmsg(MSG707);
  }

  EXIT0;
}
