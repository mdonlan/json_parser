//
//  main.cpp
//  json_parser
//
//  Created by Michael Donlan on 8/21/21.
//

#include <iostream>
#include <string>
#include <vector>


enum class Token_Type {
	OPEN_BRACKET,
	STRING_VALUE,
	NAME,
	NUMBER,
	BOOL,
	COLON,
	END_OF_FILE
};

struct Token {
	Token_Type type;
	
	std::string str;
	float number = 0.0f;
	bool bool_val = false;
	
//	union {
//		float number;
//		std::string str;
//		char c;
//	};
};


struct Parser {
	unsigned int index;
	bool eof = false;
	std::string str;
	std::vector<Token> tokens;
	std::string cache; // chars left that were not matching anything
};

enum class AST_Node_Type {
	ROOT,
	OBJECT,
	NAME
};

struct AST_Node {
	AST_Node_Type type;
	std::vector<AST_Node> children;
};

struct AST {
	AST_Node root;
};



void consume(Parser* parser);
char peek(Parser* parser, unsigned int index);
void eat_whitespace(Parser* parser);
void create_ast(std::vector<Token>& tokens);

int main(int argc, const char * argv[]) {
	Parser* parser = new Parser;
	
	std::string test_json = R"(
		{
		"userId": 1,
		"id": 1,
		"title": "delectus aut autem",
		"completed": false,
		"test_bool": true
		}
	)";
	
	parser->str = test_json;
	
	// lex (create tokens)
	while (!parser->eof) {
		consume(parser);
	}
	
	// parse (create ast)
	create_ast(parser->tokens);
	
	return 0;
}

void consume(Parser* parser) {
	eat_whitespace(parser);
	char c = parser->str[parser->index];
	
	// match against tokens
	if (c == '\0') {
		parser->eof = true;
		Token token;
		token.type = Token_Type::END_OF_FILE;
		parser->tokens.push_back(token);
	} else if (c == '{') { // left bracket token
		Token token;
		token.type = Token_Type::OPEN_BRACKET;
		parser->tokens.push_back(token);
		parser->index++;
	} else if (c == ':') {
		Token token;
		token.type = Token_Type::COLON;
		parser->tokens.push_back(token);
		parser->index++;
	} else if (c == '"') { // name/decl token or string value token
		Token token;
//		token.type = STRING;
		
		std::string str;
		unsigned int p_index = parser->index + 1;
		bool found_end_of_string = false;
		
		while (!found_end_of_string) {
			char pc = peek(parser, p_index);
			if (pc == '"') {
				found_end_of_string = true;
				eat_whitespace(parser);
				if (peek(parser, p_index + 1) == ':') {
					// this token is a name/declaration token
					token.type = Token_Type::NAME;
				} else {
					// this token is a string value
					token.type = Token_Type::STRING_VALUE;
				}
			} else {
				str.push_back(pc);
				p_index++;
			}
		}
		
		token.str = str;
		parser->tokens.push_back(token);
		parser->index += str.size() + 2; // +2 for ""
	} else if (isdigit(c)) { // number token
		Token token;
		token.type = Token_Type::NUMBER;
		
		std::string num_str(1, c);
		bool found_end_of_number = false;
		unsigned int p_index = parser->index + 1;
		
		while (!found_end_of_number) {
			char pc = peek(parser, p_index);
			if (!isdigit(pc)) {
				found_end_of_number = true;
			} else {
				num_str += pc;
				p_index++;
			}
		}
		
		token.number = std::stoi(num_str);
		parser->tokens.push_back(token);
		parser->index += num_str.size();
	} else if (parser->cache.compare("false") == 0) { // is there a better way to do BOOLS?
		Token token;
		token.type = Token_Type::BOOL;
		token.bool_val = false;
		parser->tokens.push_back(token);
		parser->index += parser->cache.size() - 1;
		parser->cache.clear();
	} else if (parser->cache.compare("true") == 0) {
		Token token;
		token.type = Token_Type::BOOL;
		token.bool_val = true;
		parser->tokens.push_back(token);
		parser->index += 5;
		parser->cache.clear();
	}
	else {
		if (isalpha(c)) {
			parser->cache += c;
		}
		
		parser->index++;
	}
	
//	parser->index++;
}

char peek(Parser* parser, unsigned int index) {
	return parser->str[index];
}

void eat_whitespace(Parser* parser) {
	char c = parser->str[parser->index];
	while (c == ' ' || c == '\n' || c == '\t') {
		parser->index++;
		c = parser->str[parser->index];
	}
}

void create_ast(std::vector<Token>& tokens) {
	AST ast;
	
	int token_index = 0;
	Token current_token = tokens[token_index];
	
	AST_Node root_node;
	root_node.type = AST_Node_Type::ROOT;
	ast.root = root_node;
	
	AST_Node& current_node = ast.root;
	
	while (current_token.type != Token_Type::END_OF_FILE) {
		if (current_token.type == Token_Type::OPEN_BRACKET) {
			AST_Node node;
			node.type = AST_Node_Type::OBJECT;
			current_node.children.push_back(node);
		} else if (current_token.type == Token_Type::NAME) {
			AST_Node node;
			node.type = AST_Node_Type::NAME;
			current_node.children.push_back(node);
		}
		
		token_index++;
		current_token = tokens[token_index];
	}
	
	int a = 0;
}
