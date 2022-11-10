//
//  json.cpp
//  json_parser
//
//  Created by Michael Donlan on 8/23/21.
//

#include "Json.h"
#include <fstream>
#include <math.h>

Parser* _parser = new Parser;

void consume(Parser* parser) {
	eat_whitespace(parser);
	char c = parser->str[parser->index];
	
	// match against tokens
	if (c == '\0') {
		parser->eof = true;
		Token token;
		token.type = Token_Type::END_OF_FILE;
		token.value = '\0';
		parser->tokens.push_back(token);
	} else if (c == '{') { // left bracket token
		Token token;
		token.type = Token_Type::OPEN_CURLY_BRACKET;
		token.value = '{';
		parser->tokens.push_back(token);
		parser->index++;
	} else if (c == ':') {
		Token token;
		token.type = Token_Type::COLON;
		token.value = ':';
		parser->tokens.push_back(token);
		parser->index++;
	} else if (c == '[') {
		Token token;
		token.type = Token_Type::OPEN_SQUARE_BRACKET;
		token.value = '[';
		parser->tokens.push_back(token);
		parser->index++;
	} else if (c == ']') {
		Token token;
		token.type = Token_Type::CLOSED_SQUARE_BRACKET;
		token.value = ']';
		parser->tokens.push_back(token);
		parser->index++;
	} else if (c == '}') {
		Token token;
		token.type = Token_Type::CLOSED_CURLY_BRACKET;
		token.value = '}';
		parser->tokens.push_back(token);
		parser->index++;
	} else if (c == ',') {
		Token token;
		token.type = Token_Type::COMMA;
		token.value = ',';
		parser->tokens.push_back(token);
		parser->index++;
	} else if (c == '/') {
		if (peek(parser, parser->index + 1) == '/') {
			// we found the start of a comment, read until end of line char, then throw away? do we want to keep comments?
			std::string str = "/"; // add starting /
			unsigned int p_index = parser->index + 1;
			bool found_end = false;
			while (!found_end) {
				char pc = peek(parser, p_index);
				if (pc == '\n' || pc == '\r') {
					found_end = true;
				} else {
					str += pc;
					p_index++;
				}
			}
			
			parser->index = p_index;
		}
	} else if (c == '"') { // name/decl token or string value token
		Token token;
		std::string str;
		unsigned int p_index = parser->index + 1;
		bool found_end_of_string = false;
		int attempts = 0;
		
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
				
				if (p_index >= parser->str.size()) {
					// hit end of the string w/out finding a closing quote
					found_end_of_string = true;
					token.type = Token_Type::UNTERMINATED_STRING;
				}
			}
			
			if (attempts > 5000) {
				assert(false);
			}
			attempts++;
		}
		
		token.value = str;
		parser->tokens.push_back(token);
		parser->index += str.size() + 2; // +2 for ""
	} else if (isdigit(c)) { // number token
		Token token;
		token.type = Token_Type::NUMBER;
		
		std::string num_str(1, c);
		bool found_end_of_number = false;
		unsigned int p_index = parser->index + 1;
		
		// check if number is negative
		bool is_neg = false;
		char prev_char = peek(parser, parser->index - 1);
		if (prev_char == '-') is_neg = true;
		
		while (!found_end_of_number) {
			char pc = peek(parser, p_index);
			if (!isdigit(pc) && pc != '.') {
				found_end_of_number = true;
			} else {
				num_str += pc;
				p_index++;
			}
		}
		
		float value = std::stof(num_str);
		if (is_neg) value *= -1;
		token.value = value;
		parser->tokens.push_back(token);
		parser->index += num_str.size();
	}
	else {
		if (isalpha(c)) {
			parser->cache += c;
			
			if (parser->cache.compare("false") == 0) { // is there a better way to do BOOLS?
				Token token;
				token.type = Token_Type::BOOL;
				token.value = false;
				parser->tokens.push_back(token);
				parser->cache.clear();
			} else if (parser->cache.compare("true") == 0) {
				Token token;
				token.type = Token_Type::BOOL;
				token.value = true;
				parser->tokens.push_back(token);
				parser->cache.clear();
			}
		}
		
		parser->index++;
	}
}

