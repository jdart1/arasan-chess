// EngineCo.cpp : handles communication with the chess engine process
// Copyright 2001, 2014 by Jon Dart. All Rights Reserved.
//

#include "stdafx.h"
#include "arasan.h"
#include "globals.h"
#include "EngineCo.h"
#include "guiview.h"
extern "C" {
#include <ctype.h>
#include <string.h>
}
#include "mainfrm.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// EngineCommunication

static HANDLE hEngine,hWriteHandle;

static CRITICAL_SECTION queueCritSection;

static int queueSize = 0;
static const int QUEUE_MAX = 20;
static char *cmdQueue[QUEUE_MAX];
static BOOL ready = FALSE;
int force = 0;

static const char *ENGINE_NAME = "arasanx-32.exe";

BOOL engineReady()
{
   return ready;
}


int commandCount()
{
   EnterCriticalSection(&queueCritSection);
   int size = queueSize;
   LeaveCriticalSection(&queueCritSection);
   return size;
}


// read and remove a command from the pending queue
char * readCommand()
{
   char *retVal;
   EnterCriticalSection(&queueCritSection);
   if (queueSize) {
      retVal = _strdup(cmdQueue[0]);
      free(cmdQueue[0]);
      for (int i = 0; i < queueSize-1; i++) {
         cmdQueue[i] = cmdQueue[i+1];
      }
      --queueSize;
   }
   else
      retVal = NULL;
   LeaveCriticalSection(&queueCritSection);
   return retVal;
}


void pushCommand(LPCSTR cmd)
{
   EnterCriticalSection(&queueCritSection);
   ASSERT(queueSize < QUEUE_MAX);
   cmdQueue[queueSize++] = _strdup(cmd);
   LeaveCriticalSection(&queueCritSection);
}


static void DisplayError(LPCSTR function)
{
   LPTSTR errMsg = NULL;
   int nError = GetLastError();
   int nChars = FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      nError,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // Default language
      errMsg,
      512,
      NULL
      );
   CString msg;

   if (nChars == 0) {
      CString msg2;
      switch (nError) {
         case 2:
            AfxFormatString1(msg2,IDS_ENGINE_NOT_FOUND,ENGINE_NAME);
            break;
         case 5:
            msg2.LoadString(IDS_ENGINE_ACCESS_DENIED);
            break;
         default:
            break;
      }
      AfxFormatString2(msg,IDS_ENGINE_STARTUP,msg2,"");
   }
   else
      AfxFormatString2(msg,IDS_ENGINE_STARTUP,function,errMsg);
   AfxMessageBox(msg);
   LocalFree(errMsg);
}


HANDLE startProcess(const char *cmdLine, HANDLE hChildStdOut,HANDLE hChildStdIn, HANDLE hChildStdErr)
{
   BOOL result;
   STARTUPINFO si;
   PROCESS_INFORMATION pi;

   ZeroMemory(&si,sizeof(STARTUPINFO));
   si.cb = sizeof(STARTUPINFO);
   ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));
   si.dwFlags = STARTF_USESTDHANDLES;
   si.hStdOutput = hChildStdOut;
   si.hStdInput  = hChildStdIn;
   si.hStdError  = hChildStdErr;
   si.wShowWindow = SW_HIDE;

   TRACE("calling CreateProcess with command line %s\n",cmdLine);
   result = CreateProcess(
      NULL,
      LPSTR(cmdLine),                // executable path + any arguments
      NULL,                                       // security attributes
      NULL,                                       // thread attributes
      TRUE,                                       // bInheritHandles
   #ifdef _DEBUG
      0,
   #else
   // creation flags
      CREATE_NO_WINDOW | DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP,
   #endif
      NULL,                                       // environment block
      NULL,                                       // current directory
      &si,
      &pi);

   if (result) {
      // detach from the child process
      CloseHandle(pi.hThread);
      //CloseHandle(pi.hProcess);
      return pi.hProcess;
   }
   else {
      DisplayError("CreateProcess");
      return NULL;
   }
}


