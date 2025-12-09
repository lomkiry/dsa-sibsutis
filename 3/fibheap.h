#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct Node {
    int key;
    struct Node *parent;
    struct Node *child;
    struct Node *left;
    struct Node *right;
    int degree;
    bool mark;
    char *value;
};

struct Fibheap {
    int size; //nnode(типа)
    struct Node *min;
};

struct Fibheap *build_heap(void);
struct Fibheap *Fibheap_insert(struct Fibheap *heap, int key, char *value);
struct Node *Fibheap_min(struct Fibheap *heap);
struct Fibheap *Fibheap_union(struct Fibheap *heap1, struct Fibheap *heap2);
struct Fibheap *Fibheap_extractmin(struct Fibheap *heap);
struct Fibheap *Fibheap_decrease_key(struct Fibheap *heap, struct Node *node, int newkey);
struct Fibheap *Fibheap_delete(struct Fibheap *heap, int key);