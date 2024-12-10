// Copyright 2003 by Jon Dart. All Rights Reserved.
// disphint.cpp: implementation of the DisplayHint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "disphint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DisplayHint::DisplayHint(const Board &currentBoard,Move m)
{
   setDirty(StartSquare(m));
   setDirty(DestSquare(m));
   switch(TypeOfMove(m)) {
      case EnPassant:
         setDirty(currentBoard.enPassantSq());
         break;
      case KCastle:
      {
         Square kp = currentBoard.kingSquare(currentBoard.sideToMove());
         setDirty(kp+1);
         setDirty(kp+2);
         setDirty(kp+3);
         break;
      }
      case QCastle:
      {
         Square kp = currentBoard.kingSquare(currentBoard.sideToMove());
         setDirty(kp-1);
         setDirty(kp-2);
         setDirty(kp-3);
         setDirty(kp-4);
         break;
      }
      default:
         break;
   }
}
