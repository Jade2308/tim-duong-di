# Test Scenarios for Fixed Issues
# Kịch Bản Kiểm Thử Cho Các Lỗi Đã Sửa

## Test Environment / Môi Trường Kiểm Thử
- **Application**: Traffic Map Analyzer GUI (gui_app)
- **Map File**: map.txt
- **Build Command**: `make gui_app`
- **Run Command**: `./gui_app` or `./gui_app map.txt`

---

## Test Case 1: Edge ID Display on Map
## Kiểm Thử 1: Hiển Thị ID Tuyến Đường Trên Bản Đồ

### Test Steps / Các Bước:
1. Launch the application / Khởi động ứng dụng
2. Main menu should show map with nodes and edges / Menu chính hiển thị bản đồ với nodes và edges
3. Observe the map panel / Quan sát panel bản đồ

### Expected Results / Kết Quả Mong Đợi:
✓ Each edge displays its ID (E01, E02, E03, etc.) at the midpoint
✓ Edge IDs appear in light blue color (RGB 200, 200, 255)
✓ Edge IDs don't overlap with node IDs
✓ All edges show their IDs clearly

### Example Map Display:
```
    [A] ──── E01 ──── [B]
     │                 │
    E07               E02
     │                 │
    [D] ──── E08 ──── [I]
```

### Test Data:
- Use default map.txt with edges E01-E17
- Check edges in different orientations (horizontal, vertical, diagonal)

### Pass Criteria / Tiêu Chí Đạt:
- [ ] All edge IDs visible
- [ ] Edge IDs positioned correctly at midpoints
- [ ] No text overlap
- [ ] Light blue color is clearly visible

---

## Test Case 2: Accurate Travel Time Display
## Kiểm Thử 2: Hiển Thị Thời Gian Di Chuyển Chính Xác

### Test Steps / Các Bước:

#### Test 2A: Shortest Path
1. Select menu option "1. Tim duong di ngan nhat"
2. Enter start node: `A`
3. Enter goal node: `B`
4. Observe the travel time display

**Expected Result:**
- Display shows: `Thoi gian: X.XX phut` (e.g., "Thoi gian: 3.00 phut")
- NOT: `Thoi gian: 3 don vi`

#### Test 2B: Alternative Route
1. Select menu option "2. Goi y tuyen duong thay the"
2. Enter edge to block: `E01`
3. Enter start node: `A`
4. Enter goal node: `B`
5. Observe the travel time display

**Expected Result:**
- Display shows: `Thoi gian: Y.YY phut` with two decimal places
- Time is accurate based on path calculation

### Test Data:
| Start | Goal | Expected Format | Example Value |
|-------|------|----------------|---------------|
| A | B | X.XX phut | 3.00 phut |
| A | F | X.XX phut | 12.45 phut |
| D | H | X.XX phut | 8.73 phut |

### Pass Criteria / Tiêu Chí Đạt:
- [ ] Travel time shows two decimal places
- [ ] Unit is "phut" not "don vi"
- [ ] Time value is reasonable (not 0.00 or very large)
- [ ] Format is consistent across all path findings

---

## Test Case 3: Budget Validation
## Kiểm Thử 3: Xác Thực Ngân Sách

### Test Steps / Các Bước:

#### Test 3A: Negative Budget
1. Select menu option "3. Phan tich toi uu hoa giao thong"
2. Select any congested road from the list
3. Enter budget: `-100`
4. Observe the response

**Expected Result:**
- GUI dialog appears with title "Loi"
- Message line 1: "Ngan sach khong duoc am"
- Message line 2: "Vui long nhap lai gia tri hop le"
- NO error output in terminal
- Dialog can be dismissed by clicking or pressing key

#### Test 3B: Invalid Text Input
1. Select menu option "3. Phan tich toi uu hoa giao thong"
2. Select any congested road from the list
3. Enter budget: `abc` or `xyz123`
4. Observe the response

**Expected Result:**
- GUI dialog appears with title "Loi"
- Message line 1: "Ngan sach khong hop le"
- Message line 2: "Vui long nhap so"
- NO error output in terminal
- Dialog can be dismissed

#### Test 3C: Zero Budget
1. Select menu option "3. Phan tich toi uu hoa giao thong"
2. Select any congested road
3. Enter budget: `0`
4. Observe the response

