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
#include <unordered_map>

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
	COMMA,
	END_OF_FILE,
	UNTERMINATED_STRING,
	COMMENT
};

struct Token {
	Token_Type type;
	std::variant<std::string, float, bool, char> value;
};

struct Json_Obj;
struct Json_Value;
typedef std::vector<Json_Value> Json_Array;

struct Parser {
	unsigned int index = 0;
	bool eof = false;
	std::string str;
	std::vector<Token> tokens;
	std::string cache; // chars left that were not matching anything
	Json_Obj* current_obj = nullptr;
	Json_Array* current_arr = nullptr;
};

enum class AST_Node_Type {
	OBJECT,
	ARRAY
};

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

struct Json_Value {
	Value_Type type = Value_Type::NULL_TYPE;
	std::variant<std::string, int, float, bool, Json_Obj*, Json_Array*> value = 0;
	Json_Value& operator[](std::string key);
	void operator=(std::string str);
	void operator=(int num);
	const std::string to_str();
	float to_float();
	int to_int();
	std::vector<Json_Value> to_array();
	Json_Obj* to_obj();
	bool to_bool();
};

struct AST_Pair_Node;

struct Json_Obj {
	AST_Node_Type type;
	std::string name;
	std::vector<AST_Pair_Node*> properties;
	std::vector<Json_Value> array;
//	std::unordered_map<std::string, Json_Value> props;
	Json_Obj* parent;
};

typedef std::map<std::string, Json_Value> Json_Obj_Test;

struct AST_Pair_Node {
	std::string key;
	Json_Value value_node;
	Json_Obj* parent;
};


///* new stuff */
//struct m_json_value;
//typedef std::unordered_map<std::string, m_json_value> m_json_obj;
//struct m_json_value {
//	Value_Type type = Value_Type::NULL_TYPE;
//	std::variant<std::string, int, float, bool, m_json_obj, Json_Array*> value = 0;
////	Json_Value& operator[](std::string key);
////	void operator=(std::string str);
////	void operator=(int num);
////	const std::string to_str();
////	float to_float();
////	int to_int();
////	std::vector<Json_Value> to_array();
//	m_json_obj to_obj() {return std::get<m_json_obj>(this->value);};
////	bool to_bool();
//};
//void m_parse_tokens(std::vector<Token>& tokens);
///* end new stuff */

void create_tokens(Parser* parser);
Json_Value parse(std::string str, bool print_error = true); // print_error -- can turn off for dev/testing purposes w/ expected errors
void consume(Parser* parser);
char peek(Parser* parser, unsigned int index);
void eat_whitespace(Parser* parser);
Json_Value parse_tokens(std::vector<Token>& tokens, Parser* parser, bool print_error);
const std::string load_json_from_file(const std::string& file_name);
bool is_valid_syntax(std::vector<Token>& tokens, int token_index, std::string& err_msg, bool print_error);
void json_err(const std::string& err_msg, bool print_error);
void json_free(Json_Value& value);
void print_value(Json_Value value);
void write_json(std::string json_str, std::string filename);
std::string json_to_string(const Json_Value& json);

// new parsing stuff...
Json_Value set_root(Token token, Parser* parser);
void parse_array(Token token, Parser* parser);


#endif /* json_h */
