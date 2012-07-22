#include "Terrain.h"

double Terrain::CalculateError(Vector& Left, Vector& Right)
{
	// Calculate the line equation to follow
	int x1, x0, y1, y0;
	x0 = static_cast<int>(Left.x); x1 = static_cast<int>(Right.x);
	y0 = static_cast<int>(Left.y); y1 = static_cast<int>(Right.y);
	double x = x1 - x0;
	double y = y1 - y0;
	double ans = 0;
	double base = ( (HeightMap.Height_Map[x1][y1]) + (HeightMap.Height_Map[x0][y0]) ) / 2;		
	if(x == 0)
	{
		if(y0 < y1)
		{
			for(int i = y0; i < y1; i++)
			{				
				ans = ans + abs((HeightMap.Height_Map[x0][i] - base));
			}
		}
		else
		{
			for(int i = y1; i < y0; i++)
			{			
				ans = ans + abs((HeightMap.Height_Map[x0][i] - base));
			}
		}
	}
	else if( y == 0)
	{
		if(x0 < x1)
		{
			for(int i = x0; i < x1; i++)
			{			
				ans = ans + abs((HeightMap.Height_Map[i][y0] - base));			
			}
		}
		else
		{
			for(int i = x1; i < x0; i++)
			{				
				ans = ans + abs((HeightMap.Height_Map[i][y0] - base));			
			}
		}
	}
	else
	{
		double m = y / x;
		double b = y0 - (m * x0);
		if(x0 < x1)
		{
			for(int i = x0; i < x1; i++)
			{	
				int ya = (int)(i * m + b);
				ans = ans + abs((HeightMap.Height_Map[i][ya] - base));			
			}
		}
		else
		{
			for(int i = x1; i < x0; i++)
			{	
				int ya = (int)(i * m + b);
				ans = ans + abs((HeightMap.Height_Map[i][ya] - base));			
			}
		}
	}	
	return ans;					
}

void Terrain::DeleteHeightMap()
{
	for (int i = 0; i < HeightMap.getTerrainSize(); i++)
	{
		delete[] HeightMap.Height_Map[i]; // destroy contents of a single row		
	}
    delete[] HeightMap.Height_Map; // destroy the pointers to each row...		
	delete[] Forests;
}

int Terrain::LoadGLTextures(GLuint *texture, char *filename)
{
	bool Status = true;
	AUX_RGBImageRec *TextureImage = NULL;		
	if ( TextureImage = auxDIBImageLoad(filename) )
	{		
		glGenTextures( 1, texture);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, TextureImage->sizeX, TextureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
	}	
	else
	{
		Status = false;
	}
	if(TextureImage)
	{
		if(TextureImage->data)
		{
			delete TextureImage->data;
		}
		delete TextureImage;
		TextureImage = NULL;
	}
	return Status;	
}

Terrain::Terrain()
{	
	glMultiTexCoord1fARB = NULL;
	glMultiTexCoord2fARB = NULL;
	glActiveTextureARB = NULL;
	glClientActiveTextureARB = NULL;	
	CameraEye = Vector();
	WaterRoll = 0.0f;
}

void Terrain::ROAM(Node* T)
{
	// T has been split and T must not be the root node
	if(T->LeftChild != NULL || T->RightChild != NULL)
	{
		// Should it be merged
		if( ( T->Priority <= 1000 && T->Distance >= 700))
		{
			ForceMerge(T, T);						
		}				
		else
		{
			ROAM(T->LeftChild);
			ROAM(T->RightChild);
		}
	}
	else
	{
		// Should it be split		
		if( (T->Priority > 1000) )
		{
			if( (T->LeftChild == NULL) && (T->RightChild == NULL) && (T != TriangleTree.root) )
			{
	     		ForceSplit(T);   			
			}			
			ROAM(T->LeftChild);
			ROAM(T->RightChild);		
		}
		else if( (T->Priority > 5  && T->Distance < 700) )
		{
			if( (T->LeftChild == NULL) && (T->RightChild == NULL) && (T != TriangleTree.root) )
			{
	     		ForceSplit(T);   			
			}			
			ROAM(T->LeftChild);
			ROAM(T->RightChild);		
		}
	}	
}

void Terrain::ForceSplit(Node* T)
{
	// Recursion Call to force split bases
	if(T->BaseNeighbour != NULL)						
	{		
		if(T->BaseNeighbour->BaseNeighbour != T)
	  	{			
			ForceSplit(T->BaseNeighbour);
		}
		SplitTriangle(T);
		SplitTriangle(T->BaseNeighbour);		
		T->LeftChild->RightNeighbour = T->BaseNeighbour->RightChild;
		T->RightChild->LeftNeighbour = T->BaseNeighbour->LeftChild;
		T->BaseNeighbour->LeftChild->RightNeighbour = T->RightChild;
		T->BaseNeighbour->RightChild->LeftNeighbour = T->LeftChild;
    }
    else
    {
        SplitTriangle(T);
		T->LeftChild->RightNeighbour = NULL;
		T->RightChild->LeftNeighbour = NULL;
    }
}

