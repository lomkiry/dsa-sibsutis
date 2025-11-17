#pragma once

#include <stdlib.h>

struct bstree {
    int key;
    char *value;
    struct bstree *left;
    struct bstree *right;
};

struct bstree *bstree_create(int key, char *value);
struct bstree *bstree_add(struct bstree *tree, int key, char *value);
struct bstree *bstree_lookup(struct bstree *tree, int key);
struct bstree *bstree_delete(struct bstree *tree, int key);
struct bstree *bstree_min(struct bstree *tree);
struct bstree *bstree_max(struct bstree *tree);
struct bstree *bstree_delete(struct bstree *tree, int key);
void bstree_free(struct bstree *tree);
