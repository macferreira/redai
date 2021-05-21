#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

double erand48(unsigned short xseed[3]);

int main(int argc, char *argv[])
{
    LARGE_INTEGER frequency, start, end;
    double elapsed;
    int i, samples, count;
    unsigned short xi[3];
    double x, y;

    if (argc != 5)
    {
        printf("Usage: %s <samples> <seed1> <seed2> <seed3>\n", argv[0]);
        return 1;
    }
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    samples = atoi(argv[1]);
    xi[0] = atoi(argv[2]);
    xi[1] = atoi(argv[3]);
    xi[2] = atoi(argv[4]);
    count = 0;

#pragma omp parallel for private(x, y) reduction(+ \
                                                 : count)
    for (i = 0; i < samples; i++)
    {
        x = erand48(xi);
        y = erand48(xi);
        if (x * x + y * y <= 1.0)
            count++;
    }

    QueryPerformanceCounter(&end);
    elapsed = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("Elapsed time: %f\n", elapsed);
    printf("Estimate of pi: %7.5f\n", 4.0 * count / samples);
    return 0;
}
