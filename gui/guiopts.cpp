// Copyright 2001, 2002, 2012, 2018, 2023-2024 by Jon Dart. All Rights Reserved

#include "stdafx.h"
#include "guiopts.h"
#include <afxwin.h>

static const char *src_names[] =
{ "Time", "Incremental", "Tournament", "Depth", 0};

void GuiOptions::load_src_limits()
{

   CString opt_str = app->GetProfileString("Search Limits","Search Type",
      "Time");

   const char **p = src_names;
   int type = 0;
   while (*p) {
      if (opt_str.Compare(*p)==0)
         break;
      ++type; ++p;
   }
   if (type > 3 || type < 0) type = 0;
   src_opts.search_type = type;

   switch (type) {
      case 0:
         opt_str = app->GetProfileString("Search Limits","Seconds","5");
         src_opts.var1 = atoi((LPCSTR)opt_str);
         break;
      case 1:
         opt_str = app->GetProfileString("Search Limits","Minutes","2");
         src_opts.var1 = atoi((LPCSTR)opt_str);
         opt_str = app->GetProfileString("Search Limits","Increment","12");
         src_opts.var2 = atoi((LPCSTR)opt_str);
         break;
      case 2:
         opt_str = app->GetProfileString("Search Limits","Moves","60");
         src_opts.var1 = atoi((LPCSTR)opt_str);
         opt_str = app->GetProfileString("Search Limits","Minutes","60");
         src_opts.var2 = atoi((LPCSTR)opt_str);
         break;
      case 3:
         opt_str = app->GetProfileString("Search Limits","Ply","5");
         src_opts.var1 = atoi((LPCSTR)opt_str);
         break;
      default:
         break;
   }
}

void GuiOptions::store_src_limits()
{
   char msg[40];
   app->WriteProfileString("Search Limits","Search Type",src_names[src_opts.search_type]);
   switch (src_opts.search_type) {
      case 0:                                     /* fixed time*/
         wsprintf(msg,"%ld",src_opts.var1);
         app->WriteProfileString("Search Limits","Seconds",msg);
         break;
      case 1:                                     /* incremental */
         wsprintf(msg,"%d", src_opts.var1);
         app->WriteProfileString("Search Limits","Minutes",msg);
         wsprintf(msg,"%d", src_opts.var2);
         app->WriteProfileString("Search Limits","Increment",msg);
         break;
      case 2:                                     /* tournament */
         wsprintf(msg,"%d", src_opts.var1);
         app->WriteProfileString("Search Limits","Moves",msg);
         wsprintf(msg,"%d", src_opts.var2);
         app->WriteProfileString("Search Limits","Minutes",msg);
         break;
      case 3:                                     /* fixed depth */
         wsprintf(msg,"%ld",src_opts.var1);
         app->WriteProfileString("Search Limits","Ply",msg);
         break;

   }
}

static inline CString boolString(BOOL x)
{
   return (x ? "True" : "False");
}

void GuiOptions::save()
{
   store_src_limits();
   app->WriteProfileInt("Location","x",appX);
   app->WriteProfileInt("Location", "y",appY);
   app->WriteProfileInt("Window State", "Flags", flags);
   app->WriteProfileInt("Window State", "ShowCmd", showCmd);
   app->WriteProfileString("Appearance","Force Mono",appr.forceMono ?
      "True" : "False");
   switch (appr.board_size) {
      case Small:
         app->WriteProfileString("Appearance","Board Size","Small"); break;
      case Medium:
         app->WriteProfileString("Appearance","Board Size","Medium"); break;
      case Large:
         app->WriteProfileString("Appearance","Board Size","Large"); break;
      case XLarge:
         app->WriteProfileString("Appearance","Board Size","XLarge"); break;
      case XXLarge:
         app->WriteProfileString("Appearance","Board Size","XXLarge"); break;
   }
   app->WriteProfileString("Appearance","Font",appr.pieceFontName.c_str());
   app->WriteProfileString("Appearance","Show Coordinates",boolString(appr.show_coordinates));
   app->WriteProfileInt("Appearance","Light Square Color",appr.lightSquareColor);
   app->WriteProfileInt("Appearance","Dark Square Color",appr.darkSquareColor);
   app->WriteProfileString("Preferences","Beep After Move",boolString(gprefs.beep_after_move));
   app->WriteProfileString("Preferences","Beep On Error",boolString(gprefs.beep_on_error));
   app->WriteProfileString("Preferences","Can Resign",boolString(gprefs.can_resign));
   app->WriteProfileString("Preferences","Think When Idle",boolString(gprefs.think_when_idle));
   app->WriteProfileString("Preferences","Position Learning",boolString(gprefs.position_learning));
   app->WriteProfileInt("Preferences","Hash Table Size",gprefs.hash_table_size);
   app->WriteProfileString("Preferences","Auto Size Hash Table",boolString(gprefs.auto_size_hash_table));
   app->WriteProfileInt("Preferences","Cores",gprefs.cores);
   app->WriteProfileInt("Preferences","Strength",gprefs.strength);
   app->WriteProfileString("Preferences","TbPath",gprefs.tbPath);
   app->WriteProfileInt("Preferences","Book Variety",gprefs.bookVariety);
}


