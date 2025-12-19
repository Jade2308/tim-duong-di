# Chi tiết các thay đổi để khắc phục lỗi

## Vấn đề 1: Bản đồ hiển thị ID rất rối mắt không thể phân biệt được ID thuộc tuyến đường nào

### Thay đổi trong `main.cpp`:

**Trước đây:**
- Hiển thị tất cả các node trong danh sách dọc, mỗi node một dòng
- Hiển thị tất cả các edge với format đơn giản: `ID: src->dst | TG=time | D=length`
- Không có màu sắc phân biệt giữa ID, tên, và thông tin

**Sau khi sửa:**
- Hiển thị nodes theo hàng ngang, mỗi hàng 5 nodes, ngăn cách bằng dấu phẩy
- Định dạng edge rõ ràng hơn:
  - Dòng 1: `[ID]` (màu cyan) + tên đường + `: ` + node nguồn (màu xanh) + ` → ` + node đích (màu xanh)
  - Dòng 2: Thông tin chi tiết với indent: `    Thời gian: X | Độ dài: Y km`
- Thêm tiêu đề phân loại: "�� CÁC ĐIỂM GIAO LỘ CHÍNH" và "🛣️ CÁC TUYẾN ĐƯỜNG"

**Lợi ích:**
- Dễ phân biệt ID edge với tên đường
- Dễ nhận biết node nguồn và đích của mỗi tuyến đường
- Tiết kiệm không gian màn hình với cách hiển thị nodes theo hàng

## Vấn đề 2: Bản đồ sau khi tìm đường đi ngắn nhất và tuyến đường thay thế không hiển thị rõ tuyến đường

### Thay đổi trong `GuiRenderer.cpp` - Hàm `highlightPath()`:

**Vấn đề cũ:**
- Sử dụng hàm `latLonToScreen()` với tham số scale không đồng nhất với `drawMap()`
- `drawMap()` tính toán autoScale riêng dựa trên bounding box
- `highlightPath()` dùng scale parameter được truyền vào → tọa độ không khớp
- Đường highlight mỏng (5px) và màu vàng nhạt

**Giải pháp:**
1. Tính toán lại toàn bộ bounding box và autoScale trong `highlightPath()` giống hệt `drawMap()`
2. Sử dụng cùng công thức chuyển đổi tọa độ:
   ```cpp
   int x = static_cast<int>((lon - centerLon) * autoScale * 1000) + offsetX + 210;
   int y = static_cast<int>((centerLat - lat) * autoScale * 1000) + offsetY + 190;
   ```
3. Tăng độ rõ ràng của đường highlight:
   - Vẽ viền đen 8px
   - Vẽ đường chính vàng sáng 6px
4. Phân biệt màu sắc cho các node:
   - Node bắt đầu: màu xanh lá (0, 255, 0)
   - Node kết thúc: màu đỏ (255, 0, 0)
   - Node trung gian: màu vàng (255, 255, 0)

**Lợi ích:**
- Đường highlight hiển thị đúng vị trí trên bản đồ
- Dễ nhìn thấy tuyến đường với viền đen và màu vàng sáng
- Dễ phân biệt điểm bắt đầu, kết thúc và các điểm trung gian

## Vấn đề 3: Khi nhập ngân sách thấp hơn giá trị tối thiểu, output hiển thị lỗi và làm xuất ra terminal

### Thay đổi trong `TrafficOptimization.cpp`:

**Vấn đề cũ:**
- Không kiểm tra lỗi nhập liệu (cin.fail())
- Không validate ngân sách âm
- Không cảnh báo khi ngân sách quá thấp
- Có thể dẫn đến lỗi khi tính toán với giá trị âm hoặc không hợp lệ

**Giải pháp:**
1. Thêm kiểm tra `cin.fail()` để phát hiện nhập liệu không phải số:
   ```cpp
   if (cin.fail()) {
       cin.clear();
       cin.ignore(numeric_limits<streamsize>::max(), '\n');
       cout << "\n❌ LỖI: Ngân sách không hợp lệ. Vui lòng nhập số.\n";
       return;
   }
   ```

2. Validate ngân sách không thể âm:
   ```cpp
   if (budget < 0) {
       cout << "\n❌ LỖI: Ngân sách không thể âm. Vui lòng nhập số dương.\n";
       return;
   }
   ```

3. Cảnh báo khi ngân sách quá thấp (< 1 tỷ VNĐ):
   ```cpp
   if (budget < 1) {
       cout << "\n⚠️  CẢNH BÁO: Ngân sách quá thấp (< 1 tỷ VNĐ).\n";
       cout << "Ngân sách tối thiểu để có giải pháp cải thiện thường từ 5-10 tỷ VNĐ.\n";
       cout << "Bạn có muốn tiếp tục xem giải pháp không cần ngân sách? (y/n): ";
       // Cho phép người dùng quyết định có tiếp tục hay không
   }
   ```

4. Thêm `#include <limits>` để sử dụng `numeric_limits`

### Thay đổi trong `main_gui.cpp`:

**Giải pháp tương tự cho GUI:**
1. Validate ngân sách không âm và hiển thị dialog lỗi
2. Cảnh báo khi ngân sách < 1 tỷ VNĐ
3. Cho phép tiếp tục để xem giải pháp không cần ngân sách

**Lợi ích:**
- Ngăn chặn lỗi runtime do nhập liệu không hợp lệ
- Cảnh báo rõ ràng cho người dùng khi ngân sách quá thấp
- Không làm crash hoặc xuất lỗi ra terminal
- Output luôn được format đúng cách

## Tóm tắt các file thay đổi:

1. **main.cpp**: Cải thiện hiển thị bản đồ console (36 dòng thay đổi)
2. **GuiRenderer.cpp**: Fix tọa độ và cải thiện visualization (64 dòng thay đổi)
3. **TrafficOptimization.cpp**: Thêm validation ngân sách (29 dòng thay đổi)
4. **main_gui.cpp**: Thêm validation ngân sách cho GUI (19 dòng thay đổi)

**Tổng cộng: 148 dòng thay đổi (124 thêm, 24 xóa)**

Tất cả thay đổi đều tối ưu, tập trung vào việc khắc phục đúng 3 vấn đề được nêu ra, không thêm tính năng mới hay sửa các lỗi không liên quan.
