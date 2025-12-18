// main.cpp - Giao diện tiếng Việt và thiết kế cân đối
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
#include <vector>
#include <limits> // Dùng cho cin.ignore

#include "RoadMap.h"
#include "ShortestPath.h"
#include "AlternativeRoute.h"
#include "TrafficOptimization.h"

using namespace std;

// Simple colors using ANSI (still ASCII escape sequences)
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define INVERT  "\033[7m"

// Helper function to truncate UTF-8 string to a maximum display width
string utf8_truncate(const string& str, size_t maxDisplayWidth) {
    size_t width = 0;
    size_t bytePos = 0;
    
    for (size_t i = 0; i < str.length(); ) {
        if (width >= maxDisplayWidth) {
            break;
        }
        
        unsigned char c = str[i];
        
        // Skip ANSI escape sequences
        if (c == '\033') {
            while (i < str.length() && str[i] != 'm') {
                i++;
            }
            if (i < str.length()) i++;  // Skip the 'm'
            continue;
        }
        
        // Determine UTF-8 character length and validate boundaries
        size_t charLen;
        if (c < 0x80) {
            charLen = 1;
        } else if ((c & 0xE0) == 0xC0) {
            charLen = 2;
        } else if ((c & 0xF0) == 0xE0) {
            charLen = 3;
        } else if ((c & 0xF8) == 0xF0) {
            charLen = 4;
        } else {
            // Invalid UTF-8 start byte, skip it
            i++;
            continue;
        }
        
        // Check if we have enough bytes for this character
        if (i + charLen > str.length()) {
            break;  // Incomplete character at end of string
        }
        
        i += charLen;
        width++;
        bytePos = i;
    }
    
    return str.substr(0, bytePos);
}

// Helper function to count UTF-8 characters (not bytes)
size_t utf8_length(const string& str) {
    size_t count = 0;
    for (size_t i = 0; i < str.length(); ) {
        unsigned char c = str[i];
        size_t charLen;
        
        if (c < 0x80) {
            charLen = 1;
        } else if ((c & 0xE0) == 0xC0) {
            charLen = 2;
        } else if ((c & 0xF0) == 0xE0) {
            charLen = 3;
        } else if ((c & 0xF8) == 0xF0) {
            charLen = 4;
        } else {
            // Invalid UTF-8 start byte
            i++;
            continue;
        }
        
        // Check boundary
        if (i + charLen > str.length()) {
            break;  // Incomplete character
        }
        
        i += charLen;
        count++;
    }
    return count;
}

// Helper function to get display width (considering ANSI escape codes)
size_t display_width(const string& str) {
    size_t width = 0;
    bool in_escape = false;
    
    for (size_t i = 0; i < str.length(); ) {
        if (str[i] == '\033') {
            in_escape = true;
            i++;
        } else if (in_escape && str[i] == 'm') {
            in_escape = false;
            i++;
        } else if (in_escape) {
            i++;
        } else {
            unsigned char c = str[i];
            size_t charLen;
            
            if (c < 0x80) {
                charLen = 1;
            } else if ((c & 0xE0) == 0xC0) {
                charLen = 2;
            } else if ((c & 0xF0) == 0xE0) {
                charLen = 3;
            } else if ((c & 0xF8) == 0xF0) {
                charLen = 4;
            } else {
                // Invalid UTF-8, skip
                i++;
                continue;
            }
            
            // Check boundary
            if (i + charLen > str.length()) {
                break;
            }
            
            i += charLen;
            width++;
        }
    }
    return width;
}

// Initialize console for UTF-8 and ANSI (safe)
void enableConsole() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
}

// Clear input buffer to prevent stray keypresses from interfering with menu navigation
void clearInputBuffer() {
    while (_kbhit()) {
        _getch();
    }
}

// Simple fixed-width box printing using ASCII chars
const int BOX_WIDTH = 64;

