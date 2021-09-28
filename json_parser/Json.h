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
#include <map>


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
//	NULL_TYPE,
	COMMA,
	END_OF_FILE,
//	ERROR,
	UNTERMINATED_STRING
};

struct Token {
	Token_Type type;
	std::variant<std::string, float, bool, char> value;
};

struct Parser {
	unsigned int index = 0;
	bool eof = false;
	std::string str;
	std::vector<Token> tokens;
	std::string cache; // chars left that were not matching anything
};

enum class AST_Node_Type {
	OBJECT,
	ARRAY,
//	STRING
};

struct AST_Pair_Node;

enum class Value_Type {
	NUMBER,
	STRING,
	BOOL,
	OBJECT,
	ARRAY,
	ERROR,
	NULL_TYPE,
	EMPTY
};

struct Json_Obj;


struct Basic_Value {
	Value_Type type;
	std::variant<std::string, int, float, bool, Json_Obj*, std::vector<Basic_Value>> value;
	
	Basic_Value operator[](std::string key);
//	Basic_Value operator=(int i);
//	Basic_Value operator[](int i);
	
//	template <typename T>
//	void operator=(T value) {
//		auto json = parse_tokens(value);
//		int a = 0;
//	}
	
	Basic_Value* operator=(std::string str);
	Basic_Value operator=(int num);
	
	const std::string to_str();
	float to_float();
	int to_int();
	std::vector<Basic_Value> to_array();
	Json_Obj* to_obj();
	bool to_bool();
};

struct Json_Obj {
	AST_Node_Type type;
	std::string name;
	std::vector<AST_Pair_Node*> properties;
	std::vector<Basic_Value> array;
	Json_Obj* parent;
};

//typedef std::vector<Basic_Value> V_Node_List;

// should we replace this w/ a map???
struct AST_Pair_Node {
	std::string key;
	Basic_Value value_node;
	Json_Obj* parent;
};

struct Json {
	Basic_Value value = { .type = Value_Type::NULL_TYPE, .value = 0 };
	
	Basic_Value& operator[](std::string key);
	
//	~Json();
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


void lex(Parser* parser);
Json parse(std::string str, bool print_error = true); // print_error -- can turn off for dev/testing purposes w/ expected errors
void consume(Parser* parser);
char peek(Parser* parser, unsigned int index);
void eat_whitespace(Parser* parser);
Json parse_tokens(std::vector<Token>& tokens, bool print_error);
const std::string load_json_from_file(const std::string& file_name);
//void print_ast(AST* ast);
//void pretty_print(int indent, Print_Type type, Print_Data data, bool new_line = true);
//void print_object(AST_Node* node, int indent);
//void print_array(std::vector<Basic_Value> array, int indent);
//void print_value(Basic_Value value_node, int indent = 0);
//std::string get_string(Basic_Value value_node);
//float get_number(Basic_Value value_node);
//std::vector<Basic_Value> get_array(Basic_Value value_node);
//AST_Node* get_object(Basic_Value value_node);
//bool get_bool(Basic_Value value_node);
bool is_valid_syntax(std::vector<Token>& tokens, int token_index, std::string& err_msg, bool print_error);
void json_err(const std::string& err_msg, bool print_error);
void json_free(Basic_Value& value);

#endif /* json_h */
