#include <iostream>
#include <chrono>
#include <math.h>
#include <ostream>
#include <pthread.h>
#include <stdio.h>
#define NUM_RECT 1000000000
#define NUMTHREADS 4
double integralMinimum = 0.0;
double integralMaximum = 1.0;
double pi = 0.0;
pthread_mutex_t gLock;

void *Area(void *pArg)
{
    int myNum = *((int *)pArg);
    double step = integralMaximum / NUM_RECT;
    double partialSum = 0.0, x, y;

    for (int i = myNum; i < NUM_RECT; i += NUMTHREADS)
    {
        x = i * step;
        y = 4 / (1 + x * x);
        partialSum += step * y;
    }
    pthread_mutex_lock(&gLock);
    pi += partialSum;
    pthread_mutex_unlock(&gLock);
    return 0;
}

int main(int argc, char *argv[])
{
    // start measuring the execution time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    pthread_t tHandles[NUMTHREADS];
    int tNum[NUMTHREADS], i;
    pthread_mutex_init(&gLock, NULL);
    for (i = 0; i < NUMTHREADS; ++i)
    {
        tNum[i] = i;
        pthread_create(&tHandles[i],      // Returned thread handle
                       NULL,              // Thread Attributes
                       Area,              // Thread function
                       (void *)&tNum[i]); // Data for Area()
    }
    for (i = 0; i < NUMTHREADS; ++i)
    {
        pthread_join(tHandles[i], NULL);
    }

    std::cout << "Multithreaded calculation using " << NUMTHREADS << " threads\n";
    std::cout << "Real value of Pi: 3.1415926535897932384626433\n";
    printf("Calc value of Pi: %26.25f\n", pi);
    pthread_mutex_destroy(&gLock);

    // end measuring the execution time and show the time taken in seconds and milliseconds
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time (Seconds) = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;
    std::cout << "Time (Milliseconds) = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

    return 0;
}