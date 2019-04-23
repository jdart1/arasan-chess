// Copyright 2016, 2018 by Jon Dart. All Rights Reserved.
#ifndef _TOPO_H
#define _TOPO_H

// Support for hardware topology queries and affinity. Depends
// on the hwloc library.

#include "constant.h"

extern "C" {
#include <hwloc.h>
};

#include <string>

struct ThreadInfo;

class Topology {

public:
    Topology();
    virtual ~Topology();

    std::string description() const;

    // Bind the specified thread to an available processing unit (call
    // from the current thread).
    // Returns 0 on success.
    int bind(ThreadInfo *);

    // Recalculate topology. Called after thread pool is resized.
    void recalc();

private:
    int init();
    void cleanup();
    int computeSet();

    hwloc_topology_t topo;
    // allowable cpu set per thread
    hwloc_cpuset_t cpuset[Constants::MaxCPUs];
};

#endif
