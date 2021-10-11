//
//  json.cpp
//  json_parser
//
//  Created by Michael Donlan on 8/23/21.
//

#include "Json.h"
#include <fstream>
#include <math.h>

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
//			printf("comment str: %s\n", str.c_str());
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
		
		while (!found_end_of_number) {
			char pc = peek(parser, p_index);
			if (!isdigit(pc) && pc != '.') {
				found_end_of_number = true;
			} else {
				num_str += pc;
				p_index++;
			}
		}
		
		token.value = std::stof(num_str);
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

char peek(Parser* parser, unsigned int index) {
	return parser->str[index];
}

void eat_whitespace(Parser* parser) {
	char c = parser->str[parser->index];
	while (isspace(c)) {
		parser->index++;
		c = parser->str[parser->index];
	}
}

Json parse_tokens(std::vector<Token>& tokens, bool print_error) {
	Json json;
	int token_index = 0;
	Token current_token = tokens[token_index];
	Json_Obj* current_node = nullptr;
	AST_Pair_Node* current_pair_node = nullptr;
	std::string err_msg;
	
	while (current_token.type != Token_Type::END_OF_FILE) {
		
		if (is_valid_syntax(tokens, token_index, err_msg, print_error)) {
			
		} else {
			json.value.type = Value_Type::ERROR;
			json.value.value = err_msg;
			return json;
		}
		
		// handle setting the root value
		if (current_node == nullptr) {
			switch (current_token.type) {
				case Token_Type::STRING_VALUE: {
					Json_Value value;
					value.type = Value_Type::STRING;
					value.value = std::get<std::string>(current_token.value);
			
					json.value = value;
					break;
				}
				case Token_Type::NUMBER: {
					Json_Value value;
					value.type = Value_Type::NUMBER;
					value.value = std::get<float>(current_token.value);
					
					json.value = value;
					break;
				}
				case Token_Type::BOOL: {
					Json_Value value;
					value.type = Value_Type::BOOL;
					value.value = std::get<bool>(current_token.value);
					
					json.value = value;
					
					break;
				}
				case Token_Type::CLOSED_SQUARE_BRACKET:
					assert(false);
					break;
				case Token_Type::OPEN_SQUARE_BRACKET: {
					Json_Obj* new_array_node = new Json_Obj;
					new_array_node->type = AST_Node_Type::ARRAY;
					new_array_node->name = "ARRAY";
					new_array_node->parent = current_node;
					
					Json_Value value;
					value.type = Value_Type::ARRAY;
					value.value = new_array_node;
					
					current_node = new_array_node;
					json.value = value;
					
					break;
				}
				case Token_Type::OPEN_CURLY_BRACKET: {
					Json_Obj* new_object_node = new Json_Obj;
					new_object_node->type = AST_Node_Type::OBJECT;
					new_object_node->name = "ROOT";
					
					Json_Value value;
					value.type = Value_Type::OBJECT;
					value.value = new_object_node;
					json.value = value;

					current_node = new_object_node;
					break;
				}
				case Token_Type::COLON: {
					assert(false);
					break;
				}
				case Token_Type::CLOSED_CURLY_BRACKET: {
					assert(false);
					break;
				}
				case Token_Type::NAME: {
					assert(false);
					break;
				}
				default:
					assert(false);
					break;
			}
		} // handle arrays
		else if (current_node && current_node->type == AST_Node_Type::ARRAY) {
			Json_Value value_node;
			
			switch (current_token.type) {
				case Token_Type::STRING_VALUE:
					value_node.type = Value_Type::STRING;
					value_node.value = std::get<std::string>(current_token.value);
					current_node->array.push_back(value_node);
					break;
				case Token_Type::NUMBER:
					value_node.type = Value_Type::NUMBER;
					value_node.value = std::get<float>(current_token.value);
					current_node->array.push_back(value_node);
					break;
				case Token_Type::BOOL:
					value_node.type = Value_Type::BOOL;
					value_node.value = std::get<bool>(current_token.value);
					current_node->array.push_back(value_node);
					break;
				case Token_Type::CLOSED_SQUARE_BRACKET:
					current_node = current_node->parent;
					break;
				case Token_Type::OPEN_SQUARE_BRACKET: {
					Json_Obj* new_array_node = new Json_Obj;
					new_array_node->type = AST_Node_Type::ARRAY;
					new_array_node->name = "ARRAY";
					new_array_node->parent = current_node;
					
					value_node.type = Value_Type::ARRAY;
					value_node.value = new_array_node;
					
					// since we are in array section push as new item in array
					assert(current_node->type == AST_Node_Type::ARRAY);
					current_node->array.push_back(value_node);
					
					current_node = new_array_node;
					
					break;
				}
				case Token_Type::OPEN_CURLY_BRACKET: {
					Json_Obj* new_object_node = new Json_Obj;
					new_object_node->type = AST_Node_Type::OBJECT;
					new_object_node->name = "UNNAMED";
					new_object_node->parent = current_node;
					
					value_node.type = Value_Type::OBJECT;
					value_node.value = new_object_node;
					current_node->array.push_back(value_node);
					
					current_node = new_object_node;
					break;
				}
				default:
					break;
			}
			
			// handle everything else
		} else {
			if (current_token.type == Token_Type::OPEN_CURLY_BRACKET) { // object node
				
				// setup root node
				if (!current_node) {
					Json_Obj* root_node = new Json_Obj;
					root_node->type = AST_Node_Type::OBJECT;
					root_node->name = "ROOT";
					json.value.value = root_node;
					current_node = root_node;
				} else {
					assert(current_node);
					AST_Pair_Node* pair_node = current_node->properties[current_node->properties.size() - 1];
					pair_node->value_node.type = Value_Type::OBJECT;
					current_pair_node = pair_node;
					
					Json_Obj* new_object_node = new Json_Obj;
					new_object_node->type = AST_Node_Type::OBJECT;
					new_object_node->name = pair_node->key;
					new_object_node->parent = current_node;
					pair_node->value_node.value = new_object_node;
					
					current_node = new_object_node;
				}
				
				
			} else if (current_token.type == Token_Type::NAME) { // name node
				if (current_node->type != AST_Node_Type::OBJECT) assert(false);
				
				AST_Pair_Node* pair_node = new AST_Pair_Node;
				pair_node->key = std::get<std::string>(current_token.value);
				pair_node->parent = current_node;
				current_node->properties.push_back(pair_node);
				current_pair_node = pair_node;
			} else if (current_token.type == Token_Type::STRING_VALUE) {
				if (current_node->type != AST_Node_Type::OBJECT) assert(false);
				
				AST_Pair_Node* pair_node = current_node->properties[current_node->properties.size() - 1];
				
				Json_Value value_node;
				value_node.type = Value_Type::STRING;
				value_node.value = std::get<std::string>(current_token.value);
				
				pair_node->value_node = value_node;
			} else if (current_token.type == Token_Type::NUMBER) {
				if (current_node->type != AST_Node_Type::OBJECT) assert(false);
				
				AST_Pair_Node* pair_node = current_node->properties[current_node->properties.size() - 1];
				
				Json_Value value_node;
				value_node.type = Value_Type::NUMBER;
				value_node.value = std::get<float>(current_token.value);
				
				pair_node->value_node = value_node;
			} else if (current_token.type == Token_Type::BOOL) {
				if (current_node->type != AST_Node_Type::OBJECT) assert(false);
				
				AST_Pair_Node* pair_node = current_node->properties[current_node->properties.size() - 1];
				
				Json_Value value_node;
				value_node.type = Value_Type::BOOL;
				value_node.value = std::get<bool>(current_token.value);
				
				pair_node->value_node = value_node;
			} else if (current_token.type == Token_Type::OPEN_SQUARE_BRACKET) {
				
				Json_Obj* new_array_node = new Json_Obj;
				new_array_node->type = AST_Node_Type::ARRAY;
				new_array_node->name = "ARRAY";
				new_array_node->parent = current_node;
				
				current_pair_node->value_node.type = Value_Type::ARRAY;
				current_pair_node->value_node.value = new_array_node;
				
				current_node = new_array_node;
			} else if (current_token.type == Token_Type::CLOSED_SQUARE_BRACKET) {
				assert(false); // we shouldn't hit here becuase we should deal w/ this in the in_array section
			} else if (current_token.type == Token_Type::CLOSED_CURLY_BRACKET) {
				// end of object, set the parent as the current node
				if (current_node->parent) {
					current_node = current_node->parent;
				} else {
					if (current_node->name.compare("ROOT") != 0) {
						// every time we close an object it should have a parent that we revert to unless its the ROOT
						assert(false);
					}
				}
			} else if (current_token.type == Token_Type::COLON) {
	//			assert(false);
			} else if (current_token.type == Token_Type::COMMA) {
				
			} else {
				assert(false); // we should not reach this point, if we do it means wh have a token type that we are not checking
			}
		}
		
		token_index++;
		current_token = tokens[token_index];
	}

	return json;
}

