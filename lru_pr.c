#include <stdio.h>

int main() {
    int frames[10], pages[30], counter[10];
    int n, m, pageFaults = 0;
    int time = 0, flag1, flag2, i, j, pos, min;

    printf("Enter number of frames: ");
    scanf("%d", &n);
    printf("Enter number of pages: ");
    scanf("%d", &m);

    printf("Enter the reference string:\n");
    for(i = 0; i < m; ++i)
        scanf("%d", &pages[i]);

    for(i = 0; i < n; ++i) {
        frames[i] = -1;
        counter[i] = 0;
    }

    printf("\nPage Replacement Process (LRU Counter Method):\n");
    printf("--------------------------------------------------\n");

    for(i = 0; i < m; ++i) {
        flag1 = flag2 = 0;   //

        for(j = 0; j < n; ++j) {
            if(frames[j] == pages[i]) {
                time++;
                counter[j] = time;   // Update recent use time
                flag1 = flag2 = 1;
                break;
            }
        }

        if(flag1 == 0) { // Page fault - check for free frame
            for(j = 0; j < n; ++j) {
                if(frames[j] == -1) {
                    time++;
                    frames[j] = pages[i];
                    counter[j] = time;
                    pageFaults++;
                    flag2 = 1;
                    break;
                }
            }
        }

        if(flag2 == 0) { // Page fault - no free frame, find LRU victim
            min = counter[0];
            pos = 0;

            for(j = 1; j < n; ++j) {
                if(counter[j] < min) { // Find the page with the smallest (oldest) time
                    min = counter[j];   //
                    pos = j;   //
                }
            }

            time++;
            frames[pos] = pages[i];
            counter[pos] = time;
            pageFaults++;
        }

        printf("For page %2d: ", pages[i]);
        for(j = 0; j < n; ++j) {
            if(frames[j] == -1)
                printf(" -");
            else
                printf("%2d", frames[j]);
        }

        if(flag1 == 1)
            printf(" (Hit)\n");
        else
            printf(" (Page Fault)\n");
    }

    printf("\nTotal Page Faults = %d\n", pageFaults);
    printf("Total Page Hits = %d\n", m - pageFaults);

    return 0;
}