char peek(Parser* parser, unsigned int index) { return parser->str[index]; }

void eat_whitespace(Parser* parser) {
	char c = parser->str[parser->index];
	while (isspace(c)) {
		parser->index++;
		c = parser->str[parser->index];
	}
}

void set_root(Parser* parser) {
	Token token = parser->tokens[parser->token_index];
	
	switch (token.type) {
		case Token_Type::STRING_VALUE: {
			Json_Value value;
			value.type = Value_Type::STRING;
			value.value = std::get<std::string>(token.value);
			parser->root_value = value;
			parser->has_set_root = true;
			break;
		}
		case Token_Type::NUMBER: {
			Json_Value value;
			value.type = Value_Type::NUMBER;
			value.value = std::get<float>(token.value);
			parser->root_value = value;
			parser->has_set_root = true;
			break;
		}
		case Token_Type::BOOL: {
			Json_Value value;
			value.type = Value_Type::BOOL;
			value.value = std::get<bool>(token.value);
			parser->root_value = value;
			parser->has_set_root = true;
			break;
		}
		case Token_Type::OPEN_SQUARE_BRACKET: {
			parse_array(parser);
			break;
		}
		case Token_Type::OPEN_CURLY_BRACKET: {
			parse_object(parser);
			break;
		}
		default: {
			Json_Value value;
			value.type = Value_Type::ERROR;
			value.value = "json error: invalid root node";
			parser->root_value = value;
			parser->has_set_root = true;
		}
	}
}

void add_value(Parser* parser, Json_Value value) {
	if (parser->active_value.type == Value_Type::OBJECT) {
		parser->active_value.to_obj()[parser->active_name] = value;
	} else if (parser->active_value.type == Value_Type::ARRAY) {
		parser->active_value.to_array().push_back(value);
	} else {
		assert(false);
	}
}

void parse_token(Token token, Parser* parser) {
	std::string err_msg;
	bool print_error = false; // move this to parser?
	if (!is_valid_syntax(parser->tokens, parser->token_index, err_msg, print_error)) {
		Json_Value error_value;
		error_value.type = Value_Type::ERROR;
		error_value.value = err_msg;
		parser->root_value = error_value;
		parser->finished = true;
	}
	
	if (parser->has_set_root == false) {
		set_root(parser);
	} else if (token.type == Token_Type::NAME) {
		std::string str = std::get<std::string>(token.value);
		parser->active_value.to_obj()[str] = Json_Value{};
		parser->active_name = str;
	} else if (token.type == Token_Type::STRING_VALUE) {
		Json_Value string_value;
		string_value.type = Value_Type::STRING;
		string_value.value = std::get<std::string>(token.value);
		add_value(parser, string_value);
	} else if (token.type == Token_Type::OPEN_SQUARE_BRACKET) {
		parse_array(parser);
	} else if (token.type == Token_Type::OPEN_CURLY_BRACKET) {
		parse_object(parser);
	} else if (token.type == Token_Type::NUMBER) {
		Json_Value value;
		value.type = Value_Type::NUMBER;
		value.value = std::get<float>(token.value);
		add_value(parser, value);
	} else if (token.type == Token_Type::BOOL) {
		Json_Value value;
		value.type = Value_Type::BOOL;
		value.value = std::get<bool>(token.value);
		add_value(parser, value);
	} else if (token.type == Token_Type::COLON ||
			   token.type == Token_Type::COMMA ||
			   token.type == Token_Type::UNTERMINATED_STRING) {
		// skip these
	} else if (token.type == Token_Type::END_OF_FILE) {
		parser->finished = true;
	} else {
		assert(false);
	}
	
	parser->token_index++;
}

