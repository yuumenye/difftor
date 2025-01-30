#ifndef READ_H
#define READ_H

const char terminating_ch[] = "\0";

struct tokens {
        char **data;
        int len;
        int capacity;
        int size;
        int pos;
};

struct tree *get_expression_tree(void);

#endif
