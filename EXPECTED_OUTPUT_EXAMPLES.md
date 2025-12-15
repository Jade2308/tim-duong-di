# Expected Output Examples

This document shows expected outputs based on the changes to the traffic optimization system.

## Example 1: Map Display (All Nodes and Edges)

### Input:
User loads `map.txt`

### Output:
```
+--------------------------------------------------------------+
|      🗺️ THÔNG TIN BẢN ĐỒ HIỆN TẠI    |
|==============================================================|
| Tổng Nodes (Điểm Giao Lộ): 10                |
| Tổng Edges (Đoạn Đường): 36                |
|--------------------------------------------------------------|
| TẤT CẢ CÁC NODE:                                      |
|  - A                                                         |
|  - B                                                         |
|  - C                                                         |
|  - D                                                         |
|  - E                                                         |
|  - F                                                         |
|  - G                                                         |
|  - H                                                         |
|  - I                                                         |
|  - J                                                         |
|--------------------------------------------------------------|
| TẤT CẢ CÁC EDGE:                                      |
| E01: A->B | TG=0 | D=2                                       |
| E02: B->E | TG=0 | D=3                                       |
| E03: E->F | TG=0 | D=1                                       |
| E04: F->G | TG=0 | D=2                                       |
| E05: G->C | TG=0 | D=3                                       |
| E06: C->A | TG=0 | D=1                                       |
| E07: A->D | TG=0 | D=2                                       |
| E08: D->I | TG=0 | D=1                                       |
| E09: I->B | TG=0 | D=1                                       |
| E10: D->C | TG=0 | D=2                                       |
| E11: E->H | TG=0 | D=4                                       |
| E12: H->F | TG=0 | D=3                                       |
| E13: G->H | TG=0 | D=2                                       |
| E14: J->A | TG=0 | D=4                                       |
| E15: I->J | TG=0 | D=3                                       |
| E16: C->H | TG=0 | D=5                                       |
| E17: E->J | TG=0 | D=4                                       |
| E18: D->G | TG=0 | D=2                                       |
+--------------------------------------------------------------+
```

**Change:** All 10 nodes and all 18 original edges are shown (36 total including reverse edges).

---

## Example 2: Traffic Optimization - Insufficient Budget

### Input:
```
Nhập ID tuyến đường bị ùn tắc (ví dụ: E01): E16
Nhập ngân sách tối đa (tỷ VNĐ): 50
```

### Output:
```
=== PHÂN TÍCH TÌNH TRẠNG ===
Tuyến đường ùn tắc: E16 (TechHighway)
Chiều: C → H
Lưu lượng xe hiện tại: 2500 xe/giờ
Sức chứa thiết kế: 3000 xe/giờ
Mức độ quá tải: 83%
⚠️  Tuyến đường đang trong tình trạng gần quá tải.

=== VẤN ĐỀ ===
⚠ Không tìm thấy giải pháp khả thi trong ngân sách 50 tỷ VNĐ.
💰 Ngân sách tối thiểu cần thiết để xây tuyến đường mới: 122 tỷ VNĐ
📊 Thiếu hụt ngân sách: 72 tỷ VNĐ

=== GIẢI PHÁP THAY THẾ (KHÔNG CẦN NGÂN SÁCH) ===
1. Điều chỉnh thời gian đèn tín hiệu:
   - Tăng thời gian đèn xanh cho hướng C → H
   - Thời gian đèn xanh đề xuất: 59 giây (hiện tại: 60 giây)

2. Điều tiết luồng giao thông:
   - Hạn chế xe tải nặng vào giờ cao điểm (7-9h và 17-19h)
   - Khuyến khích phân làn theo loại phương tiện

3. Quản lý tốc độ:
   - Tốc độ tối đa khuyến nghị: 56 km/h (để tăng độ an toàn)
   - Đặt biển báo tốc độ điện tử thông minh

4. Giám sát và điều phối:
   - Lắp đặt camera giám sát lưu lượng xe
   - Triển khai hệ thống điều khiển tín hiệu thích ứng (Adaptive Traffic Control)

5. Thông tin và cảnh báo:
   - Cảnh báo tài xế về tình trạng tắc đường qua ứng dụng di động
   - Đề xuất tuyến đường thay thế cho người dân

⏱️  Thời gian triển khai: 2-4 tuần
💰 Chi phí ước tính: 5-10 tỷ VNĐ (chủ yếu cho thiết bị và công nghệ)
```

**Changes:**
1. ✅ Shows exact budget gap (72 tỷ VNĐ)
2. ✅ Provides detailed traffic signal management solutions
3. ✅ Includes timeline and cost estimates
4. ✅ 5 specific actionable steps

