#ifndef AVLTREE_H_
#define AVLTREE_H_

#include <stdlib.h>

#define MAX(a, b) (((a) >= (b))?(a):(b))
#define HEIGHT(x) ((x)?((x)->height):(0))

/*
   IMPORTANT!

   As we stick to pure C, we cannot use templates. We will just assume
   some type Item was previously defined.
 */
// -----------------------------------------------------------------------------

typedef struct node{
	void* elem;
	void* info;
	struct node *pt;
	struct node *lt;
	struct node *rt;
	struct node* next;
	struct node* prev;
	struct node* end;
	long height;
}TreeNode;

typedef struct TTree{
	TreeNode *root;
	void* (*createElement)(void*);
	void (*destroyElement)(void*);
	void* (*createInfo)(void*);
	void (*destroyInfo)(void*);
	int (*compare)(void*, void*);
	long size;
}TTree;

/*
 * Name function: createTree
 * Return: the memory address of the tree
 * Arguments: the name of the functions
 * Purpose: allocate memory for the tree and initialize the functions
 */
TTree* createTree(void* (*createElement)(void*), void (*destroyElement)(void*),
		void* (*createInfo)(void*), void (*destroyInfo)(void*),
		int compare(void*, void*)) {

	TTree* tree = (TTree*)malloc(sizeof(TTree));
	if(tree == NULL) {
		printf("Not enough memory\n");
		return NULL;
	}
	tree->root = NULL;
	tree->size = 0;
	tree->createElement = createElement;
	tree->destroyElement = destroyElement;
	tree->createInfo = createInfo;
	tree->destroyInfo = destroyInfo;
	tree->compare = compare;
	return tree;
}

/*
 * Name function: createTreeNode
 * Return: the memory address of a new node
 * Arguments: the tree, the value and the info
 * Purpose: allocate memory for the node
 */
TreeNode* createTreeNode(TTree *tree, void* value, void* info) {	
	TreeNode* newNode = (TreeNode*) malloc(sizeof(TreeNode));
	if(newNode == NULL) {
		printf("Not enough memory\n");
		return NULL;
	}

	newNode->pt = newNode->rt = newNode->lt = NULL;
	newNode->next = newNode->prev = NULL;
	newNode->end = newNode;
	newNode->height = 1;
	newNode->info = tree->createInfo(info);
	newNode->elem = tree->createElement(value);

	return newNode;
}

/*
 * Name function: destroyTreeNode
 * Return: void (it does not return a value)
 * Arguments: the tree and the node
 * Purpose: free the memory of a node
 */
void destroyTreeNode(TTree *tree, TreeNode* node) {
	tree->destroyInfo(node->info);
	tree->destroyElement(node->elem);
	free(node);
}

/*
 * Name function: isEmpty
 * Return: 1 if the tree is empty, 0 otherwise
 * Arguments: the tree
 * Purpose: check if there are nodes in a tree
 */
int isEmpty(TTree* tree) {
	if(tree->root == NULL) {
		return 1;
	}
	return 0;
}

/*
 * Name function: search
 * Return: the memory adress of a node with a specific elem
 * Arguments: the tree, the node after which I am searching
 * Purpose: find a node with a specific data
 */
TreeNode* search(TTree* tree, TreeNode* x, void* elem) {
	TreeNode *node;
	node = x;

	//start searching after the given node
	while(node != NULL) {
		if(tree->compare(node->elem, elem) == 0) {
			return node;
		} else {
			if(tree->compare(node->elem, elem) > 0) {
				node = node->lt;
			} else {
				node = node->rt;
			}
		}
	}
	return NULL;
}

/*
 * Name function: minimum
 * Return: the memory adress of a node with the minimum elem
 * Arguments: the tree, the node after which I am searching
 * Purpose: find a node with a specific data
 */
TreeNode* minimum(TTree*tree, TreeNode* x) {
	TreeNode *node = x, *p = NULL;

	//finding the node in the left subtree
	while(node != NULL) {
		p = node;
		node = node->lt;
	}
	return p;
}

