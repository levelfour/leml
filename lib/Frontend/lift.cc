#include <algorithm>
#include <cstring>
#include <stack>
#include <set>
#include "leml.hh"
#include "lift.hh"

void freeVariables(NExpression* exp, std::set<std::string>& fvs, TypeEnv& extEnv, TypeEnv& localEnv, TypeEnv localBound = TypeEnv());

void extendArgs(NExpression *exp, NIdentifier func, std::vector<NExpression*> args, TypeEnv extEnv, TypeEnv localEnv, TypeEnv localBound) {
	if(args.size() == 0) return;

	std::stack<NExpression*> stackExp;
	stackExp.push(exp);

	// pseudo-recursion on expression using stack (partially)
	while(!stackExp.empty()) {
		NExpression *exp = stackExp.top(); stackExp.pop();

		if(typeid(*exp) == typeid(NIdentifier)) {
			NIdentifier *e = reinterpret_cast<NIdentifier*>(exp);
			if(e->name == func.name) {
				std::vector<NExpression*> *newArgs = new std::vector<NExpression*>(args);
				NCallExpression call(func, newArgs);
				std::memcpy(static_cast<void*>(exp), static_cast<void*>(&call), sizeof(NCallExpression));
			}
		} else if(typeid(*exp) == typeid(NUnaryExpression)) {
			NUnaryExpression *e = reinterpret_cast<NUnaryExpression*>(exp);
			stackExp.push(&e->expr);
		} else if(typeid(*exp) == typeid(NBinaryExpression)) {
			NBinaryExpression *e = reinterpret_cast<NBinaryExpression*>(exp);
			stackExp.push(&e->lhs);
			stackExp.push(&e->rhs);
		} else if(typeid(*exp) == typeid(NCompExpression)) {
			NCompExpression *e = reinterpret_cast<NCompExpression*>(exp);
			stackExp.push(&e->lhs);
			stackExp.push(&e->rhs);
		} else if(typeid(*exp) == typeid(NIfExpression)) {
			NIfExpression *e = reinterpret_cast<NIfExpression*>(exp);
			stackExp.push(&e->cond);
			stackExp.push(&e->true_exp);
			stackExp.push(&e->false_exp);
		} else if(typeid(*exp) == typeid(NLetExpression)) {
			NLetExpression *e = reinterpret_cast<NLetExpression*>(exp);
			stackExp.push(e->assign);
			if(e->id.name != func.name) {
				TypeEnv localEnv1 = localEnv;
				localEnv1[e->id.name] = e->t;
				extendArgs(e->eval, func, args, extEnv, localEnv1, localBound);
			}
		} else if(typeid(*exp) == typeid(NLetRecExpression)) {
			NLetRecExpression *e = reinterpret_cast<NLetRecExpression*>(exp);
			TypeEnv localEnv1 = localEnv;
			localEnv1[e->proto->id.name] = e->t;
			for(auto arg: e->proto->args) {
				localEnv1[arg->id.name] = arg->t;
			}
			if(e->proto->id.name != func.name) {
				extendArgs(&e->body, func, args, extEnv, localEnv1, localBound);
				extendArgs(&e->eval, func, args, extEnv, localEnv1, localBound);
				for(auto arg: args) {
					NIdentifier *a = reinterpret_cast<NIdentifier*>(arg);
					if(localEnv1.find(a->name) == localEnv1.end()) {
						std::set<std::string> fvs;
						freeVariables(e, fvs, extEnv, localEnv1, localBound);
					}
				}
			}
		} else if(typeid(*exp) == typeid(NCallExpression)) {
			// extend args of call expression
			NCallExpression *e = reinterpret_cast<NCallExpression*>(exp);

			NIdentifier *funid = reinterpret_cast<NIdentifier*>(&e->fun);
			if(funid && funid->name == func.name) {
				// push additional args in front of original args
				for(auto it = args.rbegin(); it != args.rend(); it++) {
					e->args->insert(e->args->begin(), *it);
				}
			} else {
				// callee-function is higher-order function
			}
			for(auto arg: *e->args) {
				stackExp.push(arg);
			}
		} else if(typeid(*exp) == typeid(NArrayExpression)) {
			NArrayExpression *e = reinterpret_cast<NArrayExpression*>(exp);
			stackExp.push(&e->length);
			stackExp.push(&e->data);
		} else if(typeid(*exp) == typeid(NArrayGetExpression)) {
			NArrayGetExpression *e = reinterpret_cast<NArrayGetExpression*>(exp);
			stackExp.push(&e->array);
			stackExp.push(&e->index);
		} else if(typeid(*exp) == typeid(NArrayPutExpression)) {
			NArrayPutExpression *e = reinterpret_cast<NArrayPutExpression*>(exp);
			stackExp.push(&e->array);
			stackExp.push(&e->index);
			stackExp.push(&e->exp);
		} else if(typeid(*exp) == typeid(NTupleExpression)) {
			NTupleExpression *e = reinterpret_cast<NTupleExpression*>(exp);
			for(auto elem: e->elems) {
				stackExp.push(elem);
			}
		} else if(typeid(*exp) == typeid(NLetTupleExpression)) {
			NLetTupleExpression *e = reinterpret_cast<NLetTupleExpression*>(exp);
			extendArgs(&e->exp, func, args, extEnv, localEnv, localBound);
			for(auto let: e->ids) {
				if(let->id.name == func.name) {
					continue;
				}
			}
			stackExp.push(&e->eval);
		}
	}
}

