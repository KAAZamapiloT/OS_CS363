#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PROCESSES 10
#define MAX_BURSTS 20

// A simple queue for the Ready Queue
typedef struct {
    int items[MAX_PROCESSES];
    int front;
    int rear;
} Queue;

// Represents a process with multiple bursts
typedef struct {
    int pid;
    int arrival_time;
    int burst_count;
    int bursts[MAX_BURSTS];     // Alternating CPU, I/O, CPU, ...
    int current_burst;          // Index for the current burst
    int remaining_burst_time;   // Time left for the current CPU or I/O burst
    
    // Stats
    int waiting_time;
    int turnaround_time;
    int completion_time;
    bool is_completed;
} Process;

// --- Queue Helper Functions ---
void init_queue(Queue* q) {
    q->front = -1;
    q->rear = -1;
}

bool is_queue_empty(Queue* q) {
    return q->front == -1;
}

void enqueue(Queue* q, int value) {
    if (q->front == -1) {
        q->front = 0;
    }
    q->rear = (q->rear + 1) % MAX_PROCESSES;
    q->items[q->rear] = value;
}

int dequeue(Queue* q) {
    if (is_queue_empty(q)) {
        return -1;
    }
    int item = q->items[q->front];
    if (q->front == q->rear) {
        q->front = -1;
        q->rear = -1;
    } else {
        q->front = (q->front + 1) % MAX_PROCESSES;
    }
    return item;
}
// --- End Queue Helpers ---

// Function to print the final results
void print_results(Process processes[], int n) {
    float total_wt = 0, total_tat = 0;
    printf("\n--- Final Results ---\n");
    printf("PID\tAT\tCT\tTAT\tWT\n");
    printf("------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        int total_cpu_time = 0;
        for (int j = 0; j < processes[i].burst_count; j += 2) {
            total_cpu_time += processes[i].bursts[j];
        }
        processes[i].waiting_time = processes[i].turnaround_time - total_cpu_time;

        printf("%d\t%d\t%d\t%d\t%d\n",
               processes[i].pid, processes[i].arrival_time,
               processes[i].completion_time, processes[i].turnaround_time,
               processes[i].waiting_time);
        total_tat += processes[i].turnaround_time;
        total_wt += processes[i].waiting_time;
    }
    printf("------------------------------------------\n");
    printf("Average Turnaround Time: %.2f\n", total_tat / n);
    printf("Average Waiting Time: %.2f\n", total_wt / n);
}


int main() {
    int n, time_quantum;
    Process processes[MAX_PROCESSES];
    Process waiting_list[MAX_PROCESSES];
    int waiting_count = 0;

    printf("Enter the number of processes: ");
    scanf("%d", &n);
    printf("Enter the time quantum: ");
    scanf("%d", &time_quantum);

    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        printf("\nEnter details for P%d:\n", i + 1);
        printf("  Arrival Time: ");
        scanf("%d", &processes[i].arrival_time);
        printf("  Number of Bursts (CPU + I/O): ");
        scanf("%d", &processes[i].burst_count);
        printf("  Enter burst times (CPU1 IO1 CPU2 IO2 ...):\n");
        for (int j = 0; j < processes[i].burst_count; j++) {
            scanf("%d", &processes[i].bursts[j]);
        }
        processes[i].current_burst = 0;
        processes[i].remaining_burst_time = processes[i].bursts[0];
        processes[i].is_completed = false;
    }

    Queue ready_queue;
    init_queue(&ready_queue);
    
    int current_time = 0;
    int completed_processes = 0;
    int current_process_idx = -1;
    int quantum_slice = 0;

    printf("\n--- Gantt Chart Simulation ---\n");

    while (completed_processes < n) {
        // 1. Add newly arrived processes to the ready queue
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time == current_time) {
                enqueue(&ready_queue, i);
                printf("Time %d: P%d arrived and added to ready queue.\n", current_time, processes[i].pid);
            }
        }

        // 2. Check for I/O completions and move processes back to ready queue
        for (int i = 0; i < waiting_count; i++) {
            waiting_list[i].remaining_burst_time--;
            if (waiting_list[i].remaining_burst_time == 0) {
                // I/O is done, move back to ready queue for its next CPU burst
                int p_idx = waiting_list[i].pid - 1;
                processes[p_idx].current_burst++;
                processes[p_idx].remaining_burst_time = processes[p_idx].bursts[processes[p_idx].current_burst];
                enqueue(&ready_queue, p_idx);
                printf("Time %d: P%d finished I/O and added to ready queue.\n", current_time, processes[p_idx].pid);
                
                // Remove from waiting list by shifting elements
                for (int j = i; j < waiting_count - 1; j++) {
                    waiting_list[j] = waiting_list[j + 1];
                }
                waiting_count--;
                i--; // Adjust index after removal
            }
        }
        
        // 3. Handle the currently running process
        if (current_process_idx != -1) {
            processes[current_process_idx].remaining_burst_time--;
            quantum_slice--;

            // Check if current CPU burst is finished
            if (processes[current_process_idx].remaining_burst_time == 0) {
                printf("Time %d: P%d finished CPU burst %d.\n", current_time + 1, processes[current_process_idx].pid, (processes[current_process_idx].current_burst / 2) + 1);
                processes[current_process_idx].current_burst++;

                // If there are more bursts, move to I/O waiting list
                if (processes[current_process_idx].current_burst < processes[current_process_idx].burst_count) {
                    processes[current_process_idx].remaining_burst_time = processes[current_process_idx].bursts[processes[current_process_idx].current_burst];
                    waiting_list[waiting_count++] = processes[current_process_idx];
                    printf("         P%d moved to waiting for I/O (%dms).\n", processes[current_process_idx].pid, processes[current_process_idx].remaining_burst_time);
                } else { // Otherwise, the process is completed
                    processes[current_process_idx].is_completed = true;
                    processes[current_process_idx].completion_time = current_time + 1;
                    completed_processes++;
                    printf("         P%d has completed.\n", processes[current_process_idx].pid);
                }
                current_process_idx = -1; // CPU is now free
            } 
            // Check if time quantum expired
            else if (quantum_slice == 0) {
                printf("Time %d: P%d's time quantum expired. Moved to ready queue.\n", current_time + 1, processes[current_process_idx].pid);
                enqueue(&ready_queue, current_process_idx);
                current_process_idx = -1; // CPU is now free
            }
        }

        // 4. If CPU is idle, pick a new process from ready queue
        if (current_process_idx == -1 && !is_queue_empty(&ready_queue)) {
            current_process_idx = dequeue(&ready_queue);
            quantum_slice = time_quantum;
            printf("Time %d: P%d is dispatched to CPU.\n", current_time, processes[current_process_idx].pid);
        }

        // If CPU is idle and ready queue is empty, print idle time
        if (current_process_idx == -1 && is_queue_empty(&ready_queue) && completed_processes < n) {
             if (waiting_count > 0) {
                 // only print if there's a reason to wait
                 printf("Time %d: CPU is idle.\n", current_time);
             }
        }

        current_time++;
    }

    print_results(processes, n);

    return 0;
}