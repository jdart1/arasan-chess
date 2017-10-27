// Copyright 1996, 2008, 2013, 2017 by Jon Dart. All Rights Reserved.
//
#ifndef __EPDREC_H__
#define __EPDREC_H__

#include <fstream>
#include <string>
#include <list>
#include <utility>

using namespace std;

// This class encapsulates info about one EPD
// (Extended Position Description) record.

class EPDRecord
{
public:
    EPDRecord();
    ~EPDRecord();

    bool hasVal(const string &key) const;

    bool getVal(const string &key, string &val) const;

    void add(const string &key, const string &val);

    bool hasError() const
    {
        return err.length() > 0;
    }

    const string &getError() const
    {
        return err;
    }

    void setError(const string &anErr)
    {
        err = anErr;
    }
    
    bool getData(int i,string &key,string &val) const;
    
    unsigned getSize() const
    {
       return (unsigned)cmds.size();
    }
    
    void clear();
    
private:
    list< pair<string,string> > cmds;
    string err;
};

#endif
