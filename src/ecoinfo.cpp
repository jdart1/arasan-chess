// Copyright 1994 by Jon Dart.
#include "ecoinfo.h"

#include <cstring>

ECO_Info::ECO_Info()
{
}

ECO_Info::ECO_Info( const char *code, const char *opening_name, hash_t hc) 
{
    my_hashcode = hc;
    if (code)
    {
       strncpy(my_eco,code,3);
    }
    else
       *my_eco = '\0';
    if (opening_name)
        my_opening_name = (char*)opening_name;
    else
        my_opening_name = nullptr;
}

ECO_Info::ECO_Info(const ECO_Info &e)
{
    my_hashcode = e.my_hashcode;
    strncpy(my_eco,e.my_eco,3);
    if (e.my_opening_name)
    {
        my_opening_name = strdup(e.my_opening_name);
    }
    else
        my_opening_name = nullptr;
}

ECO_Info::~ECO_Info()
{
}

ECO_Info &ECO_Info::operator = (const ECO_Info &e)
{
    if (this !=&e)
    {
       my_hashcode = e.my_hashcode;
       strncpy(my_eco,e.my_eco,3);
       if (e.my_opening_name)
       {
           my_opening_name = strdup(e.my_opening_name);
       }
       else
           my_opening_name = nullptr;
    }
    return *this;
}



