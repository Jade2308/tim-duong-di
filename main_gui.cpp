// main_gui.cpp - GUI version of the traffic map application
#include <iostream>
#include <string>
#include <vector>
#include <SDL2/SDL.h>

#include "RoadMap.h"
#include "ShortestPath.h"
#include "AlternativeRoute.h"
#include "TrafficOptimization.h"
#include "GuiRenderer.h"

using namespace std;

// Menu states
enum MenuState {
    MENU_MAIN,
    MENU_SHORTEST_PATH,
    MENU_ALTERNATIVE_ROUTE,
    MENU_TRAFFIC_OPTIMIZATION,
    MENU_LOAD_MAP
};

// Input dialog for getting text input
string showInputDialog(GuiRenderer& gui, const string& prompt) {
    string input;
    bool done = false;
    
    while (!done) {
        SDL_Event event;
        while (gui.pollEvent(event)) {
            if (event.type == SDL_QUIT) {
                return "";
            }
            
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
                    done = true;
                } else if (event.key.keysym.sym == SDLK_BACKSPACE && input.length() > 0) {
                    input.pop_back();
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return "";
                }
            }
            
            if (event.type == SDL_TEXTINPUT) {
                input += event.text.text;
            }
        }
        
        // Render input dialog
        gui.clear(Color(40, 40, 50));
        
        gui.drawPanel(200, 200, 600, 200, "Input");
        gui.drawText(prompt, 220, 250, Color(255, 255, 255));
        gui.drawText("> " + input + "_", 220, 300, Color(200, 200, 255));
        gui.drawText("Press ENTER to confirm, ESC to cancel", 220, 350, Color(150, 150, 150));
        
        gui.present();
        SDL_Delay(16);  // ~60 FPS
    }
    
    return input;
}

void showMessageDialog(GuiRenderer& gui, const string& title, const vector<string>& messages) {
    bool done = false;
    
    while (!done) {
        SDL_Event event;
        while (gui.pollEvent(event)) {
            if (event.type == SDL_QUIT) {
                return;
            }
            
            if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
                done = true;
            }
        }
        
        gui.clear(Color(40, 40, 50));
        
        int panelHeight = 150 + messages.size() * 30;
        gui.drawPanel(150, 150, 700, panelHeight, title);
        
        int y = 200;
        for (const auto& msg : messages) {
            gui.drawText(msg, 170, y, Color(255, 255, 255));
            y += 30;
        }
        
        gui.drawText("Press any key or click to continue", 170, y + 20, Color(150, 150, 150));
        
        gui.present();
        SDL_Delay(16);
    }
}

void drawMainMenu(GuiRenderer& gui, RoadMap& map) {
    gui.clear(Color(40, 40, 50));
    
    // Draw title
    gui.drawTitle("HE THONG PHAN TICH BAN DO GIAO THONG");
    
    // Draw map panel
    gui.drawPanel(50, 80, 500, 500, "Ban Do Hien Tai");
    gui.drawMap(map, 80, 120, 1.0);
    
    // Draw info panel
    gui.drawPanel(570, 80, 400, 200, "Thong Tin Ban Do");
    int totalNodes = map.getNodeIds().size();
    int totalEdges = map.getEdges().size();
    
    gui.drawText("Tong Nodes: " + to_string(totalNodes), 590, 120, Color(100, 255, 100));
    gui.drawText("Tong Edges: " + to_string(totalEdges), 590, 150, Color(100, 255, 100));
    
    // Draw menu buttons
    gui.clearButtons();
    gui.addButton(Button(570, 300, 400, 50, "1. Tim duong di ngan nhat", 1));
    gui.addButton(Button(570, 360, 400, 50, "2. Goi y tuyen duong thay the", 2));
    gui.addButton(Button(570, 420, 400, 50, "3. Phan tich toi uu hoa giao thong", 3));
    gui.addButton(Button(570, 480, 400, 50, "4. Tai them ban do tu file", 4));
    gui.addButton(Button(570, 540, 400, 50, "5. Thoat", 5));
    
    // Draw buttons
    for (size_t i = 0; i < 5; i++) {
        gui.drawButton(gui.buttons[i]);
    }
    
    gui.present();
}

void handleShortestPath(GuiRenderer& gui, RoadMap& map) {
    // Get start node
    string start = showInputDialog(gui, "Nhap ID Node Bat dau (Start):");
    if (start.empty()) return;
    
    // Get goal node
    string goal = showInputDialog(gui, "Nhap ID Node Ket thuc (Goal):");
    if (goal.empty()) return;
    
    // Check if nodes exist
    if (!map.hasNode(start)) {
        showMessageDialog(gui, "Loi", {"Node bat dau '" + start + "' khong ton tai."});
        return;
    }
    
    if (!map.hasNode(goal)) {
        showMessageDialog(gui, "Loi", {"Node ket thuc '" + goal + "' khong ton tai."});
        return;
    }
    
    // Find shortest path
    ShortestPath sp(map);
    vector<string> path;
    double time = sp.findShortestPath(start, goal, path);
    
    if (time < 0) {
        showMessageDialog(gui, "Ket qua", {"Khong tim thay duong di tu " + start + " den " + goal});
    } else {
        // Show result with visualization
        bool done = false;
        while (!done) {
            SDL_Event event;
            while (gui.pollEvent(event)) {
                if (event.type == SDL_QUIT) {
                    return;
                }
                if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
                    done = true;
                }
            }
            
            gui.clear(Color(40, 40, 50));
            
            // Draw map with highlighted path
            gui.drawPanel(50, 80, 500, 500, "Duong di ngan nhat");
            gui.drawMap(map, 80, 120, 1.0);
            gui.highlightPath(map, path, 80, 120, 1.0);
            
            // Draw result panel
            gui.drawPanel(570, 80, 400, 500, "Ket qua");
            gui.drawText("Tuyen duong:", 590, 120, Color(255, 255, 100));
            
            int y = 150;
            string pathStr;
            for (size_t i = 0; i < path.size(); i++) {
                pathStr += path[i];
                if (i < path.size() - 1) pathStr += " -> ";
            }
            
            // Split long path into multiple lines
            int charsPerLine = 30;
            for (size_t i = 0; i < pathStr.length(); i += charsPerLine) {
                string line = pathStr.substr(i, charsPerLine);
                gui.drawText(line, 590, y, Color(255, 255, 255));
                y += 25;
            }
            
            gui.drawText("Thoi gian: " + to_string((int)time) + " don vi", 
                        590, y + 20, Color(100, 255, 100));
            
            gui.drawText("Press any key to continue", 590, 520, Color(150, 150, 150));
            
            gui.present();
            SDL_Delay(16);
        }
    }
}

