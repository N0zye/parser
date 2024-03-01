#include "parser.hpp"
#include <iostream>

int main() {
		
	MathExpression f("3*x+cos(x+2)");
	f.setExpression("x+cos(x+2)");
	std::cout << f(88);
	
	return 0;
}
