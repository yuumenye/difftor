#include <stdlib.h>
#include <stdio.h>
#include "tree.h"

struct tree *tree_ctor(void)
{
        struct tree *tree = (struct tree*) calloc(1, sizeof(struct tree));
        if (!tree) {
                fprintf(stderr, "error: couldn't allocate memory\n");
                exit(1);
        }
        return tree;
}

void tree_dtor(struct tree *tree)
{
        subtree_dtor(tree->root);
        free(tree);
        tree = NULL;
}

void subtree_dtor(struct node *node)
{
        if (!node)
                return;
        subtree_dtor(node->left);
        subtree_dtor(node->right);
        node_dtor(node);
}

struct node *node_ctor(enum val_type type, double val,
                struct node *left, struct node *right)
{
        struct node *node = (struct node*) calloc(1, sizeof(struct node));
        if (!node) {
                fprintf(stderr, "error: couldn't allocate memory\n");
                exit(1);
        }

        node->type = type;
        node->val = val;
        node->left = left;
        node->right = right;

        return node;
}

void node_dtor(struct node *node)
{
        free(node);
        node = NULL;
}

struct node *subtree_copy(struct node *node)
{
        if (!node)
                return NULL;

        struct node *copy = node_ctor(node->type, node->val,
                        subtree_copy(node->left), subtree_copy(node->right));

        return copy;
}
