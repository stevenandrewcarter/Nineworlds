#include "LinMaths.h"

double lm_dst(Vector p1, Vector p2) {
    double ReturnAnswer;
    double x = p2.x - p1.x;
    double y = p2.y - p1.y;
    double z = p2.z - p1.z;
    ReturnAnswer = sqrt((x * x) + (y * y) + (z * z));
    return ReturnAnswer;
}

double lm_radians(double Angle) {
    const float PI = 3.141592653589793;
    double Rad = ((2 * PI) * Angle) / 360;
    return Rad;
}

double lm_degreesin(double angle) {
    const float PI = 3.141592653589793;
    double Theta = ((2 * PI) * angle) / 360;
    double answer = sin(Theta);
    return answer;
}

float lm_degree(double radian) {
    const float PI = 3.141592653589793;
    double Theta = (360 * radian) / (2 * PI);
    return Theta;
}

double lm_degreecos(double angle) {
    const float PI = 3.141592653589793;
    double Theta = ((2 * PI) * angle) / 360;
    double answer = cos(Theta);
    return answer;
}

Vector lm_parametric(Vector p0, Vector p1, double t) {
    double d1 = p1.x - p0.x;
    double d2 = p1.y - p0.y;
    double d3 = p1.z - p0.z;
    double X = p0.x + (d1 * t);
    double Y = p0.y + (d2 * t);
    double Z = p0.z + (d3 * t);
    Vector returnPoint = Vector(X, Y, Z);
    return returnPoint;
}

Vector lm_vector(Vector p0, Vector p1) {
    Vector ans = Vector(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z);
    return ans;
}

double lm_dotproduct(Vector u, Vector v) {
    double ans;
    ans = (u.x * v.x) + (u.y * v.y) + (u.z * v.z);
    return ans;
}

HalfSpace ClassifyPoint(const Vector &planePoint, const Vector &planeNormal, const Vector &point) {
    double D, s;
    D = -((planeNormal.x * planePoint.x) + (planeNormal.y * planePoint.y) + (planeNormal.z * planePoint.z));
    s = planeNormal.x * point.x + planeNormal.y * point.y + planeNormal.z * point.z + D;
    if (s < 0) return NEGATIVE;
    if (s > 0) return POSITIVE;
    return ON_PLANE;
}

Vector Perp2D(Vector v) {
    return Vector(-v.y, v.x);
}

double PointLeftOfSegment(Vector p, Vector a, Vector b) {
    return lm_dotproduct(p - a, Perp2D(b - a));// > 0.0f;
}

int PointInTriangle(Vector p, Vector a, Vector b, Vector c) {
    double a1 = PointLeftOfSegment(p, a, b);
    double b1 = PointLeftOfSegment(p, b, c);
    double c1 = PointLeftOfSegment(p, c, a);
    if ((a1 > 0 && b1 > 0 && c1 > 0) || (a1 < 0 && b1 < 0 && c1 < 0)) {
        return true;
    } else {
        return false;
    }
    return 1;
}

