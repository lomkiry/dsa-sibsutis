#include "rbtree.h"
#include <stdio.h>

// Левый поворот
struct rbtree *LeftRotate(struct rbtree *root, struct rbtree *x) {
    // Сохраняем правый лист
    struct rbtree *y = x->right;
    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent != NULL)
        if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    else
        root = y;
    y->left = x;
    x->parent = y;
    return root;
}

// Правый поворот
struct rbtree *RightRotate(struct rbtree *root, struct rbtree *x) {
    struct rbtree *y = x->left;
    x->left = y->right;
    if (y->right != NULL)
        y->right->parent = x;
    y->parent = x->parent;
    if (x->parent != NULL)
        if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    else
        root = y;
    y->right = x;
    x->parent = y;
    return root;
}


// Востанавливает красно-черные свойства
struct rbtree *rbtree_fixup(struct rbtree *root, struct rbtree *z) {
    if (root == NULL) return z;

    while (z->parent != NULL && z->parent->color == RED) {
        if (z->parent->parent != NULL && z->parent == z->parent->parent->left) {
            // Левая ветвь
            struct rbtree *y = z->parent->parent->right; // Дядя
            if (y != NULL && y->color == RED) {
                // Case 1: красный дядя
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                // Case 2 & 3: черный дядя
                if (z == z->parent->right) {
                    // Case 2: угол -> линия
                    z = z->parent;
                    root = LeftRotate(root, z);
                }
                // Case 3: линия
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                root = RightRotate(root, z->parent->parent);
            }
        } else if (z->parent->parent != NULL) {
            // Правая ветвь
            struct rbtree *y = z->parent->parent->left; // Дядя
            if (y != NULL && y->color == RED) {
                // Case 1 (зеркальный): красный дядя
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                // Case 2 & 3 (зеркальный): черный дядя
                if (z == z->parent->left) {
                    // Case 2 (зеркальный): угол -> линия
                    z = z->parent;
                    root = RightRotate(root, z);
                }
                // Case 3 (зеркальный): линия
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                root = LeftRotate(root, z->parent->parent);
            }
        }
    }
    root->color = BLACK;
    return root;
}

struct rbtree *rbtree_delete_fixup(struct rbtree *root, struct rbtree *x, struct rbtree *x_parent) {
    while (x != root && (x == NULL || x->color == BLACK)) {
        if (x == x_parent->left) {
            // x - левый ребенок
            struct rbtree *w = x_parent->right;  // Брат
            
            // Брат красный
            if (w->color == RED) {
                w->color = BLACK;
                x_parent->color = RED;
                root = LeftRotate(root, x_parent);
                w = x_parent->right;
            }
            
            // Оба ребенка брата черные
            if ((w->left == NULL || w->left->color == BLACK) &&
                (w->right == NULL || w->right->color == BLACK)) {
                w->color = RED;
                x = x_parent;
                x_parent = x->parent;
            } else {
                // Левый ребенок брата красный, правый черный
                if (w->right == NULL || w->right->color == BLACK) {
                    if (w->left != NULL) w->left->color = BLACK;
                    w->color = RED;
                    root = RightRotate(root, w);
                    w = x_parent->right;
                }
                
                // Правый ребенок брата красный
                w->color = x_parent->color;
                x_parent->color = BLACK;
                if (w->right != NULL) w->right->color = BLACK;
                root = LeftRotate(root, x_parent);
                x = root;
                break;
            }
        } else {
            // Симметричные случаи для правого ребенка
            struct rbtree *w = x_parent->left;
            
            if (w->color == RED) {
                w->color = BLACK;
                x_parent->color = RED;
                root = RightRotate(root, x_parent);
                w = x_parent->left;
            }
            
            if ((w->right == NULL || w->right->color == BLACK) &&
                (w->left == NULL || w->left->color == BLACK)) {
                w->color = RED;
                x = x_parent;
                x_parent = x->parent;
            } else {
                if (w->left == NULL || w->left->color == BLACK) {
                    if (w->right != NULL) w->right->color = BLACK;
                    w->color = RED;
                    root = LeftRotate(root, w);
                    w = x_parent->left;
                }
                
                w->color = x_parent->color;
                x_parent->color = BLACK;
                if (w->left != NULL) w->left->color = BLACK;
                root = RightRotate(root, x_parent);
                x = root;
                break;
            }
        }
    }
    
