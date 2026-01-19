#ifndef RTREE_H
#define RTREE_H

#include <stdint.h>

struct rtree {
    char *key;
    uint32_t value;
    struct rtree *child;
    struct rtree *sibling;
};

struct rtree *rtree_create();
struct rtree *rtree_insert(struct rtree *root, char *key, uint32_t value);
struct rtree *rtree_lookup(struct rtree *root, char *key);
struct rtree *rtree_delete(struct rtree *root, char *key);
void rtree_print(struct rtree *root, int level);
struct rtree *GetChild(struct rtree *node, char c);
void SetChild(struct rtree *node, char c, struct rtree *child);

#endif