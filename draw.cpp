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
static void write_edges(FILE *file, struct node *node);
static void write_num(FILE *file, struct node *node);
static void write_var(FILE *file, struct node *node);
static void write_op(FILE *file, struct node *node);
static void get_op_name(struct node *node, char *name, const int namelen);

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
        fprintf(file, "}\n");
}

static void write_params(FILE *file)
{
        fprintf(file, "  node [\n");
        fprintf(file, "    shape = \"box\";\n");
        fprintf(file, "    style = \"rounded, filled\";\n");
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
                        break;
        }
        write_edges(file, node);
        write_nodes(file, node->left);
        write_nodes(file, node->right);
}

static void write_edges(FILE *file, struct node *node)
{
        if (!node->parent)
                return;

        if (node->parent->left == node)
                fprintf(file, "  n%p -- n%p;\n", node->parent, node);
        else
                fprintf(file, "  n%p -- n%p;\n", node->parent, node);
}

static void write_num(FILE *file, struct node *node)
{
        fprintf(file, "  n%p [label = \"%d\", fillcolor = \"#FFF8DE\"];\n",
                        node, node->value);
}

static void write_var(FILE *file, struct node *node)
{
        fprintf(file, "  n%p [label = \"%c\", fillcolor = \"#C5D3E8\"];\n",
                        node, node->value);
}

static void write_op(FILE *file, struct node *node)
{
        const int namelen = 10;
        char name[namelen] = "";
        get_op_name(node, name, namelen);

        fprintf(file, "  n%p [label = \"%s\", fillcolor = \"#D0E8C5\"];\n",
                        node, name);
}

static void get_op_name(struct node *node, char *name, const int namelen)
{
        struct op_desc ops[] = {ADD, "+",
                                SUB, "-",
                                MUL, "*",
                                DIV, "/",
                                POW, "^"};
        size_t nops = sizeof(ops)/sizeof(ops[0]); 

        for (size_t i = 0; i < nops; ++i)
                if (node->value == ops[i].val) {
                        strncpy(name, ops[i].name, namelen);
                        break;
                }
}
