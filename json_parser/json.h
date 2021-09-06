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
	std::variant<std::string, float, bool, char> value;
};

struct Parser {
	unsigned int index;
	bool eof = false;
	std::string str;
	std::vector<Token> tokens;
	std::string cache; // chars left that were not matching anything
};

enum class AST_Node_Type {
	OBJECT,
	ARRAY
};

struct AST_Pair_Node;

enum class Value_Type {
	NUMBER,
	STRING,
	BOOL,
	OBJECT,
	ARRAY,
	ERROR
};

struct AST_Node;


struct AST_Value_Node {
	Value_Type type;
	std::variant<std::string, float, bool, AST_Node*, std::vector<AST_Value_Node>> value;
	
	AST_Value_Node operator[](std::string key);
	AST_Value_Node operator[](int i);
};

struct AST_Node {
	AST_Node_Type type;
	std::string name;
	std::vector<AST_Pair_Node*> properties;
	std::vector<AST_Value_Node> array;
	AST_Node* parent;
};

typedef std::vector<AST_Value_Node> V_Node_List;

// should we replace this w/ a map???
struct AST_Pair_Node {
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
	
	AST_Value_Node operator[](std::string key);
};

//class GetValue {
//public:
	
//};

void lex(Parser* parser);
Json_Data parse(std::string str);
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

std::string get_string(AST_Value_Node value_node);
float get_number(AST_Value_Node value_node);
std::vector<AST_Value_Node> get_array(AST_Value_Node value_node);
std::vector<AST_Pair_Node*> get_object(AST_Value_Node value_node);
bool get_bool(AST_Value_Node value_node);



#endif /* json_h */
