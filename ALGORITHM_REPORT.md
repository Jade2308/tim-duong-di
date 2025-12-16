# BÁO CÁO THUẬT TOÁN HỆ THỐNG PHÂN TÍCH BẢN ĐỒ GIAO THÔNG

## 📋 Mục Lục
1. [Tổng Quan Hệ Thống](#tổng-quan-hệ-thống)
2. [Danh Sách Thuật Toán](#danh-sách-thuật-toán)
3. [Chi Tiết Thuật Toán](#chi-tiết-thuật-toán)
4. [Phân Tích Độ Phức Tạp](#phân-tích-độ-phức-tạp)
5. [Cấu Trúc Dữ Liệu](#cấu-trúc-dữ-liệu)

---

## 1. Tổng Quan Hệ Thống

### 1.1 Mô Tả
Hệ thống Phân Tích Bản Đồ Giao Thông là một ứng dụng phân tích và tối ưu hóa giao thông đô thị. Hệ thống sử dụng các thuật toán đồ thị và kỹ thuật tối ưu hóa để:
- Tìm đường đi ngắn nhất giữa hai điểm
- Gợi ý tuyến đường thay thế khi có đoạn đường bị chặn
- Phát hiện và tối ưu hóa các tuyến đường bị ùn tắc
- Đề xuất các giải pháp xây dựng cơ sở hạ tầng

### 1.2 Kiến Trúc
```
┌─────────────────────────────────────────────────────────────┐
│                    Lớp Giao Diện (UI)                       │
│              main.cpp / main_gui.cpp                        │
└────────────────────┬────────────────────────────────────────┘
                     │
        ┌────────────┴────────────┬──────────────┬────────────┐
        │                         │              │            │
┌───────▼─────────┐   ┌──────────▼────────┐  ┌─▼──────────┐  │
│ ShortestPath    │   │ AlternativeRoute  │  │ Traffic    │  │
│  (Dijkstra)     │   │   (Dijkstra +     │  │Optimization│  │
│                 │   │    Blocking)      │  │            │  │
└───────┬─────────┘   └──────────┬────────┘  └─┬──────────┘  │
        │                        │              │             │
        └────────────┬───────────┴──────────────┴─────────────┘
                     │
           ┌─────────▼──────────┐
           │     RoadMap        │
           │  (Cấu trúc đồ thị) │
           └────────────────────┘
```

### 1.3 Đặc Điểm Chính
- **Ngôn ngữ**: C++17
- **Thư viện đồ họa**: SDL2 (cho phiên bản GUI)
- **Cấu trúc dữ liệu**: Đồ thị có hướng có trọng số
- **Thuật toán chính**: Dijkstra, DFS/BFS, Greedy Algorithm

---

## 2. Danh Sách Thuật Toán

### 2.1 Thuật Toán Cốt Lõi

| STT | Tên Thuật Toán | File | Mục đích | Độ phức tạp |
|-----|----------------|------|----------|-------------|
| 1 | **Dijkstra** | ShortestPath.cpp | Tìm đường đi ngắn nhất | O((V+E) log V) |
| 2 | **Alternative Route Finding** | AlternativeRoute.cpp | Tìm đường thay thế | O((V+E) log V) |
| 3 | **Traffic Optimization** | TrafficOptimization.cpp | Tối ưu giao thông | O(E²) |
| 4 | **Congestion Detection** | TrafficOptimization.cpp | Phát hiện ùn tắc | O(E) |
| 5 | **Bypass Route Generation** | TrafficOptimization.cpp | Tạo đường vòng | O(V × E) |

Trong đó:
- V = Số lượng nodes (điểm giao lộ)
- E = Số lượng edges (đoạn đường)

### 2.2 Thuật Toán Hỗ Trợ

| STT | Tên | Chức năng | Độ phức tạp |
|-----|-----|-----------|-------------|
| 1 | **Budget Gap Calculation** | Tính thiếu hụt ngân sách | O(1) |
| 2 | **Proposal Selection** | Chọn phương án tốt nhất | O(n) |
| 3 | **Capacity Check** | Kiểm tra quá tải | O(1) |
| 4 | **Adjacent Road Finding** | Tìm đường liền kề tắc | O(E) |

---

## 3. Chi Tiết Thuật Toán

### 3.1 Thuật Toán Dijkstra (Shortest Path)

**File**: `ShortestPath.cpp`

**Mục đích**: Tìm đường đi ngắn nhất từ điểm bắt đầu đến điểm đích dựa trên thời gian di chuyển.

**Đầu vào**:
- `start`: Node bắt đầu (string)
- `goal`: Node đích (string)
- `map`: Đồ thị giao thông

**Đầu ra**:
- `outPath`: Vector chứa đường đi (vector<string>)
- Thời gian di chuyển (double)

**Thuật toán**:
```
1. Khởi tạo:
   - dist[v] = ∞ cho tất cả nodes
   - dist[start] = 0
   - priority_queue pq (min-heap)

2. Thêm (0, start) vào pq

3. While pq không rỗng:
   a. Lấy (d, u) từ pq
   b. Nếu d > dist[u]: skip
   c. For mỗi edge (u, v):
      - w = travelTime(edge)
      - Nếu dist[v] > dist[u] + w:
        * dist[v] = dist[u] + w
        * parent[v] = u
        * Thêm (dist[v], v) vào pq

4. Truy vết đường đi từ goal về start qua parent
5. Đảo ngược đường đi
6. Trả về dist[goal] và outPath
```

**Trọng số**: `travelTime = length / speed`

**Đặc điểm**:
- Sử dụng priority_queue (min-heap) để luôn chọn node có khoảng cách nhỏ nhất
- Xử lý đồ thị có hướng với trọng số dương
- An toàn với edges không tồn tại trong parent map

**Độ phức tạp**:
- **Thời gian**: O((V + E) log V)
  - V lần pop từ priority queue: O(V log V)
  - E lần cập nhật dist và push: O(E log V)
- **Không gian**: O(V) cho dist, parent, và priority queue

---

### 3.2 Thuật Toán Alternative Route (Tìm Đường Thay Thế)

**File**: `AlternativeRoute.cpp`

**Mục đích**: Tìm đường đi thay thế khi một đoạn đường bị chặn hoặc tắc nghẽn.

**Đầu vào**:
- `blockedEdgeId`: ID đoạn đường bị chặn
- `start`: Node bắt đầu
- `goal`: Node đích

**Đầu ra**:
- Đường đi thay thế (nếu có)
- Thời gian di chuyển mới

**Thuật toán**:
```
1. Đánh dấu edge bị chặn: map.blockEdge(blockedEdgeId)

2. Gọi thuật toán Dijkstra trên đồ thị đã bị chặn:
   - Dijkstra tự động bỏ qua edge bị block
   - Tìm đường đi từ start → goal

3. Nếu tìm thấy đường:
   - Hiển thị đường đi
   - Hiển thị thời gian
   Ngược lại:
   - Thông báo không có đường thay thế

4. Bỏ chặn tất cả edges: map.unblockAll()
```

**Đặc điểm**:
- Tái sử dụng thuật toán Dijkstra
- Thao tác block/unblock edge an toàn
- Có thể tìm nhiều đường thay thế bằng cách block nhiều edges

**Độ phức tạp**:
- **Thời gian**: O((V + E) log V) - giống Dijkstra
- **Không gian**: O(V + E)

---

### 3.3 Thuật Toán Traffic Optimization

**File**: `TrafficOptimization.cpp`

#### 3.3.1 Phát Hiện Ùn Tắc Tự Động (Automatic Congestion Detection)

**Mục đích**: Quét toàn bộ hệ thống để tìm các tuyến đường bị quá tải.

**Thuật toán**:
```
Function detectCongestedRoads():
    congestedRoads = []
    
    For each edge in map:
        If edge.flow > edge.capacity:
            congestionInfo = {
                edgeId: edge.id,
                edgeName: edge.name,
                srcNode: edge.src,
                dstNode: edge.dst,
                flow: edge.flow,
                capacity: edge.capacity,
                congestionRatio: flow / capacity,
                overloadPercent: ((flow - capacity) / capacity) × 100
            }
            congestedRoads.append(congestionInfo)
    
    Sort congestedRoads by congestionRatio (descending)
    Return congestedRoads
```

**Độ phức tạp**:
- **Thời gian**: O(E log E) - O(E) cho scan + O(E log E) cho sort
- **Không gian**: O(E)

---

#### 3.3.2 Tìm Đường Vòng (Bypass Route Generation)

**Mục đích**: Tạo các phương án đường vòng để giảm tải cho đường bị tắc.

**Thuật toán**:
```
Function findPotentialNewRoads(congestedEdge, budget):
    proposals = []
    
    // Kiểm tra quá tải
    If flow ≤ capacity:
        Return []
    
    roadCost = congestedEdge.budget
    src = congestedEdge.src
    dst = congestedEdge.dst
    
    // CẤP 1: Xây dựng đường mới (budget ≥ roadCost)
    If budget ≥ roadCost:
        For each intermediateNode in map:
            If node != src AND node != dst:
                // Kiểm tra có đường từ src → intermediate
                path1 = findShortestPath(src, intermediate)
                // Kiểm tra có đường từ intermediate → dst  
                path2 = findShortestPath(intermediate, dst)
                
                If path1 exists AND path2 exists:
                    proposal = createBypassProposal(
                        src, dst, intermediate,
                        type: NEW_ROAD
                    )
                    proposals.append(proposal)
        
        // Tìm chuỗi đường liền kề bị tắc
        adjacentCongested = findAdjacentCongestedRoads(congestedEdge)
        If adjacentCongested.length ≥ 2:
            totalCost = sum(edge.budget for edge in adjacentCongested)
            If budget > (2/3) × totalCost:
                proposal = createDirectBypassProposal(adjacentCongested)
                proposals.append(proposal)
    
    // CẤP 2: Mở rộng làn đường (roadCost/3 ≤ budget < roadCost)
    Else If budget ≥ roadCost/3 AND budget < roadCost:
        proposal = createExpandLanesProposal(congestedEdge)
        proposals.append(proposal)
    
    // CẤP 3: Giải pháp không xây dựng (budget < roadCost/3)
    Else:
        Return [] // Sẽ hiển thị giải pháp quản lý đèn tín hiệu
    
    Return proposals
```

**Ước tính chi phí và hiệu quả**:
```
NEW_ROAD:
- Chi phí = averageCost × 1.75
- Giảm lưu lượng: 30-60% tùy loại đường
- Thời gian tiết kiệm: ~10 phút

EXPAND_LANES:
- Chi phí = roadCost × 0.5
- Tăng sức chứa: +40%
- Không giảm lưu lượng, nhưng tăng khả năng xử lý

DIRECT_BYPASS:
- Chi phí = tổng chi phí chuỗi đường × 0.9
- Giảm lưu lượng: ~50% tổng lưu lượng
- Giải pháp tối ưu cho chuỗi đường tắc liền kề
```

**Độ phức tạp**:
- **Thời gian**: O(V × (V + E) log V)
  - Loop qua V intermediate nodes
  - Mỗi lần gọi Dijkstra: O((V + E) log V)
- **Không gian**: O(V + E)

---

#### 3.3.3 Tìm Đường Liền Kề Bị Tắc

**Mục đích**: Phát hiện chuỗi các đường liền kề đều bị ùn tắc.

**Thuật toán**:
```
Function findAdjacentCongestedRoads(startEdge):
    path = [startEdge.id]
    currentNode = startEdge.dst
    visited = {startEdge.id}
    
    // Tìm đường tiếp theo từ dst của edge hiện tại
    While true:
        found = false
        For each edge in outgoingEdges(currentNode):
            If edge.id not in visited AND isOverCapacity(edge):
                path.append(edge.id)
                visited.add(edge.id)
                currentNode = edge.dst
                found = true
                Break
        
        If not found:
            Break
    
    Return path
```

**Ví dụ**: Nếu B→E, E→F, F→G đều bị tắc, thuật toán sẽ trả về [E02, E03, E04].

**Độ phức tạp**:
- **Thời gian**: O(E) - trong worst case duyệt tất cả edges
- **Không gian**: O(E)

---

#### 3.3.4 Lựa Chọn Phương Án Tốt Nhất

**Mục đích**: Chọn phương án có hiệu quả cao nhất từ danh sách đề xuất.

**Thuật toán**:
```
Function selectBestProposal(proposals):
    If proposals is empty:
        Return null
    
    bestProposal = proposals[0]
    bestScore = -∞
    
    For each proposal in proposals:
        // Tính điểm dựa trên:
        // 1. Giảm lưu lượng (traffic reduction)
        // 2. Chi phí (cost - càng thấp càng tốt)
        // 3. Thời gian tiết kiệm (time saved)
        
        score = (trafficReduction × 100) / estimatedCost + travelTimeSaved
        
        If score > bestScore:
            bestScore = score
            bestProposal = proposal
    
    Return bestProposal
```

**Công thức điểm**: 
```
score = (traffic_reduction_percent × 100) / cost_billions + time_saved_minutes
```

**Độ phức tạp**:
- **Thời gian**: O(n) với n = số phương án
- **Không gian**: O(1)

---

#### 3.3.5 Tính Ngân Sách Tối Thiểu

**Mục đích**: Ước tính ngân sách cần thiết khi không đủ ngân sách.

**Thuật toán**:
```
Function estimateMinimumBudget(congestedEdge):
    If congestedEdge.budget > 0:
        // Lấy 1/3 chi phí đường làm ngân sách tối thiểu
        // (để có thể mở rộng làn đường)
        Return congestedEdge.budget / 3.0
    Else:
        // Nếu không có thông tin, dùng giá trị mặc định
        Return DEFAULT_NEW_ROAD_COST / 3.0
```

**Độ phức tạp**: O(1)

---

### 3.4 Giải Pháp Quản Lý Đèn Tín Hiệu

**Mục đích**: Đề xuất giải pháp không cần xây dựng khi ngân sách không đủ.

**Các giải pháp**:

1. **Điều Chỉnh Thời Gian Đèn Tín Hiệu**
   - Tăng thời gian đèn xanh cho hướng chính
   - Chi phí: 5-10 tỷ VNĐ
   - Thời gian: 2-4 tuần

2. **Điều Tiết Luồng Giao Thông**
   - Hạn chế xe tải trong giờ cao điểm
   - Không cần ngân sách xây dựng
   - Chỉ cần biển báo và giám sát

3. **Quản Lý Tốc Độ**
   - Điều chỉnh giới hạn tốc độ
   - Cải thiện luồng giao thông đều đặn

4. **Giám Sát và Điều Phối**
   - Camera giám sát
   - Hệ thống điều khiển thích ứng

5. **Thông Tin và Cảnh Báo**
   - Ứng dụng di động
   - Bảng điện tử thông báo

---

## 4. Phân Tích Độ Phức Tạp

### 4.1 Bảng Tổng Hợp

| Thuật toán | Best Case | Average Case | Worst Case | Không gian |
|-----------|-----------|--------------|------------|------------|
| Dijkstra | O((V+E) log V) | O((V+E) log V) | O((V+E) log V) | O(V) |
| Alternative Route | O((V+E) log V) | O((V+E) log V) | O((V+E) log V) | O(V) |
| Congestion Detection | O(E) | O(E log E) | O(E log E) | O(E) |
| Bypass Generation | O(V² log V) | O(VE log V) | O(VE log V) | O(V+E) |
| Adjacent Road Finding | O(1) | O(E/2) | O(E) | O(E) |

### 4.2 Giải Thích

**Dijkstra - O((V+E) log V)**:
- Sử dụng min-heap (priority_queue)
- Mỗi node được pop 1 lần: V × log V
- Mỗi edge được relax tối đa 1 lần: E × log V
- Tổng: O((V+E) log V)

**Bypass Generation - O(V × (V+E) log V)**:
- Loop qua V intermediate nodes
- Với mỗi node, chạy 2 lần Dijkstra
- Mỗi Dijkstra: O((V+E) log V)
- Tổng: O(V × (V+E) log V)

---

## 5. Cấu Trúc Dữ Liệu

### 5.1 Node (Điểm Giao Lộ)

```cpp
struct Node {
    string id;          // ID duy nhất (ví dụ: "A", "B")
    string name;        // Tên node (ví dụ: "Ngã Tư Hàng Xanh")
    double latitude;    // Vĩ độ
    double longitude;   // Kinh độ
};
```

### 5.2 Edge (Đoạn Đường)

```cpp
struct Edge {
    string id;          // ID đoạn đường (ví dụ: "E01")
    string name;        // Tên đường (ví dụ: "Đường Nguyễn Huệ")
    double length;      // Độ dài (km)
    double speed;       // Tốc độ cho phép (km/h)
    double capacity;    // Sức chứa (xe/giờ)
    double flow;        // Lưu lượng hiện tại (xe/giờ)
    double budget;      // Chi phí xây dựng (tỷ VNĐ)
    string src;         // Node nguồn
    string dst;         // Node đích
    Direction direction; // ONE_WAY, TWO_WAY
    RoadType type;      // MAIN_ROAD, SECONDARY_ROAD, etc.
    bool isBlocked;     // Có bị chặn không
    bool isReverse;     // Là đường chiều ngược không
    
    // Phương thức tính thời gian
    double travelTime() const {
        return (speed > 0) ? (length / speed) * 60 : 999999;
    }
};
```

### 5.3 NewRoadProposal (Đề Xuất Xây Dựng)

```cpp
struct NewRoadProposal {
    string srcNode;             // Node bắt đầu
    string dstNode;             // Node kết thúc
    string intermediateNode;    // Node trung gian (cho đường 2 đoạn)
    double estimatedCost;       // Chi phí ước tính (tỷ VNĐ)
    double trafficReduction;    // Giảm lưu lượng (%)
    double travelTimeSaved;     // Thời gian tiết kiệm (phút)
    string reasoning;           // Lý do đề xuất
    bool isTwoSegment;          // Đường 2 đoạn hay không
    ProposalType type;          // NEW_ROAD, EXPAND_LANES, DIRECT_BYPASS
    vector<string> congestedPath; // Chuỗi đường tắc (cho DIRECT_BYPASS)
};
```

### 5.4 CongestionInfo (Thông Tin Ùn Tắc)

```cpp
struct CongestionInfo {
    string edgeId;              // ID đoạn đường
    string edgeName;            // Tên đoạn đường
    string srcNode;             // Node nguồn
    string dstNode;             // Node đích
    double flow;                // Lưu lượng (xe/giờ)
    double capacity;            // Sức chứa (xe/giờ)
    double congestionRatio;     // Tỷ lệ = flow/capacity
    double overloadPercent;     // % quá tải = (flow-capacity)/capacity × 100
};
```

### 5.5 RoadMap (Đồ Thị Giao Thông)

```cpp
class RoadMap {
private:
    unordered_map<string, Node> nodes_;
    vector<Edge> edges_;
    unordered_map<string, vector<Edge>> adjList_; // Adjacency list
    
public:
    // Quản lý nodes
    void addNode(const Node& node);
    bool hasNode(const string& id);
    vector<string> getNodeIds();
    
    // Quản lý edges
    void addEdge(const Edge& edge);
    bool hasEdge(const string& id);
    Edge* getEdgeById(const string& id);
    vector<Edge> getEdges();
    vector<Edge> outgoingEdges(const string& nodeId);
    
    // Chặn/Bỏ chặn đường
    void blockEdge(const string& edgeId);
    void unblockAll();
    
    // Load từ file
    bool loadFromFile(const string& filename);
};
```

**Cấu trúc adjacency list**:
```
adjList_["A"] = [Edge(A→B), Edge(A→C), Edge(A→D)]
adjList_["B"] = [Edge(B→E), Edge(B→F)]
...
```

---

## 6. Định Dạng File Dữ Liệu

### 6.1 File map.txt

```
<số_lượng_nodes>
<node_id> <node_name> <latitude> <longitude>
...

<số_lượng_edges>
<edge_id> <edge_name> <length> <speed> <capacity> <flow> <budget> <src> <dst> <direction> <type>
...
```

**Ví dụ**:
```
10
A NGA_TU_A 10.7756 106.7019
B NGA_TU_B 10.7812 106.7042
C NGA_TU_C 10.7889 106.7088

18
E01 DUONG_AB 2.5 60 2000 1800 50000 A B ONE_WAY MAIN_ROAD
E02 DUONG_BC 3.2 50 1500 1600 35000 B C ONE_WAY SECONDARY_ROAD
```

**Giải thích các trường**:
- `direction`: ONE_WAY (một chiều), TWO_WAY (hai chiều)
- `type`: MAIN_ROAD (đường chính), SECONDARY_ROAD (đường phụ), HIGHWAY (cao tốc), LOCAL_ROAD (đường nội bộ)

---

## 7. Kết Luận

### 7.1 Điểm Mạnh

1. **Hiệu quả**: Sử dụng Dijkstra với priority queue đảm bảo hiệu suất tốt
2. **Linh hoạt**: Hỗ trợ nhiều loại phân tích và tối ưu hóa
3. **Thực tế**: Tính toán dựa trên dữ liệu thực (lưu lượng, sức chứa, ngân sách)
4. **Mở rộng**: Dễ dàng thêm thuật toán và tính năng mới

### 7.2 Hạn Chế và Cải Tiến Tiềm Năng

1. **Dijkstra không tối ưu với trọng số âm**: Không vấn đề vì thời gian di chuyển luôn dương
2. **Chưa xử lý giao thông động**: Có thể mở rộng với thuật toán A* và cập nhật real-time
3. **Ước tính chi phí đơn giản**: Có thể cải thiện bằng machine learning
4. **Chưa tối ưu toàn cục**: Có thể áp dụng thuật toán tối ưu hóa mạng lưới (Network Flow)

### 7.3 Khuyến Nghị Sử Dụng

- **Dijkstra**: Phù hợp với mạng lưới giao thông đô thị (V < 10,000, E < 50,000)
- **Bypass Generation**: Nên giới hạn số intermediate nodes khi V lớn
- **Budget Optimization**: Cần cập nhật chi phí xây dựng thường xuyên

---

## 8. Tài Liệu Tham Khảo

1. **Dijkstra's Algorithm**: 
   - Dijkstra, E. W. (1959). "A note on two problems in connexion with graphs"

2. **Graph Theory**:
   - Cormen, T. H., et al. (2009). "Introduction to Algorithms" (3rd ed.)

3. **Traffic Optimization**:
   - Wardrop, J. G. (1952). "Some Theoretical Aspects of Road Traffic Research"

4. **Data Structures**:
   - Skiena, S. S. (2008). "The Algorithm Design Manual" (2nd ed.)

---

**Ngày tạo**: 2025-12-16  
**Phiên bản**: 1.0  
**Tác giả**: Traffic Map Analysis System Development Team
