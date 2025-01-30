#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tree.h"
#include "dsl.h"
#include "read.h"

const int buf_size = 1024;
int n_tokens = 50;
const int token_len = 10;
const char terminating_ch[] = "\0";

static char **tokens_ctor(void);
static void tokens_dtor(char **tokens);
static char **tokenize(char *buf);

static struct node *get_g(char **tokens, int *pos);
static struct node *get_e(char **tokens, int *pos);
static struct node *get_t(char **tokens, int *pos);
static struct node *get_p(char **tokens, int *pos);
static struct node *get_n(char **tokens, int *pos);
static struct node *get_v(char **tokens, int *pos);
static struct node *get_s(char **tokens, int *pos);

struct tree *get_expression_tree(void)
{
        struct tree *tree = tree_ctor();
        char buf[buf_size] = "";

        if (!fgets(buf, buf_size, stdin)) {
                fprintf(stderr, "error: couldn't read input\n");
                exit(1);
        }
        buf[strcspn(buf, "\n")] = '\0';

        char **tokens = tokenize(buf);

        int pos = 0;
        tree->root = get_g(tokens, &pos);

        tokens_dtor(tokens);

        return tree;
}

static char **tokenize(char *buf)
{
        char *token = NULL;
        const char token_seps[] = " ";

        char **tokens = tokens_ctor();

        token = strtok(buf, token_seps);

        int i = 0;
        for (; token != NULL; ++i) {
                strncpy(tokens[i], token, token_len);
                token = strtok(NULL, token_seps);
        }
        strncpy(tokens[i], terminating_ch, token_len);

        return tokens;
}

static char **tokens_ctor(void)
{
        char **tokens = (char **) calloc(n_tokens, sizeof(char *));

        if (!tokens) {
                fprintf(stderr, "error: couldn't allocate memory\n");
                exit(1);
        }

        for (int i = 0; i < n_tokens; ++i) {
                tokens[i] = (char *) calloc(token_len, sizeof(char));

                if (!tokens[i]) {
                        fprintf(stderr, "error: couldn't allocate memory\n");
                        exit(1);
                }
        }

        return tokens;
}

static void tokens_dtor(char **tokens)
{
        for (int i = 0; i < n_tokens; ++i) {
                free(tokens[i]);
                tokens[i] = NULL;
        }

        free(tokens);
        tokens = NULL;
}

static struct node *get_g(char **tokens, int *pos)
{
        struct node *node = get_e(tokens, pos);
        if (strncmp(tokens[*pos], terminating_ch, token_len) != 0) {
                fprintf(stderr, "get_g: syntax error\n");
                exit(1);
        }
        return node;
}

static struct node *get_e(char **tokens, int *pos)
{
        struct node *node = get_t(tokens, pos);
        while (strncmp(tokens[*pos], "+", token_len) == 0 || strncmp(tokens[*pos], "-", token_len) == 0) {
                int op = *tokens[*pos];
                ++*pos;
                struct node *node1 = get_t(tokens, pos);
                if (op == '+')
                        node = _ADD(node, node1);
                else
                        node = _SUB(node, node1);
        }
        return node;
}

static struct node *get_t(char **tokens, int *pos)
{
        struct node *node = get_s(tokens, pos);
        while (strncmp(tokens[*pos], "*", token_len) == 0 || strncmp(tokens[*pos], "/", token_len) == 0) {
                int op = *tokens[*pos];
                ++*pos;
                struct node *node1 = get_s(tokens, pos);
                if (op == '*')
                        node = _MUL(node, node1);
                else
                        node = _DIV(node, node1);
        }
        return node;
}

static struct node *get_p(char **tokens, int *pos)
{
        if (strncmp(tokens[*pos], "(", token_len) == 0) {
                ++*pos;
                struct node *node = get_e(tokens, pos);
                if (strncmp(tokens[*pos], ")", token_len) != 0) {
                        fprintf(stderr, "get_p: syntax error\n");
                        exit(1);
                }
                ++*pos;
                return node;
        } else {
                struct node *val = get_n(tokens, pos);
                if (!val)
                        return get_v(tokens, pos);
                return val;
        }
}

static struct node *get_n(char **tokens, int *pos)
{
        double val = 0;
        char *endptr = NULL;

        val = strtof(tokens[*pos], &endptr);

        if (endptr == tokens[*pos]) {
                return NULL;
        } else if (*endptr != '\0') {
                fprintf(stdout, "get_n: syntax error\n");
                exit(1);
        } else {
                ++*pos;
                return _NUM(val, NULL, NULL);
        }
}

static struct node *get_v(char **tokens, int *pos)
{
        if (strncmp(tokens[*pos], "x", token_len) != 0) {
                fprintf(stderr, "get_v: syntax error\n");
                exit(1);
        }

        ++*pos;
        return _X(NULL, NULL);
}

static struct node *get_s(char **tokens, int *pos)
{
        struct node *node = get_p(tokens, pos);
        while (strncmp(tokens[*pos], "^", token_len) == 0) {
                int op = *tokens[*pos];
                ++*pos;
                struct node *node1 = get_p(tokens, pos);
                node = _POW(node, node1);
        }
        return node;
}
