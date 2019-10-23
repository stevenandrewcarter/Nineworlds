#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Vector.h"
#include <vector>

using namespace std;

class Triangle {
public:
    //Constructor
    Triangle(Vector nApex, Vector nLeft, Vector nRight) {
        Apex = nApex;
        Left = nLeft;
        Right = nRight;
        //Tree. = NULL;
    };

    Triangle() {
        Apex = Vector();
        Left = Vector();
        Right = Vector();
        //Tree = NULL;
    };
    Vector Apex;
    Vector Left;
    Vector Right;
    vector<Vector> Tree;
};

#endif