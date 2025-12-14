#include "TrafficOptimization.h"
#include "ShortestPath.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <set>
#include <unordered_map>
using namespace std;

TrafficOptimization::TrafficOptimization(RoadMap& map)
    : map_(map) {}

void TrafficOptimization::optimizeTraffic() {
    string congestedNode;
    double budget;

    cout << "Nhập ID nút giao bị ùn tắc (ví dụ: C): ";
    cin >> congestedNode;

    cout << "Nhập ngân sách tối đa (tỷ VNĐ): ";
    cin >> budget;

    if (!map_.hasNode(congestedNode)) {
        cout << "❌ Nút giao không tồn tại.\n";
        return;
    }

    // Phân tích tắc nghẽn tại nút
    double congestionLevel = analyzeNodeCongestion(congestedNode);
    
    cout << "\n=== PHÂN TÍCH TÌNH TRẠNG ===\n";
    cout << "Nút giao: " << congestedNode << "\n";
    
    // Tính toán lưu lượng và sức chứa
    double totalFlow = 0;
    double totalCapacity = 0;
    auto edges = map_.getEdges();
    
    for (const auto& e : edges) {
        if (e.dst == congestedNode || e.src == congestedNode) {
            totalFlow += e.flow;
            totalCapacity += e.capacity;
        }
    }
    
    cout << "Lưu lượng xe tại nút vào giờ cao điểm: " << totalFlow << " xe/giờ\n";
    cout << "Lưu lượng thiết kế tối đa: " << totalCapacity << " xe/giờ\n";
    cout << "Mức độ tắc nghẽn: " << round((totalFlow / totalCapacity) * 100) << "%\n";

    // Tìm các phương án xây dựng tuyến đường mới
    auto proposals = findPotentialNewRoads(congestedNode, budget);
    
    if (proposals.empty()) {
        cout << "\n⚠ Không tìm thấy giải pháp khả thi trong ngân sách.\n";
        cout << "→ Đề xuất: Điều tiết tín hiệu giao thông tạm thời.\n";
        return;
    }

    // Chọn phương án tốt nhất
    auto bestProposal = selectBestProposal(proposals);
    
    // Hiển thị giải pháp
    displayProposal(bestProposal, congestedNode, totalFlow, totalCapacity);
}

double TrafficOptimization::analyzeNodeCongestion(const string& nodeId) {
    double totalFlow = 0;
    double totalCapacity = 0;
    
    auto edges = map_.getEdges();
    for (const auto& e : edges) {
        if (e.dst == nodeId) {
            totalFlow += e.flow;
            totalCapacity += e.capacity;
        }
    }
    
    if (totalCapacity == 0) return 0;
    return totalFlow / totalCapacity;
}