int calc_hash_size()
{
   // Size the hash table according to how much memory we have.
   static int sizes[12]= {
      997, 1997, 3001, 4003, 4999, 6007, 6997, 8003,
      10007, 12007, 15013, 18103
   };

   DWORD memSize = GlobalCompact(0);
   int i;
   for (i = 0; i < 11; i++)
      if (16*((long)sizes[i]) > memSize/16)
         break;
   return 32L*sizes[i]/1000;
}


GuiOptions::GuiOptions(CWinApp *guiApp) :
app(guiApp)
{

   CString opt_str;

   load_src_limits();
   opt_str = app->GetProfileString("Preferences","Beep After Move","True");
   gprefs.beep_after_move = opt_str == "True";
   opt_str = app->GetProfileString("Preferences","Beep On Error","True");
   gprefs.beep_on_error = opt_str == "True";
   opt_str = app->GetProfileString("Preferences","Can Resign","True");
   gprefs.can_resign = opt_str == "True";
   opt_str = app->GetProfileString("Preferences","Think When Idle","True");
   gprefs.think_when_idle = opt_str == "True";
   opt_str = app->GetProfileString("Preferences","Position Learning","False");
   gprefs.position_learning = opt_str == "True";
   gprefs.hash_table_size = app->GetProfileInt("Preferences","Hash Table Size",32);
   opt_str = app->GetProfileString("Preferences","Auto Size Hash Table","True");
   gprefs.auto_size_hash_table = opt_str == "True";
   if (gprefs.auto_size_hash_table) {
      gprefs.hash_table_size = calc_hash_size();
   }
   gprefs.cores = app->GetProfileInt("Preferences","Cores",1);
   gprefs.strength = app->GetProfileInt("Preferences","Strength",100);
   gprefs.tbPath = app->GetProfileString("Preferences","TbPath","");
   gprefs.bookVariety = app->GetProfileInt("Preferences","Book Variety",50);

   opt_str = app->GetProfileString("Appearance","Force Mono","False");
   appr.forceMono = (opt_str == "True") ? TRUE : FALSE;

   opt_str = app->GetProfileString("Appearance","Show Coordinates","True");
   appr.show_coordinates = opt_str == "True";
   appr.pieceFontName = app->GetProfileString("Appearance","Font","Chess Berlin");

   appX = app->GetProfileInt("Location","x",50);
   appY = app->GetProfileInt("Location","y",50);
   flags = app->GetProfileInt("Window State", "Flags", 0);
   showCmd = app->GetProfileInt("Window State", "ShowCmd", 1 /*SW_SHOW_NORMAL*/);

   opt_str = app->GetProfileString("Appearance","Board Size","Small");
   appr.board_size = Small;
   if (opt_str == "Medium")
      appr.board_size = Medium;
   else if (opt_str == "Large")
      appr.board_size = Large;
   else if (opt_str == "XLarge")
      appr.board_size = XLarge;
   else if (opt_str == "XXLarge")
      appr.board_size = XXLarge;
   appr.lightSquareColor  = (COLORREF)app->GetProfileInt("Appearance","Light Square Color",0x80ffff);
   appr.darkSquareColor = (COLORREF)app->GetProfileInt("Appearance","Dark Square Color",0x408040);
}


void GuiOptions::set_app_location(int x, int y)
{
   appX = x; appY = y;
}


void GuiOptions::get_app_location(int &x, int &y)
{
   x = appX; y = appY;
}
