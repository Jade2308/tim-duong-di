# Tính năng Phát hiện Ùn tắc Tự động (Automatic Congestion Detection)

## Tổng quan

Tính năng mới này tự động phân tích toàn bộ hệ thống giao thông và phát hiện các tuyến đường bị ùn tắc, thay vì yêu cầu người dùng phải nhập thủ công ID của tuyến đường bị tắc.

## Vấn đề đã giải quyết

**Trước đây:**
- Người dùng phải biết trước ID của tuyến đường bị ùn tắc
- Phải nhập thủ công ID tuyến đường (ví dụ: E01, E02)
- Không có cách nào để xem tổng quan tất cả các tuyến đường bị tắc

**Bây giờ:**
- Hệ thống tự động quét và phát hiện TẤT CẢ tuyến đường quá tải
- Hiển thị danh sách chi tiết với mức độ nghiêm trọng
- Người dùng chọn từ danh sách được đề xuất

## Cách hoạt động

### 1. Phát hiện tự động
Khi chọn chức năng "Phân tích tối ưu hóa giao thông", hệ thống sẽ:

```cpp
// Quét tất cả các edge trong bản đồ
for (each edge in map) {
    if (edge.flow > edge.capacity) {
        // Đánh dấu là tuyến đường bị tắc
        congestionInfo = {
            edgeId, flow, capacity,
            congestionRatio = flow/capacity,
            overloadPercent = (flow-capacity)/capacity * 100
        }
    }
}
```

### 2. Phân loại mức độ nghiêm trọng

Hệ thống phân loại tuyến đường bị tắc theo 4 mức độ:

- 🔴 **CỰC KỲ NGHIÊM TRỌNG**: Quá tải > 100% (flow > 2 × capacity)
- 🟠 **NGHIÊM TRỌNG**: Quá tải 50-100%
- 🟡 **TRUNG BÌNH**: Quá tải 20-50%
- 🟢 **NHẸ**: Quá tải 0-20%

### 3. Hiển thị danh sách

```
╔════════════════════════════════════════════════════════════════╗
║           DANH SÁCH CÁC TUYẾN ĐƯỜNG BỊ ÙN TẮC              ║
╠════════════════════════════════════════════════════════════════╣
║  Tìm thấy 3 tuyến đường đang bị quá tải                      ║
╚════════════════════════════════════════════════════════════════╝

┌────────────────────────────────────────────────────────────────┐
│ 1. E04 - HillPass
├────────────────────────────────────────────────────────────────┤
│ Chiều:          F → G
│ Lưu lượng:      1300 xe/giờ
│ Sức chứa:       1000 xe/giờ
│ Tỷ lệ:          130%
│ Mức độ:         🟡 TRUNG BÌNH (quá tải +30%)
└────────────────────────────────────────────────────────────────┘
```

### 4. Lựa chọn của người dùng

Người dùng chọn tuyến đường từ danh sách hoặc nhập "0" để hủy:
```
📌 Nhập ID tuyến đường muốn phân tích (hoặc nhập 0 để hủy): E04
Nhập ngân sách tối đa (tỷ VNĐ): 50000
```

## Các thay đổi kỹ thuật

### Thêm cấu trúc dữ liệu mới

```cpp
struct CongestionInfo {
    std::string edgeId;
    std::string edgeName;
    std::string srcNode;
    std::string dstNode;
    double flow;
    double capacity;
    double congestionRatio;   // flow/capacity
    double overloadPercent;   // (flow-capacity)/capacity * 100
};
```

### Thêm phương thức mới

1. **detectCongestedRoads()**: Quét và tìm tất cả tuyến đường bị tắc
   - Duyệt qua tất cả các edge (bỏ qua reverse edges)
   - Kiểm tra điều kiện: flow > capacity
   - Sắp xếp theo mức độ quá tải giảm dần

2. **displayCongestedRoadsList()**: Hiển thị danh sách tuyến đường bị tắc
   - Hiển thị với format đẹp mắt
   - Phân loại mức độ nghiêm trọng với icon màu
   - Hiển thị thông tin chi tiết: flow, capacity, tỷ lệ

### Cập nhật optimizeTraffic()

```cpp
void TrafficOptimization::optimizeTraffic() {
    // Bước 1: Tự động phát hiện
    auto congestedRoads = detectCongestedRoads();
    
    // Bước 2: Kiểm tra nếu không có tắc
    if (congestedRoads.empty()) {
        cout << "✅ KHÔNG CÓ TUYẾN ĐƯỜNG NÀO BỊ QUÁ TẢI!\n";
        return;
    }
    
    // Bước 3: Hiển thị danh sách
    displayCongestedRoadsList(congestedRoads);
    
    // Bước 4: Người dùng chọn
    // ... (code tiếp theo)
}
```

## Ví dụ sử dụng

### Trường hợp 1: Có tuyến đường bị tắc

```
Chọn chức năng: 3. Phân tích tối ưu hóa giao thông

🔍 Đang quét hệ thống để phát hiện các tuyến đường ùn tắc...

╔════════════════════════════════════════════════════════════════╗
║           DANH SÁCH CÁC TUYẾN ĐƯỜNG BỊ ÙN TẮC              ║
╠════════════════════════════════════════════════════════════════╣
║  Tìm thấy 3 tuyến đường đang bị quá tải                      ║
╚════════════════════════════════════════════════════════════════╝

[Danh sách các tuyến đường...]

📌 Nhập ID tuyến đường muốn phân tích (hoặc nhập 0 để hủy): E04
Nhập ngân sách tối đa (tỷ VNĐ): 50000

=== PHÂN TÍCH TÌNH TRẠNG ===
[Chi tiết phân tích...]
```

### Trường hợp 2: Không có tuyến đường nào bị tắc

```
Chọn chức năng: 3. Phân tích tối ưu hóa giao thông

🔍 Đang quét hệ thống để phát hiện các tuyến đường ùn tắc...

✅ KHÔNG CÓ TUYẾN ĐƯỜNG NÀO BỊ QUÁ TẢI!
Tất cả các tuyến đường đang hoạt động bình thường (lưu lượng ≤ sức chứa).
```

## Lợi ích

1. **Tiện lợi hơn**: Không cần biết trước ID tuyến đường bị tắc
2. **Tổng quan**: Xem được toàn bộ tình trạng hệ thống giao thông
3. **Ưu tiên**: Các tuyến đường được sắp xếp theo mức độ nghiêm trọng
4. **Trực quan**: Hiển thị với màu sắc và biểu tượng dễ hiểu
5. **Linh hoạt**: Người dùng có thể chọn hủy nếu không muốn phân tích

## Kiểm thử

File test: `map.txt`

Kết quả mong đợi:
- Phát hiện 3 tuyến đường bị tắc: E02, E03, E04
- E04 có mức quá tải cao nhất (+30%)
- E02 quá tải +25%
- E03 quá tải +15%

## Tương thích ngược

Tính năng mới vẫn giữ nguyên workflow cũ:
1. Hiển thị danh sách tuyến đường bị tắc
2. Người dùng chọn ID từ danh sách
3. Nhập ngân sách
4. Phân tích và đề xuất giải pháp

Chỉ khác là bây giờ hệ thống tự động tìm và hiển thị danh sách, thay vì yêu cầu người dùng tự nhập ID.
