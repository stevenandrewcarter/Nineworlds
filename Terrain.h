#ifndef TERRAIN_H
#define TERRAIN_H

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library
#include "glext.h"			// Header File for MultiTexturing
#include "BinTree.h"		// Handcrafted Binary tree
#include "DiamondSquare.h"  // Diamond Square Algorithm, Creates Heightmaps
#include "LinMaths.h"		// Handcrafted Mathematical functions
#include "SurfaceCreator.h" // Generates the Terrain Textures
#include "Triangle.h"		// Storage class for triangles
#include "Vector.h"			// Vector Class
#include "3ds.h"

#define __ARB_ENABLE true
#define MAX_EXTENSION_SPACE 10240
#define MAX_EXTENSION_LENGTH 256
#define MAX_TEXTURES 100

// These are for our multitexture defines
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1

#define GL_COMBINE_ARB                        0x8570
#define GL_RGB_SCALE_ARB                    0x8573

typedef struct {
    GLubyte *imageData;
    GLuint bpp;
    GLuint width;
    GLuint height;
    GLuint texID;
} TextureImage;

struct Plane {
    float a, b, c, d;
};

class Terrain {
public:
    // Constructor
    Terrain();

    // Mutators
    void DrawTerrain(Vector CurrentCamera,
                     const Vector viewFrustrum[]);                                // Draws the terrain and sets the camera points
    void GenerateHeightMap(int Iterations, double Height, double HDecay);    // Generates a new terrain
    void DeleteHeightMap();                                                    // Removes Heightmap from memory
private:
    GLvoid BuildLists(double Ratio, int DList, t3DModel Model);

    bool LoadTGA(TextureImage *texture, char *filename);

    int LoadGLTextures(GLuint *texture, char *filename);

    void DrawTriangleTree(Node *Next);                                        // Draws the triangle tree
    void ROAM(Node *T);                                                        // ROAM algorithm
    void ForceSplit(Node *T);

    void MergeNode(Node *T);

    void SplitTriangle(Node *T);

    void ForceMerge(Node *T, Node *End);

    bool inFrustrum(const Vector &TestPoint);

    double CalculateError(Vector &Left, Vector &Right);

    void DrawWater();

    void DrawSkyBox();

    void DrawTerrainObjects(float x, float y, float z, int pos, int type);

    void GenerateTerrainObjects(int Number, int types, int amounta, int amountb);

    TextureImage SkyBoxTexture;

    PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB;
    PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
    PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
    PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;

    BinTree TriangleTree;
    DiamondSquare HeightMap;

    // Terrain Parameters
    Vector CameraEye;
    Vector ViewFrustrum[4];
    GLuint ShadowTexture;
    GLuint SurfaceTexture;
    GLuint WaterTexture;

    bool isInString(char *string, const char *search);

    bool initMultitexture(void);

    bool multitextureSupported;
    bool useMultitexture;
    GLint maxTexelUnits;
    float WaterRoll;
    float SkyRoll;
    Vector *Forests;

    CLoad3DS g_Load3ds;
    t3DModel g_3DModel;
    t3DModel g_3DModel1;
    GLuint Object1;
    float O1Offset;
    GLuint Object2;
    float O2Offset;
};

#endif