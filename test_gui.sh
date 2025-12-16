#!/bin/bash
# Test script for GUI application

echo "=== GUI Application Build and Test ==="
echo ""

# Test 1: Check if binary exists
echo "Test 1: Checking if gui_app binary exists..."
if [ -f gui_app ]; then
    echo "✓ PASS: gui_app binary found"
else
    echo "✗ FAIL: gui_app binary not found"
    exit 1
fi

# Test 2: Check if map.txt exists
echo ""
echo "Test 2: Checking if map.txt exists..."
if [ -f map.txt ]; then
    echo "✓ PASS: map.txt found"
else
    echo "⚠ WARNING: map.txt not found (optional)"
fi

# Test 3: Verify binary is executable
echo ""
echo "Test 3: Checking if gui_app is executable..."
if [ -x gui_app ]; then
    echo "✓ PASS: gui_app is executable"
else
    echo "✗ FAIL: gui_app is not executable"
    exit 1
fi

# Test 4: Check dependencies
echo ""
echo "Test 4: Checking SDL2 dependencies..."

# Check platform for appropriate dependency tool
if command -v ldd &> /dev/null; then
    # Linux
    if ldd gui_app | grep -q "libSDL2"; then
        echo "✓ PASS: SDL2 library linked"
    else
        echo "✗ FAIL: SDL2 library not linked"
        exit 1
    fi
    
    if ldd gui_app | grep -q "libSDL2_ttf"; then
        echo "✓ PASS: SDL2_ttf library linked"
    else
        echo "✗ FAIL: SDL2_ttf library not linked"
        exit 1
    fi
elif command -v otool &> /dev/null; then
    # macOS
    if otool -L gui_app | grep -q "SDL2"; then
        echo "✓ PASS: SDL2 library linked"
    else
        echo "✗ FAIL: SDL2 library not linked"
        exit 1
    fi
    
    if otool -L gui_app | grep -q "SDL2_ttf"; then
        echo "✓ PASS: SDL2_ttf library linked"
    else
        echo "✗ FAIL: SDL2_ttf library not linked"
        exit 1
    fi
else
    echo "⚠ WARNING: Cannot check dependencies (ldd/otool not available)"
fi

# Test 5: Verify source files
echo ""
echo "Test 5: Checking source files..."
required_files=("GuiRenderer.h" "GuiRenderer.cpp" "main_gui.cpp" "RoadMap.h" "RoadMap.cpp" "Makefile")
all_found=true
for file in "${required_files[@]}"; do
    if [ -f "$file" ]; then
        echo "  ✓ $file"
    else
        echo "  ✗ $file (missing)"
        all_found=false
    fi
done

if $all_found; then
    echo "✓ PASS: All required source files found"
else
    echo "✗ FAIL: Some source files missing"
    exit 1
fi

echo ""
echo "=== All Tests Passed! ==="
echo ""
echo "To run the GUI application:"
echo "  ./gui_app"
echo ""
echo "Note: The application requires a display (X11/Wayland)."
echo "On headless servers, you can use Xvfb:"
echo "  xvfb-run ./gui_app"