string boxTop() {
    return "+" + string(BOX_WIDTH - 2, '-') + "+\n";
}
string boxBottom() {
    return "+" + string(BOX_WIDTH - 2, '-') + "+\n";
}
string boxLine(const string& content) {
    int inner = BOX_WIDTH - 2;
    string s = content;
    size_t displayLen = display_width(s);
    
    if ((int)displayLen > inner) {
        // Properly truncate UTF-8 string at character boundary
        s = utf8_truncate(s, inner);
        displayLen = display_width(s);
    }
    
    int pad = inner - (int)displayLen;
    if (pad < 1) pad = 1;
    return "| " + s + string(pad - 1, ' ') + "|\n";
}
string boxCenter(const string& content) {
    int inner = BOX_WIDTH - 2;
    string s = content;
    size_t displayLen = display_width(s);
    
    if ((int)displayLen > inner) {
        // Properly truncate UTF-8 string at character boundary
        s = utf8_truncate(s, inner);
        displayLen = display_width(s);
    }
    
    int left = (inner - (int)displayLen) / 2;
    int right = inner - (int)displayLen - left;
    if (left < 0) left = 0;
    if (right < 1) right = 1;
    return "| " + string(left, ' ') + s + string(right - 1, ' ') + "|\n";
}

// map display
string currentMapText = "";

string buildMapDisplay(RoadMap& map) {
    string out;
    out += boxTop();
    out += boxCenter(CYAN "🗺️ THÔNG TIN BẢN ĐỒ HIỆN TẠI" RESET);
    out += "|" + string(BOX_WIDTH - 2, '=') + "|\n";
    
    int totalNodes = map.getNodeIds().size();
    int totalEdges = map.getEdges().size();

    if (totalNodes == 0) {
        out += boxCenter(RED "--- BẢN ĐỒ TRỐNG ---" RESET);
        out += boxBottom();
        return out;
    }

    out += boxLine(GREEN "Tổng Nodes (Điểm Giao Lộ): " + to_string(totalNodes) + RESET);
    out += boxLine(GREEN "Tổng Edges (Đoạn Đường): " + to_string(totalEdges) + RESET);
    out += "|" + string(BOX_WIDTH - 2, '-') + "|\n";
    
    // Hiển thị TẤT CẢ các node
    out += boxLine("TẤT CẢ CÁC NODE:");
    for (auto id : map.getNodeIds()) {
        out += boxLine(" - " + id);
    }
    out += "|" + string(BOX_WIDTH - 2, '-') + "|\n";

    // Hiển thị TẤT CẢ các edge (chỉ hiển thị edge gốc, không hiển thị reverse)
    out += boxLine("TẤT CẢ CÁC EDGE:");
    for (auto e : map.getEdges()) {
        if (!e.isReverse) {  // Chỉ hiển thị edge gốc
            double time = e.avgSpeed > 0 ? e.length / e.avgSpeed : 1e9;
            // TG = Thời gian (Time), D = Độ dài (Length)
            string line = e.id + ": " + e.src + "->" + e.dst;
            line += " | TG=" + to_string((int)time) + " | D=" + to_string((int)e.length);
            out += boxLine(line);
        }
    }

    out += boxBottom();
    return out;
}

// menu show (arrow keys)
int showMenu(const vector<string>& items) {
    int index = 0;
    while (true) {
        system("cls");
        
        // TIÊU ĐỀ
        cout << boxTop();
        cout << boxCenter(GREEN "🚗 HỆ THỐNG PHÂN TÍCH BẢN ĐỒ GIAO THÔNG" RESET);
        cout << boxBottom();
        cout << "\n";
        
        // BẢN ĐỒ HIỆN TẠI
        cout << currentMapText << "\n";
        
        // KHUNG MENU
        cout << boxTop();
        cout << boxCenter(YELLOW "💡 CHỌN CHỨC NĂNG (Dùng UP/DOWN, Enter để chọn)" RESET);
        cout << boxBottom();
        
        // Các mục menu
        for (int i = 0; i < (int)items.size(); ++i) {
            string line;
            if (i == index) {
                // highlighted
                line = string(INVERT) + "-> " + items[i] + RESET;
            } else {
                line = "   " + items[i];
            }
            // center-ish print
            int pad = (BOX_WIDTH - (int)line.length()) / 2;
            if (pad < 0) pad = 0;
            cout << string(pad, ' ') << line << "\n";
        }
        cout << "\n";

        int k = _getch();
        if (k == 224) {
            k = _getch();
            if (k == 72) index = (index - 1 + items.size()) % items.size(); // up
            else if (k == 80) index = (index + 1) % items.size(); // down
        } else if (k == 13) {
            return index;
        }
    }
}

