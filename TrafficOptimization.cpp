#include "TrafficOptimization.h"
#include "ShortestPath.h"
#include <iostream>
#include <cmath>
#include <algorithm> // Thêm thư viện này
using namespace std;

TrafficOptimization::TrafficOptimization(RoadMap& map)
    : map_(map) {}

void TrafficOptimization::optimizeTraffic() {
    string edgeId;
    double budget;

    cout << "Nhập ID tuyến đường bị tắc: ";
    cin >> edgeId;

    cout << "Nhập ngân sách: ";
    cin >> budget;

    auto e = map_.getEdgeById(edgeId);
    if (!e) {
        cout << "Tuyến không tồn tại.\n";
        return;
    }

    double flow = e->flow;
    double cap = e->capacity;
    double ratio = flow / cap;

    cout << "\n--- PHÂN TÍCH ---\n";
    cout << "Flow: " << flow << "\n";
    cout << "Capacity: " << cap << "\n";
    cout << "Tắc nghẽn: " << round(ratio * 100) << "%\n";

    // 1) Thử phân luồng (Ưu tiên số 1)
    {
        ShortestPath sp(map_);
        vector<string> alt;
        // Tìm tuyến thay thế (nếu có)
        double t = sp.findShortestPath(e->src, e->dst, alt);

        if (t > 0 && alt.size() > 0) { // Đảm bảo có tuyến và có đường đi
            cout << "\n✔ Giải pháp: Phân luồng\n";
            cout << "Tuyến thay thế: ";
            for (auto &x : alt) cout << x << " ";
            cout << "\n→ Vì sao: miễn phí và giảm tải ngay, đặc biệt nếu tuyến thay thế đủ tốt.\n";
            return;
        }
    }

    // 2) Mở rộng đường (Ưu tiên số 2, chỉ khi phân luồng không hiệu quả)
    // Giảm tắc nghẽn xuống ngưỡng an toàn (ví dụ: ratio 85% = 0.85)
    double targetRatio = 0.85;
    double neededCapacity = flow / targetRatio;
    double deltaCapacity = neededCapacity - cap;

    if (deltaCapacity > 0) {
        // Giả định: costPerUnit = budget_hien_tai / capacity_hien_tai
        // Lưu ý: Trong thực tế, cần hàm tính chi phí chính xác hơn
        double costPerUnit = e->budget / e->capacity; 
        double costToExpand = costPerUnit * deltaCapacity;

        // Làm tròn chi phí để hiển thị đẹp hơn (làm tròn đến trăm gần nhất)
        costToExpand = round(costToExpand / 100.0) * 100.0; 

        if (costToExpand <= budget) {
            cout << "\n✔ Giải pháp: Mở rộng đường\n";
            cout << "Cần tăng thêm " << deltaCapacity << " capacity (đạt tỉ lệ " << targetRatio * 100 << "%)\n";
            cout << "Chi phí ước tính: " << costToExpand << " (VND)\n";
            cout << "→ Vì sao: giải quyết tắc nghẽn xuống mức an toàn trong ngân sách.\n";
            return;
        }
    }


    // 3) Xây tuyến mới (Ưu tiên số 3)
    // Chi phí giả định cho tuyến mới (80,000 ~ 120,000)
    double costNewRoadMin = 80000;
    double costNewRoadMax = 120000;

    if (budget >= costNewRoadMin) {
        cout << "\n✔ Giải pháp: Xây tuyến mới\n";
        cout << "Chi phí: " << costNewRoadMin << " ~ " << costNewRoadMax << " (VND)\n";
        cout << "→ Vì sao: ngân sách đủ lớn để xây dựng một tuyến đường thay thế, giúp phân luồng dài hạn.\n";
        return;
    }

    // 4) Giải pháp cuối (Khi tất cả giải pháp trên đều không khả thi)
    cout << "\n⚠ Ngân sách thấp/Giải pháp khác quá tốn kém, chọn: Điều tiết tín hiệu\n";
    cout << "→ Vì sao: là giải pháp tạm thời, không cần ngân sách lớn, giúp giảm tắc nghẽn ngắn hạn (10-15 phút).\n";
}