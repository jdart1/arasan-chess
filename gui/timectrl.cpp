// Copyright 1994 by Jon Dart.

#include "timectrl.h"

#include <stdio.h>

Time_Control::Time_Control()
{
   Search_Limit lim;
   lim.max_ply = 2;
   limits = lim;
   srctype = Fixed_Ply;
}


Time_Control::Time_Control(Search_Type typ, Search_Limit lim)
{
   limits = lim;
   srctype = typ;
}


const char *Time_Control::Image() const
{
   static char msg[40];
   switch (get_search_type()) {
      case 0:                                     /* Fixed_Ply */
         sprintf(msg,"%d ply",limits.max_ply);
         break;
      case 1:                                     /* Time_Limit */
      case 2:                                     /* Time_Target */
         sprintf(msg,"%ld.%0d seconds/move",
            limits.centiseconds/100,(int)(limits.centiseconds % 100));
         break;
      case 3:                                     /* Game */
         sprintf(msg,"game in %d minutes",
            limits.limit.minutes);
         break;
      case 4:                                     /* Tournament */
         sprintf(msg,"%d moves/%d minutes",
            limits.limit.moves,limits.limit.minutes);
      case 5:                                     /* Incremental */
         sprintf(msg,"%d minutes/%d second increment",
            limits.limit.minutes,limits.limit.increment);
      default:
         break;
   }
   return msg;
}


Time_Info::Time_Info()
:Time_Control(),
period(0),
last_time_control(0),
bonus(0)
{
}


Time_Info::Time_Info(Search_Type t, Search_Limit lim)
:Time_Control(t,lim),
period(0),
last_time_control(0),
bonus(0)
{
}
