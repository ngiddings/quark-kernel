#pragma once

/**
 * @brief An AVL tree.
 * 
 */
struct avltree_t {
    int height;
    int key;
    void *value;
    struct avltree_t *left, *right;
};

/**
 * @brief Inserts a new node into an AVL tree.
 * 
 * A new node will be allocated and assigned the provided key and value.
 * 
 * @param tree A pointer to the tree to insert into.
 * @param key The key to associate the new node with.
 * @param value A pointer to store into the new node.
 */
struct avltree_t *avl_insert(struct avltree_t *tree, int key, void *value);

/**
 * @brief Removes the node associated with `key` from `tree`.
 * 
 * @param tree A pointer to the tree to remove from.
 * @param key The key of the node to remove.
 * @return struct avltree_t* The pointer stored in the node that was just removed.
 */
struct avltree_t *avl_remove(struct avltree_t *tree, int key);

/**
 * @brief Searches for a node in `tree` with a matching key, and returns the
 * 
 * @param tree 
 * @param key 
 * @return void* 
 */
void *avl_get(struct avltree_t *tree, int key);

/**
 * @brief Removes every node present on the given tree.
 * 
 * @param tree
 * @return NULL
 */
void *avl_clear(struct avltree_t *tree);