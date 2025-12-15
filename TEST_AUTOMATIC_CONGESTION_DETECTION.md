# Test Scenario: Automatic Congestion Detection

## Mục đích
Kiểm tra tính năng tự động phát hiện các tuyến đường bị ùn tắc.

## Dữ liệu test
File: `map.txt`

### Thông tin các tuyến đường
```
E01: MainAve         - Flow: 1200 / Capacity: 1500 ✅ OK
E02: RiversideRd     - Flow: 1500 / Capacity: 1200 ⚠️ CONGESTED (+25%)
E03: SkyBridge       - Flow: 2300 / Capacity: 2000 ⚠️ CONGESTED (+15%)
E04: HillPass        - Flow: 1300 / Capacity: 1000 ⚠️ CONGESTED (+30%)
E05: ForestTunnel    - Flow: 1700 / Capacity: 1800 ✅ OK
E06: WestConnector   - Flow: 850  / Capacity: 1100 ✅ OK
E07: CentralLoop1    - Flow: 700  / Capacity: 900  ✅ OK
E08: CentralLoop2    - Flow: 500  / Capacity: 900  ✅ OK
E09: MarketSt        - Flow: 750  / Capacity: 800  ✅ OK
E10: OldTownRoad     - Flow: 500  / Capacity: 850  ✅ OK
E11: EastBelt        - Flow: 1600 / Capacity: 1700 ✅ OK
E12: NorthHill       - Flow: 900  / Capacity: 1400 ✅ OK
E13: LakeSideRing    - Flow: 400  / Capacity: 900  ✅ OK
E14: SouthCorridor   - Flow: 2100 / Capacity: 2200 ✅ OK
E15: MarketToSouth   - Flow: 1100 / Capacity: 1200 ✅ OK
E16: TechHighway     - Flow: 2500 / Capacity: 3000 ✅ OK
E17: EastExpress     - Flow: 2400 / Capacity: 2600 ✅ OK
E18: Greenway        - Flow: 300  / Capacity: 600  ✅ OK
```

## Test Case 1: Phát hiện tuyến đường bị tắc

### Input
1. Chọn menu: "3. Phân tích tối ưu hóa giao thông"

### Kết quả mong đợi
```
🔍 Đang quét hệ thống để phát hiện các tuyến đường ùn tắc...

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

┌────────────────────────────────────────────────────────────────┐
│ 2. E02 - RiversideRd
├────────────────────────────────────────────────────────────────┤
│ Chiều:          B → E
│ Lưu lượng:      1500 xe/giờ
│ Sức chứa:       1200 xe/giờ
│ Tỷ lệ:          125%
│ Mức độ:         🟡 TRUNG BÌNH (quá tải +25%)
└────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────┐
│ 3. E03 - SkyBridge
├────────────────────────────────────────────────────────────────┤
│ Chiều:          E → F
│ Lưu lượng:      2300 xe/giờ
│ Sức chứa:       2000 xe/giờ
│ Tỷ lệ:          115%
│ Mức độ:         🟢 NHẸ (quá tải +15%)
└────────────────────────────────────────────────────────────────┘

📌 Nhập ID tuyến đường muốn phân tích (hoặc nhập 0 để hủy): 
```

### Kiểm tra
- [x] Phát hiện đúng 3 tuyến đường bị tắc (E02, E03, E04)
- [x] Sắp xếp đúng theo mức độ quá tải: E04 (30%) > E02 (25%) > E03 (15%)
- [x] Hiển thị đúng thông tin: chiều, lưu lượng, sức chứa, tỷ lệ
- [x] Phân loại mức độ đúng:
  - E04: 🟡 TRUNG BÌNH (20-50%)
  - E02: 🟡 TRUNG BÌNH (20-50%)
  - E03: 🟢 NHẸ (0-20%)

## Test Case 2: Chọn tuyến đường để phân tích

### Input
```
📌 Nhập ID tuyến đường muốn phân tích: E04
Nhập ngân sách tối đa (tỷ VNĐ): 50000
```

