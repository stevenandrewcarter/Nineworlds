#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
#include <gl\glaux.h>									// Header File For The GLaux Library
#include <math.h>

#include "Camera.h"
#include "LinMaths.h"


// We increased the speed a bit from the Camera Strafing Tutorial
// This is how fast our camera moves
#define kSpeed    50.0f

// Our global float that stores the elapsed time between the current and last frame
float g_FrameInterval = 0.0f;

void CalculateFrameRate(HWND g_hWnd) {
    static float framesPerSecond = 0.0f;        // This will store our fps
    static float lastTime = 0.0f;        // This will hold the time from the last frame
    static char strFrameRate[50] = {0};            // We will store the string here for the window title

    static float frameTime = 0.0f;                // This stores the last frame's time

    // Get the current time in seconds
    float currentTime = timeGetTime() * 0.001f;

    // Here we store the elapsed time between the current and last frame,
    // then keep the current frame in our static variable for the next frame.
    g_FrameInterval = currentTime - frameTime;
    frameTime = currentTime;

    // Increase the frame counter
    ++framesPerSecond;

    // Now we want to subtract the current time by the last time that was stored
    // to see if the time elapsed has been over a second, which means we found our FPS.
    if (currentTime - lastTime > 1.0f) {
        // Here we set the lastTime to the currentTime
        lastTime = currentTime;

        // Copy the frames per second into a string to display in the window title bar
        sprintf(strFrameRate, "Current Frames Per Second: %d", int(framesPerSecond));

        // Set the window title bar to our string
        SetWindowText(g_hWnd, strFrameRate);

        // Reset the frames per second
        framesPerSecond = 0;
    }
}

CCamera::CCamera() {
    Vector vZero = Vector(0.0, 0.0, 0.0);        // Init a vector to 0 0 0 for our position
    Vector vView = Vector(0.0, 1.0, 0.5);        // Init a starting view vector (looking up and out the screen)
    Vector vUp = Vector(0.0, 1.0, 0.0);        // Init a standard up vector (Rarely ever changes)

    m_vPosition = vZero;                    // Init the position to zero
    m_vView = vView;                    // Init the view to a std starting view
    m_vUpVector = vUp;                        // Init the UpVector
}

void CCamera::PositionCamera(float positionX, float positionY, float positionZ,
                             float viewX, float viewY, float viewZ,
                             float upVectorX, float upVectorY, float upVectorZ) {
    Vector vPosition = Vector(positionX, positionY, positionZ);
    Vector vView = Vector(viewX, viewY, viewZ);
    Vector vUpVector = Vector(upVectorX, upVectorY, upVectorZ);

    // The code above just makes it cleaner to set the variables.
    // Otherwise we would have to set each variable x y and z.

    m_vPosition = vPosition;                    // Assign the position
    m_vView = vView;                        // Assign the view
    m_vUpVector = vUpVector;                    // Assign the up vector
}

void CCamera::SetViewByMouse() {
    POINT mousePos;                                    // This is a window structure that holds an X and Y
    int middleX = SCREEN_WIDTH >> 1;                // This is a binary shift to get half the width
    int middleY = SCREEN_HEIGHT >> 1;                // This is a binary shift to get half the height
    float angleY = 0.0f;                            // This is the direction for looking up or down
    float angleZ = 0.0f;                            // This will be the value we need to rotate around the Y axis (Left and Right)
    static float currentRotX = 0.0f;

    // Get the mouse's current X,Y position
    GetCursorPos(&mousePos);

    // If our cursor is still in the middle, we never moved... so don't update the screen
    if ((mousePos.x == middleX) && (mousePos.y == middleY)) return;

    // Set the mouse position to the middle of our window
    SetCursorPos(middleX, middleY);

    // Get the direction the mouse moved in, but bring the number down to a reasonable amount
    angleY = (float) ((middleX - mousePos.x)) / 500.0f;
    angleZ = (float) ((middleY - mousePos.y)) / 500.0f;

    // Here we keep track of the current rotation (for up and down) so that
    // we can restrict the camera from doing a full 360 loop.
    currentRotX -= angleZ;

    // If the current rotation (in radians) is greater than 1.0, we want to cap it.
    /*if(currentRotX > 1.0f)
        currentRotX = 1.0f;
    // Check if the rotation is below -1.0, if so we want to make sure it doesn't continue
    else if(currentRotX < -1.0f)
        currentRotX = -1.0f;
    // Otherwise, we can rotate the view around our position
    else*/
    {
        // To find the axis we need to rotate around for up and down
        // movements, we need to get a perpendicular vector from the
        // camera's view vector and up vector.  This will be the axis.
        Vector vAxis = (m_vView - m_vPosition) * m_vUpVector;//Cross(m_vView - m_vPosition, m_vUpVector);
        vAxis = vAxis.Normalize();//vAxis//Normalize(vAxis);

        // Rotate around our perpendicular axis and along the y-axis
        RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
        RotateView(angleY, 0, 1, 0);
    }
}

