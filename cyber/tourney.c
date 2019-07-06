#include <header.h>
#include <globals.h>

/****************************************************************************
    edit tournament setup
*/
short EXPORT Tourney_Edit(void) {
  short   i;
  long    score;
  FILE    *fp;
  Clear_Display_Window();

  if (STATE == TOURNEY_PROMPT) {
    prfmsg(MSG879);
    Prompt(getmsg(MSG880), TOURNEY_EDIT_PROMPT1);
    EXIT0;
  }

  if (STATE == TOURNEY_EDIT_PROMPT1) {
    if (margc && sameas(margv[0], CANCEL)) {
      STATE = MAIN_PROMPT;
      Tourney_Services();
      EXIT0;
    }

    if (margc) {
      i = INUM(margv[0]);

      if (i >= 0 && i <= 3) {
        tourney->type = i;
        fp = fopen(TOURNEY_FILE, READB);
        fwrite(tourney, TOURNEY_SIZE, 1, fp);
        fclose(fp);
      }
    }

    if (tourney->type == ENDGAME_TOURNEY) {
      Prompt(getmsg(MSG881), TOURNEY_EDIT_PROMPT2);
      EXIT0;
    }

    if (tourney->type == TIMED_TOURNEY) {
      Prompt(getmsg(MSG884), TOURNEY_EDIT_PROMPT3);
      EXIT0;
    }

    if (tourney->type == SCORE_TOURNEY) {
      Prompt(getmsg(MSG882), TOURNEY_EDIT_PROMPT4);
      EXIT0;
    }
  }

  if (STATE == TOURNEY_EDIT_PROMPT2) {
    if (margc && sameas(margv[0], CANCEL)) {
      STATE = MAIN_PROMPT;
      Tourney_Services();
      EXIT0;
    }

    if (margc) {
      i = INUM(margv[0]);

      if (i > 0) {
        tourney->winners = i;
        fp = fopen(TOURNEY_FILE, READB);
        fwrite(tourney, TOURNEY_SIZE, 1, fp);
        fclose(fp);
      }
    }
  }

  if (STATE == TOURNEY_EDIT_PROMPT3) {
    if (margc && sameas(margv[0], CANCEL)) {
      STATE = MAIN_PROMPT;
      Tourney_Services();
      EXIT0;
    }

    if (margc) {
      i = INUM(margv[0]);

      if (i > 0) {
        tourney->days = i;

        if (tourney->phase == GAME) {
          tourney->phase_end = time(0);
          tourney->phase_end += tourney->days * 3600L * 24L;
        }

        fp = fopen(TOURNEY_FILE, READB);
        fwrite(tourney, TOURNEY_SIZE, 1, fp);
        fclose(fp);
      }
    }
  }

  if (STATE == TOURNEY_EDIT_PROMPT4) {
    if (margc && sameas(margv[0], CANCEL)) {
      STATE = MAIN_PROMPT;
      Tourney_Services();
      EXIT0;
    }

    if (margc) {
      score = LNUM(margv[0]);

      if (score > 0) {
        tourney->score = score;
        fp = fopen(TOURNEY_FILE, READB);
        fwrite(tourney, TOURNEY_SIZE, 1, fp);
        fclose(fp);
      }
    }

    sprintf(str, getmsg(MSG883), tourney->score);
    Prompt(str, TOURNEY_EDIT_PROMPT2);
    EXIT0;
  }

  sprintf
  (
    vdatmp,
    tourney_edit_format,
    tourney->postgame_hours, '\0',
    tourney->credits[0], '\0',
    tourney->prizes[0], '\0',
    tourney->credits[1], '\0',
    tourney->prizes[1], '\0',
    tourney->credits[2], '\0',
    tourney->prizes[2], '\0',
    tourney->credits[3], '\0',
    tourney->prizes[3], '\0',
    tourney->credits[4], '\0',
    tourney->prizes[4], '\0',
    tourney->credits[5], '\0',
    tourney->prizes[5], '\0',
    tourney->credits[6], '\0',
    tourney->prizes[6], '\0',
    tourney->credits[7], '\0',
    tourney->prizes[7], '\0',
    tourney->credits[8], '\0',
    tourney->prizes[8], '\0'
  );
  clrprf();
  MY_PORT->editing = TRUE;
  STATE = MSG878;
  fsdroom(MSG878, tourney_screen_spec, 1);
  fsdapr(vdaptr, vdasiz, vdatmp);
  fsdbkg(fsdrft());
  fsdego(vfyadn, Tourney_Edit_Done);
  My_Outprf(MY_PORT->usrnum);
  EXIT0;
}
/****************************************************************************
    display the results of previous tournaments
*/
short Previous_Winners(void) {
  short   i;
  short   n;
  char    userid[UIDSIZ + 2];

  if (STATE == TOURNEY_PROMPT) {
    MY_PORT->misc = 0;
    MY_PORT->fp = fopen(WINNER_FILE, READB);
    n = fseek(MY_PORT->fp, 0L, SEEK_END);
  } else {
    if (margc && sameas(margv[0], ANS_NO)) {
      fclose(MY_PORT->fp);
      MY_PORT->prompt_y = PROMPT_Y;
      Redraw();
      prf("\x1B[%d;1H", DISP_Y);
      STATE = MAIN_PROMPT;
      Tourney_Services();
      EXIT0;
    }

    n = fseek(MY_PORT->fp, WINNERS_SIZE * (-1L), SEEK_CUR);
  }

  if (n == 0) {
    fseek(MY_PORT->fp, WINNERS_SIZE * (-1L), SEEK_CUR);
    n = fread(&glob->fame, WINNERS_SIZE, 1, MY_PORT->fp);
  }

  if (n == 0) {
    fclose(MY_PORT->fp);

    if (MY_PORT->misc == 0) {
      Clear_Display_Window();
      prfmsg(MSG888);
    } else {
      MY_PORT->prompt_y = PROMPT_Y;
      Redraw();
      prf("\x1B[%d;1H", DISP_Y);
      prfmsg(MSG889);
    }

    STATE = MAIN_PROMPT;
    Tourney_Services();
    EXIT0;
  }

  prf("%c%s", CTRL_S, CLEAR_SCREEN);
  prfmsg(MSG890, glob->fame.date);

  for (i = 0; i != 9; ++i) {
    if (glob->fame.alias[i][0]) {
      sprintf(userid, "(%s)", Brief_Userid(glob->fame.userid[i]));
      sprintf
      (
        str,
        getmsg(MSG891),
        i + 1,
        glob->fame.alias[i],
        userid,
        rank[glob->fame.rank[i]].title,
        glob->fame.score[i]
      );
      prf(str);
      prfmsg(MSG892, glob->fame.prize[i]);
    }
  }

  ++MY_PORT->misc;
  MY_PORT->prompt_y = ASYNCH_Y + 2;
  Prompt(getmsg(MSG243), WINNERS_PROMPT);
  EXIT0;
}
/****************************************************************************
    during cleanup, see if the tournament is over
*/
short Tourney_Check(void) {
  short   i;
  long    ltemp;
  short   temp;
  long    lowest;
  short   low_index;
  short   n;
  short   done;
  short   index = -1;
  short   indexes[9];
  char    place[4];
  long    score[9];
  char    *cp;
  FILE    *fp;
  Phase_Check(0);

  if (tourney->phase != POSTGAME || tourney->type == 0) {
    EXIT0;
  }

  fp = fopen(BRIEF_FILE, READB);

  for (n = 0; n != 9; ++n) {
    indexes[n] = -1;
    score[n] = -1;
  }

  do {
    n = fread(&glob->brief, BRIEF_SIZE, 1, fp);
    ++index;

    if
    (
      n == 1 &&
      !sameas(glob->brief.userid, OPEN_RECORD) &&
      !sameto(A_GANG, glob->brief.userid) &&
      !glob->brief.perm[GAMEOP_PERM]
    ) {
      for (i = done = 0; i != 9 && !done; ++i) {
        if (indexes[i] == -1) {
          indexes[i] = index;
          score[i] = glob->brief.score;
          done = TRUE;
        }
      }

      if (!done) {
        lowest = -1;

        for (i = 0; i != 9; ++i) {
          if (score[i] < lowest || lowest == -1) {
            lowest = score[i];
            low_index = i;
          }
        }

        if (glob->brief.score > lowest) {
          score[low_index] = glob->brief.score;
          indexes[low_index] = glob->brief.index;
        }
      }
    }
  } while (n == 1);

  for (i = 0; i != 9; ++i) {
    for (n = 0; n != 9; ++n) {
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

  strcpy(glob->fame.date, ncdate(today()));

  for (i = 0; i != 9; ++i) {
    if (score[i] == -1) {
      glob->fame.alias[i][0] = 0;
    } else {
      fseek(fp, (long) indexes[i] * BRIEF_SIZE, SEEK_SET);
      fread(&glob->brief, BRIEF_SIZE, 1, fp);
      strcpy(glob->fame.alias[i], glob->brief.alias);
      strcpy(glob->fame.userid[i], glob->brief.userid);
      glob->fame.score[i] = glob->brief.score;
      glob->fame.rank[i] = His_Rank(glob->brief.score, glob->brief.won);

      if (!tourney->results_tallied && tourney->credits[i]) {
        addcrd(glob->brief.userid, l2as(tourney->credits[i]), 0);
      }

      if (tourney->credits[i] && tourney->prizes[i][0]) {
        sprintf
        (
          str,
          "%ld system credits and %s",
          tourney->credits[i],
          tourney->prizes[i]
        );
      } else if (tourney->credits[i]) {
        sprintf
        (
          str,
          "%ld system credits",
          tourney->credits[i]
        );
      } else if (tourney->prizes[i][0]) {
        strcpy(str, tourney->prizes[i]);
      } else {
        strcpy(str, "Nothing");
      }

      strcpy(glob->fame.prize[i], str);

      if (!tourney->results_tallied && !sameas(str, "Nothing")) {
        if (i == 0) {
          strcpy(place, "1st");
        } else if (i == 1) {
          strcpy(place, "2nd");
        } else if (i == 2) {
          strcpy(place, "3rd");
        } else {
          sprintf(place, "%dth", i + 1);
        }

        setmem(mp, sizeof(struct message), 0);
        stzcpy(mp->to, glob->brief.userid, UIDSIZ);
        stzcpy(mp->from, "Sysop", UIDSIZ);
        strcpy(mp->topic, "LoC Prize Notification!");
#ifdef MBBS
        mp->flags = 0;
        mp->crdate = today();
        mp->crtime = now();
        mp->msgno = ++sv.msgtot;
        stzcpy(mp->userto, glob->brief.userid, UIDSIZ);
#endif
        prfmsg(MSG893, place, str);
        stpans(prfbuf);

        for (cp = prfbuf; *cp != '\0'; cp++) {
          if (*cp == '\n') {
            *cp = '\r';
          } else if (*cp == '\a') {
            *cp = ' ';
          }
        }

#ifdef MBBS
        strcpy(mp->text, prfbuf);
        clrprf();
        bbs_sendmsg(mp, glob->brief.userid);
#else
        simpsnd(mp, prfbuf, "");
        clrprf();
#endif
      }
    }
  }

  fclose(fp);

  if (!tourney->results_tallied) {
    fp = fopen(WINNER_FILE, APPENDB);
    fwrite(&glob->fame, WINNERS_SIZE, 1, fp);
    fclose(fp);
    tourney->results_tallied = TRUE;
    fp = fopen(TOURNEY_FILE, READB);
    fwrite(tourney, TOURNEY_SIZE, 1, fp);
    fclose(fp);
    NOTIFY("LoC: Winners Posted!");
  }

  if (time(0) > tourney->phase_end) {
    Reset_Game();
  }

  EXIT0;
}
/****************************************************************************
    finished entering tourney data
*/
void Tourney_Edit_Done(MSHORT save) {
  short   i;
  short   n;
  FILE    *fp;
  usrptr->state = glob->module_id;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active && Port[i]->usrnum == usrnum) {
      myp = i;
    }
  }

  MY_PORT->editing = FALSE;
  setmbk(glob->cfg);

  if (save) {
    fsdfxt(0, str, 4);
    tourney->postgame_hours = atoi(str);

    for (i = 0, n = 1; i != 9; ++i) {
      fsdfxt(n, str, 7);
      tourney->credits[i] = atol(str);
      ++n;
      fsdfxt(n, tourney->prizes[i], 37);
      ++n;
    }

    fp = fopen(TOURNEY_FILE, READB);
    fwrite(tourney, TOURNEY_SIZE, 1, fp);
    fclose(fp);
  }

  Redraw();
  prf(NL);
  STATE = MAIN_PROMPT;
  Tourney_Services();
  prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
  prf("%s%s", MY_PORT->prompt, CLEAR_RIGHT);
  My_Outprf(MY_PORT->usrnum);
  btumil(usrnum, 79 - MY_PORT->prompt_len);
  rstmbk();
}
/****************************************************************************
    manually change tourney phase
*/
short Phase_Change(void) {
  long    dur;
  FILE    *fp;
  Clear_Display_Window();

  if (margc == 0 || sameas(margv[0], CANCEL)) {
    STATE = MAIN_PROMPT;
    Tourney_Services();
    EXIT0;
  }

  if (STATE == PHASE_PROMPT1) {
    if (sameas(margv[0], "A")) {
      MY_PORT->misc = 'A';
      Prompt(getmsg(MSG877), PHASE_PROMPT2);
      EXIT0;
    }

    if (sameas(margv[0], "B")) {
      tourney->phase = GAME;
      tourney->phase_start = time(0);

      if (tourney->type == TIMED_TOURNEY) {
        tourney->phase_end = time(0) + (tourney->days * 3600L * 24L);
      } else {
        tourney->phase_end = 0;
      }

      fp = fopen(TOURNEY_FILE, READB);
      fwrite(tourney, TOURNEY_SIZE, 1, fp);
      fclose(fp);
      STATE = MAIN_PROMPT;
      Tourney_Services();
      EXIT0;
    }

    if (sameas(margv[0], "C")) {
      MY_PORT->misc = 'C';
      Prompt(getmsg(MSG877), PHASE_PROMPT2);
      EXIT0;
    }

    STATE = MAIN_PROMPT;
    Tourney_Services();
    EXIT0;
  }

  dur = LNUM(margv[0]);
  tourney->phase_start = time(0);
  tourney->phase_end = time(0) + (dur * 60);

  if (MY_PORT->misc == 'A') {
    tourney->phase = PREGAME;
  } else {
    tourney->phase = POSTGAME;
  }

  fp = fopen(TOURNEY_FILE, READB);
  fwrite(tourney, TOURNEY_SIZE, 1, fp);
  fclose(fp);
  STATE = MAIN_PROMPT;
  Tourney_Services();
  EXIT0;
}
/****************************************************************************
    see if the tournament phase needs to be changed
*/
short Phase_Check(short option) {
  FILE    *fp;

  if (tourney->type == 0) {
    return (TRUE);
  }

  if (tourney->phase == PREGAME) {
    if (time(0) > tourney->phase_end) {
      tourney->phase = GAME;
      tourney->phase_start = time(0);

      if (tourney->type == TIMED_TOURNEY) {
        tourney->phase_end = time(0) + (tourney->days * 3600L * 24L);
      } else {
        tourney->phase_end = 0;
      }

      fp = fopen(TOURNEY_FILE, READB);
      fwrite(tourney, TOURNEY_SIZE, 1, fp);
      fclose(fp);
      return (TRUE);
    }

    if (option) {
      if (me.perm[GAMEOP_PERM]) {
        return (TRUE);
      }

      Send(getmsg(MSG870A), myp, TRUE);
      Send(getmsg(MSG870B), myp, FALSE);
    }

    return (FALSE);
  }

  if (tourney->phase == GAME) {
    if (tourney->type == TIMED_TOURNEY) {
      if (time(0) < tourney->phase_end) {
        return (TRUE);
      }
    } else if (glob->winners < tourney->winners) {
      return (TRUE);
    }

    tourney->phase = POSTGAME;
    tourney->phase_start = time(0);
    tourney->phase_end = time(0) + (tourney->postgame_hours * 3600L);
    fp = fopen(TOURNEY_FILE, READB);
    fwrite(tourney, TOURNEY_SIZE, 1, fp);
    fclose(fp);

    if (option) {
      if (me.perm[GAMEOP_PERM]) {
        return (TRUE);
      }

      Send(getmsg(MSG872A), myp, TRUE);
      Send(getmsg(MSG872B), myp, FALSE);
    }

    return (FALSE);
  }

  if (time(0) > tourney->phase_end && tourney->results_tallied) {
    glob->reset = TRUE;
    return (FALSE);
  }

  if (option) {
    if (me.perm[GAMEOP_PERM]) {
      return (TRUE);
    }

    Send(getmsg(MSG871A), myp, TRUE);
    Send(getmsg(MSG871B), myp, FALSE);
  }

  return (FALSE);
}
/****************************************************************************
    tournament services
*/
short Tourney_Services(void) {
  short   i;
  long    dur;
  char    place[4];
  FILE    *fp;

  if (STATE == MAIN_PROMPT) {
    if (!me.perm[GAMEOP_PERM]) {
      prfmsg(MSG857);
      Prompt(getmsg(MSG858), TOURNEY_PROMPT);
    } else {
      prfmsg(MSG859);
      Prompt(getmsg(MSG860), TOURNEY_PROMPT);
    }

    EXIT0;
  }

  if (STATE == TOURNEY_PROMPT) {
    if (margc == 0 || sameas(margv[0], CANCEL)) {
      STATE = MAIN_PROMPT;
      EXIT0;
    }

    if (sameas(margv[0], "A")) {
      fp = fopen(TOURNEY_FILE, READB);
      fread(tourney, TOURNEY_SIZE, 1, fp);
      fclose(fp);
      Phase_Check(0);
      prf("%c%s", CTRL_S, CLEAR_SCREEN);
      dur = (tourney->phase_end - time(0)) / 3600L;

      if (dur > 1) {
        sprintf(str, "%ld hours", dur);
      } else if (dur == 1) {
        sprintf(str, "%ld hour", dur);
      } else {
        dur = (tourney->phase_end - time(0)) / 60L;

        if (dur > 1) {
          sprintf(str, "%ld minutes", dur);
        } else if (dur == 1) {
          sprintf(str, "%ld minute", dur);
        } else {
          dur = tourney->phase_end - time(0);

          if (dur < 0) {
            strcpy(str, "After System Clean-up");
          } else {
            sprintf(str, "%ld seconds", dur);
          }
        }
      }

      if (tourney->phase == PREGAME) {
        prfmsg(MSG861, str);
      } else if (tourney->phase == GAME) {
        dur = (time(0) - tourney->phase_start) / (3600L * 24L);

        if (dur > 1) {
          sprintf(str, "%ld days", dur);
        } else if (dur == 1) {
          sprintf(str, "%ld day", dur);
        } else {
          dur = (time(0) - tourney->phase_start) / 3600L;

          if (dur > 1) {
            sprintf(str, "%ld hours", dur);
          } else if (dur == 1) {
            sprintf(str, "%ld hour", dur);
          } else {
            dur = (time(0) - tourney->phase_start) / 60L;

            if (dur > 1) {
              sprintf(str, "%ld minutes", dur);
            } else if (dur == 1) {
              sprintf(str, "%ld minute", dur);
            } else {
              dur = time(0) - tourney->phase_start;
              sprintf(str, "%ld seconds", dur);
            }
          }
        }

        prfmsg(MSG862, str);
      } else if (tourney->phase == POSTGAME) {
        if (tourney->results_tallied) {
          prfmsg(MSG863A, str);
        } else {
          prfmsg(MSG863B, str);
        }
      }

      prf(NL);

      if (tourney->type == ENDGAME_TOURNEY) {
        prfmsg(MSG864, tourney->winners);
      } else if (tourney->type == SCORE_TOURNEY) {
        prfmsg(MSG865, tourney->winners, l2as(tourney->score));
      } else if (tourney->type == TIMED_TOURNEY) {
        if (tourney->phase == PREGAME) {
          sprintf(str, "%d days", tourney->days);
        } else if (tourney->phase == GAME) {
          dur = (tourney->phase_end - time(0)) / (3600L * 24L);

          if (dur > 1) {
            sprintf(str, "%ld days", dur);
          } else if (dur == 1) {
            sprintf(str, "%ld day", dur);
          } else {
            dur = (tourney->phase_end - time(0)) / 3600L;

            if (dur > 1) {
              sprintf(str, "%ld hours", dur);
            } else if (dur == 1) {
              sprintf(str, "%ld hour", dur);
            } else {
              dur = (tourney->phase_end - time(0)) / 60L;

              if (dur > 1) {
                sprintf(str, "%ld minutes", dur);
              } else if (dur == 1) {
                sprintf(str, "%ld minute", dur);
              } else {
                dur = tourney->phase_end - time(0);

                if (dur < 0) {
                  strcpy(str, "<Game Over>");
                } else {
                  sprintf(str, "%ld seconds", dur);
                }
              }
            }
          }
        } else {
          strcpy(str, "<Game Over>");
        }

        prfmsg(M866, str);
      } else {
        prfmsg(MSG867);
      }

      prfmsg(MSG924, l2as(glob->egamscor));
      prf(NL);

      for (i = 0; i != MAX_WINNERS; ++i) {
        if (i == 0) {
          strcpy(place, "1st");
        } else if (i == 1) {
          strcpy(place, "2nd");
        } else if (i == 2) {
          strcpy(place, "3rd");
        } else {
          sprintf(place, "%dth", i + 1);
        }

        if (tourney->type) {
          if (tourney->credits[i] && tourney->prizes[i][0]) {
            sprintf
            (
              str,
              "%ld system credits and %s",
              tourney->credits[i],
              tourney->prizes[i]
            );
          } else if (tourney->credits[i]) {
            sprintf
            (
              str,
              "%ld system credits",
              tourney->credits[i]
            );
          } else if (tourney->prizes[i][0]) {
            strcpy(str, tourney->prizes[i]);
          } else {
            strcpy(str, "Nothing");
          }
        } else {
          strcpy(str, "Nothing");
        }

        prfmsg(MSG868, place, str);
      }

      MY_PORT->prompt_y = ASYNCH_Y - 1;
      Prompt(getmsg(MSG207), TSTATUS_PROMPT);
      EXIT0;
    } else if (sameas(margv[0], "B")) {
      Previous_Winners();
      EXIT0;
    } else if (sameas(margv[0], "C") && me.perm[GAMEOP_PERM]) {
      Clear_Display_Window();
      prfmsg(MSG875);
      Prompt(getmsg(MSG876), PHASE_PROMPT1);
      EXIT0;
    } else if (sameas(margv[0], "D") && me.perm[GAMEOP_PERM]) {
      Tourney_Edit();
      EXIT0;
    } else if (sameas(margv[0], "E") && me.perm[GAMEOP_PERM]) {
      Reset_Tournament();
      Clear_Display_Window();
      prfmsg(MSG869);
      STATE = MAIN_PROMPT;
      Tourney_Services();
      EXIT0;
    } else if (sameas(margv[0], "F") && me.perm[GAMEOP_PERM]) {
      Clear_Display_Window();
      prfmsg(MSG885);
      Prompt(getmsg(MSG886), RESET_PROMPT);
      EXIT0;
    }
  }

  EXIT0;
}
/****************************************************************************
    restart all of the data files
*/
short Reset_Game(void) {
  short   i;
  short   active = FALSE;

  for (i = 0; i != max_guys; ++i) {
    if (Port[i]->active) {
      active = TRUE;

      if (Port[i]->access_level != IDLE) {
        myp = i;
        Jack_Out();
      }
    }
  }

  if (active) {
    EXIT0;
  }

  for (i = 0; i != max_systems; ++i) {
    if
    (
      Sys[i]->in_use &&
      Sys[i]->type != CON_SYS &&
      Sys[i]->type != COP_SYS
    ) {
      EXIT0;
    }
  }

  unlink(FREENET_FILE);
  unlink(USER_FILE);
  unlink(DECK_FILE);
  unlink(GANG_FILE);
  unlink(BRIEF_FILE);
  Init_Data_Files();
  Reset_Tournament();
  glob->reset = FALSE;
  NOTIFY("LoC: Game Reset");
  EXIT0;
}
/****************************************************************************
    reset current tournament to defaults
*/
void Reset_Tournament(void) {
  FILE    *fp;
  tourney->results_tallied = FALSE;
  tourney->type = numopt(TTYPE, -32767, 32767);
  tourney->phase = PREGAME;
  tourney->phase_start = time(0);
  tourney->phase_end = lngopt(THOURS1, 1, GCMAXLONG) * 3600L;
  tourney->phase_end += time(0);
  tourney->postgame_hours = numopt(THOURS2, -32767, 32767);

  if (tourney->type == ENDGAME_TOURNEY) {
    tourney->winners = numopt(TPLYERS1, -32767, 32767);
  } else if (tourney->type == SCORE_TOURNEY) {
    tourney->winners = numopt(TPLYERS2, -32767, 32767);
    tourney->score = lngopt(TSCORE, 1, GCMAXLONG);
  } else if (tourney->type == TIMED_TOURNEY) {
    tourney->days = numopt(TDAYS, -32767, 32767);
  } else {
    tourney->phase = GAME;
  }

  tourney->credits[0] = lngopt(TCREDS1, 0, GCMAXLONG);
  tourney->credits[1] = lngopt(TCREDS2, 0, GCMAXLONG);
  tourney->credits[2] = lngopt(TCREDS3, 0, GCMAXLONG);
  tourney->credits[3] = lngopt(TCREDS4, 0, GCMAXLONG);
  tourney->credits[4] = lngopt(TCREDS5, 0, GCMAXLONG);
  tourney->credits[5] = lngopt(TCREDS6, 0, GCMAXLONG);
  tourney->credits[6] = lngopt(TCREDS7, 0, GCMAXLONG);
  tourney->credits[7] = lngopt(TCREDS8, 0, GCMAXLONG);
  tourney->credits[8] = lngopt(TCREDS9, 0, GCMAXLONG);
  strcpy(tourney->prizes[0], stgopt(TPRIZE1));
  strcpy(tourney->prizes[1], stgopt(TPRIZE2));
  strcpy(tourney->prizes[2], stgopt(TPRIZE3));
  strcpy(tourney->prizes[3], stgopt(TPRIZE4));
  strcpy(tourney->prizes[4], stgopt(TPRIZE5));
  strcpy(tourney->prizes[5], stgopt(TPRIZE6));
  strcpy(tourney->prizes[6], stgopt(TPRIZE7));
  strcpy(tourney->prizes[7], stgopt(TPRIZE8));
  strcpy(tourney->prizes[8], stgopt(TPRIZE9));
  unlink(TOURNEY_FILE);
  Create_New(TOURNEY_FILE);
  fp = fopen(TOURNEY_FILE, APPENDB);
  fwrite(tourney, TOURNEY_SIZE, 1, fp);
  fclose(fp);
}
/****************************************************************************
    see if the tournament file needs to be initialized
*/
short EXPORT Init_Tourney(void) {
  short   n;
  FILE    *fp;
  fp = fopen(TOURNEY_FILE, READB);
  n = fread(tourney, TOURNEY_SIZE, 1, fp);
  fclose(fp);

  if (n != 1) {
    /* swenson: disable tournaments for now */
    /* Reset_Tournament();*/
    EXIT0;
  }

  EXIT0;
}