    if (x != NULL) x->color = BLACK;
    return root;
}

// Создает узел
struct rbtree *rbtree_create_node(struct rbtree *parent, int key, char *value){
    struct rbtree *node = malloc(sizeof(*node));
    if (node == NULL)
        return NULL;
    // Копируем строку
    if (value != NULL) {
        node->value = malloc(strlen(value) + 1);
        if (node->value == NULL) {
            free(node);
            return NULL;
        }
        strcpy(node->value, value);
    } else {
        node->value = NULL;
    }

    // Вставляем все значения
    node->key = key;
    node->color = RED;
    node->left = NULL;
    node->right = NULL;
    node->parent = parent;

    return node;
}

// Добавляет узел в дерево
struct rbtree *rbtree_add(struct rbtree *root, int key, char *value) {
    struct rbtree *tree = root, *parent = NULL;
    
    // Ищем куда вставить
    while (tree != NULL) {
        parent = tree;
        if (key < tree->key) 
            tree = tree->left;
        else if (key > tree->key) 
            tree = tree->right;
        else 
            return root;  // Ключ уже существует
    }
    
    // Создаем новый красный узел
    struct rbtree *node = rbtree_create_node(parent, key, value);
    if (node == NULL) return root;
    
    // Прикрепляем узел к дереву 
    if (parent == NULL) {
        root = node;  // Дерево было пустым
    } else if (key < parent->key) {
        parent->left = node;
    } else {
        parent->right = node;
    }
    
    // Балансируем дерево и возвращаем новый root
    return rbtree_fixup(root, node);   
}

struct rbtree *rbtree_lookup(struct rbtree *tree, int key) {
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

struct rbtree *rbtree_min(struct rbtree *tree) {
    if (tree == NULL)
        return NULL;
    while (tree->left != NULL)
        tree = tree->left;
    return tree;
}

struct rbtree *rbtree_max(struct rbtree *tree) {
    if (tree == NULL)
        return NULL;
    while (tree->right != NULL)
        tree = tree->right;
    return tree;
}

struct rbtree *rbtree_delete(struct rbtree *root, int key) {
    struct rbtree *z = rbtree_lookup(root, key);
    if (z == NULL) return root;  
    
    struct rbtree *y, *x;
    
    // Находим узел для удаления (y)
    if (z->left == NULL || z->right == NULL) {
        y = z; // 0 или 11 детей у узла z
    } else {
        y = rbtree_min(z->right);  
    }
    
    // Находим ребенка y (x)
    if (y->left != NULL) {
        x = y->left;
    } else {
        x = y->right;
    }
    
    // Удаляем y из дерева
    if (x != NULL) {
        x->parent = y->parent;
    }
    
    if (y->parent == NULL) {
        root = x;  // y был корнем
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    
    // Если y != z, копируем данные
    if (y != z) {
        z->key = y->key;
        free(z->value);
        z->value = malloc(strlen(y->value) + 1);
        strcpy(z->value, y->value);
    }
    
    // Если удаленный узел был черным, балансируем
    if (y->color == BLACK) {
        root = rbtree_delete_fixup(root, x, y->parent);
    }
    
    free(y->value);
    free(y);
    return root;
}

void rbtree_free(struct rbtree *root) {
    while (root != NULL) {
        root = rbtree_delete(root, root->key);
    }
    free(root);
}

void rbtree_print_dfs(struct rbtree *root, int level) {
    if (root == NULL) {
        return;  // Базовый случай рекурсии
    }
    
    // 1. Сначала идем в ПРАВОЕ поддерево (будет выведено СЛЕВА)
    rbtree_print_dfs(root->right, level + 1);
    
    // 2. Выводим текущий узел с отступами
    for (int i = 0; i < level; i++) {
        printf("   ");  // 3 пробела на каждый уровень
    }
    
    // Выводим ключ и цвет узла
    if (root->color == RED) {
        printf("%d[R]", root->key);
    } else {
        printf("%d[B]", root->key);
    }
    
    printf("\n");
    
    // 3. Затем идем в ЛЕВОЕ поддерево (будет выведено СПРАВА)
    rbtree_print_dfs(root->left, level + 1);
}
