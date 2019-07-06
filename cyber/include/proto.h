extern void  Display_Background(short x, short y);
extern void  Cashier_Lock(short s);
extern short Report_Add_System(short net_x, short net_y, short x, short y);
extern short Perm_Report(void);
extern void  My_Outprf(short user_num);
extern short Previous_Winners(void);
extern short Tourney_Check(void);
extern short Tourney_Edit(void);
extern short Phase_Change(void);
extern short Reset_Game(void);
extern short Phase_Check(short option);
extern void  Reset_Tournament(void);
extern short Tourney_Services(void);
extern short Init_Tourney(void);
extern short Backup_Services(void);
extern short Sentry_Check(short s, short i);
extern short Cyberia(void);
extern short Read_Gang_Mail(void);
extern short Post_Gang(void);
extern short New_Gang_Mail(void);
extern short Gang_Scoring(void);
extern short Dead_Gangster(short gang_num, short index, short rank);
extern short File_Server_Check(short s);
extern short Upload_Gang_Data(short s, short g);
extern short Download_Gang_Data(short s);
extern short Cloak_Entropy(short s, short strength);
extern short Gang_Lock_Find(short city);
extern short Gang_Lock_List(void);
extern short Gang_Server(void);
extern void  Gang_Ledger(void);
extern short Gang_Bank(void);
extern short Init_Gang_Guy(short g);
extern short Set_Rank(void);
extern short Read_Gang(short g);
extern short Write_Gang(short g);
extern void  Gang_Edit_Done(MSHORT save);
extern void  Tourney_Edit_Done(MSHORT save);
extern void  Force_Context(void);
extern short Edit_Gang_Ranks(void);
extern short New_Leader(void);
extern short Rename_Gang(void);
extern short Kick_Out(void);
extern short Accept(void);
extern short Invite(void);
extern short Read_Custom_Cities(short world_x, short world_y);
extern GBOOL cyber_input(void);
extern GBOOL cyber_login(void);
extern void  cyber_hup(void);
extern void  cyber_clean(void);
extern void  cyber_delete(char *account);
extern void  cyber_shutdown(void);
extern GBOOL cyber_asynch(void);
extern void  init__muicyber(void);
extern void  cyber_status(void);
extern void  Get_Config(void);
extern short Unique_Check(void);
extern char  Stroke_Monitor(int chan, int c);
extern void  Cyber_Server(void);
extern void  Display_Edit_City(void);
extern void  Read_System_Names(void);
extern short Next_System_Name(void);
extern short Gang_Members(void);
extern short Resign(void);
extern short List_Gangs(void);
extern short Net_Move_Check(short spaces, short command);
extern short Gang_Services1(void);
extern short Gang_Services2(void);
extern short Gang_Menu(void);
extern short New_Mission(void);
extern short Current_Mission(void);
extern short Use_Hydra(short s);
extern short Aliens(void);
extern short Who(void);
extern short Endgame(void);
extern void  Init_Valhalla(void);
extern short New_FreeNet(void);
extern short Read_FreeNet(void);
extern short Put_FreeNet_Message(struct freenet_struct *msg);
extern short Put_Gang_Message(struct freenet_struct *msg, short g);
extern short Post_FreeNet(void);
extern short Check_Mail(void);
extern void  New_Mail_Check(void);
extern void  Erase_Mail(short index);
extern short Post_Private(void);
extern short Write_Email(void);
extern short Mail(void);
extern short Memory_Unlock(void);
extern short Jump_To_System(short s, short i, short city_x, short city_y);
extern void  Create_Brief(void);
extern void  Set_Brief
(
  struct human_struct *human,
  struct brief_human_struct *brief
);
extern void  Display_My_Rank(void);
extern short Outlaw(short option);
extern void  Clear_Old_Characters(void);
extern short Crash_Check(short g, short *score);
extern short Traced(short g);
extern void  Reset_Location_Values(void);
extern void  Free_BBS_Stuff(struct system_struct *sys);
extern short Read_Human(short index, struct human_struct *someone);
extern short Find_Random_User(char *bank_code, char *internet_code);
extern short Find_Random_Gang(char *bank_code, char *server_code);
extern short Knight_Target(short s, short *sys_num, short *cpu_num);
extern short Perm_Memory(struct system_struct *sys);
extern short Perm_FSE(void);
extern short Perm_Doc_Add_FSE(void);
extern void  Perm_Doc_Add_Done(MSHORT save);
extern short Perm_Prog_Add_FSE(void);
extern void  Perm_Prog_Add_Done(MSHORT save);
extern short Perm_Doc_Edit_FSE(void);
extern void  Perm_Doc_Edit_Done(MSHORT save);
extern short Perm_Prog_Edit_FSE(void);
extern void  Perm_Prog_Edit_Done(MSHORT save);
extern void  Server_Update(void);
extern short Design_Perm(void);
extern void  Display_Doc(struct memory_struct *file);
extern short Terminate_String(char *msg);
extern void  Help_Dump(void);
extern int   Download_Handler(int tshcod);
extern short Scored(short g, short score, short gang_score);
extern short Close_System_Check(short s, short option);
extern short System_Track(short s, short i, short x, short y);
extern short Add_Phone(struct system_struct *sys);
extern short Improve_Program(short option);
extern short Installable_Programs(short option);
extern short Memory_Zip(void);
extern void  Probed_Program(short g, short p);
extern void  Probed_Guy(short g);
extern short Probed(void);
extern short Ran_Program(short m, short p, short *cmd_matches, char *next);
extern short No_Local_Internet(void);
extern short Program_Help(char *word);
extern short Internet_Billing(void);
extern short Street_Jack_In(void);
extern void  Top_Nine(short option);
extern short List_Programs(short option);
extern short List_Documents(short option);
extern void  City_Who(void);
extern void  System_Who(void);
extern void  Erase_Memory(short s, short m);
extern short Memory_Services(void);
extern void  Help_Icons(void);
extern short Local_Gabber(short g);
extern short Already_Detected(short s, short r, short g, short p);
extern short Guy_Already_Detected(short s, short r, short g);
extern short Dayfile(void);
extern short Day_Out(void);
extern long  LRAND(long n);
extern void  Destroy_My_Deck(short confiscated);
extern void  Cop_Scan(void);
extern short Cop_Actions(void);
extern void  Init_Cops(void);
extern short Select_Con_Target(short s, short i);
extern short Con_Actions(short s);
extern void  Init_Cons(void);
extern void  Clear_Display_Window(void);
extern void  Service_Display(void);
extern short Offline_Services(void);
extern short Fence_Check(short option);
extern void  Help_Programs(void);
extern short Fence(short s, short i);
extern void  Bank_Records(void);
extern void  Internet_Records(void);
extern void  Add_Bank_Record
(
  struct human_struct *a_human,
  long   amount,
  short  net_x,
  short  net_y,
  char   *alias,
  char   *action
);
extern void  Add_Internet_Record
(
  struct human_struct *a_human,
  long   amount,
  short  net_x,
  short  net_y,
  char   *alias,
  char   *action
);
extern short Rent_Flat(void);
extern short CPU_Bought(void);
extern short Rent(void);
extern short Relocate(short option);
extern short Display_Cities(void);
extern void  Fix_Alias(char *name);
extern short System_Is_Running(short s, short p);
extern short Use_Program(short s, short p);
extern void  Create_New(char *path);
extern short Write_City(short net_x, short net_y, struct city_struct *some_city);
extern short Write_Custom_City
(
  short  net_x,
  short  net_y,
  struct city_struct *some_city
);
extern short Read_City(short net_x, short net_y, struct city_struct *some_city);
extern short Read_Custom_City
(
  short net_x,
  short net_y,
  struct city_struct *some_city
);
extern void  Display_A_System(struct system_struct *deck);
extern short Design_Deck(void);
extern char  *Brief_Userid(char *userid);
extern short Sys_Actions(short s);
extern short Change_Alias(void);
extern void  Gab_Message(char *msg_txt);
extern void  Idle_Message(char *msg_txt);
extern short Local_Gablink(short g);
extern short Armor_Defend_Hydra(short s, short i, short n, short dmg, short g);
extern short Defend_Hydra(short s, short i, short n, short g);
extern short Hydra_Combat(short s, short i, short g);
extern short Loading_Time(short s, short m_index, short p_index);
extern short Ran_Into_Guy(short g);
extern short Ran_Into_Program(short g, short p);
extern void  System_Program_Message(char *msg_txt, short sys_num, short s, short i);
extern void  City_Program_Message(char *msg_txt, short x, short y, short s, short i);
extern short Hydra_Has(short p, short s, short i);
extern short Visible_Hydra(short g, short s, short i);
extern short Hydra_Select(void);
extern short Configure_Program(void);
extern short Run_Tactical(char *which);
extern short Defense_Check(short s, short i, short g);
extern short Combat_Defense_Check(short s, short i, short g);
extern short Hydra_Defense_Check(short s, short i);
extern short Hydra_Armor_Check(short s, short i, short dmg);
extern short Armor_Check(short s, short i, short g, short dmg);
extern short Combat_Armor_Check(short s, short i, short g, short dmg);
extern short Seeker_Leaving_System(short s, short i);
extern short Seek_In_Local_System(short s, short i);
extern short Seek_In_Remote_City(short s, short i);
extern short Cop_Seek_Remote_City(short s, short i);
extern short Hospital(void);
extern short His_Rank(long score, short won);
extern short Virus(short s, short i);
extern short Mason(short s, short i);
extern short Snout(short s, short i);
extern short Patriot(short s, short i);
extern short Anti_Virus(short s, short i);
extern void  Stop_All_Programs(short s);
extern void  Boot_Users_From(short s);
extern void  Stop_Programs_In(short g);
extern void  Dead_Timers(void);
extern void  Sort_Programs(void);
extern short Seeker(short s, short i);
extern short Program_Seeker(short s, short i);
extern short Seek_Remote(short s, short i);
extern short Cop_Seek_Remote(short s, short i);
extern short Find_Guy(char *name);
extern short Find_Target_Program(char *name, short *sys_index, short *cpu);
extern void  Cloak_Guy(short g);
extern void  Cloak_Hydra(short s, short i);
extern void  Reveal_Guy(short g);
extern short Visible(short s, short g);
extern short Add_Memory(struct system_struct *sys, short i);
extern short Add_CPU(struct system_struct *sys, short i);
extern void  Remove_CPU(struct system_struct *sys, short i);
extern void  Remove_Memory(struct system_struct *sys, short mu_num);
extern void  System_Message(char *msg_txt, short s);
extern void  Cloakable_System_Message(char *msg_txt, short s, short p);
extern void  City_Message(char *msg_txt, short x, short y, short p);
extern void  Global_Message(char *msg_txt, short p);
extern void  Random_Space(short *x, short *y);
extern void  Write_Deck(short index, struct system_struct *deck);
extern void  Find_Enemies(short s, short i);
extern short Guardian(short s, short i);
extern short Trace(short s, short i);
extern short Radar(short s, short i);
extern short Find_System_Route
(
  short s_num,
  short from_x,
  short from_y,
  short to_x,
  short to_y,
  short runner_s,
  short *next,
  short index,
  short strength
);
extern short Find_City_Route
(
  struct city_struct *city,
  short  net_x,
  short  net_y,
  short  from_x,
  short  from_y,
  short  to_x,
  short  to_y,
  short  *next
);
extern short Sys_Track
(
  short  s,
  struct cpu_struct *runner,
  short  target_x,
  short  target_y,
  short  runner_s,
  short  *stuck
);
extern short City_Track
(
  struct cpu_struct *runner,
  short  target_x,
  short  target_y,
  short  s
);
extern char  *Runner_System_Name(short s, short i);
extern short Runner_System_Number(short s, short i);
extern short RANDOM(short n);
extern short Run_System_Program(short s, short p_num);
extern short Program_Stops(short s, short i);
extern struct cpu_struct *Find_Program
(
  short net_x,
  short net_y,
  short city_x,
  short city_y,
  short sys_x,
  short sys_y,
  short *sys_num,
  short *cpu_num
);
extern short Stop_Program(short option);
extern short Buying(void);
extern short Selling(void);
extern void  Buy_Hardware(void);
extern void  Sell_Hardware(void);
extern short Create_Program(void);
extern short Hammering(short i, short s);
extern void  Hammered(short i, short s);
extern short Cracking(short i, short s);
extern short Cracked(short i, short s);
extern short Gate_Crashing(short i, short s);
extern short Gate_Crashed(short i, short s);
extern short Crashing(short i, short s);
extern short Crashed(short i, short s);
extern short Torping(short i, short s);
extern short Torped(short i, short s);
extern short Locking(short i, short s);
extern void  Locked(short i, short s);
extern short Unlocking(short i, short s);
extern short Unlocked(short i, short s);
extern void  CPU_Status(short s);
extern short Find_System_Space(char *where, short *x, short *y, short spaces);
extern short Find_City_Space(char *where, short *x, short *y, short spaces);
extern void  Update_Others(short view, short nx, short ny, short cx, short cy);
extern void  Quick_Update
(
  short view,
  short nx, short ny,
  short cx, short cy,
  short old_x, short old_y,
  short new_x, short new_y
);
extern short Load_Program(void);
extern short Load_System_Program(short s, short i, short m_index, short p_index);
extern short Inside(short g);
extern short Program_Inside(short s, short i);
extern void  Open_The_Eye(void);
extern short Update_Human(struct human_struct *someone);
extern short Find_Internet_User
(
  struct human_struct *someone,
  char *internet_code
);
extern short Find_Gang_Server
(
  struct human_struct *someone,
  char *internet_code
);
extern short Find_Bank_User
(
  struct human_struct *someone,
  char *bank_code
);
extern short Find_Alias
(
  struct human_struct *someone,
  char *alias
);
extern short Write_User(struct human_struct *someone);
extern short Find_Internet_User(struct human_struct *a_human, char *id);
extern short Crash_LD_Link(void);
extern void  Redraw(void);
extern void  Player_Edit_Done(MSHORT save);
extern void  Perm_Edit_Done(MSHORT save);
extern void  Update_Location(void);
extern void  Main_Prompt(void);
extern void  Prompt(char *prompt, short p);
extern short Dump_File(char *file);
extern short Parse_Input(void);
extern void  Init_Data_Files(void);
extern void  NOTIFY(char *txt);
extern void  System_View(short quick_x, short quick_y);
extern void  World_View(void);
extern void  City_View(short quick_x, short quick_y);
extern void  Put(short y, short x, char *str, short which);
extern void  Payday_Check(short exists);
extern void  Write_My_Rec(short option);
extern short Write_Gang_Rec(short option);
extern short Read_My_Rec(struct human_struct *my_record);
extern short Newguy(short option);
extern void  Init_Cities(void);
extern void  Init_Custom_Cities(void);
extern short Init_World(void);
extern short Sys_Timers(short s);
extern void  Port_Check(void);
extern short System_Move(void);
extern short City_Move(void);
extern short Net_Move(short spaces, short move_to_ld);
extern short Moving(void);
extern void  Move_To_System(short net_x, short net_y, short city_x, short city_y);
extern short System_Move_Check(short net_x, short net_y, short city_x, short city_y);
extern short Move_Me_To
(
  short net_x,
  short net_y,
  short city_x,
  short city_y,
  short jackout,
  short back
);
extern void  Leaving_System(short jacking_out);
extern short File_Utility(short p);
extern void  Print_Direction(short dir);
extern short Find_Memory(char *where, short *s, short *m);
extern short Find_Chip(char *where, short *m);
extern short Memory_Summary(void);
extern short Backup_Summary(void);
extern short Remove_From_Memory(short s, short m, short index);
extern short Remove_From_Backup(short m, short index);
extern short Add_To_Memory
(
  struct memory_struct *memory,
  struct memory_struct *file
);
extern short Memory_Erase(void);
extern short Memory_Read(void);
extern short Memory_Move(void);
extern short Memory_Store(short option);
extern short Memory_Retrieve(short option);
extern short Memory_Contents(struct memory_struct *memory, short option);
extern short File_Size(struct memory_struct *file);
extern void  Configure_Con_Hydra(struct memory_struct *hydra);
extern void  Configure_Cop_Hydra(struct memory_struct *hydra);
extern short Memory_Usage(struct memory_struct *memory);
extern short Num_Files(struct memory_struct *memory);
extern void  Set_System_Path(short sys_x, short sys_y, short net_x, short net_y);
extern void  Set_Valhalla_Path(short sys_x, short sys_y);
extern void  Set_Custom_Path(short sys_x, short sys_y, short net_x, short net_y);
extern void  Set_Perm_Path(short sys_x, short sys_y, short net_x, short net_y);
extern void  Set_Custom_Perm_Path(short sys_x, short sys_y, short net_x, short net_y);
extern void  Init_Net_Index(void);
extern void  Init_System(struct system_struct *sys, short sys_type);
extern short Jack_Out(void);
extern short Jack_In(void);
extern short Jacking_In(void);
extern short Back_Up(void);
extern void  Sign_Me_Off(void);
extern short Sign_Me_On(void);
extern short Send(char *msg, short p, short beep);
extern void  Help_Commands(void);
extern void  Command_Sort(short *sorted_index);
extern void  Program_Sort(short *sorted_index);
extern short Help(void);
extern short Greet_User(void);
extern void  Clear_Net_Display(short option);
extern short Edit_Player(void);
extern short Edit_World(void);
extern short Edit(void);
extern short Look(char *option);
extern short List_My_Perms(void);
extern short Where(short option);
extern void  List_My_Options(void);
extern void  Display_Time(void);
extern void  Finances(void);
extern void  Display_Deck(void);
extern void  Set_Perms(void);
extern short Communicate(short option);
extern void  Do_Command(void);
extern short Set_Options(void);
