// Copyright 2016 by Jon Dart. All Rights Reserved.
#ifndef _TOPO_H
#define _TOPO_H

// Support for hardware topology queries and affinity. Depends
// on the hwloc library.

extern "C" {
#include <hwloc.h>
};

#include <string>

class Topology {

 public:
  Topology();
  virtual ~Topology();

  std::string description() const;

  // Bind the thread with the specified index to an available
  // processing unit (call from the current thread).
  // Attempt to keep consecutive threads local to a socket and/or
  // other enclosing unit (NUMA node, cache). If offset is non-zero,
  // then available PUs in range 0..offset-1 are not used.
  // Returns 0 on success.
  int bind(int index);

  // Recalculate topology. Called after thread pool is resized.
  void recalc();

  // Set allowable cpu set to all threads (so no affinity is set)
  void reset();

 private:
  int init();
  void cleanup();
  void computeSet();

  hwloc_topology_t topo;
  // allowable cpu set
  hwloc_cpuset_t set;
};

#endif