**Expected Result:**
- Budget accepted (zero is valid)
- Analysis proceeds
- Shows "No feasible solution" message with minimum budget needed

#### Test 3D: Valid Small Budget
1. Select menu option "3. Phan tich toi uu hoa giao thong"
2. Select a congested road (e.g., if available)
3. Enter budget: `10` (small but valid)
4. Observe the response

**Expected Result:**
- Budget accepted
- Analysis shows either:
  - Feasible solution if budget is enough, OR
  - "No feasible solution" with minimum budget requirement
- NO terminal errors

### Test Data:
| Input | Type | Expected Behavior |
|-------|------|-------------------|
| -100 | Negative | Error dialog in GUI |
| -0.01 | Negative | Error dialog in GUI |
| abc | Invalid text | Error dialog in GUI |
| 123abc | Invalid text | Error dialog in GUI |
| 0 | Valid | Analysis proceeds |
| 10 | Valid | Analysis proceeds |
| 50000 | Valid | Analysis proceeds |

### Pass Criteria / Tiêu Chí Đạt:
- [ ] Negative budgets show error in GUI, not terminal
- [ ] Invalid text shows error in GUI, not terminal
- [ ] Error dialogs have descriptive messages
- [ ] Valid budgets (including 0) are accepted
- [ ] No crashes or exceptions

---

## Test Case 4: Button Responsiveness
## Kiểm Thử 4: Độ Nhạy Của Nút Bấm

### Test Steps / Các Bước:

#### Test 4A: Immediate Button Response
1. Select menu option "3. Phan tich toi uu hoa giao thong"
2. Wait for congested roads list to appear
3. Move mouse over different road buttons
4. Observe hover effects
5. Click on different buttons quickly

**Expected Result:**
- Hover effect appears immediately when mouse enters button
- Hover effect disappears immediately when mouse leaves button
- Button click is registered immediately (< 100ms)
- No lag or delay in button highlighting
- No need to click multiple times

#### Test 4B: Fast Sequential Clicks
1. Navigate to congested roads selection screen
2. Rapidly move mouse over multiple buttons
3. Click several buttons in quick succession (not the same button)

**Expected Result:**
- Each hover is detected correctly
- Each click is registered
- UI responds smoothly
- No "frozen" buttons
- No need to wait between clicks

#### Test 4C: Keyboard Navigation Responsiveness
1. On congested roads selection screen
2. Press ESC key

**Expected Result:**
- Immediately returns to main menu
- No delay
- No need to press ESC multiple times

### Performance Benchmarks:
| Action | Max Acceptable Delay | Target |
|--------|---------------------|---------|
| Mouse hover | 50ms | 16ms (60 FPS) |
| Button click | 100ms | 16ms (60 FPS) |
| ESC response | 100ms | Immediate |

### Pass Criteria / Tiêu Chí Đạt:
- [ ] Hover effects appear instantly
- [ ] Single click always registers
- [ ] No need for multiple clicks
- [ ] ESC key works on first press
- [ ] No lag during mouse movement
- [ ] Smooth 60 FPS rendering maintained

---

## Test Case 5: End-to-End Complete Workflow
## Kiểm Thử 5: Quy Trình Hoàn Chỉnh End-to-End

### Scenario: Full Traffic Analysis Workflow
### Kịch Bản: Quy Trình Phân Tích Giao Thông Đầy Đủ

1. **Launch Application**
   - Run: `./gui_app map.txt`
   - Verify: Map loads with edge IDs visible ✓

2. **Find Shortest Path**
   - Select: Option 1
   - Input: Start=A, Goal=F
   - Verify: Time shows as "X.XX phut" ✓

3. **Try Alternative Route**
   - Select: Option 2
   - Input: Block E03, Start=E, Goal=F
   - Verify: Alternative path found, time in "phut" format ✓

4. **Test Budget Validation**
   - Select: Option 3
   - Input: Invalid budget "-50"
   - Verify: GUI error dialog appears ✓
   - Dismiss dialog
   - Input: Valid budget "100"
   - Verify: Analysis proceeds ✓

5. **Test Button Responsiveness**
   - Hover over different road options
   - Verify: Immediate hover feedback ✓
   - Click a road
   - Verify: Immediate selection ✓

