#include <stdio.h>
#include <string.h>
#include <limits.h>

#define MAXN 200
#define MAXSEGS (8 * MAXN)
#define MAXLINE 256

typedef struct {
    int id;
    int at, bt;
    int ct, tat, wt;
    int rem;
} Proc;

typedef struct {
    char label[16];
    int start, end;
} Segment;

typedef struct {
    int data[4 * MAXN];
    int head, tail;
} Queue;

static void q_init(Queue *q) { q->head = q->tail = 0; }
static int q_empty(Queue *q) { return q->head == q->tail; }
static void q_push(Queue *q, int v) { q->data[q->tail++] = v; }
static int q_pop(Queue *q) { return q->data[q->head++]; }

static void push_segment(Segment segs[], int *segc, const char *label, int s, int e) {
    if (s >= e) return;
    if (*segc > 0) {
        Segment *prev = &segs[*segc - 1];
        if (strcmp(prev->label, label) == 0 && prev->end == s) {
            prev->end = e;
            return;
        }
    }
    strncpy(segs[*segc].label, label, sizeof(segs[*segc].label) - 1);
    segs[*segc].label[sizeof(segs[*segc].label) - 1] = '\0';
    segs[*segc].start = s;
    segs[*segc].end = e;
    (*segc)++;
}

// Gantt chart and table printing functions (similar to previous files)
static int write_repeat(char *buf, int pos, int max, char ch, int count) {
    for (int k = 0; k < count && pos < max; ++k) buf[pos++] = ch;
    return pos;
}

static int write_centered(char *buf, int pos, int max, const char *txt, int w) {
    int len = (int)strlen(txt);
    if (w < 1) return pos;
    if (len >= w) { for (int i = 0; i < w && pos < max; ++i) buf[pos++] = txt[i]; return pos; }
    int left = (w - len) / 2;
    for (int i = 0; i < left && pos < max; ++i) buf[pos++] = ' ';
    for (int i = 0; i < len && pos < max; ++i) buf[pos++] = txt[i];
    while ((left + len) < w && pos < max) { buf[pos++] = ' '; ++left; }
    return pos;
}

static void advance_spaces(int *cursor, int n) {
    for (int i = 0; i < n; ++i) putchar(' ');
    *cursor += n;
}

static void render_gantt(Segment segs[], int segc, int first_time, int last_time) {
    if (segc <= 0 || last_time <= first_time) { printf("(no timeline)\n"); return; }
    int total = last_time - first_time;
    int scale = 1;
    if (total <= 60) scale = 2;
    if (total <= 30) scale = 3;
    if (total <= 20) scale = 4;
    if (scale > 8) scale = 8;

    char line1[4 * MAXLINE]; int p1 = 0;
    memset(line1, 0, sizeof(line1));
    printf("\n==================== GANTT CHART ====================\n");

    for (int i = 0; i < segc; ++i) {
        int dur = segs[i].end - segs[i].start;
        int w = dur * scale; if (w < 3) w = 3;
        p1 = write_repeat(line1, p1, (int)sizeof(line1) - 1, ' ', 1);
        if (strcmp(segs[i].label, "idle") == 0) {
            p1 = write_repeat(line1, p1, (int)sizeof(line1) - 1, '.', w);
        } else {
            if (w >= 5) {
                p1 = write_repeat(line1, p1, (int)sizeof(line1) - 1, '=', 1);
                p1 = write_centered(line1, p1, (int)sizeof(line1) - 1, segs[i].label, w - 2);
                p1 = write_repeat(line1, p1, (int)sizeof(line1) - 1, '=', 1);
            } else {
                p1 = write_centered(line1, p1, (int)sizeof(line1) - 1, segs[i].label, w);
            }
        }
    }
    p1 = write_repeat(line1, p1, (int)sizeof(line1) - 1, ' ', 1);
    line1[p1] = '\0';
    printf("%s\n", line1);

    int cursor = 0, accum = 0;
    for (int i = 0; i < segc; ++i) {
        int dur = segs[i].end - segs[i].start;
        int w = dur * scale; if (w < 3) w = 3;
        int boundary_col = accum + i;
        if (boundary_col > cursor) advance_spaces(&cursor, boundary_col - cursor);
        char buf[32]; int len = snprintf(buf, sizeof(buf), "%d", segs[i].start);
        fputs(buf, stdout); cursor += len;
        accum += w;
    }
    int final_col = accum + segc;
    if (final_col > cursor) advance_spaces(&cursor, final_col - cursor);
    { char buf[32]; int len = snprintf(buf, sizeof(buf), "%d", last_time);
      fputs(buf, stdout); cursor += len; }
    putchar('\n');
    printf("=====================================================\n");
}

