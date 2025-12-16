# Makefile for Traffic Map Application

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LIBS_GUI = -lSDL2 -lSDL2_ttf

# Source files
COMMON_SRCS = RoadMap.cpp ShortestPath.cpp AlternativeRoute.cpp TrafficOptimization.cpp
GUI_SRCS = main_gui.cpp GuiRenderer.cpp $(COMMON_SRCS)

# Targets
all: gui_app

gui_app: $(GUI_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(GUI_SRCS) $(LIBS_GUI)

clean:
	rm -f gui_app *.o

.PHONY: all clean
