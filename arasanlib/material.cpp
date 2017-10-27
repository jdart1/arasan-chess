// Copyright 1993 by Jon Dart.  All Rights Reserved.

#include "material.h"

// data layout in info field:
//bits	
//0-3	pawns
//4-7	knights
//8-11	bishops
//12-15	rooks
//16-19	queens
//20    kings

const int32_t Material::masks[8] =
{ 0, 1, 0x10, 0x100, 0x1000, 0x10000, 0x100000, 0};

const int32_t Material::masks2[8] =
{ 0, 0xf, 0xf0, 0xf00, 0xf000, 0xf0000, 0x100000, 0};

const int32_t Material::levels[8] =
{ 0, 0, 3, 3, 5, 9, 0, 0};

