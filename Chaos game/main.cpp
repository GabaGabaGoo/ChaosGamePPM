/*
Author: Gabriel Goble
Date: 4/20/24
Program Name : Chaos Game

Changelog :
4/20/24: removed specific setup point functions and relpaced them with generateStartingPointsNgon()
4/21/24: added allowSameVertexInARow which when disables stops the same vertex from being chosen twice in a row
4/22/24: added noNeighborIfRepeat which if the same vertex is chosen twice, stops that and the two verticies on either
side from being chosen again
4/24/24: made ppm file names store the images generation information 
4/24/24: added centerpoint as an option of the vertex


---------- SPECIAL NOTES ---------
PPM File names::
the name of the ppm files stores information in this format
polygonDegree_percentToNextPoint%_gridWidth_X_gridHeight_sameVertexInARow_noNeighboringIfRepeat_enableCenterVertex.ppm
sameVertexInARow and noNeighboringIfRepeat will have a lowercase x in front if they are disabled

noNeighboringIfRepeat: this option makes sure the chosen vertex does not neighbor the previously chosen one if the previous two
verticies were the same

noNeighboringIfRepeat should only be enabled if allowSameVertexInARow if also enabled

enableConsoleDrawing should only be set to true if gridWidth and gridHeight are less than about 80
*/


#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>


const float M_PI = 3.141592;

using namespace std;

//debugging options
const bool enableShowflowDebug = false;
const bool enableShowflowPausing = true;
const int minShowflowLevel = 1;

//grid options
const int gridWidth = 500;
const int gridHeight = 500;
short grid[gridWidth][gridHeight];

//general iteration options
int polygonDegree = 4;
float percentToNextPoint = 50;
const int numIterations = 10'000'000;

//special iteration options
bool allowSameVertexInARow = false;
bool noNeighboringIfRepeat = false;
bool enableCenterVertex = false;

//runtime output and input options
const bool enableConsoleDrawing = false;
const bool enableUserPerameters = true;
const bool showPercentDone = true;

//ppm variables
const int maxPPMSaturation = 255;
string ppmFileName = "ChaosGame.ppm";
ofstream ppmFile;

struct point {
    int x;
    int y;
};

vector<point> startingPoints;

void showflow(string func, int level) {
    if (enableShowflowDebug && level >= minShowflowLevel) {
        cout << "Entering --- " << func << endl;
        if (enableShowflowPausing) system("pause");
    }
}

//userInput / printing functions
bool getYesOrNo() {
    showflow("getYesOrNo", 1);
    bool validAnswer = false;
    char answer;
    while (!validAnswer) {
        cin >> answer;
        if (answer == 'y' || answer == 'n') {
            validAnswer = true;
            break;
        }
        cout << "Invalid answer, Do you want to keep this image? (y/n)" << endl;
    }
    if (answer == 'y') return true;
    else return false;
}
void percentDone() { 
    showflow("percentDone", 1);
    static int percent = 1;
    cout << percent++ << "% done \r";
}
void getUserPerameters() {
    showflow("getUserPerameters", 2);
    cout << "Welcome to the chaos game, Please enter your settings" << endl;
    cout << "What degree polygon do you want?  EX.(3): " << endl;
    cin >> polygonDegree;
    cout << "What percent to the next point would you like? EX(50) :" << endl;
    cin >> percentToNextPoint;
    cout << "Do you want same vertex repeating? (y/n)" << endl;
    allowSameVertexInARow = getYesOrNo();
    if (allowSameVertexInARow) {
        cout << "Do you want to disable neighbor verticies on repeat? (y/n)" << endl;
        noNeighboringIfRepeat = getYesOrNo();
    }
    if (!noNeighboringIfRepeat) {
        cout << "Do you want the center vertex? (y/n)" << endl;
        enableCenterVertex = getYesOrNo();
    }
    system("cls");
}

