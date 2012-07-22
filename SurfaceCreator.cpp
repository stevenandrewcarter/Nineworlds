#include "SurfaceCreator.h"

SurfaceCreator::SurfaceCreator(float **Map, int Terrain_Size, float Terrain_Height, int TileSet)
{
	CurrentSet = TileSet;
	Height_Map = Map;
	T_Size = Terrain_Size;
	T_Height = Terrain_Height;
	Init();
	BlendSurfaceTexture();	
	GenerateLightMap();
	SmoothShadowMap();		
}

void SurfaceCreator::BlendSurfaceTexture()
{
	float h1 , f0, f1, f2, f3;										//h1 is heigtmap value at position x/z, f0 and f1 are percentage of color resulting due to the heightmap value stored in h1
	BYTE r[5], g[5], b[5], new_r, new_g, new_b;							//Store color at pixel x/z of both images (i1 and i2), store the new calculated values

	for (int y = 0; y < Surface.getheight(); y++)							//Go through all pixels
	{
		for (int x = 0; x < Surface.getwidth(); x++)
		{			
			h1 = Height_Map[x][y];
			h1 = (float)(h1 + T_Height);
			int absHeight = (int)(T_Height * 2);
			// Snow peaks
			if(h1 > absHeight)
			{
				h1 = (float)absHeight;
			}			
			h1 = (h1 / absHeight) * 255;
			f0 = Texture_Factor(256, h1, 0);
	        f1 = Texture_Factor(192, h1, 0);
		    f2 = Texture_Factor(128, h1, 0);
			f3 = Texture_Factor(64, h1, 0);			

			int ax = x % VHighAlt.getwidth();
			int ay = y % VHighAlt.getheight();
			VHighAlt.getcolor(ax, ay, r[0], g[0], b[0]);
			HighAlt.getcolor(ax, ay, r[1], g[1], b[1]);					//Get color of pixel x/z from bitmap
			MidAlt.getcolor(ax, ay, r[2], g[2], b[2]);					//Get color of pixel x/z from bitmap
			LowAlt.getcolor(ax, ay, r[3], g[3], b[3]);					//Get color of pixel x/z from bitmap			
			
			new_r = (unsigned)Limit255( (r[0] * f0) + (r[1] * f1) + (r[2] * f2) + (r[3] * f3) );	//Calculate new r value for the pixel to be written
			new_g = (unsigned)Limit255( (g[0] * f0) + (g[1] * f1) + (g[2] * f2) + (g[3] * f3) );	//Calculate new g value for the pixel to be written
			new_b = (unsigned)Limit255( (b[0] * f0) + (b[1] * f1) + (b[2] * f2) + (b[3] * f3) );	//Calculate new b value for the pixel to be written		
			Surface.setcolor(x, y, new_r, new_g, new_b);				//Set new pixel color in final
		}
	}		
	if (!Surface.save ("Data/Surface.bmp") )							//Save bitmap
	{
		MessageBox(NULL, "Couldn't save all Tex", "ERROR", MB_ICONINFORMATION | MB_OK); 		
	}
}

float SurfaceCreator::Limit255(float a)		
{
	if(a < 0.0f) 
	{
		return 0.0f;
	}
	else if(a > 255.0f) 
	{
		return 255.0f;
	}
	else 
	{
		return a;
	}
}

float SurfaceCreator::Texture_Factor(int h1, float h2, float f)		//Check for percentage of color
{
  float percent;
  percent = (64.0f - abs(h1 - h2)) / 64.0f;
 
  if(percent < 0.0f) percent = 0.0f;
  else if(percent > 1.0f) percent = 1.0f;

  return percent;

}

