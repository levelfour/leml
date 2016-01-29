#include "infer.hh"
#include "syntax.hh"

static void typeAssersion(LemlTypeTag tag) {
	assert( tag == Unit || tag == Bool || tag == Int ||
			tag == Float || tag == Fun || tag == Tuple ||
			tag == Array || tag == Var);
}

template<class T>
static void vectorLengthAssersion(std::vector<T> v1, std::vector<T> v2) {
	if(v1.size() != v2.size()) {
		// TODO: raise Exception
		throw "iteration over sequences of different length";
	}
}

// type r1 occurs in t ?
static bool occur(LemlType* r1, LemlType* t) {
	assert(t != nullptr);

	if(t->tag == Fun) {
		bool result = false;
		for(auto ty: t->array) {
			result |= occur(r1, ty);
		}
		result |= occur(r1, t->data);
		return result;
	} else if(t->tag == Tuple) {
		bool result = false;
		for(auto ty: t->array) {
			result |= occur(r1, ty);
		}
		return result;
	} else if(t->tag ==	Array) {
		return occur(r1, t->data);
	} else if(t->tag == Var && t->data == r1 && r1 != nullptr) {
		return true;
	} else if(t->tag == Var && t->data == nullptr) {
		return false;
	} else if(t->tag == Var) {
		return occur(r1, t->data);
	} else {
		return false;
	}
}

void unify(LemlType* t1, LemlType* t2) {
	if(t1->tag == Unit && t2->tag == Unit) {
		return;
	} else if(t1->tag == Bool && t2->tag == Bool) {
		return;
	} else if(t1->tag == Int && t2->tag == Int) {
		return;
	} else if(t1->tag == Float && t2->tag == Float) {
		return;
	} else if(t1->tag == Fun && t2->tag == Fun) {
		vectorLengthAssersion<LemlType*>(t1->array, t2->array);
		for(std::string::size_type i = 0; i < t1->array.size(); i++) {
			unify(t1->array[i], t2->array[i]);
		}
		unify(t1->data, t2->data);
	} else if(t1->tag == Tuple && t2->tag == Tuple) {
		vectorLengthAssersion<LemlType*>(t1->array, t2->array);
		for(std::string::size_type i = 0; i < t1->array.size(); i++) {
			unify(t1->array[i], t2->array[i]);
		}
	} else if(t1->tag == Array && t2->tag == Array) {
		unify(t1->data, t2->data);
	} else if(t1->tag == Var && t2->tag == Var && t1->data == t2->data) {
		return;
	} else if(t1->tag == Var && t1->data != nullptr) {
		unify(t1->data, t2);
	} else if(t2->tag == Var && t2->data != nullptr) {
		unify(t1, t2->data);
	} else if(t1->tag == Var && t1->data == nullptr) {
		if(occur(t1->data, t2)) {
			throw UnificationError(t1, t2);
		}
		t1->data = t2;
	} else if(t2->tag == Var && t2->data == nullptr) {
		if(occur(t2->data, t1)) {
			throw UnificationError(t1, t2);
		}
		t2->data = t1;
	} else {
		throw UnificationError(t1, t2);
	}
}

LemlType* infer(NExpression* expr) {
	return infer(expr, TypeEnv());
}