void parse_object(Parser* parser) {
	Json_Value obj_value;
	obj_value.type = Value_Type::OBJECT;
	obj_value.value = new Json_Obj_Test{};
	
	if (!parser->has_set_root) {
		parser->root_value = obj_value;
		parser->has_set_root = true;
	} else {
		if (parser->active_value.type == Value_Type::OBJECT) {
			parser->active_value.to_obj()[parser->active_name] = obj_value;
		} else if (parser->active_value.type == Value_Type::ARRAY) {
			parser->active_value.to_array().push_back(obj_value);
		}
	}
	
	parser->parents.push_back(parser->active_value);
	parser->active_value = obj_value;

	parser->token_index++;
	Token current_token = parser->tokens[parser->token_index];
	
	while (current_token.type != Token_Type::CLOSED_CURLY_BRACKET && !parser->finished) {
		parse_token(current_token, parser);
		if (parser->token_index > parser->tokens.size() - 1) continue;
		current_token = parser->tokens[parser->token_index];
	}
	
	parser->active_value = parser->parents.back();
	parser->parents.pop_back();
}

void parse_array(Parser* parser) {
	Json_Value arr_value;
	arr_value.type = Value_Type::ARRAY;
	arr_value.value = new Json_Array{};
	
	if (!parser->has_set_root) {
		parser->root_value = arr_value;
		parser->has_set_root = true;
	} else {
		if (parser->active_value.type == Value_Type::ARRAY) {
			add_value(parser, arr_value);
		} else if (parser->active_value.type == Value_Type::OBJECT) {
			parser->active_value.to_obj()[parser->active_name] = arr_value;
		}
	}

	parser->parents.push_back(parser->active_value);
	parser->active_value = arr_value;
	
	parser->token_index++;
	Token current_token = parser->tokens[parser->token_index];
	
	while (current_token.type != Token_Type::CLOSED_SQUARE_BRACKET) {
		parse_token(current_token, parser);
		current_token = parser->tokens[parser->token_index];
	}
	
	if (parser->parents.size() > 0) {
		parser->active_value = parser->parents[parser->parents.size() - 1];
	} else {
		parser->active_value = Json_Value{};
	}
}

Json_Value parse_tokens(std::vector<Token>& tokens, Parser* parser, bool print_error) {
	Token current_token = tokens[parser->token_index];
	std::string err_msg;
	
	while (!parser->finished) {
		parse_token(current_token, parser);
		if (parser->token_index > parser->tokens.size() - 1) continue;
		current_token = parser->tokens[parser->token_index];
	}
	return parser->root_value;
}

void create_tokens(Parser* parser) {
	while (!parser->eof) {
		consume(parser);
	}
}

Json_Value parse(std::string str, bool print_error) {
	Parser* parser = new Parser { .str = str };
	_parser = parser;
	
	create_tokens(parser);
	Json_Value json = parse_tokens(parser->tokens, parser, print_error);
	
	delete parser;
	return json;
}

const std::string load_json_from_file(const std::string& file_name) {
	std::ifstream json_test_file(file_name);
	std::string json_test_str;
	if (!json_test_file.is_open()) {
		printf("ERROR - Failed to open file test_json.json\n");
	}
	json_test_str.assign((std::istreambuf_iterator<char>(json_test_file)), (std::istreambuf_iterator<char>()));

	json_test_file.close();
	
	return json_test_str;
}

Json_Value& Json_Value::operator[](int i) {
	Json_Value value;
	
	if (this->type == Value_Type::ARRAY) {
		Json_Array& arr = this->to_array();
		return arr[i];
	} else {
		assert(false);
	}
}