/*
 * Name function: maximum
 * Return: the memory adress of a node with the maximum elem
 * Arguments: the tree, the node after which I am searching
 * Purpose: find a node with a specific data
 */
TreeNode* maximum(TTree* tree, TreeNode* x) {
	TreeNode *node = x, *p = NULL;

	//finding the node in the right subtree
	while(node != NULL) {
		p = node;
		node = node->rt;
	}
	return p;
}

/*
 * Name function: successor
 * Return: the memory adress of the successor of a node
 * Arguments: the tree, a certain node
 * Purpose: find the successor of a node
 */
TreeNode* successor(TTree* tree, TreeNode* x) {
	//if there is a right subtree, the successor is the minimum of the subtree
	if(x->rt != NULL) {
		return minimum(tree, x->rt);
	} else {
		TreeNode *t;
		t=x->pt;
		//searching for the successor between the parents of the given node
		while(t != NULL) {
			if(tree->compare(t->elem, x->elem) > 0) {
				return t;
			}
			t=t->pt;
		}
		return t;
	}
}

/*
 * Name function: predecessor
 * Return: the memory adress of the predecessor of a node
 * Arguments: the tree, a certain node
 * Purpose: find the predecessor of a node
 */
TreeNode* predecessor(TTree* tree, TreeNode* x) {
	//if there is a left subtree, the predecessor is the maximum of the subtree
	if(x->lt != NULL) {
		return maximum(tree, x->lt);
	} else {
		TreeNode *t;
		t=x->pt;
		//searching for the successor between the parents of the given node
		while(t != NULL){
			if(tree->compare(t->elem, x->elem) < 0) {
				return t;
			}
			t=t->pt;
		}
		return t;
	}
}

/*
 * Name function: max
 * Return: the maximum of two integers
 * Arguments: two integers
 * Purpose: get the maximum between two in integers
 */
long max(long a, long b) {
	return (a > b)? a : b;
}

/*
 * Name function: avlRotateLeft
 * Return: void (it does not return a value)
 * Arguments: the tree, the node I am doing the rotation for
 * Purpose: rotate the tree so that it is still balanced
 */
void avlRotateLeft(TTree* tree, TreeNode* x) {
	TreeNode* root = x;
	TreeNode* pivot = x->rt;
	TreeNode* pivot_left = pivot->lt;

	//update the parent of pivot
	pivot->pt = x->pt;

	//in case the left subtree exists, chance its parent
	if(pivot_left != NULL) {
		pivot_left->pt = x;
		x->rt = pivot_left;
	}
	//case if x was the root of the tree
	if(x == tree->root) {
		tree->root = pivot;
	} else {
		//change the links
		if(x->pt->lt == x) {
			x->pt->lt = pivot;
		} else {
			x->pt->rt = pivot;
		}
	}

	pivot->lt = x;
	pivot->lt->rt=pivot_left;
	x->pt = pivot;

	//change heights
	int pivot_hl = 0, pivot_hr = 0, x_hl = 1, x_hr = 1;
	if(x->rt != NULL) {
		x_hr = x->rt->height;
	}
	if(x->lt != NULL) {
		x_hl = x->lt->height;
	}
	x->height = max(x_hl, x_hr) + 1;

	if(pivot->lt != NULL)   {
		pivot_hl = pivot->lt->height;
	}
	if(pivot->rt != NULL) {
		pivot_hr = pivot->rt->height;
	}
	pivot->height = max(pivot_hl, pivot_hr) + 1;
}

/*
 * Name function: avlRotateRight
 * Return: void (it does not return a value)
 * Arguments: the tree, the node I am doing the rotation for
 * Purpose: rotate the tree so that it is still balanced
 */
