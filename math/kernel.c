#include <stdio.h>
#include <math.h>

#define N 3  // Number of variables
#define EPSILON 1e-6

// Function to perform Gaussian Elimination
void gaussianElimination(float a[N][N+1]) {
    for (int i = 0; i < N; i++) {
        // Make the diagonal element 1
        float diag = a[i][i];
        if (fabs(diag) < EPSILON) {
            printf("Pivot too small or zero. Try a different matrix.\n");
            return;
        }
        for (int j = 0; j <= N; j++) {
            a[i][j] /= diag;
        }

        // Eliminate below
        for (int k = i + 1; k < N; k++) {
            float factor = a[k][i];
            for (int j = 0; j <= N; j++) {
                a[k][j] -= factor * a[i][j];
            }
        }
    }

    // Back-substitution
    printf("\nKernel (Null Space) solutions (in terms of free variable z):\n");
    float x = a[0][2] * -1;
    float y = a[1][2] * -1;
    printf("x = %.2f * z\n", x);
    printf("y = %.2f * z\n", y);
    printf("z = z (free variable)\n");
}

int main() {
    // Example: 3x3 matrix A for system Ax = 0
    // Matrix [A | 0]
    float matrix[N][N+1] = {
        {1, 0.5, 2, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    };

    printf("Solving A * x = 0 for kernel...\n");
    gaussianElimination(matrix);

    return 0;
}
