// Copyright 1994 by Jon Dart.  All Rights Reserved.

#ifndef _ECOINFO_H
#define _ECOINFO_H

#include "chess.h"

class ECO_Info {
    // this class represents one entry in the ECO hash table.

    ECO_Info();

    public:

    ECO_Info( const char *code, const char *opening_name,
              hash_t hashcode );

    ECO_Info(const ECO_Info &);
    
    virtual ~ECO_Info();

    ECO_Info &operator = (const ECO_Info &);
    
    const char * get_eco() const
    {
        return my_eco;
    }
    
    const char * get_opening_name() const
    {
        return my_opening_name;
    }

    hash_t hash_code() const
    {
	return my_hashcode;
    }
    
    private:

    hash_t my_hashcode;
    char *my_opening_name;
    char my_eco[3];
};

#endif
