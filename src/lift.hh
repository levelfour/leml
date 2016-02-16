#ifndef __LIFT_HH__
#define __LIFT_HH__

#include "syntax.hh"
#include "type.hh"

bool lambdaLifting(TypeEnv env, NExpression *program);

#endif // __LIFT_HH__