bool SurfaceCreator::Init()
{	
	if(CurrentSet == 1)
	{
		VHighAlt.load("Textures/Tilesets/Desert/MUD_TOP.bmp");
		HighAlt.load("Textures/Tilesets/Desert/SAND_UP.bmp");
		MidAlt.load("Textures/Tilesets/Desert/SAND_MID.bmp");	
		LowAlt.load("Textures/Tilesets/Desert/SAND_LOW.bmp");	
	}
	else if(CurrentSet == 2)
	{
		VHighAlt.load("Textures/Tilesets/Mountains/SNOW.bmp");
		HighAlt.load("Textures/Tilesets/Mountains/ROCK_HIGH.bmp");
		MidAlt.load("Textures/Tilesets/Mountains/GRASS_MID.bmp");	
		LowAlt.load("Textures/Tilesets/Mountains/GRASS_LOW.bmp");	
	}
	else if(CurrentSet == 3)
	{
		VHighAlt.load("Textures/Tilesets/Tropics/ROCK_HIGH.bmp");
		HighAlt.load("Textures/Tilesets/Tropics/GRASS_HIGH.bmp");
		MidAlt.load("Textures/Tilesets/Tropics/GRASS_MID.bmp");	
		LowAlt.load("Textures/Tilesets/Tropics/SAND_LOW.bmp");	
	}
	else if(CurrentSet == 4)
	{
		VHighAlt.load("Textures/Tilesets/Volcanic/ROCK_TOP.bmp");
		HighAlt.load("Textures/Tilesets/Volcanic/ROCK_HIGH.bmp");
		MidAlt.load("Textures/Tilesets/Volcanic/ROCK_MID.bmp");	
		LowAlt.load("Textures/Tilesets/Volcanic/ROCK_LOW.bmp");	
	}
	Surface.create( T_Size , T_Size );	
	Shadows.create( T_Size , T_Size );
	return true;
}

void SurfaceCreator::GenerateLightMap()
{
	float Shadow_Map;
	BYTE new_color;
	for(int i = 0; i < T_Size; i++)
	{
		for(int j = 0; j < T_Size; j++)
		{
			Shadow_Map = TestPoint(i,j);
			if(Shadow_Map < 0.0f) Shadow_Map = 0.0f;
			else if(Shadow_Map > 1.0f) Shadow_Map = 1.0f;
			new_color = Shadow_Map * 255;
			Shadows.setcolor(i, j, new_color, new_color, new_color);				
		}		
	}		
}

float SurfaceCreator::TestPoint(int x, int y)
{
	Vector Sun = Vector(256 , 256 , 200);
	Vector A   = Vector( x , y , Height_Map[x][y]);	
	for(double t = 0; t < 1; t+=0.05)
	{
		Vector Compare = lm_parametric(A, Sun, t);
		if(Compare.z < Height_Map[(int)Compare.x][(int)Compare.y])
		{
			return 0.5f;
		}
	}
	Vector L = Sun - A;
	Vector Left = Vector(x,y,Height_Map[x][y]); 
	Vector Up = Vector(x,y,Height_Map[x][y]);
	if( !(x + 2 > T_Size) )
	{
		Left = Vector(x + 1, y , Height_Map[x+1][y]);
	}
	if( !(y + 2 > T_Size) )
	{
		Up = Vector(x, y + 1, Height_Map[x][y+1]);
	}
	Vector RLeft = A - Left;
	Vector RUp = A - Up;
	Vector N = RLeft.Normalize() * RUp.Normalize();
	float ret = lm_dotproduct( L.Normalize(), N.Normalize());
	if(ret < 0.0f)
	{
		return 0.5f;
	}
	float retv = 0.5f + ret;
	return retv;
}

void SurfaceCreator::SmoothShadowMap()
{
	SoftShadows.create(T_Size, T_Size);
	BYTE r[9], g[9], b[9], new_r;							
	for(int x = 1; x < T_Size - 1; x++)
	{
		for(int y = 1; y < T_Size - 1; y++)
		{
			Shadows.getcolor(x    , y    , r[0], g[0], b[0]);
			Shadows.getcolor(x    , y + 1, r[1], g[1], b[1]);					//Get color of pixel x/z from bitmap2
			Shadows.getcolor(x    , y - 1, r[2], g[2], b[2]);					//Get color of pixel x/z from bitmap2
			Shadows.getcolor(x + 1, y    , r[3], g[3], b[3]);					//Get color of pixel x/z from bitmap1			
			Shadows.getcolor(x - 1, y    , r[4], g[4], b[4]);					//Get color of pixel x/z from bitmap2
			Shadows.getcolor(x - 1, y - 1, r[5], g[5], b[5]);					//Get color of pixel x/z from bitmap2
			Shadows.getcolor(x - 1, y + 1, r[6], g[6], b[6]);					//Get color of pixel x/z from bitmap2
			Shadows.getcolor(x + 1, y - 1, r[7], g[7], b[7]);					//Get color of pixel x/z from bitmap2
			Shadows.getcolor(x + 1, y + 1, r[8], g[8], b[8]);					//Get color of pixel x/z from bitmap2			
			new_r = Limit255((r[0] + r[1] + r[2] + r[3] + r[4] + r[5] + r[6] + r[7] + r[8]) / 9);
			SoftShadows.setcolor(x, y, new_r, new_r, new_r);
		}
	}
	SoftShadows.save("Data/Shadows.bmp");
}