void lex(Parser* parser) {
	while (!parser->eof) {
		consume(parser);
	}
}

Json parse(std::string str, bool print_error) {
	Parser* parser = new Parser;
	parser->str = str;
	lex(parser);
	Json json_data;
	json_data = parse_tokens(parser->tokens, print_error);
	
	delete parser;
	return json_data;
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


// use this overload to access data withing a Json_Data object
// check for keys that match the string and return their value node
// if there is no matching key then return an empty value
Json_Value& Json::operator[](std::string key) {
	Json_Obj* current_node = nullptr;
	if (this->value.type == Value_Type::OBJECT) {
		current_node = this->value.to_obj();
	}

	for (AST_Pair_Node* pair_node : current_node->properties) {
		if (pair_node->key.compare(key) == 0) {
			return pair_node->value_node;
		}
	}
	
	// if we don't find a matching property name then create a empty property with the key
	AST_Pair_Node* pair_node = new AST_Pair_Node;
	pair_node->key = key;
	pair_node->parent = current_node;
	current_node->properties.push_back(pair_node);
	
	Json_Value value_node;
	value_node.type = Value_Type::EMPTY;
	value_node.value = 0;
	
	pair_node->value_node = value_node;
	
	return pair_node->value_node;
}

// use this overload to access data withing a AST_Value_Node object
// check for keys that match the string and return their value node
Json_Value Json_Value::operator[](std::string key) {
	Json_Value value_node;
	
	if (this->type == Value_Type::ARRAY) {
		
	} else if (this->type == Value_Type::OBJECT) {
		auto node = std::get<Json_Obj*>(this->value);
		for (AST_Pair_Node* pair_node : node->properties) {
			if (pair_node->key.compare(key) == 0) {
				return pair_node->value_node;
			}
		}
	}
	
	assert(false);
	return value_node;
}

float get_number(Json_Value value_node) {
	return std::get<float>(value_node.value);
}

bool get_bool(Json_Value value_node) {
	return std::get<bool>(value_node.value);
}

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
				prev_token.type == Token_Type::BOOL) {
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
			if (prev_token.type == Token_Type::COLON) {
				return true;
			} else {
				err_msg = "Invalid token before Number";
				json_err(err_msg, print_error);
			}
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

const std::string Json_Value::to_str() {
	return std::get<std::string>(this->value);
}

float Json_Value::to_float() {
	return std::get<float>(this->value);
}

int Json_Value::to_int() {
	return (int)std::get<float>(this->value);
}

std::vector<Json_Value> Json_Value::to_array() {
	Json_Obj* array_node = std::get<Json_Obj*>(this->value);
	return array_node->array;
}

Json_Obj* Json_Value::to_obj() {
	return std::get<Json_Obj*>(this->value);
}
bool Json_Value::to_bool() {
	return std::get<bool>(this->value);
}

void json_free(Json_Value& value) {
	if (value.type == Value_Type::OBJECT) {
		for (auto property : value.to_obj()->properties) {
			if (property->value_node.type == Value_Type::OBJECT || property->value_node.type == Value_Type::ARRAY) {
				json_free(property->value_node);
			}
		}
		auto obj = value.to_obj();
		delete obj;
	} else if (value.type == Value_Type::ARRAY) {
		for (auto item : value.to_array()) {
			if (item.type == Value_Type::OBJECT || item.type == Value_Type::ARRAY) {
				json_free(item);
			}
		}
		auto arr = value.to_obj();
		delete arr;
	}
}

void Json_Value::operator=(int num) {
	std::string str = std::to_string(num);
	Json json = parse(str);
	
	this->type = Value_Type::NUMBER;
	this->value = json.value.value;
}

void Json_Value::operator=(std::string str) {
	std::string wrapped_str = '\"' + str + '\"'; // we need to wrap the string in quotes for the parser to view it as a string
	Json json = parse(wrapped_str);
	
	this->type = Value_Type::STRING;
	this->value = json.value.value;
}

void print_value(Json_Value value) {
	if (value.type == Value_Type::STRING) {
		printf("%s\n", value.to_str().c_str());
	} else if (value.type == Value_Type::NUMBER) {
		printf("%d\n", value.to_int());
	}
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

void do_indent(std::string& str, int indent) {
	for (int i = 0; i < indent; i++) {
		str += '\t';
	}
}

std::string get_string_from_value(Json_Value value, int indent) {
	std::string str;
	if (value.type == Value_Type::OBJECT) {
		indent++;
		str += "{\n";
		for (auto& prop : value.to_obj()->properties) {
			do_indent(str, indent);
			str += "\"";
			str += prop->key;
			str += "\"";
			str += ": ";
			str += get_string_from_value(prop->value_node, indent);
			
			if (prop != value.to_obj()->properties.back()) {
				str += ",\n";
			}
		}
		indent--;
		str += "\n";
		do_indent(str, indent);
		str += '}';
	} else if (value.type == Value_Type::STRING) {
		str += "\"";
		str += value.to_str();
		str += "\"";
	} else if (value.type == Value_Type::NUMBER) {
		if (value.to_float() == ceil(value.to_float())) {
			str += std::to_string(value.to_int());
		} else {
			str += std::to_string(value.to_float());
		}
	}
	
	return str;
}

std::string json_to_string(const Json& json) {
	Json_Value current_value = json.value;
	std::string result = get_string_from_value(current_value, 0);
	printf("%s\n", result.c_str());
	return result;
}
