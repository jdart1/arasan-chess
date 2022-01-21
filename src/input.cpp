// Copyright 2021 by Jon Dart. All Rights Reserved.
//
#include "input.h"
#include "globals.h"

#include <cstdio>
#include <iostream>

extern "C" {
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/resource.h>
#include <sys/select.h>
#endif
};

static unsigned processCmdChars(char *buf,unsigned len,std::vector<std::string> &cmds, std::mutex &mtx) {
    // try to parse the buffer into command lines
    std::string cmd;
    unsigned last = 0;
    for (unsigned i = 0; i < len; ++i) {
        char c = buf[i];
        if (c == '\r' || c == '\n') {
            // handle CR + LF if present
            if (i+1 < len && (buf[i+1] == '\r' || buf[i+1] == '\n')) i++;
            if (cmd.length()) {
                last = i;
                std::unique_lock<std::mutex> lock(mtx);
                cmds.push_back(cmd);
                cmd.clear();
            }
        } else {
            cmd += c;
        }
    }
    if (cmd.length()) {
        const unsigned unparsed = len - last;
        // copy incomplete line to start of buffer
        std::memcpy(buf,buf+last,unparsed);
        return unparsed;
    } else {
        return 0;
    }
}

Input::Input()
    : buf_index(0)
{
}

bool Input::checkInput(std::vector<std::string> &cmds, std::mutex &mtx) {
#ifdef _WIN32
    DWORD nchar;
    if (PeekNamedPipe(GetStdHandle(STD_INPUT_HANDLE), NULL, 0,
                      NULL, &nchar, NULL)) {
         for (unsigned i = 0; i < nchar && buf_index < BUF_SIZE; i++) {
            buf[buf_index++] = getc(stdin);
         }
         // parse into commands:
         buf_index = processCmdChars(buf,buf_index,cmds,mtx);
         return true;
    } else {
        return false;
    }
#else
    fd_set readfds;
    struct timeval tv;
    int data;

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    // set a timeout so it does not block with no input.
    tv.tv_sec=0;
    tv.tv_usec=500;
    int ret = select(16, &readfds, 0, 0, &tv);
    if (ret == 0) {
        // no data available
        return false;
    }
    else if (ret == -1) {
        perror("select");
        return false;
    }
    data=FD_ISSET(STDIN_FILENO, &readfds);
    if (data>0) {
        // we have something to read
        int bytes = read(STDIN_FILENO,buf+buf_index,BUF_SIZE-buf_index);
        if (bytes == -1) {
            perror("input poll: read");
            return false;
        }
        else if (bytes) {
            // parse into commands:
            buf_index = processCmdChars(buf + buf_index, bytes, cmds, mtx);
            return true;
        }
    }
    return false;
#endif
}

bool Input::readInput(std::vector<std::string> &cmds, std::mutex &mtx)
{
#ifdef _WIN32
    BOOL bSuccess;
    DWORD dwRead;
    if (_isatty(_fileno(stdin))) {
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        // we are reading direct from the console, enable echo & control-char
        // processing
        if (!SetConsoleMode(hStdin, ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT |
                            ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT)) {
            std::cerr << "SetConsoleMode failed" << std::endl;
        }
        bSuccess = ReadConsole(hStdin, buf + buf_index, BUF_SIZE - buf_index, &dwRead, NULL);
        if (bSuccess) {
            buf_index = processCmdChars(buf, dwRead, cmds, mtx);
            return true;
        }
        else {
            return false;
        }
    }
#endif
    // Linux/Mac or non-console Windows code
    std::string cmd;
    if (std::getline(std::cin, cmd)) {
        if (cmd.length()) {
            std::unique_lock<std::mutex> lock(mtx);
            cmds.push_back(cmd);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

