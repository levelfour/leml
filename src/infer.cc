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
	} else if(t->tag == Var && t->data == r1) {
		return true;
	} else if(t->tag == Var && t->data == nullptr) {
		return false;
	} else if(t->tag == Var) {
		return occur(t->data, r1);
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
		if(occur(t1->data, t2)) throw "unify";
		t1->data = t2;
	} else if(t2->tag == Var && t2->data == nullptr) {
		if(occur(t2->data, t1)) throw "unify";
		t2->data = t1;
	} else {
		throw "unify";
	}
}

LemlType *infer(NExpression* expr, TypeEnv env) {
	if(typeid(expr) == typeid(NUnit)) {
		return typeUnit;
	} else if(typeid(expr) == typeid(NBoolean)) {
		return typeBool;
	} else if(typeid(expr) == typeid(NInteger)) {
		return typeInt;
	} else if(typeid(expr) == typeid(NFloat)) {
		return typeFloat;
	} else if(typeid(expr) == typeid(NUnaryExpression)) {
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
	} else if(typeid(expr) == typeid(NBinaryExpression)) {
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
			case LEq:
			case LNeq:
			case LLT:
			case LLE:
			case LGT:
			case LGE:
				unify(infer(&e->lhs, env), infer(&e->rhs, env));
				return typeBool;
		}
	} else if(typeid(expr) == typeid(NIfExpression)) {
		NIfExpression* e = dynamic_cast<NIfExpression*>(expr);
		unify(infer(&e->cond, env), typeBool);
		auto* t2 = infer(&e->true_exp, env);
		auto* t3 = infer(&e->false_exp, env);
		unify(t2, t3);
		return t2;
	} else if(typeid(expr) == typeid(NLetExpression)) {
		NLetExpression* e = dynamic_cast<NLetExpression*>(expr);
		unify(e->t, infer(e->assign, env));
		env[&e->id] = e->t;
		return infer(e->eval, env);
	} else if(typeid(expr) == typeid(NLetRecExpression)) {
		NLetRecExpression* e = dynamic_cast<NLetRecExpression*>(expr);
		env[&e->id] = e->t;
		// TODO:
		return infer(&e->eval, env);
	} else if(typeid(expr) == typeid(NCallExpression)) {
		NCallExpression* e = dynamic_cast<NCallExpression*>(expr);
		// TODO:
		return nullptr;
	} else if(typeid(expr) == typeid(NArrayExpression)) {
		NArrayExpression* e = dynamic_cast<NArrayExpression*>(expr);
		return new LemlType({Array, infer(&e->data, env), {}});
	}

	// failure
	return nullptr;
}

// LemlType* -> llvm::Type*
static llvm::Type* llvmType(LemlType* type) {
	typeAssersion(type->tag);
	switch(type->tag) {
		case Unit:
			return llvm::Type::getVoidTy(llvm::getGlobalContext());
		case Bool:
			return reinterpret_cast<llvm::Type*>(llvm::Type::getInt1Ty(llvm::getGlobalContext()));
		case Int:
			return reinterpret_cast<llvm::Type*>(llvm::Type::getInt32Ty(llvm::getGlobalContext()));
		case Float:
			return llvm::Type::getFloatTy(llvm::getGlobalContext());
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
			return llvm::StructType::get(
					llvm::getGlobalContext(),
					llvm::makeArrayRef(types));
		}
		case Array:
			return llvm::PointerType::getUnqual(llvmType(type->data));
		case Var:
			throw "undecidable type";
	}
}
