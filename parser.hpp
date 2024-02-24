#include <string>
#include <vector>
#include <variant>
#include <stdexcept>
#include <queue>
#include <cmath>
#include <unordered_map>
#include <functional>

#ifndef PARSER_HPP_GUARD
#define PARSER_HPP_GUARD

enum TokenType {
	NUMBER,
	ADDITIVE_OPERATOR,
	MULTIPLICATIVE_OPERATOR,
	EXPONENTIATION_OPERATOR,
	FUNCTION,
	VARIABLE,
	OPEN_PAREN,
	CLOSE_PAREN,
	UNKNOWN
};

enum ParserTypes {
	Expression,
	BinaryExpression,
	MathFunction,
	NumericLiteral,
	Identifier
};

struct Token {
	Token(TokenType type, std::string value)
		: type(type), value(value) {}

	TokenType type;
	std::string value;
};

using NodeType = std::variant<std::string, char, double>;
struct ASTnode {
	ASTnode(ParserTypes type, NodeType value) 
		: type(type), value(value), left(nullptr), right(nullptr) {}
	//Overload
	ASTnode(ParserTypes type, NodeType value, ASTnode* left) 
		: type(type), value(value), left(left), right(nullptr) {}
	//Overload
	ASTnode(ParserTypes type, NodeType value, ASTnode* left, ASTnode* right) 
		: type(type), value(value), left(left), right(right) {}

	ParserTypes type;
	NodeType value;
	ASTnode* left;
	ASTnode* right;
};

// SYNTAX ERROR DEFINITION
namespace std {
	class syntax_error : public std::logic_error {
	public:
		explicit syntax_error(const std::string& message)
		: std::logic_error(message) {}
		
		explicit syntax_error(const char* message)
		: std::logic_error(message) {}
	};
}

// TOKENIZER
// TODO redo the tokenizer in a single class using RegExp (#include <regex>)

std::string findFullNumber(const std::string& expression, int begin) {
	std::string fullNumericLiteral = "";
	for (int i = begin; expression.size(); i++) {
		if (!isdigit(expression[i])) {
			break;
		} else {
			fullNumericLiteral += expression[i];
		}
	}
	return fullNumericLiteral;
}
 
std::string findFullFunction(const std::string& expression, int begin) {
	//any function (such as sin cos tan atan ecc) *must* be followed by a @ "("
	std::string fullFunction = "";
	for (int i = begin; expression.size(); i++) {
		if (expression[i] == '(') {
			break;
		} else {
			fullFunction += expression[i];
		}
	}
	return fullFunction;
}

std::queue<Token> tokenize(const std::string& expression) {
	std::queue<Token> tokens;

	for (int i = 0; i < (int)expression.size(); i++) {
		if (expression[i] == ' ') {
			continue;
		} else if (expression[i] == 'x') {
			tokens.push(Token(TokenType::VARIABLE, "x"));
		} else if (expression[i] == '(') {
			tokens.push(Token(TokenType::OPEN_PAREN, "("));
		} else if (expression[i] == ')') {
			tokens.push(Token(TokenType::CLOSE_PAREN, ")"));
		} else if (ispunct(expression[i])) {
			if(expression[i] == '+' || expression[i] == '-'){
				tokens.push(Token(TokenType::ADDITIVE_OPERATOR, std::string(1, expression[i])));
			}else if(expression[i] == '*' || expression[i] == '/'){
				tokens.push(Token(TokenType::MULTIPLICATIVE_OPERATOR, std::string(1, expression[i])));	
			} else if(expression[i] == '^'){
				tokens.push(Token(TokenType::EXPONENTIATION_OPERATOR, std::string(1, expression[i])));
			}
		} else if (isdigit(expression[i])) {
			tokens.push(Token(TokenType::NUMBER, findFullNumber(expression, i)));
			i += findFullNumber(expression, i).size() - 1;
		} else if (isalpha(expression[i])) {
			tokens.push(Token(TokenType::FUNCTION, findFullFunction(expression, i)));
			i += findFullFunction(expression, i).size() - 1;
		} else tokens.push(Token(TokenType::UNKNOWN, "??"));
	}

	return tokens;
}


//PARSER

class Parser {
private:
	std::queue<Token> _tokenstream;
	bool tokenstream_is_valid() {
		std::queue<Token> tokenstream_copy = _tokenstream;
    	while (!tokenstream_copy.empty()) {
    	    // Get the front token
    	    Token current_token = tokenstream_copy.front();
    	    tokenstream_copy.pop(); 
    	    if (current_token.type == TokenType::UNKNOWN) {
    	        return false; // Invalid token found, return false
    	    }

    	}
   		// If no invalid tokens were found, return true
    	return true;
	}
public:
	
	ASTnode* parse(std::string expression) {
		_tokenstream = tokenize(expression);
		if (!tokenstream_is_valid()) {
			throw(std::syntax_error("Tokenizer: unexpected token"));
		}
		else {
			return Expression();	
		}
	}
	
	/* 
	*Expression 
	*	: AdditiveExpression
	*	;
	*/
	ASTnode* Expression() {
		return  AdditiveExpression();
	}
	
	/*
	*AdditiveExpression
	*	: MultiplicativeExpression
	*	| AdditiveExpression ADDITIVE_OPERATOR MultiplicativeExpression
	*	;
	*/
	ASTnode* AdditiveExpression() {
		return _BinaryExpression(Parser::MultiplicativeExpression, ADDITIVE_OPERATOR);
	}
	