void Terrain::SplitTriangle(Node* T)
{
	Triangle t = TriangleTree.getCurrentNode(T);
	TriangleTree.ExpandNode(T);
	
	// Step 1 : Get the old triangles points		
	Vector Apex = t.Apex;
	Vector Left = t.Left;
	Vector Right = t.Right;

	// Step 2 : Get new Apex point which is the midpoint between Left and Right		
	int lx = static_cast<int>(Left.x);
	int ly = static_cast<int>(Left.y);
	int rx = static_cast<int>(Right.x);
	int ry = static_cast<int>(Right.y);
	Vector newApex = Vector(static_cast<float>((lx + rx) / 2), static_cast<float>((ly + ry) / 2));    

	// Step 3 : Create first triangle and add to tree ( This is left half of old triangle )	
	double TriangleLeft = CalculateError(Apex , Left);				
	Triangle NewTLeft = Triangle(newApex, Apex, Left); // Apex = new Apex, Left = old Left, Right = Old Apex			
	for(int i = 0; (unsigned)i < t.Tree.size(); i++)
	{	
		Vector Current = t.Tree[i];
		if(PointInTriangle(Vector(Current.x, Current.y), Vector(newApex.x, newApex.y), Vector(Apex.x, Apex.y), Vector(Left.x, Left.y)))		
		{
			NewTLeft.Tree.push_back(Current);			
		}
	}

	// Step 4 : Create the second triangle and add to tree	
	double TriangleRight = CalculateError(Right , Apex);				
	Triangle NewTRight = Triangle(newApex, Right, Apex);	
	for(int i = 0; (unsigned)i < t.Tree.size(); i++)
	{	
		Vector Current = t.Tree[i];
		if(PointInTriangle(Vector(Current.x, Current.y), Vector(newApex.x, newApex.y), Vector(Right.x, Right.y), Vector(Apex.x, Apex.y) ))	
		{
			NewTRight.Tree.push_back(Current);			
		}
	}


	T->LeftChild->LeftNeighbour = T->RightChild;
	T->RightChild->RightNeighbour = T->LeftChild;	
	T->LeftChild->BaseNeighbour = T->LeftNeighbour;	
	T->RightChild->BaseNeighbour = T->RightNeighbour;
	
	if(T->LeftNeighbour != NULL)
	{		    
	   	if(T->LeftNeighbour->BaseNeighbour == T)
		{	
          	T->LeftNeighbour->BaseNeighbour = T->LeftChild;
			T->LeftNeighbour->Parent->RightNeighbour = T->LeftChild;		
		}
		else
		{		 
			T->LeftNeighbour->RightNeighbour = T->LeftChild;
		}
	}
	if(T->RightNeighbour != NULL)
	{
		if(T->RightNeighbour->BaseNeighbour == T)
		{		    
			T->RightNeighbour->BaseNeighbour = T->RightChild;
			T->RightNeighbour->Parent->LeftNeighbour = T->RightChild;
		}
		else
		{ 			    
			T->RightNeighbour->LeftNeighbour = T->RightChild;
		}
	}	
	TriangleTree.InsertAtNode(T->LeftChild, NewTLeft, TriangleLeft);
	TriangleTree.InsertAtNode(T->RightChild, NewTRight, TriangleRight);	
}

void Terrain::ForceMerge(Node* T, Node* End)
{
	if(T != TriangleTree.root)
	{
		if(TriangleTree.isInternal(T->LeftChild))
		{	 
			ForceMerge(T->LeftChild, End);
		}
		else if(TriangleTree.isInternal(T->RightChild))
		{        
			ForceMerge(T->RightChild, End);
		}	
		else if(TriangleTree.isExternal(T))
		{
			ForceMerge(T->Parent, End);
		}
		else if( TriangleTree.isExternal(T->LeftChild) && TriangleTree.isExternal(T->RightChild) ) // Left and Right children of T are external
		{				
			MergeNode(T);
			if(T != End)
			{           
				ForceMerge(T->Parent, End);
			}
		}
	}
}

