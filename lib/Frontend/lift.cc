#include <algorithm>
#include <string>
#include <set>
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
		// TODO: higher-order function
		NIdentifier funid = *reinterpret_cast<NIdentifier*>(&e->fun);
		if(funid.name == func.name) {
			(*e->args).insert(e->args->end(), args.begin(), args.end());
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

void freeVariables(std::set<std::string>& fvs, TypeEnv& env, NExpression* exp) {
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
		freeVariables(fvs, env, &e->expr);
	} else if(typeid(*exp) == typeid(NBinaryExpression)) {
		NBinaryExpression *e = reinterpret_cast<NBinaryExpression*>(exp);
		freeVariables(fvs, env, &e->lhs);
		freeVariables(fvs, env, &e->rhs);
	} else if(typeid(*exp) == typeid(NCompExpression)) {
		NCompExpression *e = reinterpret_cast<NCompExpression*>(exp);
		freeVariables(fvs, env, &e->lhs);
		freeVariables(fvs, env, &e->rhs);
	} else if(typeid(*exp) == typeid(NIfExpression)) {
		NIfExpression *e = reinterpret_cast<NIfExpression*>(exp);
		freeVariables(fvs, env, &e->true_exp);
		freeVariables(fvs, env, &e->false_exp);
		freeVariables(fvs, env, &e->cond);
	} else if(typeid(*exp) == typeid(NLetExpression)) {
		NLetExpression *e = reinterpret_cast<NLetExpression*>(exp);
		TypeEnv newEnv = env;
		env[e->id.name] = e->t;
		freeVariables(fvs, env, e->eval);
		fvs.erase(e->id.name);
		freeVariables(fvs, env, e->assign);
	} else if(typeid(*exp) == typeid(NIdentifier)) {
		NIdentifier *e = reinterpret_cast<NIdentifier*>(exp);
		if(env.find(e->name) == env.end()) {
			// not found
			fvs.insert(e->name);
		}
	} else if(typeid(*exp) == typeid(NLetRecExpression)) {
		NLetRecExpression *e = reinterpret_cast<NLetRecExpression*>(exp);
		// bound function and args to environment
		TypeEnv newEnv = env;
		newEnv[e->proto->id.name] = e->t;
		for(auto arg: e->proto->args) {
			newEnv[arg->id.name] = arg->t;
		}
		// search free variables in let rec body (independently from former environment)
		std::set<std::string> clsFvs;
		freeVariables(clsFvs, newEnv, &e->body);
		// add fvs to args
		NFundefExpression *proto = e->proto;
		std::vector<NExpression*> args;
		for(auto fv: clsFvs) {
			NIdentifier *argid = new NIdentifier(fv);
			NLetExpression *arg = new NLetExpression(*argid);
			// free variable must be defined in the outer environment
			LemlType *argtype = newEnv[fv];
#ifdef LEML_DEBUG
			assert(argtype != nullptr);
#endif
			arg->t = argtype;
			proto->args.push_back(arg);
			args.push_back(arg);
		}
		// add new args to function calls in let rec eval recursively
		extendArgs(proto->id, args, &e->eval);
		// continue lambda lifting in let rec eval using former environment
		newEnv = env;
		newEnv[e->proto->id.name] = e->t;
		freeVariables(fvs, newEnv, &e->eval);
		fvs.erase(e->proto->id.name);
	} else if(typeid(*exp) == typeid(NCallExpression)) {
		NCallExpression *e = reinterpret_cast<NCallExpression*>(exp);
		for(auto arg: *e->args) {
			freeVariables(fvs, env, arg);
		}
		// TODO: check higher-order function
		freeVariables(fvs, env, &e->fun);
	} else if(typeid(*exp) == typeid(NArrayExpression)) {
		return;
	} else if(typeid(*exp) == typeid(NArrayGetExpression)) {
		NArrayGetExpression *e = reinterpret_cast<NArrayGetExpression*>(exp);
		freeVariables(fvs, env, &e->array);
		freeVariables(fvs, env, &e->index);
	} else if(typeid(*exp) == typeid(NArrayPutExpression)) {
		NArrayPutExpression *e = reinterpret_cast<NArrayPutExpression*>(exp);
		freeVariables(fvs, env, &e->array);
		freeVariables(fvs, env, &e->index);
		freeVariables(fvs, env, &e->exp);
	} else if(typeid(*exp) == typeid(NTupleExpression)) {
		NTupleExpression *e = reinterpret_cast<NTupleExpression*>(exp);
		for(auto elem: e->elems) {
			freeVariables(fvs, env, elem);
		}
	} else if(typeid(*exp) == typeid(NLetTupleExpression)) {
		NLetTupleExpression *e = reinterpret_cast<NLetTupleExpression*>(exp);
		freeVariables(fvs, env, &e->eval);
		for(auto let: e->ids) {
			fvs.erase(let->id.name);
		}
		freeVariables(fvs, env, &e->exp);
	}
}

bool lambdaLifting(TypeEnv env, NExpression *program) {
	// free variable set
	std::set<std::string> fvs;

	// the instance of lambda lifting
	freeVariables(fvs, env, program);

	return true;
}