inline CString GetLastErrorText(BOOL bShowMessageBoxAlso = TRUE)
{
   LPVOID lpMsgBuf;
   FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
   // Default language
      NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf, 0, NULL );

   CString cstrMessageText = (LPTSTR)lpMsgBuf;

   if(bShowMessageBoxAlso == TRUE) {
      // Display the string.
      MessageBox( NULL, cstrMessageText, _T("GetLastErrorText"), MB_OK|MB_ICONINFORMATION );
   }
   // Free the buffer.
   LocalFree( lpMsgBuf );
   return cstrMessageText;
}


#define ErrorExit(msg) { comm->errorMsg=strdup(msg); \
   comm->result = GetLastError(); \
   GetLastErrorText(); \
   return -1; \
}

UINT CDECL EngineThreadProc( LPVOID pParam )
{
   HANDLE hOutputReadTmp,hOutputRead,hOutputWrite;
   HANDLE hInputWriteTmp,hInputRead,hInputWrite;
   HANDLE hErrorWrite;

   InitializeCriticalSection(&queueCritSection);

   ThreadComm *comm = (ThreadComm*)pParam;
   char *moduleName = comm->moduleName;
   std::string app_path = globals::derivePath(ENGINE_NAME);
   // Turn on verbose engine output if debugging or trace
#ifdef _DEBUG
   app_path += " -t";
#else
   if (doTrace) app_path += " -t";
#endif

   SECURITY_ATTRIBUTES sa;

   // Redirect child handles. See Microsoft Support article Q190351

   // Set up the security attributes struct.
   sa.nLength= sizeof(SECURITY_ATTRIBUTES);
   sa.lpSecurityDescriptor = NULL;
   sa.bInheritHandle = TRUE;

   // Create the child output pipe.
   if (!CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0))
      DisplayError("CreatePipe");

   // Create a duplicate of the output write handle for the std error
   // write handle. This is necessary in case the child application
   // closes one of its std output handles.
   if (!DuplicateHandle(GetCurrentProcess(),hOutputWrite,
      GetCurrentProcess(),&hErrorWrite,0,
      TRUE,DUPLICATE_SAME_ACCESS))
      DisplayError("DuplicateHandle");

   // Create the child input pipe.
   if (!CreatePipe(&hInputRead,&hInputWriteTmp,&sa,0))
      DisplayError("CreatePipe");

   // Create new output read handle and the input write handles. Set
   // the Properties to FALSE. Otherwise, the child inherits the
   // properties and, as a result, non-closeable handles to the pipes
   // are created.
   if (!DuplicateHandle(GetCurrentProcess(),hOutputReadTmp,
      GetCurrentProcess(),
      &hOutputRead,                               // Address of new handle.
      0,FALSE,                                    // Make it uninheritable.
      DUPLICATE_SAME_ACCESS))
      DisplayError("DuplicateHandle");

   if (!DuplicateHandle(GetCurrentProcess(),hInputWriteTmp,
      GetCurrentProcess(),
      &hInputWrite,                               // Address of new handle.
      0,FALSE,                                    // Make it uninheritable.
      DUPLICATE_SAME_ACCESS))
      DisplayError("DuplicateHandle");

   // Close inheritable copies of the handles you do not want to be
   // inherited.
   if (!CloseHandle(hOutputReadTmp)) DisplayError("CloseHandle");
   if (!CloseHandle(hInputWriteTmp)) DisplayError("CloseHandle");
   if (!CloseHandle(hErrorWrite)) DisplayError("CloseHandle");

   //comm->app->setEngineHandle(pHandle);

   HANDLE hProcess = startProcess(app_path.c_str(),hOutputWrite,hInputRead,hErrorWrite);

   hWriteHandle = hInputWrite;
   ready = TRUE;

   // big buffer may be needed if engine has debugging on.
   const int BUFSIZE = 16384;
   char chBuf[BUFSIZE];
   DWORD dwRead = 0;
   int bufLen = 0;
   int offset = 0;

   // Now we're ready to communicate. Wait for engine input.
   for (;;) {
      int err;
      // read a chunk of input (all that is available)
      err = ReadFile( hOutputRead, chBuf+bufLen, BUFSIZE-bufLen, &dwRead,
                      NULL);
      if (err == 0) {
          DWORD errcode = GetLastError();
          if (errcode == ERROR_PIPE_BUSY) {
              continue;
          } else if (errcode == ERROR_BROKEN_PIPE) {
              if (doTrace) {
                  traceFile << "error, chess engine died or disconnected." << endl;
              }
              break;
          } else {
              if (doTrace) {
                  traceFile << "error from read: " << errcode << endl;
              }
              break;
          }
      } else if (dwRead == 0) {
          if (doTrace) {
              traceFile << "warning, no chars from read" << endl;
              break;
          }
      }

      // split into lines.
      static const int CMD_BUF_SIZE=4096;
      char cmdBuf[CMD_BUF_SIZE];
      char *cmd = cmdBuf;
      char *buf = chBuf;
      bufLen += dwRead;

      int n = 0;
      int sep = n;
      while (sep < bufLen) {
         // find the next separator
         int found = 0;
         while (sep < bufLen) {
            if (chBuf[sep] == '\012' || chBuf[sep] == '\015') {
               found++;
               break;
            }
            sep++;
         }
         if (found) {
            if (sep-n>0) {
               // copy line to command buffer
               ASSERT(sep-n < CMD_BUF_SIZE);
               memcpy(cmdBuf,chBuf+n,sep-n);
            }
            cmdBuf[sep-n] = '\0';
            if (strlen(cmdBuf) != 0) {
               TRACE("from engine: %s\n",cmdBuf);
               if (doTrace) traceFile << "from engine: " << cmdBuf << endl;
               // engine output that starts with '#' is only for tracing.
               if (*cmdBuf != '#') {
                   pushCommand(cmdBuf);
                   // dispatch command to the view class
                   if (comm->app->getView())
                       comm->app->getView()->SendMessage(WM_COMMAND,ID_ENGINE_REPLY,0L);
                   else {
                       TRACE("GUI not ready: could not dispatch command\n");
                   }
               }
            }
            // skip over any remaining end of line chars
            while (sep<bufLen && (chBuf[sep]=='\015' || chBuf[sep] == '\012')) sep++;
            n=sep;
         }
         else {
            // stuff left but it's not a complete line
            //TRACE("incomplete line, breaking\n");
            break;
         }
      }
      // now we have read and processed up to the nth character.
      // remove those from the buffer
      if (n) {
         memcpy(chBuf,chBuf+n,n);
         bufLen -= n;
      }
   }
   TRACE("exiting read thread");
   return 0;
}


