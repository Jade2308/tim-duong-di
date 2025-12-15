#include "GUI.h"
#include <cmath>
#include <algorithm>
#include <iostream>

GUI::GUI(RoadMap& map)
    : window_(nullptr)
    , renderer_(nullptr)
    , font_(nullptr)
    , fontSmall_(nullptr)
    , map_(map)
    , currentPathDistance_(0)
    , running_(false)
{
}

GUI::~GUI() {
    cleanup();
}

bool GUI::init() {
    if (!initSDL()) {
        return false;
    }
    
    initButtons();
    calculateNodePositions();
    
    statusMessage_ = "Chào mừng! Chọn điểm bắt đầu và điểm kết thúc";
    running_ = true;
    
    return true;
}

bool GUI::initSDL() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL không thể khởi tạo! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Initialize TTF
    if (TTF_Init() < 0) {
        std::cerr << "SDL_ttf không thể khởi tạo! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    
    // Create window
    window_ = SDL_CreateWindow(
        "Traffic Optimization & Pathfinding System",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (window_ == nullptr) {
        std::cerr << "Không thể tạo cửa sổ! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create renderer
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (renderer_ == nullptr) {
        std::cerr << "Không thể tạo renderer! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Load fonts
    font_ = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
    if (font_ == nullptr) {
        std::cerr << "Không thể load font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    
    fontSmall_ = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 12);
    if (fontSmall_ == nullptr) {
        std::cerr << "Không thể load font nhỏ! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    
    return true;
}

void GUI::initButtons() {
    int buttonY = WINDOW_HEIGHT - BUTTON_HEIGHT - BUTTON_MARGIN;
    int startX = BUTTON_MARGIN;
    
    buttons_.push_back(Button(startX, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, "Tìm đường ngắn nhất"));
    startX += BUTTON_WIDTH + BUTTON_MARGIN;
    
    buttons_.push_back(Button(startX, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, "Đường thay thế"));
    startX += BUTTON_WIDTH + BUTTON_MARGIN;
    
    buttons_.push_back(Button(startX, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, "Phát hiện tắc nghẽn"));
    startX += BUTTON_WIDTH + BUTTON_MARGIN;
    
    buttons_.push_back(Button(startX, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, "Reset"));
}

void GUI::calculateNodePositions() {
    auto nodeIds = map_.getNodeIds();
    if (nodeIds.empty()) {
        return;
    }
    
    // Use circular layout for better visualization
    positionNodesCircular();
}

void GUI::positionNodesCircular() {
    auto nodeIds = map_.getNodeIds();
    int numNodes = nodeIds.size();
    if (numNodes == 0) return;
    
    // Center of the circle
    int centerX = WINDOW_WIDTH / 2;
    int centerY = (WINDOW_HEIGHT - BUTTON_HEIGHT - 2 * BUTTON_MARGIN) / 2;
    
    // Radius of the circle (adjust for window size)
    int radius = std::min(centerX - 100, centerY - 100);
    
    // Angle between each node
    double angleStep = 2 * M_PI / numNodes;
    
    for (int i = 0; i < numNodes; ++i) {
        double angle = i * angleStep - M_PI / 2; // Start from top
        int x = centerX + static_cast<int>(radius * cos(angle));
        int y = centerY + static_cast<int>(radius * sin(angle));
        
        guiNodes_[nodeIds[i]] = GUINode(nodeIds[i], x, y);
    }
}

void GUI::positionNodesGrid() {
    auto nodeIds = map_.getNodeIds();
    int numNodes = nodeIds.size();
    if (numNodes == 0) return;
    
    int cols = static_cast<int>(sqrt(numNodes)) + 1;
    int rows = (numNodes + cols - 1) / cols;
    
    int marginX = 100;
    int marginY = 50;
    int spacingX = (WINDOW_WIDTH - 2 * marginX) / (cols + 1);
    int spacingY = (WINDOW_HEIGHT - BUTTON_HEIGHT - 2 * BUTTON_MARGIN - 2 * marginY) / (rows + 1);
    
    for (int i = 0; i < numNodes; ++i) {
        int row = i / cols;
        int col = i % cols;
        int x = marginX + (col + 1) * spacingX;
        int y = marginY + (row + 1) * spacingY;
        
        guiNodes_[nodeIds[i]] = GUINode(nodeIds[i], x, y);
    }
}

void GUI::run() {
    while (running_) {
        handleEvents();
        update();
        render();
        
        // Cap frame rate
        SDL_Delay(16); // ~60 FPS
    }
}

void GUI::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running_ = false;
        }
        else if (event.type == SDL_MOUSEMOTION) {
            handleMouseMotion(event.motion.x, event.motion.y);
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                handleMouseClick(event.button.x, event.button.y);
            }
        }
    }
}

