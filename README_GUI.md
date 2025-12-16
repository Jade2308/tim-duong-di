# Hệ Thống Phân Tích Bản Đồ Giao Thông - GUI Version

## Mô tả (Description)

Ứng dụng phân tích bản đồ giao thông với giao diện đồ họa (GUI) được xây dựng bằng SDL2. 
Ứng dụng cho phép người dùng tương tác với bản đồ giao thông, tìm đường đi ngắn nhất, 
gợi ý tuyến đường thay thế và phân tích tối ưu hóa giao thông.

This is a traffic map analysis application with a graphical user interface (GUI) built with SDL2.
The application allows users to interact with traffic maps, find shortest paths, 
suggest alternative routes, and analyze traffic optimization.

## Tính năng (Features)

1. **Hiển thị bản đồ trực quan (Visual Map Display)**
   - Vẽ các nút (nodes) và đoạn đường (edges) trên giao diện đồ họa
   - Màu sắc biểu thị mức độ tắc nghẽn giao thông:
     - 🟢 Xanh lá: Thông thoáng
     - 🟠 Cam: Đông đúc
     - 🔴 Đỏ: Tắc nghẽn

2. **Tìm đường đi ngắn nhất (Shortest Path Finding)**
   - Nhập điểm bắt đầu và kết thúc
   - Hiển thị đường đi được đánh dấu màu vàng trên bản đồ
   - Hiển thị thời gian di chuyển dự kiến

3. **Gợi ý tuyến đường thay thế (Alternative Route Suggestion)**
   - Chọn đoạn đường bị chặn
   - Tìm đường đi thay thế

4. **Phân tích tối ưu hóa giao thông (Traffic Optimization Analysis)**
   - Phân tích các đoạn đường tắc nghẽn
   - Đề xuất các giải pháp tối ưu hóa

5. **Tải bản đồ từ file (Load Map from File)**
   - Hỗ trợ định dạng file map.txt

## Yêu cầu hệ thống (System Requirements)

### Linux
- SDL2 library
- SDL2_ttf library
- g++ compiler with C++17 support

### Cài đặt trên Ubuntu/Debian:
```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
```

## Biên dịch (Compilation)

### Sử dụng Makefile:
```bash
make
```

### Hoặc biên dịch thủ công:
```bash
g++ -std=c++17 -o gui_app main_gui.cpp GuiRenderer.cpp RoadMap.cpp \
    ShortestPath.cpp AlternativeRoute.cpp TrafficOptimization.cpp \
    -lSDL2 -lSDL2_ttf
```

## Chạy ứng dụng (Running the Application)

```bash
./gui_app
```

Ứng dụng sẽ tự động tải file `map.txt` nếu có trong thư mục hiện tại.

### Chỉ định file bản đồ khác (Specify different map file):
```bash
./gui_app mymap.txt
```

## Cách sử dụng (How to Use)

1. **Menu chính**: Sử dụng chuột để di chuyển và nhấp vào các nút chức năng
2. **Nhập dữ liệu**: Khi được yêu cầu nhập, gõ văn bản và nhấn ENTER
3. **Xem kết quả**: Đường đi sẽ được đánh dấu trên bản đồ bằng màu vàng
4. **Quay lại menu**: Nhấn phím bất kỳ hoặc click chuột sau khi xem kết quả

## Định dạng file bản đồ (Map File Format)

File `map.txt` có định dạng:
```
<số_lượng_nodes>
<node_id> <node_name> <latitude> <longitude>
...

<số_lượng_edges>
<edge_id> <edge_name> <length> <speed> <capacity> <flow> <budget> <src> <dst> <direction> <type>
...
```

## Cấu trúc dự án (Project Structure)

- `main_gui.cpp` - Điểm vào chương trình GUI
- `GuiRenderer.h/cpp` - Xử lý vẽ giao diện đồ họa
- `RoadMap.h/cpp` - Quản lý dữ liệu bản đồ
- `ShortestPath.h/cpp` - Thuật toán tìm đường đi ngắn nhất
- `AlternativeRoute.h/cpp` - Tìm đường đi thay thế
- `TrafficOptimization.h/cpp` - Phân tích tối ưu hóa giao thông
- `Makefile` - File build tự động

## Thay đổi so với phiên bản Terminal (Changes from Terminal Version)

- ✅ Giao diện đồ họa thay cho terminal
- ✅ Hiển thị bản đồ trực quan với nodes và edges
- ✅ Tương tác bằng chuột
- ✅ Đường đi được đánh dấu màu sắc trên bản đồ
- ✅ Màu sắc biểu thị tình trạng giao thông
- ✅ Hỗ trợ đa nền tảng (cross-platform)

## Troubleshooting

### Lỗi "Failed to load font":
Ứng dụng sẽ tiếp tục chạy nhưng không hiển thị văn bản. Đảm bảo font DejaVu hoặc Liberation được cài đặt:
```bash
sudo apt-get install fonts-dejavu fonts-liberation
```

### Lỗi "SDL could not initialize":
Kiểm tra SDL2 đã được cài đặt đúng:
```bash
sudo apt-get install --reinstall libsdl2-dev libsdl2-ttf-dev
```

## Tác giả (Author)

Traffic Map Analysis System - GUI Version

## License

See project LICENSE file
