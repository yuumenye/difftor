#ifndef TREE_H
#define TREE_H

struct node {
        int val;
        struct node *left;
        struct node *right;
};

struct tree {
        struct node *root;
};

struct tree *tree_ctor(void);
void tree_dtor(struct tree *tree);
struct node *node_ctor(void);
void node_dtor(struct node *node);

#endif
