#include "parser.hpp"
#include <iostream>

using namespace std;

int main() {
		
	Parser parser;

	AST testAST = parser("3*sin(x+2) - 6");
	
	cout << "Hello, World!" << endl;
	return 0;
}