void writeToEngine(const char *cmd)
{

   if (ready == FALSE) {
      TRACE("ignoring command: \"%s\" - not ready yet\n",cmd);
      return;
   }
   DWORD written;
   if (!::WriteFile(hWriteHandle,cmd,strlen(cmd),&written,NULL)) {
      DisplayError("WriteFile");
   }
   int len = strlen(cmd);
   if (len && cmd[len-1] != '\n') {
       // add newline if not already terminated with one
       char buf[1];
       *buf = '\n';
       if (!::WriteFile(hWriteHandle,buf,1,&written,NULL)) {
           DisplayError("WriteFile");
       }
       if (doTrace) traceFile << "to engine: " << cmd << endl;
       TRACE("sending to engine: %s\n",cmd);
   }
   else {
       if (doTrace) traceFile << "to engine: " << cmd;
       TRACE("sending to engine: %s",cmd);
   }
}


void setForce(int value)
{
   if (doTrace) traceFile << "setForce: was " << force << ", is " << value << endl;
   if (value) {
      if (!force) {
         writeToEngine("force\n");
         force = 1;
         ArasanMainFrame *mainFrm = (ArasanMainFrame*)AfxGetMainWnd();
         if (mainFrm) mainFrm->setStatus(ArasanMainFrame::Ready);
      }
   }
   else if (force) {
      writeToEngine("go\n");
      force = 0;
   }
}


void startSearch()
{
   writeToEngine("go\n");
   force = 0;
}