//ppmSetupFunctions
void setPPMFileName() {//sets the ppm file name depending on the perameters set for the game
    showflow("setPPMFileName", 1);
    string tempName = "";
    tempName = tempName + to_string(polygonDegree) + '_' + to_string(static_cast<int>(percentToNextPoint)) + '%';
    tempName = tempName + to_string(gridWidth) + 'X' + to_string(gridHeight) + '_';
    tempName += (allowSameVertexInARow ? "SV_" : "xSV_");
    tempName += (noNeighboringIfRepeat ? "NR_" : "xNR_");
    tempName += (enableCenterVertex ? "C" : "xC");
    tempName += ".ppm";
    ppmFileName = tempName;
}
void openPPMFile() {
    showflow("openPPMFile", 1);
    ppmFile.open(ppmFileName);
    if (!ppmFile) {
        cout << "Failed to open ppm file... exiting...";
        exit(1);
    }
}
void formatPPMFile() { //sets the header stuff for the ppm file
    showflow("formatPPmFile", 1);
    ppmFile << "P3" << endl;
    ppmFile << gridWidth << " " << gridHeight << " " << maxPPMSaturation << endl;
}
void setupPPMFile() {
    showflow("setupPPMFile", 1);
    setPPMFileName();
    openPPMFile();
    formatPPMFile();
}

//grid and iteration functions
void addPointToGrid(point p) {
    showflow("addPointToGrid", 2);

    //guards against out of bounds error caused by percentToNextPoint > 100
    if (p.x >= gridWidth || p.x < 0 ||
        p.y >= gridHeight || p.y < 0) {
        return;
    }

    grid[p.x][p.y] += 1;
}
point getPercentDistancePoint(point p1, point p2 , float percent) {
    showflow("getPercentDistancePoint", 2);
    point newPoint;

    float tempX = p1.x + ( p2.x - p1.x ) * (static_cast<float>(percent) / 100);
    float tempY = p1.y + (p2.y - p1.y) * (static_cast<float>(percent) / 100);

    newPoint.x = round(tempX);//rounds becuase the grid is stored as integers
    newPoint.y = round(tempY);
    return newPoint;
}
void generateStartingPointsNgon(int n) {
    showflow("generateStartingPointsNgon", 3);

    float angleOfset = 2 * M_PI / n;
    int middlePointX = gridWidth / 2;
    int middlePointY = gridHeight / 2;
    int radius = gridWidth / 2;

    // generates the polygon verticies using trigonometric identities
    for (int i = 0; i < n; i++) {
        point p = { middlePointX + radius * cos(-M_PI / 2 + i * angleOfset ), middlePointY + radius * sin(-M_PI / 2 + i * angleOfset) };
        startingPoints.push_back(p);
        addPointToGrid(p);
    }

    //adds the center if the option is enabled
    if (enableCenterVertex) {
        point center = { middlePointX, middlePointY };
        startingPoints.push_back(center);
        addPointToGrid(center);
    }
}
point getVertexNoNeighbor() { // gets next vertex if noNeighboringIfRepeat is enabled
    showflow("getNextVertex", 2);
    
    static point secondLast = { -1, 0 };
    static point last = { 0 , 0 };
    static int index = 0;
    //checks if the previous two verticies were the same
    if (last.x == secondLast.x && last.y == secondLast.y) {
        //generates the valid indicies for the next vertex
        int tempIndex = (rand() % (polygonDegree - 3) + index + 2) % (polygonDegree);
        index = tempIndex;
        point p = startingPoints[index];
        secondLast = last;
        last = p;
        return p;
    }
    else {
        //if the last two verticies werent the same, just selects a random vertex
        index = (rand() % (polygonDegree));
        point p = startingPoints[index];
        secondLast = last;
        last = p;
        return p;
    }
}
point getVertexNoRepeat() { //if sameVertexInARow is false, generates a vertex different from the previous one
    static point previousVertex = { 0 , 0 };
    point vertex = startingPoints[rand() % startingPoints.size()];
    while (vertex.x == previousVertex.x && vertex.y == previousVertex.y) {//loops until the previous and current vertex are different
        vertex = startingPoints[rand() % startingPoints.size()];
    }
    previousVertex = vertex;
    return vertex;
}
void iterateChaos() {
    showflow("iterateChaos", 2);
    point previousPoint = { rand() % gridWidth, rand() % gridHeight }; //initializes to a random point in our grid
    addPointToGrid(previousPoint);
   
    for (int i = 0; i < numIterations; i++) {
        if (showPercentDone && i % (numIterations/100) == 0) {
            percentDone();
        }
        point vertex;
        
        //gets vertex depending on the enabled settings
        if (noNeighboringIfRepeat) vertex = getVertexNoNeighbor();
        else if (!allowSameVertexInARow) vertex = getVertexNoRepeat();
        else vertex = startingPoints[rand() % startingPoints.size()];
        
        //gets next point
        point newPoint = getPercentDistancePoint(previousPoint, vertex, percentToNextPoint);
        addPointToGrid(newPoint);
        previousPoint = newPoint;
    }
}

