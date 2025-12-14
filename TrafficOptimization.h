#pragma once
#include "RoadMap.h"
#include <string>
#include <vector>

struct NewRoadProposal {
    std::string srcNode;
    std::string dstNode;
    double estimatedCost;
    double trafficReduction;
    double travelTimeSaved;
    std::string reasoning;
};

class TrafficOptimization {
public:
    TrafficOptimization(RoadMap& map);
    void optimizeTraffic();

private:
    RoadMap& map_;
    
    // Helper methods
    double analyzeNodeCongestion(const std::string& nodeId);
    std::vector<NewRoadProposal> findPotentialNewRoads(const std::string& congestedNode, double budget);
    NewRoadProposal selectBestProposal(const std::vector<NewRoadProposal>& proposals);
    void displayProposal(const NewRoadProposal& proposal, const std::string& congestedNode, double currentFlow, double capacity);
};
