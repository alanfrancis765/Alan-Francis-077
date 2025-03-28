#include <stdio.h>
// it's an optimal code
int main() {
    int ref[25], fr[10], n, i, k, f, fi = 0, faults = 0, miss;

    printf("Enter the length of reference string:\n");
    scanf("%d", &n);

    printf("Enter the reference string:\n");
    for (i = 0; i < n; i++)
        scanf("%d", &ref[i]);

    printf("Enter the number of page frames:\n");
    scanf("%d", &f);

    if (f > 10) {
        printf("Error: Max supported frames is 10.\n");
        return 1;
    }

    for (i = 0; i < f; i++)
        fr[i] = -1;

    for (i = 0; i < n; i++) {
        miss = 1;
        
        for (k = 0; k < f; k++) {
            if (ref[i] == fr[k]) {
                miss = 0;
                break;
            }
        }

        if (miss) {
            fr[fi] = ref[i]; 
            faults++;
            fi = (fi + 1) % f;
        }
    }

    printf("Number of page faults: %d\n", faults);
    return 0;
}
