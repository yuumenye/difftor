#ifndef TREE_H
#define TREE_H

struct node {
        int type;
        int value;
        struct node *left;
        struct node *right;
        struct node *parent;
}

struct tree {
        struct node *root;
}

#endif
