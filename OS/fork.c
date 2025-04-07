#include <stdio.h>
#include <unistd.h>

int main() {
    printf("A");
    printf("\nPID: %d", getpid());
    printf("\nPPID: %d", getppid());
    fflush(stdout);

    int B = fork();
    if (B == 0) {
        printf("\nB");
        printf("\nPID: %d", getpid());
        printf("\nPPID: %d", getppid());
        fflush(stdout);

        int C = fork();
        if (C == 0) {
            printf("\nC");
            printf("\nPID: %d", getpid());
            printf("\nPPID: %d", getppid());
            fflush(stdout);
        }
    } else if (B > 0) {
        int D = fork();
        if (D == 0) {
            printf("\nD");
            printf("\nPID: %d", getpid());
            printf("\nPPID: %d", getppid());
            fflush(stdout);
        }
    }
    return 0;
}
