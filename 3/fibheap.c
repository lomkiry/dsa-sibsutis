#include "fibheap.h"
#include <math.h>

struct Fibheap *build_heap(void) {
    struct Fibheap *heap = (struct Fibheap*)malloc(sizeof(struct Fibheap));
    if (heap) {
        heap->size = 0;
        heap->min = NULL;
    }
    return heap;
}

void Fibheap_add_node_to_root_list(struct Node *node, struct Node *h) {
    if (h == NULL) {
        return;
    }
    
    if (h->left == h) {
        /* Случай 1: список содержит один корень */
        h->left = node;
        h->right = node;
        node->right = h;
        node->left = h;
    } else {
        /* Случай 2: список содержит более одного корня */
        struct Node *lnode = h->left;
        h->left = node;
        node->right = h;
        node->left = lnode;
        lnode->right = node;
    }
}

struct Fibheap *Fibheap_insert(struct Fibheap *heap, int key, char *value) {
    struct Node *new_node = (struct Node*)malloc(sizeof(struct Node));
    if (!new_node)
        return heap;
    
    // Инициализация узла 
    new_node->key = key;
    new_node->value = (char*)malloc(strlen(value) + 1);
    if (new_node->value) {
        strcpy(new_node->value, value);
    } else {
        free(new_node);
        return heap;
    }
    new_node->parent = NULL;
    new_node->child = NULL;
    new_node->degree = 0;
    new_node->mark = false;
    new_node->left = new_node;
    new_node->right = new_node;
    
    // Добавляем узел в корневой список
    Fibheap_add_node_to_root_list(new_node, heap->min);
    
    // Обновляем min
    if (heap->min == NULL || key < heap->min->key) {
        heap->min = new_node;
    }
    
    heap->size++;
    return heap;
}

struct Node *Fibheap_min(struct Fibheap *heap) {
    return heap->min;
}

struct Node *FibHeapLinkList(struct Node *h1, struct Node *h2) {
    if (!h1) return h2;
    if (!h2) return h1;

    struct Node *h1_right = h1->right;
    struct Node *h2_left  = h2->left;

    h1->right = h2;
    h2->left  = h1;

    h2_left->right = h1_right;
    h1_right->left = h2_left;

    return h1;
}


struct Fibheap *Fibheap_union(struct Fibheap *heap1, struct Fibheap *heap2) {
    struct Fibheap *heap = build_heap();
    heap->min = heap1->min;
    FibHeapLinkList(heap1->min, heap2->min);

    if ((!heap1->min) || (!heap2->min && heap2->min->key < heap->min->key))
        heap->min = heap2->min;

    heap->size = heap1->size + heap2->size;

    free(heap1);
    free(heap2);
    
    return heap;
}

int D(int n) {
    if (n == 0) return 0;
    return (int)floor(log2(n));
}

void Fibheap_remove_node_from_root_list(struct Node *node, struct Fibheap *heap) {
    if (node->right == node) {
        if (heap->min == node) {
            heap->min = NULL;
        }
    } else {
        node->left->right = node->right;
        node->right->left = node->left;
        if (heap->min == node) {
            heap->min = node->right;
        }
    }
    
    node->left = node;
    node->right = node;
    node->parent = NULL;
}

void Fibheap_link(struct Fibheap *heap, struct Node *y, struct Node *x) {
    Fibheap_remove_node_from_root_list(y, heap);
    
    if (x->child == NULL) {
        x->child = y;
        y->left = y;
        y->right = y;
    } else {
        struct Node *child = x->child;
        struct Node *child_left = child->left;
        
        child->left = y;
        y->right = child;
        y->left = child_left;
        child_left->right = y;
    }
    
    y->parent = x;
    x->degree++;
    y->mark = false;
}

void Fibheap_consolidate(struct Fibheap *heap) {
    int max_degree = D(heap->size) + 1;
    struct Node **A = (struct Node**)calloc(max_degree, sizeof(struct Node*));
    
    struct Node *start = heap->min;
    struct Node *current = start;
    
    if (current != NULL) {
        do {
            struct Node *x = current;
            struct Node *next = current->right;
            int d = x->degree;
            
            while (A[d] != NULL) {
                struct Node *y = A[d];
                if (x->key > y->key) {
                    struct Node *temp = x;
                    x = y;
                    y = temp;
                }
                Fibheap_link(heap, y, x);
                A[d] = NULL;
                d++;
            }
            A[d] = x;
            current = next;
        } while (current != start);
    }
    
    heap->min = NULL;
    for (int i = 0; i < max_degree; i++) {
        if (A[i] != NULL) {
            if (heap->min == NULL) {
                heap->min = A[i];
                A[i]->left = A[i];
                A[i]->right = A[i];
            } else {
                Fibheap_add_node_to_root_list(A[i], heap->min);
                if (A[i]->key < heap->min->key) {
                    heap->min = A[i];
                }
            }
        }
    }
    
    free(A);
}

struct Fibheap *Fibheap_extractmin(struct Fibheap *heap) {
    struct Node *z = heap->min;
    if (z == NULL) {
        return NULL;
    }
    
    if (z->child != NULL) {
        struct Node *child = z->child;
        struct Node *current = child;
        
        do {
            struct Node *next = current->right;
            Fibheap_add_node_to_root_list(current, heap->min);
            current->parent = NULL;
            current = next;
        } while (current != child);
    }
    
    Fibheap_remove_node_from_root_list(z, heap);
    
    if (z == z->right) {
        heap->min = NULL;
    } else {
        heap->min = z->right;
        Fibheap_consolidate(heap);
    }
    
    heap->size--;
    return heap;
}

void Fibheap_cut(struct Fibheap *heap, struct Node *x, struct Node *y) {
    if (x->right == x) {
        y->child = NULL;
    } else {
        x->left->right = x->right;
        x->right->left = x->left;
        if (y->child == x) {
            y->child = x->right;
        }
    }
    
    y->degree = y->degree - 1;
    
    Fibheap_add_node_to_root_list(x, heap->min);
    x->parent = NULL;
    x->mark = false;
}

void Fibheap_cascading_cut(struct Fibheap *heap, struct Node *y) {
    struct Node *z = y->parent;
    if (z == NULL) {
        return;
    }
    
    if (y->mark == false) {
        y->mark = true;
    } else {
        Fibheap_cut(heap, y, z);
        Fibheap_cascading_cut(heap, z);
    }
}

struct Fibheap *Fibheap_decrease_key(struct Fibheap *heap, struct Node *x, int newkey) {
    if (newkey > x->key) {
        return heap; 
    }
    x->key = newkey;
    struct Node *y = x->parent;
    
    if (y != NULL && x->key < y->key) {
        Fibheap_cut(heap, x, y);
        Fibheap_cascading_cut(heap, y);
    }
    
    if (heap->min == NULL || x->key < heap->min->key) {
        heap->min = x;
    }
    
    return heap;
}

struct Fibheap *Fibheap_delete(struct Fibheap *heap, int key) {
    struct Node *node_to_delete = NULL;
    
    if (heap->min != NULL) {
        struct Node *current = heap->min;
        do {
            if (current->key == key) {
                node_to_delete = current;
                break;
            }
            current = current->right;
        } while (current != heap->min);
    }
    
    if (node_to_delete == NULL) {
        return heap; 
    }
    
    Fibheap_decrease_key(heap, node_to_delete, -2147483647); 
    
    Fibheap_extractmin(heap);
    
    return heap;
}