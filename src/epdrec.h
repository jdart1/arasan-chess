// Copyright 1996, 2008, 2013, 2017, 2021 by Jon Dart. All Rights Reserved.
//
#ifndef __EPDREC_H__
#define __EPDREC_H__

#include <fstream>
#include <string>
#include <list>
#include <utility>

// This class encapsulates info about one EPD
// (Extended Position Description) record.

class EPDRecord
{
public:
    EPDRecord();
    ~EPDRecord();

    bool hasVal(const std::string &key) const;

    bool getVal(const std::string &key, std::string &val) const;

    void add(const std::string &key, const std::string &val);

    bool hasError() const
    {
        return err.length() > 0;
    }

    const std::string &getError() const
    {
        return err;
    }

    void setError(const std::string &anErr)
    {
        err = anErr;
    }
    
    bool getData(int i,std::string &key,std::string &val) const;
    
    unsigned getSize() const
    {
       return (unsigned)cmds.size();
    }
    
    void clear();
    
private:
    using pairlist = std::list< std::pair<std::string,std::string> >;

    pairlist cmds;
    std::string err;
};

#endif
