# Visual Guide to Changes / Hướng Dẫn Trực Quan Về Các Thay Đổi

## 1. Map Display - Route ID Visibility

### Before / Trước:
```
Map showing:
  [Node A] ────────────── [Node B]
      ↓                        ↓
   Only node IDs visible
```

### After / Sau:
```
Map showing:
  [Node A] ──── E01 ───── [Node B]
      ↓          ↓             ↓
   Node ID   Edge ID      Node ID
   (all visible on map)
```

**Implementation:**
- Edge ID displayed at midpoint: `(x1+x2)/2, (y1+y2)/2`
- Color: Light blue `RGB(200, 200, 255)` for visibility
- Position: Offset +5, -5 from midpoint to avoid overlap

---

## 2. Travel Time Display Accuracy

### Before / Trước:
```
Display: "Thoi gian: 15 don vi"
         ^^^^^^^^^^^^^^^^^^^^^^^
         - Integer only (no decimals)
         - Unclear unit ("don vi")
```

### After / Sau:
```
Display: "Thoi gian: 15.47 phut"
         ^^^^^^^^^^^^^^^^^^^^^^^^
         - Two decimal places (%.2f)
         - Clear unit ("phut" = minutes)
```

**Technical Change:**
```cpp
// OLD
gui.drawText("Thoi gian: " + to_string((int)time) + " don vi", ...);

// NEW
char timeBuffer[50];
snprintf(timeBuffer, sizeof(timeBuffer), "Thoi gian: %.2f phut", time);
gui.drawText(timeBuffer, ...);
```

---

## 3. Budget Validation Flow

### Before / Trước:
```
User Input: -100
     ↓
No validation
     ↓
Error printed to terminal ❌
(User doesn't see error in GUI)
```

### After / Sau:
```
User Input: -100
     ↓
Validation check
     ↓
if (budget < 0) → Show GUI Error Dialog ✓
     ↓
"Ngan sach khong duoc am"
"Vui long nhap lai gia tri hop le"
```

**Validation Logic:**
```cpp
try {
    budget = stod(budgetStr);
    
    if (budget < 0) {
        showMessageDialog(gui, "Loi", {
            "Ngan sach khong duoc am", 
            "Vui long nhap lai gia tri hop le"
        });
        return;  // Stop processing
    }
} catch (...) {
    showMessageDialog(gui, "Loi", {
        "Ngan sach khong hop le", 
        "Vui long nhap so"
    });
    return;  // Stop processing
}
```

---

## 4. Event Handling Fix - Button Responsiveness

### Before / Trước - PROBLEMATIC:
```
Main Loop {
    Event Loop 1 {
        pollEvent() ← Gets some events
        Handle quit, escape
    }
    
    Draw UI
    
    Event Loop 2 {  ← PROBLEM!
        pollEvent() ← Tries to get more events
        Handle mouse, clicks
    }
    
    ⚠️ Issue: Some events processed in loop 1 are lost by loop 2
    ⚠️ Result: Buttons become unresponsive
}
```

### After / Sau - FIXED:
```
Main Loop {
    Event Loop (Single) {
        pollEvent() ← Gets ALL events
        Handle ALL events:
          - quit
          - escape
          - mouse motion  ← Now processed immediately
          - mouse clicks  ← Now processed immediately
    }
    
    Draw UI
    
    ✓ All events processed in one place
    ✓ No event loss
    ✓ Immediate button response
}
```

**Code Structure:**
```cpp
while (selectingRoad) {
    SDL_Event event;
    
    // SINGLE event loop - processes ALL events
    while (gui.pollEvent(event)) {
        if (event.type == SDL_QUIT) return;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) return;
        
        // Mouse handling now in SAME loop
        if (event.type == SDL_MOUSEMOTION) {
            gui.handleMouseMotion(event.motion.x, event.motion.y);
        }
        
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int btnId = gui.handleMouseClick(event.button.x, event.button.y);
            if (btnId >= 0) {
                selectedIndex = btnId;
                selectingRoad = false;
            }
        }
    }
    
    // Draw UI
    gui.clear(...);
    // ... drawing code ...
    gui.present();
    SDL_Delay(16);  // ~60 FPS
}
```

---

