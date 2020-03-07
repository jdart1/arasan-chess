// Support for the "bench" command
// Copyright 2020 by Jon Dart. All Rights Reserved.
//
#ifndef _BENCH_H
#define _BENCH_H

#include <iostream>
#include <string>

class SearchController;

class Bench {
public:
    Bench() = default;
    virtual ~Bench() = default;
    
    struct Results 
    {
        uint64_t nodes;
        uint64_t time;
        Results() : nodes(0ULL), time(0ULL) {};
    };

    Results bench(int hashSize=1000, int depth=13, int cores=1, bool verbose=false);

    friend std::ostream & operator << (std::ostream &o, const Results &results);

private:
    void benchLine(SearchController *searcher, const std::string &epd, Results &results, int depthLimit, bool verbose);
};

#endif
