#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdio.h>

int points;
double maxXY;
std::string fileName = "original.dat";

int main(int argc, char *argv[])
{
    // arguments: #1 is n number of points, #2 the max x and y value.
    if (argc == 3)
    {
        points = std::atoi(argv[1]);
        maxXY = std::stod(argv[2]);
    }
    else
    {
        std::cout << "Usage: " << argv[0] << " <number of points> <max x and y value>" << std::endl;
    }

    double x, y;
    double arrX[100];
    double arrY[100];

    double min = 0.0;
    double max = maxXY;

    /**
     * generate random coordinates
     */
    for (int i = 0; i < points; i++)
    {
        x = (max - min) * ( (double)rand() / (double)RAND_MAX ) + min;
        y = (max - min) * ( (double)rand() / (double)RAND_MAX ) + min;

        arrX[i] = x;
        arrY[i] = y;
    }

    /**
     * write to the dat file
     */
    std::ofstream datFile;
    datFile.open(fileName);

    // first line of the file is the total number of points
    datFile << points << std::endl;

    // iterate the aray to write all the coordinates
    for (int i = 0; i < points; i++)
    {
        datFile << arrX[i] << "," << arrY[i] << std::endl;
    }

    datFile.close();

    return 0;
}
