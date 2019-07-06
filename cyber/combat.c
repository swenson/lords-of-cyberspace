#include <header.h>
#include <globals.h>

/****************************************************************************
    see if hydra can defend an attack
*/
short EXPORT Hydra_Defense_Check(short s, short i) {
  short   guy_roll;
  short   my_roll;
  short   sys_num;
  short   cpu_num;
  short   secs;
  sys_num = SYS_PROGRAM.target_s;
  cpu_num = SYS_PROGRAM.target_index;

  if (SYS_PROGRAM.loading) {
    return (TRUE);
  }

  if (Hydra_Has(SHIELD, sys_num, cpu_num)) {
    guy_roll = SEEK_SOURCE.strength + RANDOM(glob->fate);
    my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

    if (guy_roll > my_roll) {
      sprintf
      (
        str,
        getmsg(MSG36),
        SYS_SOURCE.name,
        SEEK_SOURCE.name
      );
      Send(str, s, FALSE);
      Program_Stops(s, i);
      return (FALSE);
    }
  }

  if (Hydra_Has(ARMOR, sys_num, cpu_num)) {
    guy_roll = SEEK_SOURCE.strength + RANDOM(glob->fate);
    my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

    if (guy_roll > my_roll) {
      sprintf
      (
        str,
        getmsg(MSG37),
        SYS_SOURCE.name,
        SEEK_SOURCE.name
      );
      Send(str, s, FALSE);
      Program_Stops(s, i);
      return (FALSE);
    }
  }

  if (Hydra_Has(STATIK, sys_num, cpu_num)) {
    guy_roll = SEEK_SOURCE.strength + RANDOM(glob->fate);
    my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

    if (guy_roll > my_roll) {
      sprintf
      (
        str,
        getmsg(MSG38),
        SYS_SOURCE.name,
        SEEK_SOURCE.name
      );
      Send(str, s, FALSE);
      secs = SEEK_SOURCE.strength * glob->statsecs;

      if (SYS_PROGRAM.loading < secs) {
        SYS_PROGRAM.loading = secs;
      }

      return (FALSE);
    }
  }

  return (TRUE);
}
/****************************************************************************
    see if I can defend an attack
*/
short EXPORT Defense_Check(short s, short i, short g) {
  short   p;
  short   m;
  short   c;
  short   strength;
  short   guy_roll;
  short   my_roll;
  short   secs;

  for (p = 0; p != Sys[g]->cpus; ++p) {
    if
    (
      !GUY_PROGRAM.loading &&
      GUY_PROGRAM.m_index >= 0 &&
      (GUY_SOURCE.index == STATIK || GUY_SOURCE.index == ARMOR)
    ) {
      guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
      my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

      if (GUY_SOURCE.index == STATIK) {
        if (guy_roll > my_roll) {
          sprintf
          (
            str,
            getmsg(MSG41),
            SYS_SOURCE.name,
            GUY_SOURCE.name
          );
          Send(str, s, FALSE);
          Send(str, g, FALSE);
          secs = GUY_SOURCE.strength * glob->statsecs;

          if (SYS_PROGRAM.loading < secs) {
            SYS_PROGRAM.loading = secs;
          }

          return (FALSE);
        }

        sprintf
        (
          str,
          getmsg(MSG42),
          GUY_SOURCE.name,
          SYS_SOURCE.name
        );
        Send(str, g, FALSE);
      } else {
        if (guy_roll > my_roll) {
          sprintf
          (
            str,
            getmsg(MSG39),
            SYS_SOURCE.name,
            GUY_SOURCE.name
          );
          Send(str, s, FALSE);
          Send(str, g, FALSE);
          Program_Stops(s, i);
          return (FALSE);
        }
      }
    }
  }

  for (p = 0; p != Sys[g]->cpus; ++p) {
    if
    (
      !GUY_PROGRAM.loading &&
      GUY_PROGRAM.m_index >= 0 &&
      GUY_SOURCE.index == SHIELD
    ) {
      guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
      my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

      if (guy_roll > my_roll) {
        sprintf
        (
          str,
          getmsg(MSG39),
          SYS_SOURCE.name,
          GUY_SOURCE.name
        );
        Send(str, s, FALSE);
        Send(str, g, FALSE);
        Program_Stops(s, i);
        return (FALSE);
      }

      --GUY_SOURCE.strength;

      if (my_roll > guy_roll * 2) {
        --GUY_SOURCE.strength;

        if (RANDOM(2) == 1) {
          --GUY_SOURCE.strength;
        }
      }

      strength = GUY_SOURCE.strength;
      m = GUY_PROGRAM.m_index;
      c = GUY_PROGRAM.p_index;
      sprintf
      (
        str,
        getmsg(MXX40),
        GUY_SOURCE.name,
        SYS_SOURCE.name,
        GUY_SOURCE.name,
        GUY_SOURCE.strength
      );
      Send(str, g, FALSE);
      Send(str, s, FALSE);

      if (strength <= 0) {
        sprintf(str, getmsg(MSG894), GUY_SOURCE.name);
        Send(str, g, FALSE);
        Program_Stops(g, p);
        Remove_From_Memory(g, m, c);
      }

      Program_Stops(s, i);
      return (FALSE);
    }
  }

  return (TRUE);
}
/****************************************************************************
    see if armor saved me some damage
*/
short EXPORT Armor_Check(short s, short i, short g, short dmg) {
  short   p;
  short   saved;

  for (p = 0; p != Sys[g]->cpus && dmg; ++p) {
    if
    (
      !GUY_PROGRAM.loading &&
      GUY_PROGRAM.m_index >= 0 &&
      GUY_SOURCE.index == ARMOR
    ) {
      saved = RANDOM(3);
      dmg -= saved;

      if (dmg < 0) {
        dmg = 0;
      }

      if (saved > 1) {
        sprintf
        (
          str,
          getmsg(MSG43),
          GUY_SOURCE.name,
          saved,
          SYS_SOURCE.name
        );
      } else {
        sprintf
        (
          str,
          getmsg(MSSG43),
          GUY_SOURCE.name,
          saved,
          SYS_SOURCE.name
        );
      }

      Send(str, g, FALSE);
    }
  }

  return (dmg);
}
/****************************************************************************
    see if armor saved hydra some damage
*/
short EXPORT Hydra_Armor_Check(short s, short i, short dmg) {
  short   saved;
  short   sys_num;
  short   cpu_num;
  sys_num = SYS_PROGRAM.target_s;
  cpu_num = SYS_PROGRAM.target_index;

  if (SYS_PROGRAM.loading) {
    return (dmg);
  }

  if (Hydra_Has(ARMOR, sys_num, cpu_num)) {
    saved = RANDOM(3);
    dmg -= saved;

    if (dmg < 0) {
      dmg = 0;
    }

    if (saved > 1) {
      sprintf
      (
        str,
        getmsg(MSG44),
        SEEK_SOURCE.name,
        saved,
        SYS_SOURCE.name
      );
    } else {
      sprintf
      (
        str,
        getmsg(MSG45),
        SEEK_SOURCE.name,
        saved,
        SYS_SOURCE.name
      );
    }

    Send(str, sys_num, FALSE);
  }

  return (dmg);
}
/****************************************************************************
    see if armor saved me some damage
*/
short EXPORT Combat_Armor_Check(short s, short i, short g, short dmg) {
  short   p;
  short   guy_roll;
  short   my_roll;

  for (p = 0; p != Sys[g]->cpus && dmg; ++p) {
    if
    (
      !GUY_PROGRAM.loading &&
      GUY_PROGRAM.m_index >= 0 &&
      GUY_SOURCE.index == ARMOR
    ) {
      guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
      my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

      if (guy_roll > my_roll) {
        --dmg;

        if (dmg < 0) {
          dmg = 0;
        }

        sprintf(str, getmsg(MSG396), GUY_SOURCE.name, SYS_SOURCE.name);
        Send(str, g, FALSE);
        prfmsg(MSG397, GUY_SOURCE.name, SYS_SOURCE.name);
      }
    }
  }

  return (dmg);
}
/****************************************************************************
    see if I can defend an attack
*/
short Combat_Defense_Check(short s, short i, short g) {
  short   p;
  short   m;
  short   c;
  short   strength;
  short   guy_roll;
  short   my_roll;
  short   secs;

  for (p = 0; p != Sys[g]->cpus; ++p) {
    if
    (
      !GUY_PROGRAM.loading &&
      GUY_PROGRAM.m_index >= 0 &&
      (GUY_SOURCE.index == STATIK || GUY_SOURCE.index == ARMOR)
    ) {
      guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
      my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

      if (GUY_SOURCE.index == ARMOR) {
        if (guy_roll > my_roll) {
          sprintf
          (
            str,
            getmsg(MSG398),
            SYS_SOURCE.name,
            GUY_SOURCE.name
          );
          Send(str, g, FALSE);
          prfmsg(MSG399, SYS_SOURCE.name, GUY_SOURCE.name);
          SYS_PROGRAM.loading = GUY_SOURCE.strength;
          return (FALSE);
        }
      } else if (GUY_SOURCE.index == STATIK) {
        if (guy_roll > my_roll) {
          sprintf
          (
            str,
            getmsg(MSG400),
            SYS_SOURCE.name,
            GUY_SOURCE.name
          );
          Send(str, g, FALSE);
          prfmsg(MSG401, SYS_SOURCE.name, GUY_SOURCE.name);
          secs = GUY_SOURCE.strength * glob->statsecs;

          if (SYS_PROGRAM.loading < secs) {
            SYS_PROGRAM.loading = secs;
          }

          return (FALSE);
        }
      }
    }
  }

  for (p = 0; p != Sys[g]->cpus; ++p) {
    if
    (
      !GUY_PROGRAM.loading &&
      GUY_PROGRAM.m_index >= 0 &&
      GUY_SOURCE.index == SHIELD
    ) {
      SYS_PROGRAM.loading = GUY_SOURCE.strength;
      guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
      my_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

      if (guy_roll > my_roll) {
        sprintf
        (
          str,
          getmsg(MSG398),
          SYS_SOURCE.name,
          GUY_SOURCE.name
        );
        Send(str, g, FALSE);
        prfmsg(MSG399, SYS_SOURCE.name, GUY_SOURCE.name);
        Program_Stops(s, i);
        return (FALSE);
      }

      --GUY_SOURCE.strength;

      if (my_roll > guy_roll * 2) {
        --GUY_SOURCE.strength;

        if (RANDOM(2) == 1) {
          --GUY_SOURCE.strength;
        }
      }

      strength = GUY_SOURCE.strength;
      m = GUY_PROGRAM.m_index;
      c = GUY_PROGRAM.p_index;
      sprintf
      (
        str,
        getmsg(MYY40),
        GUY_SOURCE.name,
        glob->name,
        SYS_SOURCE.name,
        GUY_SOURCE.name,
        GUY_SOURCE.strength
      );
      Send(str, g, FALSE);
      prfmsg
      (
        MXX40,
        GUY_SOURCE.name,
        SYS_SOURCE.name,
        GUY_SOURCE.name,
        GUY_SOURCE.strength
      );
      prf(NL);

      if (strength <= 0) {
        sprintf(str, getmsg(MSG894), GUY_SOURCE.name);
        Send(str, g, FALSE);
        Program_Stops(g, p);
        Remove_From_Memory(g, m, c);
      }

      Program_Stops(s, i);
      return (FALSE);
    }
  }

  return (TRUE);
}
/****************************************************************************
    user ran into a program, see if we hurt it
*/
short EXPORT Ran_Into_Program(short g, short p) {
  short   i;
  short   m;
  short   c;
  short   dmg;
  short   guy_roll;
  short   my_roll;
  short   nailed;
  short   dead = FALSE;
  short   did_something = FALSE;
  short   score;

  if (g == myp) {
    prfmsg(MSG414, GUY_SOURCE.name);
    EXIT0;
  }

  if (g < max_guys && His_Rank(me.score, me.won) < glob->pvprank) {
    prfmsg(MSG414, GUY_SOURCE.name);
    prfmsg(M661, glob->pvprank);
    EXIT0;
  }

  if
  (
    g < max_guys &&
    His_Rank(Port[g]->guy.score, Port[g]->guy.won) < glob->pvprank
  ) {
    prfmsg(MSG414, GUY_SOURCE.name);
    prfmsg(M662, glob->pvprank);
    EXIT0;
  }

  if (glob->outlaw && g < max_guys) {
    if (!me.outlaw) {
      prfmsg(MSG414, GUY_SOURCE.name);
      prfmsg(MSSG670);
      EXIT0;
    }

    if (!Port[g]->guy.outlaw) {
      prfmsg(MSG414, GUY_SOURCE.name);
      prfmsg(MSSG671);
      EXIT0;
    }
  }

  if (Visible(g, myp)) {
    strcpy(glob->name, me.alias);
  } else {
    sprintf(glob->name, "Cloaked %s", me.alias);
  }

  if (Visible_Hydra(myp, g, p)) {
    strcpy(glob->name2, GUY_SOURCE.name);
  } else {
    sprintf(glob->name2, "Cloaked %s", GUY_SOURCE.name);
  }

  for (i = 0; i != MY_DECK->cpus && !dead; ++i) {
    if
    (
      MY_PROGRAM.m_index >= 0 &&
      MY_PROGRAM.loading &&
      (
        MY_SOURCE.index == TENTACLE ||
        MY_SOURCE.index == GLUEGUN ||
        MY_SOURCE.index == COBRA ||
        MY_SOURCE.index == PHASER
      )
    ) {
      prfmsg(MSG802, MY_PROGRAM.loading, MY_SOURCE.name);
    } else if
    (
      MY_PROGRAM.m_index >= 0 &&
      MY_SOURCE.index == TENTACLE &&
      !MY_PROGRAM.loading
    ) {
      if (Sys[g]->type == CON_SYS) {
        GUY_PROGRAM.target_index = me.index;
      } else {
        Proc[g]->enemy[myp] = TRUE;
      }

      MY_PROGRAM.loading = Loading_Time
                           (
                             myp,
                             MY_PROGRAM.m_index,
                             MY_PROGRAM.p_index
                           );

      if (GUY_SOURCE.class == HYDRA) {
        MY_PROGRAM.target_s = g;
        MY_PROGRAM.target_index = p;
        nailed = Hydra_Defense_Check(myp, i);
      } else {
        nailed = TRUE;
      }

      if (nailed) {
        guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
        my_roll = MY_SOURCE.strength + RANDOM(glob->fate);

        if (my_roll > guy_roll) {
          did_something = TRUE;
          prfmsg(MSG402, MY_SOURCE.name, glob->name2);
          sprintf
          (
            str,
            getmsg(MSG403),
            GUY_SOURCE.name,
            glob->name,
            MY_SOURCE.name
          );
          Send(str, g, FALSE);

          if (GUY_PROGRAM.loading < glob->tentsecs) {
            GUY_PROGRAM.loading = glob->tentsecs;
            MY_PROGRAM.loading += glob->tentsecs;
          }
        }
      }
    } else if
    (
      MY_PROGRAM.m_index >= 0 &&
      MY_SOURCE.index == GLUEGUN &&
      !MY_PROGRAM.loading
    ) {
      if (Sys[g]->type == CON_SYS) {
        GUY_PROGRAM.target_index = me.index;
      } else {
        Proc[g]->enemy[myp] = TRUE;
      }

      MY_PROGRAM.loading = Loading_Time
                           (
                             myp,
                             MY_PROGRAM.m_index,
                             MY_PROGRAM.p_index
                           );

      if (GUY_SOURCE.class == HYDRA) {
        MY_PROGRAM.target_s = g;
        MY_PROGRAM.target_index = p;
        nailed = Hydra_Defense_Check(myp, i);
      } else {
        nailed = TRUE;
      }

      if (nailed) {
        guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
        my_roll = MY_SOURCE.strength + RANDOM(glob->fate);

        if (my_roll > guy_roll) {
          did_something = TRUE;
          prfmsg(MSG404, MY_SOURCE.name, glob->name2);
          sprintf
          (
            str,
            getmsg(MSG405),
            glob->name,
            MY_SOURCE.name,
            GUY_SOURCE.name
          );
          Send(str, g, FALSE);
          dmg = MY_SOURCE.strength * glob->gluesecs;

          if (GUY_PROGRAM.loading < dmg) {
            GUY_PROGRAM.loading = dmg;
          }
        }
      }
    } else if
    (
      MY_PROGRAM.m_index >= 0 &&
      MY_SOURCE.index == COBRA &&
      !MY_PROGRAM.loading
    ) {
      if (Sys[g]->type == CON_SYS) {
        GUY_PROGRAM.target_index = me.index;
      } else {
        Proc[g]->enemy[myp] = TRUE;
      }

      MY_PROGRAM.loading = Loading_Time
                           (
                             myp,
                             MY_PROGRAM.m_index,
                             MY_PROGRAM.p_index
                           );

      if (GUY_SOURCE.class == HYDRA) {
        MY_PROGRAM.target_s = g;
        MY_PROGRAM.target_index = p;
        nailed = Hydra_Defense_Check(myp, i);
      } else {
        nailed = TRUE;
      }

      if (nailed) {
        guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
        my_roll = MY_SOURCE.strength + RANDOM(glob->fate);

        if (my_roll > guy_roll) {
          did_something = TRUE;
          dmg = RANDOM(MY_SOURCE.strength);

          if (GUY_SOURCE.class == HYDRA) {
            dmg = Hydra_Armor_Check(myp, i, dmg);
          }

          if (dmg) {
            if (dmg >= GUY_SOURCE.strength) {
              dmg = GUY_SOURCE.strength;

              if (GUY_SOURCE.value) {
                score = GUY_SOURCE.strength;
              } else {
                score = 0;
              }

              prfmsg(MSG406, MY_SOURCE.name, glob->name2);
              sprintf
              (
                str,
                getmsg(MSG407),
                glob->name,
                MY_SOURCE.name,
                GUY_SOURCE.name
              );
              Send(str, g, FALSE);
            } else {
              prfmsg(MSG408, MY_SOURCE.name, glob->name2, dmg);
              sprintf
              (
                str,
                getmsg(MSG409),
                glob->name,
                MY_SOURCE.name,
                GUY_SOURCE.name,
                dmg
              );
              Send(str, g, FALSE);
            }

            GUY_SOURCE.strength -= dmg;
            m = GUY_PROGRAM.m_index;
            c = GUY_PROGRAM.p_index;

            if (GUY_SOURCE.strength <= 0) {
              Program_Stops(g, p);
              Remove_From_Memory(g, m, c);

              if (score) {
                Scored(myp, score, 1);
              }
            }
          }
        }
      }
    } else if
    (
      MY_PROGRAM.m_index >= 0 &&
      MY_SOURCE.index == PHASER &&
      !MY_PROGRAM.loading
    ) {
      if (Sys[g]->type == CON_SYS) {
        GUY_PROGRAM.target_index = me.index;
      } else {
        Proc[g]->enemy[myp] = TRUE;
      }

      MY_PROGRAM.loading = Loading_Time
                           (
                             myp,
                             MY_PROGRAM.m_index,
                             MY_PROGRAM.p_index
                           );

      if (GUY_SOURCE.class == HYDRA) {
        MY_PROGRAM.target_s = g;
        MY_PROGRAM.target_index = p;
        nailed = Hydra_Defense_Check(myp, i);
      } else {
        nailed = TRUE;
      }

      if (nailed) {
        guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
        my_roll = MY_SOURCE.strength + RANDOM(glob->fate);

        if (my_roll > guy_roll) {
          did_something = TRUE;
          dmg = RANDOM(MY_SOURCE.strength) * glob->phsrpowr;

          if (GUY_SOURCE.class == HYDRA) {
            dmg = Hydra_Armor_Check(myp, i, dmg);
          }

          if (dmg) {
            if (dmg >= GUY_PROGRAM.rez) {
              dmg = GUY_PROGRAM.rez;
              prfmsg(MSG410, MY_SOURCE.name, glob->name2);
              sprintf
              (
                str,
                getmsg(MSG411),
                glob->name,
                MY_SOURCE.name,
                GUY_SOURCE.name
              );
              Send(str, g, FALSE);
            } else {
              prfmsg(MSG412, MY_SOURCE.name, glob->name2, dmg);
              sprintf
              (
                str,
                getmsg(MSG413),
                glob->name,
                MY_SOURCE.name,
                GUY_SOURCE.name,
                dmg
              );
              Send(str, g, FALSE);
            }

            GUY_PROGRAM.rez -= dmg;

            if (GUY_PROGRAM.rez <= 0) {
              Program_Stops(g, p);
            }
          }
        }
      }
    }
  }

  if (!did_something) {
    prfmsg(MSG414, glob->name2);
  }

  EXIT0;
}
/****************************************************************************
    user ran into another player, see if we hurt them
*/
short EXPORT Ran_Into_Guy(short g) {
  short   p;
  short   my_roll;
  short   guy_roll;
  short   dmg;
  short   i;
  short   c;
  short   m;
  short   nailed;
  short   did_something = FALSE;
  short   old_port = myp;
  short   dead = FALSE;
  short   score;

  if (Visible(g, myp)) {
    strcpy(glob->name, me.alias);
  } else {
    sprintf(glob->name, "Cloaked %s", me.alias);
  }

  if (Visible(myp, g)) {
    strcpy(glob->name2, Port[g]->guy.alias);
  } else {
    sprintf(glob->name2, "Cloaked %s", Port[g]->guy.alias);
  }

  if (!me.perm[GAMEOP_PERM] && His_Rank(me.score, me.won) < glob->pvprank) {
    prfmsg(MSG414, glob->name2);
    prfmsg(M661, glob->pvprank);
    EXIT0;
  }

  if
  (
    !me.perm[GAMEOP_PERM] &&
    His_Rank(Port[g]->guy.score, Port[g]->guy.won) < glob->pvprank
  ) {
    prfmsg(MSG414, glob->name2);
    prfmsg(M662, glob->pvprank);
    EXIT0;
  }

  if (!me.perm[GAMEOP_PERM] && glob->outlaw) {
    if (!me.outlaw) {
      prfmsg(MSG414, glob->name2);
      prfmsg(MSSG670);
      EXIT0;
    }

    if (!Port[g]->guy.outlaw) {
      prfmsg(MSG414, glob->name2);
      prfmsg(MSSG671);
      EXIT0;
    }
  }

  if (!MY_PORT->enemy[g]) {
    prfmsg(MSG414, glob->name2);
    EXIT0;
  }

  for (i = 0; i != MY_DECK->cpus && !dead; ++i) {
    if
    (
      MY_PROGRAM.m_index >= 0 &&
      MY_PROGRAM.loading &&
      (
        MY_SOURCE.index == CHAINSAW ||
        MY_SOURCE.index == COBRA ||
        MY_SOURCE.index == PHASER ||
        MY_SOURCE.index == SKULLDRILL ||
        MY_SOURCE.index == TENTACLE ||
        MY_SOURCE.index == CATTLEPROD ||
        MY_SOURCE.index == GLUEGUN
      )
    ) {
      prfmsg(MSG802, MY_PROGRAM.loading, MY_SOURCE.name);
    } else if
    (
      MY_PROGRAM.m_index >= 0 &&
      MY_SOURCE.index == CHAINSAW &&
      !MY_PROGRAM.loading
    ) {
      if (MY_PORT->enemy[g] == -1) {
        sprintf(str, getmsg(MSG895), glob->name2);
        Prompt(str, COMBAT_PROMPT);
        MY_PORT->misc = g;
        EXIT0;
      }

      did_something = TRUE;
      MY_PROGRAM.loading = Loading_Time
                           (
                             myp,
                             MY_PROGRAM.m_index,
                             MY_PROGRAM.p_index
                           );
      nailed = Combat_Defense_Check(myp, i, g);

      if (nailed) {
        dmg = Combat_Armor_Check(myp, i, g, glob->chaindmg);

        if (dmg) {
          prfmsg(MSG416, MY_SOURCE.name, glob->name2, dmg);
          My_Outprf(usrnum);
          sprintf
          (
            str,
            getmsg(MSG417),
            glob->name,
            MY_SOURCE.name,
            dmg
          );
          Send(str, g, FALSE);
          Port[g]->guy.damage += dmg;

          if (Port[g]->guy.damage >= glob->dpts) {
            strcpy(str, getmsg(MSG15));
            Send(str, g, FALSE);
            myp = g;
            Jack_Out();
            dead = TRUE;
          } else {
            myp = g;
            Update_Location();
          }

          myp = old_port;
        }
      }
    } else if
    (
      MY_PROGRAM.m_index >= 0 &&
      MY_SOURCE.index == SKULLDRILL &&
      !MY_PROGRAM.loading &&
      Port[g]->guy.score > 0
    ) {
      if (MY_PORT->enemy[g] == -1) {
        sprintf(str, getmsg(MSG895), glob->name2);
        Prompt(str, COMBAT_PROMPT);
        MY_PORT->misc = g;
        EXIT0;
      }

      did_something = TRUE;
      MY_PROGRAM.loading = Loading_Time
                           (
                             myp,
                             MY_PROGRAM.m_index,
                             MY_PROGRAM.p_index
                           );
      nailed = Combat_Defense_Check(myp, i, g);

      if (nailed) {
        dmg = Combat_Armor_Check(myp, i, g, glob->drilldmg);

        if (dmg) {
          prfmsg(MSG418, MY_SOURCE.name, glob->name2, dmg);
          My_Outprf(usrnum);
          sprintf
          (
            str,
            getmsg(MSSG419),
            glob->name,
            MY_SOURCE.name,
            dmg
          );
          Send(str, g, FALSE);
          myp = g;
          me.score -= dmg;

          if (me.score < 0) {
            me.score = 0;
          }

          Update_Location();
          myp = old_port;
        }
      }
    } else if
    (
      MY_PROGRAM.m_index >= 0 &&
      MY_SOURCE.index == TENTACLE &&
      !MY_PROGRAM.loading
    ) {
      if (MY_PORT->enemy[g] == -1) {
        sprintf(str, getmsg(MSG895), glob->name2);
        Prompt(str, COMBAT_PROMPT);
        MY_PORT->misc = g;
        EXIT0;
      }

      did_something = TRUE;
      MY_PROGRAM.loading = Loading_Time
                           (
                             myp,
                             MY_PROGRAM.m_index,
                             MY_PROGRAM.p_index
                           );
      nailed = Combat_Defense_Check(myp, i, g);

      if (nailed) {
        prfmsg(MSG402, MY_SOURCE.name, glob->name2);
        sprintf
        (
          str,
          getmsg(MSG421),
          glob->name,
          MY_SOURCE.name
        );
        Send(str, g, FALSE);

        if (Port[g]->icebolt_time < glob->tentsecs) {
          Port[g]->icebolt_time = glob->tentsecs;
          MY_PROGRAM.loading += glob->tentsecs;
        }
      }
    } else if
    (
      MY_PROGRAM.m_index >= 0 &&
      MY_SOURCE.index == CATTLEPROD &&
      !MY_PROGRAM.loading
    ) {
      if (MY_PORT->enemy[g] == -1) {
        sprintf(str, getmsg(MSG895), glob->name2);
        Prompt(str, COMBAT_PROMPT);
        MY_PORT->misc = g;
        EXIT0;
      }

      did_something = TRUE;
      MY_PROGRAM.loading = Loading_Time
                           (
                             myp,
                             MY_PROGRAM.m_index,
                             MY_PROGRAM.p_index
                           );
      nailed = Combat_Defense_Check(myp, i, g);

      if (nailed) {
        prfmsg(MSG422, MY_SOURCE.name, glob->name2);
        My_Outprf(usrnum);
        sprintf
        (
          str,
          getmsg(MSG423),
          glob->name,
          MY_SOURCE.name
        );
        Send(str, g, FALSE);
        strcpy(str, getmsg(MSG63));
        Send(str, g, FALSE);
        myp = g;

        for (c = 0; c != MY_DECK->cpus; ++c) {
          MY_DECK->crash_time[c] = glob->prodsecs;
        }

        Jack_Out();
        dead = TRUE;
        myp = old_port;
      }
    } else if
    (
      MY_PROGRAM.m_index >= 0 &&
      MY_SOURCE.index == GLUEGUN &&
      !MY_PROGRAM.loading
    ) {
      if (MY_PORT->enemy[g] == -1) {
        sprintf(str, getmsg(MSG895), glob->name2);
        Prompt(str, COMBAT_PROMPT);
        MY_PORT->misc = g;
        EXIT0;
      }

      MY_PROGRAM.loading = Loading_Time
                           (
                             myp,
                             MY_PROGRAM.m_index,
                             MY_PROGRAM.p_index
                           );
      nailed = Combat_Defense_Check(myp, i, g);

      if (nailed) {
        for (p = 0; p != Sys[g]->cpus; ++p) {
          if
          (
            GUY_PROGRAM.m_index >= 0 &&
            (
              GUY_SOURCE.index == NETSPEED ||
              GUY_SOURCE.index == CLOAK ||
              GUY_SOURCE.index == RADAR ||
              GUY_SOURCE.index == MUFFLER ||
              GUY_SOURCE.index == SHIELD ||
              GUY_SOURCE.index == ARMOR ||
              GUY_SOURCE.index == CHAINSAW ||
              GUY_SOURCE.index == SKULLDRILL ||
              GUY_SOURCE.index == TENTACLE ||
              GUY_SOURCE.index == CATTLEPROD ||
              GUY_SOURCE.index == GLUEGUN ||
              GUY_SOURCE.index == STATIK ||
              GUY_SOURCE.index == PHASER ||
              GUY_SOURCE.index == COBRA
            )
          ) {
            guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
            my_roll = MY_SOURCE.strength + RANDOM(glob->fate);

            if (my_roll > guy_roll) {
              did_something = TRUE;
              prfmsg(MSG404, MY_SOURCE.name, GUY_SOURCE.name);
              sprintf
              (
                str,
                getmsg(MSG405),
                glob->name,
                MY_SOURCE.name,
                GUY_SOURCE.name
              );
              Send(str, g, FALSE);
              dmg = MY_SOURCE.strength * glob->gluesecs;

              if (GUY_PROGRAM.loading < dmg) {
                GUY_PROGRAM.loading = dmg;
              }
            }
          }
        }
      }
    } else if
    (
      MY_PROGRAM.m_index >= 0 &&
      MY_SOURCE.index == COBRA &&
      !MY_PROGRAM.loading
    ) {
      if (MY_PORT->enemy[g] == -1) {
        sprintf(str, getmsg(MSG895), glob->name2);
        Prompt(str, COMBAT_PROMPT);
        MY_PORT->misc = g;
        EXIT0;
      }

      nailed = Combat_Defense_Check(myp, i, g);

      if (nailed) {
        for (p = 0; p != Sys[g]->cpus; ++p) {
          if
          (
            GUY_PROGRAM.m_index >= 0 &&
            (
              GUY_SOURCE.index == CLOAK ||
              GUY_SOURCE.index == RADAR ||
              GUY_SOURCE.index == MUFFLER ||
              GUY_SOURCE.index == SHIELD ||
              GUY_SOURCE.index == ARMOR ||
              GUY_SOURCE.index == CHAINSAW ||
              GUY_SOURCE.index == SKULLDRILL ||
              GUY_SOURCE.index == TENTACLE ||
              GUY_SOURCE.index == CATTLEPROD ||
              GUY_SOURCE.index == GLUEGUN ||
              GUY_SOURCE.index == STATIK ||
              GUY_SOURCE.index == PHASER ||
              GUY_SOURCE.index == COBRA ||
              GUY_SOURCE.class == ENDKEY ||
              GUY_SOURCE.index == NETSPEED
            )
          ) {
            MY_PROGRAM.loading = Loading_Time
                                 (
                                   myp,
                                   MY_PROGRAM.m_index,
                                   MY_PROGRAM.p_index
                                 );
            guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
            my_roll = MY_SOURCE.strength + RANDOM(glob->fate);

            if (my_roll > guy_roll) {
              did_something = TRUE;
              dmg = RANDOM(MY_SOURCE.strength);

              if (dmg >= GUY_SOURCE.strength) {
                dmg = GUY_SOURCE.strength;

                if (GUY_SOURCE.value) {
                  score = GUY_SOURCE.strength;
                } else {
                  score = 0;
                }

                prfmsg
                (
                  MSG406,
                  MY_SOURCE.name,
                  GUY_SOURCE.name
                );
                sprintf
                (
                  str,
                  getmsg(MSG407),
                  glob->name,
                  MY_SOURCE.name,
                  GUY_SOURCE.name
                );
                Send(str, g, FALSE);
              } else {
                prfmsg
                (
                  MSG408,
                  MY_SOURCE.name,
                  GUY_SOURCE.name,
                  dmg
                );
                sprintf
                (
                  str,
                  getmsg(MSG409),
                  glob->name,
                  MY_SOURCE.name,
                  GUY_SOURCE.name,
                  dmg
                );
                Send(str, g, FALSE);
              }

              GUY_SOURCE.strength -= dmg;
              m = GUY_PROGRAM.m_index;
              c = GUY_PROGRAM.p_index;

              if (GUY_SOURCE.strength <= 0) {
                Program_Stops(g, p);
                Remove_From_Memory(g, m, c);

                if (score) {
                  Scored(myp, score, 1);
                }
              }
            }
          }
        }
      }
    } else if
    (
      MY_PROGRAM.m_index >= 0 &&
      MY_SOURCE.index == PHASER &&
      !MY_PROGRAM.loading
    ) {
      if (MY_PORT->enemy[g] == -1) {
        sprintf(str, getmsg(MSG895), glob->name2);
        Prompt(str, COMBAT_PROMPT);
        MY_PORT->misc = g;
        EXIT0;
      }

      MY_PROGRAM.loading = Loading_Time
                           (
                             myp,
                             MY_PROGRAM.m_index,
                             MY_PROGRAM.p_index
                           );
      nailed = Combat_Defense_Check(myp, i, g);

      if (nailed) {
        for (p = 0; p != Sys[g]->cpus; ++p) {
          if
          (
            GUY_PROGRAM.m_index >= 0 &&
            (
              GUY_SOURCE.index == CLOAK ||
              GUY_SOURCE.index == RADAR ||
              GUY_SOURCE.index == MUFFLER ||
              GUY_SOURCE.index == SHIELD ||
              GUY_SOURCE.index == ARMOR ||
              GUY_SOURCE.index == CHAINSAW ||
              GUY_SOURCE.index == SKULLDRILL ||
              GUY_SOURCE.index == TENTACLE ||
              GUY_SOURCE.index == CATTLEPROD ||
              GUY_SOURCE.index == GLUEGUN ||
              GUY_SOURCE.index == STATIK ||
              GUY_SOURCE.index == PHASER ||
              GUY_SOURCE.index == COBRA ||
              GUY_SOURCE.class == ENDKEY ||
              GUY_SOURCE.index == NETSPEED
            )
          ) {
            guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
            my_roll = MY_SOURCE.strength + RANDOM(glob->fate);

            if (my_roll > guy_roll) {
              dmg = RANDOM(MY_SOURCE.strength) * glob->phsrpowr;

              if (dmg >= GUY_PROGRAM.rez) {
                dmg = GUY_PROGRAM.rez;
                prfmsg
                (
                  MSG410,
                  MY_SOURCE.name,
                  GUY_SOURCE.name
                );
                sprintf
                (
                  str,
                  getmsg(MSG411),
                  glob->name,
                  MY_SOURCE.name,
                  GUY_SOURCE.name
                );
                Send(str, g, FALSE);
              } else {
                prfmsg
                (
                  MSG412,
                  MY_SOURCE.name,
                  GUY_SOURCE.name,
                  dmg
                );
                sprintf
                (
                  str,
                  getmsg(MSG413),
                  glob->name,
                  MY_SOURCE.name,
                  GUY_SOURCE.name,
                  dmg
                );
                Send(str, g, FALSE);
              }

              GUY_PROGRAM.rez -= dmg;

              if (GUY_PROGRAM.rez <= 0) {
                Program_Stops(g, p);
              }
            }
          }
        }

        did_something = TRUE;
        dmg = RANDOM(MY_SOURCE.strength) * glob->phsrpowr;
        dmg = Combat_Armor_Check(myp, i, g, dmg);
        Port[g]->rez -= dmg;

        if (dmg) {
          prfmsg(MSG412, MY_SOURCE.name, glob->name2, dmg);

          if (Port[g]->rez <= 0) {
            prfmsg(MSG435, Port[g]->guy.alias);
          }

          My_Outprf(usrnum);
          sprintf
          (
            str,
            getmsg(MSG436),
            glob->name,
            MY_SOURCE.name,
            dmg
          );
          Send(str, g, FALSE);

          if (Port[g]->rez <= 0) {
            strcpy(str, getmsg(MSG437));
            Send(str, g, FALSE);
            myp = g;
            Jack_Out();
            dead = TRUE;
          } else {
            myp = g;
            Update_Location();
          }

          myp = old_port;
        }
      }
    }
  }

  if (!did_something) {
    prfmsg(MSG414, glob->name2);
  }

  EXIT0;
}
/****************************************************************************
    hydra nails it's target player, see if it hurt them
*/
short Hydra_Combat(short s, short i, short g) {
  short   p;
  short   my_roll;
  short   hydra_roll;
  short   dmg;
  short   c;
  short   m;
  short   nailed;
  short   score;
  short   loading = 0;

  if (Visible_Hydra(g, s, i)) {
    strcpy(glob->name, SYS_SOURCE.name);
  } else {
    sprintf(glob->name, "Cloaked %s", SYS_SOURCE.name);
  }

  if (Hydra_Has(CHAINSAW, s, i)) {
    loading = Loading_Time
              (
                s,
                SYS_PROGRAM.m_index,
                SYS_PROGRAM.p_index
              );
    nailed = Defend_Hydra(s, i, CHAINSAW, g);

    if (nailed == -1) {
      EXIT0;
    }

    if (nailed) {
      dmg = Armor_Defend_Hydra(s, i, CHAINSAW, glob->chaindmg, g);

      if (dmg) {
        sprintf
        (
          str,
          getmsg(MSG438),
          SYS_SOURCE.name,
          Programs[CHAINSAW].Name,
          me.alias,
          dmg
        );
        Send(str, s, FALSE);
        sprintf
        (
          str,
          getmsg(MSG417),
          glob->name,
          Programs[CHAINSAW].Name,
          dmg
        );
        Send(str, g, FALSE);
        me.damage += dmg;

        if (me.damage >= glob->dpts) {
          strcpy(str, getmsg(MSG15));
          Send(str, g, FALSE);
          Jack_Out();
          EXIT0;
        } else {
          Update_Location();
        }
      }
    }
  }

  if (Hydra_Has(SKULLDRILL, s, i) && me.score > 0) {
    loading = Loading_Time
              (
                s,
                SYS_PROGRAM.m_index,
                SYS_PROGRAM.p_index
              );
    nailed = Defend_Hydra(s, i, SKULLDRILL, g);

    if (nailed == -1) {
      EXIT0;
    }

    if (nailed) {
      dmg = Armor_Defend_Hydra(s, i, SKULLDRILL, glob->drilldmg, g);

      if (dmg) {
        sprintf
        (
          str,
          getmsg(MSG440),
          SYS_SOURCE.name,
          Programs[SKULLDRILL].Name,
          me.alias,
          dmg
        );
        Send(str, s, FALSE);
        sprintf
        (
          str,
          getmsg(MSSG419),
          glob->name,
          Programs[SKULLDRILL].Name,
          dmg
        );
        Send(str, g, FALSE);
        me.score -= dmg;

        if (me.score < 0) {
          me.score = 0;
        }

        Update_Location();
      }
    }
  }

  if (Hydra_Has(TENTACLE, s, i) && !SYS_PROGRAM.tentacle_timer) {
    loading = Loading_Time
              (
                s,
                SYS_PROGRAM.m_index,
                SYS_PROGRAM.p_index
              );
    nailed = Defend_Hydra(s, i, TENTACLE, g);

    if (nailed == -1) {
      EXIT0;
    }

    if (nailed) {
      sprintf
      (
        str,
        getmsg(MSG442),
        SYS_SOURCE.name,
        Programs[TENTACLE].Name,
        me.alias
      );
      Send(str, s, FALSE);
      sprintf
      (
        str,
        getmsg(MSG421),
        glob->name,
        Programs[TENTACLE].Name
      );
      Send(str, g, FALSE);

      if (MY_PORT->icebolt_time < glob->tentsecs) {
        MY_PORT->icebolt_time = glob->tentsecs;
        SYS_PROGRAM.tentacle_timer = loading + glob->tentsecs;
      }
    }
  }

  if (Hydra_Has(CATTLEPROD, s, i)) {
    loading = Loading_Time
              (
                s,
                SYS_PROGRAM.m_index,
                SYS_PROGRAM.p_index
              );
    nailed = Defend_Hydra(s, i, CATTLEPROD, g);

    if (nailed == -1) {
      EXIT0;
    }

    if (nailed) {
      sprintf
      (
        str,
        getmsg(MSG444),
        SYS_SOURCE.name,
        Programs[CATTLEPROD].Name,
        me.alias
      );
      Send(str, s, FALSE);
      sprintf
      (
        str,
        getmsg(MSG423),
        glob->name,
        Programs[CATTLEPROD].Name
      );
      Send(str, g, FALSE);

      for (c = 0; c != MY_DECK->cpus; ++c) {
        MY_DECK->crash_time[c] = glob->prodsecs;
      }

      strcpy(str, getmsg(MSG63));
      Send(str, g, FALSE);
      Jack_Out();
      EXIT0;
    }
  }

  if (Hydra_Has(GLUEGUN, s, i)) {
    nailed = Defend_Hydra(s, i, GLUEGUN, g);

    if (nailed == -1) {
      EXIT0;
    }

    if (nailed) {
      for (p = 0; p != MY_DECK->cpus; ++p) {
        if
        (
          GUY_PROGRAM.m_index >= 0 &&
          (
            GUY_SOURCE.index == CLOAK ||
            GUY_SOURCE.index == RADAR ||
            GUY_SOURCE.index == MUFFLER ||
            GUY_SOURCE.index == SHIELD ||
            GUY_SOURCE.index == ARMOR ||
            GUY_SOURCE.index == CHAINSAW ||
            GUY_SOURCE.index == SKULLDRILL ||
            GUY_SOURCE.index == TENTACLE ||
            GUY_SOURCE.index == CATTLEPROD ||
            GUY_SOURCE.index == GLUEGUN ||
            GUY_SOURCE.index == STATIK ||
            GUY_SOURCE.index == PHASER ||
            GUY_SOURCE.index == COBRA ||
            GUY_SOURCE.index == NETSPEED
          )
        ) {
          loading = Loading_Time
                    (
                      s,
                      SYS_PROGRAM.m_index,
                      SYS_PROGRAM.p_index
                    );
          my_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
          hydra_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

          if (hydra_roll > my_roll) {
            sprintf
            (
              str,
              getmsg(MSG446),
              SYS_SOURCE.name,
              Programs[GLUEGUN].Name,
              GUY_SOURCE.name
            );
            Send(str, s, FALSE);
            sprintf
            (
              str,
              getmsg(MSG405),
              glob->name,
              Programs[GLUEGUN].Name,
              GUY_SOURCE.name
            );
            Send(str, g, FALSE);
            dmg = SYS_SOURCE.strength * glob->gluesecs;

            if (GUY_PROGRAM.loading < dmg) {
              GUY_PROGRAM.loading = dmg;
            }
          }
        }
      }
    }
  }

  if (Hydra_Has(COBRA, s, i)) {
    nailed = Defend_Hydra(s, i, COBRA, g);

    if (nailed == -1) {
      EXIT0;
    }

    if (nailed) {
      for (p = 0; p != MY_DECK->cpus; ++p) {
        if
        (
          GUY_PROGRAM.m_index >= 0 &&
          (
            GUY_SOURCE.index == CLOAK ||
            GUY_SOURCE.index == RADAR ||
            GUY_SOURCE.index == MUFFLER ||
            GUY_SOURCE.index == SHIELD ||
            GUY_SOURCE.index == ARMOR ||
            GUY_SOURCE.index == CHAINSAW ||
            GUY_SOURCE.index == SKULLDRILL ||
            GUY_SOURCE.index == TENTACLE ||
            GUY_SOURCE.index == CATTLEPROD ||
            GUY_SOURCE.index == GLUEGUN ||
            GUY_SOURCE.index == STATIK ||
            GUY_SOURCE.index == PHASER ||
            GUY_SOURCE.index == COBRA ||
            GUY_SOURCE.class == ENDKEY ||
            GUY_SOURCE.index == NETSPEED
          )
        ) {
          loading = Loading_Time
                    (
                      s,
                      SYS_PROGRAM.m_index,
                      SYS_PROGRAM.p_index
                    );
          my_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
          hydra_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

          if (hydra_roll > my_roll) {
            dmg = RANDOM(SYS_SOURCE.strength);

            if (dmg >= GUY_SOURCE.strength) {
              dmg = GUY_SOURCE.strength;

              if (GUY_SOURCE.value) {
                score = GUY_SOURCE.strength;
              } else {
                score = 0;
              }

              sprintf
              (
                str,
                getmsg(MSG448),
                SYS_SOURCE.name,
                Programs[COBRA].Name,
                GUY_SOURCE.name
              );
              Send(str, s, FALSE);
              sprintf
              (
                str,
                getmsg(MSG407),
                glob->name,
                Programs[COBRA].Name,
                GUY_SOURCE.name
              );
              Send(str, g, FALSE);
            } else {
              sprintf
              (
                str,
                getmsg(MSG450),
                SYS_SOURCE.name,
                Programs[COBRA].Name,
                GUY_SOURCE.name,
                dmg
              );
              Send(str, s, FALSE);
              sprintf
              (
                str,
                getmsg(MSG409),
                glob->name,
                Programs[COBRA].Name,
                GUY_SOURCE.name,
                dmg
              );
              Send(str, g, FALSE);
            }

            GUY_SOURCE.strength -= dmg;
            m = GUY_PROGRAM.m_index;
            c = GUY_PROGRAM.p_index;

            if (GUY_SOURCE.strength <= 0) {
              Program_Stops(g, p);
              Remove_From_Memory(g, m, c);

              if (s < max_guys && score) {
                Scored(s, score, 1);
              }
            }
          }
        }
      }
    }
  }

  if (Hydra_Has(PHASER, s, i)) {
    loading = Loading_Time
              (
                s,
                SYS_PROGRAM.m_index,
                SYS_PROGRAM.p_index
              );
    nailed = Defend_Hydra(s, i, PHASER, g);

    if (nailed == -1) {
      EXIT0;
    }

    if (nailed) {
      for (p = 0; p != MY_DECK->cpus; ++p) {
        if
        (
          GUY_PROGRAM.m_index >= 0 &&
          (
            GUY_SOURCE.index == CLOAK ||
            GUY_SOURCE.index == RADAR ||
            GUY_SOURCE.index == MUFFLER ||
            GUY_SOURCE.index == SHIELD ||
            GUY_SOURCE.index == ARMOR ||
            GUY_SOURCE.index == CHAINSAW ||
            GUY_SOURCE.index == SKULLDRILL ||
            GUY_SOURCE.index == TENTACLE ||
            GUY_SOURCE.index == CATTLEPROD ||
            GUY_SOURCE.index == GLUEGUN ||
            GUY_SOURCE.index == STATIK ||
            GUY_SOURCE.index == PHASER ||
            GUY_SOURCE.index == COBRA ||
            GUY_SOURCE.class == ENDKEY ||
            GUY_SOURCE.index == NETSPEED
          )
        ) {
          hydra_roll = SYS_SOURCE.strength + RANDOM(glob->fate);
          my_roll = GUY_SOURCE.strength + RANDOM(glob->fate);

          if (hydra_roll > my_roll) {
            dmg = RANDOM(SYS_SOURCE.strength) * glob->phsrpowr;

            if (dmg >= GUY_PROGRAM.rez) {
              dmg = GUY_PROGRAM.rez;
              sprintf
              (
                str,
                getmsg(MSG452),
                SYS_SOURCE.name,
                Programs[PHASER].Name,
                GUY_SOURCE.name
              );
              Send(str, s, FALSE);
              sprintf
              (
                str,
                getmsg(MSG411),
                glob->name,
                Programs[PHASER].Name,
                GUY_SOURCE.name
              );
              Send(str, g, FALSE);
            } else {
              sprintf
              (
                str,
                getmsg(MSG454),
                SYS_SOURCE.name,
                Programs[PHASER].Name,
                GUY_SOURCE.name,
                dmg
              );
              Send(str, s, FALSE);
              sprintf
              (
                str,
                getmsg(MSG413),
                glob->name,
                Programs[PHASER].Name,
                GUY_SOURCE.name,
                dmg
              );
              Send(str, g, FALSE);
            }

            GUY_PROGRAM.rez -= dmg;

            if (GUY_PROGRAM.rez <= 0) {
              Program_Stops(g, p);
            }
          }
        }
      }

      dmg = RANDOM(SYS_SOURCE.strength) * glob->phsrpowr;
      dmg = Armor_Defend_Hydra(s, i, PHASER, dmg, g);
      MY_PORT->rez -= dmg;

      if (dmg) {
        sprintf
        (
          str,
          getmsg(MSG454),
          SYS_SOURCE.name,
          Programs[PHASER].Name,
          me.alias,
          dmg
        );
        Send(str, s, FALSE);

        if (MY_PORT->rez <= 0) {
          sprintf(str, getmsg(MSG457), me.alias);
          Send(str, s, FALSE);
        }

        sprintf
        (
          str,
          getmsg(MSG436),
          glob->name,
          Programs[PHASER].Name,
          dmg
        );
        Send(str, g, FALSE);

        if (MY_PORT->rez <= 0) {
          strcpy(str, getmsg(MSG437));
          Send(str, g, FALSE);
          Jack_Out();
          EXIT0;
        } else {
          Update_Location();
        }
      }
    }
  }

  SYS_PROGRAM.loading = loading;
  EXIT0;
}
/****************************************************************************
    see if I can defend an attack by a hydra
*/
short Defend_Hydra(short s, short i, short n, short g) {
  short   p;
  short   m;
  short   c;
  short   strength;
  short   hydra_roll;
  short   guy_roll;
  short   secs;

  for (p = 0; p != Sys[g]->cpus; ++p) {
    if
    (
      !GUY_PROGRAM.loading &&
      GUY_PROGRAM.m_index >= 0 &&
      (GUY_SOURCE.index == STATIK || GUY_SOURCE.index == ARMOR)
    ) {
      guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
      hydra_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

      if (GUY_SOURCE.index == ARMOR) {
        if (guy_roll > hydra_roll) {
          sprintf
          (
            str,
            getmsg(MSG459),
            SYS_SOURCE.name,
            Programs[n].Name,
            GUY_SOURCE.name
          );
          Send(str, s, FALSE);
          sprintf
          (
            str,
            getmsg(MSG460),
            glob->name,
            Programs[n].Name,
            GUY_SOURCE.name
          );
          Send(str, g, FALSE);
          SYS_PROGRAM.loading = GUY_SOURCE.strength;
          return (FALSE);
        }
      } else if (GUY_SOURCE.index == STATIK) {
        if (guy_roll > hydra_roll) {
          sprintf
          (
            str,
            getmsg(MSG461),
            SYS_SOURCE.name,
            GUY_SOURCE.name
          );
          Send(str, s, FALSE);
          sprintf
          (
            str,
            getmsg(MSG462),
            glob->name,
            GUY_SOURCE.name
          );
          Send(str, g, FALSE);
          secs = GUY_SOURCE.strength * glob->statsecs;

          if (SYS_PROGRAM.loading < secs) {
            SYS_PROGRAM.loading = secs;
          }

          return (FALSE);
        }
      }
    }
  }

  for (p = 0; p != Sys[g]->cpus; ++p) {
    if
    (
      !GUY_PROGRAM.loading &&
      GUY_PROGRAM.m_index >= 0 &&
      GUY_SOURCE.index == SHIELD
    ) {
      hydra_roll = SYS_SOURCE.strength + RANDOM(glob->fate);
      guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);

      if (guy_roll > hydra_roll) {
        sprintf
        (
          str,
          getmsg(MSG459),
          SYS_SOURCE.name,
          Programs[n].Name,
          GUY_SOURCE.name
        );
        Send(str, s, FALSE);
        sprintf
        (
          str,
          getmsg(MSG460),
          glob->name,
          Programs[n].Name,
          GUY_SOURCE.name
        );
        Send(str, g, FALSE);
        Program_Stops(s, i);
        return (-1);
      }

      --GUY_SOURCE.strength;

      if (hydra_roll > guy_roll * 2) {
        --GUY_SOURCE.strength;

        if (RANDOM(2) == 1) {
          --GUY_SOURCE.strength;
        }
      }

      strength = GUY_SOURCE.strength;
      m = GUY_PROGRAM.m_index;
      c = GUY_PROGRAM.p_index;
      sprintf
      (
        str,
        getmsg(MYY40),
        GUY_SOURCE.name,
        glob->name,
        Programs[n].Name,
        GUY_SOURCE.name,
        GUY_SOURCE.strength
      );
      Send(str, g, FALSE);
      sprintf
      (
        str,
        getmsg(MXX40),
        GUY_SOURCE.name,
        SYS_SOURCE.name,
        GUY_SOURCE.name,
        GUY_SOURCE.strength
      );
      Send(str, s, FALSE);

      if (strength <= 0) {
        sprintf(str, getmsg(MSG894), GUY_SOURCE.name);
        Send(str, g, FALSE);
        Program_Stops(g, p);
        Remove_From_Memory(g, m, c);
      }

      Program_Stops(s, i);
      return (-1);
    }
  }

  return (TRUE);
}
/****************************************************************************
    see if armor saved me some hydra damage
*/
short EXPORT Armor_Defend_Hydra(short s, short i, short n, short dmg, short g) {
  short   p;
  short   hydra_roll;
  short   guy_roll;

  for (p = 0; p != Sys[g]->cpus && dmg; ++p) {
    if
    (
      !GUY_PROGRAM.loading &&
      GUY_PROGRAM.m_index >= 0 &&
      GUY_SOURCE.index == ARMOR
    ) {
      guy_roll = GUY_SOURCE.strength + RANDOM(glob->fate);
      hydra_roll = SYS_SOURCE.strength + RANDOM(glob->fate);

      if (guy_roll > hydra_roll) {
        --dmg;

        if (dmg < 0) {
          dmg = 0;
        }

        sprintf
        (
          str,
          getmsg(MSG463),
          GUY_SOURCE.name,
          glob->name,
          Programs[n].Name
        );
        Send(str, g, FALSE);
        sprintf
        (
          str,
          getmsg(MSG464),
          SYS_SOURCE.name,
          Programs[n].Name,
          GUY_SOURCE.name
        );
        Send(str, s, FALSE);
      }
    }
  }

  return (dmg);
}
