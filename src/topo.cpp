// Copyright 2016, 2018 by Jon Dart. All Rights Reserved.
#include "topo.h"

#include <iostream>
#include <limits>
#include <sstream>

#include "globals.h"

Topology::Topology()
{
    int ret = init();
    if (ret) {
        cout << "error initializing topology" << std::endl;
    }
    else {
        cout << description() << endl;
        computeSet();
    }
}

Topology::~Topology()
{
    cleanup();
}

std::string Topology::description() const {
    int socks = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_SOCKET);
    int cores = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_CORE);
    int cpus  = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_PU);
    std::stringstream s;
    s << "detected " << socks << " socket(s), " <<
        cores << " core(s), " << cpus << " logical processing units.";
    return s.str();
}

int Topology::bind(ThreadInfo *thread)
{
    return hwloc_set_thread_cpubind(topo,thread->thread_id,cpuset[thread->index],HWLOC_CPUBIND_THREAD | HWLOC_CPUBIND_STRICT);
}

void Topology::recalc() {
    cleanup();
    init();
    computeSet();
}

int Topology::computeSet()
{
    const int n = options.search.ncpus;

    if (options.search.set_processor_affinity) {
        hwloc_obj_t root = hwloc_get_root_obj(topo);
        int result = hwloc_distrib(topo, &root, 1, cpuset, n,  std::numeric_limits<int>::max(), static_cast<unsigned long>(0));
        if (result) {
            cerr << "hwloc_distrib failed" << endl;
            return -1;
        }
        // hwloc_distrib may return a range of cpus on which threads can be
        // allocated. Use hwloc_bitmap_singlify here to assign each thread
        // to a unique CPU, minimizing migration costs.
        for (int i = 0; i < n; i++) {
            hwloc_bitmap_singlify(cpuset[i]);
        }
    } else {
        hwloc_const_cpuset_t allowed = hwloc_topology_get_allowed_cpuset(topo);
        // each thread can be allocated on any node:
        for (int i = 0; i < n; i++) {
#ifdef _WIN32
            // under Windows we must allocate threads in a processor group,
            // and so the allowed CPUs cannot cross a 64-bit boundary
            auto allowed_group = hwloc_bitmap_dup(allowed);
            int first = hwloc_bitmap_first(allowed_group);
            int last = hwloc_bitmap_last(allowed_group);
            if (first == -1 || last == -1) {
                cerr << "no avaialable CPUS" << endl;
                return -1;
            }
            for (int j = first; j < last; j++) {
                if (j < i/64 || j >= 64*(1+i/64)) {
                    hwloc_bitmap_clr(allowed_group,j);
                }
            }
            hwloc_bitmap_copy(cpuset[i],allowed_group);
            hwloc_bitmap_free(allowed_group);
#else
            hwloc_bitmap_copy(cpuset[i],allowed);
#endif
        }
    }
    return 0;
}


void Topology::cleanup()
{
    hwloc_topology_destroy(topo);
    for (int i = 0; i < Constants::MaxCPUs; i++) {
        hwloc_bitmap_free(cpuset[i]);
    }
}

int Topology::init() {
    for (int i = 0; i < Constants::MaxCPUs; i++) {
        cpuset[i] = hwloc_bitmap_alloc();
    }
    if (hwloc_topology_init(&topo)) {
        return 1;
    }
    return hwloc_topology_load(topo);
}


