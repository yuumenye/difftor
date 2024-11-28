#ifndef TREE_H
#define TREE_H

struct node {
        int type;
        int value;
        struct node *left;
        struct node *right;
        struct node *parent;
};

struct tree {
        struct node *root;
};

struct tree *tree_ctor(void);
void tree_dtor(struct tree *tree);
struct node *node_ctor(void);
void node_dtor(struct node *node);

#endif