//displaying and cleanup functions
void displayGridInConsole() {//only is displayed if enableConsoleDrawing is true, prints the fractal to the console
    showflow("displayGrid", 3);
    for (int col = 0; col < gridHeight; col++) {
        for (int row = 0; row < gridWidth; row++) {
            if (grid[row][col] != 0)
                cout << "A ";
            else
                cout << "  ";
        }
        cout << endl;
    }
}
void convertGridToPPM() {//converts and saves the grid into the ppm file
    showflow("convertGridToPPM", 2);
    for (int col = 0; col < gridHeight; col++) {
        for (int row = 0; row < gridWidth; row++) {
            int num = grid[row][col];
            ppmFile << "   " << num << " 0 " << num / 12 << "   ";
        }
        ppmFile << endl;
    }
}
void displayGrid() {
    if (enableConsoleDrawing) displayGridInConsole();
    convertGridToPPM();
    ppmFile.close();
    system(ppmFileName.c_str());
}
void wantToKeepThis() {//asks the user if they want the ppm file to be saved, if they answer no the file is deleted
    showflow("wantToKeepThis", 3);
    cout << "Do you want to keep this image? (y/n)" << endl;
    if (!getYesOrNo())remove(ppmFileName.c_str());
}

int main() {
    if(enableUserPerameters) getUserPerameters();
    setupPPMFile();
    generateStartingPointsNgon(polygonDegree);
    iterateChaos();
    displayGrid();
    wantToKeepThis();
    
    return EXIT_SUCCESS;
}



/* ---------- OLD CODE ---------------
void generateStartingPointsTriangle() {
    point p1 = { gridWidth /2, 0 };
    point p2 = { 0, gridHeight - 1 };
    point p3 = { gridWidth - 1, gridHeight - 1 };
    startingPoints.push_back(p1);
    startingPoints.push_back(p2);
    startingPoints.push_back(p3);
    addPointToGrid(p1);
    addPointToGrid(p2);
    addPointToGrid(p3);
}
void generateStartingPointsSquare() {
    point p1 = { gridWidth - 1, 0};
    point p2 = { 0, gridHeight - 1 };
    point p3 = { gridWidth - 1, gridHeight - 1 };
    point p4 = { 0, 0 };
    startingPoints.push_back(p1);
    startingPoints.push_back(p2);
    startingPoints.push_back(p3);
    startingPoints.push_back(p4);
    addPointToGrid(p1);
    addPointToGrid(p2);
    addPointToGrid(p3);
    addPointToGrid(p4);

}
void generateStartingPointsPentagon() {
    int middlePointX = gridWidth / 2;
    int middlePointY = gridHeight / 2;
    int radius = gridWidth / 2;

    point p1 = { middlePointX + radius * cos(-M_PI/2), middlePointY + radius * sin(-M_PI/2)};
    point p2 = { middlePointX + radius * cos(-M_PI / 2 + 2*M_PI/5), middlePointY + radius * sin(-M_PI / 2 + 2 * M_PI / 5) };
    point p3 = { middlePointX + radius * cos(-M_PI / 2 + 4 * M_PI / 5), middlePointY + radius * sin(-M_PI / 2 + 4 * M_PI / 5) };
    point p4 = { middlePointX + radius * cos(-M_PI / 2 + 6 * M_PI / 5), middlePointY + radius * sin(-M_PI / 2 + 6 * M_PI / 5) };
    point p5 = { middlePointX + radius * cos(-M_PI / 2 + 8 * M_PI / 5), middlePointY + radius * sin(-M_PI / 2 + 8 * M_PI / 5) };
    startingPoints.push_back(p1);
    startingPoints.push_back(p2);
    startingPoints.push_back(p3);
    startingPoints.push_back(p4);
    startingPoints.push_back(p5);
    addPointToGrid(p1);
    addPointToGrid(p2);
    addPointToGrid(p3);
    addPointToGrid(p4);
    addPointToGrid(p5);
}





*/