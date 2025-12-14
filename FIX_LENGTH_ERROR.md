# Báo cáo sửa lỗi std::length_error trong main.cpp

## Vấn đề
Khi chạy file `main.exe` và nhập file `map.txt`, chương trình bị crash với lỗi:
```
terminate called after throwing an instance of 'std::length_error'
  what():  basic_string::_M_create
```

## Nguyên nhân
Lỗi xảy ra trong hai hàm `boxLine()` và `boxCenter()` (dòng 53-72 của file main.cpp).

Các hàm này được sử dụng để vẽ các khung box với độ rộng cố định (64 ký tự). Tuy nhiên, khi chuỗi văn bản chứa:
- Mã màu ANSI (như `CYAN "\033[36m"`, `RESET "\033[0m"`)
- Ký tự Unicode/emoji (như "🗺️")

Chiều dài thực tế của chuỗi (bao gồm cả mã ANSI) có thể vượt quá 62 ký tự. Khi tính toán khoảng trống (padding), chương trình có thể tính ra giá trị âm, và khi truyền giá trị âm vào hàm `string()` thì gây ra exception `std::length_error`.

Ví dụ:
```cpp
// Chuỗi này có length > 62 do mã ANSI
string content = CYAN "🗺️ THÔNG TIN BẢN ĐỒ HIỆN TẠI" RESET;
int pad = 62 - content.length(); // Có thể âm!
string(pad - 1, ' '); // ERROR nếu pad <= 0
```

## Giải pháp đã áp dụng
Đã thêm các kiểm tra an toàn (validation) vào 2 hàm:

### 1. Hàm `boxLine()` (dòng 58-59)
```cpp
int pad = inner - (int)s.length();
// Đảm bảo pad - 1 không âm
if (pad < 1) pad = 1;
return "| " + s + string(pad - 1, ' ') + "|\n";
```

### 2. Hàm `boxCenter()` (dòng 68-70)
```cpp
int left = (inner - (int)s.length()) / 2;
int right = inner - (int)s.length() - left;
// Đảm bảo left và right - 1 không âm
if (left < 0) left = 0;
if (right < 1) right = 1;
return "| " + string(left, ' ') + s + string(right - 1, ' ') + "|\n";
```

## Kết quả
- ✅ Chương trình không còn crash khi chạy với map.txt
- ✅ Các thay đổi tối thiểu - chỉ 5 dòng code được thêm vào
- ✅ Không ảnh hưởng đến chức năng hiện có
- ⚠️ Trong trường hợp chuỗi quá dài, box có thể rộng hơn 64 ký tự một chút, nhưng không gây crash

## Cách biên dịch lại
Nếu bạn muốn biên dịch lại file main.cpp trên Windows:
```bash
g++ -std=c++17 -o main.exe main.cpp RoadMap.cpp ShortestPath.cpp AlternativeRoute.cpp TrafficOptimization.cpp
```

## Ghi chú kỹ thuật
Một giải pháp hoàn hảo hơn sẽ là loại bỏ mã ANSI trước khi tính độ dài chuỗi, nhưng điều này yêu cầu thay đổi phức tạp hơn nhiều. Giải pháp hiện tại đã đủ để khắc phục lỗi crash một cách an toàn và đơn giản.