void GUI::handleMouseMotion(int x, int y) {
    // Check button hover
    for (auto& button : buttons_) {
        button.isHovered = button.contains(x, y);
    }
    
    // Check node hover
    hoveredNodeId_ = findNodeAtPosition(x, y);
    for (auto& pair : guiNodes_) {
        pair.second.isHovered = (pair.first == hoveredNodeId_);
    }
}

void GUI::handleMouseClick(int x, int y) {
    // Check buttons
    for (const auto& button : buttons_) {
        if (button.contains(x, y)) {
            handleButtonClick(button);
            return;
        }
    }
    
    // Check nodes
    std::string clickedNode = findNodeAtPosition(x, y);
    if (!clickedNode.empty()) {
        if (startNodeId_.empty()) {
            startNodeId_ = clickedNode;
            guiNodes_[clickedNode].isStart = true;
            statusMessage_ = "Đã chọn điểm bắt đầu: " + clickedNode + ". Chọn điểm kết thúc.";
        }
        else if (endNodeId_.empty() && clickedNode != startNodeId_) {
            endNodeId_ = clickedNode;
            guiNodes_[clickedNode].isEnd = true;
            statusMessage_ = "Đã chọn điểm kết thúc: " + clickedNode + ". Nhấn 'Tìm đường ngắn nhất'.";
        }
    }
}

void GUI::handleButtonClick(const Button& button) {
    if (button.label == "Tìm đường ngắn nhất") {
        findShortestPath();
    }
    else if (button.label == "Đường thay thế") {
        findAlternativeRoute();
    }
    else if (button.label == "Phát hiện tắc nghẽn") {
        detectCongestion();
    }
    else if (button.label == "Reset") {
        reset();
    }
}

void GUI::update() {
    // Update logic if needed (animations, etc.)
}

void GUI::render() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer_, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, COLOR_BACKGROUND.a);
    SDL_RenderClear(renderer_);
    
    // Render map
    renderMap();
    
    // Render path if exists
    if (!currentPath_.empty()) {
        renderPath();
    }
    
    // Render buttons
    renderButtons();
    
    // Render status bar
    renderStatusBar();
    
    // Present
    SDL_RenderPresent(renderer_);
}

void GUI::renderMap() {
    renderEdges();
    renderNodes();
}

void GUI::renderEdges() {
    auto edges = map_.getEdges();
    
    for (const auto& edge : edges) {
        // Only render non-reverse edges to avoid duplicates
        if (edge.isReverse) continue;
        
        auto srcIt = guiNodes_.find(edge.src);
        auto dstIt = guiNodes_.find(edge.dst);
        
        if (srcIt == guiNodes_.end() || dstIt == guiNodes_.end()) continue;
        
        const GUINode& srcNode = srcIt->second;
        const GUINode& dstNode = dstIt->second;
        
        // Choose color based on congestion
        Color edgeColor = isEdgeCongested(edge) ? COLOR_EDGE_CONGESTED : COLOR_EDGE_NORMAL;
        
        drawLine(srcNode.x, srcNode.y, dstNode.x, dstNode.y, EDGE_WIDTH, edgeColor);
        
        // Draw weight/distance at midpoint
        int midX = (srcNode.x + dstNode.x) / 2;
        int midY = (srcNode.y + dstNode.y) / 2;
        
        std::string weightText = std::to_string(static_cast<int>(edge.length));
        renderText(weightText, midX, midY, COLOR_TEXT, fontSmall_);
    }
}

