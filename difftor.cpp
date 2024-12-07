#include <stdlib.h>
#include <stdio.h>
#include "difftor.h"
#include "tree.h"
#include "draw.h"

static struct node *differentiate(struct node *curr, struct node *last);
static struct node *const_rule(struct node *curr, struct node *last);
static struct node *const_rule_cont(struct node *curr, struct node *last);
static struct node *sum_rule(struct node *curr, struct node *last);
static struct node *sum_rule_cont(struct node *curr, struct node *last);
static struct node *power_rule(struct node *curr, struct node *last);
static struct node *power_rule_cont(struct node *curr, struct node *last);

static struct node *search_var(struct node *node);
void fill_node_params(struct node *node, int type, int val,
                struct node *left, struct node *right, struct node *parent);

struct tree *tree_differentiate(struct tree *tree)
{
        struct tree *dtree = tree_ctor();
        dtree->root = differentiate(tree->root, NULL);
        return dtree;
}

static struct node *differentiate(struct node *curr, struct node *last)
{
        struct node *node = NULL;

        struct node *(*funcs[])(struct node *, struct node *) =
                        {const_rule_cont, sum_rule_cont, power_rule_cont};

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

static struct node *power_rule_cont(struct node *curr, struct node *last)
{
        if (curr->type == OP && curr->val == POW)
                return power_rule(curr, last);
        return NULL;
}

static struct node *const_rule(struct node *curr, struct node *last)
{
        struct node *node = node_ctor();
        fill_node_params(node, NUM, 0, NULL, NULL, last);
        return node;
}

static struct node *sum_rule(struct node *curr, struct node *last)
{
        struct node *node = node_ctor();
        fill_node_params(node, OP, ADD, differentiate(curr->left, node),
                        differentiate(curr->right, node), last);
        return node;
}

static struct node *power_rule(struct node *curr, struct node *last)
{
        struct node *mul = node_ctor();
        struct node *num1 = node_ctor();
        struct node *pow = node_ctor();
        struct node *num2 = node_ctor();
        struct node *var = node_ctor();
        struct node *sub = node_ctor();
        struct node *num3 = node_ctor();

        fill_node_params(mul, OP, MUL, pow, num1, last);
        fill_node_params(num1, NUM, curr->right->val, NULL, NULL, mul);
        fill_node_params(pow, OP, POW, var, sub, mul);
        fill_node_params(var, VAR, curr->left->val, NULL, NULL, pow);
        fill_node_params(sub, OP, SUB, num2, num3, pow);
        fill_node_params(num2, NUM, curr->right->val, NULL, NULL, sub);
        fill_node_params(num3, NUM, 1, NULL, NULL, sub);

        return mul;
}

void fill_node_params(struct node *node, int type, int val,
                struct node *left, struct node *right, struct node *parent)
{
        if (!node)
                return;

        node->type = type;
        node->val = val;
        node->left = left;
        node->right = right;
        node->parent = parent;
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
