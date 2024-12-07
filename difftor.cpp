#include <stdlib.h>
#include <stdio.h>
#include "difftor.h"
#include "tree.h"
#include "draw.h"

static struct node *differentiate(struct node *curr, struct node *last);
static struct node *parse_node(struct node *curr, struct node *last);
static struct node *const_rule(struct node *curr, struct node *last);
static struct node *const_rule_cont(struct node *curr, struct node *last);
static struct node *sum_rule(struct node *curr, struct node *last);
static struct node *sum_rule_cont(struct node *curr, struct node *last);

static struct node *search_var(struct node *node);

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

        struct node *node = parse_node(curr, last);

        node->left = differentiate(curr->left, node);
        node->right = differentiate(curr->right, node);

        return node;
}

static struct node *parse_node(struct node *curr, struct node *last)
{
        struct node *node = NULL;

        struct node *(*funcs[])(struct node *, struct node *) =
                        {const_rule_cont, sum_rule_cont};

        int nfuncs = sizeof(funcs)/sizeof(funcs[0]);

        for (int i = 0; i < nfuncs; ++i)
                if (node = funcs[i](curr, last))
                        break;
        
        if (!node) {
                fprintf(stderr, "error: couldn't parse node\n");
                exit(1);
        }

        return node;
}

static struct node *const_rule_cont(struct node *curr, struct node *last)
{
        if (curr->type == NUM)
                return const_rule(curr, last);

        return NULL;
}

static struct node *sum_rule_cont(struct node *curr, struct node *last)
{
        if (curr->type == OP && curr->val == ADD)
                return sum_rule(curr, last);

        return NULL;
}

static struct node *const_rule(struct node *curr, struct node *last)
{
        struct node *node = node_ctor();

        node->type = NUM;
        node->val = 0;
        node->parent = last;

        return node;
}

static struct node *sum_rule(struct node *curr, struct node *last)
{
        struct node *node = node_ctor();

        node->type = OP;
        node->val = ADD;
        node->parent = last;

        node->left = differentiate(curr->left, node);
        node->right = differentiate(curr->right, node);

        return node;
}

static struct node *search_var(struct node *node)
{
        if (!node)
                return NULL;

        struct node *found = NULL;

        if (node->type == VAR)
                found = node;

        if (found)
                return found;

        found = search_var(node->left);
        if (!found)
                found = search_var(node->right);

        return found;
}