void Terrain::MergeNode(Node* T)
{
	if(T->LeftNeighbour != NULL)
	{
		if(T->LeftNeighbour->LeftChild != NULL || T->LeftNeighbour->RightChild != NULL)
		{
			// Merge LeftNeighbour
		    ForceMerge(T->LeftNeighbour, T->LeftNeighbour);			
		}      
		if(T->LeftNeighbour->BaseNeighbour == T->LeftChild)
		{
			T->LeftNeighbour->BaseNeighbour = T;
			T->LeftNeighbour->Parent->RightNeighbour = T;
		}
		else
		{
			T->LeftNeighbour->RightNeighbour = T;				
		} 		
	}      
	if(T->RightNeighbour != NULL)
	{
		if(T->RightNeighbour->LeftChild != NULL || T->RightNeighbour->RightChild != NULL)
		{
			// Merge RightNeighbour
			ForceMerge(T->RightNeighbour, T->RightNeighbour);      
		}      
		if(T->RightNeighbour->BaseNeighbour == T->RightChild)
		{
			T->RightNeighbour->BaseNeighbour = T;
			T->RightNeighbour->Parent->LeftNeighbour = T;
		}
		else
		{
			T->RightNeighbour->LeftNeighbour = T;				
		}  				
	}	

	// Remove Children		
	Node* Left = T->LeftChild;
	Left->Parent = NULL;
	Left->LeftNeighbour = NULL;
	Left->RightNeighbour = NULL;
	Left->BaseNeighbour = NULL;

	Node* Right = T->RightChild;
	Right->Parent = NULL;
	Right->LeftNeighbour = NULL;
	Right->RightNeighbour = NULL;
	Right->BaseNeighbour = NULL;

	T->LeftChild = NULL;
	T->RightChild = NULL;  

	if(T->BaseNeighbour != NULL)
	{
		if(T->BaseNeighbour->LeftChild != NULL && T->BaseNeighbour->RightChild != NULL)
		{
			ForceMerge(T->BaseNeighbour, T->BaseNeighbour);
		}
	}      	
	delete Left;
	delete Right;	
	Left = NULL;
	Right = NULL;	
}

void Terrain::DrawTerrain(Vector CurrentCamera, const Vector viewFrustrum[])
{
	CameraEye = CurrentCamera;	
	ViewFrustrum[0] = viewFrustrum[0];	
	ViewFrustrum[1] = viewFrustrum[1];
	ViewFrustrum[2] = viewFrustrum[2];
	ViewFrustrum[3] = viewFrustrum[3];
	ROAM(TriangleTree.root);
	DrawSkyBox();
	DrawTriangleTree(TriangleTree.root);
	glEnable(GL_BLEND);	
	DrawWater();
	glDisable(GL_BLEND);		
	glFlush();	
}

