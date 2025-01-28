#ifndef TREE_H
#define TREE_H

enum op_type {
        ADD,
        SUB,
        MUL,
        DIV,
        POW,
        SIN,
        COS,
        TAN,
        ASIN,
        ACOS,
        ATAN,
        SINH,
        COSH,
        TANH,
        LOG
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
        double val;
        struct node *left;
        struct node *right;
};

struct tree {
        struct node *root;
};

struct tree *tree_ctor(void);
void tree_dtor(struct tree *tree);
struct node *node_ctor(enum val_type type, double val,
                struct node *left, struct node *right);
void node_dtor(struct node *node);
void subtree_dtor(struct node *node);
struct node *subtree_copy(struct node *node);

#endif
