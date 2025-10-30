#include <stdio.h>

int main() {
    int frames[10], pages[30];
    int n, m, pageFaults = 0; // [cite: 40]
    int i, j, k, pos = 0, flag;

    printf("Enter number of frames: ");
    scanf("%d", &n); // [cite: 40]
    printf("Enter number of pages: ");
    scanf("%d", &m); // [cite: 41]

    printf("Enter the reference string:\n");
    for(i = 0; i < m; ++i)
        scanf("%d", &pages[i]);

    for(i = 0; i < n; ++i)
        frames[i] = -1; // [cite: 42]

    printf("\nPage Replacement Process (FIFO):\n");
    printf("------------------------------------\n");

    for(i = 0; i < m; ++i) {
        flag = 0;

        for(j = 0; j < n; ++j) {
            if(frames[j] == pages[i]) {
                flag = 1; // Page hit
                break;
            }
        }

        if(flag == 0) { // Page fault [cite: 43]
            frames[pos] = pages[i];
            pos = (pos + 1) % n; // Move to the next position in the FIFO queue
            pageFaults++;

            printf("For page %2d: ", pages[i]);
            for(k = 0; k < n; ++k) {
                if(frames[k] == -1)
                    printf(" -");
                else
                    printf("%2d", frames[k]);
            }
            printf(" (Page Fault)\n");
        } else {
            printf("For page %2d: ", pages[i]);
            for(k = 0; k < n; ++k) {
                if(frames[k] == -1)
                    printf(" -");
                else
                    printf("%2d", frames[k]);
            }
            printf(" (Hit)\n");
        }
    }

    printf("\nTotal Page Faults = %d\n", pageFaults); // [cite: 45]
    printf("Total Page Hits = %d\n", m - pageFaults);

    return 0; // [cite: 45]
}