void CCamera::RotateView(float angle, float x, float y, float z) {
    Vector vNewView;

    // Get the view vector (The direction we are facing)
    Vector vView = m_vView - m_vPosition;

    // Calculate the sine and cosine of the angle once
    float cosTheta = (float) cos(angle);
    float sinTheta = (float) sin(angle);

    // Find the new x position for the new rotated point
    vNewView.x = (cosTheta + (1 - cosTheta) * x * x) * vView.x;
    vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta) * vView.y;
    vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta) * vView.z;

    // Find the new y position for the new rotated point
    vNewView.y = ((1 - cosTheta) * x * y + z * sinTheta) * vView.x;
    vNewView.y += (cosTheta + (1 - cosTheta) * y * y) * vView.y;
    vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta) * vView.z;

    // Find the new z position for the new rotated point
    vNewView.z = ((1 - cosTheta) * x * z - y * sinTheta) * vView.x;
    vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta) * vView.y;
    vNewView.z += (cosTheta + (1 - cosTheta) * z * z) * vView.z;

    // Now we just add the newly rotated vector to our position to set
    // our new rotated view of our camera.
    m_vView = m_vPosition + vNewView;
}

void CCamera::StrafeCamera(float speed) {
    // Add the strafe vector to our position
    m_vPosition.x += m_vStrafe.x * speed;
    m_vPosition.z += m_vStrafe.z * speed;

    // Add the strafe vector to our view
    m_vView.x += m_vStrafe.x * speed;
    m_vView.z += m_vStrafe.z * speed;
}

void CCamera::MoveCamera(float speed) {
    // Get the current view vector (the direction we are looking)
    Vector vVector = m_vView - m_vPosition;
    vVector = vVector.Normalize();//Normalize(vVector);

    m_vPosition.x += vVector.x * speed;        // Add our acceleration to our position's X
    m_vPosition.y += vVector.y * speed;        // Add our acceleration to our position's Y
    m_vPosition.z += vVector.z * speed;        // Add our acceleration to our position's Z
    m_vView.x += vVector.x * speed;            // Add our acceleration to our view's X
    m_vView.y += vVector.y * speed;            // Add our acceleration to our view's Y
    m_vView.z += vVector.z * speed;            // Add our acceleration to our view's Z
}

