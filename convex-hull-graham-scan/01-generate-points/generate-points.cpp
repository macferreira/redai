#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <vector>

struct Point
{
    double x, y;
};

std::vector<Point> points;

int totalPoints;
int maxXY = 1000;
std::string fileName;

int main(int argc, char *argv[])
{
    // arguments: #1 is n number of points, #2 is the name of the output file.
    if (argc == 3)
    {
        totalPoints = std::atoi(argv[1]);
        fileName = argv[2];
    }
    else
    {
        std::cout << "Usage: " << argv[0] << " <number of points> <output file>" << std::endl;
    }

    double min = 0.0;
    double max = maxXY * 1.0;

    /**
     * generate random coordinates
     */
    Point p;
    for (int i = 0; i < totalPoints; i++)
    {
        p.x = (max - min) * ((double)rand() / (double)RAND_MAX) + min;
        p.y = (max - min) * ((double)rand() / (double)RAND_MAX) + min;

        points.push_back(p);
    }

    /**
     * write to the dat file
     */
    std::ofstream datFile;
    datFile.open(fileName);

    // iterate the vector to write all the coordinates
    for (int i = 0; i < points.size(); i++)
    {
        datFile << points[i].x << "," << points[i].y << std::endl;
    }

    datFile.close();

    return 0;
}