---

## Example 3: Traffic Optimization - Sufficient Budget

### Input:
```
Nhập ID tuyến đường bị ùn tắc (ví dụ: E01): E16
Nhập ngân sách tối đa (tỷ VNĐ): 200
```

### Output:
```
=== PHÂN TÍCH TÌNH TRẠNG ===
Tuyến đường ùn tắc: E16 (TechHighway)
Chiều: C → H
Lưu lượng xe hiện tại: 2500 xe/giờ
Sức chứa thiết kế: 3000 xe/giờ
Mức độ quá tải: 83%
⚠️  Tuyến đường đang trong tình trạng gần quá tải.

=== GIẢI PHÁP ĐỀ XUẤT ===
Phương án được chọn: Xây dựng tuyến đường mới
  - Chiều: C → F

Chi phí dự kiến: 122 tỷ VNĐ

=== LÝ DO ===
Xây dựng tuyến đường từ C đến F để tạo lối đi thay thế, giảm áp lực trên tuyến E16.

=== PHÂN TÍCH HIỆU QUẢ ===
• Giảm lưu lượng trên tuyến E16:
  - Giảm 40% lưu lượng
  - Từ 2500 xe/giờ xuống còn 1500 xe/giờ
  - Mức độ quá tải mới: 50%

• Thông số tuyến đường mới đề xuất:
  - Lưu lượng xe dự kiến: 1000 xe/giờ
  - Sức chứa thiết kế khuyến nghị: 1300 xe/giờ
  - Chiều đi: Một chiều (ONE_WAY)
  - Loại đường: Đường chính (MAIN_ROAD)

• Lợi ích bổ sung:
  - Tăng độ linh hoạt mạng lưới giao thông
  - Giảm thời gian di chuyển trung bình 10 phút
  - Giảm thiểu nguy cơ tắc nghẽn dây chuyền
```

**Changes:**
1. ✅ Shows specific route direction (C → F)
2. ✅ Before/after traffic flow analysis
3. ✅ Recommended capacity for new road
4. ✅ Road type specification (ONE_WAY, MAIN_ROAD)
5. ✅ Traffic volume projections

---

## Example 4: Two-Segment Route Proposal

### Input:
```
Nhập ID tuyến đường bị ùn tắc (ví dụ: E01): E01
Nhập ngân sách tối đa (tỷ VNĐ): 300
```

### Possible Output:
```
=== GIẢI PHÁP ĐỀ XUẤT ===
Phương án được chọn: Xây dựng tuyến đường vòng qua nút D
  - Đoạn 1: A → D
  - Đoạn 2: D → B

Chi phí dự kiến: 244 tỷ VNĐ

=== LÝ DO ===
Xây dựng tuyến đường vòng qua nút D để phân tán lưu lượng xe. Tạo 2 đoạn: A→D và D→B.

=== PHÂN TÍCH HIỆU QUẢ ===
• Giảm lưu lượng trên tuyến E01:
  - Giảm 60% lưu lượng
  - Từ 1200 xe/giờ xuống còn 480 xe/giờ
  - Mức độ quá tải mới: 32%

• Thông số tuyến đường mới đề xuất:
  - Lưu lượng xe dự kiến: 720 xe/giờ
  - Sức chứa thiết kế khuyến nghị: 936 xe/giờ
  - Chiều đi: Hai chiều (TWO_WAY)
  - Loại đường: Đường chính (MAIN_ROAD)

• Lợi ích bổ sung:
  - Tăng độ linh hoạt mạng lưới giao thông
  - Giảm thời gian di chuyển trung bình 10 phút
  - Giảm thiểu nguy cơ tắc nghẽn dây chuyền
```

**Changes:**
1. ✅ Shows two-segment route with intermediate node
2. ✅ TWO_WAY specification for bypass routes
3. ✅ Higher traffic reduction percentage (60%)

---

## Summary of Improvements

### 1. Map Display
- **Before:** Partial list (first 4 items)
- **After:** Complete list of all nodes and edges

### 2. Input Method
- **Before:** Node ID (e.g., "C")
- **After:** Edge/Route ID (e.g., "E16")

### 3. Problem Description
- **Before:** Generic "not feasible" message
- **After:** Exact budget gap calculation and clear shortfall amount

### 4. Traffic Signal Solutions
- **Before:** Generic suggestion
- **After:** 5 detailed, actionable steps with timelines and costs

### 5. Road Proposals
- **Before:** Basic route description
- **After:** 
  - Direction specification
  - Before/after traffic analysis
  - Recommended capacity
  - Road type (ONE_WAY/TWO_WAY, MAIN_ROAD)
  - Traffic volume projections
