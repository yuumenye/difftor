#ifndef TREE_H
#define TREE_H

enum op_type {
        ADD,
        SUB,
        MUL,
        DIV,
        POW
};

enum val_type {
        NUM,
        VAR,
        OP
};

struct op_desc {
        enum op_type val;
        const char *name;
};

struct node {
        enum val_type type;
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
