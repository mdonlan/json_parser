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

struct Json_Value;
typedef std::vector<Json_Value> Json_Array;

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

typedef std::map<std::string, Json_Value> Json_Object;

struct Json_Value {
	Json_Value(){};
	Json_Value(int num) { value = num; type = Value_Type::NUMBER;};
	Json_Value(std::string str) { value = str; type = Value_Type::STRING;};
	Json_Value(Json_Object* obj) { value = obj; type = Value_Type::OBJECT;};
	Json_Value(Json_Array* arr) { value = arr; type = Value_Type::ARRAY;};
	Value_Type type = Value_Type::NULL_TYPE;
	std::variant<std::monostate, std::string, int, float, bool, Json_Array*, Json_Object*> value = 0;
	Json_Value& operator[](std::string key);
	Json_Value& operator[](int i);
	void operator=(std::string str);
	void operator=(int num);
	void operator=(Json_Object* obj);
	void operator=(bool val);
	void operator=(float val);
	void operator=(unsigned int val);
	void operator=(const char* val);
	Json_Value& operator[](const char key[]);
	const std::string to_str();
	float to_float();
	int to_int();
	Json_Array& to_array();
	Json_Object& to_obj();
	bool to_bool();
	
	// new operators for implicit conversion
	operator float() const {return std::get<float>(value); };
	operator int() const {return std::get<float>(value); };
	operator size_t() const {return std::get<float>(value); };
	operator std::string() const {return std::get<std::string>(value); };
};

struct Parser {
	unsigned int index = 0;
	bool eof = false;
	std::string str;
	std::vector<Token> tokens;
	std::string cache; // chars left that were not matching anything
	Json_Value root_value;
	Json_Value active_value;
	std::string active_name; // the last name set
	unsigned int token_index = 0;
	bool has_set_root = false;
	std::vector<Json_Value> parents;
	bool finished = false; // is the parser finished parsing
};

extern Parser* _parser;

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
void write_json(std::string json_str, std::string filename);
std::string json_to_string(const Json_Value& json);
void do_indent(std::string& str, int indent);
std::string get_string_from_value(Json_Value value, int indent);

// new parsing stuff...
void set_root(Parser* parser);
void parse_array(Parser* parser);
void parse_token(Token token, Parser* parser);
void parse_object(Parser* parser);
void add_value(Parser* parser, Json_Value value);

#endif /* json_h */