void Terrain::GenerateHeightMap(int Iterations, double Height, double HDecay)
{
	HeightMap.calculate(Iterations, Height, HDecay);	
	int Select = 1 + rand() % (4 - 1 + 1);		
	// Load .3DS file into model structure	
	if(Select == 1)
	{
		GenerateTerrainObjects(50, 2 , 40, 10);
		g_Load3ds.Import3DS(&g_3DModel, "Textures/Tilesets/Desert/Models/PILLAR.3DS");
		double Ratio = 0.25;
		O1Offset = 0.0f;
		BuildLists(Ratio, 1, g_3DModel);		
		for(int i = 0; i < g_3DModel.numOfObjects; i++)
		{
			// Free the faces, normals, vertices, and texture coordinates.
			delete [] g_3DModel.pObject[i].pFaces;
			delete [] g_3DModel.pObject[i].pNormals;
			delete [] g_3DModel.pObject[i].pVerts;
			delete [] g_3DModel.pObject[i].pTexVerts;
		}	
		
		g_Load3ds.Import3DS(&g_3DModel1, "Textures/Tilesets/Desert/Models/STATUE.3DS");
		Ratio = 0.25;
		O1Offset = 0.0f;
		BuildLists(Ratio, 2, g_3DModel1);

		// Go through all the objects in the scene
		for(int i = 0; i < g_3DModel1.numOfObjects; i++)
		{
			// Free the faces, normals, vertices, and texture coordinates.
			delete [] g_3DModel1.pObject[i].pFaces;
			delete [] g_3DModel1.pObject[i].pNormals;
			delete [] g_3DModel1.pObject[i].pVerts;
			delete [] g_3DModel1.pObject[i].pTexVerts;
		}			
	}
	else if(Select == 2)
	{		
		GenerateTerrainObjects(100, 2 , 50, 50);
		g_Load3ds.Import3DS(&g_3DModel, "Textures/Tilesets/Mountains/Models/PINE.3DS");
		double Ratio = 0.25;
		O1Offset = 15.5f;
		BuildLists(Ratio, 1, g_3DModel);		
		for(int i = 0; i < g_3DModel.numOfObjects; i++)
		{
			// Free the faces, normals, vertices, and texture coordinates.
			delete [] g_3DModel.pObject[i].pFaces;
			delete [] g_3DModel.pObject[i].pNormals;
			delete [] g_3DModel.pObject[i].pVerts;
			delete [] g_3DModel.pObject[i].pTexVerts;
		}	
		
		g_Load3ds.Import3DS(&g_3DModel1, "Textures/Tilesets/Mountains/Models/MAPLE.3DS");
		Ratio = 5.0;
		O2Offset = 0.0f;
		BuildLists(Ratio, 2, g_3DModel1);

		// Go through all the objects in the scene
		for(int i = 0; i < g_3DModel1.numOfObjects; i++)
		{
			// Free the faces, normals, vertices, and texture coordinates.
			delete [] g_3DModel1.pObject[i].pFaces;
			delete [] g_3DModel1.pObject[i].pNormals;
			delete [] g_3DModel1.pObject[i].pVerts;
			delete [] g_3DModel1.pObject[i].pTexVerts;
		}		
	}
	else if(Select == 3)
	{
		GenerateTerrainObjects(100, 2 , 50, 50);
		g_Load3ds.Import3DS(&g_3DModel, "Textures/Tilesets/Tropics/Models/TREE3.3DS");
		double Ratio = 0.5;
		O1Offset = 0.0f;
		BuildLists(Ratio, 1, g_3DModel);		
		for(int i = 0; i < g_3DModel.numOfObjects; i++)
		{
			// Free the faces, normals, vertices, and texture coordinates.
			delete [] g_3DModel.pObject[i].pFaces;
			delete [] g_3DModel.pObject[i].pNormals;
			delete [] g_3DModel.pObject[i].pVerts;
			delete [] g_3DModel.pObject[i].pTexVerts;
		}	
		
		g_Load3ds.Import3DS(&g_3DModel1, "Textures/Tilesets/Tropics/Models/PALM.3DS");
		Ratio = 0.75;
		O2Offset = 10.0f;
		BuildLists(Ratio, 2, g_3DModel1);

		// Go through all the objects in the scene
		for(int i = 0; i < g_3DModel1.numOfObjects; i++)
		{
			// Free the faces, normals, vertices, and texture coordinates.
			delete [] g_3DModel1.pObject[i].pFaces;
			delete [] g_3DModel1.pObject[i].pNormals;
			delete [] g_3DModel1.pObject[i].pVerts;
			delete [] g_3DModel1.pObject[i].pTexVerts;
		}		
	
	}
	else if(Select == 4)
	{		
		GenerateTerrainObjects(100, 2 , 75, 15);
		g_Load3ds.Import3DS(&g_3DModel, "Textures/Tilesets/Volcanic/Models/DEADTREE.3DS");
		double Ratio = 0.1;
		O1Offset = -1.0f;
		BuildLists(Ratio, 1, g_3DModel);		
		for(int i = 0; i < g_3DModel.numOfObjects; i++)
		{
			// Free the faces, normals, vertices, and texture coordinates.
			delete [] g_3DModel.pObject[i].pFaces;
			delete [] g_3DModel.pObject[i].pNormals;
			delete [] g_3DModel.pObject[i].pVerts;
			delete [] g_3DModel.pObject[i].pTexVerts;
		}	
		
		g_Load3ds.Import3DS(&g_3DModel1, "Textures/Tilesets/Volcanic/Models/TREE1.3DS");
		Ratio = 0.15;
		O2Offset = 2.0f;
		BuildLists(Ratio, 2, g_3DModel1);

		// Go through all the objects in the scene
		for(int i = 0; i < g_3DModel1.numOfObjects; i++)
		{
			// Free the faces, normals, vertices, and texture coordinates.
			delete [] g_3DModel1.pObject[i].pFaces;
			delete [] g_3DModel1.pObject[i].pNormals;
			delete [] g_3DModel1.pObject[i].pVerts;
			delete [] g_3DModel1.pObject[i].pTexVerts;
		}	
	
	}
	SurfaceCreator s1 = SurfaceCreator(HeightMap.Height_Map, HeightMap.getTerrainSize() - 1, static_cast<float>(Height), Select);
	multitextureSupported = initMultitexture();

	TriangleTree.ExpandNode(TriangleTree.root);
	Node* Current = TriangleTree.root;
	
	//Create Triangle t1	
	Vector Apex = Vector(0, 0, HeightMap.Height_Map[0][0]);
	Vector Left = Vector(0, static_cast<float>(HeightMap.getTerrainSize() - 1), HeightMap.Height_Map[0][HeightMap.getTerrainSize() - 1]);
	Vector Right = Vector(static_cast<float>(HeightMap.getTerrainSize() - 1), 0,HeightMap.Height_Map[HeightMap.getTerrainSize() - 1][0] );
	Triangle t = Triangle(Apex, Left, Right);
	for(int i = 0; i < 100; i++)
	{	
		if(PointInTriangle(Vector(Forests[i].x, Forests[i].y), Apex, Left, Right))	
		{
			t.Tree.push_back(Forests[i]);
		}
	}
	double E = CalculateError(Left, Right);		
	Current->LeftChild->BaseNeighbour = Current->RightChild;
	TriangleTree.InsertAtNode(Current->LeftChild, t, E);	

	// Create Triangle t2	
	Apex.set(static_cast<float>(HeightMap.getTerrainSize() - 1) , static_cast<float>(HeightMap.getTerrainSize() - 1), HeightMap.Height_Map[HeightMap.getTerrainSize() - 1][HeightMap.getTerrainSize() - 1]);
	Left.set(static_cast<float>(HeightMap.getTerrainSize() - 1), 0, HeightMap.Height_Map[HeightMap.getTerrainSize() - 1][0]);
	Right.set(0, static_cast<float>(HeightMap.getTerrainSize() - 1), HeightMap.Height_Map[0][HeightMap.getTerrainSize() - 1]);
	Triangle t2 = Triangle(Apex, Left, Right);	
	for(int i = 0; i < 100; i++)
	{	
		if(PointInTriangle(Vector(Forests[i].x, Forests[i].y), Apex, Left, Right))
		{
			t2.Tree.push_back(Forests[i]);
		}
	}	
	Current->RightChild->BaseNeighbour = Current->LeftChild;
	TriangleTree.InsertAtNode(Current->RightChild, t2, E);	

	LoadTGA(&SkyBoxTexture, "SkyBox/CLOUDS.tga");
	LoadGLTextures(&WaterTexture, "Textures/WATER1.bmp");
	LoadGLTextures(&SurfaceTexture , "Data/Surface.bmp");
	LoadGLTextures(&ShadowTexture  , "Data/Shadows.bmp");	
}

