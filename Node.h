#ifndef NODE_H
#define NODE_H

#include "Triangle.h"
#include <iostream>

using namespace std;

class Node {
public:
    Node() {
        LeftChild = NULL;
        RightChild = NULL;
        Parent = NULL;
        LeftNeighbour = NULL;
        RightNeighbour = NULL;
        BaseNeighbour = NULL;
        InFrustrum = true;
        InFrustrumPrev = true;
    };

    Node(Triangle t) {
        LeftChild = NULL;
        RightChild = NULL;
        Parent = NULL;
        LeftNeighbour = NULL;
        RightNeighbour = NULL;
        BaseNeighbour = NULL;
        data = t;
        InFrustrum = true;
        InFrustrumPrev = true;
    };

    // Triangle Pointers
    Node *LeftNeighbour;
    Node *RightNeighbour;
    Node *BaseNeighbour;
    double Distance;
    double Priority;
    int Depth;
    bool InFrustrum;
    bool InFrustrumPrev;

    // Tree Pointers
    Node *LeftChild;
    Node *RightChild;
    Node *Parent;
private:
    Triangle data;

    friend class BinTree;
};

#endif