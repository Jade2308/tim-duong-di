# Fix: Menu Navigation Issue After Loading Map

## Vấn đề (Problem)
Sau khi tải bản đồ thành công, khi người dùng nhấn Enter để tiếp tục từ `system("pause")`, chương trình tự động chọn mục menu đầu tiên (Tìm đường đi ngắn nhất) mà không cho phép người dùng điều hướng menu bằng phím mũi tên.

**English:** After successfully loading the map, when the user presses Enter to continue from `system("pause")`, the program automatically selects the first menu item (Shortest Path) without allowing the user to navigate the menu using arrow keys.

## Nguyên nhân (Root Cause)
Khi `system("pause")` được gọi, người dùng nhấn Enter để tiếp tục. Phím Enter này vẫn còn trong bộ đệm đầu vào (input buffer). Khi hàm `showMenu()` được gọi ngay sau đó, `_getch()` đọc phím Enter còn trong bộ đệm và ngay lập tức return index 0, khiến menu tự động chọn mục đầu tiên.

**English:** When `system("pause")` is called, the user presses Enter to continue. This Enter key remains in the input buffer. When `showMenu()` is called immediately after, `_getch()` reads the buffered Enter key and immediately returns index 0, causing the menu to automatically select the first item.

## Giải pháp (Solution)
Tạo một hàm helper `clearInputBuffer()` để xóa bộ đệm đầu vào và gọi nó sau mỗi lần gọi `system("pause")`:

```cpp
// Clear input buffer to prevent stray keypresses from interfering with menu navigation
void clearInputBuffer() {
    while (_kbhit()) {
        _getch();
    }
}
```

Sau đó gọi hàm này sau mỗi `system("pause")`:
```cpp
system("pause");
clearInputBuffer();
```

Giải pháp này:
1. `_kbhit()` kiểm tra xem có phím nào trong bộ đệm hay không
2. `_getch()` đọc và loại bỏ phím đó khỏi bộ đệm
3. Vòng lặp `while` tiếp tục cho đến khi bộ đệm trống hoàn toàn

**English:** This solution:
1. `_kbhit()` checks if there are any keys in the buffer
2. `_getch()` reads and removes that key from the buffer
3. The `while` loop continues until the buffer is completely empty

## Các thay đổi (Changes Made)
Đã tạo hàm helper `clearInputBuffer()` (dòng 37-42) và gọi nó sau tất cả các vị trí gọi `system("pause")`:

1. **Dòng 37-42**: Định nghĩa hàm `clearInputBuffer()`
2. **Dòng 197**: Sau khi tải bản đồ ban đầu
3. **Dòng 242**: Sau khi hiển thị kết quả tìm đường ngắn nhất
4. **Dòng 268**: Sau khi hiển thị kết quả đường thay thế
5. **Dòng 276**: Sau khi hiển thị kết quả tối ưu hóa giao thông
6. **Dòng 293**: Sau khi tải bản đồ mới

**English:** Created helper function `clearInputBuffer()` (lines 37-42) and called it after all `system("pause")` calls at the locations listed above.

## Cách kiểm tra (How to Test)

### Kịch bản kiểm tra 1 (Test Scenario 1):
1. Chạy chương trình: `main.exe` hoặc `project.exe`
2. Nhập tên file: `map.txt`
3. Đợi bản đồ được tải
4. Nhấn Enter để tiếp tục từ system("pause")
5. **Kết quả mong đợi**: Menu hiển thị với mục đầu tiên được highlight, người dùng có thể dùng phím UP/DOWN để di chuyển

**English:** Run program, enter filename `map.txt`, wait for map to load, press Enter to continue, **Expected**: Menu displays with first item highlighted, user can use UP/DOWN keys to navigate.

### Kịch bản kiểm tra 2 (Test Scenario 2):
1. Từ menu chính, chọn bất kỳ tùy chọn nào (1-4)
2. Hoàn thành tác vụ và nhấn Enter tại system("pause")
3. **Kết quả mong đợi**: Quay lại menu, có thể điều hướng bình thường

**English:** From main menu, select any option (1-4), complete the task and press Enter at system("pause"), **Expected**: Return to menu, can navigate normally.

## Lưu ý kỹ thuật (Technical Notes)
- Hàm `_kbhit()` và `_getch()` đến từ `<conio.h>` (chỉ có trên Windows)
- Giải pháp này chỉ hoạt động trên Windows với MinGW hoặc MSVC compiler
- Trên Linux/Mac, cần sử dụng thư viện khác như ncurses hoặc termios

**English:** `_kbhit()` and `_getch()` are from `<conio.h>` (Windows only). This solution works only on Windows with MinGW or MSVC compiler. On Linux/Mac, need to use different libraries like ncurses or termios.
