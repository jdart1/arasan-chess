// Copyright 2003 by Jon Dart. All Rights Reserved.
// disphint.h: interface for the DisplayHint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISPHINT_H__8A996FA6_6570_4814_9FFE_7D62AB711AF7__INCLUDED_)
#define AFX_DISPHINT_H__8A996FA6_6570_4814_9FFE_7D62AB711AF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif                                            // _MSC_VER > 1000

#include "board.h"

class DisplayHint : public CObject
{
   public:
      DisplayHint() {
      }

      // Initialize the DisplayHint based on a move about to
      // to made. "board" should have the position before the
      // move.
      DisplayHint(const Board &board, Move m);

      virtual ~DisplayHint() {
      }

      void setDirty(Square sq) {
         dirty.set(sq);
      }
      void clearDirty(Square sq) {
         dirty.clear(sq);
      }
      BOOL isDirty(Square sq) {
         return (BOOL)dirty.isSet(sq);
      }

   private:
      Bitboard dirty;
};
#endif                                            // !defined(AFX_DISPHINT_H__8A996FA6_6570_4814_9FFE_7D62AB711AF7__INCLUDED_)