6. **Complete Analysis**
   - Review results
   - Verify: All information displayed correctly
   - Press ESC
   - Verify: Return to main menu

7. **Exit Application**
   - Select: Option 5 (Thoat)
   - Verify: Clean exit

### Full Pass Criteria:
- [ ] All 4 fixes working correctly
- [ ] No terminal errors during entire session
- [ ] Smooth user experience throughout
- [ ] All GUI dialogs work properly
- [ ] No crashes or hangs

---

## Regression Testing
## Kiểm Thử Hồi Quy

Ensure that fixes didn't break existing functionality:

### Test Existing Features Still Work:
1. **Load Map from File**
   - [ ] map.txt loads correctly
   - [ ] All nodes and edges parsed
   - [ ] No data corruption

2. **Shortest Path Algorithm**
   - [ ] Dijkstra still finds optimal path
   - [ ] Path calculation is correct
   - [ ] No infinite loops

3. **Alternative Route**
   - [ ] Blocked edges respected
   - [ ] Alternative paths found when possible
   - [ ] "No route" message when appropriate

4. **Traffic Optimization**
   - [ ] Congestion detection works
   - [ ] Proposals generated correctly
   - [ ] Budget calculations accurate

5. **UI Rendering**
   - [ ] All panels draw correctly
   - [ ] Colors are correct
   - [ ] No visual artifacts
   - [ ] Text is readable

---

## Bug Reproduction Tests (Verify Fixes)
## Kiểm Thử Tái Hiện Lỗi (Xác Minh Sửa Chữa)

### Before Fix vs After Fix

#### Bug 1: Missing Edge IDs
**Before:** Edge IDs not visible on map
**After:** All edge IDs visible at midpoints ✓

#### Bug 2: Inaccurate Time Display
**Before:** "Thoi gian: 15 don vi"
**After:** "Thoi gian: 15.47 phut" ✓

#### Bug 3: Terminal Error Output
**Before:** Budget error prints to terminal
**After:** Budget error shows in GUI dialog ✓

#### Bug 4: Unresponsive Buttons
**Before:** Need to click 2-3 times, lag
**After:** Single click works, no lag ✓

---

## Performance Testing
## Kiểm Thử Hiệu Năng

### Metrics to Monitor:

1. **Frame Rate**
   - Target: 60 FPS (16ms per frame)
   - Measure during: Map display, button hovering
   - Tool: Visual observation or FPS counter

2. **Response Time**
   - Button click to action: < 100ms
   - Input dialog to display: < 50ms
   - ESC key to menu: < 50ms

3. **Memory Usage**
   - Monitor for memory leaks
   - Should be stable during extended use
   - No gradual increase in RAM usage

4. **CPU Usage**
   - Idle: < 5%
   - Active interaction: < 25%
   - Path calculation: Spike OK, should return to normal

---

## Test Report Template
## Mẫu Báo Cáo Kiểm Thử

```
Date: ___________
Tester: ___________
Build: gui_app (commit: a79cbc7)

Test Case 1: Edge ID Display
Status: [ ] PASS [ ] FAIL
Notes: _________________________________

Test Case 2: Travel Time Accuracy
Status: [ ] PASS [ ] FAIL
Notes: _________________________________

Test Case 3: Budget Validation
Status: [ ] PASS [ ] FAIL
Notes: _________________________________

Test Case 4: Button Responsiveness
Status: [ ] PASS [ ] FAIL
Notes: _________________________________

Test Case 5: End-to-End Workflow
Status: [ ] PASS [ ] FAIL
Notes: _________________________________

Regression Tests:
Status: [ ] PASS [ ] FAIL
Issues: _________________________________

Overall Assessment:
[ ] All tests passed - Ready for production
[ ] Some tests failed - See notes
[ ] Major issues found - Further work needed

Additional Comments:
_____________________________________
_____________________________________
```

---

## Known Limitations

1. **SDL2 Requirement**: Application requires SDL2 and SDL2_ttf libraries
2. **Platform**: Built for Windows (exe) and Linux (compilation required)
3. **Font File**: Requires fonts in `assets/fonts/arial.ttf`

---

**End of Test Scenarios Document**