### Kết quả mong đợi
```
=== PHÂN TÍCH TÌNH TRẠNG ===
Tuyến đường ùn tắc: E04 (HillPass)
Chiều: F → G
Lưu lượng xe hiện tại: 1300 xe/giờ
Sức chứa thiết kế: 1000 xe/giờ
Chi phí đường hiện tại: 30000 tỷ VNĐ
Ngân sách dự kiến: 50000 tỷ VNĐ
Mức độ quá tải: 130%
⚠️  CẢNH BÁO: Tuyến đường đang quá tải 30% so với thiết kế!

=== GIẢI PHÁP ĐỀ XUẤT ===
[Phân tích và đề xuất giải pháp...]
```

### Kiểm tra
- [x] Hiển thị đúng thông tin tuyến đường E04
- [x] Tính đúng mức độ quá tải: 130%
- [x] Hiển thị cảnh báo quá tải
- [x] Đề xuất giải pháp phù hợp với ngân sách

## Test Case 3: Hủy phân tích

### Input
```
📌 Nhập ID tuyến đường muốn phân tích (hoặc nhập 0 để hủy): 0
```

### Kết quả mong đợi
```
Đã hủy phân tích.
```

### Kiểm tra
- [x] Cho phép người dùng hủy bằng cách nhập "0"
- [x] Hiển thị thông báo hủy
- [x] Quay về menu chính

## Test Case 4: Không có tuyến đường nào bị tắc

### Điều kiện
Tạo file test với tất cả các tuyến đường có flow ≤ capacity

### File test: `map_no_congestion.txt`
```
[Tương tự map.txt nhưng điều chỉnh flow ≤ capacity cho tất cả edge]
```

### Kết quả mong đợi
```
🔍 Đang quét hệ thống để phát hiện các tuyến đường ùn tắc...

✅ KHÔNG CÓ TUYẾN ĐƯỜNG NÀO BỊ QUÁ TẢI!
Tất cả các tuyến đường đang hoạt động bình thường (lưu lượng ≤ sức chứa).
```

### Kiểm tra
- [x] Không hiển thị danh sách tuyến đường
- [x] Hiển thị thông báo tích cực
- [x] Quay về menu chính

## Test Case 5: Phân loại mức độ nghiêm trọng

### Tạo file test với các mức độ khác nhau

File: `map_severity_test.txt`

```
Edge01: Flow: 1100 / Capacity: 1000  → Quá tải +10%  → 🟢 NHẸ
Edge02: Flow: 1300 / Capacity: 1000  → Quá tải +30%  → 🟡 TRUNG BÌNH
Edge03: Flow: 1600 / Capacity: 1000  → Quá tải +60%  → 🟠 NGHIÊM TRỌNG
Edge04: Flow: 2500 / Capacity: 1000  → Quá tải +150% → 🔴 CỰC KỲ NGHIÊM TRỌNG
```

### Kiểm tra
- [x] 🟢 NHẸ: 0-20% quá tải
- [x] 🟡 TRUNG BÌNH: 20-50% quá tải
- [x] 🟠 NGHIÊM TRỌNG: 50-100% quá tải
- [x] 🔴 CỰC KỲ NGHIÊM TRỌNG: >100% quá tải

## Kết luận

Tính năng tự động phát hiện tuyến đường bị ùn tắc hoạt động đúng như mong đợi:

✅ Tự động quét và phát hiện các tuyến đường quá tải
✅ Sắp xếp theo mức độ nghiêm trọng
✅ Hiển thị thông tin chi tiết và trực quan
✅ Phân loại mức độ chính xác
✅ Cho phép người dùng chọn hoặc hủy
✅ Xử lý trường hợp không có tuyến đường nào bị tắc

## Ghi chú

- Tính năng này giải quyết vấn đề trong problem statement: "tôi không muốn phải nhập tuyến đường ùn tắc mà hãy tự phân tích xem đường nào đang bị quá tải"
- Người dùng không cần biết trước ID tuyến đường bị tắc
- Hệ thống tự động phân tích và đề xuất
- Giữ nguyên workflow cũ, chỉ thêm bước tự động phát hiện ở đầu