	/*
	*MultiplicativeExpression
	*	: ExponentiativeExpression
	*	| MultiplicativeExpression MULTIPLICATIVE_OPERATOR ExponentiativeExpression
	*	;
	*/
	ASTnode* MultiplicativeExpression() {
		return _BinaryExpression(Parser::ExponentiativeExpression, MULTIPLICATIVE_OPERATOR);
	}
	
	/*
	*ExponentiativeExpression
	*	: PrimaryExpression
	*	| ExponentiativeExpression EXPONENTIATION_OPERATOR PrimaryExpression
	*	;
	*/
	ASTnode* ExponentiativeExpression() {

		return _BinaryExpression(Parser::PrimaryExpression, EXPONENTIATION_OPERATOR);
	}
	
	//* BinaryExpression helper method
	ASTnode* _BinaryExpression(ASTnode* (Parser::*builderName)(), TokenType operatorToken) {
		ASTnode* left = (this->*builderName)();

		while (!_tokenstream.empty() && (_tokenstream.front().type == operatorToken)) {
			char op = _tokenstream.front().value.at(0); // Get the operator
			_tokenstream.pop(); // Remove the operator token
			ASTnode* right = (this->*builderName)(); // Parse the right operand, consuming the passed function
			
			// Create a new AST node representing the addition operation
			left = new ASTnode(ParserTypes::BinaryExpression, op, left, right);
		}
		
		return left;
	}

	/*
	*PrimaryExpression
	*	: Literal
	*	| ParenthesizedExpression
	*	| MathFunction
	*	;
	*/
	ASTnode* PrimaryExpression() {
		switch (_tokenstream.front().type) {
		case OPEN_PAREN:
			return ParenthesizedExpression();
			break;
		case FUNCTION:
			return MathFunction();
			break;
		default:
			return Literal();
			break;
		}
	}
	/*
	*MathFunction
	*	: FUNCTION ParenthesizedExpression
	*	;
	*/
	ASTnode* MathFunction() {
		std::string functionName = _tokenstream.front().value;
		_tokenstream.pop();
		return new ASTnode(ParserTypes::MathFunction, functionName, ParenthesizedExpression());
	}

	/*
	*ParenthesizedExpression
	*	: OPEN_PAREN Expression CLOSE_PAREN
	*	;
	*/
	ASTnode* ParenthesizedExpression() {
		_tokenstream.pop();
		ASTnode* expression = Expression();
		_tokenstream.pop();
		return expression;
	}

	/*
	*Literal
	*	: NumericLiteral
	*	| VariableLiteral
	*	;
	*/
	ASTnode* Literal() {
		switch (_tokenstream.front().type) {
		case VARIABLE:
			return Identifier();
			break;
		case NUMBER:
			return NumericLiteral();
			break;
		default:
			throw(std::syntax_error("Literal: unexpected literal production: " + _tokenstream.front().value));
			break;
		}
	}
	
	/*
	*VariableLiteral
	*	: VARIABLE
	*	;
	*/
	ASTnode* Identifier() {
		ASTnode* ret = new ASTnode(ParserTypes::Identifier, _tokenstream.front().value[0]); //note Var is only 1 char
		//Considering only the first char (there's going to be only 1 char anyways cuz of tokenizer rules)
		_tokenstream.pop();
		
		return ret;
	}
	
	
	/* 
	*NumericLiteral
	*	: NUMBER
	*	;
	*/
	ASTnode* NumericLiteral() {
		ASTnode* ret = new ASTnode(ParserTypes::NumericLiteral, std::stod(_tokenstream.front().value));
		
		_tokenstream.pop(); // Eat the token
		
		return ret;
	}
	
};


// INTERPRETER

class Interpreter {
public:
	//NOTE Post order traversal
	//note: left -> right -> root 
	double eval(ASTnode* root) {
		if (root->type == ParserTypes::BinaryExpression) {
			return _compute_bin(eval(root->left), std::get<char>(root->value), eval(root->right));
		}else if(root->type == ParserTypes::NumericLiteral) {
			return std::get<double>(root->value);
		} else if(root->type == ParserTypes::MathFunction) {
			return _compute_funct(std::get<std::string>(root->value), eval(root->left));
		 } else if(root->type == ParserTypes::Identifier) {
			return NAN; //TODO: fix this
		}
		 else {
			throw std::syntax_error("Eval: unexpected type");
		}
	}

	// Helper methods
	double _compute_funct(std::string function, double x) {
		if (function == "sin") {
        return sin(x);
    } else if (function == "cos") {
        return cos(x);
    } else if (function == "tan") {
        return tan(x);
    } else if (function == "asin") {
        return asin(x);
    } else if (function == "acos") {
        return acos(x);
    } else if (function == "atan") {
        return atan(x);
    } else {
        throw std::syntax_error("Unsupported function: " + function);
    }
	}

	double _compute_bin(double left, char op, double right) {
		switch (op)
		{
		case '+':
			return left + right;
			break;
		case '-':
			return left - right;
			break;
		case '*':
			return left * right;
			break;
		case '/':
			return left / right;
			break;
		case '^':
			return pow(left, right);
			break;
		default:
			throw std::syntax_error("Eval: unexpected operator");
			break;
		}
	}
};


#endif /* PARSER_HPP_GUARD */