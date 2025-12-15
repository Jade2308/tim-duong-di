#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "RoadMap.h"
#include "ShortestPath.h"
#include "AlternativeRoute.h"
#include "TrafficOptimization.h"

// Constants for GUI
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const int NODE_RADIUS = 25;
const int EDGE_WIDTH = 2;
const int PATH_WIDTH = 5;
const int BUTTON_HEIGHT = 40;
const int BUTTON_WIDTH = 200;
const int BUTTON_MARGIN = 10;

// Colors
struct Color {
    Uint8 r, g, b, a;
    Color(Uint8 red = 255, Uint8 green = 255, Uint8 blue = 255, Uint8 alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}
};

// Color constants
const Color COLOR_BACKGROUND(240, 240, 240, 255);
const Color COLOR_NODE_NORMAL(100, 149, 237, 255);    // Cornflower blue
const Color COLOR_NODE_START(34, 139, 34, 255);       // Forest green
const Color COLOR_NODE_END(220, 20, 60, 255);         // Crimson red
const Color COLOR_NODE_HOVER(255, 215, 0, 255);       // Gold
const Color COLOR_EDGE_NORMAL(128, 128, 128, 255);    // Gray
const Color COLOR_EDGE_CONGESTED(255, 165, 0, 255);   // Orange
const Color COLOR_PATH(30, 144, 255, 255);            // Dodger blue
const Color COLOR_TEXT(0, 0, 0, 255);                 // Black
const Color COLOR_BUTTON(70, 130, 180, 255);          // Steel blue
const Color COLOR_BUTTON_HOVER(100, 149, 237, 255);   // Cornflower blue

// GUI node structure (visual representation)
struct GUINode {
    std::string id;
    int x;
    int y;
    bool isStart;
    bool isEnd;
    bool isHovered;
    
    GUINode() : x(0), y(0), isStart(false), isEnd(false), isHovered(false) {}
    GUINode(const std::string& nodeId, int posX, int posY) 
        : id(nodeId), x(posX), y(posY), isStart(false), isEnd(false), isHovered(false) {}
};

// GUI button structure
struct Button {
    SDL_Rect rect;
    std::string label;
    bool isHovered;
    
    Button(int x, int y, int w, int h, const std::string& text)
        : rect{x, y, w, h}, label(text), isHovered(false) {}
    
    bool contains(int px, int py) const {
        return px >= rect.x && px <= rect.x + rect.w &&
               py >= rect.y && py <= rect.y + rect.h;
    }
};

class GUI {
public:
    GUI(RoadMap& map);
    ~GUI();
    
    bool init();
    void run();
    void cleanup();
    
private:
    // SDL components
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    TTF_Font* font_;
    TTF_Font* fontSmall_;
    
    // Data references
    RoadMap& map_;
    
    // GUI state
    std::unordered_map<std::string, GUINode> guiNodes_;
    std::vector<Button> buttons_;
    std::string startNodeId_;
    std::string endNodeId_;
    std::vector<std::string> currentPath_;
    double currentPathDistance_;
    std::string statusMessage_;
    std::string hoveredNodeId_;
    std::string hoveredEdgeId_;
    bool running_;
    
    // Private methods
    bool initSDL();
    void initButtons();
    void calculateNodePositions();
    
    void handleEvents();
    void handleMouseMotion(int x, int y);
    void handleMouseClick(int x, int y);
    void handleButtonClick(const Button& button);
    
    void update();
    void render();
    
    void renderMap();
    void renderEdges();
    void renderNodes();
    void renderPath();
    void renderButtons();
    void renderText(const std::string& text, int x, int y, const Color& color, TTF_Font* font = nullptr);
    void renderStatusBar();
    
    void drawCircle(int centerX, int centerY, int radius, const Color& color);
    void drawLine(int x1, int y1, int x2, int y2, int width, const Color& color);
    void drawFilledRect(const SDL_Rect& rect, const Color& color);
    
    void findShortestPath();
    void findAlternativeRoute();
    void detectCongestion();
    void reset();
    
    std::string findNodeAtPosition(int x, int y);
    bool isEdgeCongested(const Edge& edge) const;
    
    // Helper for positioning nodes in a circle/grid
    void positionNodesCircular();
    void positionNodesGrid();
};
