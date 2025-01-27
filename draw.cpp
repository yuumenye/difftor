#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tree.h"
#include "draw.h"

static void generate_filename(char *filename, int namelen);
static void compile_dot(char *filename);
static void write_all(FILE *file, struct node *node);
static void write_params(FILE *file);
static void write_nodes(FILE *file, struct node *node);
static void write_num(FILE *file, struct node *node);
static void write_var(FILE *file, struct node *node);
static void write_op(FILE *file, struct node *node);
static void write_edges(FILE *file, struct node *node);

/* draw tree using graphviz */
void tree_draw(struct tree *tree)
{
        const int namelen = 25;
        char filename[namelen] = "";
        generate_filename(filename, namelen);

        FILE *file = fopen(filename, "w");
        write_all(file, tree->root);
        fclose(file);

        compile_dot(filename);
}

static void generate_filename(char *filename, int namelen)
{
        static int filenum = 0;
        snprintf(filename, namelen, "log/tree%03d.dot", filenum++);
}

static void compile_dot(char *filename)
{
        const int cmdlen = 100;
        char cmd[cmdlen] = "";
        snprintf(cmd, cmdlen, "dot -T png %s -o %.11s.png", filename, filename);
        system(cmd);
}

static void write_all(FILE *file, struct node *node)
{
        fprintf(file, "graph grph {\n");
        write_params(file);
        write_nodes(file, node);
        write_edges(file, node);
        fprintf(file, "}\n");
}

static void write_params(FILE *file)
{
        fprintf(file, "  node [\n");
        fprintf(file, "    shape = \"circle\";\n");
        fprintf(file, "    style = \"filled\";\n");
        fprintf(file, "  ]\n");
        fprintf(file, "  edge [label = \" \"];\n");
}

static void write_nodes(FILE *file, struct node *node)
{
        if (!node)
                return;

        switch (node->type) {
                case NUM:
                        write_num(file, node);
                        break;
                case VAR:
                        write_var(file, node);
                        break;
                case OP:
                        write_op(file, node);
                        break;
                default:
                        fprintf(stderr, "error: unknown value type\n");
                        exit(1);
        }

        write_nodes(file, node->left);
        write_nodes(file, node->right);
}

static void write_num(FILE *file, struct node *node)
{
        fprintf(file, "  n%p [label = \"%d\", fillcolor = \"#FFF8DE\"];\n",
                        node, node->val);
}

static void write_var(FILE *file, struct node *node)
{
        fprintf(file, "  n%p [label = \"%c\", fillcolor = \"#C5D3E8\"];\n",
                        node, node->val);
}

static void write_op(FILE *file, struct node *node)
{
        struct op_desc operations[] = {ADD, "+", SUB, "-", MUL, "*", DIV, "/",
                POW, "^"};
        size_t n_operations = sizeof(operations)/sizeof(operations[0]);

        const int namelen = 5;
        char name[namelen] = "";

        for (size_t i = 0; i < n_operations; ++i)
                if (node->val == operations[i].val) {
                        strncpy(name, operations[i].name, namelen);
                        break;
                }

        fprintf(file, "  n%p [label = \"%s\", fillcolor = \"#D0E8C5\"];\n",
                        node, name);
}

static void write_edges(FILE *file, struct node *node)
{
        if (!node)
                return;

        if (node->left)
                fprintf(file, "  n%p -- n%p;\n", node, node->left);
        if (node->right)
                fprintf(file, "  n%p -- n%p;\n", node, node->right);

        write_edges(file, node->left);
        write_edges(file, node->right);
}
