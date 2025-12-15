#pragma once
#include "RoadMap.h"
#include <string>
#include <vector>

struct NewRoadProposal {
    std::string srcNode;
    std::string dstNode;
    std::string intermediateNode;  // For two-segment routes
    double estimatedCost;
    double trafficReduction;
    double travelTimeSaved;
    std::string reasoning;
    bool isTwoSegment = false;
};

class TrafficOptimization {
public:
    TrafficOptimization(RoadMap& map);
    void optimizeTraffic();

private:
    RoadMap& map_;
    
    // Helper methods
    double analyzeNodeCongestion(const std::string& nodeId);
    std::vector<NewRoadProposal> findPotentialNewRoads(const Edge& congestedEdge, double budget);
    NewRoadProposal selectBestProposal(const std::vector<NewRoadProposal>& proposals);
    void displayProposal(const NewRoadProposal& proposal, const Edge& congestedEdge);
    double estimateMinimumBudget(const Edge& congestedEdge);
    void displayTrafficSignalSolution(const Edge& congestedEdge);
};
