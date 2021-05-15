#include <fstream>
#include <GL/glut.h>
#include <iostream>
#include <ostream>
#include <vector>

struct Point
{
    double x, y;
};
struct Line
{
    double x, y;
};

std::vector<Point> points;
std::vector<Line> lines;

void display(void)
{
    // background color
    glClearColor(1.0, 1.0, 1.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT);

    // x axis
    glColor3ub(254, 0, 0);
    glBegin(GL_LINES);
    glVertex2f(-1.0, 0.0);
    glVertex2f(1.0, 0.0);
    glEnd();

    // y axis
    glColor3ub(254, 0, 0);
    glBegin(GL_LINES);
    glVertex2f(0.0, -1.0);
    glVertex2f(0.0, 1.0);
    glEnd();


    // points
    glPointSize(4);
    glBegin(GL_POINTS);
    glColor3f(0, 0, 0);
    for (int i = 0; i < points.size(); i++)
    {
        glVertex2f(points[i].x/100.0, points[i].y/100.0);
    }
    glEnd();

    // lines
    glBegin(GL_LINES);
    glColor3ub(0, 0, 254);
    for (int i = 0; i < lines.size(); i++)
    {
        glVertex2f(lines[i].x/100.0, lines[i].y/100.0);

        if (i == lines.size()-1) {
            glVertex2f(lines[0].x/100.0, lines[0].y/100.0);
        } else {
            glVertex2f(lines[i+1].x/100.0, lines[i+1].y/100.0);
        }
    }
    glEnd();

    glFlush();
}

int getPointsNumber(std::string fileName)
{
    std::string line;
    std::ifstream pointsFile(fileName);

    if (pointsFile.is_open())
    {
        getline(pointsFile, line);
    }

    pointsFile.close();

    return std::stoi(line);
}

// split the values of the text file (input is string separated by comma)
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

// add lines from the input file to the vector
void addLines(std::string fileName)
{
    std::string line;
    std::vector<double> splittedValuesVector;
    std::ifstream linesFile(fileName);

    Line l;

    while (!linesFile.eof())
    {
        getline(linesFile, line);

        if (line != "")
        {
            splittedValuesVector = splitValues(line);
            l.x = splittedValuesVector[0];
            l.y = splittedValuesVector[1];
            lines.push_back(l);
        }
    }
}

// add points from the input file to the vector
void addPoints(std::string fileName)
{
    std::string line;
    std::vector<double> splittedValuesVector;
    std::ifstream pointsFile(fileName);

    // Read and throw away the first line as it is the number of points
    getline(pointsFile, line);

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
    std::string pointsFileName;
    std::string linesFileName;

    // number of points
    int totalPoints;

    // arguments:
    //     #1 is the file with the points data (e.g. points.dat)
    //     #2 is the file with the lines data that form the convex hull (e.g. lines.dat)
    if (argc == 3)
    {
        pointsFileName = argv[1];
        linesFileName = argv[2];
    }
    else
    {
        std::cout << "Usage: " << argv[0] << " <points file>  <lines file>" << std::endl;
        return 0;
    }

    totalPoints = getPointsNumber(pointsFileName);
    addPoints(pointsFileName);
    addLines(linesFileName);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Convex Hull");
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
