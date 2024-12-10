#include "drawutil.h"

// This is based on sample code from MSDN

// This OnDraw() implementation uses GDI paths to draw the outline of
// some text in a TrueType font. The path is used to record the way
// the TrueType font would be drawn. Then, the function uses the data
// returned from CDC::GetPath() to draw the font--without filling it.

void DrawUtil::Draw(CDC* pDC, const CRect &loc, LPCSTR text)
{
   // use a path to record how the text was drawn
   pDC->BeginPath();
   pDC->TextOut(loc.left, loc.top, text, 1);
   pDC->EndPath();

   // Find out how many points are in the path. Note that
   // for long strings or complex fonts, this number might be
   // gigantic!
   int nNumPts = pDC->GetPath(NULL, NULL, 0);
   if (nNumPts == 0)
      return;

   // Allocate memory to hold points and stroke types from
   // the path.
   LPPOINT lpPoints = new POINT[nNumPts];
   if (lpPoints == NULL)
      return;
   LPBYTE lpTypes = new BYTE[nNumPts];
   if (lpTypes == NULL) {
      delete [] lpPoints;
      return;
   }
   // Now that we have the memory, really get the path data.
   nNumPts = pDC->GetPath(lpPoints, lpTypes, nNumPts);

   // If it worked, draw the lines. Win95 and Win98 don't support
   // the PolyDraw API, so we use our own member function to do
   // similar work. If you're targeting only Windows NT, you can
   // use the PolyDraw() API and avoid the PolyDraw() member function.

   CPen blackPen(PS_SOLID,1,RGB(0,0,0));
   CPen *oldPen = pDC->SelectObject(&blackPen);
   if (nNumPts != -1)
      PolyDraw(pDC, lpPoints, lpTypes, nNumPts);

   pDC->SelectObject(oldPen);

   // Release the memory we used
   delete [] lpPoints;
   delete [] lpTypes;

   return;
}


void DrawUtil::PolyDraw(CDC* pDC, CONST LPPOINT lppt, CONST LPBYTE lpbTypes,
int cCount)
{
   int nIndex;
   LPPOINT pptLastMoveTo = NULL;

   // for each of the points we have...
   for (nIndex = 0; nIndex < cCount; nIndex++) {
      switch(lpbTypes[nIndex]) {
         // React to information from the path by drawing the data
         // we received. For each of the points, record our own
         // "last active point" so we can close figures, lines, and
         // Beziers.

         case PT_MOVETO:
            if (pptLastMoveTo != NULL && nIndex > 0)
               pDC->LineTo(pptLastMoveTo->x, pptLastMoveTo->y);
            pDC->MoveTo(lppt[nIndex].x, lppt[nIndex].y);
            pptLastMoveTo = &lppt[nIndex];
            break;

         case PT_LINETO | PT_CLOSEFIGURE:
            pDC->LineTo(lppt[nIndex].x, lppt[nIndex].y);
            if (pptLastMoveTo != NULL)
               pDC->LineTo(pptLastMoveTo->x, pptLastMoveTo->y);
            pptLastMoveTo = NULL;
            break;

         case PT_LINETO:
            pDC->LineTo(lppt[nIndex].x, lppt[nIndex].y);
            break;

         case PT_BEZIERTO | PT_CLOSEFIGURE:
            ASSERT(nIndex + 2 <= cCount);
            pDC->PolyBezierTo(&lppt[nIndex], 3);
            nIndex += 2;
            if (pptLastMoveTo != NULL)
               pDC->LineTo(pptLastMoveTo->x, pptLastMoveTo->y);
            pptLastMoveTo = NULL;
            break;

         case PT_BEZIERTO:
            ASSERT(nIndex + 2 <= cCount);
            pDC->PolyBezierTo(&lppt[nIndex], 3);
            nIndex += 2;
            break;
      }
   }

   // If the figure was never closed and should be,
   // close it now.
   if (pptLastMoveTo != NULL && nIndex > 1)
      pDC->LineTo(pptLastMoveTo->x, pptLastMoveTo->y);
}
