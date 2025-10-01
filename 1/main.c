#include "rbtree.h"

int main(void) {
    struct rbtree *root = NULL;
    rbtree_add(root, 1, "1");
    rbtree_print_dfs(root, 0);
    rbtree_add(root, 2, "2");
    rbtree_add(root, 2, "2");
    rbtree_add(root, 3, "3");
    rbtree_add(root, 5, "5");
    rbtree_add(root, 4, "4");
    rbtree_print_dfs(root, 1);
}
