#include "bstree.h"

struct bstree *bstree_create(int key, char *value) {
    struct bstree *node;
    node = malloc(sizeof(*node));
    if (node != NULL) {
        node->key = key;
        node->value = value;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

struct bstree *bstree_add(struct bstree *tree, int key, char *value) {
    struct bstree *parent = NULL, *current = tree;
    
    while (current != NULL) {
        parent = current;
        if (key < current->key)
            current = current->left;
        else if (key > current->key)
            current = current->right;
        else
            return tree;  
    }
    
    struct bstree *node = bstree_create(key, value);
    if (node == NULL) return tree;
    
    if (parent == NULL) {
        return node;
    } else if (key < parent->key) {
        parent->left = node;
    } else {
        parent->right = node;
    }
    
    return tree;
}

struct bstree *bstree_lookup(struct bstree *tree, int key) {
    while (tree != NULL) {
        if (key == tree->key) {
            return tree;
        } else if (key < tree->key) {
            tree = tree->left;
        } else {
            tree = tree->right;
        }
    }
    return tree;
}

struct bstree *bstree_min(struct bstree *tree) {
    if (tree == NULL)
        return NULL;
    while (tree->left != NULL)
        tree = tree->left;
    return tree;
}

struct bstree *bstree_max(struct bstree *tree) {
    if (tree == NULL)
        return NULL;
    while (tree->right != NULL)
        tree = tree->right;
    return tree;
}


struct bstree *bstree_delete(struct bstree *tree, int key) {
    if (tree == NULL)
        return tree;
    
    if (key < tree->key) {
        tree->left = bstree_delete(tree->left, key);
    } else if (key > tree->key) {
        tree->right = bstree_delete(tree->right, key);
    } else {
        
        if (tree->left == NULL) {
            struct bstree *temp = tree->right;
            free(tree);
            return temp;
        } else if (tree->right == NULL) {
            struct bstree *temp = tree->left;
            free(tree);
            return temp;
        }
        
        struct bstree *temp = bstree_min(tree->right);
        
        tree->key = temp->key;
        tree->value = temp->value;
        
        tree->right = bstree_delete(tree->right, temp->key);
    }
    
    return tree;
}

void bstree_free(struct bstree *tree) {
    if (tree == NULL)
        return;
    
    bstree_free(tree->left);
    bstree_free(tree->right);
    
    free(tree);
}
