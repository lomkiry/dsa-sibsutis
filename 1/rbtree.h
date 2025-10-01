#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define RED 1
#define BLACK 0

struct rbtree {
    int key;
    char *value;
    char color; 
    struct rbtree *parent;
    struct rbtree *left;
    struct rbtree *right;
};

struct rbtree *rbtree_add(struct rbtree *root, int key, char *value);
struct rbtree *rbtree_lookup(struct rbtree *root, int key);
struct rbtree *rbtree_delete(struct rbtree *root, int key);
struct rbtree *rbtree_min(struct rbtree *root);
struct rbtree *rbtree_max(struct rbtree *root);
void rbtree_free(struct rbtree *root);
void rbtree_print_dfs(struct rbtree *root, int level);
