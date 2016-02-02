#include <algorithm>
#include <string>
#include <set>
#include "leml.hh"
#include "lift.hh"

void extendArgs(NIdentifier func, std::vector<NExpression*> args, NExpression *exp) {
	if(typeid(*exp) == typeid(NUnaryExpression)) {
		NUnaryExpression *e = reinterpret_cast<NUnaryExpression*>(exp);
		extendArgs(func, args, &e->expr);
	} else if(typeid(*exp) == typeid(NBinaryExpression)) {
		NBinaryExpression *e = reinterpret_cast<NBinaryExpression*>(exp);
		extendArgs(func, args, &e->lhs);
		extendArgs(func, args, &e->rhs);
	} else if(typeid(*exp) == typeid(NCompExpression)) {
		NCompExpression *e = reinterpret_cast<NCompExpression*>(exp);
		extendArgs(func, args, &e->lhs);
		extendArgs(func, args, &e->rhs);
	} else if(typeid(*exp) == typeid(NIfExpression)) {
		NIfExpression *e = reinterpret_cast<NIfExpression*>(exp);
		extendArgs(func, args, &e->cond);
		extendArgs(func, args, &e->true_exp);
		extendArgs(func, args, &e->false_exp);
	} else if(typeid(*exp) == typeid(NLetExpression)) {
		NLetExpression *e = reinterpret_cast<NLetExpression*>(exp);
		extendArgs(func, args, e->assign);
		if(e->id.name != func.name) {
			extendArgs(func, args, e->eval);
		}
	} else if(typeid(*exp) == typeid(NLetRecExpression)) {
		NLetRecExpression *e = reinterpret_cast<NLetRecExpression*>(exp);
		if(e->proto->id.name != func.name) {
			extendArgs(func, args, &e->body);
			extendArgs(func, args, &e->eval);
		}
	} else if(typeid(*exp) == typeid(NCallExpression)) {
		// extend args of call expression
		NCallExpression *e = reinterpret_cast<NCallExpression*>(exp);
		for(auto arg: *e->args) {
			extendArgs(func, args, arg);
		}
		// TODO: higher-order function
		NIdentifier *funid = reinterpret_cast<NIdentifier*>(&e->fun);
		if(funid && funid->name == func.name) {
			e->args->insert(e->args->end(), args.begin(), args.end());
		}
	} else if(typeid(*exp) == typeid(NArrayPutExpression)) {
		NArrayPutExpression *e = reinterpret_cast<NArrayPutExpression*>(exp);
		extendArgs(func, args, &e->exp);
	} else if(typeid(*exp) == typeid(NTupleExpression)) {
		NTupleExpression *e = reinterpret_cast<NTupleExpression*>(exp);
		for(auto elem: e->elems) {
			extendArgs(func, args, elem);
		}
	} else if(typeid(*exp) == typeid(NLetTupleExpression)) {
		NLetTupleExpression *e = reinterpret_cast<NLetTupleExpression*>(exp);
		extendArgs(func, args, &e->exp);
		for(auto let: e->ids) {
			if(let->id.name == func.name) {
				return;
			}
		}
		extendArgs(func, args, &e->eval);
	}
}

