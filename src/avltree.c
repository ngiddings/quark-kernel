#include "avltree.h"
#include "heap.h"
#include "stddef.h"

struct avltree_t *avl_new(int key, void *value)
{
    struct avltree_t *new_tree = kmalloc(sizeof(struct avltree_t));
    if(new_tree != NULL)
    {
        new_tree->height = 1;
        new_tree->left = new_tree->right = NULL;
        new_tree->key = key;
        new_tree->value = value;
    }
    return new_tree;
}

int avl_height(struct avltree_t *tree)
{
    if(tree == NULL)
    {
        return 0;
    }
    else
    {
        return tree->height;
    }
}

int avl_balance(struct avltree_t *tree)
{
    if(tree == NULL)
    {
        return 0;
    }
    else
    {
        return avl_height(tree->left) - avl_height(tree->right);
    }
}

void avl_update_height(struct avltree_t *tree)
{
    if(tree != NULL)
    {
        tree->height = 1 + (avl_height(tree->left) > avl_height(tree->right) ? avl_height(tree->left) : avl_height(tree->right));
    }
}

struct avltree_t *avl_right_rotate(struct avltree_t *y)
{
    struct avltree_t *x = y->left;
    struct avltree_t *z = x->right;
    x->right = y;
    y->left = z;
    avl_update_height(x);
    avl_update_height(y);
    return x;
}

struct avltree_t *avl_left_rotate(struct avltree_t *x)
{
    struct avltree_t *y = x->right;
    struct avltree_t *z = y->left;
    y->left = x;
    x->right = z;
    avl_update_height(x);
    avl_update_height(y);
    return y;
}

struct avltree_t *avl_insert(struct avltree_t *tree, int key, void *value)
{
    if(tree == NULL)
    {
        return avl_new(key, value);
    }
    else if(key < tree->key)
    {
        tree->left = avl_insert(tree->left, key, value);
    }
    else if(key > tree->key)
    {
        tree->right = avl_insert(tree->right, key, value);
    }
    else
    {
        return tree;
    }

    avl_update_height(tree);
    int balance = avl_balance(tree);
    if(balance > 1 && key < tree->left->key)
    {
        return avl_right_rotate(tree);        
    }
    else if(balance < -1 && key > tree->right->key)
    {
        return avl_left_rotate(tree);
    }
    else if(balance > 1 && key > tree->left->key)
    {
        tree->left = avl_left_rotate(tree->left);
        return avl_right_rotate(tree);
    }
    else if(balance < -1 && key < tree->right->key)
    {
        tree->right = avl_right_rotate(tree->right);
        return avl_left_rotate(tree);
    }

    return tree;
}

struct avltree_t *avl_remove(struct avltree_t *tree, int key)
{
    if(tree == NULL)
    {
        return NULL;
    }
    else if(key < tree->key)
    {
        tree->left = avl_remove(tree->left, key);
    }
    else if(key > tree->key)
    {
        tree->right = avl_remove(tree->right, key);
    }
    else if(tree->left == NULL || tree->right == NULL)
    {
        struct avltree_t *child = tree->left == NULL ? tree->right : tree->left;
        if(child == NULL)
        {
            child = tree;
            tree = NULL;
        }
        else
        {
            *tree = *child;
        }
        kfree(child);
    }
    else
    {
        struct avltree_t *min = tree->right;
        while(min->left != NULL)
        {
            min = min->left;
        }
        tree->key = min->key;
        tree->value = min->value;
        tree->right = avl_remove(tree->right, min->key);
    }
    
    if(tree == NULL)
    {
        return NULL;
    }
    avl_update_height(tree);
    int balance = avl_balance(tree);
    if(balance > 1 && avl_balance(tree->left) >= 0)
    {
        return avl_right_rotate(tree);
    }
    else if(balance > 1 && avl_balance(tree->left) < 0)
    {
        tree->left = avl_left_rotate(tree->left);
        return avl_right_rotate(tree);
    }
    else if(balance < -1 && avl_balance(tree->right) <= 0)
    {
        return avl_left_rotate(tree);
    }
    else if(balance < -1 && avl_balance(tree->right) > 0)
    {
        tree->right = avl_right_rotate(tree->right);
        return avl_left_rotate(tree);
    }

    return tree;
}

void *avl_get(struct avltree_t *tree, int key)
{
    while(tree != NULL)
    {
        if(key < tree->key)
        {
            tree = tree->left;
        }
        else if(key > tree->key)
        {
            tree = tree->right;
        }
        else
        {
            return tree->value;
        }
    }
    return NULL;
}

void *avl_clear(struct avltree_t *tree)
{
    while(tree != NULL)
    {
        tree = avl_remove(tree, tree->key);
    }
    return tree;
}