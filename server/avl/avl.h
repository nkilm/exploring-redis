#include <stddef.h>
#include <stdint.h>

// Structure representing a node in the AVL tree
struct AVLNode
{
    uint32_t depth;
    uint32_t cnt;
    AVLNode *left;
    AVLNode *right;
    AVLNode *parent;
};

// Initialize an AVLNode
void avl_init(AVLNode *node);

// Get the depth of an AVLNode
uint32_t avl_depth(AVLNode *node);

// Get the count of nodes in the subtree rooted at the given node
uint32_t avl_cnt(AVLNode *node);

// Perform an AVL tree insertion with rebalancing
AVLNode *avl_insert(AVLNode *root, AVLNode *node, int (*compare)(AVLNode *, AVLNode *));

// Perform an AVL tree deletion with rebalancing
AVLNode *avl_delete(AVLNode *root, AVLNode *node, int (*compare)(AVLNode *, AVLNode *));

// Functions to balance the height of the AVL tree

// fix imbalanced nodes and maintain invariants until the root is reached
AVLNode *avl_fix(AVLNode *node);

// detach a node and returns the new root of the tree
AVLNode *avl_del(AVLNode *node);

// offset into the succeeding or preceding node.
// note: the worst-case is O(log(n)) regardless of how long the offset is.
AVLNode *avl_offset(AVLNode *node, int64_t offset);
