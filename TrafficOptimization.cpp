#include "TrafficOptimization.h"
#include "ShortestPath.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <set>
#include <unordered_map>
using namespace std;

// Named constants for traffic optimization calculations
namespace {
    const double NEW_ROAD_COST_MULTIPLIER = 1.75;  // New roads cost ~1.75x average
    const double DEFAULT_NEW_ROAD_COST = 70000.0;   // Default: 700 billion VND
    const double INDIRECT_FLOW_REDIRECT_RATIO = 0.5; // 50% of indirect flow can redirect
    const double DIRECT_FLOW_REDIRECT_RATIO = 0.3;   // 30% of direct flow can redirect
    const double ESTIMATED_TIME_SAVINGS_MINUTES = 10.0; // Estimated travel time reduction
}

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
    
    cout << "\n=== PHÂN TÍCH TÌNH TRẠNG ===\n";
    cout << "Vị trí ùn tắc: Nút giao " << congestedNode << "\n";
    
    // Tính toán lưu lượng và sức chứa - tính tổng lưu lượng đi VÀO nút
    double totalIncomingFlow = 0;
    double totalIncomingCapacity = 0;
    auto edges = map_.getEdges();
    
    for (const auto& e : edges) {
        // Chỉ tính các edge đi VÀO nút tắc nghẽn
        if (e.dst == congestedNode && !e.isReverse) {
            totalIncomingFlow += e.flow;
            totalIncomingCapacity += e.capacity;
        }
    }
    
    cout << "Lưu lượng xe tại nút " << congestedNode << " vào giờ cao điểm: " 
         << totalIncomingFlow << " xe/giờ\n";
    cout << "Lưu lượng thiết kế tối đa của nút " << congestedNode << ": " 
         << totalIncomingCapacity << " xe/giờ\n";
    
    // Guard against division by zero
    if (totalIncomingCapacity > 0) {
        double congestionPercent = (totalIncomingFlow / totalIncomingCapacity) * 100;
        cout << "Mức độ quá tải: " << round(congestionPercent) << "%\n";
    } else {
        cout << "Mức độ quá tải: Không xác định (sức chứa = 0)\n";
    }

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
    displayProposal(bestProposal, congestedNode, totalIncomingFlow, totalIncomingCapacity);
}

