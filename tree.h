#ifndef TREE_H
#define TREE_H

enum op {
        ADD = 0,
        SUB = 1,
        MUL = 2,
        DIV = 3
};

enum type {
        NUM = 0,
        VAR = 1,
        OP = 2
};

struct node {
        enum type type;
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
