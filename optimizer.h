#include "parser.h"

// Sovle all constant expressions in a tree. Returns 0 if the sub-tree is not optimizable
int solve_constant_exp(Ast* ast);

// Applies all optimizations to an bastract syntax tree
void optimizer(Ast* ast);