void avlRotateRight(TTree* tree, TreeNode* y) {
	TreeNode* root = y;
	TreeNode* pivot = y->lt;
	TreeNode* pivot_right = pivot->rt;

	//update the parent of pivot
	pivot->pt = y->pt;

	//in case the left subtree exists, chance its parent
	if(pivot_right != NULL) {
		pivot_right->pt = y;
		y->lt = pivot_right;
	}
	//case if x was the root of the tree
	if(y == tree->root) {
		tree->root = pivot;
	} else {
		//change the links
		if(y->pt->lt == y) {
			y->pt->lt = pivot;
		} else {
			y->pt->rt = pivot;
		}
	}

	pivot->rt = y;
	pivot->rt->lt=pivot_right;
	y->pt = pivot;

	//change heights
	int pivot_hl = 0, pivot_hr = 0, y_hl = 0, y_hr = 0;
	if(y->rt != NULL) {
		y_hr = y->rt->height;
	}
	if(y->lt != NULL) {
		y_hl = y->lt->height;
	}
	y->height = max(y_hl, y_hr) + 1;

	if(pivot->lt != NULL)   {
		pivot_hl = pivot->lt->height;
	}
	if(pivot->rt != NULL) {
		pivot_hr = pivot->rt->height;
	}
	pivot->height = max(pivot_hl, pivot_hr) + 1;
}

/*
 * Name function: avlGetBalance
 * Return: the height difference between the left and right subtree
 * Arguments: the tree, the node I check the balance for
 * Purpose: keep the tree balanced
 */
int avlGetBalance(TTree* tree, TreeNode *x) {
	int l_height = 0, r_height = 0;
	if (x == NULL) {
		return 0;
	}
	if(x->lt != NULL) {
		l_height = x->lt->height;
	}
	if(x->rt != NULL) {
		r_height = x->rt->height;
	}
	return l_height - r_height;
}

/*
 * Name function: avlFixUp
 * Return: void (it does not return a value)
 * Arguments: the tree, the unbalanced node and its balance
 * Purpose: rotate the tree so that it is balanced
 */
void avlFixUp(TTree* tree, TreeNode* y, int balance) {

	if(balance > 1 && tree->compare(y->lt->lt->elem, y->lt->elem) < 0) {
		avlRotateRight(tree, y);
		return;
	}
	if(y->rt->rt != NULL) {
		if(balance < -1 && tree->compare(y->rt->rt->elem, y->rt->elem) > 0) {
			avlRotateLeft(tree, y);
			return;
		}
	}	
	if(y->rt->lt != NULL) {
		if(balance < -1 && tree->compare(y->rt->lt->elem, y->rt->elem) < 0) {
			avlRotateRight(tree, y->rt);
			avlRotateLeft(tree, y);
			return;
		}
	}
	if(y->lt->rt != NULL) {
		if(balance > 1 && tree->compare(y->lt->rt->elem, y->lt->elem) > 0) {
			avlRotateLeft(tree, y->lt);
			avlRotateRight(tree, y);
			return;
		}
	}
}

/*
 * Name function: refreshHeights
 * Return: void (it does not return a value)
 * Arguments: the tree, the node above which I am updating the heights
 * Purpose: update the height of nodes after an insertion
 */
void refreshHeights(TTree *tree, TreeNode *copy) {
	while(copy != NULL)   {
		if(copy->lt == NULL && copy->rt == NULL) {
			copy->height = 1;
			copy = copy->pt;
		} else {
			//if there is no left subtree the height is given by the right subtree
			if(copy->lt == NULL) {
				copy->height = copy->rt->height + 1;
				copy = copy->pt;
			} else {
				//if there is no right subtree the height is given by the left subtree
				if(copy->rt == NULL) {
					copy->height = copy->lt->height + 1;
					copy = copy->pt;
				} else {
					copy->height = max(copy->lt->height, copy->rt->height) + 1;
					copy = copy->pt;
				}
			}
		}
	}
}

/*
 * Name function: insert
 * Return: void (it does not return a value)
 * Arguments: the tree, the elem and the value to be inserted
 * Purpose: insert data into the tree and keep it balanced by doing rotations
 */
