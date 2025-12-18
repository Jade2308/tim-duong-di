#include "GuiRenderer.h"
#include <iostream>
#include <cmath>
#include <algorithm>

GuiRenderer::GuiRenderer()
    : windowWidth(0), windowHeight(0) {
}

GuiRenderer::~GuiRenderer() {
    cleanup();
}

bool GuiRenderer::init(const std::string& title, int width, int height) {
    windowWidth = width;
    windowHeight = height;
    
    // Create SFML window
    window.create(sf::VideoMode(width, height), title, sf::Style::Titlebar | sf::Style::Close);
    if (!window.isOpen()) {
        std::cerr << "Window could not be created!" << std::endl;
        return false;
    }
    
    // Set framerate limit
    window.setFramerateLimit(60);
    
    // Load fonts - try to use a default system font
    bool fontLoaded = false;
    
    // Try DejaVu Sans first (common on Linux)
    if (font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        fontLoaded = true;
    }
    // Try Liberation Sans (alternative on Linux)
    else if (font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
        fontLoaded = true;
    }
    // Try FreeSans (another common Linux font)
    else if (font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeSans.ttf")) {
        fontLoaded = true;
    }
    // Try Windows fonts
    else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        fontLoaded = true;
    }
    
    if (!fontLoaded) {
        std::cerr << "Failed to load font! Text will not be rendered." << std::endl;
        std::cerr << "Continuing without font support." << std::endl;
    }
    
    // Try to load bold font for titles
    bool titleFontLoaded = false;
    if (titleFont.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        titleFontLoaded = true;
    }
    else if (titleFont.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf")) {
        titleFontLoaded = true;
    }
    else if (titleFont.loadFromFile("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf")) {
        titleFontLoaded = true;
    }
    else if (titleFont.loadFromFile("C:/Windows/Fonts/arialbd.ttf")) {
        titleFontLoaded = true;
    }
    
    if (!titleFontLoaded && fontLoaded) {
        titleFont = font; // Use regular font if bold is not available
    }
    
    return true;
}

void GuiRenderer::cleanup() {
    if (window.isOpen()) {
        window.close();
    }
}

bool GuiRenderer::pollEvent(sf::Event& event) {
    return window.pollEvent(event);
}

void GuiRenderer::handleMouseMotion(int x, int y) {
    for (auto& button : buttons) {
        button.hovered = button.contains(x, y);
    }
}

int GuiRenderer::handleMouseClick(int x, int y) {
    for (const auto& button : buttons) {
        if (button.contains(x, y)) {
            return button.id;
        }
    }
    return -1;
}

void GuiRenderer::clear(const Color& color) {
    window.clear(color.toSFColor());
}

void GuiRenderer::present() {
    window.display();
}

void GuiRenderer::drawText(const std::string& text, int x, int y, const Color& color, int fontSize) {
    // Create SFML text object
    sf::Text sfText;
    sfText.setFont(font);
    sfText.setString(text);
    sfText.setCharacterSize(fontSize);
    sfText.setFillColor(color.toSFColor());
    sfText.setPosition(static_cast<float>(x), static_cast<float>(y));
    
    window.draw(sfText);
}

void GuiRenderer::drawRect(int x, int y, int w, int h, const Color& color, bool filled) {
    sf::RectangleShape rect(sf::Vector2f(static_cast<float>(w), static_cast<float>(h)));
    rect.setPosition(static_cast<float>(x), static_cast<float>(y));
    
    if (filled) {
        rect.setFillColor(color.toSFColor());
    } else {
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineColor(color.toSFColor());
        rect.setOutlineThickness(1.0f);
    }
    
    window.draw(rect);
}

