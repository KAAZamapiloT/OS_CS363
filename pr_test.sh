#!/bin/bash

# --- 1. COMPILE THE PROGRAMS ---
# (Assumes fifo.c and lru_counter.c are in the same directory)

echo "Compiling fifo.c..."
gcc  fifo_pr.c -o fifo

echo "Compiling lru_counter.c..."
gcc  lru_pr.c -o lru_counter

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed. Please check the C files for errors."
    exit 1
fi

echo "Compilation successful."
echo "========================================="


# --- 2. TEST CASE 1: FROM THE PDF ---
# This test uses the exact data provided in the lab manual.
# Expected FIFO Faults: 15 [cite: 291]
# Expected LRU Faults:  12 [cite: 770]

echo "--- RUNNING TEST 1: PDF Example ---"
echo ""
echo "Running FIFO (Test 1)..."
./fifo << EOF
3
20
7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1
EOF

echo ""
echo "Running LRU (Test 1)..."
./lru_counter << EOF
3
20
7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1
EOF

echo "========================================="


# --- 3. TEST CASES 2 & 3: BELADY'S ANOMALY CHECK ---
# Reference String: 1 2 3 4 1 2 5 1 2 3 4 5 (12 pages)
# We will run this with 3 frames, then 4 frames.
#
# * FIFO should have MORE faults with 4 frames (9 -> 10 faults).
# * LRU should have FEWER faults with 4 frames (10 -> 8 faults).

echo "--- RUNNING TEST 2: Belady's Anomaly (3 Frames) ---"
echo ""
echo "Running FIFO (Test 2)..."
./fifo << EOF
3
12
1 2 3 4 1 2 5 1 2 3 4 5
EOF

echo ""
echo "Running LRU (Test 2)..."
./lru_counter << EOF
3
12
1 2 3 4 1 2 5 1 2 3 4 5
EOF

echo ""
echo "--- RUNNING TEST 3: Belady's Anomaly (4 Frames) ---"
echo ""
echo "Running FIFO (Test 3)..."
./fifo << EOF
4
12
1 2 3 4 1 2 5 1 2 3 4 5
EOF

echo ""
echo "Running LRU (Test 3)..."
./lru_counter << EOF
4
12
1 2 3 4 1 2 5 1 2 3 4 5
EOF

echo "========================================="


# --- 4. TEST CASE 4: SIMPLE REPEATED ACCESS (SANITY CHECK) ---
# This test should result in 1 page fault and 4 hits.

echo "--- RUNNING TEST 4: Simple Repeated Access ---"
echo ""
echo "Running FIFO (Test 4)..."
./fifo << EOF
3
5
1 1 1 1 1
EOF

echo ""
echo "Running LRU (Test 4)..."
./lru_counter << EOF
3
5
1 1 1 1 1
EOF

echo "========================================="
echo "All tests complete."
