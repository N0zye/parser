#include <string>
#include <vector>
#include <variant>
#include <stdexcept>
#include <queue>

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

struct ASTNode;
using NodeType = std::variant<std::tuple<ASTNode*,char, ASTNode*>, std::pair<std::string, ASTNode*>, ASTNode*, double, char>;

struct ASTNode {
	
	ASTNode(ParserTypes type, NodeType value) 
	: type(type), value(value) {}
	
	ParserTypes type;
	NodeType value;
};

struct AST{
	ASTNode* root;
	
	AST(ASTNode* root)
	: root(root) {}
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
	

	//* Post order traversal
	//NOTE: left -> right -> root 
	void* eval(AST tree) {
		
	}

	AST operator()(std::string expression) {
		_tokenstream = tokenize(expression);
		if (!tokenstream_is_valid()) {
			throw(std::syntax_error("Tokenizer: unexpected token"));
		}
		else {
			return AST{new ASTNode(ParserTypes::Expression, Expression())};	
		}
	}
	
	/* 
	*Expression 
	*	: AdditiveExpression
	*	;
	*/
	ASTNode* Expression() {
		return  AdditiveExpression();
	}
	
	/*
	*AdditiveExpression
	*	: MultiplicativeExpression
	*	| AdditiveExpression ADDITIVE_OPERATOR MultiplicativeExpression
	*	;
	*/
	ASTNode* AdditiveExpression() {
		ASTNode* left = MultiplicativeExpression(); // Parse the left operand
		
		// Check if there are more tokens and if they are additive operators
		while (!_tokenstream.empty() && (_tokenstream.front().type == ADDITIVE_OPERATOR)) {
			Token op = _tokenstream.front(); // Get the operator
			_tokenstream.pop(); // Remove the operator token
			ASTNode* right = MultiplicativeExpression(); // Parse the right operand
			
			// Create a new AST node representing the addition operation
			left = new ASTNode(ParserTypes::BinaryExpression, std::make_tuple(left, op.value[0], right));
		}
		
		return left;
	}
	
	/*
	*MultiplicativeExpression
	*	: ExponentiativeExpression
	*	| MultiplicativeExpression MULTIPLICATIVE_OPERATOR ExponentiativeExpression
	*	;
	*/
	ASTNode* MultiplicativeExpression() {
		ASTNode* left = ExponentiativeExpression(); // Parse the left operand
		
		// Check if there are more tokens and if they are additive operators
		while (!_tokenstream.empty() && (_tokenstream.front().type == MULTIPLICATIVE_OPERATOR)) {
			Token op = _tokenstream.front(); // Get the operator
			_tokenstream.pop(); // Remove the operator token
			ASTNode* right = ExponentiativeExpression(); // Parse the right operand
			
			// Create a new AST node representing the addition operation
			left = new ASTNode(ParserTypes::BinaryExpression, std::make_tuple(left, op.value[0], right));
		}
		
		return left;
	}
	
	/*
	*ExponentiativeExpression
	*	: PrimaryExpression
	*	| ExponentiativeExpression EXPONENTIATION_OPERATOR PrimaryExpression
	*	;
	*/
	ASTNode* ExponentiativeExpression() {
		ASTNode* left = PrimaryExpression(); // Parse the left operand
		
		// Check if there are more tokens and if they are additive operators
		while (!_tokenstream.empty() && (_tokenstream.front().type == EXPONENTIATION_OPERATOR)) {
			Token op = _tokenstream.front(); // Get the operator
			_tokenstream.pop(); // Remove the operator token
			ASTNode* right = PrimaryExpression(); // Parse the right operand
			
			// Create a new AST node representing the addition operation
			left = new ASTNode(ParserTypes::BinaryExpression, std::make_tuple(left, op.value[0], right));
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
	ASTNode* PrimaryExpression() {
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
	ASTNode* MathFunction() {
		std::string functionName = _tokenstream.front().value;
		_tokenstream.pop();
		return new ASTNode(ParserTypes::MathFunction, std::make_pair(functionName, ParenthesizedExpression()));
	}

	/*
	*ParenthesizedExpression
	*	: OPEN_PAREN Expression CLOSE_PAREN
	*	;
	*/
	ASTNode* ParenthesizedExpression() {
		_tokenstream.pop();
		ASTNode* expression = Expression();
		_tokenstream.pop();
		return expression;
	}

	/*
	*Literal
	*	: NumericLiteral
	*	| VariableLiteral
	*	;
	*/
	ASTNode* Literal() {
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
	ASTNode* Identifier() {
		ASTNode* ret = new ASTNode(ParserTypes::Identifier, _tokenstream.front().value[0]); //* Var is only 1 char
		//Considering only the first char (there's going to be only 1 char anyways cuz of tokenizer rules)
		_tokenstream.pop();
		
		return ret;
	}
	
	
	/* 
	*NumericLiteral
	*	: NUMBER
	*	;
	*/
	ASTNode* NumericLiteral() {
		ASTNode* ret = new ASTNode(ParserTypes::NumericLiteral, std::stod(_tokenstream.front().value));
		_tokenstream.pop();
		
		return ret;
	}
	
};

#endif /* PARSER_HPP_GUARD */