void insert(TTree* tree, void* elem, void* info) {
	if(tree == NULL) {
		return;
	}	
	TreeNode *new_node = createTreeNode(tree, elem, info);

	//if there is no element in the tree
	if(isEmpty(tree) != 0) {
		tree->root = new_node;
		tree->size = 1;
	} else {
		TreeNode *copy, *prev;
		copy = tree->root;
		//find the right place to insert the new node
		while(copy != NULL) {
			prev = copy;
			if(tree->compare(copy->elem, elem) > 0) {
				copy = copy->lt;
			} else {  
				if(tree->compare(copy->elem, elem) < 0) {
					copy = copy->rt;
				} else {
					//if the node already exists update links
					new_node->next = copy->end->next;
					new_node->prev = copy->end;
					new_node->prev->next = new_node;
					if(new_node->next != NULL) {
						new_node->next->prev = new_node;
					}
					copy->end = new_node;
					return;
				}
			}
		} 

		//set the parent of the new node
		if(tree->compare(prev->elem, elem)) {
			//check if it should be added in the left or right position
			if(tree->compare(prev->elem, elem) > 0) {
				new_node->pt = prev;
				prev->lt = new_node;
				new_node->prev = prev;
				new_node->prev->next = new_node;
			} else {
				new_node->pt = prev;
				prev->rt = new_node;
				new_node->prev = prev->end;
				new_node->prev->end->next = new_node;
			}
			tree->size++;
			copy = prev;
			//change the height of each node
			refreshHeights(tree, copy);
		}
		//in case the tree got unbalanced, make rotations
		int balance;
		copy = prev;
		balance = avlGetBalance(tree, copy);
		while(copy->pt != NULL && abs(balance) <= 1) {
			copy = copy->pt;
			balance = avlGetBalance(tree, copy);
		}
		if(balance > 1 && tree->compare(elem, copy->lt->elem) < 0) {
			avlRotateRight(tree, copy);
			return;
		}
		if(balance < -1 && tree->compare(elem, copy->rt->elem) > 0) {
			avlRotateLeft(tree, copy);
			return;
		}
		if(balance < -1 && tree->compare(elem, copy->rt->elem) < 0) {
			avlRotateRight(tree, copy->rt);
			avlRotateLeft(tree, copy);
			return;
		}
		if(balance > 1 && tree->compare(elem, copy->lt->elem) > 0) {
			avlRotateLeft(tree, copy->lt);
			avlRotateRight(tree, copy);
			return;
		}
	}
}

/*
 * Name function: change
 * Return: void (it does not return a value)
 * Arguments: the tree, the node and its parent
 * Purpose: erase the link between the node and the parent
 */
void change(TTree* tree, TreeNode* node, TreeNode* parent) {
	//if the leaf is a left child erase link and update heights
	if(node == parent->lt) {
		parent->lt == NULL;
		if(parent->rt != NULL) {
			parent->height = parent->rt->height + 1;
		} else {
			parent->height = 1;
		}
	} else {
		//if the leaf is a right child erase link and update heights
		parent->rt == NULL;
		if(parent->lt != NULL) {
			parent->height = parent->lt->height + 1;
		} else {
			parent->height = 1;
		}
	}
}


/*
 * Name function: deleteLeaf
 * Return: void (it does not return a value)
 * Arguments: the tree, the node to be deleted
 * Purpose: free the memory of a leaf and keep the tree balanced
 */
void deleteLeaf(TTree* tree, TreeNode* node) {
	TreeNode *parent = node->pt;
	//if it is the only node in the tree
	if(parent == NULL) {
		tree->root = NULL;
		tree->size = 0;
	} else {
		//update the links of the list
		if(node->end->next != NULL) {
			node->end->next->prev = node->end->prev;
		}
		if(node->end->prev != NULL) {
			node->end->prev->next = node->end->next;
		}
		node->end = node->end->prev;
		tree->size--;

		change(tree, node, parent);
		if(node->pt->lt == node) {
			node->pt->lt = NULL;
		}
		if(node->pt->rt == node) {
			node->pt->rt = NULL;
		}

		//keeping the tree balanced
		TreeNode *copy = parent;
		refreshHeights(tree, copy->pt);
		int balance;
		balance = avlGetBalance(tree, parent);
		copy = parent;
		while(copy->pt != NULL && abs(balance) <= 1) {
			copy = copy->pt;
			balance = avlGetBalance(tree, copy);
		}
		if(abs(balance) > 1) {
			avlFixUp(tree, copy, balance);
		}
	}
	destroyTreeNode(tree, node);
}