## Event Processing Timeline Comparison

### Before (Problematic):
```
Time: 0ms    16ms   32ms   48ms   64ms
      |      |      |      |      |
User clicks button at 5ms
      ↓
Event created
      |
Loop 1 polls (gets quit/escape only)
      |
Loop 2 tries to poll (event might be lost)
      |
Button click missed! ❌
      |
User clicks again at 25ms
      |
Sometimes works, sometimes doesn't 😕
```

### After (Fixed):
```
Time: 0ms    16ms   32ms   48ms   64ms
      |      |      |      |      |
User clicks button at 5ms
      ↓
Event created
      |
Single loop polls ALL events
      ↓
Button click processed immediately ✓
      |
Selection made, response instant! 🎯
```

---

## Color Coding on Map

### Edge Colors (unchanged, for reference):
- **Green** `RGB(50, 150, 50)`: Normal flow (< 70% capacity)
- **Orange** `RGB(200, 150, 50)`: High flow (70-90% capacity)
- **Red** `RGB(200, 50, 50)`: Congested (> 90% capacity)

### Text Colors:
- **White** `RGB(255, 255, 255)`: Node IDs, general text
- **Light Blue** `RGB(200, 200, 255)`: Edge IDs (NEW!)
- **Yellow** `RGB(255, 255, 100)`: Section headers
- **Green** `RGB(100, 255, 100)`: Positive info (time, stats)

---

## File Change Summary

```
GuiRenderer.cpp
├── Function: drawMap()
│   └── Added: Edge ID display at midpoint
│       Lines: +5
│       Impact: Visual enhancement
│
main_gui.cpp
├── Added: #include <cstdio>
│   Impact: Required for snprintf()
│
├── Function: handleShortestPath()
│   └── Modified: Time display format
│       Lines: -2, +4
│       Impact: Better accuracy
│
├── Function: handleAlternativeRoute()
│   └── Modified: Time display format
│       Lines: -2, +4
│       Impact: Better accuracy
│
└── Function: handleTrafficOptimization()
    ├── Modified: Event loop consolidation
    │   Lines: -18, +12
    │   Impact: Fixed responsiveness
    │
    └── Added: Budget validation
        Lines: +5
        Impact: Better error handling

Total: 35 additions, 25 deletions
```

---

## Testing Checklist

- [ ] Map displays all edge IDs clearly
- [ ] Edge IDs don't overlap with node IDs
- [ ] Travel time shows 2 decimal places
- [ ] Travel time unit says "phut" not "don vi"
- [ ] Entering negative budget shows error dialog
- [ ] Entering text as budget shows error dialog
- [ ] Error dialogs display in GUI, not terminal
- [ ] Buttons respond immediately to clicks
- [ ] Mouse hover effects work smoothly
- [ ] No lag when selecting congested roads
- [ ] ESC key works to cancel operations
- [ ] All menu navigation works smoothly

---

## Performance Impact

All changes are **minimal performance impact**:

1. **Edge ID Display**: O(E) where E = number of edges
   - Adds one text draw per edge
   - Negligible impact on modern systems

2. **Time Format**: O(1)
   - snprintf is fast, single call per display
   - No performance difference

3. **Budget Validation**: O(1)
   - Simple comparison, instant check
   - Actually improves UX by preventing bad data

4. **Event Loop Fix**: **Positive impact!**
   - Removed duplicate polling
   - More efficient event processing
   - Better responsiveness

---

## Compatibility

- **C++ Standard**: C++17 (no change)
- **SDL2 Version**: Any (no API changes)
- **Platform**: Windows/Linux/Mac (unchanged)
- **Backward Compatibility**: 100% (no breaking changes)
- **Data Format**: Compatible with existing map.txt files

---

## Future Enhancements (Optional)

If you want to improve further:

1. **Edge ID Display**:
   - Add option to toggle edge IDs on/off
   - Add edge name next to ID for long routes

2. **Time Display**:
   - Show hours:minutes format for long routes
   - Add estimated arrival time

3. **Budget Input**:
   - Add slider for budget selection
   - Show minimum required budget before input

4. **Button Feedback**:
   - Add click animation
   - Add progress bar for long operations

---

**End of Visual Guide**