void Terrain::DrawTriangleTree(Node* Next)
{
	Triangle t = TriangleTree.getCurrentNode(Next);		
	Vector Apex = t.Apex;
	Vector Camera = Vector(CameraEye.x,  CameraEye.z , CameraEye.y);
	double dst = lm_dst(Camera, Apex);
	Next->Distance = dst;
	int x = static_cast<int>(Apex.x);
	int y = static_cast<int>(Apex.y);
	float z = HeightMap.Height_Map[x][y];
		
	if(TriangleTree.isInternal(Next))
	{
		DrawTriangleTree(TriangleTree.LeftChild(Next));
		DrawTriangleTree(TriangleTree.RightChild(Next));	
	}
	else
	{					
		Vector Left = t.Left;
		Vector Right = t.Right;			
		if(dst < 600)
		{
			//Left
			x = static_cast<int>(Left.x);
			y = static_cast<int>(Left.y);
			z = HeightMap.Height_Map[x][y];									
			Vector TrianglePoints = Vector(static_cast<float>(x) , static_cast<float>(z) , static_cast<float>(y));								
			bool showLeft = inFrustrum(TrianglePoints);		
			
			//Right
			x = static_cast<int>(Right.x);
			y = static_cast<int>(Right.y);
			z = HeightMap.Height_Map[x][y];								
			TrianglePoints = Vector(static_cast<float>(x) , static_cast<float>(z) , static_cast<float>(y));	
			bool showRight = inFrustrum(TrianglePoints);
		
			//Apex point
			x = static_cast<int>(Apex.x);
			y = static_cast<int>(Apex.y);
			z = HeightMap.Height_Map[x][y];							
			TrianglePoints = Vector(static_cast<float>(x) , static_cast<float>(z) , static_cast<float>(y));							
			bool showApex = inFrustrum(TrianglePoints);		
		
			if(showLeft == true || showRight == true || showApex == true)
			{
				glActiveTextureARB(GL_TEXTURE0_ARB);
				glEnable(GL_TEXTURE_2D);							// Enable Textures
				glBindTexture(GL_TEXTURE_2D, SurfaceTexture);				
				glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);			
				
				glActiveTextureARB(GL_TEXTURE1_ARB);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, ShadowTexture);
				glEnable(GL_TEXTURE_2D);							// Enable Textures
				GLfloat size = static_cast<float>(HeightMap.getTerrainSize() - 1);				
				glBegin(GL_TRIANGLE_STRIP);							
							
					x = static_cast<int>(Left.x);
					y = static_cast<int>(Left.y);
					z = HeightMap.Height_Map[x][y];	
					GLfloat t1 = x / size;
					GLfloat t2 = -(size - y) / size;					
					glMultiTexCoord2fARB( GL_TEXTURE0_ARB, t1, t2);
					glMultiTexCoord2fARB( GL_TEXTURE1_ARB, t1, t2);					
					glVertex3f((float)x , (float)z, (float)y);	
	
					//Right								
					x = static_cast<int>(Right.x);
					y = static_cast<int>(Right.y);
					z = HeightMap.Height_Map[x][y];									
					t1 = x / size;
					t2 = -(size - y) / size;					
					glMultiTexCoord2fARB( GL_TEXTURE0_ARB, t1, t2);
					glMultiTexCoord2fARB( GL_TEXTURE1_ARB, t1, t2);					
					glVertex3f((float)x , (float)z, (float)y);		
	
					//Apex point								
					x = static_cast<int>(Apex.x);
					y = static_cast<int>(Apex.y);
					z = HeightMap.Height_Map[x][y];						
					t1 = x / size;
					t2 = -(size - y) / size;					
					glMultiTexCoord2fARB( GL_TEXTURE0_ARB, t1, t2);
					glMultiTexCoord2fARB( GL_TEXTURE1_ARB, t1, t2);					
					glVertex3f((float)x , (float)z, (float)y);					
				glEnd();						
				for(int i = 0; i < t.Tree.size(); i++)
				{
					x = static_cast<int>(Left.x);
					y = static_cast<int>(Left.y);
					z = HeightMap.Height_Map[x][y];	
					Vector Q1 = Vector(x, y, z);

					x = static_cast<int>(Right.x);
					y = static_cast<int>(Right.y);
					z = HeightMap.Height_Map[x][y];									
					Vector Q2 = Vector(x, y, z);

					x = static_cast<int>(Apex.x);
					y = static_cast<int>(Apex.y);
					z = HeightMap.Height_Map[x][y];											
					Vector Q = Vector(x, y ,z); 
					Vector N = (Q - Q1) * (Q - Q2);
					N = N.Normalize();					
					Vector P = t.Tree[i];
					Vector PlanePoint = P - (N * (lm_dotproduct(N, (P - Q))));
					DrawTerrainObjects(PlanePoint.x, PlanePoint.z, PlanePoint.y, t.Tree[i].t, t.Tree[i].Object);
				}
				glActiveTextureARB(GL_TEXTURE1_ARB);
				glDisable(GL_TEXTURE_2D);
				glActiveTextureARB(GL_TEXTURE0_ARB);
				glDisable(GL_TEXTURE_2D);			
			}					
		}
	}
}

