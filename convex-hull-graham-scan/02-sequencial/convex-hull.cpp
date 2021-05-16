#include <fstream>
#include <iostream>
#include <stack>
#include <stdlib.h>
#include <vector>
using namespace std;

std::string exportFileName = "lines.dat";

struct Point
{
    double x, y;
};

// A global point needed for sorting points with reference
// to the first point Used in compare function of qsort()
Point p0;

// A utility function to find next to top in a stack
Point nextToTop(stack<Point> &S)
{
    Point p = S.top();
    S.pop();
    Point res = S.top();
    S.push(p);
    return res;
}

// Swap two points
void swap(Point &p1, Point &p2)
{
    Point temp = p1;
    p1 = p2;
    p2 = temp;
}

// Return the square of distance between p1 and p2
double distSq(Point p1, Point p2)
{
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

// Find orientation of ordered triplet (p, q, r).
// Returns the following values:
// 0: p, q and r are colinear
// 1: Clockwise
// 2: Counterclockwise
int orientation(Point p, Point q, Point r)
{
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

    if (val == 0)
    {
        // colinear
        return 0;
    }

    // clock or counterclock wise
    return (val > 0) ? 1 : 2;
}

// Used by function qsort() to sort an array of points with respect to the first point
int compare(const void *vp1, const void *vp2)
{
    Point *p1 = (Point *)vp1;
    Point *p2 = (Point *)vp2;

    // find orientation
    int o = orientation(p0, *p1, *p2);
    if (o == 0)
    {
        return (distSq(p0, *p2) >= distSq(p0, *p1)) ? -1 : 1;
    }

    return (o == 2) ? -1 : 1;
}

// Calculates convex hull of a set of n points and exports them to a file
void convexHull(std::vector<Point> points)
{
    int n = points.size();
    // find the bottommost point
    double ymin = points[0].y;
    int min = 0;
    for (int i = 1; i < n; i++)
    {
        double y = points[i].y;

        // pick the bottom-most or chose the left most point in case of tie
        if ((y < ymin) || (ymin == y && points[i].x < points[min].x))
        {
            ymin = points[i].y, min = i;
        }
    }

    // place the bottom-most point at first position
    swap(points[0], points[min]);

    // Sort n-1 points with respect to the first point.
    // A point p1 comes before p2 in sorted output if p2 has larger polar angle (in counterclockwise direction) than p1.
    p0 = points[0];
    qsort(&points[1], n - 1, sizeof(Point), compare);

    // If two or more points make same angle with p0, remove all but the one that is farthest from p0
    // in above sorting, the criteria was to keep the farthest point at the end when more than one points have same angle.
    int m = 1; // initialize size of modified array
    for (int i = 1; i < n; i++)
    {
        // Keep removing i while angle of i and i+1 is same
        // with respect to p0
        while (i < n - 1 && orientation(p0, points[i], points[i + 1]) == 0)
        {
            i++;
        }

        points[m] = points[i];
        m++; // Update size of modified array
    }

    // if modified array of points has less than 3 points, convex hull is not possible
    if (m < 3) {
        return;
    }

    // create an empty stack and push first three points
    stack<Point> S;
    S.push(points[0]);
    S.push(points[1]);
    S.push(points[2]);

    // process remaining n-3 points
    for (int i = 3; i < m; i++)
    {
        // Keep removing top while the angle formed by points next-to-top,
        // top, and points[i] makes a non-left turn.
        while (S.size() > 1 && orientation(nextToTop(S), S.top(), points[i]) != 2)
            S.pop();
        S.push(points[i]);
    }

    // Stack has the output points, put the contents in the export file.
    // Put points counterclockwise.
    std::vector<Point> exportPointsVector;
    while (!S.empty())
    {
        Point p = S.top();
        exportPointsVector.push_back(p);
        S.pop();
    }

    std::ofstream exportFile;
    exportFile.open(exportFileName);
    for (int i = exportPointsVector.size() - 1; i >= 0; i--)
    {
        exportFile << exportPointsVector[i].x << "," << exportPointsVector[i].y << std::endl;
    }
    exportFile.close();
}

// Split the values of the text file (input is string separated by comma).
std::vector<double> splitValues(std::string str)
{
    std::vector<double> valuesPair;
    std::string word = "";
    for (auto x : str)
    {
        if (x == ',')
        {
            valuesPair.push_back(std::stod(word));
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    valuesPair.push_back(std::stod(word));

    return valuesPair;
}

// Add points from the input file to the points vector
void addPoints(std::vector<Point> &points, std::string fileName)
{
    std::string line;
    std::vector<double> splittedValuesVector;
    std::ifstream pointsFile(fileName);

    Point p;

    while (!pointsFile.eof())
    {
        getline(pointsFile, line);

        if (line != "")
        {
            splittedValuesVector = splitValues(line);
            p.x = splittedValuesVector[0];
            p.y = splittedValuesVector[1];
            points.push_back(p);
        }
    }
}

int main(int argc, char *argv[])
{
    std::string fileName;

    // Arguments: #1 is the name of the input file.
    if (argc == 2)
    {
        fileName = argv[1];
    }
    else
    {
        std::cout << "Usage: " << argv[0] << " <input file>" << std::endl;
        return 0;
    }

    std::vector<Point> points;

    addPoints(points, fileName);

    convexHull(points);

    return 0;
}
