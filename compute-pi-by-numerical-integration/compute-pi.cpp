#include <iostream>
#include <chrono>
#include <math.h>
#include <ostream>
#include <pthread.h>
#include <stdio.h>
long numRectangles = 1000000000;
int numThreads = 4;
double integralMinimum = 0.0;
double integralMaximum = 1.0;
double pi = 0.0;
pthread_mutex_t gLock;

// thread function that calculates the area of the rectangles and sums them
void *calcAreasAndSum(void *pArg)
{
    int myNum = *((int *)pArg);
    double step = integralMaximum / numRectangles;
    double partialSum = 0.0, x, y;

    for (int i = myNum; i < numRectangles; i += numThreads)
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

void setArgs(int argNumThreads, long argNumRectangles = 0)
{
    if (argNumThreads > 0)
    {
        numThreads = argNumThreads;
    }
    if (argNumRectangles > 0)
    {
        numRectangles = argNumRectangles;
    }
}

int main(int argc, char *argv[])
{
    // arguments: #1 is number of threads, #2 is number of rectangles.
    if (argc == 2)
    {
        setArgs(std::atoi(argv[1]));
    }
    else if (argc == 3)
    {
        setArgs(std::atoi(argv[1]), std::atol(argv[2]));
    }

    // start measuring the execution time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    pthread_t tHandles[numThreads];
    int tNum[numThreads], i;
    pthread_mutex_init(&gLock, NULL);

    // create the threads
    for (i = 0; i < numThreads; ++i)
    {
        tNum[i] = i;
        pthread_create(&tHandles[i],      // returned thread handle
                       NULL,              // thread attributes
                       calcAreasAndSum,   // thread function
                       (void *)&tNum[i]); // data for calcAreasAndSum()
    }

    // wait for the created threads to terminate
    for (i = 0; i < numThreads; ++i)
    {
        pthread_join(tHandles[i], NULL);
    }

    std::cout << "Multithreaded calculation using " << numThreads << " threads (" << numRectangles << " Rectangles)" << std::endl;
    std::cout << "Real value of Pi: 3.1415926535897932384626433" << std::endl;
    printf("Calc value of Pi: %26.25f\n", pi);
    pthread_mutex_destroy(&gLock);

    // end measuring the execution time and show the time taken in seconds and milliseconds
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time (Seconds) = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;
    std::cout << "Time (Milliseconds) = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

    return 0;
}
