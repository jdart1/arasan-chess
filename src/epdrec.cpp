// Copyright 1996, 2008, 2013 by Jon Dart. All Rights Reserved.
//
#include "epdrec.h"
#include <string.h>
#include <iterator>

EPDRecord::EPDRecord()
{
}

EPDRecord::~EPDRecord()
{
}

bool EPDRecord::hasVal(const string &key) const
{
   list< pair<string,string> >::const_iterator it = cmds.begin();
   while (it != cmds.end())
   {
      if ((*it).first == key)
      {
         return true;
      }
      it++;
   }
   return false;
}

bool EPDRecord::getVal(const string &key, string &val) const
{
   list< pair<string,string> >::const_iterator it = cmds.begin();
   while (it != cmds.end())
   {
      if ((*it).first == key)
      {
         val = (*it).second;
         return true;
      }
      it++;
   }
   return false;
}

bool EPDRecord::getData(int i,string &key,string &val) const
{
   list< pair<string,string> >::const_iterator it = cmds.begin();
   int index = 0;
   while (it != cmds.end())
   {
      if (index == i)
      {
         val = (*it).second;
         key = (*it).first;
         return true;
      }
      ++index;
      it++;
   }
   return false;
}

void EPDRecord::add(const string &key, const string &value)
{
   cmds.push_back(pair<string,string>(key,value));
}

void EPDRecord::clear() {
   err = "";
   cmds.clear();
}