void freeVariables(NExpression* exp, std::set<std::string>& fvs, TypeEnv& extEnv, TypeEnv& localEnv, TypeEnv localBound) {
	std::stack<NExpression*> stackExp;
	stackExp.push(exp);

	// pseudo-recursion on expression using stack (partially)
	while(!stackExp.empty()) {
		NExpression *exp = stackExp.top(); stackExp.pop();
		TypeEnv env = localEnv;
		if(typeid(*exp) != typeid(NLetRecExpression) && !localBound.empty()) {
			env = localBound;
		}

		if(typeid(*exp) == typeid(NUnit)) {
			continue;
		} else if(typeid(*exp) == typeid(NBoolean)) {
			continue;
		} else if(typeid(*exp) == typeid(NInteger)) {
			continue;
		} else if(typeid(*exp) == typeid(NFloat)) {
			continue;
		} else if(typeid(*exp) == typeid(NUnaryExpression)) {
			NUnaryExpression *e = reinterpret_cast<NUnaryExpression*>(exp);
			stackExp.push(&e->expr);
		} else if(typeid(*exp) == typeid(NBinaryExpression)) {
			NBinaryExpression *e = reinterpret_cast<NBinaryExpression*>(exp);
			stackExp.push(&e->lhs);
			stackExp.push(&e->rhs);
		} else if(typeid(*exp) == typeid(NCompExpression)) {
			NCompExpression *e = reinterpret_cast<NCompExpression*>(exp);
			stackExp.push(&e->lhs);
			stackExp.push(&e->rhs);
		} else if(typeid(*exp) == typeid(NIfExpression)) {
			NIfExpression *e = reinterpret_cast<NIfExpression*>(exp);
			stackExp.push(&e->true_exp);
			stackExp.push(&e->false_exp);
			stackExp.push(&e->cond);
		} else if(typeid(*exp) == typeid(NLetExpression)) {
			NLetExpression *e = reinterpret_cast<NLetExpression*>(exp);
			TypeEnv newEnv = env;
			if(e->assign && e->eval) {
				newEnv[e->id.name] = e->t;
				freeVariables(e->eval, fvs, extEnv, newEnv);
				fvs.erase(e->id.name);
				freeVariables(e->assign, fvs, extEnv, newEnv);
			}
		} else if(typeid(*exp) == typeid(NIdentifier)) {
			NIdentifier *e = reinterpret_cast<NIdentifier*>(exp);
			if(extEnv.find(e->name) == extEnv.end() && env.find(e->name) == env.end()) {
				// not found => add as a new free variable
				fvs.insert(e->name);
			}
		} else if(typeid(*exp) == typeid(NLetRecExpression)) {
			NLetRecExpression *e = reinterpret_cast<NLetRecExpression*>(exp);

			// add function to extEnv (function is not to be regarded as free variable)
			// TODO: avoid confliction between function name (just renaming)
			extEnv[e->proto->id.name] = e->t;

			// bound function and args to environment
			TypeEnv bound;
			bound[e->proto->id.name] = e->t;
			for(auto arg: e->proto->args) {
				bound[arg->id.name] = arg->t;
			}

			TypeEnv catEnv = localEnv; // localEnv + localBound
			catEnv.insert(localBound.begin(), localBound.end());

			// search free variables in let rec body (independently from former environment)
			std::set<std::string> clsFvs;
			freeVariables(&e->body, clsFvs, extEnv, catEnv, bound);
			fvs.insert(clsFvs.begin(), clsFvs.end());

			if(gVerbose) {
				std::cerr << "scan freevars in function `" << e->proto->id.name << "`\n";
			}

			// add fvs to args
			NFundefExpression *proto = e->proto;
			std::vector<NExpression*> args;
			for(auto fv: clsFvs) {
				if(bound.find(fv) != bound.end()) {
					// if fv is contained in local bound, it's bounded
					continue;
				}
				NIdentifier *arg = new NIdentifier(fv);
				NLetExpression *bind = new NLetExpression(*arg);
				// free variable must be defined in the outer environment
				LemlType *argtype = catEnv[fv];
#ifdef LEML_DEBUG
				if(argtype == nullptr) {
					std::cerr << "error: reference `" << fv << "` is not defined" << std::endl;
				}
				assert(argtype != nullptr);
#endif
				args.insert(args.begin(), arg);
				bind->t = argtype;
				proto->args.insert(proto->args.begin(), bind);
				e->t->array.insert(e->t->array.begin(), argtype);
				if(gVerbose) {
					std::cerr << " * " << arg->name << ": " << *argtype << std::endl;
				}
			}

			extEnv[e->proto->id.name] = e->t;

			// add new args to function calls in let rec eval recursively
			TypeEnv newLocalEnv = env;
			for(auto fv: clsFvs) {
				newLocalEnv.erase(fv);
				bound[fv] = env[fv];
			}
			extendArgs(&e->body, proto->id, args, extEnv, newLocalEnv, bound);
			extendArgs(&e->eval, proto->id, args, extEnv, newLocalEnv, bound);

			// continue lambda lifting in let rec eval using former environment
			TypeEnv newEnv = localEnv;
			newEnv[e->proto->id.name] = e->t;
			freeVariables(&e->eval, fvs, extEnv, newEnv, localBound);
			fvs.erase(e->proto->id.name);
		} else if(typeid(*exp) == typeid(NCallExpression)) {
			NCallExpression *e = reinterpret_cast<NCallExpression*>(exp);
			for(auto arg: *e->args) {
				stackExp.push(arg);
			}
			stackExp.push(&e->fun);
		} else if(typeid(*exp) == typeid(NArrayExpression)) {
			NArrayExpression *e = reinterpret_cast<NArrayExpression*>(exp);
			stackExp.push(&e->length);
			stackExp.push(&e->data);
		} else if(typeid(*exp) == typeid(NArrayGetExpression)) {
			NArrayGetExpression *e = reinterpret_cast<NArrayGetExpression*>(exp);
			stackExp.push(&e->array);
			stackExp.push(&e->index);
		} else if(typeid(*exp) == typeid(NArrayPutExpression)) {
			NArrayPutExpression *e = reinterpret_cast<NArrayPutExpression*>(exp);
			stackExp.push(&e->array);
			stackExp.push(&e->index);
			stackExp.push(&e->exp);
		} else if(typeid(*exp) == typeid(NTupleExpression)) {
			NTupleExpression *e = reinterpret_cast<NTupleExpression*>(exp);
			for(auto elem: e->elems) {
				stackExp.push(elem);
			}
		} else if(typeid(*exp) == typeid(NLetTupleExpression)) {
			NLetTupleExpression *e = reinterpret_cast<NLetTupleExpression*>(exp);
			freeVariables(&e->eval, fvs, extEnv, env);
			for(auto let: e->ids) {
				fvs.erase(let->id.name);
			}
			stackExp.push(&e->exp);
		}
	}
}

bool lambdaLifting(TypeEnv extEnv, NExpression *program) {
	// environment mapping variable to its type
	TypeEnv localEnv;

	// free variable set
	std::set<std::string> fvs;

	// the instance of lambda lifting
	freeVariables(program, fvs, extEnv, localEnv);

	return true;
}
