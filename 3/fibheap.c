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
    
    // Инициализация узла (одинаково)
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
    
    // Добавляем узел в корневой список (по псевдокоду)
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

struct Node *FibHeapLinkList(struct Node *heap1, struct Node *heap2) {
    if (!heap1 || !heap2)
        return NULL;

    struct Node *right1 = heap1->right;
    struct Node *left1 = heap1->left;

    heap1->right = heap2;
    heap2->left = heap1;

    right1->left = left1;
    left1->right = right1;

    return heap1;
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

// Функция удаления узла из корневого списка
void Fibheap_remove_node_from_root_list(struct Node *node, struct Fibheap *heap) {
    if (node->right == node) {
        // Единственный узел в списке
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
    
    // Изолируем узел
    node->left = node;
    node->right = node;
    node->parent = NULL;
}

void Fibheap_link(struct Fibheap *heap, struct Node *y, struct Node *x) {
    // Удаляем y из корневого списка
    Fibheap_remove_node_from_root_list(y, heap);
    
    // Делаем y дочерним узлом x
    if (x->child == NULL) {
        x->child = y;
        y->left = y;
        y->right = y;
    } else {
        // Добавляем y в список дочерних узлов x
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
    
    // Находим начальный узел для обхода
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
                    // Меняем x и y местами
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
    
    // Перестраиваем корневой список
    heap->min = NULL;
    for (int i = 0; i < max_degree; i++) {
        if (A[i] != NULL) {
            if (heap->min == NULL) {
                // Создаем новый корневой список
                heap->min = A[i];
                A[i]->left = A[i];
                A[i]->right = A[i];
            } else {
                // Добавляем в существующий корневой список
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
    
    // Добавляем всех детей z в корневой список
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
    
    // Удаляем z из корневого списка
    Fibheap_remove_node_from_root_list(z, heap);
    
    if (z == z->right) {
        // z был единственным узлом в корневом списке
        heap->min = NULL;
    } else {
        heap->min = z->right;
        Fibheap_consolidate(heap);
    }
    
    heap->size--;
    return heap;
}

void Fibheap_cut(struct Fibheap *heap, struct Node *x, struct Node *y) {
    // Удаляем x из списка дочерних узлов y
    if (x->right == x) {
        // x - единственный ребенок
        y->child = NULL;
    } else {
        x->left->right = x->right;
        x->right->left = x->left;
        if (y->child == x) {
            y->child = x->right;
        }
    }
    
    y->degree = y->degree - 1;
    
    // Добавляем x в список корней кучи heap
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
        return heap; /* Новый ключ больше текущего значения ключа */
    }
    x->key = newkey;
    struct Node *y = x->parent;
    
    if (y != NULL && x->key < y->key) {
        /* Нарушены свойства min-heap: ключ родителя больше */
        /* Вырезаем x и переносим его в список корней */
        Fibheap_cut(heap, x, y);
        Fibheap_cascading_cut(heap, y);
    }
    
    /* Корректируем указатель на минимальный узел */
    if (heap->min == NULL || x->key < heap->min->key) {
        heap->min = x;
    }
    
    return heap;
}

struct Fibheap *Fibheap_delete(struct Fibheap *heap, int key) {
    // Сначала нужно найти узел с данным ключом
    // Для простоты предположим, что у нас есть функция поиска
    struct Node *node_to_delete = NULL;
    
    // Обход кучи для поиска узла с ключом
    if (heap->min != NULL) {
        struct Node *current = heap->min;
        do {
            if (current->key == key) {
                node_to_delete = current;
                break;
            }
            // Здесь должна быть рекурсивная проверка детей
            current = current->right;
        } while (current != heap->min);
    }
    
    if (node_to_delete == NULL) {
        return heap; // Узел не найден
    }
    
    // Уменьшаем ключ до минимально возможного значения
    Fibheap_decrease_key(heap, node_to_delete, -2147483647); 
    
    // Извлекаем минимальный узел (который теперь наш узел)
    Fibheap_extractmin(heap);
    
    return heap;
}