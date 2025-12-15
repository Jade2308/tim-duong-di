#pragma once
#include "RoadMap.h"
#include <string>
#include <vector>

enum class ProposalType {
    NEW_ROAD,           // Build new road (budget >= road cost)
    EXPAND_LANES,       // Expand lanes (budget 1/3 to <1x road cost)
    DIRECT_BYPASS       // Direct bypass for adjacent congested roads
};

struct NewRoadProposal {
    std::string srcNode;
    std::string dstNode;
    std::string intermediateNode;  // For two-segment routes
    double estimatedCost;
    double trafficReduction;
    double travelTimeSaved;
    std::string reasoning;
    bool isTwoSegment = false;
    ProposalType type = ProposalType::NEW_ROAD;
    std::vector<std::string> congestedPath;  // For direct bypass of adjacent roads
};

struct CongestionInfo {
    std::string edgeId;
    std::string edgeName;
    std::string srcNode;
    std::string dstNode;
    double flow;
    double capacity;
    double congestionRatio;  // flow/capacity
    double overloadPercent;   // (flow-capacity)/capacity * 100
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
    
    // New methods for updated recommendation system
    bool isOverCapacity(const Edge& edge);
    std::vector<std::string> findAdjacentCongestedRoads(const Edge& startEdge);
    NewRoadProposal createExpandLanesProposal(const Edge& congestedEdge);
    NewRoadProposal createDirectBypassProposal(const std::vector<std::string>& congestedPath, double budget);
    
    // Automatic congestion detection methods
    std::vector<CongestionInfo> detectCongestedRoads();
    void displayCongestedRoadsList(const std::vector<CongestionInfo>& congestedRoads);
};
