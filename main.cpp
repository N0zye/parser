#include "parser.hpp"
#include <iostream>

using namespace std;

int main() {
		
	Parser parser;

	AST testAST = parser("3*sin(3+2) - 6");
	
	cout << "Hello, World!" << endl;
	return 0;
}