LemlType *infer(NExpression* expr, TypeEnv env) {
	assert(expr != nullptr);

	if(typeid(*expr) == typeid(NUnit)) {
		return typeUnit;
	} else if(typeid(*expr) == typeid(NBoolean)) {
		return typeBool;
	} else if(typeid(*expr) == typeid(NInteger)) {
		return typeInt;
	} else if(typeid(*expr) == typeid(NFloat)) {
		return typeFloat;
	} else if(typeid(*expr) == typeid(NUnaryExpression)) {
		NUnaryExpression* e = dynamic_cast<NUnaryExpression*>(expr);
		switch(e->op) {
			case LNot:
				unify(typeBool, infer(&e->expr, env));
				return typeBool;
			case LNeg:
				unify(typeInt, infer(&e->expr, env));
				return typeInt;
			case LFNeg:
				unify(typeFloat, infer(&e->expr, env));
				return typeFloat;
		}
	} else if(typeid(*expr) == typeid(NBinaryExpression)) {
		NBinaryExpression* e = dynamic_cast<NBinaryExpression*>(expr);
		switch(e->op) {
			case LAdd:
			case LSub:
			case LMul:
			case LDiv:
				unify(typeInt, infer(&e->lhs, env));
				unify(typeInt, infer(&e->rhs, env));
				return typeInt;
			case LFAdd:
			case LFSub:
			case LFMul:
			case LFDiv:
				unify(typeFloat, infer(&e->lhs, env));
				unify(typeFloat, infer(&e->rhs, env));
				return typeFloat;
		}
	} else if(typeid(*expr) == typeid(NCompExpression)) {
		NCompExpression* e = dynamic_cast<NCompExpression*>(expr);
		switch(e->op) {
			case LEq:
			case LNeq:
			case LLT:
			case LLE:
			case LGT:
			case LGE:
				unify(infer(&e->lhs, env), infer(&e->rhs, env));
				return typeBool;
		}
	} else if(typeid(*expr) == typeid(NIfExpression)) {
		NIfExpression* e = dynamic_cast<NIfExpression*>(expr);
		unify(infer(&e->cond, env), typeBool);
		auto* t2 = infer(&e->true_exp, env);
		auto* t3 = infer(&e->false_exp, env);
		unify(t2, t3);
		return t2;
	} else if(typeid(*expr) == typeid(NLetExpression)) {
		NLetExpression* e = dynamic_cast<NLetExpression*>(expr);
		unify(e->t, infer(e->assign, env));
		env[e->id.name] = e->t;
		return infer(e->eval, env);
	} else if(typeid(*expr) == typeid(NIdentifier)) {
		NIdentifier* e = dynamic_cast<NIdentifier*>(expr);
		auto t = env.find(e->name);
		if(t != env.end()) {
			// env memorized this variable
			return t->second;
		} else {
			// TODO: inference on external variable
			return nullptr;
		}
	} else if(typeid(*expr) == typeid(NLetRecExpression)) {
		NLetRecExpression* e = dynamic_cast<NLetRecExpression*>(expr);
		env[e->id.name] = e->t;
		TypeEnv envBody = env;
		std::vector<LemlType*> typeArgs;
		for(auto arg: e->args) {
			envBody[arg->id.name] = arg->t;
			typeArgs.push_back(arg->t);
		}
		unify(e->t, new LemlType({Fun, infer(&e->body, envBody), typeArgs}));
		return infer(&e->eval, env);
	} else if(typeid(*expr) == typeid(NCallExpression)) {
		NCallExpression* e = dynamic_cast<NCallExpression*>(expr);
		auto* t = newty();
		std::vector<LemlType*> typeArgs;
		for(auto arg: *e->args) {
			typeArgs.push_back(infer(arg, env));
		}
		unify(infer(&e->fun, env), new LemlType({Fun, t, typeArgs}));
		return t;
	} else if(typeid(*expr) == typeid(NArrayExpression)) {
		NArrayExpression* e = dynamic_cast<NArrayExpression*>(expr);
		return new LemlType({Array, infer(&e->data, env), {}});
	} else if(typeid(*expr) == typeid(NArrayGetExpression)) {
		NArrayGetExpression* e = dynamic_cast<NArrayGetExpression*>(expr);
		auto* t = newty();
		unify(new LemlType({Array, t, {}}), infer(&e->array, env));
		unify(typeInt, infer(&e->index, env));
		return t;
	} else if(typeid(*expr) == typeid(NArrayPutExpression)) {
		NArrayPutExpression* e = dynamic_cast<NArrayPutExpression*>(expr);
		auto* t = infer(&e->exp, env);
		unify(new LemlType({Array, t, {}}), infer(&e->array, env));
		unify(typeInt, infer(&e->index, env));
		return typeUnit;
	} else if(typeid(*expr) == typeid(NTupleExpression)) {
		NTupleExpression* e = dynamic_cast<NTupleExpression*>(expr);
		std::vector<LemlType*> types;
		for(auto elem: e->elems) {
			types.push_back(infer(elem, env));
		}
		return new LemlType({Tuple, nullptr, types});
	} else if(typeid(*expr) == typeid(NLetTupleExpression)) {
		NLetTupleExpression* e = dynamic_cast<NLetTupleExpression*>(expr);
		std::vector<LemlType*> types;
		for(auto elem: e->ids) {
			types.push_back(elem->t);
		}

		auto ty = new LemlType({Tuple, nullptr, types});
		unify(ty, infer(&e->exp, env));

		for(auto elem: e->ids) {
			env[elem->id.name] = elem->t;
		}

		return infer(&e->eval, env);
	}

	// failure
	return nullptr;
}

// dereference `Var` in type
LemlType* deref(LemlType* type) {
#ifdef LEML_DEBUG
	assert(type != nullptr);
#endif

	switch(type->tag) {
		case Fun:
		{
			std::vector<LemlType*> typeArgs;
			for(auto* t: type->array) {
				typeArgs.push_back(deref(t));
			}
			type->data = deref(type->data);
			type->array = typeArgs;
			break;
		}
		case Tuple:
		{
			std::vector<LemlType*> types;
			for(auto* t: type->array) {
				types.push_back(deref(t));
			}
			type->array = types;
			break;
		}
		case Array:
			type->data = deref(type->data);
			break;
		case Var:
			if(type->data == nullptr) {
				std::cerr << "uninstantiated type variable detected; assuming int" << std::endl;
				type = typeInt;
			} else {
				type = deref(type->data);
			}
			break;
	}
	return type;
}

// LemlType* -> llvm::Type*
llvm::Type* llvmType(LemlType* type) {
	typeAssersion(type->tag);
	switch(type->tag) {
		case Unit:
			return reinterpret_cast<llvm::Type*>(llvm::Type::getInt32Ty(llvm::getGlobalContext()));
		case Bool:
			return reinterpret_cast<llvm::Type*>(llvm::Type::getInt1Ty(llvm::getGlobalContext()));
		case Int:
			return reinterpret_cast<llvm::Type*>(llvm::Type::getInt32Ty(llvm::getGlobalContext()));
		case Float:
			return llvm::Type::getDoubleTy(llvm::getGlobalContext());
		case Fun:
		{
			std::vector<llvm::Type*> argtypes;
			for(auto ty: type->array) {
				argtypes.push_back(llvmType(ty));
			}
			return llvm::FunctionType::get(
					llvmType(type->data),
					llvm::makeArrayRef(argtypes),
					false);
		}
		case Tuple:
		{
			std::vector<llvm::Type*> types;
			for(auto ty: type->array) {
				types.push_back(llvmType(ty));
			}
			auto typeStruct = llvm::StructType::get(
					llvm::getGlobalContext(),
					llvm::makeArrayRef(types));
			return llvm::PointerType::getUnqual(typeStruct);
		}
		case Array:
			return llvm::PointerType::getUnqual(llvmType(type->data));
		case Var:
			return llvmType(deref(type));
	}
	return nullptr;
}

LemlType* check(NExpression* program) {
	LemlType* t;

	try {
		t = infer(program);
		if(t == nullptr) {
			std::cerr << "type check failure" << std::endl;
			exit(EXIT_FAILURE);
		}
	} catch(UnificationError e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return deref(t);
}
