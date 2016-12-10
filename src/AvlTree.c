/**
 * @file AvlTree.c
 *
 * Contains implementations of the functions defined in AvlTree.h, as well as some static helper functions.
 */

#include <stdlib.h>
#include <string.h>
#include "AvlTree.h"

/**
 * This function is used as comparrison function, is `avlInit()` is passed `NULL` for the argument `compare`.
 *
 * @return 0
 */
int dummyCompare(const void *a, const void *b)
{
	// if I wrote `return 0;` the compiler would complain, because of unused parameters.
	return a - a + b - b;
}

/**
 * Allocates and initializes a new `AvlNode` with a given value and all pointers set to `NULL`.
 *
 * @param value The value of the new node.
 * @return Pointer to the newly allocated node, or `NULL` on failure.
 */
static struct AvlNode *nodeCreate(void *value)
{
	struct AvlNode *node;

	node = malloc(sizeof(struct AvlNode));
	if (node == NULL)
		return NULL;

	memset(node, 0, sizeof(struct AvlNode));
	node->value = value;

	return node;
}

/**
 * Performs a rotation around an unbalanced node, to rebalance the tree.
 *
 * *NOTE:* this function assumes, that `node` is unbalanced. Calling this function on a balanced node is undefined
 * behaviour.
 *
 * @param node Points to the unbalanced node around which the rotation is performed.
 * @param tree Points to the tree `node` is in. This is only needed in case `node` is the root of the tree (because the
 * root is reassigned, then).
 * @param right Specifies the direction of rotation (0 = left (counterclockwise), non-zero = right (clockwise)).
 */
static void nodeRotate(struct AvlNode *node, struct AvlTree *tree, int right)
{
	if (node == NULL)
		return;
	if (tree == NULL)
		return;

	// pointer to the parents child field that points to 'node'
	struct AvlNode **parents_child;
	// pointer to the child of 'node' that gets rotated up
	struct AvlNode *child;

	// initialize variables
	if (node->parent == NULL)
	{
		parents_child = &tree->root;
	}
	else if (node == node->parent->left)
	{
		parents_child = &node->parent->left;
	}
	else // if (node == node->parent->right)
	{
		parents_child = &node->parent->right;
	}

	child = right ? node->left : node->right;

	// the next few pointer assignments is the actual rotation process
	*parents_child = child;
	child->parent = node->parent;

	if (right)
	{
		node->left = child->right;
		if (child->right != NULL)
		{
			child->right->parent = node;
		}

		child->right = node;
		node->parent = child;
	}
	else
	{
		node->right = child->left;
		if (child->left != NULL)
		{
			child->left->parent = node;
		}

		child->left = node;
		node->parent = child;
	}

	// update balance factors
	signed char child_old_balance = child->balance;

	if (right)
	{
		child->balance++;
		node->balance += -child_old_balance + 1;
	}
	else
	{
		child->balance--;
		node->balance += -child_old_balance - 1;
	}
}

/**
 * Brings a node back to balance, if it's not. This is accomplished by performing tree rotations. If the node is already
 * in balance ( |balance factor| < 2 ), nothing happens.
 *
 * @param node Points to the node that might be imbalanced.
 * @param tree Points to the tree that contains `node`.
 */
static void nodeFixBalance(struct AvlNode *node, struct AvlTree *tree)
{
	if (node == NULL)
		return;
	if (tree == NULL)
		return;

	// if node is right heavy
	if (node->balance > 0)
	{
		if (node->right->balance < 0)
		{
			// rotate right around the right child
			nodeRotate(node->right, tree, 1);
		}

		// rotate left
		nodeRotate(node, tree, 0);
	}
	// if node is left heavy
	else if (node->balance < 0)
	{
		if (node->left->balance > 0)
		{
			// rotate left aroun left child
			nodeRotate(node->left, tree, 0);
		}

		// rotate right
		nodeRotate(node, tree, 1);
	}
}

/**
 * Frees the given node and it's child nodes recursively.
 *
 * @param node Points to the node to be freed.
 */
