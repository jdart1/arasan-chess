#ifndef __DRAWUTIL_H
#define __DRAWUTIL_H

#include <afxwin.h>

class DrawUtil
{
   public:
      static void Draw(CDC* pDC, const CRect &loc, LPCSTR text);

   private:
      static void PolyDraw(CDC* pDC, CONST LPPOINT lppt, CONST LPBYTE lpbTypes,
         int cCount);
};
#endif
