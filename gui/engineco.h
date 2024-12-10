// Copyright 2001 by Jon Dart. All Rights Reserved.
//
#if !defined(_ENGINE_CO_H)
#define _ENGINE_CO_H

#include "arasan.h"

struct ThreadComm
{
   char *moduleName;
   ArasanGuiApp *app;
   HANDLE hChildReadHandle;
   HANDLE hChildWriteHandle;
   int result;
   char *errorMsg;
   char *cmdLine;
};

/////////////////////////////////////////////////////////////////////////////
// EngineCommunication thread method

extern UINT CDECL EngineThreadProc( LPVOID pParam );

// write to the engine
extern void writeToEngine(const char *cmd);

// access to queue of commands read from engine
extern int commandCount();
extern char * readCommand();
extern void pushCommand(LPCSTR cmd);
extern BOOL engineReady();
extern int force;
extern void setForce(int  value);
extern void startSearch();
#endif
