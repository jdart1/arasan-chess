// Copyright 1994, 2008 by Jon Dart

#include "eco.h"

#define HASHSIZE 61

ECO::ECO()
{
   // fill up hash table
   for (const ECOData *d = eco_codes; d->eco; d++)
   {
        map<hash_t,const ECOData *>::value_type p(d->hash_code,d);
        ht.insert(p);
   }
}

void ECO::classify( const MoveArray &moves, string &result, string &name)
{
    // We take kind of a simple-minded approach to ECO classification.
    // Each ECO code is associated with a chess position.  We follow the
    // game moves until we have found the last one that leads to a
    // hash code matching an ECO position, then report that ECO code.
    // This doesn't allow for openings that eventually transpose into
    // an ECO subline (e.g. A04/01) but don't ever hit the position
    // for the main ECO code (e.g. A04) - this is possible.     
    result = "";
    name = "";
    for ( unsigned i = 0; i < moves.num_moves(); i++)
    {
        map<hash_t,const ECOData*>::const_iterator it =
         ht.find(moves[i].hashcode());
        if (it != ht.end())
        {
   	   const ECOData *hit = (*it).second;
	   result = hit->eco;
           // Not all ECO lines have opening names.
           if (hit->opening_name)
	     name = hit->opening_name;
        }
    }
}
