#include <stdlib.h>
#include <stdio.h>
#include "difftor.h"
#include "tree.h"
#include "draw.h"

static struct node *differentiate(struct node *curr, struct node *last);
static struct node *parse_node(struct node *curr, struct node *last);
static struct node *const_rule(struct node *curr, struct node *last);
static struct node *sum_rule(struct node *curr, struct node *last);

struct tree *tree_differentiate(struct tree *tree)
{
        struct tree *dtree = tree_ctor();
        dtree->root = differentiate(tree->root, NULL);
        return dtree;
}

static struct node *differentiate(struct node *curr, struct node *last)
{
        if (!curr)
                return NULL;

        struct node *node = NULL;

        node = parse_node(curr, last);

        node->left = differentiate(curr->left, node);
        node->right = differentiate(curr->right, node);

        return node;
}

static struct node *parse_node(struct node *curr, struct node *last)
{
        if (curr->type == NUM)
                return const_rule(curr, last);
        else if (curr->type == OP && curr->value == ADD)
                return sum_rule(curr, last);

        return NULL;
}

static struct node *const_rule(struct node *curr, struct node *last)
{
        struct node *node = node_ctor();

        node->type = NUM;
        node->value = 0;
        node->parent = last;

        return node;
}

static struct node *sum_rule(struct node *curr, struct node *last)
{
        struct node *node = node_ctor();

        node->type = OP;
        node->value = ADD;
        node->parent = last;

        node->left = differentiate(curr->left, node);
        node->right = differentiate(curr->right, node);

        return node;
}
