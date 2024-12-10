// Copyright 2001, 2002, 2012, 2023-2024 by Jon Dart. All Rights Reserved
#ifndef _GUIOPTS_H
#define _GUIOPTS_H

#include "timectrl.h"
#include "searchop.h"

// This class holds information on user preferences for the GUI.
// These preferences are saved into the system registry.

class CWinApp;

struct GeneralPreferences
{
   BOOL beep_after_move;
   BOOL beep_on_error;
   BOOL can_resign;
   BOOL think_when_idle;
   BOOL position_learning;
   int hash_table_size;
   BOOL auto_size_hash_table;
   int cores;
   int strength;
   CString tbPath;
   int bookVariety;
};

class GuiOptions
{

   public:

      enum BoardSize { Small, Medium, Large, XLarge, XXLarge };

      struct Appearance
      {
         BOOL show_coordinates;
         string pieceFontName;
         BoardSize board_size;
         COLORREF lightSquareColor, darkSquareColor;
         BOOL forceMono;
      };

      GuiOptions(CWinApp *);

      const Search_Limit_Options &get_src_limits() const {
         return src_opts;
      }

      void set_src_limits(const Search_Limit_Options &new_opts) {
         src_opts = new_opts;
      }

      BOOL beep_after_move() const {
         return gprefs.beep_after_move;
      }

      BOOL beep_on_error() const {
         return gprefs.beep_on_error;
      }

      BOOL can_resign() const {
         return gprefs.can_resign;
      }

      BOOL think_when_idle() const {
         return gprefs.think_when_idle;
      }

      BOOL position_learning() const {
         return gprefs.position_learning;
      }

      BOOL show_coordinates() const {
         return appr.show_coordinates;
      }

      void setShowCoordinates(BOOL value) {
         appr.show_coordinates = value;
      }

      int hash_table_size() const {
         return gprefs.hash_table_size;
      }

      BOOL auto_size_hash_table() const {
         return gprefs.auto_size_hash_table;
      }

      int cores() const {
         return gprefs.cores;
      }

      int strength() const {
          return gprefs.strength;
      }

      int bookVariety() const {
          return gprefs.bookVariety;
      }

      const CString &tbPath() const {
          return gprefs.tbPath;
      }

      void setTbPath(const CString &path) {
        gprefs.tbPath = path;
      }

      void getGeneralPreferences(GeneralPreferences &prefs) {
         prefs = gprefs;
      }

      void setGeneralPreferences(const GeneralPreferences &prefs) {
         gprefs = prefs;
      }

      void save();

      void set_app_location(int x, int y);

      void get_app_location(int &x, int &y);

      LPCSTR getPieceFontName() const
      {
         return appr.pieceFontName.c_str();
      }

      void setPieceFontName(LPCSTR font) {
         appr.pieceFontName = font;
      }

      void setBoardSize(BoardSize size) {
         appr.board_size = size;
      }

      const BoardSize getBoardSize() const
      {
         return appr.board_size;
      }

      const COLORREF getLightSquareColor() const
      {
         return appr.lightSquareColor;
      }

      const void setLightSquareColor(COLORREF color) {
         appr.lightSquareColor = color;
      }

      const COLORREF getDarkSquareColor() const
      {
         return appr.darkSquareColor;
      }

      const void setDarkSquareColor(COLORREF color) {
         appr.darkSquareColor = color;
      }

      const BOOL getForceMono() {
         return appr.forceMono;
      }

      void setForceMono(BOOL flag) {
         appr.forceMono = flag;
      }

      UINT getFlags() const {
         return flags;
      }

      void setFlags(UINT f) {
         flags = f;
      }

      UINT getShowCmd() const {
         return showCmd;
      }

      void setShowCmd(UINT cmd) {
         showCmd = cmd;
      }

   private:
      void load_src_limits();
      void store_src_limits();

      Search_Limit_Options src_opts;
      GeneralPreferences gprefs;
      Appearance appr;
      CWinApp *app;
      int appX, appY;
      UINT flags, showCmd;
};

extern int calc_hash_size();
#endif
