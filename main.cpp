#include "parser.hpp"
#include <iostream>

using namespace std;

int main() {
		
	Parser parser;
	Interpreter interpreter;
	//NOTE: every thing with trig functions is considered in rad, not deg
	cout << interpreter.eval(parser.parse("3*sin(2) * (2-1)")) << endl;
	
	return 0;
}
