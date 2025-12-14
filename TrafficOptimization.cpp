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
    string congestedEdgeId;
    double budget;

    cout << "Nhập ID tuyến đường bị ùn tắc (ví dụ: E01): ";
    cin >> congestedEdgeId;

    cout << "Nhập ngân sách tối đa (tỷ VNĐ): ";
    cin >> budget;

    if (!map_.hasEdge(congestedEdgeId)) {
        cout << "❌ Tuyến đường không tồn tại.\n";
        return;
    }
    
    // Lấy thông tin edge
    auto edgePtr = map_.getEdgeById(congestedEdgeId);
    if (!edgePtr) {
        cout << "❌ Không thể lấy thông tin tuyến đường.\n";
        return;
    }
    
    Edge& congestedEdge = *edgePtr;
    
    cout << "\n=== PHÂN TÍCH TÌNH TRẠNG ===\n";
    cout << "Tuyến đường ùn tắc: " << congestedEdge.id << " (" << congestedEdge.name << ")\n";
    cout << "Chiều: " << congestedEdge.src << " → " << congestedEdge.dst << "\n";
    cout << "Lưu lượng xe hiện tại: " << congestedEdge.flow << " xe/giờ\n";
    cout << "Sức chứa thiết kế: " << congestedEdge.capacity << " xe/giờ\n";
    
    // Tính mức độ quá tải
    if (congestedEdge.capacity > 0) {
        double congestionPercent = (congestedEdge.flow / congestedEdge.capacity) * 100;
        cout << "Mức độ quá tải: " << round(congestionPercent) << "%\n";
        
        if (congestionPercent > 100) {
            cout << "⚠️  CẢNH BÁO: Tuyến đường đang quá tải " << round(congestionPercent - 100) << "% so với thiết kế!\n";
        } else if (congestionPercent > 80) {
            cout << "⚠️  Tuyến đường đang trong tình trạng gần quá tải.\n";
        }
    } else {
        cout << "Mức độ quá tải: Không xác định (sức chứa = 0)\n";
    }

    // Tìm các phương án xây dựng tuyến đường mới
    auto proposals = findPotentialNewRoads(congestedEdge, budget);
    
    if (proposals.empty()) {
        // Tính ngân sách tối thiểu cần thiết
        double minBudgetNeeded = estimateMinimumBudget(congestedEdge);
        cout << "\n=== VẤN ĐÈ ===\n";
        cout << "⚠ Không tìm thấy giải pháp khả thi trong ngân sách " << budget << " tỷ VNĐ.\n";
        cout << "💰 Ngân sách tối thiểu cần thiết để xây tuyến đường mới: " 
             << round(minBudgetNeeded) << " tỷ VNĐ\n";
        cout << "📊 Thiếu hụt ngân sách: " << round(minBudgetNeeded - budget) << " tỷ VNĐ\n";
        
        cout << "\n=== GIẢI PHÁP THAY THẾ (KHÔNG CẦN NGÂN SÁCH) ===\n";
        displayTrafficSignalSolution(congestedEdge);
        return;
    }

    // Chọn phương án tốt nhất
    auto bestProposal = selectBestProposal(proposals);
    
    // Hiển thị giải pháp
    displayProposal(bestProposal, congestedEdge);
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