void GuiRenderer::drawLine(int x1, int y1, int x2, int y2, const Color& color, int thickness) {
    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(static_cast<float>(x1), static_cast<float>(y1)), color.toSFColor()),
        sf::Vertex(sf::Vector2f(static_cast<float>(x2), static_cast<float>(y2)), color.toSFColor())
    };
    
    if (thickness <= 1) {
        window.draw(line, 2, sf::Lines);
    } else {
        // For thick lines, draw a rectangle rotated along the line
        float length = std::sqrt(static_cast<float>((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)));
        float angle = std::atan2(static_cast<float>(y2-y1), static_cast<float>(x2-x1)) * 180.0f / 3.14159265f;
        
        sf::RectangleShape thickLine(sf::Vector2f(length, static_cast<float>(thickness)));
        thickLine.setPosition(static_cast<float>(x1), static_cast<float>(y1));
        thickLine.setRotation(angle);
        thickLine.setFillColor(color.toSFColor());
        
        window.draw(thickLine);
    }
}

void GuiRenderer::drawCircle(int cx, int cy, int radius, const Color& color, bool filled) {
    sf::CircleShape circle(static_cast<float>(radius));
    circle.setPosition(static_cast<float>(cx - radius), static_cast<float>(cy - radius));
    
    if (filled) {
        circle.setFillColor(color.toSFColor());
    } else {
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineColor(color.toSFColor());
        circle.setOutlineThickness(1.5f);
    }
    
    window.draw(circle);
}

void GuiRenderer::drawButton(const Button& button) {
    // Draw button background
    Color bgColor = button.hovered ? Color(70, 130, 180) : Color(50, 100, 150);
    drawRect(static_cast<int>(button.rect.left), static_cast<int>(button.rect.top), 
             static_cast<int>(button.rect.width), static_cast<int>(button.rect.height), 
             bgColor, true);
    
    // Draw button border
    drawRect(static_cast<int>(button.rect.left), static_cast<int>(button.rect.top), 
             static_cast<int>(button.rect.width), static_cast<int>(button.rect.height), 
             Color(200, 200, 200), false);
    
    // Draw button text (centered)
    sf::Text text;
    text.setFont(font);
    text.setString(button.label);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);
    
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(
        button.rect.left + (button.rect.width - textBounds.width) / 2.0f - textBounds.left,
        button.rect.top + (button.rect.height - textBounds.height) / 2.0f - textBounds.top
    );
    
    window.draw(text);
}

void GuiRenderer::addButton(const Button& button) {
    buttons.push_back(button);
}

void GuiRenderer::clearButtons() {
    buttons.clear();
}

void GuiRenderer::latLonToScreen(double lat, double lon, int& x, int& y, 
                                 int offsetX, int offsetY, double scale) {
    // Simple conversion: treat lat/lon as cartesian coordinates
    // Scale and offset to fit in window
    x = static_cast<int>((lon - DEFAULT_CENTER_LON) * scale * 1000) + offsetX;
    y = static_cast<int>((DEFAULT_CENTER_LAT - lat) * scale * 1000) + offsetY;  // Inverted Y axis
}

