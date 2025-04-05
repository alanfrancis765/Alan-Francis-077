#include <stdio.h>

int main() {
    int PID[10], AT[10], BT[10], CT[10], TAT[10], WT[10], n;
    int temp, time = 0, over = 0, count;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        PID[i] = i + 1;
        printf("Enter Arrival Time and Burst Time for Process %d: ", i + 1);
        scanf("%d %d", &AT[i], &BT[i]);
    }

    // Sort processes by arrival time
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (AT[i] > AT[j]) {
                temp = AT[i]; AT[i] = AT[j]; AT[j] = temp;
                temp = BT[i]; BT[i] = BT[j]; BT[j] = temp;
                temp = PID[i]; PID[i] = PID[j]; PID[j] = temp;
            }
        }
    }

    // SJF Scheduling
    while (over < n) {
        count = 0;

        for (int i = over; i < n; i++) {
            if (AT[i] <= time)
                count++;
            else
                break;
        }

        // Sort ready queue by burst time
        if (count > 1) {
            for (int i = over; i < over + count; i++) {
                for (int j = i + 1; j < over + count; j++) {
                    if (BT[i] > BT[j]) {
                        temp = BT[i]; BT[i] = BT[j]; BT[j] = temp;
                        temp = PID[i]; PID[i] = PID[j]; PID[j] = temp;
                        temp = AT[i]; AT[i] = AT[j]; AT[j] = temp;
                    }
                }
            }
        }

        // If CPU is idle, move time forward
        if (AT[over] > time)
            time = AT[over];

        time += BT[over];
        CT[over] = time;
        TAT[over] = CT[over] - AT[over];
        WT[over] = TAT[over] - BT[over];
        over++;
    }

    // Output
    printf("\nPID  AT  BT  CT  TAT  WT\n");
    for (int i = 0; i < n; i++) {
        printf("%3d  %2d  %2d  %2d  %3d  %2d\n", PID[i], AT[i], BT[i], CT[i], TAT[i], WT[i]);
    }

    float avgTAT = 0, avgWT = 0;
    for (int i = 0; i < n; i++) {
        avgTAT += TAT[i];
        avgWT += WT[i];
    }
    avgTAT /= n;
    avgWT /= n;

    printf("\nAverage Turnaround Time: %.2f", avgTAT);
    printf("\nAverage Waiting Time: %.2f\n", avgWT);

    return 0;
}
