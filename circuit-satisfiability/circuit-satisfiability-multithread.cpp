/*
*   Circuit Satisfiability (32bit input), multithreaded version
*
*/
#include <windows.h>
#include <stdio.h>

/* Return 1 if 'i'th bit of 'n' is 1; 0 otherwise */
#define EXTRACT_BIT(n, i) ((n & (1LL << i)) ? 1 : 0)

/* Globals */
long long vals = 4294967296LL; /* = 4G */
int nt = 2;

/* Thread function */
DWORD WINAPI check_circuit(void *arg)
{
    unsigned int id = *(unsigned int *)arg;

    int v[32]; /* Each element is a bit of z */
    int i;
    long long k;

    for (k = id; k < vals; k += nt)
    {
        for (i = 0; i < 32; i++)
            v[i] = EXTRACT_BIT(k, i);
        if (
            ((v[0] && v[1]) && (!v[1] || !v[3]) && (v[2] || v[3]) && (!v[3] || !v[4]) && (v[4] || !v[5]) && (v[5] || !v[6]) && (v[5] || v[6]) && (v[6] || !v[15]) && (v[7] || !v[8]) && (!v[7] || !v[13]) && (v[8] || v[9]) && (v[8] || !v[9]) && (!v[9] || !v[10]) && (v[9] || v[11]) && (v[10] || v[11]) && (v[12] || v[13]) && (v[13] || !v[14]) && (v[14] || v[15])) &&
            ((v[16] && v[17]) && (!v[17] || !v[19]) && (v[18] || v[19]) && (!v[19] || !v[20]) && (v[20] || !v[21]) && (v[21] || !v[22]) && (v[21] || v[22]) && (v[22] || !v[31]) && (v[23] || !v[24]) && (!v[23] || !v[29]) && (v[24] || v[25]) && (v[24] || !v[25]) && (!v[25] || !v[26]) && (v[25] || v[27]) && (v[26] || v[27]) && (v[28] || v[29]) && (v[29] || !v[30]) && (v[30] || v[31])))
        {
            printf("%d) %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n", id,
                   v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9],
                   v[10], v[11], v[12], v[13], v[14], v[15], v[16], v[17], v[18], v[19],
                   v[20], v[21], v[22], v[23], v[24], v[25], v[26], v[27], v[28], v[29],
                   v[30], v[31]);
        }
    }
    return 0;
}

void main(int argc, char *argv[])
{
    int k, *thr_nr;
    HANDLE *thr_hnd;
    long long freq, ts1, ts2;

    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

    if (argc < 2)
    {
        printf("Usage: %s <nr of threads>\n", argv[0]);
    }
    else
        nt = atoi(argv[1]);
    thr_nr = (int *)malloc(nt * sizeof(int));
    thr_hnd = (HANDLE *)malloc(nt * sizeof(HANDLE));
    for (k = 0; k < nt; k++)
        thr_nr[k] = k;

    QueryPerformanceCounter((LARGE_INTEGER *)&ts1);
    for (k = 0; k < nt - 1; k++)
        thr_hnd[k] = CreateThread(NULL, 0, check_circuit, &(thr_nr[k]), 0, NULL);
    check_circuit(&(thr_nr[nt - 1]));
    WaitForMultipleObjects(nt - 1, thr_hnd, TRUE, INFINITE);

    QueryPerformanceCounter((LARGE_INTEGER *)&ts2);
    printf("Done (Time: %.3f us)\n", ((double)(ts2 - ts1)) * 1000000 / freq);
}
