# Test Cases for New Recommendation System

## Overview
The new recommendation system has been updated to provide three types of recommendations based on budget and road conditions:

1. **Build New Road**: When budget >= road cost and traffic exceeds capacity
2. **Expand Lanes**: When budget is between 1/3 and less than road cost and traffic exceeds capacity
3. **Direct Bypass**: When adjacent roads are all congested and budget > 2/3 of total cost

## Test Scenarios

### Scenario 1: Build New Road (Budget >= Road Cost)
**Input:**
- Congested Road: E01 (cost = 40000 tỷ VNĐ)
- Current flow: 1200 xe/giờ
- Capacity: 1500 xe/giờ
- Budget: 50000 tỷ VNĐ

**Expected Output:**
- Recommendation: Build new road (bypass route)
- Reasoning: Budget (50000 tỷ) >= road cost (40000 tỷ)
- The system should find intermediate nodes to create bypass routes

**Note:** E01 is not over capacity (1200 < 1500), so no recommendation will be given unless flow > capacity

### Scenario 2: Build New Road (Congested Road)
**Input:**
- Congested Road: E03 (cost = 80000 tỷ VNĐ)
- Current flow: 2300 xe/giờ
- Capacity: 2000 xe/giờ (OVER CAPACITY!)
- Budget: 100000 tỷ VNĐ

**Expected Output:**
- Recommendation: Build new road (bypass route)
- Reasoning: Budget (100000 tỷ) >= road cost (80000 tỷ)
- Traffic is over capacity (2300 > 2000)
- System will suggest building bypass through intermediate nodes

### Scenario 3: Expand Lanes
**Input:**
- Congested Road: E04 (cost = 30000 tỷ VNĐ)
- Current flow: 1300 xe/giờ
- Capacity: 1000 xe/giờ (OVER CAPACITY!)
- Budget: 15000 tỷ VNĐ (between 10000 and 30000)

**Expected Output:**
- Recommendation: Expand lanes
- Reasoning: Budget (15000 tỷ) is between 1/3 (10000 tỷ) and road cost (30000 tỷ)
- Traffic is over capacity (1300 > 1000)
- Expected capacity increase: ~40% (400 xe/giờ)
- New capacity: ~1400 xe/giờ

### Scenario 4: Direct Bypass for Adjacent Congested Roads
**Setup:**
To test this scenario, we need to modify test data so that multiple adjacent roads are congested:
- E02: B→E (flow: 1500, capacity: 1200) - OVER CAPACITY
- E03: E→F (flow: 2300, capacity: 2000) - OVER CAPACITY
- E04: F→G (flow: 1300, capacity: 1000) - OVER CAPACITY

**Input:**
- Start with Road: E03 (E→F)
- Budget: 150000 tỷ VNĐ
- Total cost of E02+E03+E04 = 35000+80000+30000 = 145000 tỷ VNĐ
- 2/3 of total = 96667 tỷ VNĐ

**Expected Output:**
- Recommendation: Build direct bypass from B to G
- Reasoning: Budget (150000 tỷ) > 2/3 total cost (96667 tỷ)
- All roads B→E→F→G are congested
- Direct road B→G would bypass all three congested roads

### Scenario 5: Insufficient Budget
**Input:**
- Congested Road: E05 (cost = 90000 tỷ VNĐ)
- Current flow: 1700 xe/giờ
- Capacity: 1800 xe/giờ (not over capacity)
- Budget: 5000 tỷ VNĐ

**Expected Output:**
- No recommendations (road not over capacity)
- If road were over capacity with budget < 1/3 cost (30000 tỷ):
  - Display traffic signal solution instead

## Logic Flow

```
1. Check if road is over capacity
   - If NO: return empty proposals
   - If YES: continue

2. Get road cost from Edge.budget

3. Check budget conditions:
   a. If budget >= roadCost:
      - Find bypass routes through intermediate nodes
      - Create NEW_ROAD proposals
   
   b. Else if budget >= roadCost/3 AND budget < roadCost:
      - Create EXPAND_LANES proposal
      - Estimated cost: ~50% of road cost
      - Capacity increase: ~40%
   
   c. Check for adjacent congested roads:
      - Find all adjacent roads that are also congested
      - Calculate total cost of congested chain
      - If budget > 2/3 * totalCost:
        - Create DIRECT_BYPASS proposal

4. Select best proposal (highest traffic reduction)

5. Display appropriate recommendation based on proposal type
```

## Code Changes Summary

### TrafficOptimization.h
- Added `ProposalType` enum (NEW_ROAD, EXPAND_LANES, DIRECT_BYPASS)
- Added fields to `NewRoadProposal` struct:
  - `type`: Type of proposal
  - `congestedPath`: List of congested road IDs for bypass
- Added new helper methods:
  - `isOverCapacity()`: Check if edge flow > capacity
  - `findAdjacentCongestedRoads()`: Find chain of congested roads
  - `createExpandLanesProposal()`: Create lane expansion proposal
  - `createDirectBypassProposal()`: Create direct bypass proposal

### TrafficOptimization.cpp
- Modified `optimizeTraffic()`: Display road cost and budget
- Rewrote `findPotentialNewRoads()`:
  - Implement three-tier logic based on budget
  - Call appropriate helper methods
- Updated `displayProposal()`:
  - Handle different proposal types
  - Show appropriate information for each type
- Implemented new helper methods

## Testing Recommendations

1. **Test with E03 (SkyBridge)**: flow=2300, capacity=2000, budget=80000
   - With budget 100000: Should recommend new road
   - With budget 40000: Should recommend expand lanes
   - With budget 10000: Should show traffic signal solution

2. **Test with E04 (HillPass)**: flow=1300, capacity=1000, budget=30000
   - With budget 15000: Should recommend expand lanes
   - With budget 35000: Should recommend new road

3. **Test adjacent congestion**: Modify flow values to make E02, E03, E04 all congested
   - With large budget (>100000): Should detect chain and recommend direct bypass

## Expected Benefits

1. **More precise recommendations**: Based on actual road cost vs budget
2. **Cost-effective solutions**: Expand lanes when budget is limited
3. **System-wide thinking**: Detect congested chains and recommend direct bypasses
4. **Better resource utilization**: Match solution to available budget
