#!/bin/bash

# A script to compile and run the Banker's Algorithm C program.
# It first prints the automated input, then runs the program with that input.

C_SOURCE_FILE="bankers.c"
EXECUTABLE_NAME="bankers"

# --- Step 1: Define the input data in a multiline variable ---
# Using $(cat <<'EOF') is a reliable way to create a multiline string.
INPUT_DATA=$(cat <<'EOF'
5
3
0 1 0
2 0 0
3 0 2
2 1 1
0 0 2
7 5 3
3 2 2
9 0 2
2 2 2
4 3 3
3 3 2
EOF
)

# --- Step 2: Compile the C program ---
echo "Compiling ${C_SOURCE_FILE}..."
gcc ${C_SOURCE_FILE} -o ${EXECUTABLE_NAME}

# Check if compilation was successful. A non-zero exit code means failure.
if [ $? -ne 0 ]; then
    echo "Compilation failed. Please check for errors in the C code."
    exit 1
fi

echo "Compilation successful."
echo

# --- Step 3: Print the input that will be used ---
echo "----------------------------------------"
echo "INPUT being fed to the program:"
echo "----------------------------------------"
echo "${INPUT_DATA}"
echo "----------------------------------------"
echo

# --- Step 4: Run the program and pipe the input into it ---
echo "----------------------------------------"
echo "PROGRAM OUTPUT:"
echo "----------------------------------------"
# The 'echo' command sends the content of the variable to standard output,
# and the pipe '|' redirects that output to the standard input of your program.
echo "${INPUT_DATA}" | ./${EXECUTABLE_NAME}
echo "----------------------------------------"
echo "Program finished."