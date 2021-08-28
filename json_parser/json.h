//
//  json.h
//  json_parser
//
//  Created by Michael Donlan on 8/23/21.
//

#ifndef json_h
#define json_h

#include <string>
#include <vector>
#include <variant>

enum class Token_Type {
	OPEN_CURLY_BRACKET,
	CLOSED_CURLY_BRACKET,
	STRING_VALUE,
	NAME,
	NUMBER,
	BOOL,
	COLON,
	OPEN_SQUARE_BRACKET,
	CLOSED_SQUARE_BRACKET,
	END_OF_FILE
};

struct Token {
	Token_Type type;
	
	std::variant <
		std::string,
		float,
		bool,
		char
	> value;
	
//	std::string str;
//	float number = 0.0f;
//	bool bool_val = false;
	
//	union {
//		float _number;
//		char* _str;
//		bool _bool_val;
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

struct AST_Pair_Node;

struct AST_Node {
	AST_Node_Type type;
	std::string name;
//	AST_Node* left;
//	AST_Node* right;
//	std::vector<AST_Node*> array_members;
	std::vector<AST_Pair_Node*> properties;
	AST_Node* parent;
};

enum class Value_Type {
	NUMBER,
	STRING,
	BOOL,
	OBJECT,
	ARRAY
};

struct AST_Value_Node {
	Value_Type type;
//	std::string str;
//	float number = 0.0f;
//	bool bool_val = false;
//	AST_Node* object;
//	std::vector<AST_Value_Node> array;
	
	std::variant<std::string, float, bool, AST_Node*, std::vector<AST_Value_Node>> value;
};

struct AST_Pair_Node {
//	AST_Node_Type type;
	std::string key;
	AST_Value_Node value_node;
	AST_Node* parent;
};

struct AST {
	AST_Node* root;
};

enum class Print_Type {
	STRING,
	NUMBER,
	BOOL
};

struct Print_Data {
	std::string str;
	float number;
	bool bool_val;
};

struct Json_Data {
	AST* ast;
	std::vector<Token> tokens;
};



void lex(Parser* parser);
Json_Data* parse(std::string str);
void consume(Parser* parser);
char peek(Parser* parser, unsigned int index);
void eat_whitespace(Parser* parser);
AST* create_ast(std::vector<Token>& tokens);
const std::string load_json_from_file(const std::string& file_name);

void print_ast(AST* ast);
//void pretty_print(int indent, Print_Type type, Print_Data data, bool new_line = true);
void print_object(AST_Node* node, int indent);
void print_array(std::vector<AST_Value_Node> array, int indent);
void print_value(AST_Value_Node value_node, int indent = 0);

#endif /* json_h */
