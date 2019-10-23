#ifndef LINMATHS_H
#define LINMATHS_H

#include "Vector.h"
#include <cmath>

/*
 - Linmaths.h
 - Contains equations for calculating linear equations
 - + Distance between two points in 3d dimension
 - + Spherical co-ordinates conversion
 - + Parametric line points
*/

enum HalfSpace {
    NEGATIVE = -1,
    ON_PLANE = 0,
    POSITIVE = 1,
};

double
lm_dst(Vector p1, Vector p2);                                        // Calculates the distance between two points in R3
Vector lm_parametric(Vector p0, Vector p1,
                     double t);                    // Calculates the XYZ value using the line parametric from p1 to p2
double lm_degreesin(double angle);

double lm_degreecos(double angle);

double lm_norm(Vector p0);

Vector lm_vector(Vector p0, Vector p1);

double lm_dotproduct(Vector u, Vector v);

double lm_radians(double Angle);

HalfSpace ClassifyPoint(const Vector &planePoint, const Vector &planeNormal, const Vector &point);

bool lm_trianglePoint(Vector v0, Vector v1, Vector v2, Vector p);

float lm_degree(double radian);

Vector Perp2D(Vector v);

double PointLeftOfSegment(Vector p, Vector a, Vector b);

int PointInTriangle(Vector p, Vector a, Vector b, Vector c);

#endif