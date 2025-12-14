#pragma once
#include "RoadMap.h"
#include <string>
#include <vector>

class TrafficOptimization {
public:
    TrafficOptimization(RoadMap& map);
    void optimizeTraffic();

private:
    RoadMap& map_;
};
