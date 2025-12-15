// main.cpp - Giao diện tiếng Việt và thiết kế cân đối
#include <iostream>
#include <string>
#include <vector>
#include <limits> // Dùng cho cin.ignore
#include <clocale>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include "RoadMap.h"
#include "ShortestPath.h"
#include "AlternativeRoute.h"
#include "TrafficOptimization.h"
#include "GUI.h"

using namespace std;

// Simple colors using ANSI (still ASCII escape sequences)
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define INVERT  "\033[7m"

// Initialize console for UTF-8 and ANSI (safe)
void enableConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
#else
    // On Linux, UTF-8 is usually default, no special setup needed
    setlocale(LC_ALL, "");
#endif
}

// Cross-platform getch implementation
int getch_cross() {
#ifdef _WIN32
    return _getch();
#else
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
#endif
}

// Cross-platform kbhit implementation
int kbhit_cross() {
#ifdef _WIN32
    return _kbhit();
#else
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    
    return 0;
#endif
}

// Cross-platform pause function
void pause_cross() {
#ifdef _WIN32
    system("pause");
#else
    cout << "Nhấn Enter để tiếp tục...";
    cin.get();
#endif
}

// Clear input buffer to prevent stray keypresses from interfering with menu navigation
void clearInputBuffer() {
    while (kbhit_cross()) {
        getch_cross();
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
    if ((int)s.length() > inner) s = s.substr(0, inner);
    int pad = inner - (int)s.length();
    // Ensure pad - 1 is non-negative
    if (pad < 1) pad = 1;
    return "| " + s + string(pad - 1, ' ') + "|\n";
}
string boxCenter(const string& content) {
    int inner = BOX_WIDTH - 2;
    string s = content;
    if ((int)s.length() > inner) s = s.substr(0, inner);
    int left = (inner - (int)s.length()) / 2;
    int right = inner - (int)s.length() - left;
    // Ensure left and right - 1 are non-negative
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
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        
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

        int k = getch_cross();
        if (k == 224 || k == 27) { // 224 for Windows, 27 (ESC) for Linux arrow keys
            k = getch_cross();
            if (k == 72 || k == 65) index = (index - 1 + items.size()) % items.size(); // up (72=Win, 65=Linux)
            else if (k == 80 || k == 66) index = (index + 1) % items.size(); // down (80=Win, 66=Linux)
        } else if (k == 13 || k == 10) { // Enter (13=Win, 10=Linux)
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

    // Hiển thị menu chọn chế độ
    cout << boxTop();
    cout << boxCenter(GREEN "🚗 HỆ THỐNG PHÂN TÍCH BẢN ĐỒ GIAO THÔNG" RESET);
    cout << boxBottom();
    cout << "\n";
    cout << "Chọn chế độ hoạt động:\n";
    cout << "1. Chế độ Console (Text-based)\n";
    cout << "2. Chế độ GUI (Đồ họa)\n";
    cout << "Lựa chọn của bạn (1 hoặc 2): ";
    
    int modeChoice;
    cin >> modeChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    if (modeChoice == 2) {
        // Chế độ GUI
        cout << GREEN << "\n🎨 Đang khởi động chế độ GUI...\n" << RESET;
        cout << "Nhập tên file bản đồ (mặc định: map.txt): ";
        string file;
        getline(cin, file);
        
        if (file.empty()) {
            file = "map.txt";
        }
        
        if (!map.loadFromFile(file)) {
            cout << RED << "❌ Lỗi: Không thể tải file: " << file << RESET << "\n";
            cout << "Nhấn Enter để thoát...";
            cin.get();
            return 1;
        }
        
        cout << GREEN << "✅ Tải thành công " << map.getNodeIds().size() << " Nodes và " << map.getEdges().size() << " Edges." << RESET << "\n";
        
        GUI gui(map);
        if (!gui.init()) {
            cout << RED << "❌ Lỗi: Không thể khởi tạo GUI!" << RESET << "\n";
            cout << "Nhấn Enter để thoát...";
            cin.get();
            return 1;
        }
        
        cout << GREEN << "✅ GUI đã khởi động thành công!\n" << RESET;
        gui.run();
        
        return 0;
    }
    
    // Chế độ Console (code gốc)
    cout << GREEN << "\n📝 Đang khởi động chế độ Console...\n" << RESET;

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
    pause_cross();
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

        system("clear");
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
                    cout << GREEN << "✅ ĐƯỜNG ĐI NGẮN NHẤT ĐÃ TÌM THẤY:" << RESET << "\n";
                    cout << "   Tuyến đường: ";
                    for (auto &n : path) cout << n << (n == path.back() ? "" : " -> ");
                    cout << "\n   " << CYAN << "Tổng thời gian di chuyển: " << t << " đơn vị thời gian." << RESET << "\n";
                }
            }
            pause_cross();
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
                alt.suggestAlternative(edgeId, s, g);
            }
            pause_cross();
            clearInputBuffer();
        }
        else if (choice == 2) {
            // 3. Tối ưu hóa giao thông
            cout << CYAN << "📈 Đang chạy Phân tích Tối ưu hóa Giao thông...\n" << RESET;
            TrafficOptimization opt(map);
            opt.optimizeTraffic();
            pause_cross();
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
            pause_cross();
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