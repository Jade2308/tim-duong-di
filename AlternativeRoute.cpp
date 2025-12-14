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

    if (t < 0) cout << "❌ Không có tuyến thay thế.\n";
    else {
        cout << "✔ Tuyến thay thế: ";
        for (auto &p : path) cout << p << " ";
        cout << "\nThời gian: " << t << "\n";
    }

    map_.unblockAll();
}
