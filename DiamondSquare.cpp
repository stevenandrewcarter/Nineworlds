#include <cmath>			//Mathematical Functions
#include <ctime>			//Time Functions
#include "DiamondSquare.h"  //This HeaderFile
#include <iomanip>
#include <cstdlib>
//#include "LinMaths.h"

int DiamondSquare::getTerrainSize() const {
    return Terrain_Size;
}

void DiamondSquare::DiamondStep(int TopLeftX, int TopLeftY, int BottomRightX, int BottomRightY, int SquareLength) {
    int midpointX = (TopLeftX + BottomRightX) / 2;
    int midpointY = (TopLeftY + BottomRightY) / 2;
    float offset =
            (Height_Map[TopLeftX][TopLeftY] + Height_Map[BottomRightX][TopLeftY] + Height_Map[TopLeftX][BottomRightY] +
             Height_Map[BottomRightX][BottomRightY]) / 4;
    offset = offset + (float) (-H + (H - (-H)) * rand() * (1.0 / RAND_MAX));
    Height_Map[midpointX][midpointY] = offset;
}

void DiamondSquare::SquareStep(int X1, int Y1, int X2, int Y2, int X3, int Y3) {
    //X3, Y3 will be midpoint
    float offset = (Height_Map[X1][Y1] + Height_Map[X2][Y2] + Height_Map[X3][Y3]) / 3;
    offset = offset + (float) (-H + (H - (-H)) * rand() * (1.0 / RAND_MAX));
    if (Y1 == Y2) {
        int midpointX = (X1 + X2) / 2;
        Height_Map[midpointX][Y1] = offset;
    } else {
        int midpointY = (Y1 + Y2) / 2;
        Height_Map[X1][midpointY] = offset;
    }
}

void DiamondSquare::calculate(int iterations, double hrange, double hdecay) {
    Iterations = iterations;
    H = hrange;
    HDecay = hdecay;

    int Iter = Iterations - 1;
    Terrain_Size = (int) ((2 * pow(2, Iter)) + 1);

    //Set up 2d array
    Height_Map = new float *[Terrain_Size];

    // for each row, setup the columns
    for (int i = 0; i < Terrain_Size; i++) {
        Height_Map[i] = new float[Terrain_Size]; // create a single row of ints
    }

    int seed = static_cast<int>(time(0));
    srand(seed);
    //Initial seed of the corners
    Height_Map[0][0] = 25.0f;
    Height_Map[Terrain_Size - 1][0] = 25.0f;
    Height_Map[0][Terrain_Size - 1] = 25.0f;
    Height_Map[Terrain_Size - 1][Terrain_Size - 1] = 25.0f;
    for (int Iter = 0; Iter < Iterations; Iter++) {
        double Blocks = pow(4.0, (double) Iter);
        int BlockLength = (int) ((Terrain_Size - 1) / Blocks);
        if (Blocks > 1) {
            Blocks = sqrt(Blocks);
            BlockLength = (int) ((Terrain_Size - 1) / Blocks);
        }
        for (int x = 0; x < (int) Blocks; x++) {
            for (int y = 0; y < (int) Blocks; y++) {
                //Set co-ordinates
                int TopLeftX = x * BlockLength;
                int TopLeftY = y * BlockLength;
                int BottomRightX = TopLeftX + BlockLength;
                int BottomRightY = TopLeftY + BlockLength;
                int MidpointX = (TopLeftX + BottomRightX) / 2;
                int MidpointY = (TopLeftY + BottomRightY) / 2;

                //Diamond Step
                DiamondStep(TopLeftX, TopLeftY, BottomRightX, BottomRightY, BlockLength);

                //Square Step
                //Top Diamond
                SquareStep(TopLeftX, TopLeftY, BottomRightX, TopLeftY, MidpointX, MidpointY);
                //Left Diamond
                SquareStep(TopLeftX, TopLeftY, TopLeftX, BottomRightY, MidpointX, MidpointY);
                //Bottom Diamond
                SquareStep(BottomRightX, TopLeftY, BottomRightX, BottomRightY, MidpointX, MidpointY);
                //Right Diamond
                SquareStep(TopLeftX, BottomRightY, BottomRightX, BottomRightY, MidpointX, MidpointY);
            }
        }
        //Ratio roughness
        H = H * HDecay;
    }
}