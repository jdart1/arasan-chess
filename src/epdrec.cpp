// Copyright 1996, 2008, 2013, 2021 by Jon Dart. All Rights Reserved.
//
#include "epdrec.h"
#include <cstring>
#include <iterator>

EPDRecord::EPDRecord()
{
}

EPDRecord::~EPDRecord()
{
}

bool EPDRecord::hasVal(const std::string &key) const
{
   pairlist::const_iterator it = cmds.begin();
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

bool EPDRecord::getVal(const std::string &key, std::string &val) const
{
   pairlist::const_iterator it = cmds.begin();
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

bool EPDRecord::getData(int i,std::string &key,std::string &val) const
{
   pairlist::const_iterator it = cmds.begin();
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

void EPDRecord::add(const std::string &key, const std::string &value)
{
    cmds.push_back(std::pair<std::string,std::string>(key,value));
}

void EPDRecord::clear() {
   err = "";
   cmds.clear();
}