// use this overload to access data withing a Json_Data object
// check for keys that match the string and return their value node
// if there is no matching key then return an empty value
Json_Value& Json_Value::operator[](std::string key) {
	if (this->type == Value_Type::OBJECT) {
		Json_Obj_Test& obj = this->to_obj();
		
		if (obj.contains(key)) {
			return obj.at(key); // if key exists return ref
		} else {
			// if no key, create it and return ref
			Json_Value value;
			obj.insert({key, value});
			return obj.at(key);
		}
	} else if (this->type == Value_Type::NULL_TYPE) {
		Json_Value value;
		this->type = Value_Type::OBJECT;
		this->value = new Json_Obj_Test{};
		Json_Obj_Test& obj = this->to_obj();
		obj[key] = Json_Value{};
		
		return this->to_obj()[key];
	} else {
		assert(false);
	}
	assert(false);
}

float get_number(Json_Value value_node) { return std::get<float>(value_node.value); }
bool get_bool(Json_Value value_node) { return std::get<bool>(value_node.value); }

bool is_valid_syntax(std::vector<Token>& tokens, int token_index, std::string& err_msg, bool print_error) {
	Token current_token = tokens[token_index];
	Token prev_token;
	
	if (current_token.type == Token_Type::UNTERMINATED_STRING) {
		err_msg = "Unterminated string found.";
		json_err(err_msg, print_error);
		return false;
	}
	
	if (token_index == 0) { // on first token
		if (current_token.type == Token_Type::OPEN_CURLY_BRACKET ||
			current_token.type == Token_Type::STRING_VALUE ||
			current_token.type == Token_Type::OPEN_SQUARE_BRACKET ||
			current_token.type == Token_Type::NUMBER ||
			current_token.type == Token_Type::BOOL) {
			return true;
		} else {
			err_msg = "Invalid token at index 0.";
			json_err(err_msg, print_error);
		}
	} else {
		prev_token = tokens[token_index - 1];
		if (current_token.type == Token_Type::COLON) {
			if (prev_token.type == Token_Type::NAME) {
				return true;
			} else {
				err_msg = "Invalid char ':'.";
				json_err(err_msg, print_error);
			}
		} else if (current_token.type == Token_Type::NAME) {
			if (prev_token.type == Token_Type::COMMA ||
				prev_token.type == Token_Type::OPEN_CURLY_BRACKET) {
				return true;
			} else {
				err_msg = "Invalid token before a Name.";
				json_err(err_msg, print_error);
			}
		} else if (current_token.type == Token_Type::COMMA) {
			if (prev_token.type == Token_Type::NUMBER ||
				prev_token.type == Token_Type::STRING_VALUE ||
				prev_token.type == Token_Type::BOOL ||
				prev_token.type == Token_Type::CLOSED_CURLY_BRACKET ||
				prev_token.type == Token_Type::CLOSED_SQUARE_BRACKET) {
				return true;
			} else {
				err_msg = "Invalid token ','.";
				json_err(err_msg, print_error);
			}
		} else if (current_token.type == Token_Type::CLOSED_CURLY_BRACKET) {
			if (prev_token.type == Token_Type::NUMBER ||
				prev_token.type == Token_Type::STRING_VALUE ||
				prev_token.type == Token_Type::BOOL ||
				prev_token.type == Token_Type::CLOSED_SQUARE_BRACKET ||
				prev_token.type == Token_Type::CLOSED_CURLY_BRACKET ||
				prev_token.type == Token_Type::OPEN_CURLY_BRACKET) {
				return true;
			} else {
				err_msg = "Invalid token '}'.";
				json_err(err_msg, print_error);
			}
		} else if (current_token.type == Token_Type::STRING_VALUE) {
			if (prev_token.type == Token_Type::OPEN_SQUARE_BRACKET ||
				prev_token.type == Token_Type::COMMA ||
				prev_token.type == Token_Type::CLOSED_SQUARE_BRACKET ||
				prev_token.type == Token_Type::COLON) {
				return true;
			} else {
				err_msg = "Invalid token before String Value.";
				json_err(err_msg, print_error);
			}
		} else if (current_token.type == Token_Type::CLOSED_SQUARE_BRACKET) {
			if (prev_token.type == Token_Type::STRING_VALUE ||
				prev_token.type == Token_Type::OPEN_SQUARE_BRACKET ||
				prev_token.type == Token_Type::CLOSED_SQUARE_BRACKET ||
				prev_token.type == Token_Type::CLOSED_CURLY_BRACKET ||
				prev_token.type == Token_Type::COMMA ||
				prev_token.type == Token_Type::BOOL ||
				prev_token.type == Token_Type::NUMBER) {
				return true;
			} else {
				err_msg = "Invalid token ']'.";
				json_err(err_msg, print_error);
			}
		} else if (current_token.type == Token_Type::OPEN_SQUARE_BRACKET) {
			if (prev_token.type == Token_Type::OPEN_SQUARE_BRACKET ||
				prev_token.type == Token_Type::COLON) {
				return true;
			} else {
				err_msg = "Invalid token '['.";
				json_err(err_msg, print_error);
			}
		} else if (current_token.type == Token_Type::OPEN_CURLY_BRACKET) {
			if (prev_token.type == Token_Type::OPEN_SQUARE_BRACKET ||
				prev_token.type == Token_Type::COMMA ||
				prev_token.type == Token_Type::COLON) {
				return true;
			} else {
				err_msg = "Invalid token '{'.";
				json_err(err_msg, print_error);
			}
		} else if (current_token.type == Token_Type::BOOL) {
			if (prev_token.type == Token_Type::OPEN_SQUARE_BRACKET ||
				prev_token.type == Token_Type::COMMA ||
				prev_token.type == Token_Type::COLON) {
				return true;
			} else {
				err_msg = "Unexpectd bool value";
				json_err(err_msg, print_error);
			}
		} else if (current_token.type == Token_Type::NUMBER) {
			if (prev_token.type == Token_Type::COLON ||
				prev_token.type == Token_Type::COMMA ||
				prev_token.type == Token_Type::OPEN_SQUARE_BRACKET) {
				return true;
			} else {
				err_msg = "Invalid token before Number";
				json_err(err_msg, print_error);
			}
		} else if (current_token.type == Token_Type::END_OF_FILE) {
			return true;
		} else {
			assert(false);
		}
	}

	return false;
}