bool Terrain::inFrustrum(const Vector& TestPoint)
{	
	HalfSpace ViewLeft  = ClassifyPoint(CameraEye, ViewFrustrum[0], TestPoint);
	HalfSpace ViewRight = ClassifyPoint(CameraEye, ViewFrustrum[1], TestPoint);		
	HalfSpace ViewUp    = ClassifyPoint(CameraEye, ViewFrustrum[2], TestPoint);
	HalfSpace ViewDown  = ClassifyPoint(CameraEye, ViewFrustrum[3], TestPoint);	
	if( ViewLeft == POSITIVE && ViewRight == NEGATIVE && ViewDown == NEGATIVE && ViewUp == POSITIVE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Terrain::DrawWater()
{
	float size = (float)(HeightMap.getTerrainSize() - 1);	
	glBindTexture(GL_TEXTURE_2D, WaterTexture);
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 0.7f);
	Vector Camera = Vector(CameraEye.x, CameraEye.z, CameraEye.y);	
	float WaterLevel = 0.0f;	
	glBegin(GL_QUADS);
		for(int i = 1; i < size - 10; i+=10)
		{
			for(int j = 1; j < size - 10; j+=10)
			{					
				if(!(HeightMap.Height_Map[i][j] > WaterLevel + 5) || !(HeightMap.Height_Map[i + 10][j + 10] > WaterLevel + 5) || !(HeightMap.Height_Map[i + 10][j] > WaterLevel + 5) || !(HeightMap.Height_Map[i][j + 10] > WaterLevel + 5))
				{
					if(lm_dst(Vector(i      , j     ), Camera) < 600)
					{
						GLfloat t1 = i / (size / 2);
						GLfloat t2 = -( (size / 2) - j) / (size / 2);					
						glTexCoord2d( t1 + WaterRoll, WaterRoll + t2); glVertex3f(static_cast<float>(i)      , WaterLevel , static_cast<float>(j)     );
						t1 = (i + 10) / (size / 2);
						t2 = -( (size / 2) - j) / (size / 2);										
						glTexCoord2d( t1 + WaterRoll, WaterRoll + t2); glVertex3f(static_cast<float>(i + 10) , WaterLevel  , static_cast<float>(j)     );
						t1 = (i + 10) / (size / 2);
						t2 = -( (size / 2) - (j + 10)) / (size / 2);										
						glTexCoord2d( t1 + WaterRoll, WaterRoll + t2); glVertex3f(static_cast<float>(i + 10) , WaterLevel , static_cast<float>(j + 10));
						t1 = i / (size / 2);
						t2 = -( (size / 2) - (j + 10)) / (size / 2);										
						glTexCoord2d( t1 + WaterRoll, WaterRoll + t2); glVertex3f(static_cast<float>(i)      , WaterLevel  , static_cast<float>(j + 10));
					}
				}								
			}
		}
	glEnd();		
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	WaterRoll+=0.0005f;	
	glDisable(GL_TEXTURE_2D);		
}

void Terrain::DrawSkyBox()
{	
	glBindTexture(GL_TEXTURE_2D, SkyBoxTexture.texID);	
	glEnable(GL_TEXTURE_2D);	
	float size = (float)(HeightMap.getTerrainSize());	
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_QUADS);
		float x = -(size / 2) + (float)CameraEye.x;
		float y = CameraEye.y + 50.0f;
		float z = -(size / 2) + (float)CameraEye.z;
		glTexCoord2d(SkyRoll + 0.0f, 0.0f); glVertex3f(x , y, z);

		x = -(size / 2) + (float)CameraEye.x;
		y = CameraEye.y + 50.0f;
		z =  (size / 2) + (float)CameraEye.z;		
		glTexCoord2d(SkyRoll + 0.0f, 1.0f); glVertex3f(x , y, z);

		x = (size / 2) + (float)CameraEye.x;
		y = CameraEye.y + 50.0f;
		z = (size / 2) + (float)CameraEye.z;		
		glTexCoord2d(SkyRoll + 1.0f, 1.0f); glVertex3f(x , y, z);

		x = (size / 2) + CameraEye.x;
		y = CameraEye.y + 50.0f;
		z = -(size / 2) + CameraEye.z;				
		glTexCoord2d(SkyRoll + 1.0f, 0.0f); glVertex3f(x , y, z);
	glEnd();	
	glDisable(GL_TEXTURE_2D);		
	glEnable(GL_DEPTH_TEST);
	SkyRoll+=0.00025f;
}

