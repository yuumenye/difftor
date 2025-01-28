#ifndef DSL_H
#define DSL_H

#define _X(left, right) node_ctor(VAR, 'x', left, right)

#define _ADD(left, right) node_ctor(OP, ADD, left, right)
#define _SUB(left, right) node_ctor(OP, SUB, left, right)
#define _MUL(left, right) node_ctor(OP, MUL, left, right)
#define _DIV(left, right) node_ctor(OP, DIV, left, right)
#define _POW(left, right) node_ctor(OP, POW, left, right)

#define _NUM(val, left, right) node_ctor(NUM, val, left, right)

#endif