static void nodeFree(struct AvlNode *node)
{
	if (node == NULL)
		return;

	nodeFree(node->left);
	nodeFree(node->right);
	free(node);
}

/**
 * Searches for an item in a tree. If the item was not found and `insert` is non-zero, then a new node is
 * allocated and inserted into the tree (no rebalancing!).
 *
 * @param tree Points to the tree to search in.
 * @param item The item to search/insert.
 * @param insert This parameter specifies what happens, if the item was not found. If `insert` is `0`, nothing happens.
 * If it's not 0, the item get inserted into the tree.
 * @param created If `created` is `NULL`, it has no effect. If not, a boolean value (false = 0, true = non-zero) is
 * stored at the location `created` points to. This value tells the caller if a new node was created (true) or not
 * (false).
 * @return `NULL`, if `tree` is `NULL`, or if the item was not found or couldn't be inserted (e.g. due to a malloc
 * error).<br/>
 * Otherwise a pointer to the found/inserted item is returned.
 */
static struct AvlNode *nodeSearch(struct AvlTree *tree, void *item, int insert, int *created)
{
	// Calls `nodeCreate()` and updates `created`.
	struct AvlNode *createNode(void *value)
	{
		struct AvlNode *result = nodeCreate(value);

		if (created != NULL)
			*created = (result != NULL);

		return result;
	}

	if (created != NULL)
		*created = 0;

	if (tree == NULL)
		return NULL;

	// special case of an empty tree
	if (tree->root == NULL)
	{
		if (insert)
			return tree->root = createNode(item);
		else
			return NULL;
	}

	struct AvlNode *parent;
	struct AvlNode *current;
	int comp;

	current = tree->root;

	// find where the new node has to be inserted
	while (current != NULL)
	{
		parent = current;
		comp = tree->compare(item, current->value);

		if (comp < 0)
		{
			current = current->left;
		}
		else if (comp > 0)
		{
			current = current->right;
		}
		else
		{
			// node already exists
			return current;
		}
	}

	// insert the new node
	if (insert)
	{
		current = createNode(item);

		if (current != NULL)
		{
			if (comp < 0)
				parent->left = current;
			else
				parent->right = current;

			current->parent = parent;
		}
	}

	return current;
}

/**
 * Updates the balance factors of a tree after inserting a node.
 *
 * @param node The new inserted node.
 * @return Pointer to the node that has to be rebalanced or `NULL` if the entire tree is still in balance.
 */
struct AvlNode *nodeUpdateBalance(struct AvlNode *node)
{
	if (node == NULL)
		return NULL;

	// go up the tree, as long as we are not root
	while (node->parent != NULL)
	{
		// update the parents balance factor
		if (node == node->parent->left)
		{
			node->parent->balance--;
		}
		else // if (node == node->parent->right)
		{
			node->parent->balance++;
		}

		// go up on level (note: we could be at the root node then)
		node = node->parent;

		if (node->balance == 0)
		{
			// the height of this subtree didn't change, so no further balance-factor-updating is needed
			break;
		}
		else if (abs(node->balance) == 2)
		{
			// node needs to be rebalanced
			return node;
		}
		else if (abs(node->balance) > 2)
		{
			// impossible happened
			// but try rebalancing anyway
			return node;
		}
	}

	return NULL;
}

void avlInit(struct AvlTree *this, int (*compare)(const void *, const void *))
{
	if (this == NULL)
		return;

	this->root = NULL;
	this->compare = compare == NULL ? &dummyCompare : compare;
}

void avlFree(struct AvlTree *this)
{
	if (this == NULL)
		return;

	nodeFree(this->root);
}

int avlContains(struct AvlTree *this, void *item)
{
	return nodeSearch(this, item, 0, NULL) != NULL;
}

int avlInsert(struct AvlTree *this, void *item)
{
	struct AvlNode *node;
	int created;

	// insert node
	node = nodeSearch(this, item, 1, &created);
	if (node == NULL)
		return 0;

	if (!created)
		return 0;

	// fix balance
	node = nodeUpdateBalance(node);
	nodeFixBalance(node, this);

	return 1;
}