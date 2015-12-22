#include "syntax.hh"

// stream printer for NExpression
std::ostream &operator<<(std::ostream &os, const NExpression &expr) {
	return expr.print(os);
}
