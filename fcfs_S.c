#include <stdio.h>
#include <string.h>

#define MAXN 200
#define MAXSEGS (3 * MAXN)
#define MAXLINE 200

typedef struct {
    int id;
    int at;
    int bt;
    int ct;
    int tat;
    int wt;
    int input_idx;
} Proc;

typedef struct {
    char label[16];
    int start;
    int end;
} Segment;

static void stable_sort_by_arrival(Proc p[], int n) {
    int i, j;
    for (i = 1; i < n; ++i) {
        Proc key = p[i];
        j = i - 1;
        while (j >= 0 && (p[j].at > key.at ||
                         (p[j].at == key.at && p[j].input_idx > key.input_idx))) {
            p[j + 1] = p[j];
            --j;
        }
        p[j + 1] = key;
    }
}

static int write_repeat(char *buf, int pos, int max, char ch, int count) {
    int k;
    for (k = 0; k < count && pos < max; ++k) buf[pos++] = ch;
    return pos;
}

static int write_centered(char *buf, int pos, int max, const char *text, int w) {
    int len = (int)strlen(text);
    int i;
    if (w < 1) return pos;
    if (len > w - 2) {
        for (i = 0; i < w && pos < max; ++i) buf[pos++] = '=';
        return pos;
    }
    int left = (w - len) / 2;
    for (i = 0; i < left && pos < max; ++i) buf[pos++] = ' ';
    for (i = 0; i < len && pos < max; ++i) buf[pos++] = text[i];
    while ((left + len) < w && pos < max) {
        buf[pos++] = ' ';
        ++left;
    }
    return pos;
}

static void advance_spaces(int *cursor, int n) {
    int s;
    for (s = 0; s < n; ++s) putchar(' ');
    *cursor += n;
}

static void render_gantt(Segment segs[], int segc, int first_time, int last_time) {
    if (segc <= 0 || last_time <= first_time) {
        printf(" (no timeline)\n");
        return;
    }
    int total_time = last_time - first_time;
    int scale = 1;
    if (total_time <= 60) scale = 2;
    if (total_time <= 30) scale = 3;
    if (total_time <= 20) scale = 4;
    if (scale > 8) scale = 8;

    char line1[4 * MAXLINE];
    int p1 = 0;
    memset(line1, 0, sizeof(line1));
    printf("\n==================== GANTT CHART ====================\n");

    {
        int i;
        for (i = 0; i < segc; ++i) {
            int dur = segs[i].end - segs[i].start;
            int width = dur * scale;
            if (width < 3) width = 3;
            p1 = write_repeat(line1, p1, (int)sizeof(line1) - 1, ' ', 1);
            if (strcmp(segs[i].label, "cs") == 0) {
                p1 = write_repeat(line1, p1, (int)sizeof(line1) - 1, '-', width);
            } else if (strcmp(segs[i].label, "idle") == 0) {
                p1 = write_repeat(line1, p1, (int)sizeof(line1) - 1, '.', width);
            } else {
                if (width >= 5) {
                    p1 = write_repeat(line1, p1, (int)sizeof(line1) - 1, '=', 1);
                    p1 = write_centered(line1, p1, (int)sizeof(line1) - 1, segs[i].label, width - 2);
                    p1 = write_repeat(line1, p1, (int)sizeof(line1) - 1, '=', 1);
                } else {
                    p1 = write_repeat(line1, p1, (int)sizeof(line1) - 1, '=', width);
                }
            }
        }
    }
    p1 = write_repeat(line1, p1, (int)sizeof(line1) - 1, ' ', 1);
    line1[p1] = '\0';
    printf("%s\n", line1);

    {
        int cursor = 0;
        int accum_cols = 0;
        int i;
        for (i = 0; i < segc; ++i) {
            int dur = segs[i].end - segs[i].start;
            int width = dur * scale;
            if (width < 3) width = 3;
            int boundary_col = accum_cols + i;
            if (boundary_col > cursor) advance_spaces(&cursor, boundary_col - cursor);
            char buf[32];
            int len;
            len = snprintf(buf, sizeof(buf), "%d", segs[i].start);
            fputs(buf, stdout);
            cursor += len;
            accum_cols += width;
        }
        int final_boundary_col = accum_cols + segc;
        if (final_boundary_col > cursor) advance_spaces(&cursor, final_boundary_col - cursor);
        char buf[32];
        int len;
        len = snprintf(buf, sizeof(buf), "%d", last_time);
        fputs(buf, stdout);
        cursor += len;
    }
    putchar('\n');
    printf("=====================================================\n");
}


