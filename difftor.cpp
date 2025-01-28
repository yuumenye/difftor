/*
 * difftor - differentiate expressions
 */

#include <stdlib.h>
#include <stdio.h>
#include "dsl.h"
#include "draw.h"
#include "tree.h"
#include "difftor.h"

static struct node *differentiate(struct node *curr);

static struct node *const_rule_cont(struct node *curr);
static struct node *const_rule(struct node *curr);

static struct node *sum_rule_cont(struct node *curr);
static struct node *sum_rule(struct node *curr);

static struct node *differentiate_var_cont(struct node *curr);
static struct node *differentiate_var(struct node *curr);

static struct node *power_rule_cont(struct node *curr);
static struct node *power_rule(struct node *curr);

static struct node *product_rule_cont(struct node *curr);
static struct node *product_rule(struct node *curr);

static struct node *quotient_rule_cont(struct node *curr);
static struct node *quotient_rule(struct node *curr);

struct tree *tree_differentiate(struct tree *tree)
{
        struct tree *diffed_tree = tree_ctor();
        diffed_tree->root = differentiate(tree->root);
        return diffed_tree;
}

static struct node *differentiate(struct node *curr)
{
        struct node *node = NULL;

        struct node *(*funcs[])(struct node *) = {
                const_rule_cont, differentiate_var_cont, sum_rule_cont,
        power_rule_cont, product_rule_cont, quotient_rule_cont};
        
        int nfuncs = sizeof(funcs)/sizeof(funcs[0]);

        for (int i = 0; i < nfuncs; ++i)
                if (node = funcs[i](curr))
                        break;
        
        if (!node) {
                fprintf(stderr, "error: couldn't parse node\n");
                exit(1);
        }

        return node;
}

static struct node *const_rule_cont(struct node *curr)
{
        if (curr->type == NUM)
                return const_rule(curr);

        return NULL;
}

static struct node *const_rule(struct node *curr)
{
        struct node *node = _NUM(0, NULL, NULL);
        return node;
}

static struct node *differentiate_var_cont(struct node *curr)
{
        if (curr->type == VAR)
                return differentiate_var(curr);

        return NULL;
}

static struct node *differentiate_var(struct node *curr)
{
        struct node *node = _NUM(1, NULL, NULL);
        return node;
}

static struct node *sum_rule_cont(struct node *curr)
{
        if (curr->type == OP && (curr->val == ADD || curr->val == SUB))
                return sum_rule(curr);

        return NULL;
}

static struct node *sum_rule(struct node *curr)
{
        struct node *node = NULL;

        if (curr->val == ADD)
                node = _ADD(differentiate(curr->left), differentiate(curr->right));
        else
                node = _SUB(differentiate(curr->left), differentiate(curr->right));

        return node;
}

static struct node *power_rule_cont(struct node *curr)
{
        if (curr->type == OP && curr->val == POW)
                return power_rule(curr);

        return NULL;
}

static struct node *power_rule(struct node *curr)
{
        int n = curr->right->val;
        struct node *node = _MUL(_NUM(n, NULL, NULL), _MUL(_POW(subtree_copy(curr->left),
                                        _SUB(_NUM(n, NULL, NULL), _NUM(1, NULL, NULL))),
                                differentiate(curr->left)));
        return node;
}

static struct node *product_rule_cont(struct node *curr)
{
        if (curr->type == OP && curr->val == MUL)
                return product_rule(curr);

        return NULL;
}

static struct node *product_rule(struct node *curr)
{
        struct node *u = curr->left;
        struct node *v = curr->right;

        struct node *node = _ADD(_MUL(differentiate(u), subtree_copy(v)),
                        _MUL(subtree_copy(u), differentiate(v)));

        return node;
}

static struct node *quotient_rule_cont(struct node *curr)
{
        if (curr->type == OP && curr->val == DIV)
                return quotient_rule(curr);

        return NULL;
}

static struct node *quotient_rule(struct node *curr)
{
        struct node *u = curr->left;
        struct node *v = curr->right;

        struct node *node = _DIV(_SUB(_MUL(differentiate(u), subtree_copy(v)),
                                _MUL(subtree_copy(u), differentiate(v))),
                        _POW(v, _NUM(2, NULL, NULL)));

        return node;
}
