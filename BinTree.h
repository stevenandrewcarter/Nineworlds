#ifndef BINTREE_H
#define BINTREE_H

#include "Node.h"

// This bintree is built to only use Triangle members

class BinTree
{
public:
	BinTree();												// Constructor
	// Mutators	
	void ExpandNode(Node* Current);							// Expands current node
	void InsertAtNode(Node* Current, Triangle t, double p); // Places data at node
	void RemoveNode();										// Removes Current Node	
	// Accessors
	bool isRoot(Node* Current) const;						// Returns true if current node is root
	bool isLeftChild(Node* Current) const;					// Returns true if current node is a left child
	bool isRightChild(Node* Current) const;					// Returns true if current node is a right child
	bool isInternal(Node* Current) const;					// Returns true if current node is internal
	bool isExternal(Node* Current) const;					// Returns true if current node is external
	Node* Parent(Node* Current) const;						// Moves the current position to the parent node
	Node* LeftChild(Node* Current) const;					// Returns the LeftChild
	Node* RightChild(Node* Current) const;					// Returns the RightChild
	Node* Sibling(Node* Current) const;						// Returns the Sibling
	Triangle getCurrentNode(Node* Current) const;			// Returns the data stored in the node
	int getSize() const;									// Returns tree size
	int getDepth(Node* Current) const;						// Returns node depth
	Node* root;												// Root Node
private:		
	int Depth;												// The current Depth of the tree
	int Size;												// The current size of the tree
};

#endif

