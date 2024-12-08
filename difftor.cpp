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
static struct node *differentiate_var(struct node *curr, struct node *last);
static struct node *differentiate_var_cont(struct node *curr, struct node *last);
static struct node *product_rule(struct node *curr, struct node *last);
static struct node *product_rule_cont(struct node *curr, struct node *last);
static struct node *quotient_rule(struct node *curr, struct node *last);
static struct node *quotient_rule_cont(struct node *curr, struct node *last);

static struct node *search_var(struct node *node);

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
                        {const_rule_cont, sum_rule_cont, power_rule_cont,
                         differentiate_var_cont, product_rule_cont,
                         quotient_rule_cont};

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

static struct node *differentiate_var_cont(struct node *curr, struct node *last)
{
        if (curr->type == VAR)
                return differentiate_var(curr, last);
        return NULL;
}

static struct node *product_rule_cont(struct node *curr, struct node *last)
{
        if (curr->type == OP && curr->val == MUL)
                return product_rule(curr, last);
        return NULL;
}

static struct node *quotient_rule_cont(struct node *curr, struct node *last)
{
        if (curr->type == OP && curr->val == DIV)
                return quotient_rule(curr, last);
        return NULL;
}

static struct node *quotient_rule(struct node *curr, struct node *last)
{
        struct node *div = node_ctor();
        struct node *sub = node_ctor();
        struct node *mul1 = node_ctor();
        struct node *mul2 = node_ctor();
        struct node *n1 = copy_subtree(curr->left, mul2);
        struct node *n2 = copy_subtree(curr->right, mul1);
        struct node *d1 = differentiate(curr->left, mul1);
        struct node *d2 = differentiate(curr->right, mul2);
        struct node *pow = node_ctor();
        struct node *num = node_ctor();
        struct node *n3 = copy_subtree(curr->right, pow);

        fill_node_params(div, OP, DIV, sub, pow, last);
        fill_node_params(sub, OP, SUB, mul1, mul2, div);
        fill_node_params(mul1, OP, MUL, d1, n2, sub);
        fill_node_params(mul2, OP, MUL, n1, d2, sub);
        fill_node_params(pow, OP, POW, n3, num, div);
        fill_node_params(num, NUM, 2, NULL, NULL, pow);
        return div;
}

static struct node *product_rule(struct node *curr, struct node *last)
{
        struct node *add = node_ctor();
        struct node *mul1 = node_ctor();
        struct node *mul2 = node_ctor();
        struct node *n1 = copy_subtree(curr->left, mul2);
        struct node *n2 = copy_subtree(curr->right, mul1);
        struct node *d1 = differentiate(curr->left, mul1);
        struct node *d2 = differentiate(curr->right, mul2);

        fill_node_params(add, OP, ADD, mul1, mul2, last);
        fill_node_params(mul1, OP, MUL, d1, n2, add);
        fill_node_params(mul2, OP, MUL, n1, d2, add);
        return add;
}

static struct node *const_rule(struct node *curr, struct node *last)
{
        struct node *node = node_ctor();
        fill_node_params(node, NUM, 0, NULL, NULL, last);
        return node;
}

static struct node *differentiate_var(struct node *curr, struct node *last)
{
        struct node *node = node_ctor();
        fill_node_params(node, NUM, 1, NULL, NULL, last);
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
