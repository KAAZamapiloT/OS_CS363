#include <stdio.h>

int main() {
    int frames[10], pages[30];
    int n, m, pageFaults = 0;
    int i, j, k, pos, flag1, flag2, max, future[10];

    printf("Enter number of frames: ");
    scanf("%d", &n);
    printf("Enter number of pages: ");
    scanf("%d", &m);

    printf("Enter the reference string:\n");
    for (i = 0; i < m; ++i)
        scanf("%d", &pages[i]);

    for (i = 0; i < n; ++i)
        frames[i] = -1;

    printf("\nPage Replacement Process (Optimal):\n");
    printf("---------------------------------------\n");

    for (i = 0; i < m; ++i) {
        flag1 = flag2 = 0;

        // Check for Page Hit
        for (j = 0; j < n; ++j) {
            if (frames[j] == pages[i]) {
                flag1 = flag2 = 1;
                break;
            }
        }

        // Page Fault, but free frame is available
        if (flag1 == 0) {
            for (j = 0; j < n; ++j) {
                if (frames[j] == -1) {
                    frames[j] = pages[i];
                    pageFaults++;
                    flag2 = 1;
                    break;
                }
            }
        }

        // Page Fault, no free frames, find victim
        if (flag2 == 0) {
            // Find future use of all pages currently in frames
            for (j = 0; j < n; ++j) {
                future[j] = -1; // Default: will not be used again
                for (k = i + 1; k < m; ++k) {
                    if (frames[j] == pages[k]) {
                        future[j] = k; // Store the index of next use
                        break;
                    }
                }
            }

            max = -1;
            pos = -1;

            // Find the page with the farthest future use
            for (j = 0; j < n; ++j) {
                if (future[j] == -1) {
                    // This page is not used again, replace it
                    pos = j;
                    break;
                } else if (future[j] > max) {
                    // Find the page used furthest in the future
                    max = future[j];
                    pos = j;
                }
            }

            frames[pos] = pages[i]; // Replace the victim
            pageFaults++;
        }

        printf("For page %2d: ", pages[i]);
        for (j = 0; j < n; ++j) {
            if (frames[j] == -1)
                printf(" -");
            else
                printf("%2d", frames[j]);
        }

        if (flag1 == 0)
            printf(" (Page Fault)\n");
        else
            printf(" (Hit)\n");
    }

    printf("\nTotal Page Faults = %d\n", pageFaults);
    printf("Total Page Hits = %d\n", m - pageFaults);

    return 0;
}