int main(void) {
    int n, overhead;
    Proc procs[MAXN];
    Segment segs[MAXSEGS];
    int segc = 0;

    printf("Enter number of processes: ");
    if (scanf("%d", &n) != 1 || n <= 0 || n > MAXN) {
        printf("Invalid n.\n");
        return 0;
    }

    printf("Enter context-switch overhead (integer, e.g., 1): ");
    if (scanf("%d", &overhead) != 1 || overhead < 0) {
        printf("Invalid overhead.\n");
        return 0;
    }

    {
        int i;
        for (i = 0; i < n; ++i) {
            procs[i].id = i + 1;
            procs[i].input_idx = i;
            printf("Enter Arrival Time and Burst Time for P%d (AT BT): ", i + 1);
            if (scanf("%d %d", &procs[i].at, &procs[i].bt) != 2) {
                printf("Bad input.\n");
                return 0;
            }
            if (procs[i].bt < 0) {
                printf("Burst time must be non-negative.\n");
                return 0;
            }
        }
    }

    stable_sort_by_arrival(procs, n);

    {
        int i;
        int time = 0;
        long long sumBT = 0, total_overhead = 0;
        int minAT = procs[0].at;

        if (time < minAT) {
            strcpy(segs[segc].label, "idle");
            segs[segc].start = time;
            segs[segc].end = minAT;
            ++segc;
            time = minAT;
        }

        for (i = 0; i < n; ++i) {
            if (time < procs[i].at) {
                strcpy(segs[segc].label, "idle");
                segs[segc].start = time;
                segs[segc].end = procs[i].at;
                ++segc;
                time = procs[i].at;
            }
            Segment s;
            sprintf(s.label, "P%d", procs[i].id);
            s.start = time;
            s.end = time + procs[i].bt;
            sumBT += procs[i].bt;
            procs[i].ct = s.end;
            segs[segc++] = s;
            time = s.end;

            if (i < n - 1) {
                if (time >= procs[i + 1].at && overhead > 0) {
                    Segment cso;
                    strcpy(cso.label, "cs");
                    cso.start = time;
                    cso.end = time + overhead;
                    segs[segc++] = cso;
                    time += overhead;
                    total_overhead += overhead;
                }
            }
        }

        {
            int j;
            long long sumWT = 0, sumTAT = 0;
            for (j = 0; j < n; ++j) {
                procs[j].tat = procs[j].ct - procs[j].at;
                procs[j].wt = procs[j].tat - procs[j].bt;
                sumWT += procs[j].wt;
                sumTAT += procs[j].tat;
            }

            {
                int first_arrival = procs[0].at;
                // Note: The original code has a bug here for finding first_arrival
                // It should iterate through the unsorted array or sort first,
                // but since it sorts by arrival time, procs[0].at is correct.
                render_gantt(segs, segc, first_arrival, time);
            }

            printf("\n%-11s %-4s %-4s %-4s %-11s %-11s\n",
                   "Process ID", "AT", "BT", "CT", "TAT (CT-AT)", "WT (TAT-BT)");

            for (j = 0; j < n; ++j) {
                char pid[16], tat_expr[32], wt_expr[32];
                snprintf(pid, sizeof(pid), "P%d", procs[j].id);
                snprintf(tat_expr, sizeof(tat_expr), "%d-%d=%d", procs[j].ct, procs[j].at, procs[j].tat);
                snprintf(wt_expr, sizeof(wt_expr), "%d-%d=%d", procs[j].tat, procs[j].bt, procs[j].wt);
                printf("%-11s %-4d %-4d %-4d %-11s %-11s\n",
                       pid, procs[j].at, procs[j].bt, procs[j].ct, tat_expr, wt_expr);
            }

            printf("\nAverage TAT = %.2f\n", (double)sumTAT / n);
            printf("Average WT = %.2f\n", (double)sumWT / n);

            {
                int k;
                long long idle_time = 0;
                for (k = 0; k < segc; ++k) {
                    if (strcmp(segs[k].label, "idle") == 0) {
                        int s = segs[k].start < procs[0].at ? procs[0].at : segs[k].start;
                        int e = segs[k].end;
                        if (e > s) idle_time += (e - s);
                    }
                }
                
                double total_elapsed = (double)(time - procs[0].at);
                double efficiency = total_elapsed > 0 ? (double)sumBT / total_elapsed * 100.0 : 100.0;
                
                printf("\nUseful CPU time (sum BT) = %lld\n", sumBT);
                printf("Total overhead time = %lld\n", total_overhead);
                printf("Total idle time (>=ATmin) = %lld\n", idle_time);
                printf("Total elapsed (from ATmin=%d to end=%d) = %.0f\n", procs[0].at, time, total_elapsed);
                printf("Efficiency (Utilization) = %.2f%%\n", efficiency);
            }
        }
    }
    return 0;
}