bool Terrain::LoadTGA(TextureImage *texture, char *filename)
{
	GLubyte TGAheader[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	GLubyte TGAcompare[12];
	GLubyte header[6];
	GLuint bytesPerPixel;
	GLuint imageSize;
	GLuint temp;
	GLuint type = GL_RGBA;
	FILE *file = fopen(filename, "rb");
	if(file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) || memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 || fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		if(file == NULL)
			return false;
		else
		{
			fclose(file);
			return false;
		}
	}
	texture->width = header[1] * 256 + header[0];
	texture->height = header[3] * 256 + header[2];
	if( texture->width <= 0 || texture->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		fclose(file);
		return false;
	}
	texture->bpp = header[4];
	bytesPerPixel = texture->bpp / 8;
	imageSize = texture->width * texture->height * bytesPerPixel;
	texture->imageData = (GLubyte *)malloc(imageSize);
	if( texture->imageData == NULL || fread(texture->imageData, 1, imageSize, file) != imageSize)
	{
		if(texture->imageData != NULL)
			free(texture->imageData);

		fclose(file);
		return false;
	}
	for(GLuint i = 0; i < int(imageSize); i+=bytesPerPixel)
	{
		temp = texture->imageData[i];
		texture->imageData[i] = texture->imageData[i + 2];
		texture->imageData[i + 2] = temp;
	}
	fclose(file);

	glGenTextures(1, &texture[0].texID);
	glBindTexture(GL_TEXTURE_2D, texture[0].texID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if( texture[0].bpp == 24 )
	{
		type = GL_RGB;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, type, texture[0].width, texture[0].height, 0, type, GL_UNSIGNED_BYTE, texture[0].imageData);
	return true;
}

void Terrain::DrawTerrainObjects(float x, float y, float z, int pos, int type)
{
	glEnable(GL_LIGHT0);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting	
	Vector TreePoints = Vector(x,y,z);
	double dst = lm_dst(TreePoints, CameraEye);
	if(dst < 500)
	{	
		if(type == 1)
		{		
			y = y +  O1Offset;
			glTranslatef(x, y , z );		
			glRotatef(pos, 0.0f, 1.0f, 0.0f);
			glCallList(Object1);	
			glRotatef(-pos, 0.0f, 1.0f, 0.0f);
			glTranslatef(-x, -y, -z);
		}
		else
		{
			y = y +  O2Offset;
			glTranslatef(x, y , z );		
			glRotatef(pos, 0.0f, 1.0f, 0.0f);
			glCallList(Object2);	
			glRotatef(-pos, 0.0f, 1.0f, 0.0f);
			glTranslatef(-x, -y, -z);		
		}
		
	}	
	glColor3f(1.0f, 1.0f, 1.0f);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);	
}