void json_err(const std::string& err_msg, bool print_error) {
	if (print_error) {
		printf("JSON_ERROR: %s\n", err_msg.c_str());
	}
}

const std::string Json_Value::to_str() { return std::get<std::string>(this->value); }
float Json_Value::to_float() { return std::get<float>(this->value); }
int Json_Value::to_int() { return *std::get_if<float>(&this->value); }
Json_Array& Json_Value::to_array() { return (*std::get<Json_Array*>(this->value)); }
Json_Obj_Test& Json_Value::to_obj() { return (*std::get<Json_Obj_Test*>(this->value)); }
bool Json_Value::to_bool() { return std::get<bool>(this->value); }

void json_free(Json_Value& value) {
	if (value.type == Value_Type::OBJECT) {
		for (auto& it : value.to_obj()) {
			if (it.second.type == Value_Type::OBJECT) {
				json_free(it.second);
			}
		}
		delete &value.to_obj();
	} else if (value.type == Value_Type::ARRAY) {
		for (Json_Value& it : value.to_array()) {
			if (it.type == Value_Type::OBJECT) {
				json_free(it);
			}
		}
		delete &value.to_array();
	}
}

void Json_Value::operator=(int num) {
	this->type = Value_Type::NUMBER;
	this->value = (float)num;
}

void Json_Value::operator=(std::string str) {
	std::string wrapped_str = '\"' + str + '\"'; // we need to wrap the string in quotes for the parser to view it as a string
	Json_Value json = parse(wrapped_str);
	
	this->type = Value_Type::STRING;
	this->value = json.value;
}

void write_json(std::string json_str, std::string filename) {
	std::ofstream file(filename);
	  if (file.is_open()) {
		  file << json_str.c_str();
		  file.close();
	  }
	  else {
		  printf("Unable to open file.\n");
	  }
}
