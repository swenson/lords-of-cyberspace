#include <header.h>
#include <globals.h>

/****************************************************************************
	refresh everyone else's view
*/
void EXPORT Update_Others(short view, short nx, short ny, short cx, short cy) {
  short   old_p = myp;
  short   g;

  if (!glob->server) {
    My_Outprf(MY_PORT->usrnum);
  }

  for (g = 0; g != max_guys; ++g) {
    myp = g;

    if
    (
      Port[g]->active &&
      Port[g]->access_level == view &&
      Port[g]->net_x == nx &&
      Port[g]->net_y == ny
    ) {
      if (view == CITY_VIEW) {
        if (glob->server) {
          MY_PORT->redraw = TRUE;
        } else {
          MY_PORT->redraw = FALSE;
          City_View(-1, -1);
          prf("\x1B[%d;%dH", Port[g]->prompt_y, Port[g]->prompt_x);
          prf("%s", Port[g]->prompt);
          My_Outprf(Port[g]->usrnum);
        }
      } else {
        if (Port[g]->city_x == cx && Port[g]->city_y == cy) {
          if (glob->server) {
            MY_PORT->redraw = TRUE;
          } else {
            MY_PORT->redraw = FALSE;
            System_View(-1, -1);
            prf
            (
              "\x1B[%d;%dH",
              Port[g]->prompt_y,
              Port[g]->prompt_x
            );
            prf("%s", Port[g]->prompt);
            My_Outprf(Port[g]->usrnum);
          }
        }
      }
    }
  }

  myp = old_p;
}
/****************************************************************************
	refresh everyone else's view (for a single move)
*/
void EXPORT Quick_Update
(
  short   view,
  short   nx,
  short   ny,
  short   cx,
  short   cy,
  short   old_x,
  short   old_y,
  short   new_x,
  short   new_y
) {
  short   old_p = myp;
  short   g;

  if (!glob->server) {
    My_Outprf(MY_PORT->usrnum);
  }

  for (g = 0; g != max_guys; ++g) {
    myp = g;

    if
    (
      Port[g]->active &&
      Port[g]->access_level == view &&
      Port[g]->net_x == nx &&
      Port[g]->net_y == ny
    ) {
      if (view == CITY_VIEW) {
        City_View(old_x, old_y);
        City_View(new_x, new_y);
        prf("\x1B[%d;%dH", Port[g]->prompt_y, Port[g]->prompt_x);
        prf("%s", Port[g]->prompt);
        My_Outprf(Port[g]->usrnum);
      } else if (Port[g]->city_x == cx && Port[g]->city_y == cy) {
        System_View(old_x, old_y);
        System_View(new_x, new_y);
        prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
        prf("%s", MY_PORT->prompt);
        My_Outprf(MY_PORT->usrnum);
      }
    }
  }

  myp = old_p;
}
/****************************************************************************
	refresh deferred users
*/
void Server_Update(void) {
  short   old_port = myp;

  for (myp = 0; myp != max_guys; ++myp) {
    if
    (
      MY_PORT->active &&
      MY_PORT->access_level != IDLE &&
      MY_PORT->redraw
    ) {
      MY_PORT->redraw = FALSE;
      Look("X");
      prf("\x1B[%d;%dH", MY_PORT->prompt_y, MY_PORT->prompt_x);
      prf("%s", MY_PORT->prompt);
      My_Outprf(MY_PORT->usrnum);
    }
  }

  myp = old_port;
}
/****************************************************************************
	refresh my display
*/
void Redraw(void) {
  short   x, y;
  prf("%c%s", CTRL_S, CLEAR_SCREEN);
  Main_Prompt();

  if (MY_PORT->access_level != IDLE) {
    for (y = 0; y != MAX_Y; ++y) {
      for (x = 0; x != MAX_X; ++x) {
        MY_PORT->top[y][x] = -1;
      }
    }

    Look("X");
  } else {
    Clear_Net_Display(CLEAR_ALL);
  }

  Reset_Location_Values();
  Update_Location();
  My_Outprf(MY_PORT->usrnum);
}
/*****************************************************************************
   clear out the net display when not jacked in.
*/
void Clear_Net_Display(short option) {
  short   x, y;

  for (y = 0; y != MAX_Y; ++y) {
    for (x = 0; x != MAX_X; ++x) {
      MY_PORT->top[y][x] = -1;
    }
  }

  if (option == CLEAR_ALL) {
    Clear_Display_Window();
    Reset_Location_Values();
  }

  prf("\x1B[1;1H");

  if (me.option[IBM_OPT]) {
    prfmsg(DANET);
  } else {
    prfmsg(DANETA);
  }

  prf("%s\x1B[%d;1H", bold, DISP_Y);
  MY_PORT->last_view = GLOBAL_VIEW;
  My_Outprf(MY_PORT->usrnum);
}
/*****************************************************************************
   clear out the net display and show 'the eye'
*/
void Open_The_Eye(void) {
  short   x, y;
  Reset_Location_Values();

  for (y = 0; y != MAX_Y; ++y) {
    for (x = 0; x != MAX_X; ++x) {
      MY_PORT->top[y][x] = -1;
    }
  }

  Clear_Display_Window();
  prf("\x1B[1;1H");

  if (me.option[IBM_OPT]) {
    prfmsg(DAEYE);
  } else {
    prfmsg(DAEYEA);
  }

  prf("\x1B[%d;1H", DISP_Y);
  MY_PORT->last_view = GLOBAL_VIEW;
}
/******************************************************************************
	position cursor, add string
*/
void EXPORT Put(short y, short x, char *icon, short which) {
  char    an_icon[20];
  strcpy(an_icon, icon);

  if (MY_PORT->top[y][x] != which) {
    MY_PORT->refreshed = TRUE;
    MY_PORT->top[y][x] = which;

    if (!sameas(MY_PORT->last_icon, icon)) {
      strcpy(MY_PORT->last_icon, icon);
    } else {
      stpans(an_icon);
    }

    if (x == MY_PORT->last_x + 1 && y == MY_PORT->last_y) {
      prf("%s", an_icon);
    } else {
      prf("\x1B[%d;%dH%s", y + 1, x + 1, an_icon);
    }

    MY_PORT->last_x = x;
    MY_PORT->last_y = y;
  }
}
/****************************************************************************
	show what's going on in my city
*/
void EXPORT City_View(short quick_x, short quick_y) {
  short   x, y;
  short   x1, y1;
  short   x2, y2;
  short   g;
  short   s, c;
  short   did_guy;
  short   guy[MAXGUYS];
  short   target_num = 0;
  char    target[5];
  short   tactical;
  short   const_type;
  short   visible;
  struct  cpu_struct *runner;
  MY_PORT->refreshed = FALSE;
  MY_PORT->last_icon[0] = 0;
  MY_PORT->last_x = MY_PORT->last_y = -1;

  if (MY_PORT->last_view == GLOBAL_VIEW) {
    quick_x = -1;
    quick_y = -1;

    for (y = 0; y != MAX_Y; ++y) {
      for (x = 0; x != MAX_X; ++x) {
        MY_PORT->top[y][x] = -1;
      }
    }
  }

  for (g = 0; g != max_guys; ++g) {
    guy[g] = FALSE;

    if (g != myp) {
      if
      (
        Port[g]->active &&
        Port[g]->net_x == MY_PORT->net_x &&
        Port[g]->net_y == MY_PORT->net_y &&
        Port[g]->access_level != IDLE
      ) {
        guy[g] = Visible(myp, g);
      }
    }
  }

  if (quick_x == -1 || MY_PORT->tstring[0]) {
    x1 = 0;
    x2 = MAX_CITY_X;
    y1 = 0;
    y2 = MAX_CITY_Y;
  } else {
    x1 = quick_x;
    x2 = quick_x + 1;
    y1 = quick_y;
    y2 = quick_y + 1;
  }

  for (y = y1; y != y2; ++y) {
    for (x = x1; x != x2; ++x) {
      runner = Find_Program
               (
                 MY_PORT->net_x, MY_PORT->net_y,
                 x, y,
                 -1, -1,
                 &s, &c
               );

      if (runner != NULL) {
        visible = Visible_Hydra(myp, s, c);

        if (visible) {
          if
          (
            s != myp &&
            MY_PORT->tstring[0] &&
            sameto(MY_PORT->tstring, THEIR_SOURCE.name)
          ) {
            MY_PORT->top[y][x] = -1;
            tactical = TRUE;
            ++target_num;
            sprintf(target, "%d", target_num);
          } else {
            tactical = FALSE;
            sprintf(target, "%c", THEIR_SOURCE.name[0]);
          }
        }

        if (s == myp) {
          sprintf(str, "%s%s", green, target);
          Put(y, x, str, CONST);
        } else {
          if (!visible) {
            if (me.option[IBM_OPT]) {
              Put(y, x, glob->empty, 0);
            } else {
              Put(y, x, glob->aempty, 0);
            }
          } else {
            if (tactical) {
              const_type = TACT_CONST;
            } else {
              const_type = CONST;
            }

            sprintf(str, "%s%s", red, target);
            Put(y, x, str, const_type);
          }
        }
      } else if (x == MY_PORT->city_x && y == MY_PORT->city_y) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->theguy, THE_PLAYER);
        } else {
          Put(y, x, glob->atheguy, THE_PLAYER);
        }
      } else if (MY_CITY.type[x][y] == 0 || MY_CITY.type[x][y] == PHONE) {
        for (g = did_guy = 0; g < max_guys; ++g) {
          if
          (
            guy[g] &&
            Port[g]->city_x == x &&
            Port[g]->city_y == y
          ) {
            if (me.option[IBM_OPT]) {
              Put(y, x, glob->badguy, A_PLAYER);
            } else {
              Put(y, x, glob->abadguy, A_PLAYER);
            }

            did_guy = TRUE;
          }
        }

        if (!did_guy) {
          if (MY_CITY.type[x][y] == 0) {
            if (me.option[IBM_OPT]) {
              Put(y, x, glob->empty, 0);
            } else {
              Put(y, x, glob->aempty, 0);
            }
          } else if (MY_CITY.type[x][y] == PHONE) {
            if (me.option[IBM_OPT]) {
              Put(y, x, glob->ldlink, PHONE);
            } else {
              Put(y, x, glob->aldlink, PHONE);
            }
          }
        }
      } else if
      (
        MY_MISC_CITY.camoflaged[x][y] &&
        !System_Is_Running(myp, RADAR)
      ) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->empty, 0);
        } else {
          Put(y, x, glob->aempty, 0);
        }
      } else if (MY_CITY.type[x][y] == BBS) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->bbssys, BBS);
        } else {
          Put(y, x, glob->abbssys, BBS);
        }
      } else if (MY_CITY.type[x][y] == LEV4) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->lev4sys, LEV4);
        } else {
          Put(y, x, glob->alev4sys, LEV4);
        }
      } else if (MY_CITY.type[x][y] == LEV5) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->lev5sys, LEV5);
        } else {
          Put(y, x, glob->alev5sys, LEV5);
        }
      } else if (MY_CITY.type[x][y] == LEV6) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->lev6sys, LEV6);
        } else {
          Put(y, x, glob->alev6sys, LEV6);
        }
      } else if (MY_CITY.type[x][y] == LEV7) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->lev7sys, LEV7);
        } else {
          Put(y, x, glob->alev7sys, LEV7);
        }
      } else if (MY_CITY.type[x][y] == CYBERDECK) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->decksys, CYBERDECK);
        } else {
          Put(y, x, glob->adecksys, CYBERDECK);
        }
      } else if (MY_CITY.type[x][y] == VENDOR) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->vendsys, VENDOR);
        } else {
          Put(y, x, glob->avendsys, VENDOR);
        }
      } else if (MY_CITY.type[x][y] == LEV1) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->lev1sys, LEV1);
        } else {
          Put(y, x, glob->alev1sys, LEV1);
        }
      } else if (MY_CITY.type[x][y] == LEV2) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->lev2sys, LEV2);
        } else {
          Put(y, x, glob->alev2sys, LEV2);
        }
      } else if (MY_CITY.type[x][y] == LEV3) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->lev3sys, LEV3);
        } else {
          Put(y, x, glob->alev3sys, LEV3);
        }
      } else if (MY_CITY.type[x][y] == HUB) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->hubsys, HUB);
        } else {
          Put(y, x, glob->ahubsys, HUB);
        }
      } else if (MY_CITY.type[x][y] == CRIMINAL) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->crimesys, CRIMINAL);
        } else {
          Put(y, x, glob->acrimsys, CRIMINAL);
        }
      } else if (MY_CITY.type[x][y] == FILE_SERVER) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->gangsys, FILE_SERVER);
        } else {
          Put(y, x, glob->agngsys, FILE_SERVER);
        }
      } else if (MY_CITY.type[x][y] == WEB_SERVER) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->websys, WEB_SERVER);
        } else {
          Put(y, x, glob->awebsys, WEB_SERVER);
        }
      } else if (MY_CITY.type[x][y] == END_SYS) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->endsys, END_SYS);
        } else {
          Put(y, x, glob->aendsys, END_SYS);
        }
      } else if (MY_CITY.type[x][y] == ALIEN_SYS) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->aliensys, ALIEN_SYS);
        } else {
          Put(y, x, glob->aufosys, ALIEN_SYS);
        }
      } else if (MY_CITY.type[x][y] == CYBERIA_SYS) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->aliensys, CYBERIA_SYS);
        } else {
          Put(y, x, glob->aufosys, CYBERIA_SYS);
        }
      } else if (MY_CITY.type[x][y] == COP_SYS) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->copsys, COP_SYS);
        } else {
          Put(y, x, glob->acopsys, COP_SYS);
        }
      } else if (MY_CITY.type[x][y] == WORLDBANK) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->banksys, WORLDBANK);
        } else {
          Put(y, x, glob->abanksys, WORLDBANK);
        }
      }
    }

    if (MY_PORT->last_view != CITY_VIEW) {
      prf("\x1B[%d;%dH%s", y + 1, MAX_CITY_X + 1, CLEAR_RIGHT);
    }
  }

  if (MY_PORT->last_view != CITY_VIEW) {
    MY_PORT->refreshed = TRUE;
    prf("%s", bold);
    MY_PORT->last_view = CITY_VIEW;
    Reset_Location_Values();
    Update_Location();
  }

  if (MY_PORT->refreshed) {
    prf("\x1B[%d;1H", DISP_Y);
  }
}
/****************************************************************************
	display the global network map
*/
void World_View(void) {
  short   x, y;
  short   net_x;
  short   net_y;

  for (y = 0; y != MAX_Y; ++y) {
    for (x = 0; x != MAX_X; ++x) {
      MY_PORT->top[y][x] = -1;
    }
  }

  for (net_y = 0; net_y != MAX_NET_Y; ++net_y) {
    for (net_x = 0; net_x != MAX_NET_X; ++net_x) {
      if
      (
        MY_PORT->access_level != IDLE &&
        MY_PORT->net_x == net_x &&
        MY_PORT->net_y == net_y
      ) {
        if (me.option[IBM_OPT]) {
          prf
          (
            "\x1B[%d;%dH%s%s",
            net_y + 1,
            (net_x * 2) + 1,
            glob->gtheguy,
            bold
          );
        } else {
          prf
          (
            "\x1B[%d;%dH%s%s",
            net_y + 1,
            (net_x * 2) + 1,
            glob->agtheguy,
            bold
          );
        }
      } else if
      (
        !sameas(world->city[net_x][net_y].name, FRONTIER) &&
        !sameas(world->city[net_x][net_y].name, VALHALLA) &&
        !sameas(world->city[net_x][net_y].name, ALIEN_CITY) &&
        !sameas(world->city[net_x][net_y].name, CYBERIA) &&
        (!world->city[net_x][net_y].invisible || me.perm[GAMEOP_PERM])
      ) {
        if ((net_x / 2) * 2 == net_x) {
          sprintf
          (
            str,
            "%s%c%c",
            yellow,
            world->city[net_x][net_y].name[0],
            world->city[net_x][net_y].name[1]
          );
        } else {
          sprintf
          (
            str,
            "%s%c%c",
            green,
            world->city[net_x][net_y].name[0],
            world->city[net_x][net_y].name[1]
          );
        }

        prf
        (
          "\x1B[%d;%dH%s%s",
          net_y + 1,
          (net_x * 2) + 1,
          str,
          bold
        );
      } else {
        if (me.option[IBM_OPT]) {
          prf
          (
            "\x1B[%d;%dH%s%s",
            net_y + 1,
            (net_x * 2) + 1,
            glob->gempty,
            bold
          );
        } else {
          prf
          (
            "\x1B[%d;%dH%s%s",
            net_y + 1,
            (net_x * 2) + 1,
            glob->agempty,
            bold
          );
        }
      }
    }

    if (MY_PORT->last_view != GLOBAL_VIEW) {
      prf("%s", CLEAR_RIGHT);
    }
  }

  if (MY_PORT->last_view != GLOBAL_VIEW) {
    MY_PORT->last_view = GLOBAL_VIEW;
    Reset_Location_Values();
    Update_Location();
  }

  prf("\x1B[%d;1H", DISP_Y);
}
/****************************************************************************
	show what's going on in my system
*/
void EXPORT System_View(short quick_x, short quick_y) {
  short   x;
  short   y;
  short   s, c;
  short   g;
  short   cpu;
  short   visible;
  short   target_num = 0;
  char    target[5];
  short   tactical;
  short   const_type;
  short   x1, x2;
  short   y1, y2;
  struct  cpu_struct *runner;
  MY_PORT->refreshed = FALSE;
  MY_PORT->last_icon[0] = 0;
  MY_PORT->last_x = MY_PORT->last_y = -1;

  if (MY_PORT->last_view != SYSTEM_VIEW) {
    quick_x = -1;
    quick_y = -1;

    for (y = 0; y != MAX_Y; ++y) {
      for (x = 0; x != MAX_X; ++x) {
        MY_PORT->top[y][x] = -1;
      }
    }
  }

  if (quick_x == -1 || MY_PORT->tstring[0]) {
    x1 = 0;
    x2 = MAX_SYS_X;
    y1 = 0;
    y2 = MAX_SYS_Y;
  } else {
    x1 = quick_x;
    x2 = quick_x + 1;
    y1 = quick_y;
    y2 = quick_y + 1;
    MY_PORT->top[quick_y][quick_x] = -1;
  }

  for (y = y1; y != y2; ++y) {
    for (x = x1; x != x2; ++x) {
      runner = Find_Program
               (
                 MY_PORT->net_x, MY_PORT->net_y,
                 MY_PORT->city_x, MY_PORT->city_y,
                 x, y,
                 &s, &c
               );

      if (runner != NULL) {
        visible = Visible_Hydra(myp, s, c);

        if (!visible) {
          Display_Background(x, y);
        } else {
          if
          (
            s != myp &&
            MY_PORT->tstring[0] &&
            sameto(MY_PORT->tstring, THEIR_SOURCE.name)
          ) {
            MY_PORT->top[y][x] = -1;
            tactical = TRUE;
            ++target_num;
            sprintf(target, "%d", target_num);
          } else {
            tactical = FALSE;
            sprintf(target, "%c", THEIR_SOURCE.name[0]);
          }

          if (s == myp) {
            sprintf(str, "%s%s", green, target);
            Put(y, x, str, CONST);
          } else {
            if (tactical) {
              const_type = TACT_CONST;
            } else {
              const_type = CONST;
            }

            sprintf(str, "%s%s", red, target);
            Put(y, x, str, const_type);
          }
        }
      } else if (MY_SYS->node_type[x][y] == 0) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->empty, 0);
        } else {
          Put(y, x, glob->aempty, 0);
        }
      } else if (MY_SYS->node_type[x][y] == A_PLAYER) {
        if (x == MY_PORT->sys_x && y == MY_PORT->sys_y) {
          if (me.option[IBM_OPT]) {
            Put(y, x, glob->theguy, THE_PLAYER);
          } else {
            Put(y, x, glob->atheguy, THE_PLAYER);
          }
        } else {
          for (g = 0; g < max_guys; ++g) {
            if
            (
              Port[g]->active &&
              Port[g]->sys_num == MY_PORT->sys_num &&
              Port[g]->sys_x == x &&
              Port[g]->sys_y == y
            ) {
              if (Visible(myp, g)) {
                if (me.option[IBM_OPT]) {
                  Put(y, x, glob->badguy, A_PLAYER);
                } else {
                  Put(y, x, glob->abadguy, A_PLAYER);
                }
              } else {
                Display_Background(x, y);
              }
            }
          }
        }
      } else if (MY_SYS->node_type[x][y] == DATA_WALL) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->datawall, DATA_WALL);
        } else {
          Put(y, x, glob->adatawal, DATA_WALL);
        }
      } else if (MY_SYS->node_type[x][y] == PHONE) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->ldlink, PHONE);
        } else {
          Put(y, x, glob->aldlink, PHONE);
        }
      } else if (MY_SYS->node_type[x][y] == DEAD_WALL) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->deadwall, DEAD_WALL);
        } else {
          Put(y, x, glob->adeadwal, DEAD_WALL);
        }
      } else if (MY_SYS->node_type[x][y] == MEMORY) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->sysmem, MEMORY);
        } else {
          Put(y, x, glob->asysmem, MEMORY);
        }
      } else if (MY_SYS->node_type[x][y] == DEAD_MEMORY) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->sysmemx, DEAD_MEMORY);
        } else {
          Put(y, x, glob->asysmemx, DEAD_MEMORY);
        }
      } else if (MY_SYS->node_type[x][y] == CLOSED_GATE) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->clsdgate, CLOSED_GATE);
        } else {
          Put(y, x, glob->aclsgate, CLOSED_GATE);
        }
      } else if (MY_SYS->node_type[x][y] == OPENED_GATE) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->opengate, OPENED_GATE);
        } else {
          Put(y, x, glob->aopngate, OPENED_GATE);
        }
      } else if (MY_SYS->node_type[x][y] == CPU) {
        cpu = MY_SYS->node_index[x][y];

        if (me.option[IBM_OPT]) {
          if (MY_SYS->crash_time[cpu]) {
            Put(y, x, glob->syscpuc, CRASHED_CPU);
          } else {
            Put(y, x, glob->syscpui, CPU);
          }
        } else {
          if (MY_SYS->crash_time[cpu]) {
            Put(y, x, glob->asyscpuc, CRASHED_CPU);
          } else {
            Put(y, x, glob->asyscpui, CPU);
          }
        }
      }
    }

    if (MY_PORT->last_view != SYSTEM_VIEW) {
      prf("\x1B[%d;%dH%s", y + 1, MAX_SYS_X + 1, CLEAR_RIGHT);
    }
  }

  if (MY_PORT->last_view != SYSTEM_VIEW) {
    Reset_Location_Values();
    prf("%s", bold);
    MY_PORT->refreshed = TRUE;
    MY_PORT->last_view = SYSTEM_VIEW;
    Update_Location();
  }

  if (MY_PORT->refreshed) {
    prf("\x1B[%d;1H", DISP_Y);
  }
}
/****************************************************************************
	something invisible is here, put in the background
*/
void Display_Background(short x, short y) {
  if (MY_SYS->node_type[x][y] == 0) {
    if (me.option[IBM_OPT]) {
      Put(y, x, glob->empty, 0);
    } else {
      Put(y, x, glob->aempty, 0);
    }
  } else if (MY_SYS->node_type[x][y] == DATA_WALL) {
    if (me.option[IBM_OPT]) {
      Put(y, x, glob->datawall, DATA_WALL);
    } else {
      Put(y, x, glob->adatawal, DATA_WALL);
    }
  } else if (MY_SYS->node_type[x][y] == DEAD_WALL) {
    if (me.option[IBM_OPT]) {
      Put(y, x, glob->deadwall, DEAD_WALL);
    } else {
      Put(y, x, glob->adeadwal, DEAD_WALL);
    }
  } else if (MY_SYS->node_type[x][y] == CLOSED_GATE) {
    if (me.option[IBM_OPT]) {
      Put(y, x, glob->clsdgate, CLOSED_GATE);
    } else {
      Put(y, x, glob->aclsgate, CLOSED_GATE);
    }
  } else if (MY_SYS->node_type[x][y] == OPENED_GATE) {
    if (me.option[IBM_OPT]) {
      Put(y, x, glob->opengate, OPENED_GATE);
    } else {
      Put(y, x, glob->aopngate, OPENED_GATE);
    }
  }
}
/****************************************************************************
	display the system currently being edited
*/
void   EXPORT Display_A_System(deck)
struct system_struct *deck;
{
  short   i;
  short   x;
  short   y;
  MY_PORT->refreshed = FALSE;
  MY_PORT->last_icon[0] = 0;
  MY_PORT->last_x = MY_PORT->last_y = -1;

