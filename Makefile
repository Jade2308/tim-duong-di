# Makefile for Traffic Optimization System with GUI

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
SDL_CFLAGS = $(shell pkg-config --cflags sdl2 SDL2_ttf 2>/dev/null || echo "-I/usr/include/SDL2 -D_REENTRANT")
SDL_LIBS = $(shell pkg-config --libs sdl2 SDL2_ttf 2>/dev/null || echo "-lSDL2 -lSDL2_ttf")

# Source files
SOURCES = main.cpp RoadMap.cpp ShortestPath.cpp AlternativeRoute.cpp TrafficOptimization.cpp GUI.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Target executable
TARGET = traffic_gui

# Build rules
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SDL_LIBS) -lm

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -c $< -o $@

# Dependencies
main.o: main.cpp RoadMap.h ShortestPath.h AlternativeRoute.h TrafficOptimization.h GUI.h
RoadMap.o: RoadMap.cpp RoadMap.h
ShortestPath.o: ShortestPath.cpp ShortestPath.h RoadMap.h
AlternativeRoute.o: AlternativeRoute.cpp AlternativeRoute.h RoadMap.h
TrafficOptimization.o: TrafficOptimization.cpp TrafficOptimization.h RoadMap.h
GUI.o: GUI.cpp GUI.h RoadMap.h ShortestPath.h AlternativeRoute.h TrafficOptimization.h

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Rebuild everything
rebuild: clean all

# Run the program
run: $(TARGET)
	./$(TARGET)

# Install dependencies (Linux only)
install-deps:
	@echo "Installing SDL2 dependencies..."
	sudo apt-get update
	sudo apt-get install -y libsdl2-dev libsdl2-ttf-dev

# Help
help:
	@echo "Available targets:"
	@echo "  all          - Build the project (default)"
	@echo "  clean        - Remove build artifacts"
	@echo "  rebuild      - Clean and rebuild everything"
	@echo "  run          - Build and run the program"
	@echo "  install-deps - Install SDL2 dependencies (Linux)"
	@echo "  help         - Show this help message"

.PHONY: all clean rebuild run install-deps help
