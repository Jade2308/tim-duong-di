# GUI Implementation for Traffic Optimization System

## Tổng quan (Overview)

Hệ thống đã được bổ sung giao diện đồ họa (GUI) sử dụng SDL2 để trực quan hóa bản đồ giao thông và các thuật toán tìm đường.

## Yêu cầu hệ thống (Requirements)

### Windows
- MinGW/g++ compiler
- SDL2.dll (đã có sẵn trong project)
- SDL2_ttf.dll (cần tải thêm từ SDL2_ttf releases)

### Linux
```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
```

## Biên dịch (Compilation)

### Windows (MinGW)
```bash
g++ -std=c++17 -o traffic_gui.exe main.cpp RoadMap.cpp ShortestPath.cpp AlternativeRoute.cpp TrafficOptimization.cpp GUI.cpp -I<path-to-SDL2-include> -L<path-to-SDL2-lib> -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
```

### Linux
```bash
g++ -std=c++17 -o traffic_gui main.cpp RoadMap.cpp ShortestPath.cpp AlternativeRoute.cpp TrafficOptimization.cpp GUI.cpp -I/usr/include/SDL2 -D_REENTRANT -lSDL2 -lSDL2_ttf -lm
```

## Sử dụng (Usage)

### Khởi động chương trình
```bash
./traffic_gui        # Linux
traffic_gui.exe      # Windows
```

### Chọn chế độ
1. **Chế độ Console** - Giao diện dòng lệnh truyền thống
2. **Chế độ GUI** - Giao diện đồ họa mới

### Hướng dẫn sử dụng GUI

1. **Chọn điểm bắt đầu**: Click vào node màu xanh dương
   - Node sẽ chuyển sang màu xanh lá

2. **Chọn điểm kết thúc**: Click vào node khác
   - Node sẽ chuyển sang màu đỏ

3. **Tìm đường đi**: Click nút "Tìm đường ngắn nhất"
   - Đường đi sẽ được highlight màu xanh dương đậm
   - Tổng khoảng cách hiển thị ở góc trên phải

4. **Các chức năng khác**:
   - **Đường thay thế**: Tìm tuyến đường thay thế
   - **Phát hiện tắc nghẽn**: Hiển thị các đoạn đường tắc nghẽn (màu cam)
   - **Reset**: Xóa lựa chọn và kết quả

## Tính năng (Features)

### ✅ Đã hoàn thành
- [x] Cửa sổ SDL2 1200x800 pixels
- [x] Hiển thị bản đồ từ map.txt
- [x] Vẽ nodes (hình tròn với tên)
- [x] Vẽ edges với trọng số
- [x] Phân biệt màu sắc:
  - Xanh dương: Node bình thường
  - Xanh lá: Điểm bắt đầu
  - Đỏ: Điểm kết thúc
  - Vàng: Node hover
  - Xám: Edge bình thường
  - Cam: Edge tắc nghẽn
  - Xanh dương đậm: Đường đi tìm được
- [x] Highlight đường đi ngắn nhất
- [x] 4 nút điều khiển
- [x] Click chuột để chọn nodes
- [x] Hover effects
- [x] Hiển thị tổng khoảng cách
- [x] Tích hợp với các class hiện có
- [x] Cross-platform (Windows/Linux)

## Cấu trúc file (File Structure)

```
tim-duong-di/
├── GUI.h                  # Header file cho GUI class
├── GUI.cpp                # Implementation của GUI
├── main.cpp               # Updated với GUI mode option
├── RoadMap.h/cpp          # Existing (không thay đổi)
├── ShortestPath.h/cpp     # Existing (không thay đổi)
├── AlternativeRoute.h/cpp # Existing (không thay đổi)
├── TrafficOptimization.h/cpp # Existing (không thay đổi)
├── map.txt                # Dữ liệu bản đồ
└── SDL2.dll               # SDL2 library (Windows)
```

## Kiến trúc (Architecture)

### GUI Class
```cpp
class GUI {
    // SDL components
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    TTF_Font* font_;
    
    // Data references
    RoadMap& map_;
    
    // GUI state
    std::unordered_map<std::string, GUINode> guiNodes_;
    std::vector<Button> buttons_;
    std::string startNodeId_;
    std::string endNodeId_;
    std::vector<std::string> currentPath_;
    
    // Main methods
    bool init();
    void run();
    void render();
    void handleEvents();
    void cleanup();
};
```

## Screenshots

![GUI Screenshot](https://github.com/user-attachments/assets/bac1b9af-06db-4683-9274-69b33d0ddf3e)

*Giao diện GUI với bản đồ 10 nodes, các edges có màu cam là đường tắc nghẽn*

## Lưu ý kỹ thuật (Technical Notes)

1. **Font**: Sử dụng DejaVu Sans (mặc định trên Linux)
   - Windows có thể cần chỉnh đường dẫn font trong GUI.cpp

2. **Node Layout**: Sử dụng circular layout để hiển thị đẹp
   - Có thể chuyển sang grid layout bằng cách gọi `positionNodesGrid()`

3. **Performance**: Tối ưu cho đồ thị >50 nodes
   - Chỉ render non-reverse edges
   - Efficient event handling

4. **UTF-8 Support**: Hỗ trợ tiếng Việt đầy đủ

## Troubleshooting

### Lỗi "Cannot open font"
- Linux: Font path đúng (`/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf`)
- Windows: Cần cập nhật đường dẫn font trong `GUI.cpp` line 84, 89

### Lỗi "SDL2 not found"
- Đảm bảo SDL2 dev libraries đã được cài đặt
- Kiểm tra compiler flags trong lệnh biên dịch

### Không hiển thị text tiếng Việt
- Đảm bảo font hỗ trợ Unicode
- Kiểm tra SDL_ttf đã được link đúng

## Tác giả (Author)

Implemented by GitHub Copilot for Jade2308/tim-duong-di repository
