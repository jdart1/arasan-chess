// Copyright 2026 by Jon Dart. All Rights Reserved.
#ifndef _THREATFEATURE_H
#define _THREATFEATURE_H

#include "nndefs.h"

// Stub: will hold weights and index computations for threat features
// (attacker/victim piece pairs). Implementation TBD.
template <size_t alignment = DEFAULT_ALIGN>
class ThreatFeature {

public:
    ThreatFeature() = default;

    virtual ~ThreatFeature() = default;
};

#endif
