// Copyright 1994, 2008 by Jon Dart
#ifndef __ECO_H__
#define __ECO_H__

#include "movearr.h"
#include "ecodata.h"
#include <string>
#include <map>

using namespace std;

class ECO
{
public:
    ECO();
    
    // Look up a game log, return its ECO code (in "result") and the
    // descriptive name of the opening (in "name"), if there is one.	    
    void classify( const MoveArray &moves, string &result, string &name);
        
private:        
    map <hash_t,const ECOData *> ht;
};

#endif
