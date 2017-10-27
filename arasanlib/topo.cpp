// Copyright 2016 by Jon Dart. All Rights Reserved.
#include "topo.h"

#include <iostream>
#include <sstream>

#include "globals.h"
extern "C" {
#include "hwloc/helper.h"
};

Topology::Topology()
{
   int ret = init();
   if (ret) {
      std::cout << "error initializing topology" << std::endl;
   }
   else {
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

int Topology::bind(int index)
{
#ifdef NUMA
   if (!options.search.set_processor_affinity) {
      return 0;
   }
#endif
   hwloc_obj_t core = hwloc_get_obj_inside_cpuset_by_type(topo,
                                                          set,
                                                          HWLOC_OBJ_CORE,
                                                          index);
   if (!core) {
      return -1;
   }
   int result;
   // bind to only a single PU within the core
   hwloc_cpuset_t bind_set = hwloc_bitmap_dup(core->allowed_cpuset);
   hwloc_bitmap_singlify(bind_set);
   result = hwloc_set_cpubind(topo,bind_set,HWLOC_CPUBIND_STRICT | HWLOC_CPUBIND_THREAD);
#ifdef _TRACE
   char buf[100];
   hwloc_bitmap_snprintf(buf,100,bind_set);
   std::cout << "bound thread " << index << " to " << buf << std::endl;
#endif
   hwloc_bitmap_free(bind_set);
   if (result) {
      return result;
   }
   return result;
}

static void accum(hwloc_obj_t obj,hwloc_cpuset_t set, int &count,
                  int offset, int n)
{
   // do a depth-first traversal of the tree, which will place cpus
   // together that share common parent(s).
   for (unsigned i = 0; i < obj->arity; i++) {
      if (obj->children[i]->type == HWLOC_OBJ_CORE) {
         if (count >= offset && count < n) {
            // TBD: hyperthreading may cause > 1 bit to be set here
            hwloc_bitmap_or(set,set,obj->children[i]->allowed_cpuset);
         }
         ++count;
         // do not traverse below CPU level
      } else {
         accum(obj->children[i],set,count,offset,n);
      }

   }
}

void Topology::recalc() {
   cleanup();
   init();
   computeSet();
}

void Topology::reset() {
   hwloc_obj_t root = hwloc_get_root_obj(topo);
   hwloc_bitmap_free(set);
   set = hwloc_bitmap_dup(root->allowed_cpuset);
}

void Topology::computeSet()
{
   const int n = options.search.ncpus;
#ifdef NUMA
   const int offset = options.search.affinity_offset;
#else
    const int offset = 0;
#endif
    hwloc_obj_t root = hwloc_get_root_obj(topo);
   if (hwloc_bitmap_weight(root->allowed_cpuset) < n + offset) {
      std::cerr << "warning: available CPUs < requested core count + offset (" <<
         n+offset << ")" << std::endl;
   }
   int count = 0;
   hwloc_bitmap_zero(set);
   accum(root,set,count,offset,n);
}


void Topology::cleanup()
{
   hwloc_topology_destroy(topo);
   hwloc_bitmap_free(set);
}

int Topology::init() {
   set = hwloc_bitmap_alloc();
   if (hwloc_topology_init(&topo)) {
      return 1;
   }
   return hwloc_topology_load(topo);
}