double TrafficOptimization::analyzeNodeCongestion(const string& nodeId) {
    double totalFlow = 0;
    double totalCapacity = 0;
    
    auto edges = map_.getEdges();
    for (const auto& e : edges) {
        if (e.dst == nodeId && !e.isReverse) {
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
    
    // Tìm các nút có kết nối đến nút tắc nghẽn (upstream nodes)
    set<string> upstreamNodes;
    auto edges = map_.getEdges();
    
    for (const auto& e : edges) {
        if (e.dst == congestedNode) {
            upstreamNodes.insert(e.src);
        }
    }
    
    // Tìm các nút có thể đến được từ nút tắc nghẽn (downstream nodes) 
    set<string> downstreamNodes;
    for (const auto& e : edges) {
        if (e.src == congestedNode) {
            downstreamNodes.insert(e.dst);
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
    
    // Xem xét các tuyến đường mới tiềm năng: kết nối upstream nodes đến downstream nodes
    // để tạo đường tránh (bypass)
    for (const string& srcNode : upstreamNodes) {
        for (const string& dstNode : downstreamNodes) {
            // Bỏ qua nếu đã có kết nối trực tiếp
            if (existingConnections.count({srcNode, dstNode}) > 0) continue;
            if (srcNode == dstNode) continue;
            
            // Tìm các nút upstream của srcNode để tính lưu lượng có thể chuyển hướng
            set<string> secondLevelUpstream;
            for (const auto& e : edges) {
                if (e.dst == srcNode) {
                    secondLevelUpstream.insert(e.src);
                }
            }
            
            // Tính chi phí ước tính dựa trên khoảng cách và loại đường
            double estimatedCost = DEFAULT_NEW_ROAD_COST;
            double avgBudgetPerEdge = 0;
            int edgeCount = 0;
            
            for (const auto& e : edges) {
                if (e.budget > 0 && !e.isReverse) {
                    avgBudgetPerEdge += e.budget;
                    edgeCount++;
                }
            }
            
            if (edgeCount > 0) {
                avgBudgetPerEdge = avgBudgetPerEdge / edgeCount;
                estimatedCost = avgBudgetPerEdge * NEW_ROAD_COST_MULTIPLIER;
            }
            
            // Ước tính lưu lượng có thể chuyển hướng
            double potentialRedirectedFlow = 0;
            
            // Tính lưu lượng từ các nút có thể được chuyển hướng
            for (const string& upstreamNode2 : secondLevelUpstream) {
                for (const auto& e : edges) {
                    if (e.src == upstreamNode2 && e.dst == srcNode) {
                        potentialRedirectedFlow += e.flow * INDIRECT_FLOW_REDIRECT_RATIO;
                    }
                }
            }
            
            // Thêm lưu lượng trực tiếp từ srcNode đến congestedNode
            for (const auto& e : edges) {
                if (e.src == srcNode && e.dst == congestedNode) {
                    potentialRedirectedFlow += e.flow * DIRECT_FLOW_REDIRECT_RATIO;
                }
            }
            
            if (estimatedCost <= budget && potentialRedirectedFlow > 0) {
                NewRoadProposal proposal;
                proposal.srcNode = srcNode;
                proposal.dstNode = dstNode;
                proposal.estimatedCost = estimatedCost;
                proposal.trafficReduction = potentialRedirectedFlow;
                proposal.travelTimeSaved = ESTIMATED_TIME_SAVINGS_MINUTES;
                
                proposal.reasoning = "Tuyến đường này sẽ tạo một lối đi thay thế, cho phép phương tiện từ khu vực " 
                                   + srcNode + " và các khu vực lân cận đi thẳng đến " + dstNode 
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
    
    // Chọn phương án có hiệu quả cao nhất (giảm tải nhiều nhất)
    NewRoadProposal best = proposals[0];
    double bestReduction = best.trafficReduction;
    
    for (const auto& p : proposals) {
        // Ưu tiên phương án giảm tải nhiều nhất trong ngân sách
        if (p.trafficReduction > bestReduction) {
            best = p;
            bestReduction = p.trafficReduction;
        }
    }
    
    return best;
}

void TrafficOptimization::displayProposal(const NewRoadProposal& proposal, const string& congestedNode, 
                                         double currentFlow, double /* capacity */) {
    cout << "\n=== GIẢI PHÁP ĐỀ XUẤT ===\n";
    cout << "Phương án được chọn: Xây dựng tuyến đường mới nối từ nút " 
         << proposal.srcNode << " đến nút " << proposal.dstNode << "\n";
    cout << "Chi phí dự kiến: " << round(proposal.estimatedCost) << " tỷ VNĐ\n";
    
    cout << "\n=== LÝ DO ===\n";
    cout << proposal.reasoning << "\n";
    cout << "Đây là phương án tối ưu trong ngân sách " << round(proposal.estimatedCost) << " tỷ VNĐ.\n";
    
    cout << "\n=== PHÂN TÍCH HIỆU QUẢ ===\n";
    
    if (currentFlow > 0) {
        double reductionPercent = (proposal.trafficReduction / currentFlow) * 100;
        double newFlow = currentFlow - proposal.trafficReduction;
        
        cout << "• Giảm lưu lượng: Lưu lượng xe qua nút " << congestedNode 
             << " dự kiến giảm " << round(reductionPercent) << "% vào giờ cao điểm.\n";
        cout << "  (Từ " << round(currentFlow) << " xe/giờ xuống còn " << round(newFlow) << " xe/giờ)\n";
    }
    
    cout << "• Giảm thời gian di chuyển: Thời gian trung bình qua nút " << congestedNode 
         << " giảm từ 15 phút xuống còn 5 phút.\n";
    
    cout << "• Tăng hiệu suất mạng lưới: Mạng lưới giao thông trở nên linh hoạt hơn,\n";
    cout << "  cung cấp nhiều lựa chọn di chuyển, giảm thiểu tắc nghẽn dây chuyền.\n";
}