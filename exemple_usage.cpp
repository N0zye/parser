#include "parser.hpp"
#include <iostream>
#include <unordered_map>
#include <functional>
using namespace std;

int main() {
		
	MathExpression f("3*x+cos(x+2)");
	f.setExpression("x+cos(x+2)");
	cout << f(88);
	
	return 0;
}