vector<NewRoadProposal> TrafficOptimization::findPotentialNewRoads(const Edge& congestedEdge, double budget) {
    vector<NewRoadProposal> proposals;
    
    string srcNode = congestedEdge.src;
    string dstNode = congestedEdge.dst;
    
    // Lấy danh sách tất cả các nút
    auto allNodes = map_.getNodeIds();
    auto edges = map_.getEdges();
    
    // Tạo map để kiểm tra kết nối hiện có
    set<pair<string, string>> existingConnections;
    for (const auto& e : edges) {
        existingConnections.insert({e.src, e.dst});
        if (e.dir == Direction::TWO_WAY) {
            existingConnections.insert({e.dst, e.src});
        }
    }
    
    // Tìm các nút trung gian giữa src và dst để tạo đường tránh
    // Chiến lược 1: Tìm đường tránh song song (bypass)
    for (const string& intermediateNode : allNodes) {
        if (intermediateNode == srcNode || intermediateNode == dstNode) continue;
        
        // Kiểm tra xem có thể tạo đường từ srcNode -> intermediateNode -> dstNode không
        bool canConnectFromSrc = existingConnections.count({srcNode, intermediateNode}) == 0;
        bool canConnectToDst = existingConnections.count({intermediateNode, dstNode}) == 0;
        
        if (canConnectFromSrc || canConnectToDst) {
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
            
            // Ước tính lưu lượng có thể chuyển hướng (30-50% lưu lượng hiện tại)
            double potentialRedirectedFlow = congestedEdge.flow * 0.4;
            
            if (estimatedCost <= budget && potentialRedirectedFlow > 0) {
                NewRoadProposal proposal;
                
                // Đề xuất tuyến đường mới có khả năng giảm tải cao nhất
                if (canConnectFromSrc && canConnectToDst) {
                    // Tạo 2 tuyến đường mới qua nút trung gian
                    proposal.srcNode = srcNode;
                    proposal.dstNode = intermediateNode;
                    proposal.estimatedCost = estimatedCost * 2; // 2 đoạn đường
                    proposal.trafficReduction = potentialRedirectedFlow * 0.6;
                    proposal.travelTimeSaved = ESTIMATED_TIME_SAVINGS_MINUTES;
                    proposal.reasoning = "Xây dựng tuyến đường vòng qua nút " + intermediateNode 
                                       + " để phân tán lưu lượng xe. Tạo 2 đoạn: " + srcNode + "→" + intermediateNode 
                                       + " và " + intermediateNode + "→" + dstNode + ".";
                    proposal.intermediateNode = intermediateNode;
                    proposal.isTwoSegment = true;
                } else if (canConnectFromSrc) {
                    proposal.srcNode = srcNode;
                    proposal.dstNode = intermediateNode;
                    proposal.estimatedCost = estimatedCost;
                    proposal.trafficReduction = potentialRedirectedFlow * 0.3;
                    proposal.travelTimeSaved = ESTIMATED_TIME_SAVINGS_MINUTES * 0.7;
                    proposal.reasoning = "Xây dựng tuyến đường từ " + srcNode + " đến " + intermediateNode 
                                       + " để tạo lối đi thay thế, giảm áp lực trên tuyến " + congestedEdge.id + ".";
                } else {
                    proposal.srcNode = intermediateNode;
                    proposal.dstNode = dstNode;
                    proposal.estimatedCost = estimatedCost;
                    proposal.trafficReduction = potentialRedirectedFlow * 0.3;
                    proposal.travelTimeSaved = ESTIMATED_TIME_SAVINGS_MINUTES * 0.7;
                    proposal.reasoning = "Xây dựng tuyến đường từ " + intermediateNode + " đến " + dstNode 
                                       + " để tạo lối đi thay thế, giảm áp lực trên tuyến " + congestedEdge.id + ".";
                }
                
                if (proposal.estimatedCost <= budget) {
                    proposals.push_back(proposal);
                }
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


void TrafficOptimization::displayProposal(const NewRoadProposal& proposal, const Edge& congestedEdge) {
    cout << "\n=== GIẢI PHÁP ĐỀ XUẤT ===\n";
    
    if (proposal.isTwoSegment) {
        cout << "Phương án được chọn: Xây dựng tuyến đường vòng qua nút " << proposal.intermediateNode << "\n";
        cout << "  - Đoạn 1: " << proposal.srcNode << " → " << proposal.intermediateNode << "\n";
        cout << "  - Đoạn 2: " << proposal.intermediateNode << " → " << proposal.dstNode << "\n";
    } else {
        cout << "Phương án được chọn: Xây dựng tuyến đường mới\n";
        cout << "  - Chiều: " << proposal.srcNode << " → " << proposal.dstNode << "\n";
    }
    
    cout << "Chi phí dự kiến: " << round(proposal.estimatedCost) << " tỷ VNĐ\n";
    
    cout << "\n=== LÝ DO ===\n";
    cout << proposal.reasoning << "\n";
    
    cout << "\n=== PHÂN TÍCH HIỆU QUẢ ===\n";
    
    if (congestedEdge.flow > 0) {
        double reductionPercent = (proposal.trafficReduction / congestedEdge.flow) * 100;
        double newFlow = congestedEdge.flow - proposal.trafficReduction;
        double newCongestionPercent = (newFlow / congestedEdge.capacity) * 100;
        
        cout << "• Giảm lưu lượng trên tuyến " << congestedEdge.id << ":\n";
        cout << "  - Giảm " << round(reductionPercent) << "% lưu lượng\n";
        cout << "  - Từ " << round(congestedEdge.flow) << " xe/giờ xuống còn " << round(newFlow) << " xe/giờ\n";
        cout << "  - Mức độ quá tải mới: " << round(newCongestionPercent) << "%\n";
    }
    
    cout << "• Thông số tuyến đường mới đề xuất:\n";
    cout << "  - Lưu lượng xe dự kiến: " << round(proposal.trafficReduction) << " xe/giờ\n";
    cout << "  - Sức chứa thiết kế khuyến nghị: " << round(proposal.trafficReduction * 1.3) << " xe/giờ\n";
    
    if (proposal.isTwoSegment) {
        cout << "  - Chiều đi: Hai chiều (TWO_WAY)\n";
        cout << "  - Loại đường: Đường chính (MAIN_ROAD)\n";
    } else {
        cout << "  - Chiều đi: Một chiều (ONE_WAY)\n";
        cout << "  - Loại đường: Đường chính (MAIN_ROAD)\n";
    }
    
    cout << "\n• Lợi ích bổ sung:\n";
    cout << "  - Tăng độ linh hoạt mạng lưới giao thông\n";
    cout << "  - Giảm thời gian di chuyển trung bình " << round(proposal.travelTimeSaved) << " phút\n";
    cout << "  - Giảm thiểu nguy cơ tắc nghẽn dây chuyền\n";
}

double TrafficOptimization::estimateMinimumBudget(const Edge& /* congestedEdge */) {
    // Tính ngân sách trung bình của các edge hiện có
    double avgBudget = 0;
    int count = 0;
    auto edges = map_.getEdges();
    
    for (const auto& e : edges) {
        if (e.budget > 0 && !e.isReverse) {
            avgBudget += e.budget;
            count++;
        }
    }
    
    if (count > 0) {
        avgBudget = avgBudget / count;
        return avgBudget * NEW_ROAD_COST_MULTIPLIER;
    }
    
    return DEFAULT_NEW_ROAD_COST;
}

void TrafficOptimization::displayTrafficSignalSolution(const Edge& congestedEdge) {
    cout << "1. Điều chỉnh thời gian đèn tín hiệu:\n";
    cout << "   - Tăng thời gian đèn xanh cho hướng " << congestedEdge.src << " → " << congestedEdge.dst << "\n";
    
    // Tính toán thời gian đề xuất dựa trên mức độ quá tải
    if (congestedEdge.capacity > 0) {
        double congestionRatio = congestedEdge.flow / congestedEdge.capacity;
        int currentGreenTime = 60; // giả sử thời gian xanh hiện tại là 60 giây
        int recommendedGreenTime = (int)(currentGreenTime * congestionRatio * 1.2);
        
        cout << "   - Thời gian đèn xanh đề xuất: " << recommendedGreenTime << " giây (hiện tại: " << currentGreenTime << " giây)\n";
    }
    
    cout << "\n2. Điều tiết luồng giao thông:\n";
    cout << "   - Hạn chế xe tải nặng vào giờ cao điểm (7-9h và 17-19h)\n";
    cout << "   - Khuyến khích phân làn theo loại phương tiện\n";
    
    cout << "\n3. Quản lý tốc độ:\n";
    cout << "   - Tốc độ tối đa khuyến nghị: " << (int)(congestedEdge.avgSpeed * 0.8) << " km/h (để tăng độ an toàn)\n";
    cout << "   - Đặt biển báo tốc độ điện tử thông minh\n";
    
    cout << "\n4. Giám sát và điều phối:\n";
    cout << "   - Lắp đặt camera giám sát lưu lượng xe\n";
    cout << "   - Triển khai hệ thống điều khiển tín hiệu thích ứng (Adaptive Traffic Control)\n";
    
    cout << "\n5. Thông tin và cảnh báo:\n";
    cout << "   - Cảnh báo tài xế về tình trạng tắc đường qua ứng dụng di động\n";
    cout << "   - Đề xuất tuyến đường thay thế cho người dân\n";
    
    cout << "\n⏱️  Thời gian triển khai: 2-4 tuần\n";
    cout << "💰 Chi phí ước tính: 5-10 tỷ VNĐ (chủ yếu cho thiết bị và công nghệ)\n";
}