void Terrain::GenerateTerrainObjects(int Number, int types, int amounta, int amountb)
{
	Forests = new Vector[Number];	
	for(int i = 0; i < Number; i++)
	{
		bool valid = false;
		int x;
		int y;
		int a;
		int b;
		while(!valid)
		{
			a = 10;
			b = HeightMap.getTerrainSize() - 10;
			x = a + rand() % ( b - a + 1);
			y = a + rand() % ( b - a + 1);					
			if((HeightMap.Height_Map[x][y] < 70 && HeightMap.Height_Map[x][y] > 10))
			{
				valid = true;
			}
		}	
		Forests[i].x = static_cast<float>(x); 	
		Forests[i].y = static_cast<float>(y); 	
		Forests[i].z = HeightMap.Height_Map[(int)Forests[i].x][(int)Forests[i].y];
		a = 0;
		b = 360;
		int Select;
		if(amounta > 0)
		{
			Select = 1;
			amounta--;
		}
		else
		{
			Select = 2;
		}		
		float c = a + rand() % ( b - a + 1); 
		Forests[i].t = c;	
		Forests[i].Object = Select;
	}	
}

bool Terrain::isInString(char *string, const char *search) 
{
	int pos=0;
	int maxpos=strlen(search)-1;
	int len=strlen(string);	
	char *other;
	for (int i=0; i<len; i++) {
		if ((i==0) || ((i>1) && string[i-1]=='\n')) {				
			other=&string[i];			
			pos=0;													// Begin New Search
			while (string[i]!='\n') {								// Search Whole Extension-String
				if (string[i]==search[pos]) pos++;					
				if ((pos>maxpos) && string[i+1]=='\n') return true; 
				i++;
			}			
		}
	}	
	return false;													
}

bool Terrain::initMultitexture(void)
{
	char *extensions;	
	extensions=strdup((char *) glGetString(GL_EXTENSIONS));			// Fetch Extension String
	int len=strlen(extensions);
	for (int i=0; i<len; i++)										// Separate It By Newline Instead Of Blank
		if (extensions[i]==' ') extensions[i]='\n';

#ifdef EXT_INFO
	MessageBox(hWnd,extensions,"supported GL extensions",MB_OK | MB_ICONINFORMATION);
#endif

	if (isInString(extensions,"GL_ARB_multitexture")				// Is Multitexturing Supported?
		&& __ARB_ENABLE												// Override-Flag
		&& isInString(extensions,"GL_EXT_texture_env_combine"))		// Is texture_env_combining Supported?
	{	
		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&maxTexelUnits);
		glMultiTexCoord1fARB	= (PFNGLMULTITEXCOORD1FARBPROC)		wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");
		glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC)	wglGetProcAddress("glClientActiveTextureARB");		
#ifdef EXT_INFO
	MessageBox(hWnd,"The GL_ARB_multitexture extension will be used.","feature supported!",MB_OK | MB_ICONINFORMATION);
#endif
		return true;
	}
	useMultitexture=false;											
	return false;
}

GLvoid Terrain::BuildLists(double Ratio, int DList, t3DModel Model)
{
	if(DList == 1)
	{
		Object1 = glGenLists(1);
		glNewList(Object1, GL_COMPILE);
	}
	else
	{
		Object2 = glGenLists(1);
		glNewList(Object2, GL_COMPILE);
	}
	for(int i = 0; i < Model.numOfObjects; i++)
	{		
		if(Model.pObject.size() <= 0) break;	
		t3DObject *pObject = &Model.pObject[i];			
		glBegin(GL_TRIANGLES);					
			// Go through all of the faces (polygons) of the object and draw them
			for(int j = 0; j < pObject->numOfFaces; j++)
			{
				// Go through each corner of the triangle and draw it.
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					// Get the index for each point of the face
					int index = pObject->pFaces[j].vertIndex[whichVertex];									
					glNormal3f(pObject->pNormals[ index ].x, pObject->pNormals[ index ].y, pObject->pNormals[ index ].z);					
					if(Model.pMaterials.size() && pObject->materialID >= 0) 
					{
						// Get and set the color that the object is, since it must not have a texture
						BYTE *pColor = Model.pMaterials[pObject->materialID].color;
						// Assign the current color to this model
						glColor3ub(pColor[0], pColor[1], pColor[2]);
					}				
					// Pass in the current vertex of the object (Corner of current face)
					glVertex3f(pObject->pVerts[ index ].x * Ratio, pObject->pVerts[ index ].y * Ratio, pObject->pVerts[ index ].z * Ratio);
				}
			}
		glEnd();								
	}
	glEndList();
}