void freeVariables(NExpression* exp, std::set<std::string>& fvs, TypeEnv& extEnv, TypeEnv& localEnv, TypeEnv localBound = TypeEnv()) {
	TypeEnv env = localEnv;
	if(typeid(*exp) != typeid(NLetRecExpression) && !localBound.empty()) {
		env = localBound;
	}

	if(typeid(*exp) == typeid(NUnit)) {
		return;
	} else if(typeid(*exp) == typeid(NBoolean)) {
		return;
	} else if(typeid(*exp) == typeid(NInteger)) {
		return;
	} else if(typeid(*exp) == typeid(NFloat)) {
		return;
	} else if(typeid(*exp) == typeid(NUnaryExpression)) {
		NUnaryExpression *e = reinterpret_cast<NUnaryExpression*>(exp);
		freeVariables(&e->expr, fvs, extEnv, env);
	} else if(typeid(*exp) == typeid(NBinaryExpression)) {
		NBinaryExpression *e = reinterpret_cast<NBinaryExpression*>(exp);
		freeVariables(&e->lhs, fvs, extEnv, env);
		freeVariables(&e->rhs, fvs, extEnv, env);
	} else if(typeid(*exp) == typeid(NCompExpression)) {
		NCompExpression *e = reinterpret_cast<NCompExpression*>(exp);
		freeVariables(&e->lhs, fvs, extEnv, env);
		freeVariables(&e->rhs, fvs, extEnv, env);
	} else if(typeid(*exp) == typeid(NIfExpression)) {
		NIfExpression *e = reinterpret_cast<NIfExpression*>(exp);
		freeVariables(&e->true_exp, fvs, extEnv, env);
		freeVariables(&e->false_exp, fvs, extEnv, env);
		freeVariables(&e->cond, fvs, extEnv, env);
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
		extEnv[e->proto->id.name] = e->t;

		// bound function and args to environment
		TypeEnv bound = localBound;
		bound[e->proto->id.name] = e->t;
		for(auto arg: e->proto->args) {
			bound[arg->id.name] = arg->t;
		}

		// search free variables in let rec body (independently from former environment)
		std::set<std::string> clsFvs;
		freeVariables(&e->body, clsFvs, extEnv, env, bound);
		fvs.insert(clsFvs.begin(), clsFvs.end());

		if(verbose) {
			std::cerr << "scan freevars in function `" << e->proto->id.name << "`\n";
		}

		// add fvs to args
		NFundefExpression *proto = e->proto;
		std::vector<NExpression*> args;
		for(auto fv: clsFvs) {
			NIdentifier *arg = new NIdentifier(fv);
			NLetExpression *bind = new NLetExpression(*arg);
			// free variable must be defined in the outer environment
			LemlType *argtype = env[fv];
#ifdef LEML_DEBUG
			if(argtype == nullptr) {
				std::cerr << "error: reference `" << fv << "` is not defined" << std::endl;
			}
			assert(argtype != nullptr);
#endif
			args.push_back(arg);
			bind->t = argtype;
			proto->args.push_back(bind);
			e->t->array.push_back(argtype);
			if(verbose) {
				std::cerr << " * " << arg->name << ": " << *argtype << std::endl;
			}
		}

		// add new args to function calls in let rec eval recursively
		extendArgs(proto->id, args, &e->body);
		extendArgs(proto->id, args, &e->eval);

		// continue lambda lifting in let rec eval using former environment
		TypeEnv newEnv = localEnv;
		newEnv[e->proto->id.name] = e->t;
		freeVariables(&e->eval, fvs, extEnv, newEnv, localBound);
		fvs.erase(e->proto->id.name);
	} else if(typeid(*exp) == typeid(NCallExpression)) {
		NCallExpression *e = reinterpret_cast<NCallExpression*>(exp);
		for(auto arg: *e->args) {
			freeVariables(arg, fvs, extEnv, env);
		}
		// TODO: check higher-order function
		freeVariables(&e->fun, fvs, extEnv, env);
	} else if(typeid(*exp) == typeid(NArrayExpression)) {
		return;
	} else if(typeid(*exp) == typeid(NArrayGetExpression)) {
		NArrayGetExpression *e = reinterpret_cast<NArrayGetExpression*>(exp);
		freeVariables(&e->array, fvs, extEnv, env);
		freeVariables(&e->index, fvs, extEnv, env);
	} else if(typeid(*exp) == typeid(NArrayPutExpression)) {
		NArrayPutExpression *e = reinterpret_cast<NArrayPutExpression*>(exp);
		freeVariables(&e->array, fvs, extEnv, env);
		freeVariables(&e->index, fvs, extEnv, env);
		freeVariables(&e->exp, fvs, extEnv, env);
	} else if(typeid(*exp) == typeid(NTupleExpression)) {
		NTupleExpression *e = reinterpret_cast<NTupleExpression*>(exp);
		for(auto elem: e->elems) {
			freeVariables(elem, fvs, extEnv, env);
		}
	} else if(typeid(*exp) == typeid(NLetTupleExpression)) {
		NLetTupleExpression *e = reinterpret_cast<NLetTupleExpression*>(exp);
		freeVariables(&e->eval, fvs, extEnv, env);
		for(auto let: e->ids) {
			fvs.erase(let->id.name);
		}
		freeVariables(&e->exp, fvs, extEnv, env);
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
