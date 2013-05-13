#ifndef _ECODATA_H_
#define _ECODATA_H_

#include "types.h"

struct ECOData
{
	const char *eco;
	hash_t hash_code;
	const char *opening_name;
};

extern const ECOData eco_codes[];

#endif