// =================================================================
// HÀM MAIN ĐÃ CẬP NHẬT
// =================================================================
int main() {
    enableConsole();

    RoadMap map;

    // Yêu cầu tên file ban đầu
    cout << GREEN << "Nhập tên file bản đồ ban đầu (hoặc để trống để bỏ qua): " << RESET;
    string file;
    getline(cin, file); 

    if (!file.empty()) {
        if (!map.loadFromFile(file)) {
            cout << RED << "❌ Lỗi: Không thể tải file: " << file << RESET << "\n";
        } else {
            cout << GREEN << "✅ Tải thành công " << map.getNodeIds().size() << " Nodes và " << map.getEdges().size() << " Edges." << RESET << "\n";
            currentMapText = buildMapDisplay(map);
        }
    } else {
        currentMapText = buildMapDisplay(map); 
    }
    
    cout << "\n";
    system("pause");
    clearInputBuffer();

    vector<string> menu = {
        "1. Tìm đường đi ngắn nhất (Shortest Path)",
        "2. Gợi ý tuyến đường thay thế (Alternative Route)",
        "3. Phân tích tối ưu hóa giao thông (Traffic Optimization)",
        "4. Tải thêm bản đồ từ file (Load Map)",
        "5. Thoát (Exit)"
    };

    while (true) {
        int choice = showMenu(menu);

        system("cls");
        cout << currentMapText << "\n\n";

        if (choice == 0) {
            // 1. Tìm đường đi ngắn nhất
            string s, g;
            cout << YELLOW << "Nhập ID Node Bắt đầu (Start): " << RESET;
            cin >> s;
            cout << YELLOW << "Nhập ID Node Kết thúc (Goal): " << RESET;
            cin >> g;
            
            // Xóa bộ đệm sau khi dùng cin >>
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

            // Kiểm tra sự tồn tại của Node (Sử dụng hasNode)
            if (!map.hasNode(s)) { 
                cout << RED << "❌ Lỗi: Node Bắt đầu '" << s << "' không tồn tại.\n" << RESET;
            } else if (!map.hasNode(g)) { 
                cout << RED << "❌ Lỗi: Node Kết thúc '" << g << "' không tồn tại.\n" << RESET;
            } else {
                ShortestPath sp(map);
                vector<string> path;
                double t = sp.findShortestPath(s, g, path);
                if (t < 0) cout << RED << "💔 Không tìm thấy đường đi từ " << s << " đến " << g << "\n" << RESET;
                else {
                    cout << "\n";
                    cout << boxTop();
                    cout << boxCenter(GREEN "✅ ĐƯỜNG ĐI NGẮN NHẤT ĐÃ TÌM THẤY" RESET);
                    cout << "|" + string(BOX_WIDTH - 2, '=') + "|\n";
                    cout << boxLine(" ");
                    
                    // Display path with arrows
                    string pathStr = "   ";
                    for (size_t i = 0; i < path.size(); ++i) {
                        pathStr += path[i];
                        if (i < path.size() - 1) pathStr += " -> ";
                    }
                    
                    // Split long path into multiple lines if needed (UTF-8 aware)
                    size_t displayLen = display_width(pathStr);
                    if (displayLen > (size_t)(BOX_WIDTH - 4)) {
                        // Display in chunks using UTF-8 aware truncation
                        string remaining = pathStr;
                        while (display_width(remaining) > 0 && !remaining.empty()) {
                            string chunk = utf8_truncate(remaining, BOX_WIDTH - 4);
                            
                            // Safety check to prevent infinite loop
                            if (chunk.empty() || chunk.length() >= remaining.length()) {
                                // Display remaining and break
                                cout << boxLine(remaining);
                                break;
                            }
                            
                            cout << boxLine(chunk);
                            remaining = remaining.substr(chunk.length());
                        }
                    } else {
                        cout << boxLine(pathStr);
                    }
                    
                    cout << boxLine(" ");
                    cout << boxLine(CYAN "Tổng thời gian: " + to_string((int)t) + " đơn vị" RESET);
                    cout << boxBottom();
                    cout << "\n";
                }
            }
            system("pause");
            clearInputBuffer();
        }
        else if (choice == 1) {
            // 2. Gợi ý tuyến đường thay thế
            string edgeId, s, g;
            cout << YELLOW << "Nhập ID Edge (đoạn đường) cần CHẶN: " << RESET;
            cin >> edgeId;
            cout << YELLOW << "Nhập ID Node Bắt đầu (Start): " << RESET; 
            cin >> s;
            cout << YELLOW << "Nhập ID Node Kết thúc (Goal): " << RESET; 
            cin >> g;

            // Xóa bộ đệm sau khi dùng cin >>
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

            // Kiểm tra sự tồn tại (Sử dụng hasEdge và hasNode)
            if (!map.hasEdge(edgeId)) { 
                cout << RED << "❌ Lỗi: Edge cần chặn '" << edgeId << "' không tồn tại.\n" << RESET;
            } else if (!map.hasNode(s) || !map.hasNode(g)) { 
                cout << RED << "❌ Lỗi: Node Bắt đầu/Kết thúc không tồn tại.\n" << RESET;
            } else {
                cout << CYAN << "🔄 Đang tìm đường đi thay thế khi chặn Edge " << edgeId << "...\n" << RESET;
                AlternativeRoute alt(map);
                
                // Use the new method that returns a result
                auto result = alt.findAlternativeRoute(edgeId, s, g);
                
                if (!result.success) {
                    cout << "\n";
                    cout << boxTop();
                    cout << boxCenter(RED "❌ KHÔNG TÌM THẤY ĐƯỜNG THAY THẾ" RESET);
                    cout << "|" + string(BOX_WIDTH - 2, '=') + "|\n";
                    cout << boxLine(" ");
                    cout << boxLine("Edge bị chặn: " + edgeId);
                    cout << boxLine("Không có tuyến đường thay thế khả thi");
                    cout << boxLine(" ");
                    cout << boxBottom();
                } else {
                    cout << "\n";
                    cout << boxTop();
                    cout << boxCenter(GREEN "✅ TUYẾN ĐƯỜNG THAY THẾ ĐÃ TÌM THẤY" RESET);
                    cout << "|" + string(BOX_WIDTH - 2, '=') + "|\n";
                    cout << boxLine(" ");
                    cout << boxLine(RED "Edge bị chặn: " + edgeId RESET);
                    cout << boxLine(" ");
                    
                    // Display path with arrows
                    string pathStr = "   ";
                    for (size_t i = 0; i < result.path.size(); ++i) {
                        pathStr += result.path[i];
                        if (i < result.path.size() - 1) pathStr += " -> ";
                    }
                    
                    // Split long path into multiple lines if needed (UTF-8 aware)
                    size_t displayLen = display_width(pathStr);
                    if (displayLen > (size_t)(BOX_WIDTH - 4)) {
                        // Display in chunks using UTF-8 aware truncation
                        string remaining = pathStr;
                        while (display_width(remaining) > 0 && !remaining.empty()) {
                            string chunk = utf8_truncate(remaining, BOX_WIDTH - 4);
                            
                            // Safety check to prevent infinite loop
                            if (chunk.empty() || chunk.length() >= remaining.length()) {
                                // Display remaining and break
                                cout << boxLine(remaining);
                                break;
                            }
                            
                            cout << boxLine(chunk);
                            remaining = remaining.substr(chunk.length());
                        }
                    } else {
                        cout << boxLine(pathStr);
                    }
                    
                    cout << boxLine(" ");
                    cout << boxLine(CYAN "Thời gian: " + to_string((int)result.travelTime) + " đơn vị" RESET);
                    cout << boxBottom();
                    cout << "\n";
                }
            }
            system("pause");
            clearInputBuffer();
        }
        else if (choice == 2) {
            // 3. Tối ưu hóa giao thông
            cout << CYAN << "📈 Đang chạy Phân tích Tối ưu hóa Giao thông...\n" << RESET;
            TrafficOptimization opt(map);
            opt.optimizeTraffic();
            system("pause");
            clearInputBuffer();
        }
        else if (choice == 3) {
            // 4. Tải thêm bản đồ
            string f;
            cout << YELLOW << "Nhập tên file bản đồ cần tải: " << RESET;
            
            // Dùng getline >> ws để đọc tên file (có thể có khoảng trắng) và bỏ qua khoảng trắng/xuống dòng dư thừa
            getline(cin >> ws, f); 

            if (map.loadFromFile(f)) {
                currentMapText = buildMapDisplay(map);
                cout << GREEN << "✅ Tải bản đồ từ " << f << " thành công. Bản đồ đã được cập nhật.\n" << RESET;
            } else {
                cout << RED << "❌ Lỗi: Tải file " << f << " thất bại. Kiểm tra tên file và định dạng.\n" << RESET;
            }
            system("pause");
            clearInputBuffer();
        }
        else if (choice == 4) {
            // 5. Thoát
            cout << GREEN << "\n👋 Tạm biệt. Hẹn gặp lại!\n" << RESET;
            break;
        }
    }

    return 0;
}