void handleAlternativeRoute(GuiRenderer& gui, RoadMap& map) {
    // Get edge to block
    string edgeId = showInputDialog(gui, "Nhap ID Edge can chan:");
    if (edgeId.empty()) return;
    
    if (!map.hasEdge(edgeId)) {
        showMessageDialog(gui, "Loi", {"Edge '" + edgeId + "' khong ton tai."});
        return;
    }
    
    // Get start node
    string start = showInputDialog(gui, "Nhap ID Node Bat dau (Start):");
    if (start.empty()) return;
    
    // Get goal node
    string goal = showInputDialog(gui, "Nhap ID Node Ket thuc (Goal):");
    if (goal.empty()) return;
    
    if (!map.hasNode(start) || !map.hasNode(goal)) {
        showMessageDialog(gui, "Loi", {"Node bat dau hoac ket thuc khong ton tai."});
        return;
    }
    
    // Find alternative route
    gui.clear(Color(40, 40, 50));
    gui.drawText("Dang tim duong di thay the...", 400, 300, Color(255, 255, 255));
    gui.present();
    
    AlternativeRoute alt(map);
    // Note: AlternativeRoute prints to console, we'd need to modify it to return data
    // For now, just call it and show a message
    alt.suggestAlternative(edgeId, start, goal);
    
    showMessageDialog(gui, "Ket qua", {
        "Da tim xong duong di thay the khi chan Edge " + edgeId,
        "Ket qua hien thi trong console (terminal)."
    });
}

void handleTrafficOptimization(GuiRenderer& gui, RoadMap& map) {
    gui.clear(Color(40, 40, 50));
    gui.drawText("Dang chay phan tich toi uu hoa giao thong...", 300, 300, Color(255, 255, 255));
    gui.present();
    
    TrafficOptimization opt(map);
    opt.optimizeTraffic();
    
    showMessageDialog(gui, "Ket qua", {
        "Da hoan thanh phan tich toi uu hoa giao thong.",
        "Ket qua chi tiet hien thi trong console (terminal)."
    });
}

void handleLoadMap(GuiRenderer& gui, RoadMap& map) {
    string filename = showInputDialog(gui, "Nhap ten file ban do:");
    if (filename.empty()) return;
    
    if (map.loadFromFile(filename)) {
        showMessageDialog(gui, "Thanh cong", {
            "Tai ban do tu " + filename + " thanh cong!",
            "Tong " + to_string(map.getNodeIds().size()) + " nodes",
            "Tong " + to_string(map.getEdges().size()) + " edges"
        });
    } else {
        showMessageDialog(gui, "Loi", {
            "Khong the tai file: " + filename,
            "Kiem tra ten file va dinh dang."
        });
    }
}

int main(int argc, char* argv[]) {
    // Initialize GUI
    GuiRenderer gui;
    if (!gui.init("He Thong Phan Tich Ban Do Giao Thong", 1000, 600)) {
        cerr << "Failed to initialize GUI!" << endl;
        return 1;
    }
    
    // Initialize map
    RoadMap map;
    
    // Try to load map from command line argument or default file
    string mapFile = "map.txt";
    if (argc > 1) {
        mapFile = argv[1];
        cout << "Loading map from command line argument: " << mapFile << endl;
    }
    
    if (map.loadFromFile(mapFile)) {
        cout << "Loaded " << mapFile << " successfully!" << endl;
    } else {
        cout << "Could not load " << mapFile << ", starting with empty map." << endl;
    }
    
    // Main event loop
    bool quit = false;
    MenuState state = MENU_MAIN;
    
    // Enable text input
    SDL_StartTextInput();
    
    while (!quit) {
        SDL_Event event;
        
        // Handle events
        while (gui.pollEvent(event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            
            if (state == MENU_MAIN) {
                if (event.type == SDL_MOUSEMOTION) {
                    gui.handleMouseMotion(event.motion.x, event.motion.y);
                }
                
                if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                    int buttonId = gui.handleMouseClick(event.button.x, event.button.y);
                    
                    switch (buttonId) {
                        case 1:
                            handleShortestPath(gui, map);
                            break;
                        case 2:
                            handleAlternativeRoute(gui, map);
                            break;
                        case 3:
                            handleTrafficOptimization(gui, map);
                            break;
                        case 4:
                            handleLoadMap(gui, map);
                            break;
                        case 5:
                            quit = true;
                            break;
                    }
                }
            }
        }
        
        // Render
        if (state == MENU_MAIN) {
            drawMainMenu(gui, map);
        }
        
        SDL_Delay(16);  // ~60 FPS
    }
    
    SDL_StopTextInput();
    
    return 0;
}
