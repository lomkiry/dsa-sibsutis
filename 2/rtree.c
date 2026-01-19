#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "rtree.h"

struct rtree *rtree_create() {
    struct rtree *node;
    if ((node = malloc(sizeof(*node))) == NULL)
        return NULL;
    node->key = NULL;
    node->value = 0;  
    node->sibling = NULL;
    node->child = NULL;
    return node;
}

static int common_prefix(const char *str1, const char *str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0' && str1[i] == str2[i]) {
        i++;
    }
    return i;
}

static void rtree_free_node(struct rtree *node) {
    if (node == NULL) return;
    free(node->key);
    free(node);
}

struct rtree *GetChild(struct rtree *node, char c) {
    struct rtree *child = node->child;
    while (child != NULL) {
        if (child->key[0] == c) {
            return child;
        }
        child = child->sibling;
    }
    return NULL;
}

void SetChild(struct rtree *node, char c, struct rtree *child) {
    child->sibling = node->child;
    node->child = child;
}

struct rtree *rtree_insert(struct rtree *root, char *key, uint32_t value) {
    if (root == NULL) {
        root = rtree_create();
        root->key = strdup(key);
        root->value = value;
        return root;
    }

    struct rtree *node = root;
    struct rtree *parent = NULL;
    
    while (1) {
        int prefix_len = common_prefix(node->key, key);
        //случай 1 - если ключи полностью совпадают, то обновляем значение
        if (prefix_len == strlen(node->key) && prefix_len == strlen(key)) {
            node->value = value;
            return root;
        }
        //случай 2 - текущий узел является префиксом вставляемого ключа
        if (prefix_len == strlen(node->key)) {
            key += prefix_len; //сдвиг указателя на часть ключа
            parent = node;
            
            struct rtree *child = GetChild(node, key[0]);
            
            if (child == NULL) {
                struct rtree *new_child = rtree_create();
                new_child->key = strdup(key);
                new_child->value = value;
                SetChild(node, key[0], new_child);
                return root;
            }

            node = child;
        //случай 3 - ключи имеют общий префикс, но не совпадают полностью
        } else {
            struct rtree *new_node = rtree_create();
            new_node->key = strdup(node->key + prefix_len);
            new_node->value = node->value; 
            new_node->child = node->child; 
            
            char *old_key = node->key;
            node->key = malloc(prefix_len + 1);
            strncpy(node->key, old_key, prefix_len);
            node->key[prefix_len] = '\0';
            free(old_key);
            
            node->value = 0;
            node->child = new_node;
            
            if (prefix_len < strlen(key)) {
                struct rtree *insert_node = rtree_create();
                insert_node->key = strdup(key + prefix_len);
                insert_node->value = value;
                
                //вставляем узел и выравниваем в лексическом порядке
                if (strcmp(insert_node->key, new_node->key) < 0) {
                    insert_node->sibling = new_node;
                    node->child = insert_node;
                } else {
                    insert_node->sibling = new_node->sibling;
                    new_node->sibling = insert_node;
                }
            } else {
                node->value = value;
            }
            return root;
        }
    }
}

struct rtree *rtree_lookup(struct rtree *root, char *key) {
    struct rtree *node = root;
    
    while (node != NULL && key != NULL && *key != '\0') {
        int prefix_len = common_prefix(node->key, key);
        
        if (prefix_len != strlen(node->key)) {
            return NULL; 
        }
        
        key += prefix_len;
        
        if (*key == '\0') {
            return (node->value != 0) ? node : NULL;
        }
        
        struct rtree *found = GetChild(node, *key);
        
        if (found == NULL) {
            return NULL;
        }
        node = found;
    }
    
    return NULL;
}

static struct rtree *merge_with_child(struct rtree *node) {
    //проверка на существование узла который имеет одного ребенка
    if (node == NULL || node->child == NULL) return node;
    
    if (node->child->sibling == NULL) {
        struct rtree *child = node->child;
        
        char *new_key = malloc(strlen(node->key) + strlen(child->key) + 1);
        strcpy(new_key, node->key);
        strcat(new_key, child->key);
        
        free(node->key);
        node->key = new_key;
        node->value = child->value;
        
        struct rtree *old_child = node->child;
        node->child = child->child;
        free(old_child->key);
        free(old_child);
    }
    
    return node;
}

static struct rtree *rtree_delete_dfs(struct rtree *root, char *key, int *deleted) {
    if (root == NULL) return NULL;
    
    struct rtree *node = root;
    struct rtree *prev = NULL;
    
    while (node != NULL) { 
        int prefix_len = common_prefix(node->key, key);
        
        if (prefix_len == strlen(node->key) && prefix_len == strlen(key)) {
            if (node->value != 0) {
                *deleted = 1;
                
                node->value = 0;
                
                if (node->child == NULL) {
                    if (prev != NULL) {
                        prev->sibling = node->sibling;
                    } else {
                        root = node->sibling;
                    }
                    rtree_free_node(node);
                } else {
                    if (node->child->sibling == NULL) {
                        merge_with_child(node);
                    }
                }
            }
            return root;
        }
        
        if (prefix_len == strlen(node->key)) {
            char *remaining = key + prefix_len;
            struct rtree *next_sibling = node->sibling;
            node->child = rtree_delete_dfs(node->child, remaining, deleted);
            
            if (*deleted) {
                if (node->value == 0 && node->child != NULL && node->child->sibling == NULL) {
                    merge_with_child(node);
                }
                
                if (node->value == 0 && node->child == NULL) {
                    if (prev != NULL) {
                        prev->sibling = node->sibling;
                    } else {
                        root = node->sibling;
                    }
                    rtree_free_node(node);
                    return root;
                }
            }
            
            prev = node;
            node = next_sibling;
        } else {
            prev = node;
            node = node->sibling;
        }
    }
    
    return root;
}

struct rtree *rtree_delete(struct rtree *root, char *key) {
    int deleted = 0;
    struct rtree *result = rtree_delete_dfs(root, key, &deleted);
    return result;
}

void rtree_print(struct rtree *root, int level) {
    if (root == NULL) return;
    
    struct rtree *node = root;
    
    while (node != NULL) {
        for (int i = 0; i < level; i++) {
            printf("  ");
        }
        
        printf("- %s", node->key);
        if (node->value != 0) {
            printf(" [val=%u]", node->value);
        }
        printf("\n");
        
        if (node->child != NULL) {
            rtree_print(node->child, level + 1);
        }
        
        node = node->sibling;
    }
}