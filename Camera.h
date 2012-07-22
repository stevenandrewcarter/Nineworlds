#ifndef _CAMERA_H
#define _CAMERA_H
#include "Vector.h"
#include <windows.h>		// Header File For Windows
#include <stdio.h>			// Header File for Standard Input / Output
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#define SCREEN_WIDTH 800								// We want our screen width 800 pixels
#define SCREEN_HEIGHT 600								// We want our screen height 600 pixels
#define SCREEN_DEPTH 32									// We want 16 bits per pixel

// This is our camera class
class CCamera 
{
public:	
	CCamera();								// Our camera constructor
	// These are are data access functions for our camera's private data
	Vector Position() {	return m_vPosition;		}
	Vector View()		{	return m_vView;			}
	Vector UpVector() {	return m_vUpVector;		}
	Vector Strafe()	{	return m_vStrafe;		}	
	// This changes the position, view, and up vector of the camera.
	// This is primarily used for initialization
	void PositionCamera(float positionX, float positionY, float positionZ,
			 		    float viewX,     float viewY,     float viewZ,
						float upVectorX, float upVectorY, float upVectorZ);	
	void RotateView(float angle, float X, float Y, float Z);	// This rotates the camera's view around the position depending on the values passed in.
	void SetViewByMouse(); 					// This moves the camera's view by the mouse movements (First person view)
	void RotateAroundPoint(Vector vCenter, float X, float Y, float Z);	// This rotates the camera around a point 
	void StrafeCamera(float speed);			// This strafes the camera left or right depending on the speed (+/-) 
	void MoveCamera(float speed);			// This will move the camera forward or backward depending on the speed	
	void CheckForMovement();				// This checks for keyboard movement
	void Update(HWND hWnd);							// This updates the camera's view and other data (Should be called each frame)
	void Look();							// This uses gluLookAt() to tell OpenGL where to look

	Vector PlaneNormals[6];
	Vector CameraEye;

private:	
	Vector m_vPosition;					// The camera's position	
	Vector m_vView;						// The camera's view
	Vector m_vUpVector;					// The camera's up vector		
	Vector m_vStrafe;						// The camera's strafe vector
	void CalculatePlaneNormals();			// Calculates the View Frustrum Normals
};

#endif