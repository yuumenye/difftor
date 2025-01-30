#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "tree.h"
#include "dsl.h"
#include "read.h"

static struct tokens *tokens_ctor(int capacity, int len);
static void tokens_dtor(struct tokens *tokens);
static struct tokens *tokenize(char *buf);
static void token_add(struct tokens *tokens, char *token, int index);
static void tokens_grow(struct tokens *tokens);
static void tokens_realloc(struct tokens *tokens, int capacity);

static struct node *get_g(struct tokens *tokens);
static struct node *get_e(struct tokens *tokens);
static struct node *get_t(struct tokens *tokens);
static struct node *get_p(struct tokens *tokens);
static struct node *get_n(struct tokens *tokens);
static struct node *get_v(struct tokens *tokens);
static struct node *get_s(struct tokens *tokens);

struct tree *get_expression_tree(void)
{
        const int buf_size = 1024;
        struct tree *tree = tree_ctor();
        char buf[buf_size] = "";

        if (!fgets(buf, buf_size, stdin)) {
                fprintf(stderr, "error: couldn't read input\n");
                exit(1);
        }
        buf[strcspn(buf, "\n")] = '\0';

        struct tokens *tokens = tokenize(buf);
        tree->root = get_g(tokens);
        tokens_dtor(tokens);

        return tree;
}

static struct tokens *tokenize(char *buf)
{
        const int capacity = 5;
        const int token_len = 10;
        struct tokens *tokens = tokens_ctor(capacity, token_len);

        const char token_seps[] = " ";
        char *token = NULL;
        token = strtok(buf, token_seps);

        int i = 0;
        for (; token != NULL; ++i) {
                token_add(tokens, token, i);
                token = strtok(NULL, token_seps);
        }
        strncpy(tokens->data[i], terminating_ch, tokens->len);

        return tokens;
}

static struct tokens *tokens_ctor(const int capacity, const int len)
{
        struct tokens *tokens = (struct tokens *) calloc(1, sizeof(struct tokens));
        tokens->data = (char **) calloc(capacity, sizeof(char *));

        if (!tokens || !tokens->data) {
                fprintf(stderr, "error: couldn't allocate memory\n");
                exit(1);
        }

        for (int i = 0; i < capacity; ++i) {
                tokens->data[i] = (char *) calloc(len, sizeof(char));

                if (!tokens->data[i]) {
                        fprintf(stderr, "error: couldn't allocate memory\n");
                        exit(1);
                }
        }

        tokens->len = len;
        tokens->capacity = capacity;

        return tokens;
}

static void tokens_dtor(struct tokens *tokens)
{
        for (int i = 0; i < tokens->capacity; ++i) {
                free(tokens->data[i]);
                tokens->data[i] = NULL;
        }

        free(tokens->data);
        tokens->data = NULL;

        free(tokens);
        tokens = NULL;
}

static void token_add(struct tokens *tokens, char *token, int index)
{
        assert(tokens != NULL);

        tokens_grow(tokens);

        strncpy(tokens->data[index], token, tokens->len);

        ++tokens->size;
}

static void tokens_grow(struct tokens *tokens)
{
        assert(tokens != NULL);

        if (tokens->size >= tokens->capacity) {
                const int grow_val = 2;
                tokens_realloc(tokens, tokens->capacity * grow_val);
        }
}

static void tokens_realloc(struct tokens *tokens, int capacity)
{
        assert(tokens != NULL);

        tokens->data = (char **) realloc(tokens->data, capacity * sizeof(char *));

        for (int i = tokens->size; i < capacity; ++i)
                tokens->data[i] = (char *) calloc(tokens->len, sizeof(char *));

        if (!tokens->data) {
                fprintf(stderr, "error: couldn't allocate memory\n");
                exit(1);
        }

        tokens->capacity = capacity;
}

static struct node *get_g(struct tokens *tokens)
{
        struct node *node = get_e(tokens);
        if (strncmp(tokens->data[tokens->pos], terminating_ch, tokens->len) != 0) {
                fprintf(stderr, "get_g: syntax error\n");
                exit(1);
        }
        return node;
}

static struct node *get_e(struct tokens *tokens)
{
        struct node *node = get_t(tokens);
        while (strncmp(tokens->data[tokens->pos], "+", tokens->len) == 0 ||
                        strncmp(tokens->data[tokens->pos], "-", tokens->len) == 0) {
                int op = *tokens->data[tokens->pos];
                ++tokens->pos;
                struct node *node1 = get_t(tokens);
                if (op == '+')
                        node = _ADD(node, node1);
                else
                        node = _SUB(node, node1);
        }
        return node;
}

static struct node *get_t(struct tokens *tokens)
{
        struct node *node = get_s(tokens);
        while (strncmp(tokens->data[tokens->pos], "*", tokens->len) == 0 ||
                        strncmp(tokens->data[tokens->pos], "/", tokens->len) == 0) {
                int op = *tokens->data[tokens->pos];
                ++tokens->pos;
                struct node *node1 = get_s(tokens);
                if (op == '*')
                        node = _MUL(node, node1);
                else
                        node = _DIV(node, node1);
        }
        return node;
}

static struct node *get_p(struct tokens *tokens)
{
        if (strncmp(tokens->data[tokens->pos], "(", tokens->len) == 0) {
                ++tokens->pos;
                struct node *node = get_e(tokens);
                if (strncmp(tokens->data[tokens->pos], ")", tokens->len) != 0) {
                        fprintf(stderr, "get_p: syntax error\n");
                        exit(1);
                }
                ++tokens->pos;
                return node;
        } else {
                struct node *val = get_n(tokens);
                if (!val)
                        return get_v(tokens);
                return val;
        }
}

static struct node *get_n(struct tokens *tokens)
{
        double val = 0;
        char *endptr = NULL;

        val = strtof(tokens->data[tokens->pos], &endptr);

        if (endptr == tokens->data[tokens->pos]) {
                return NULL;
        } else if (*endptr != '\0') {
                fprintf(stderr, "get_n: syntax error\n");
                exit(1);
        } else {
                ++tokens->pos;
                return _NUM(val, NULL, NULL);
        }
}

static struct node *get_v(struct tokens *tokens)
{
        if (strncmp(tokens->data[tokens->pos], "x", tokens->len) != 0) {
                fprintf(stderr, "get_v: syntax error\n");
                exit(1);
        }

        ++tokens->pos;
        return _X(NULL, NULL);
}

static struct node *get_s(struct tokens *tokens)
{
        struct node *node = get_p(tokens);
        while (strncmp(tokens->data[tokens->pos], "^", tokens->len) == 0) {
                int op = *tokens->data[tokens->pos];
                ++tokens->pos;
                struct node *node1 = get_p(tokens);
                node = _POW(node, node1);
        }
        return node;
}
