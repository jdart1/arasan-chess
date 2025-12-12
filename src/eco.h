// Copyright 1994, 2008, 2021, 2025 by Jon Dart
#ifndef __ECO_H__
#define __ECO_H__

#include "movearr.h"
#include <string>
#include <map>
#include <unordered_set>
#include <memory>

struct ECOData {
    const std::string *eco;
    const std::string *opening_name;

    ECOData() : eco(nullptr), opening_name(nullptr) {
    }

    ECOData(const std::string *ec, const std::string *name) : eco(ec), opening_name(name) {
    }
};

class ECO
{
public:
    ECO();

    void init(const std::string &path);

    bool initOk() const noexcept {
        return initDone;
    }

    // Look up a game log, return its ECO code (in "result") and the
    // descriptive name of the opening (in "name"), if there is one.
    void classify( const MoveArray &moves, std::string &result, std::string &name);

private:
    // String pools to avoid duplicate storage
    std::unordered_set<std::string> ecoPool;
    std::unordered_set<std::string> namePool;

    // Map from position hash to ECO data (stored by value)
    std::unique_ptr< std::map <hash_t, ECOData> > ht;

    bool initDone;
};

#endif
