#include <iostream> 
#include <chrono>
#define NUM_RECT 1000000000
double integralMinimum = 0.0;
double integralMaximum = 1.0;
double pi = 0.0; // global accumulator for the calculated rectangle areas

int main(int argc, char *argv[])
{
    // start measuring the execution time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    double step = integralMaximum / NUM_RECT;
    double x, y;

    for (int i = 0; i < NUM_RECT; i++)
    {
        x = i*step;
        y = 4/(1+x*x);
        pi += step*y;
    }
    
    std::cout << "Single-threaded calculation\n";
    std::cout << "Real value of Pi: 3.1415926535897932384626433\n";
    printf("Calc value of Pi: %26.25f\n", pi);

    // end measuring the execution time and show the time taken in seconds and milliseconds
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time (Seconds) = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;
    std::cout << "Time (Milliseconds) = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

    return 0;
}
