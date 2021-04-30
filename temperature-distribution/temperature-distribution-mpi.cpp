#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"
#define EPSILON 0.001
#define N 800
#define TOP 100.0
#define maxval TOP
#define minval 0.0
int main(int argc, char *argv[])
{
    int i, j, nrows, first, last, start, end, size, niter;
    double eps, enew;
    double stime, etime;
    double **aux;
    double **t, **told, *telem, *toldelem;
    MPI_Status status;
    char fname[40];
    FILE *out;
    double r = 0.24;
    int np, nr;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &nr);
    if (nr == 0)
        stime = MPI_Wtime();
    first = nr * N / np;
    last = (nr + 1) * N / np - 1;
    nrows = last - first + 1;
    if (nr == 0)
    {
        t = (double **)malloc(N * sizeof(double *));
        told = (double **)malloc(N * sizeof(double *));
        telem = (double *)calloc(N * N, sizeof(double));    // initialized to 0.0
        toldelem = (double *)calloc(N * N, sizeof(double)); // initialized to 0.0
        // initialize matrices for accessing through a 2D pointer
        for (i = 0; i < N; i++)
        {
            t[i] = &telem[i * N];
            told[i] = &toldelem[i * N];
        }
        //initialize top temperature
        for (j = 0; j < N; j++)
            told[0][j] = t[0][j] = TOP;
        // distribute
        for (i = 1; i < np; i++)
        {
            start = i * N / np - 1;
            end = (i + 1) * N / np;
            if (i == (np - 1))
                end = N - 1;
            size = (end - start) + 1;
            MPI_Send(told[start], size * N, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        t = (double **)malloc((nrows + 2) * sizeof(double *));
        told = (double **)malloc((nrows + 2) * sizeof(double *));
        telem = (double *)malloc((nrows + 2) * N * sizeof(double));
        toldelem = (double *)malloc((nrows + 2) * N * sizeof(double));

        // initialize matrices for accessing through a 2D pointer
        for (i = 0; i < nrows + 2; i++)
        {
            t[i] = &telem[i * N];
            told[i] = &toldelem[i * N];
        }
        //Receive initial values
        size = nrows + 2;
        if (nr == (np - 1))
            size -= 1;
        MPI_Recv(told[0], size * N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
    }
    // calculate interior temperatures assuming heat conduction only
    niter = 0;
    start = 1;
    end = nrows;
    if (nr == 0 || nr == (np - 1))
        end -= 1;
    do
    {
        eps = 0.0;
        for (i = start; i <= end; i++)
            for (j = 1; j < (N - 1); j++)
            {
                t[i][j] = (told[i][j + 1] + told[i][j - 1] + told[i + 1][j] + told[i - 1][j]) * r +
                          told[i][j] * (1.0 - 4.0 * r);
                enew = fabs(t[i][j] - told[i][j]);
                if (enew > eps)
                    eps = enew;
            }
        //swap matrices
        aux = t;
        t = told;
        told = aux;
        if (nr == 0)
        {
            MPI_Send(told[end], N, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(told[end + 1], N, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);
        }
        if (nr != 0 && nr != (np - 1))
        {
            MPI_Send(told[1], N, MPI_DOUBLE, nr - 1, 0, MPI_COMM_WORLD);
            MPI_Send(told[end], N, MPI_DOUBLE, nr + 1, 0, MPI_COMM_WORLD);
            MPI_Recv(told[0], N, MPI_DOUBLE, nr - 1, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(told[end + 1], N, MPI_DOUBLE, nr + 1, 0, MPI_COMM_WORLD, &status);
        }
        if (nr == (np - 1))
        {
            MPI_Send(told[1], N, MPI_DOUBLE, nr - 1, 0, MPI_COMM_WORLD);
            MPI_Recv(told[0], N, MPI_DOUBLE, nr - 1, 0, MPI_COMM_WORLD, &status);
        }
        // determine the maximum eps
        MPI_Reduce(&eps, &enew, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        if (nr == 0)
            eps = enew;
        MPI_Bcast(&eps, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        niter++;
    } while (eps > EPSILON); // finish when temperatures are being steady
    //transmit results to root
    if (nr != 0)
        MPI_Send(told[1], nrows * N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    else
        for (i = 1; i < np; i++)
        {
            first = i * N / np;
            last = (i + 1) * N / np - 1;
            nrows = last - first + 1;
            MPI_Recv(told[first], nrows * N, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
        }

    if (nr == 0)
    {
        // use a well-defined image standard: Netpbm
        sprintf_s(fname, 40, "heat.pgm");
        fopen_s(&out, fname, "w+b");
        /* need a header to indicate 8-bit grayscale */
        fprintf(out, "P2 %d %d 255\n", N, N);
        for (i = 0; i < N; i++)
            for (j = 0; j < N; j++)
                fprintf(out, "%d ", (int)(((told[i][j] - minval) * 255.0) / (maxval - minval)));
        fclose(out);
        etime = MPI_Wtime();
        printf("Nr. of iterations: %d\n", niter);
        printf("Time = %.3f s\nDone\n", etime - stime);
    }
    MPI_Finalize();
    return 0;
}
