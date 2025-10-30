#include <stdio.h>

// Function to find a page in the stack
// Returns its index if found, otherwise -1
int findPage(int page, int stack[], int size) {
    for (int i = 0; i < size; i++) {
        if (stack[i] == page) {
            return i;
        }
    }
    return -1;
}

int main() {
    int frames[10], pages[30];
    int n, m, pageFaults = 0;
    int i, j, k, flag, stack_size = 0;

    printf("Enter number of frames: ");
    scanf("%d", &n);
    printf("Enter number of pages: ");
    scanf("%d", &m);

    printf("Enter the reference string:\n");
    for (i = 0; i < m; ++i)
        scanf("%d", &pages[i]);

    // Initialize frames (our stack) to -1
    for (i = 0; i < n; ++i)
        frames[i] = -1;

    printf("\nPage Replacement Process (LRU Stack Method):\n");
    printf("-----------------------------------------------\n");

    for (i = 0; i < m; ++i) {
        flag = 0;
        int page = pages[i];

        // Check for a Page Hit
        int foundIndex = findPage(page, frames, stack_size);

        if (foundIndex != -1) {
            // --- PAGE HIT ---
            flag = 1; // Set hit flag

            // Move the hit page to the top (MRU)
            int hitPage = frames[foundIndex];

            // Shift elements down from where the page was found
            for (k = foundIndex; k < stack_size - 1; k++) {
                frames[k] = frames[k+1];
            }
            // Place the hit page at the top of the stack
            frames[stack_size - 1] = hitPage;

        } else {
            // --- PAGE FAULT ---
            pageFaults++;

            if (stack_size < n) {
                // Free frame available, add page to top of stack
                frames[stack_size] = page;
                stack_size++;
            } else {
                // No free frames, replace LRU (frames[0])

                // Shift all pages down (removes frames[0])
                for (k = 0; k < n - 1; k++) {
                    frames[k] = frames[k+1];
                }
                // Add the new page to the top (MRU)
                frames[n-1] = page;
            }
        }

        // Print the current state of the frames
        printf("For page %2d: ", page);
        for(k = 0; k < n; ++k) {
            if(frames[k] == -1)
                printf(" -");
            else
                printf("%2d", frames[k]);
        }
        if(flag == 1)
            printf(" (Hit)\n");
        else
            printf(" (Page Fault)\n");
    }

    printf("\nTotal Page Faults = %d\n", pageFaults);
    printf("Total Page Hits = %d\n", m - pageFaults);

    return 0;
}