static void print_table(Proc p[], int n) {
    double avgTAT = 0, avgWT = 0;
    printf("\n%-10s %-4s %-4s %-4s %-10s %-11s %-4s %-11s\n",
           "ProcessID", "AT", "BT", "CT", "TAT", "(CT-AT)", "WT", "(TAT-BT)");
    for (int i = 0; i < n; ++i) {
        char pid[16], tatExpr[32], wtExpr[32];
        snprintf(pid, sizeof(pid), "P%d", p[i].id);
        snprintf(tatExpr, sizeof(tatExpr), "%d-%d=%d", p[i].ct, p[i].at, p[i].tat);
        snprintf(wtExpr, sizeof(wtExpr), "%d-%d=%d", p[i].tat, p[i].bt, p[i].wt);
        avgTAT += p[i].tat; avgWT += p[i].wt;
        printf("%-10s %-4d %-4d %-4d %-10d %-11s %-4d %-11s\n",
               pid, p[i].at, p[i].bt, p[i].ct, p[i].tat,
               tatExpr, p[i].wt, wtExpr);
    }
    printf("\nAverage TAT = %.2f\n", avgTAT / n);
    printf("Average WT = %.2f\n", avgWT / n);
}

int main(void) {
    int n, q;
    Proc p[MAXN];
    Segment segs[MAXSEGS]; int segc = 0;

    printf("Enter time quantum q: ");
    if (scanf("%d", &q) != 1 || q <= 0) { printf("Invalid quantum.\n"); return 0; }
    printf("Enter number of processes: ");
    if (scanf("%d", &n) != 1 || n <= 0 || n > MAXN) { printf("Invalid n.\n"); return 0; }

    for (int i = 0; i < n; ++i) {
        p[i].id = i + 1;
        printf("Enter Arrival Time and Burst Time for P%d (AT BT): ", i + 1);
        if (scanf("%d %d", &p[i].at, &p[i].bt) != 2 || p[i].bt < 0) { printf("Bad input.\n"); return 0; }
        p[i].ct = p[i].tat = p[i].wt = 0;
        p[i].rem = p[i].bt;
    }

    long long sumBT = 0;
    for (int i = 0; i < n; ++i) sumBT += p[i].bt;

    int first_arr = INT_MAX, last_ct = 0;
    for (int i = 0; i < n; ++i) if (p[i].at < first_arr) first_arr = p[i].at;

    int t = first_arr;
    int enqed[MAXN] = {0};
    Queue rq; q_init(&rq);

    for (int i = 0; i < n; ++i)
        if (!enqed[i] && p[i].at <= t && p[i].rem > 0) {
            q_push(&rq, i);
            enqed[i] = 1;
        }

    int done = 0;
    while (done < n) {
        if (q_empty(&rq)) {
            int nextAT = INT_MAX;
            for (int i = 0; i < n; ++i)
                if (p[i].rem > 0 && p[i].at > t && p[i].at < nextAT) nextAT = p[i].at;
            push_segment(segs, &segc, "idle", t, nextAT);
            t = nextAT;
            for (int i = 0; i < n; ++i)
                if (!enqed[i] && p[i].at <= t && p[i].rem > 0) { q_push(&rq, i); enqed[i] = 1; }
            continue;
        }
        
        int idx = q_pop(&rq);
        int slice = p[idx].rem < q ? p[idx].rem : q;
        
        char lab[16]; snprintf(lab, sizeof(lab), "P%d", p[idx].id);
        push_segment(segs, &segc, lab, t, t + slice);
        
        t += slice;
        p[idx].rem -= slice;
        
        for (int i = 0; i < n; ++i)
            if (!enqed[i] && p[i].at <= t && p[i].rem > 0) { q_push(&rq, i); enqed[i] = 1; }
        
        if (p[idx].rem > 0) {
            q_push(&rq, idx);
        } else {
            p[idx].ct = t;
            if (t > last_ct) last_ct = t;
            done++;
        }
    }

    for (int i = 0; i < n; ++i) {
        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;
        if (p[i].ct > last_ct) last_ct = p[i].ct;
    }
    
    render_gantt(segs, segc, first_arr, last_ct);
    print_table(p, n);
    
    double total_elapsed = (double)(last_ct - first_arr);
    double efficiency = total_elapsed > 0 ? (double)sumBT / total_elapsed * 100.0 : 100.0;
    
    printf("\nTime quantum (q) = %d\n", q);
    printf("Useful CPU time (sum BT) = %lld\n", sumBT);
    printf("Total elapsed (from ATmin= %d to end= %d) = %.0f\n", first_arr, last_ct, total_elapsed);
    printf("Efficiency (Utilization) = %.2f%%\n", efficiency);
    
    return 0;
}