void CCamera::CheckForMovement() {
    // Once we have the frame interval, we find the current speed
    float speed = kSpeed * g_FrameInterval;

    // Check if we hit the Up arrow or the 'w' key
    if (GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {

        // Move our camera forward by a positive SPEED
        MoveCamera(speed);
    }

    // Check if we hit the Down arrow or the 's' key
    if (GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {

        // Move our camera backward by a negative SPEED
        MoveCamera(-speed);
    }

    // Check if we hit the Left arrow or the 'a' key
    if (GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {

        // Strafe the camera left
        StrafeCamera(-speed);
    }

    // Check if we hit the Right arrow or the 'd' key
    if (GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {

        // Strafe the camera right
        StrafeCamera(speed);
    }
}

void CCamera::Update(HWND hWnd) {
    // Initialize a variable for the cross product result
    Vector vCross = (m_vView - m_vPosition) * m_vUpVector;//Cross(m_vView - m_vPosition, m_vUpVector);

    // Normalize the strafe vector
    m_vStrafe = vCross.Normalize();//Normalize(vCross);

    // Move the camera's view by the mouse
    SetViewByMouse();

    // This checks to see if the keyboard was pressed
    CheckForMovement();

    // Calculate our frame rate and set our frame interval for time-based movement
    CalculateFrameRate(hWnd);

    CalculatePlaneNormals();
    CameraEye.x = m_vPosition.x;
    CameraEye.y = m_vPosition.y;
    CameraEye.z = m_vPosition.z;
}

void CCamera::Look() {
    // Give openGL our camera position, then camera view, then camera up vector
    gluLookAt(m_vPosition.x, m_vPosition.y, m_vPosition.z,
              m_vView.x, m_vView.y, m_vView.z,
              m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);

}

void CCamera::CalculatePlaneNormals() {
    Vector vp1 = Vector(m_vPosition.x, m_vPosition.y, m_vPosition.z);
    Vector vp = Vector();
    Vector vd1 = Vector(m_vView.x, m_vView.y, m_vView.z);
    Vector vd = lm_vector(vp1, vd1);
    vd = vd.Normalize();
    Vector vAxis = (m_vView - m_vPosition) * m_vUpVector;//Cross(m_vView - m_vPosition, m_vUpVector);
    vAxis = vAxis.Normalize();//Normalize(vAxis);
    Vector vu = Vector(vAxis.x, vAxis.y, vAxis.z);
    vu = vu.Normalize();//lm_unitvector(vu);
    Vector right = vd * vu;//lm_crossproduct(vd, vu);

    double WIDTH = 800;
    double HEIGHT = 600;
    float thetah = static_cast<float>(lm_radians(75));
    float thetav = static_cast<float>(2 * atan(HEIGHT * tan(thetah / 2) / WIDTH));

    Vector p0 = vp;
    p0.x += vd.x - (right.x * tan(thetah / 2)) - (vu.x * tan(thetav / 2));
    p0.y += vd.y - (right.y * tan(thetah / 2)) - (vu.y * tan(thetav / 2));
    p0.z += vd.z - (right.z * tan(thetah / 2)) - (vu.z * tan(thetav / 2));
    Vector p1 = vp;
    p1.x += vd.x + right.x * tan(thetah / 2) - vu.x * tan(thetav / 2);
    p1.y += vd.y + right.y * tan(thetah / 2) - vu.y * tan(thetav / 2);
    p1.z += vd.z + right.z * tan(thetah / 2) - vu.z * tan(thetav / 2);
    Vector p2 = vp;
    p2.x += vd.x + right.x * tan(thetah / 2) + vu.x * tan(thetav / 2);
    p2.y += vd.y + right.y * tan(thetah / 2) + vu.y * tan(thetav / 2);
    p2.z += vd.z + right.z * tan(thetah / 2) + vu.z * tan(thetav / 2);
    Vector p3 = vp;
    p3.x += vd.x - (right.x * tan(thetah / 2)) + (vu.x * tan(thetav / 2));
    p3.y += vd.y - (right.y * tan(thetah / 2)) + (vu.y * tan(thetav / 2));
    p3.z += vd.z - (right.z * tan(thetah / 2)) + (vu.z * tan(thetav / 2));
    // Left Normal
    PlaneNormals[0] = p0 * p3;//lm_crossproduct(p0, p3);
    // Right Normal
    PlaneNormals[1] = p1 * p2;//lm_crossproduct(p1, p2);
    // Up Normal
    PlaneNormals[2] = p3 * p2;//lm_crossproduct(p3, p2);
    // Down Normal
    PlaneNormals[3] = p0 * p1;//lm_crossproduct(p0, p1);
    // Near Normal
    PlaneNormals[4] = vp1;

}