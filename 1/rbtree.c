#include "rbtree.h"

// Правый поворот
struct rbtree *LeftRotate(struct rbtree *root, struct rbtree *x) {
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
// Левый поворот
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
struct rbtree *rbtree_fixup(struct rbtree *root, struct rbtree *z){
    while (z->parent != NULL && z->parent->color == RED) {
           if (z->parent->parent != NULL && z->parent == z->parent->parent->left) {
               struct rbtree *y = z->parent->parent->right;
               if (y != NULL && y->color == RED) {
                   z->parent->color = BLACK;
                   y->color = BLACK;
                   z->parent->parent->color = RED;
                   z = z->parent->parent;
               } else {
                   if (z == z->parent->right) {
                       z = z->parent;
                       root = LeftRotate(root, z);
                   } else {
                       z = z->parent;
                       root = RightRotate(root, z->parent->parent);
                   }
               }
           } else if (z->parent->parent != NULL) {
               struct rbtree *y = z->parent->parent->left; 
               if (y != NULL && y->color == RED) {
                   z->parent->color = BLACK;
                   y->color = BLACK;
                   z->parent->parent->color = RED;
                   z = z->parent->parent;
               } else {
                   if (z == z->parent->left) {  
                       z = z->parent;
                       root = RightRotate(root, z);  
                   }
                   z->parent->color = 1;
                   z->parent->parent->color = 0;
                   root = LeftRotate(root, z->parent->parent);
               }
           }
    }
    root->color = 1;
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
    node->value = value;
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
            return NULL;
    }
    // Создаем новый красный узел и привязываем родителя, ключ и значение
    struct rbtree *node = rbtree_create_node(parent, key, value);
    
    // Прикрепляем узел к дереву 
    if (parent == NULL)
        root = node;
    else if (key < parent->key)
        parent->left = node;
    else
        parent->right = node;
    // Балансируем дерево
    return rbtree_fixup(root, node);   
}

void rbtree_print_dfs(struct rbtree *root, int level) {
    if (root == NULL) {
        return;
    }
    
    rbtree_print_dfs(root->right, level + 1);
    
    for (int i = 0; i < level; i++) {
        printf("   ");
    }
    
    if (root->color == 0) {
        printf("%d[R]\n", root->key);
    } else {
        printf("%d[B]\n", root->key);
    }
    
    rbtree_print_dfs(root->left, level + 1);
}