/*
 * Name function: deleteSplitNode
 * Return: void (it does not return a value)
 * Arguments: the tree, the node to be deleted
 * Purpose: free the memory of a node that has two children and update the tree
 */
void deleteSplitNode(TTree* tree, TreeNode* node) {
	TreeNode *copy;
	copy = successor(tree, node);
	TreeNode *parent = copy->pt;
	//update the links of the nodes
	copy->lt = node->lt;
	copy->rt = node->rt;
	node->rt->pt = copy;
	node->lt->pt = copy;
	if(node->prev != NULL) {
		node->prev->next = copy;
		copy->prev = node->prev->end;
	}

	//erase the link between the node and the parent
	if(copy->pt->lt == copy) {
		copy->pt->lt = NULL;
	} else {
		copy->pt->rt = NULL;
	}

	//update the parent
	copy->pt = node->pt;
	if(node == tree->root) {
		tree->root = copy;
	}

	if(node->pt == NULL) {
		copy->pt = NULL;
	} else {
		if(node->pt->lt == node) {
			node->pt->lt = copy;
		} else {
			node->pt->rt = copy; 
		}
	}
	//keeping the tree balanced
	int balance;
	balance = avlGetBalance(tree, parent);
	if(abs(balance) > 1) {
		avlFixUp(tree, parent, balance);
	}
	destroyTreeNode(tree, node);
	tree->size--;
}

/*
 * Name function: deleteOneChildNode
 * Return: void (it does not return a value)
 * Arguments: the tree, the node to be deleted
 * Purpose: free the memory of a node that has only one child
 */
void deleteOneChildNode(TTree* tree, TreeNode* node) {
	//update the links
	if(node->rt != NULL) {
		node->rt->pt = node->pt;
		if(node->pt == NULL) {
			tree->root = node->rt;
		} else {
			if(node->pt->lt == node) {
				node->pt->lt = node->rt;
			} else {
				node->pt->rt = node->rt; 
			}
		}
		refreshHeights(tree, node->pt);
	} else {
		node->lt->pt = node->pt;
		if(node->pt == NULL) {
			tree->root = node->lt;
		} else {
			//change the links
			if(node->pt->lt == node) {
				node->pt->lt = node->lt;
			} else {
				node->pt->rt = node->lt; 
			}
		}
		refreshHeights(tree, node->pt);
	}
	tree->size--;
	destroyTreeNode(tree, node);
}

/*
 * Name function: delete
 * Return: void (it does not return a value)
 * Arguments: the tree, the elem to be deleted
 * Purpose: erase a certain elem from the tree and keep it balaned
 */
void delete(TTree* tree, void* elem) {
	//search if the given elem exists in the tree
	TreeNode *node = search(tree, tree->root, elem);
	if(node == NULL)  {
		return;
	}

	//if the node has duplicates, update the links of the lists
	if(node != node->end) {
		if(node->end->next != NULL) {
			node->end->next->prev = node->end->prev;
		}
		if(node->end->prev != NULL) {
			node->end->prev->next = node->end->next;
		}

		TreeNode *del = node->end;
		node->end = node->end->prev;
		destroyTreeNode(tree, del);
		tree->size--;
		return;
	}

	// if the node is a leaf
	if(node->lt == NULL && node->rt == NULL) {
		deleteLeaf(tree, node);
	} else {
		//check for split nodes
		if(node->lt != NULL && node->rt != NULL) {
			deleteSplitNode(tree, node);
		} else {
			deleteOneChildNode(tree, node);
		}
	}
}

/*
 * Name function: destroyTree
 * Return: void (it does not return a value)
 * Arguments: the tree to be deleted
 * Purpose: free the memory of a tree
 */
void destroyTree(TTree* tree) {
	TreeNode *node;
	node = minimum(tree, tree->root);
	while(node->next != NULL) {
		node = node->next;
		destroyTreeNode(tree, node->prev); 
	}
	destroyTreeNode(tree, node);
	free(tree);
}


#endif /* AVLTREE_H_ */
