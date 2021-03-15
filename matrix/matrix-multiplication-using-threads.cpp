// MatMul.c : Multiply two square matrices using multi-threads.
//

#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>

struct targ
{
    int begin, end, dim;
};

double *mat1, *mat2, *mres;

/* Thread function: multiply rows from begin to end (1st matrix) by all columns of the 2nd matrix */
DWORD WINAPI multiply(struct targ *ta)
{
    int i, j, k;
    double *res, acc;

    res = mres + ta->begin * ta->dim;
    for (i = ta->begin; i <= ta->end; i++)
        for (j = 0; j < ta->dim; j++)
        {
            acc = 0.0;
            for (k = 0; k < ta->dim; k++)
                acc += mat1[i * ta->dim + k] * mat2[k * ta->dim + j];
            *res++ = acc;
        }
    return 0;
}

int main(int argc, char *argv[])
{
    FILE *fp1, *fp2, *fout;
    int k, nts, dim, size;
    HANDLE hnd[7];
    struct targ targar[8];
    unsigned int start, end;

    /* Reading matrices */
    start = GetTickCount(); // Start time measurement
    if (argc < 5)
    {
        printf("Usage: %s <matfile1> <matfile2> <outmat> <nthreads>\n", argv[0]);
        return 1;
    }
    if ((fp1 = fopen(argv[1], "rb")) == NULL)
    {
        printf("File %s not found!\n", argv[1]);
        return 1;
    }
    if ((fp2 = fopen(argv[2], "rb")) == NULL)
    {
        printf("File %s not found!\n", argv[2]);
        return 1;
    }
    if ((fout = fopen(argv[3], "wb")) == NULL)
    {
        printf("Cannot open file %s!\n", argv[3]);
        return 1;
    }
    nts = atoi(argv[4]);
    if (nts < 1 || nts > 8)
    {
        printf("invalid nr. of threads (1 to 8)\n");
        return 1;
    }
    fread(&dim, sizeof(int), 1, fp1);
    fread(&size, sizeof(int), 1, fp2);
    if (dim != size)
    {
        printf("Different sizes for input matrices: %d %d\n", dim, size);
        return 1;
    }
    size = dim * dim * sizeof(double);
    mat1 = (double *)malloc(size);
    mat2 = (double *)malloc(size);
    mres = (double *)malloc(size);
    if (mat1 == NULL || mat2 == NULL || mres == NULL)
    {
        printf("Insufficient memory\n");
        return 1;
    }
    fread(mat1, size, 1, fp1);
    fread(mat2, size, 1, fp2);
    end = GetTickCount(); // End time measurement (reading matrices)

    printf("Reading matrices: %.3f s\n", (end - start) / 1000.0);

    /* Create the threads and wait for them to conclude */
    start = GetTickCount();
    targar[0].begin = 0;
    targar[0].end = dim / nts - 1;
    targar[0].dim = dim;
    for (k = 1; k < nts; k++)
    {
        targar[k].begin = k * dim / nts;
        targar[k].end = (k + 1) * dim / nts - 1;
        targar[k].dim = dim;
        hnd[k - 1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)multiply, &targar[k], 0, NULL);
    }
    multiply(&targar[0]);
    WaitForMultipleObjects(nts - 1, hnd, TRUE, INFINITE);
    end = GetTickCount();

    printf("Processing time: %.3f s\n", (end - start) / 1000.0);

    /* Write the result */
    start = GetTickCount();
    fclose(fp1);
    fclose(fp2);
    fwrite(&dim, sizeof(int), 1, fout);
    fwrite(mres, size, 1, fout);
    fclose(fout);
    end = GetTickCount();

    printf("Writing result: %.3f s\n", (end - start) / 1000.0);

    return 0;
}
