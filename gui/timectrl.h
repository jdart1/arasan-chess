// Copyright 1994 by Jon Dart.  All Rights Reserved.

#ifndef _Time_Control_H
#define _Time_Control_H

#include <time.h>

enum Search_Type
{
   Fixed_Ply, Time_Limit, Time_Target, Game, Tournament,
   Incremental, None
};

struct Time_Limits
{
   union
   {
      unsigned moves;
      unsigned increment;
   };
   unsigned minutes;
};

union Search_Limit
{
   unsigned max_ply;                              // for Fixed_Ply search
   unsigned long centiseconds;                    // for Time_Limit or Time_Target searches
   Time_Limits limit;                             // for Tournament and Game searches
};

enum Control { First, Second };

class Time_Control
{
   public:

      Time_Control();

      Time_Control(Search_Type, Search_Limit);

      Search_Limit get_search_limit() const
      {
         return limits;
      }

      void set_search_limit( Search_Limit lim ) {
         limits = lim;
      }

      Search_Type get_search_type() const
      {
         return srctype;
      }

      void set_search_type( Search_Type typ ) {
         srctype = typ;
      }

      const char *Image() const;

   private:
      Search_Limit limits;
      Search_Type srctype;
};

class Time_Info : public Time_Control
{
   public:
      Time_Info();

      Time_Info(Search_Type, Search_Limit);

      int get_period() const
      {
         return period;
      }

      void set_period(const int n) {
         period = n;
      }

      int get_last_time_control() const
      {
         return last_time_control;
      }

      void set_last_time_control( const int n ) {
         last_time_control = n;
      }

      void set_time_control( const Time_Control &tc ) {
         *((Time_Control*)this) = tc;
      }

      // Get the bonus (time left from previous time control period)
      // in centiseconds
      unsigned long get_bonus() const
      {
         return bonus;
      }

      void set_bonus( unsigned long b ) {
         bonus = b;
      }

   private:
      int period;                                 // time control period, 0..n
      int last_time_control;                      // moves made at end of last time control
      unsigned long bonus;                        // time left from last time control
};
#endif
