#include <fstream>
#include <iostream>
#include <mpi.h>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
using namespace std;

int maxXY = 1000;
std::string exportFileName = "lines.dat";

struct Point
{
    double x, y;
};

typedef struct MpiPoint
{
    double x, y;
} mpiPoint;

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

// Return which point is the nearest to the Y axe (lower X) between p1 and p2
// Returns the following values:
// -1: p1 is near
// 0: p1 and p1 are in equal position
// 1: p2 s near
int lowerXValue(Point p1, Point p2)
{
    if (p1.x == p2.x && p1.x == p2.x)
    {
        return 0;
    }

    return (p1.x < p2.x) ? -1 : 1;
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

// Used by function qsort() to sort an array of points with respect to the X position
int compareXValue(const void *vp1, const void *vp2)
{
    Point *p1 = (Point *)vp1;
    Point *p2 = (Point *)vp2;

    return lowerXValue(*p1, *p2);
}

// Calculates convex hull of a set of n points and exports them to a file
std::vector<Point> convexHull(std::vector<Point> points)
{
    int n = points.size();
    std::vector<Point> convexHullVector;

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
    if (m < 3)
    {
        return convexHullVector;
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

    for (int i = exportPointsVector.size() - 1; i >= 0; i--)
    {
        convexHullVector.push_back(exportPointsVector[i]);
    }

    return convexHullVector;
}

// find upper tangent of two polygons 'a' and 'b' represented as two vectors of points
std::vector<Point> mergeHulls(std::vector<Point> a, std::vector<Point> b)
{
    int n1 = a.size(), n2 = b.size();

    int ia = 0, ib = 0;
    for (int i = 1; i < n1; i++)
    {
        if (a[i].x > a[ia].x)
            ia = i;
    }

    // ib is the leftmost point of b
    for (int i = 1; i < n2; i++)
    {
        if (b[i].x < b[ib].x)
            ib = i;
    }

    // find the upper tangent
    int inda = ia, indb = ib;
    bool done = 0;
    while (!done)
    {
        done = 1;
        while (orientation(b[indb], a[inda], a[(inda + 1) % n1]) == 0 || orientation(b[indb], a[inda], a[(inda + 1) % n1]) == 1)
            inda = (inda + 1) % n1;

        while (orientation(a[inda], b[indb], b[(n2 + indb - 1) % n2]) == 0 || orientation(a[inda], b[indb], b[(n2 + indb - 1) % n2]) == 2)
        {
            indb = (n2 + indb - 1) % n2;
            done = 0;
        }
    }

    int uppera = inda, upperb = indb;
    inda = ia, indb = ib;
    done = 0;
    int g = 0;
    while (!done) //find the lower tangent
    {
        done = 1;
        while (orientation(a[inda], b[indb], b[(indb + 1) % n2]) == 0 || orientation(a[inda], b[indb], b[(indb + 1) % n2]) == 1)
            indb = (indb + 1) % n2;

        while (orientation(b[indb], a[inda], a[(n1 + inda - 1) % n1]) == 0 || orientation(b[indb], a[inda], a[(n1 + inda - 1) % n1]) == 2)
        {
            inda = (n1 + inda - 1) % n1;
            done = 0;
        }
    }

    int lowera = inda, lowerb = indb;
    vector<Point> retHull;

    // retHull contains the convex hull after merging the two convex hulls with the points sorted counter-clockwise
    int ind = uppera;
    retHull.push_back(a[uppera]);
    while (ind != lowera)
    {
        ind = (ind + 1) % n1;
        retHull.push_back(a[ind]);
    }

    ind = lowerb;
    retHull.push_back(b[lowerb]);
    while (ind != upperb)
    {
        ind = (ind + 1) % n2;
        retHull.push_back(b[ind]);
    }
    return retHull;
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
    int np, id, dest;
    char message[100];
    MPI_Status st;
    double ts;
    std::vector<Point> points;
    std::string fileName;

    // the particioned vector will allow to each rank to read it's corresponding partition
    std::vector<std::vector<Point>> pointsParticioned;

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

    // adds points to the vector
    addPoints(points, fileName);
    // sort the points based in its x value
    qsort(&points[0], points.size(), sizeof(Point), compareXValue);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    // defining the pivots of the points vector in order to distribute work between ranks
    int interval = points.size() / np;
    int rest = points.size() % np;
    int maxPointsMessage = interval + rest;

    // create an mpi type for struct points
    const int nitems = 2;
    int blocklengths[2] = {1, 1};
    MPI_Datatype types[2] = {MPI_DOUBLE, MPI_DOUBLE};
    MPI_Datatype mpi_points_type;
    MPI_Aint offsets[2];
    offsets[0] = offsetof(mpiPoint, x);
    offsets[1] = offsetof(mpiPoint, y);
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_points_type);
    MPI_Type_commit(&mpi_points_type);

    if (id == 0)
    {
        // process 0 starts measuring time
        ts = -MPI_Wtime();

        int chunkMessageSend[2];
        for (int i = 1; i < np; i++)
        {
            // index zero holds the vector index where the rank should start
            chunkMessageSend[0] = interval * i;
            // index 1 holds the number of points the rank should process
            chunkMessageSend[1] = (i == np - 1) ? interval + rest : interval;

            MPI_Send(&chunkMessageSend, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    if (id != 0)
    {
        int chunkMessageReceived[2];
        MPI_Recv(&chunkMessageReceived, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int startingIndex = chunkMessageReceived[0];
        int chunkSize = chunkMessageReceived[1];

        // calculate it's partial hull and send it to rank 1
        std::vector<Point> tempPoints;

        for (int i = startingIndex; i < startingIndex + chunkSize; i++)
        {
            tempPoints.push_back(points[i]);
        }
        std::vector<Point> partialHull = convexHull(tempPoints);

        Point partialHullSend[partialHull.size()];
        for (int i = 0; i < partialHull.size(); i++)
        {
            partialHullSend[i] = partialHull[i];
        }
        MPI_Send(&partialHullSend, partialHull.size(), mpi_points_type, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
        // all the partial hulls to be stitched
        std::vector<std::vector<Point>> partialHulls;

        // calculate it's own partial hull (rank 1)
        std::vector<Point> tempPoints;
        for (int i = 0; i < interval; i++)
        {
            tempPoints.push_back(points[i]);
        }
        std::vector<Point> partialHull = convexHull(tempPoints);
        partialHulls.push_back(partialHull);

        // receive partial hulls from the others
        Point partialHullMessageReceived[maxPointsMessage];
        MPI_Status partialHullMsgStatus;
        for (int i = 1; i < np; i++)
        {
            int partialHullMsgCount;
            std::vector<Point> tempPartialHull;
            MPI_Recv(&partialHullMessageReceived, maxPointsMessage, mpi_points_type, i, 0, MPI_COMM_WORLD, &partialHullMsgStatus);
            MPI_Get_count(&partialHullMsgStatus, mpi_points_type, &partialHullMsgCount);
            for (int j = 0; j < partialHullMsgCount; j++)
            {
                tempPartialHull.push_back(partialHullMessageReceived[j]);
            }
            partialHulls.push_back(tempPartialHull);
        }

        while (partialHulls.size() > 1)
        {
            std::vector<Point> tempCalcVector = mergeHulls(partialHulls[partialHulls.size() - 2], partialHulls[partialHulls.size() - 1]);
            partialHulls.erase(partialHulls.begin() + (partialHulls.size() - 1));
            partialHulls.erase(partialHulls.begin() + (partialHulls.size() - 1));
            partialHulls.push_back(tempCalcVector);
        }

        // partialHulls[0] is now the final hull
        std::ofstream exportFile;
        exportFile.open(exportFileName);
        for (int i = 0; i < partialHulls[0].size(); i++)
        {
            exportFile << partialHulls[0][i].x << "," << partialHulls[0][i].y << std::endl;
        }
        exportFile.close();

        // process 0 measures time
        ts += MPI_Wtime();
        printf("Time: %f s\n", ts);
    }

    MPI_Finalize();
    return 0;
}
