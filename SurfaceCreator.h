#ifndef SURFACECREATOR_H
#define SURFACECREATOR_H
#include "bitmap.h"			// Bitmap class
#include "LinMaths.h"

class SurfaceCreator
{
public:
	SurfaceCreator(float **Map, int Terrain_Size, float Terrain_Height, int TileSet);	
private:
	bool Init();
	void SmoothShadowMap();
	void BlendSurfaceTexture();
	void GenerateLightMap();	
	void CombineTextures();
	float Texture_Factor(int h1, float h2, float f);		//Check for percentage of color
	float Limit255(float a);								//Check for invalid values
	float TestPoint(int x, int y);	

	// The bitmap objects		
	bitmap VHighAlt;
	bitmap HighAlt;
	bitmap MidAlt;	
	bitmap LowAlt;	
	bitmap Surface;	
	bitmap Shadows;
	bitmap SoftShadows;
	bitmap Combined;
	float **Height_Map;	
	int T_Size;
	float T_Height;
	int CurrentSet;
};

#endif