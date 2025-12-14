# Verification Checklist ✅

## Pre-Merge Verification

Use this checklist to verify all requirements have been met before merging the PR.

---

## Requirements Verification

### 1. Hiển thị hết tất cả các node và đường ✅

- [ ] Load map.txt in the application
- [ ] Verify the map display shows "TẤT CẢ CÁC NODE:" (not "CÁC NODE TIÊU BIỂU:")
- [ ] Count nodes displayed - should show all 10 nodes (A, B, C, D, E, F, G, H, I, J)
- [ ] Verify the map display shows "TẤT CẢ CÁC EDGE:" (not "CÁC EDGE TIÊU BIỂU:")
- [ ] Count edges displayed - should show all 18 edges (E01-E18)
- [ ] Verify no "... và X nodes/edges khác" message appears

**Expected Output:**
```
TẤT CẢ CÁC NODE:
 - A
 - B
 - C
 - D
 - E
 - F
 - G
 - H
 - I
 - J

TẤT CẢ CÁC EDGE:
E01: A->B | TG=... | D=...
E02: B->E | TG=... | D=...
...
E18: D->G | TG=... | D=...
```

---

### 2. Nhập tuyến đường ùn tắc (không phải node) ✅

- [ ] Select option 3 (Traffic Optimization)
- [ ] Verify prompt says: "Nhập ID tuyến đường bị ùn tắc (ví dụ: E01):"
- [ ] Prompt does NOT say: "Nhập ID nút giao bị ùn tắc"
- [ ] Input an edge ID (e.g., E16)
- [ ] Verify analysis shows route information:
  - [ ] "Tuyến đường ùn tắc: E16 (TechHighway)"
  - [ ] "Chiều: C → H"

**Expected Output:**
```
=== PHÂN TÍCH TÌNH TRẠNG ===
Tuyến đường ùn tắc: E16 (TechHighway)
Chiều: C → H
Lưu lượng xe hiện tại: 2500 xe/giờ
Sức chứa thiết kế: 3000 xe/giờ
Mức độ quá tải: 83%
```

---

### 3. Hiển thị rõ vấn đề (ngân sách thiếu bao nhiêu) ✅

- [ ] Test with insufficient budget (e.g., E16 with budget 50)
- [ ] Verify output shows "=== VẤN ĐỀ ===" section
- [ ] Verify shows "Ngân sách tối thiểu cần thiết: X tỷ VNĐ"
- [ ] Verify shows "Thiếu hụt ngân sách: Y tỷ VNĐ"
- [ ] Verify the shortfall calculation is correct (minimum - budget)

**Expected Output:**
```
=== VẤN ĐỀ ===
⚠ Không tìm thấy giải pháp khả thi trong ngân sách 50 tỷ VNĐ.
💰 Ngân sách tối thiểu cần thiết: 122 tỷ VNĐ
📊 Thiếu hụt ngân sách: 72 tỷ VNĐ
```

---

### 4a. Đề xuất điều tiết tín hiệu rõ hơn ✅

When budget is insufficient:

- [ ] Verify shows "=== GIẢI PHÁP THAY THẾ (KHÔNG CẦN NGÂN SÁCH) ===" section
- [ ] Verify shows 5 numbered steps:
  - [ ] 1. Điều chỉnh thời gian đèn tín hiệu
  - [ ] 2. Điều tiết luồng giao thông
  - [ ] 3. Quản lý tốc độ
  - [ ] 4. Giám sát và điều phối
  - [ ] 5. Thông tin và cảnh báo
- [ ] Verify each step has specific details (not generic)
- [ ] Verify shows timeline: "⏱️  Thời gian triển khai: 2-4 tuần"
- [ ] Verify shows cost: "💰 Chi phí ước tính: 5-10 tỷ VNĐ"

**Expected Output Sample:**
```
1. Điều chỉnh thời gian đèn tín hiệu:
   - Tăng thời gian đèn xanh cho hướng C → H
   - Thời gian đèn xanh đề xuất: 72 giây (hiện tại: 60 giây)

2. Điều tiết luồng giao thông:
   - Hạn chế xe tải nặng vào giờ cao điểm (7-9h và 17-19h)
   ...
```

---

### 4b. Đề xuất xây tuyến đường với thông số cụ thể ✅

When budget is sufficient (e.g., E16 with budget 200):

- [ ] Verify shows "=== GIẢI PHÁP ĐỀ XUẤT ===" section
- [ ] Verify shows route direction: "Chiều: X → Y"
- [ ] Verify shows "=== PHÂN TÍCH HIỆU QUẢ ===" section
- [ ] Verify shows traffic reduction:
  - [ ] "Giảm X% lưu lượng"
  - [ ] "Từ Y xe/giờ xuống còn Z xe/giờ"
  - [ ] "Mức độ quá tải mới: W%"
- [ ] Verify shows "Thông số tuyến đường mới đề xuất:"
  - [ ] "Lưu lượng xe dự kiến: X xe/giờ"
  - [ ] "Sức chứa thiết kế khuyến nghị: Y xe/giờ"
  - [ ] "Chiều đi: ONE_WAY or TWO_WAY"
  - [ ] "Loại đường: MAIN_ROAD"

**Expected Output Sample:**
```
=== GIẢI PHÁP ĐỀ XUẤT ===
Phương án được chọn: Xây dựng tuyến đường mới
  - Chiều: C → F

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
```

---

## Code Quality Verification

### Compilation

- [ ] Code compiles without errors
- [ ] No warnings during compilation
- [ ] Uses C++17 standard

**Command:**
```bash
g++ -std=c++17 -o project.exe main.cpp RoadMap.cpp ShortestPath.cpp AlternativeRoute.cpp TrafficOptimization.cpp
```

### Code Review

- [ ] All magic numbers extracted to named constants
- [ ] Clear comments for abbreviations (TG, D)
- [ ] No unused parameters
- [ ] Consistent code style

### Security

- [ ] CodeQL scan passed
- [ ] No security vulnerabilities detected

---

## Documentation Verification

- [ ] PR_README.md exists and is comprehensive
- [ ] IMPLEMENTATION_NOTES.md contains technical details
- [ ] EXPECTED_OUTPUT_EXAMPLES.md shows sample outputs
- [ ] FINAL_SUMMARY.md provides complete overview
- [ ] CHANGES_SUMMARY.txt shows visual comparison

---

## Final Checks

- [ ] All 4 main requirements met
- [ ] Code quality is good
- [ ] Documentation is complete
- [ ] Ready for Windows testing

---

## Sign-off

**Tested by:** _______________  
**Date:** _______________  
**All requirements verified:** ☐ Yes ☐ No  
**Ready to merge:** ☐ Yes ☐ No  

**Notes:**
_________________________________________________________________
_________________________________________________________________
_________________________________________________________________

---

## Post-Merge

After merging:

- [ ] Delete feature branch
- [ ] Update main README if needed
- [ ] Archive documentation files
- [ ] Create release notes

---

**Status:** Ready for verification ✅