void GUI::renderNodes() {
    for (const auto& pair : guiNodes_) {
        const GUINode& node = pair.second;
        
        // Choose color
        Color nodeColor = COLOR_NODE_NORMAL;
        if (node.isStart) nodeColor = COLOR_NODE_START;
        else if (node.isEnd) nodeColor = COLOR_NODE_END;
        else if (node.isHovered) nodeColor = COLOR_NODE_HOVER;
        
        // Draw node circle
        drawCircle(node.x, node.y, NODE_RADIUS, nodeColor);
        
        // Draw node label
        renderText(node.id, node.x - 10, node.y - NODE_RADIUS - 20, COLOR_TEXT, fontSmall_);
    }
}

void GUI::renderPath() {
    if (currentPath_.size() < 2) return;
    
    for (size_t i = 0; i < currentPath_.size() - 1; ++i) {
        auto srcIt = guiNodes_.find(currentPath_[i]);
        auto dstIt = guiNodes_.find(currentPath_[i + 1]);
        
        if (srcIt == guiNodes_.end() || dstIt == guiNodes_.end()) continue;
        
        const GUINode& srcNode = srcIt->second;
        const GUINode& dstNode = dstIt->second;
        
        drawLine(srcNode.x, srcNode.y, dstNode.x, dstNode.y, PATH_WIDTH, COLOR_PATH);
    }
}

void GUI::renderButtons() {
    for (const auto& button : buttons_) {
        Color buttonColor = button.isHovered ? COLOR_BUTTON_HOVER : COLOR_BUTTON;
        drawFilledRect(button.rect, buttonColor);
        
        // Draw button text (centered)
        int textX = button.rect.x + button.rect.w / 2 - 60;
        int textY = button.rect.y + button.rect.h / 2 - 8;
        renderText(button.label, textX, textY, Color(255, 255, 255, 255), fontSmall_);
    }
}

void GUI::renderStatusBar() {
    // Draw status bar background
    SDL_Rect statusRect = {0, 0, WINDOW_WIDTH, 30};
    drawFilledRect(statusRect, Color(200, 200, 200, 255));
    
    // Draw status text
    renderText(statusMessage_, 10, 5, COLOR_TEXT, font_);
    
    // Draw distance if path exists
    if (!currentPath_.empty() && currentPathDistance_ > 0) {
        std::string distText = "Khoảng cách: " + std::to_string(static_cast<int>(currentPathDistance_));
        renderText(distText, WINDOW_WIDTH - 200, 5, COLOR_TEXT, font_);
    }
}

void GUI::renderText(const std::string& text, int x, int y, const Color& color, TTF_Font* customFont) {
    if (text.empty()) return;
    
    TTF_Font* useFont = customFont ? customFont : font_;
    
    SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(useFont, text.c_str(), sdlColor);
    
    if (surface == nullptr) {
        std::cerr << "Không thể render text! TTF_Error: " << TTF_GetError() << std::endl;
        return;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    if (texture == nullptr) {
        std::cerr << "Không thể tạo texture từ surface! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    
    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer_, texture, nullptr, &dstRect);
    
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void GUI::drawCircle(int centerX, int centerY, int radius, const Color& color) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    
    // Using Midpoint Circle Algorithm
    int x = radius;
    int y = 0;
    int radiusError = 1 - x;
    
    while (x >= y) {
        // Draw horizontal lines to fill the circle
        SDL_RenderDrawLine(renderer_, centerX - x, centerY + y, centerX + x, centerY + y);
        SDL_RenderDrawLine(renderer_, centerX - x, centerY - y, centerX + x, centerY - y);
        SDL_RenderDrawLine(renderer_, centerX - y, centerY + x, centerX + y, centerY + x);
        SDL_RenderDrawLine(renderer_, centerX - y, centerY - x, centerX + y, centerY - x);
        
        y++;
        
        if (radiusError < 0) {
            radiusError += 2 * y + 1;
        } else {
            x--;
            radiusError += 2 * (y - x + 1);
        }
    }
}

void GUI::drawLine(int x1, int y1, int x2, int y2, int width, const Color& color) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    
    // For width > 1, draw multiple parallel lines
    if (width == 1) {
        SDL_RenderDrawLine(renderer_, x1, y1, x2, y2);
    } else {
        // Calculate perpendicular offset
        double dx = x2 - x1;
        double dy = y2 - y1;
        double length = sqrt(dx * dx + dy * dy);
        if (length == 0) return;
        
        double offsetX = -dy / length * (width / 2.0);
        double offsetY = dx / length * (width / 2.0);
        
        for (int i = 0; i < width; ++i) {
            double offset = i - width / 2.0;
            int ox = static_cast<int>(offsetX * offset);
            int oy = static_cast<int>(offsetY * offset);
            SDL_RenderDrawLine(renderer_, x1 + ox, y1 + oy, x2 + ox, y2 + oy);
        }
    }
}

