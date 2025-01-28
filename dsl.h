#ifndef DSL_H
#define DSL_H

#define _X(left, right) node_ctor(VAR, 'x', left, right)

#define _ADD(left, right) node_ctor(OP, ADD, left, right)
#define _SUB(left, right) node_ctor(OP, SUB, left, right)
#define _MUL(left, right) node_ctor(OP, MUL, left, right)
#define _DIV(left, right) node_ctor(OP, DIV, left, right)
#define _POW(left, right) node_ctor(OP, POW, left, right)

#define _SIN(arg) node_ctor(OP, SIN, arg, NULL)
#define _COS(arg) node_ctor(OP, COS, arg, NULL)
#define _TAN(arg) node_ctor(OP, TAN, arg, NULL)
#define _ASIN(arg) node_ctor(OP, ASIN, arg, NULL)
#define _ACOS(arg) node_ctor(OP, ACOS, arg, NULL)
#define _ATAN(arg) node_ctor(OP, ATAN, arg, NULL)
#define _SINH(arg) node_ctor(OP, SINH, arg, NULL)
#define _COSH(arg) node_ctor(OP, COSH, arg, NULL)
#define _TANH(arg) node_ctor(OP, TANH, arg, NULL)

#define _NUM(val, left, right) node_ctor(NUM, val, left, right)

#endif