  if (MY_PORT->last_view != EDIT_SYSTEM_VIEW) {
    for (y = 0; y != MAX_Y; ++y) {
      for (x = 0; x != MAX_X; ++x) {
        MY_PORT->top[y][x] = -1;
      }
    }
  }

  for (y = 0; y != MAX_SYS_Y; ++y) {
    for (x = 0; x != MAX_SYS_X; ++x) {
      if (deck->node_type[x][y] == 0) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->empty, 0);
        } else {
          Put(y, x, glob->aempty, 0);
        }
      } else if (deck->node_type[x][y] == DATA_WALL) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->datawall, DATA_WALL);
        } else {
          Put(y, x, glob->adatawal, DATA_WALL);
        }
      } else if (deck->node_type[x][y] == PHONE) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->ldlink, PHONE);
        } else {
          Put(y, x, glob->aldlink, PHONE);
        }
      } else if (deck->node_type[x][y] == DEAD_WALL) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->deadwall, DEAD_WALL);
        } else {
          Put(y, x, glob->adeadwal, DEAD_WALL);
        }
      } else if (deck->node_type[x][y] == CLOSED_GATE) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->clsdgate, CLOSED_GATE);
        } else {
          Put(y, x, glob->aclsgate, CLOSED_GATE);
        }
      } else if (deck->node_type[x][y] == OPENED_GATE) {
        if (me.option[IBM_OPT]) {
          Put(y, x, glob->opengate, OPENED_GATE);
        } else {
          Put(y, x, glob->aopngate, OPENED_GATE);
        }
      } else if (deck->node_type[x][y] == MEMORY) {
        if
        (
          STATE == PMEMORY_PROMPT ||
          STATE == MOVE_MEM_PROMPT
        ) {
          sprintf
          (
            str,
            "%s%c",
            blue,
            deck->node_index[x][y] + 'A'
          );
          Put(y, x, str, MEMORY);
        } else if (me.option[IBM_OPT]) {
          Put(y, x, glob->sysmem, MEMORY);
        } else {
          Put(y, x, glob->asysmem, MEMORY);
        }
      } else if (deck->node_type[x][y] == CPU) {
        if (STATE == MOVE_CPU_PROMPT) {
          sprintf
          (
            str,
            "%s%c",
            blue,
            deck->node_index[x][y] + 'A'
          );
          Put(y, x, str, CPU);
        } else if (me.option[IBM_OPT]) {
          Put(y, x, glob->syscpui, CPU);
        } else {
          Put(y, x, glob->asyscpui, CPU);
        }
      } else {
        deck->node_type[x][y] = 0;
      }
    }

    if
    (
      MY_PORT->last_view != EDIT_SYSTEM_VIEW &&
      STATE != MOVE_MEM_PROMPT &&
      STATE != MOVE_CPU_PROMPT
    ) {
      prf("\x1B[%d;%dH", y + 1, MAX_SYS_X + 1);
      prfmsg(MSG608, y, CLEAR_RIGHT);
      MY_PORT->last_icon[0] = 0;
    }
  }

  if (MY_PORT->last_view != EDIT_SYSTEM_VIEW) {
    prf("\x1B[%d;%dH%s", MAX_SYS_Y + 1, 1, CLEAR_RIGHT);

    for (x = i = 0; x != MAX_SYS_X; ++x) {
      prfmsg(MSG609, i);
      ++i;

      if (i == 10) {
        i = 0;
      }
    }

    MY_PORT->refreshed = TRUE;
    MY_PORT->last_view = EDIT_SYSTEM_VIEW;
  }

  if (MY_PORT->refreshed) {
    prf("\x1B[%d;1H", DISP_Y);
    My_Outprf(MY_PORT->usrnum);
  }
}