void GUI::drawFilledRect(const SDL_Rect& rect, const Color& color) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer_, &rect);
}

void GUI::findShortestPath() {
    if (startNodeId_.empty() || endNodeId_.empty()) {
        statusMessage_ = "Vui lòng chọn điểm bắt đầu và điểm kết thúc!";
        return;
    }
    
    ShortestPath sp(map_);
    std::vector<std::string> path;
    double distance = sp.findShortestPath(startNodeId_, endNodeId_, path);
    
    if (distance < 0) {
        statusMessage_ = "Không tìm thấy đường đi từ " + startNodeId_ + " đến " + endNodeId_;
        currentPath_.clear();
        currentPathDistance_ = 0;
    } else {
        currentPath_ = path;
        currentPathDistance_ = distance;
        statusMessage_ = "Tìm thấy đường đi ngắn nhất! Tổng thời gian: " + std::to_string(static_cast<int>(distance));
    }
}

void GUI::findAlternativeRoute() {
    if (startNodeId_.empty() || endNodeId_.empty()) {
        statusMessage_ = "Vui lòng chọn điểm bắt đầu và điểm kết thúc!";
        return;
    }
    
    // For now, just show a message
    // In a full implementation, this would integrate with AlternativeRoute class
    statusMessage_ = "Chức năng đường thay thế - Chọn edge để chặn từ console";
}

void GUI::detectCongestion() {
    // Count congested edges
    int congestedCount = 0;
    auto edges = map_.getEdges();
    for (const auto& edge : edges) {
        if (!edge.isReverse && isEdgeCongested(edge)) {
            congestedCount++;
        }
    }
    
    statusMessage_ = "Phát hiện " + std::to_string(congestedCount) + " đoạn đường tắc nghẽn (màu cam)";
}

void GUI::reset() {
    startNodeId_.clear();
    endNodeId_.clear();
    currentPath_.clear();
    currentPathDistance_ = 0;
    statusMessage_ = "Đã reset. Chọn điểm bắt đầu và điểm kết thúc";
    
    for (auto& pair : guiNodes_) {
        pair.second.isStart = false;
        pair.second.isEnd = false;
    }
}

std::string GUI::findNodeAtPosition(int x, int y) {
    for (const auto& pair : guiNodes_) {
        const GUINode& node = pair.second;
        int dx = x - node.x;
        int dy = y - node.y;
        int distSquared = dx * dx + dy * dy;
        
        if (distSquared <= NODE_RADIUS * NODE_RADIUS) {
            return node.id;
        }
    }
    return "";
}

bool GUI::isEdgeCongested(const Edge& edge) const {
    // An edge is congested if flow > capacity
    return edge.flow > edge.capacity;
}

void GUI::cleanup() {
    if (fontSmall_) {
        TTF_CloseFont(fontSmall_);
        fontSmall_ = nullptr;
    }
    
    if (font_) {
        TTF_CloseFont(font_);
        font_ = nullptr;
    }
    
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    
    TTF_Quit();
    SDL_Quit();
}