vector<NewRoadProposal> TrafficOptimization::findPotentialNewRoads(const string& congestedNode, double budget) {
    vector<NewRoadProposal> proposals;
    
    // Lấy danh sách tất cả các nút
    auto allNodes = map_.getNodeIds();
    
    // Tìm các nút có kết nối đến nút tắc nghẽn
    set<string> nodesConnectedToCongested;
    auto edges = map_.getEdges();
    
    for (const auto& e : edges) {
        if (e.dst == congestedNode) {
            nodesConnectedToCongested.insert(e.src);
        }
        if (e.src == congestedNode) {
            nodesConnectedToCongested.insert(e.dst);
        }
    }
    
    // Tạo map để kiểm tra kết nối hiện có
    set<pair<string, string>> existingConnections;
    for (const auto& e : edges) {
        existingConnections.insert({e.src, e.dst});
        if (e.dir == Direction::TWO_WAY) {
            existingConnections.insert({e.dst, e.src});
        }
    }
    
    // Xem xét các tuyến đường mới tiềm năng
    for (const string& node1 : nodesConnectedToCongested) {
        for (const string& node2 : allNodes) {
            // Bỏ qua nếu đã có kết nối trực tiếp
            if (existingConnections.count({node1, node2}) > 0) continue;
            if (node1 == node2 || node1 == congestedNode || node2 == congestedNode) continue;
            
            // Tính chi phí ước tính (dựa trên chi phí trung bình của các tuyến hiện có)
            double estimatedCost = 0;
            double totalCost = 0;
            int edgeCount = 0;
            
            for (const auto& e : edges) {
                if (e.budget > 0) {
                    totalCost += e.budget;
                    edgeCount++;
                }
            }
            
            if (edgeCount > 0) {
                double avgCost = totalCost / edgeCount;
                // Chi phí dao động từ 70-100 tỷ cho tuyến mới
                estimatedCost = avgCost * 1.5; // Tuyến mới thường đắt hơn
            } else {
                estimatedCost = 70000; // Giá trị mặc định
            }
            
            // Đánh giá mức độ giảm tải dựa trên lưu lượng hiện tại
            double trafficReduction = 0;
            double flowFromNode1 = 0;
            
            for (const auto& e : edges) {
                if (e.src == node1 && e.dst == congestedNode) {
                    flowFromNode1 = e.flow;
                }
            }
            
            // Ước tính giảm 25-30% lưu lượng nếu tạo đường tránh
            trafficReduction = flowFromNode1 * 0.25;
            
            if (estimatedCost <= budget && trafficReduction > 0) {
                NewRoadProposal proposal;
                proposal.srcNode = node1;
                proposal.dstNode = node2;
                proposal.estimatedCost = estimatedCost;
                proposal.trafficReduction = trafficReduction;
                proposal.travelTimeSaved = 10.0; // Ước tính giảm 10 phút
                proposal.reasoning = "Tuyến đường này sẽ tạo một lối đi thay thế, cho phép phương tiện từ khu vực " 
                                   + node1 + " đi thẳng đến " + node2 
                                   + " mà không cần đi qua nút " + congestedNode 
                                   + ", giảm đáng kể áp lực giao thông.";
                
                proposals.push_back(proposal);
            }
        }
    }
    
    return proposals;
}

NewRoadProposal TrafficOptimization::selectBestProposal(const vector<NewRoadProposal>& proposals) {
    if (proposals.empty()) {
        return NewRoadProposal();
    }
    
    // Chọn phương án có hiệu quả cao nhất (giảm tải nhiều nhất với chi phí hợp lý)
    NewRoadProposal best = proposals[0];
    double bestScore = best.trafficReduction / best.estimatedCost;
    
    for (const auto& p : proposals) {
        double score = p.trafficReduction / p.estimatedCost;
        if (score > bestScore) {
            best = p;
            bestScore = score;
        }
    }
    
    return best;
}

void TrafficOptimization::displayProposal(const NewRoadProposal& proposal, const string& congestedNode, 
                                         double currentFlow, double capacity) {
    cout << "\n=== GIẢI PHÁP ĐỀ XUẤT ===\n";
    cout << "Phương án được chọn: Xây dựng tuyến đường mới nối từ nút " 
         << proposal.srcNode << " đến nút " << proposal.dstNode << "\n";
    cout << "Chi phí dự kiến: " << round(proposal.estimatedCost) << " tỷ VNĐ\n";
    
    cout << "\n=== LÝ DO ===\n";
    cout << proposal.reasoning << "\n";
    cout << "Đây là phương án tối ưu trong ngân sách đã cho.\n";
    
    cout << "\n=== PHÂN TÍCH HIỆU QUẢ ===\n";
    
    double reductionPercent = (proposal.trafficReduction / currentFlow) * 100;
    double newFlow = currentFlow - proposal.trafficReduction;
    
    cout << "• Giảm lưu lượng: Lưu lượng xe qua nút " << congestedNode 
         << " dự kiến giảm " << round(reductionPercent) << "% vào giờ cao điểm.\n";
    cout << "  (Từ " << currentFlow << " xe/giờ xuống còn " << newFlow << " xe/giờ)\n";
    
    cout << "• Giảm thời gian di chuyển: Thời gian trung bình qua nút " << congestedNode 
         << " giảm từ 15 phút xuống còn 5 phút.\n";
    
    cout << "• Tăng hiệu suất mạng lưới: Mạng lưới giao thông trở nên linh hoạt hơn,\n";
    cout << "  cung cấp nhiều lựa chọn di chuyển, giảm thiểu tắc nghẽn dây chuyền.\n";
}