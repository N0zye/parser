[![Release](https://img.shields.io/github/v/release/N0zye/parser)](https://github.com/N0zye/parser/releases/latest)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE.txt)

# Parser
C++ Math Expressions parser header-only no template(kinda) library.

## Usage
[Dowload](https://github.com/N0zye/parser/releases/download/v1.0.0/parser.hpp) and include the header in the project, create an (or more) instance of the `MathExpression` class (eg. `f` like `f(x)`).
The contained expression may be changed like in the exemple.
Get the value using the `()` operator

Exemple: 
```cpp
#include "parser.hpp"
#include <iostream>

int main() {
		
	MathExpression f("3*x+cos(x+2)");
	f.setExpression("x+cos(x+2)");
	std::cout << f(88);
	
	return 0;
}
```
