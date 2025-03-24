#include <stdio.h>

void main() {
    int PID[10], AT[10], BT[10], CT[10], TAT[10], WT[10], n;
    float avgTAT = 0, avgWT = 0;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        PID[i] = i + 1;
        printf("Enter Arrival Time (AT) and Burst Time (BT) for Process %d: ", i + 1);
        scanf("%d %d", &AT[i], &BT[i]);
    }

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (AT[j] > AT[j + 1]) {
                int temp;
                
                temp = AT[j]; AT[j] = AT[j + 1]; AT[j + 1] = temp;
                temp = BT[j]; BT[j] = BT[j + 1]; BT[j + 1] = temp;
                temp = PID[j]; PID[j] = PID[j + 1]; PID[j + 1] = temp;
            }
        }
    }

    CT[0] = AT[0] + BT[0];
    TAT[0] = CT[0] - AT[0];
    WT[0] = TAT[0] - BT[0];

    for (int i = 1; i < n; i++) {
        CT[i] = (CT[i - 1] < AT[i]) ? AT[i] + BT[i] : CT[i - 1] + BT[i];
        TAT[i] = CT[i] - AT[i];
        WT[i] = TAT[i] - BT[i];
    }

    printf("\nComputed Table:\n");
    printf("+-----+-----+-----+-----+-----+-----+\n");
    printf("| PID | AT  | BT  | CT  | TAT | WT  |\n");
    printf("+-----+-----+-----+-----+-----+-----+\n");
    
    for (int i = 0; i < n; i++) {
        printf("| %3d | %3d | %3d | %3d | %3d | %3d |\n", 
                PID[i], AT[i], BT[i], CT[i], TAT[i], WT[i]);
        avgTAT += TAT[i];
        avgWT += WT[i];
    }
    
    printf("+-----+-----+-----+-----+-----+-----+\n");
    printf("\nAverage Turnaround Time: %.2f\n", avgTAT / n);
    printf("Average Waiting Time: %.2f\n", avgWT / n);
}
//end...Alan.
