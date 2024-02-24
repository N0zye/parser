#include "parser.hpp"
#include <iostream>

using namespace std;

int main() {
		
	Parser parser;
	//NOTE: every thing with trig functions is considered in rad, not deg
	ASTnode* ast = parser.parse("(sin(3)+2)*7^2");
	
	cout << parser.eval(ast) << endl;
	return 0;
}
