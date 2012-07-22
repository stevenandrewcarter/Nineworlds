#ifndef DIAMONDSQUARE_H
#define DIAMONDSQUARE_H

class DiamondSquare
{
public:		
	void calculate(int iterations, double hrange, double hdecay);
	int getTerrainSize() const;	
	
	float **Height_Map;			
private:
	int Iterations;
	double HDecay;
	double H;
	int Terrain_Size;
	
	void DiamondStep(int TopLeftX, int TopLeftY, int BottomRightX, int BottomRightY, int SquareLength);
	void SquareStep(int X1, int Y1, int X2, int Y2, int X3, int Y3);
};


#endif