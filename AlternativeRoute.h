#pragma once
#include "RoadMap.h"
#include <string>

class AlternativeRoute {
public:
    AlternativeRoute(RoadMap& map);
    void suggestAlternative(const std::string& blockedEdgeId,
                            const std::string& start,
                            const std::string& goal);
private:
    RoadMap& map_;
};
