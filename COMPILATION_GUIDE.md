# Hướng dẫn Biên dịch (Compilation Guide)

## Yêu cầu hệ thống (System Requirements)

### Windows
- **Compiler**: MinGW-w64 hoặc Visual Studio với C++17 support
- **SDL2**: Download từ [libsdl.org](https://www.libsdl.org/download-2.0.php)
- **SDL2_ttf**: Download từ [SDL2_ttf releases](https://github.com/libsdl-org/SDL_ttf/releases)

### Linux (Ubuntu/Debian)
- **Compiler**: g++ 7.0 trở lên
- **SDL2**: Cài qua package manager

## Cài đặt Dependencies

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install -y build-essential libsdl2-dev libsdl2-ttf-dev
```

### Linux (Fedora/RHEL)
```bash
sudo dnf install gcc-c++ SDL2-devel SDL2_ttf-devel
```

### macOS
```bash
brew install sdl2 sdl2_ttf
```

### Windows
1. Download SDL2 development libraries từ [libsdl.org](https://www.libsdl.org/download-2.0.php)
2. Download SDL2_ttf từ [SDL2_ttf GitHub](https://github.com/libsdl-org/SDL_ttf/releases)
3. Giải nén và đặt trong thư mục dự án hoặc thiết lập PATH

## Biên dịch

### Cách 1: Sử dụng Makefile (Linux/macOS)

```bash
# Build dự án
make

# Hoặc build và chạy
make run

# Clean build artifacts
make clean

# Rebuild từ đầu
make rebuild
```

### Cách 2: Command Line thủ công

#### Linux
```bash
g++ -std=c++17 -o traffic_gui \
    main.cpp RoadMap.cpp ShortestPath.cpp \
    AlternativeRoute.cpp TrafficOptimization.cpp GUI.cpp \
    $(pkg-config --cflags --libs sdl2 SDL2_ttf) -lm
```

#### Windows (MinGW)
```cmd
g++ -std=c++17 -o traffic_gui.exe ^
    main.cpp RoadMap.cpp ShortestPath.cpp ^
    AlternativeRoute.cpp TrafficOptimization.cpp GUI.cpp ^
    -I<SDL2-include-path> -L<SDL2-lib-path> ^
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
```

**Lưu ý**: Thay `<SDL2-include-path>` và `<SDL2-lib-path>` bằng đường dẫn thực tế.

Ví dụ:
```cmd
g++ -std=c++17 -o traffic_gui.exe ^
    main.cpp RoadMap.cpp ShortestPath.cpp ^
    AlternativeRoute.cpp TrafficOptimization.cpp GUI.cpp ^
    -IC:\SDL2\include -LC:\SDL2\lib ^
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
```

### Cách 3: Visual Studio (Windows)

1. Tạo project mới C++ Console Application
2. Thêm tất cả .cpp và .h files vào project
3. Cấu hình SDL2:
   - **Project Properties → C/C++ → General → Additional Include Directories**: Thêm `<SDL2-path>\include`
   - **Project Properties → Linker → General → Additional Library Directories**: Thêm `<SDL2-path>\lib`
   - **Project Properties → Linker → Input → Additional Dependencies**: Thêm `SDL2.lib SDL2main.lib SDL2_ttf.lib`
4. Copy SDL2.dll và SDL2_ttf.dll vào thư mục build output
5. Build project (Ctrl+B)

## Chạy chương trình

### Linux/macOS
```bash
./traffic_gui
```

### Windows
```cmd
traffic_gui.exe
```

hoặc double-click vào file `traffic_gui.exe`

## Troubleshooting

### Lỗi: "SDL2.dll not found" (Windows)
**Giải pháp**: Copy `SDL2.dll` và `SDL2_ttf.dll` vào cùng thư mục với executable

### Lỗi: "cannot find -lSDL2"
**Giải pháp**: 
- Linux: Cài SDL2 dev package: `sudo apt-get install libsdl2-dev libsdl2_ttf-dev`
- Windows: Kiểm tra đường dẫn `-L` trong lệnh biên dịch

### Lỗi: "SDL.h: No such file or directory"
**Giải pháp**:
- Linux: Cài SDL2 headers: `sudo apt-get install libsdl2-dev`
- Windows: Kiểm tra đường dẫn `-I` trong lệnh biên dịch

### Lỗi: "Không thể load font"
**Giải pháp**:
- Linux: Font DejaVu Sans thường có sẵn. Nếu không: `sudo apt-get install fonts-dejavu`
- Windows: Font Arial có sẵn trong `C:\Windows\Fonts\`. GUI sẽ tự động tìm

### Lỗi biên dịch: "structured bindings only available with -std=c++17"
**Giải pháp**: Thêm flag `-std=c++17` vào lệnh biên dịch

### GUI không hiển thị (Linux headless environment)
**Giải pháp**: Sử dụng Xvfb
```bash
Xvfb :99 -screen 0 1200x800x24 &
export DISPLAY=:99
./traffic_gui
```

## Kiểm tra Installation

Sau khi biên dịch thành công, chạy:

```bash
./traffic_gui  # Linux/macOS
# hoặc
traffic_gui.exe  # Windows
```

Bạn sẽ thấy menu:
```
Chọn chế độ hoạt động:
1. Chế độ Console (Text-based)
2. Chế độ GUI (Đồ họa)
```

Chọn `2` và nhập `map.txt` để test GUI.

## Performance Tips

1. **Debug build vs Release build**:
   - Debug: `g++ -g -std=c++17 ...`
   - Release: `g++ -O2 -std=c++17 ...`

2. **Đồ thị lớn (>50 nodes)**:
   - GUI đã được tối ưu với circular layout
   - Có thể chuyển sang grid layout nếu cần

## Liên hệ

Nếu gặp vấn đề, vui lòng mở issue trên GitHub repository.
