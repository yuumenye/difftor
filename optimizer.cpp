#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "tree.h"
#include "dsl.h"
#include "draw.h"
#include "optimizer.h"

static struct node *wrap_const(struct node *node);
static struct node *perform_arithmetic(struct node *node);

static struct node *wrap_neutral(struct node *node);
static struct node *remove_neutral(struct node *node);

static struct node *times_one(struct node *node);
static struct node *times_zero(struct node *node);
static struct node *plus_minus_zero(struct node *node);
static struct node *zero_minus(struct node *node);
static struct node *pow_one(struct node *node);
static struct node *pow_zero(struct node *node);
static struct node *div_one(struct node *node);

void tree_optimize(struct tree *tree)
{
        assert(tree != NULL);

        tree_draw(tree);
        tree->root = wrap_const(tree->root);
        tree_draw(tree);
        tree->root = wrap_neutral(tree->root);
        tree_draw(tree);
}

static struct node *wrap_const(struct node *node)
{
        if (!node)
                return NULL;

        struct node *node_left = wrap_const(node->left);

        if (node_left != node->left) {
                subtree_dtor(node->left);
                node->left = node_left;
        }

        struct node *node_right = wrap_const(node->right);

        if (node_right != node->right) {
                subtree_dtor(node->right);
                node->right = node_right;
        }

        return perform_arithmetic(node);
}

static struct node *perform_arithmetic(struct node *node)
{
        if (node->type != OP)
                return node;
        if (node->left->type != NUM || node->right->type != NUM)
                return node;

        int a = node->left->val;
        int b = node->right->val;

        struct node *result = _NUM(0, NULL, NULL);

        switch (node->val) {
                case ADD:
                       result->val = a + b;
                       break;
                case SUB:
                       result->val = a - b;
                       break;
                case MUL:
                       result->val = a * b;
                       break;
                case DIV:
                       result->val = a / b;
                       break;
                case POW:
                       result->val = pow(a, b);
                       break;
                default:
                        fprintf(stderr, "error: unknown operation\n");
                        exit(1);
        }

        return result;
}

static struct node *wrap_neutral(struct node *node)
{
        if (!node)
                return NULL;

        struct node *node_left = wrap_neutral(node->left);

        if (node_left != node->left)
                node->left = node_left;

        struct node *node_right = wrap_neutral(node->right);

        if (node_right != node->right)
                node->right = node_right;

        return remove_neutral(node);
}

static struct node *remove_neutral(struct node *node)
{
        if (node->type != OP)
                return node;

        struct node *result = NULL;

        struct node *(*funcs[])(struct node *) = {times_one, times_zero,
                plus_minus_zero, zero_minus, pow_one, pow_zero, div_one};
        
        int nfuncs = sizeof(funcs)/sizeof(funcs[0]);

        for (int i = 0; i < nfuncs; ++i)
                if ((result = funcs[i](node)) != NULL)
                        break;
        
        if (!result)
                return node;

        return result;
}

static struct node *times_one(struct node *node)
{
        if (node->val != MUL)
                return NULL;

        if (node->left->type == NUM && node->left->val == 1) {
                node_dtor(node->left);
                return node->right;
        }
        else if (node->right->type == NUM && node->right->val == 1) {
                node_dtor(node->right);
                return node->left;
        }

        return NULL;
}

static struct node *times_zero(struct node *node)
{
        if (node->val != MUL)
                return NULL;

        if (node->left->type == NUM && node->left->val == 0) {
                node_dtor(node->left);
                subtree_dtor(node->right);
                return _NUM(0, NULL, NULL);
        } else if (node->right->type == NUM && node->right->val == 0) {
                node_dtor(node->right);
                subtree_dtor(node->left);
                return _NUM(0, NULL, NULL);
        }

        return NULL;
}

static struct node *plus_minus_zero(struct node *node)
{
        if (node->val != ADD || node->val != SUB)
                return NULL;

        if (node->left->type == NUM && node->left->val == 0) {
                node_dtor(node->left);
                return node->right;
        } else if (node->right->type == NUM && node->right->val == 0) {
                node_dtor(node->right);
                return node->left;
        }

        return NULL;
}

static struct node *zero_minus(struct node *node)
{
        if (node->val != SUB)
                return NULL;

        if (node->left->type == NUM && node->left->val == 0) {
                node_dtor(node->left);
                return _MUL(_NUM(-1, NULL, NULL), node->right);
        }
        
        return NULL;
}

static struct node *pow_one(struct node *node)
{
        if (node->val != POW)
                return NULL;
        
        if (node->right->type == NUM && node->right->val == 1) {
                node_dtor(node->right);
                return node->left;
        }

        return NULL;
}

static struct node *pow_zero(struct node *node)
{
        if (node->val != POW)
                return NULL;

        if (node->right->type == NUM && node->right->val == 0) {
                node_dtor(node->right);
                return _NUM(1, NULL, NULL);
        }

        return NULL;
}

static struct node *div_one(struct node *node)
{
        if (node->val != DIV)
                return NULL;

        if (node->right->type == NUM && node->right->val == 1) {
                node_dtor(node->right);
                return node->left;
        }

        return NULL;
}