void GuiRenderer::drawMap(RoadMap& map, int offsetX, int offsetY, double scale) {
    auto nodeIds = map.getNodeIds();
    auto edges = map.getEdges();
    
    if (nodeIds.empty()) {
        drawText("Ban do trong", offsetX + 150, offsetY + 100, Color(150, 150, 150));
        return;
    }
    
    // Draw edges first (so they appear behind nodes)
    for (const auto& edge : edges) {
        if (!edge.isReverse) {  // Only draw original edges, not reverse
            auto srcNode = map.getNodeById(edge.src);
            auto dstNode = map.getNodeById(edge.dst);
            
            if (srcNode && dstNode) {
                int x1, y1, x2, y2;
                latLonToScreen(srcNode->lat, srcNode->lon, x1, y1, offsetX, offsetY, scale);
                latLonToScreen(dstNode->lat, dstNode->lon, x2, y2, offsetX, offsetY, scale);
                
                // Color based on traffic flow
                Color edgeColor(100, 100, 100);
                if (edge.flow > edge.capacity * 0.9) {
                    edgeColor = Color(200, 50, 50);  // Red for congested
                } else if (edge.flow > edge.capacity * 0.7) {
                    edgeColor = Color(200, 150, 50); // Orange for busy
                } else {
                    edgeColor = Color(50, 150, 50);  // Green for clear
                }
                
                drawLine(x1, y1, x2, y2, edgeColor, 3);
                
                // Draw edge label at midpoint
                int midX = (x1 + x2) / 2;
                int midY = (y1 + y2) / 2;
                drawText(edge.id, midX + 5, midY - 10, Color(180, 180, 180), 14);
            }
        }
    }
    
    // Draw nodes on top
    for (const auto& nodeId : nodeIds) {
        auto node = map.getNodeById(nodeId);
        if (node) {
            int x, y;
            latLonToScreen(node->lat, node->lon, x, y, offsetX, offsetY, scale);
            
            drawCircle(x, y, 8, Color(70, 130, 180), true);
            drawCircle(x, y, 8, Color(255, 255, 255), false);
            
            // Draw node label
            drawText(nodeId, x + 12, y - 5, Color(255, 255, 255));
        }
    }
}

void GuiRenderer::drawMapNode(const std::string& nodeId, double x, double y, 
                              const Color& color, int radius) {
    drawCircle(static_cast<int>(x), static_cast<int>(y), radius, color, true);
    drawCircle(static_cast<int>(x), static_cast<int>(y), radius, Color(255, 255, 255), false);
    drawText(nodeId, static_cast<int>(x) + radius + 5, static_cast<int>(y) - 5, Color(255, 255, 255));
}

void GuiRenderer::drawMapEdge(double x1, double y1, double x2, double y2, 
                              const Color& color, int thickness) {
    drawLine(static_cast<int>(x1), static_cast<int>(y1), 
             static_cast<int>(x2), static_cast<int>(y2), color, thickness);
}

void GuiRenderer::highlightPath(RoadMap& map, const std::vector<std::string>& path, 
                                int offsetX, int offsetY, double scale) {
    if (path.size() < 2) return;
    
    // Draw highlighted path
    for (size_t i = 0; i < path.size() - 1; i++) {
        auto srcNode = map.getNodeById(path[i]);
        auto dstNode = map.getNodeById(path[i+1]);
        
        if (srcNode && dstNode) {
            int x1, y1, x2, y2;
            latLonToScreen(srcNode->lat, srcNode->lon, x1, y1, offsetX, offsetY, scale);
            latLonToScreen(dstNode->lat, dstNode->lon, x2, y2, offsetX, offsetY, scale);
            
            drawLine(x1, y1, x2, y2, Color(255, 200, 0), 5);  // Yellow highlight
        }
    }
    
    // Draw path nodes
    for (const auto& nodeId : path) {
        auto node = map.getNodeById(nodeId);
        if (node) {
            int x, y;
            latLonToScreen(node->lat, node->lon, x, y, offsetX, offsetY, scale);
            
            drawCircle(x, y, 10, Color(255, 200, 0), true);
            drawCircle(x, y, 10, Color(255, 255, 255), false);
        }
    }
}

void GuiRenderer::drawTitle(const std::string& title) {
    sf::Text text;
    text.setFont(titleFont);
    text.setString(title);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition((windowWidth - textBounds.width) / 2.0f - textBounds.left, 20.0f);
    
    window.draw(text);
}

void GuiRenderer::drawPanel(int x, int y, int w, int h, const std::string& title) {
    // Draw panel background
    drawRect(x, y, w, h, Color(30, 30, 40, 200), true);
    
    // Draw panel border
    drawRect(x, y, w, h, Color(100, 100, 120), false);
    
    // Draw panel title
    if (!title.empty()) {
        drawRect(x, y, w, 30, Color(50, 50, 70), true);
        drawText(title, x + 10, y + 5, Color(255, 255, 255));
    }
}
