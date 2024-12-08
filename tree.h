#ifndef TREE_H
#define TREE_H

enum op {
        ADD = 0,
        SUB = 1,
        MUL = 2,
        DIV = 3,
        POW = 4,
};

enum val_type {
        NUM = 0,
        VAR = 1,
        OP = 2
};

struct op_desc {
        enum op val;
        const char *name;
};

struct node {
        enum val_type type;
        int val;
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
