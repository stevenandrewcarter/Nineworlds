#include "BinTree.h"

BinTree::BinTree()
{
	root = new Node();
	root->Priority = 100;
	root->Depth = 0;
	root->Distance = 0;
	Size = 0;
}

// Expand Node needs to add two new nodes to the vector
void BinTree::ExpandNode(Node* Current)
{
	Node* Left = new Node();			// Make a new LeftChild
	Node* Right = new Node();			// Make a new RightChild
	Current->LeftChild = Left;			// Set the currentNodes leftchild to leftchild
	Current->RightChild = Right;		// Set the currentNodes rightChild to rightchild
	Left->Parent = Current;				// Set Leftchilds parent
	Right->Parent = Current;			// Set rightChilds parent
	Size+=2;
	Current->LeftChild->Depth = Current->Depth + 1;
	Current->RightChild->Depth = Current->Depth + 1;
}

Triangle BinTree::getCurrentNode(Node* Current) const
{
	Triangle t = Current->data;
	return t;
}

void BinTree::InsertAtNode(Node* Current, Triangle t, double p)
{
	Current->data = t;
	Current->Priority = p;
}

bool BinTree::isExternal(Node* Current) const
{
	if(Current->LeftChild == NULL && Current->RightChild == NULL)
	{
		return true;
	}
	else
	{
		return false;
	}	
}

bool BinTree::isInternal(Node* Current) const
{
	if(Current->LeftChild != NULL || Current->RightChild != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool BinTree::isLeftChild(Node* Current) const
{
	Node* parent = Current->Parent;
	if(parent->LeftChild == Current)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool BinTree::isRightChild(Node* Current) const
{
	Node* parent = Current->Parent;
	if(parent->RightChild == Current)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool BinTree::isRoot(Node* Current) const
{
	if(Current == root)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Node* BinTree::Parent(Node* Current) const
{
	if(!isRoot(Current))
	{
		Current = Current->Parent;
	}
	return Current;
}

Node* BinTree::LeftChild(Node* Current) const
{
	return Current->LeftChild;
}

Node* BinTree::RightChild(Node* Current) const
{
	return Current->RightChild;
}

Node* BinTree::Sibling(Node* Current) const
{	
	if(Current->Parent->LeftChild == Current)
	{
		return Current->Parent->RightChild;
	}
	else
	{
		return Current->Parent->LeftChild;
	}
}

int BinTree::getSize() const
{
	return Size;
}
