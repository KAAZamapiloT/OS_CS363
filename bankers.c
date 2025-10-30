#include <stdio.h>
#include <stdbool.h> // For using bool, true, false

int main() {
    printf("Banker's Algorithm Simulation\n");

    // Number of processes and resources
    int m, n;

    printf("Enter number of processes: ");
    scanf("%d", &m);
    printf("Enter number of resources: ");
    scanf("%d", &n);

    // --- Data Structures ---
    // m = processes, n = resources
    int alloc[m][n]; // Allocation Matrix: resources currently allocated
    int max[m][n];   // Max Matrix: max resources a process can request
    int avail[n];    // Available Vector: available resources
    int need[m][n];  // Need Matrix: remaining resources a process needs

    // --- Safety Algorithm Data ---
    int finish[m];   // Finish Vector: tracks if a process has finished
    int safe[m];     // Safe Sequence array

    // --- Input from User ---
    printf("\nEnter allocation matrix:\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%d", &alloc[i][j]);
        }
    }

    printf("\nEnter max matrix:\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%d", &max[i][j]);
        }
    }

    printf("\nEnter available resources vector:\n");
    for (int i = 0; i < n; i++) {
        scanf("%d", &avail[i]);
    }

    // --- Calculate the Need Matrix ---
    // Need[i][j] = Max[i][j] - Allocation[i][j]
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            need[i][j] = max[i][j] - alloc[i][j];
        }
    }

    // --- Safety Algorithm Implementation ---

    // 1. Initialize Finish vector to false (0) for all processes
    for (int i = 0; i < m; i++) {
        finish[i] = 0;
    }

    int count = 0; // Counter for processes in safe sequence

    // 2. Find a process Pi such that Finish[i] is false and Need[i] <= Available
    while (count < m) {
        bool found = false;
        for (int i = 0; i < m; i++) {
            // Check if process i is not finished
            if (finish[i] == 0) {
                int j;
                // Check if its needs can be met by available resources
                for (j = 0; j < n; j++) {
                    if (need[i][j] > avail[j]) {
                        break; // Cannot satisfy need, break to check next process
                    }
                }

                // If inner loop completed, it means Need[i] <= Available
                if (j == n) {
                    // 3. Simulate resource allocation and release
                    for (int k = 0; k < n; k++) {
                        avail[k] += alloc[i][k];
                    }
                    // Add process to safe sequence
                    safe[count++] = i;
                    // Mark process as finished
                    finish[i] = 1;
                    found = true;
                }
            }
        }

        // If no such process was found in a full iteration, the system is in an unsafe state
        if (found == false) {
            printf("\nSystem is not in a safe state.\n");
            return 0; // Exit
        }
    }

    // If the loop completes, the system is in a safe state
    printf("\nSystem is in a safe state.\n");
    printf("Safe sequence is: ");
    for (int i = 0; i < m; i++) {
        printf("P%d", safe[i]);
        if (i < m - 1) {
            printf(" -> ");
        }
    }
    printf("\n");

    return 0;
}