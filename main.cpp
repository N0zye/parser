#include "parser.hpp"
#include <iostream>

using namespace std;

int main() {
		
	Parser parser;
	
	ASTnode* ast = parser.parse("(3+2)*7^2");

	cout << "Hello, World!" << endl;
	return 0;
}
