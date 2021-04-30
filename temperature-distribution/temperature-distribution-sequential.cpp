#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#define EPSILON 0.001
#define N 800
#define TOP 100.0

#define maxval TOP
#define minval 0.0

int main(int argc, char *argv[])
{
    int i, j, niter;
    double eps, enew;
    DWORD stime, etime;
    double **aux;
    char fname[40];
    FILE *out;

    double r = 0.24;
    double **t = (double **)malloc(N * sizeof(double *));
    double **told = (double **)malloc(N * sizeof(double *));
    double *telem = (double *)calloc(N * N, sizeof(double));    // initialized to 0.0
    double *toldelem = (double *)calloc(N * N, sizeof(double)); // initialized to 0.0

    printf("Temperature distribution problem on a square metal plate\n");
    printf("Plate size: %d x %d points\n", N, N);
    printf("Top side: 100 'C, other sides. 0 'C\n");

    stime = GetTickCount();

    // initialize matrices for accessing through a 2D pointer
    for (i = 0; i < N; i++)
    {
        t[i] = &telem[i * N];
        told[i] = &toldelem[i * N];
    }
    //initialize top temperature
    for (j = 0; j < N; j++)
        told[0][j] = t[0][j] = TOP;
    // calculate interior temperatures assuming heat conduction only
    niter = 0; // number of iterations
    do
    {
        eps = 0.0;
        for (i = 1; i < (N - 1); i++)
            for (j = 1; j < (N - 1); j++)
            {
                t[i][j] = (told[i][j + 1] + told[i][j - 1] + told[i + 1][j] + told[i - 1][j]) * r + told[i][j] * (1.0 - 4.0 * r);
                enew = fabs(t[i][j] - told[i][j]);
                if (enew > eps)
                    eps = enew;
            }

        // swap matrices
        aux = t;
        t = told;
        told = aux;
        niter++;
    } while (eps > EPSILON); // finish when temperatures are being steady

    // use a well-defined image standard: Netpbm
    sprintf_s(fname, 40, "heat.pgm");
    fopen_s(&out, fname, "w+b");
    // need a header to indicate 8-bit grayscale
    fprintf(out, "P2 %d %d 255\n", N, N);
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            fprintf(out, "%d ", (int)(((told[i][j] - minval) * 255.0) / (maxval - minval)));
    fclose(out);
    etime = GetTickCount();

    printf("Nr. of iterations: %d\n", niter);
    printf("Time = %.3f s\nDone\n", (etime - stime) / 1000.0);
    return 0;
}
