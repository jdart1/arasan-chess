// Copyright 1994, 2008, 2021 by Jon Dart
#ifndef __ECO_H__
#define __ECO_H__

#include "movearr.h"
#include "ecodata.h"
#include <string>
#include <map>

class ECO
{
public:
    ECO();
    
    // Look up a game log, return its ECO code (in "result") and the
    // descriptive name of the opening (in "name"), if there is one.	    
    void classify( const MoveArray &moves, std::string &result, std::string &name);
        
private:        
    std::map <hash_t,const ECOData *> ht;
};

#endif
