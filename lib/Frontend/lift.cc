#include <algorithm>
#include <string>
#include <set>
#include "leml.hh"
#include "lift.hh"

void freeVariables(NExpression* exp, std::set<std::string>& fvs, TypeEnv& extEnv, TypeEnv& localEnv, TypeEnv localBound = TypeEnv());

void extendArgs(NExpression *exp, NIdentifier func, std::vector<NExpression*> args, TypeEnv extEnv, TypeEnv localEnv, TypeEnv localBound) {
	if(typeid(*exp) == typeid(NUnaryExpression)) {
		NUnaryExpression *e = reinterpret_cast<NUnaryExpression*>(exp);
		extendArgs(&e->expr, func, args, extEnv, localEnv, localBound);
	} else if(typeid(*exp) == typeid(NBinaryExpression)) {
		NBinaryExpression *e = reinterpret_cast<NBinaryExpression*>(exp);
		extendArgs(&e->lhs, func, args, extEnv, localEnv, localBound);
		extendArgs(&e->rhs, func, args, extEnv, localEnv, localBound);
	} else if(typeid(*exp) == typeid(NCompExpression)) {
		NCompExpression *e = reinterpret_cast<NCompExpression*>(exp);
		extendArgs(&e->lhs, func, args, extEnv, localEnv, localBound);
		extendArgs(&e->rhs, func, args, extEnv, localEnv, localBound);
	} else if(typeid(*exp) == typeid(NIfExpression)) {
		NIfExpression *e = reinterpret_cast<NIfExpression*>(exp);
		extendArgs(&e->cond, func, args, extEnv, localEnv, localBound);
		extendArgs(&e->true_exp, func, args, extEnv, localEnv, localBound);
		extendArgs(&e->false_exp, func, args, extEnv, localEnv, localBound);
	} else if(typeid(*exp) == typeid(NLetExpression)) {
		NLetExpression *e = reinterpret_cast<NLetExpression*>(exp);
		localEnv[e->id.name] = e->t;
		extendArgs(e->assign, func, args, extEnv, localEnv, localBound);
		if(e->id.name != func.name) {
			extendArgs(e->eval, func, args, extEnv, localEnv, localBound);
		}
	} else if(typeid(*exp) == typeid(NLetRecExpression)) {
		NLetRecExpression *e = reinterpret_cast<NLetRecExpression*>(exp);
		localEnv[e->proto->id.name] = e->t;
		for(auto arg: e->proto->args) {
			localEnv[arg->id.name] = arg->t;
		}
		if(e->proto->id.name != func.name) {
			extendArgs(&e->body, func, args, extEnv, localEnv, localBound);
			extendArgs(&e->eval, func, args, extEnv, localEnv, localBound);
			for(auto arg: args) {
				NIdentifier *a = reinterpret_cast<NIdentifier*>(arg);
				if(localEnv.find(a->name) == localEnv.end()) {
					std::set<std::string> fvs;
					freeVariables(e, fvs, extEnv, localEnv, localBound);
				}
			}
		}
	} else if(typeid(*exp) == typeid(NCallExpression)) {
		// extend args of call expression
		NCallExpression *e = reinterpret_cast<NCallExpression*>(exp);

		for(auto arg: *e->args) {
			extendArgs(arg, func, args, extEnv, localEnv, localBound);
		}
		NIdentifier *funid = reinterpret_cast<NIdentifier*>(&e->fun);
		if(funid && funid->name == func.name) {
			e->args->insert(e->args->end(), args.begin(), args.end());
		} else {
			// callee-function is higher-order function
		}
	} else if(typeid(*exp) == typeid(NArrayPutExpression)) {
		NArrayPutExpression *e = reinterpret_cast<NArrayPutExpression*>(exp);
		extendArgs(&e->exp, func, args, extEnv, localEnv, localBound);
	} else if(typeid(*exp) == typeid(NTupleExpression)) {
		NTupleExpression *e = reinterpret_cast<NTupleExpression*>(exp);
		for(auto elem: e->elems) {
			extendArgs(elem, func, args, extEnv, localEnv, localBound);
		}
	} else if(typeid(*exp) == typeid(NLetTupleExpression)) {
		NLetTupleExpression *e = reinterpret_cast<NLetTupleExpression*>(exp);
		extendArgs(&e->exp, func, args, extEnv, localEnv, localBound);
		for(auto let: e->ids) {
			if(let->id.name == func.name) {
				return;
			}
		}
		extendArgs(&e->eval, func, args, extEnv, localEnv, localBound);
	}
}

void freeVariables(NExpression* exp, std::set<std::string>& fvs, TypeEnv& extEnv, TypeEnv& localEnv, TypeEnv localBound) {
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
			args.push_back(arg);
			bind->t = argtype;
			proto->args.push_back(bind);
			e->t->array.push_back(argtype);
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
			freeVariables(arg, fvs, extEnv, env);
		}
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
