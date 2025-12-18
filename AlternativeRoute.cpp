#include "AlternativeRoute.h"
#include "ShortestPath.h"
#include <iostream>

using namespace std;

AlternativeRoute::AlternativeRoute(RoadMap& map)
    : map_(map) {}

void AlternativeRoute::suggestAlternative(const string& blockedEdgeId,
                                          const string& start,
                                          const string& goal) {

    map_.blockEdge(blockedEdgeId);

    vector<string> path;
    ShortestPath sp(map_);
    double t = sp.findShortestPath(start, goal, path);

    if (t < 0) {
        cout << "❌ Không có tuyến thay thế.\n";
    } else {
        cout << "✅ TUYẾN ĐƯỜNG THAY THẾ ĐÃ TÌM THẤY:\n";
        cout << "   Tuyến đường: ";
        for (size_t i = 0; i < path.size(); ++i) {
            cout << path[i];
            if (i < path.size() - 1) cout << " -> ";
        }
        cout << "\n   Thời gian di chuyển: " << t << " đơn vị thời gian.\n";
    }

    map_.unblockAll();
}

AlternativeRouteResult AlternativeRoute::findAlternativeRoute(const string& blockedEdgeId,
                                                               const string& start,
                                                               const string& goal) {
    AlternativeRouteResult result;
    result.blockedEdgeId = blockedEdgeId;
    result.success = false;
    result.travelTime = -1;
    
    // Chặn edge
    if (! map_.blockEdge(blockedEdgeId)) {
        result.errorMessage = "Không thể chặn edge " + blockedEdgeId;
        return result;
    }
    
    // Tìm đường thay thế
    ShortestPath sp(map_);
    double t = sp.findShortestPath(start, goal, result.path);
    
    // Bỏ chặn edge
    map_.unblockAll();
    
    if (t < 0) {
        result.success = false;
        result.errorMessage = "Không tìm thấy tuyến đường thay thế";
    } else {
        result.success = true;
        result.travelTime = t;
    }
    
    return result;
}