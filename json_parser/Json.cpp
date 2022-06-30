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

void set_root(Parser* parser) {
	Token token = parser->tokens[parser->token_index];
	
	switch (token.type) {
		case Token_Type::STRING_VALUE: {
			Json_Value value;
			value.type = Value_Type::STRING;
			value.value = std::get<std::string>(token.value);
			parser->json_test = value;
			break;
		}
		case Token_Type::NUMBER: {
			Json_Value value;
			value.type = Value_Type::NUMBER;
			value.value = std::get<float>(token.value);
			parser->json_test = value;
			break;
		}
		case Token_Type::BOOL: {
			Json_Value value;
			value.type = Value_Type::BOOL;
			value.value = std::get<bool>(token.value);
			parser->json_test = value;
			break;
		}
		case Token_Type::OPEN_SQUARE_BRACKET: {
			parse_array(parser);
			break;
//			Json_Value arr = parse_array(token, parser);
			
//			Json_Obj* new_array_node = new Json_Obj;
//			new_array_node->type = AST_Node_Type::ARRAY;
//			new_array_node->name = "ARRAY";
//
//
//			Json_Value value;
//			value.type = Value_Type::ARRAY;
//			value.value = new_array_node;
//
//			parser->current_obj = new_array_node;
//			return value;
		}
		case Token_Type::OPEN_CURLY_BRACKET: {
			parse_object(parser);
			// skip don't need to do anything
			break;
//			Json_Obj* new_object_node = new Json_Obj;
//			new_object_node->type = AST_Node_Type::OBJECT;
//			new_object_node->name = "ROOT";
//
//			parser->current_obj = new_object_node;
//
//			Json_Value value;
//			value.type = Value_Type::OBJECT;
//			value.value = new_object_node;
//
//			return value;
		}
		default: {
			Json_Value value;
			value.type = Value_Type::ERROR;
			value.value = "json error: invalid root node";
			parser->json_test = value;
		}
	}
}

void add_value(Parser* parser, Json_Value value) {
	if (parser->test_active_value.type == Value_Type::OBJECT) {
		Json_Obj_Test& obj = *parser->test_active_obj;
		obj[parser->active_name] = value;
	} else if (parser->test_active_value.type == Value_Type::ARRAY) {
		parser->test_active_arr->push_back(value);
	} else {
		assert(false);
	}
}

void parse_token(Token token, Parser* parser) {
	if (parser->has_set_root == false) {
		set_root(parser);
	} else if (token.type == Token_Type::NAME) {
//		Json_Obj_Test obj = std::get<Json_Obj_Test>(parser->json_test.value);
		
		std::string str = std::get<std::string>(token.value);
		
		Json_Obj_Test& obj = *parser->test_active_obj;
		obj[str] = Json_Value{};
		parser->active_name = str;
	} else if (token.type == Token_Type::STRING_VALUE) {
//		Json_Obj_Test obj = std::get<Json_Obj_Test>(parser->json_test.value);
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
			   token.type == Token_Type::COMMA) {
		// skip these
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
//		parser->json_test_obj = obj_value;
		parser->json_test = obj_value;
		parser->has_set_root = true;
	} else {
//		(*parser->test_active_obj)[parser->active_name] = obj_value;
		if (parser->test_active_value.type == Value_Type::OBJECT) {
			(*parser->test_active_obj)[parser->active_name] = obj_value;
		} else if (parser->test_active_value.type == Value_Type::ARRAY) {
			parser->test_active_arr->push_back(obj_value);
		}
	}
	
	Json_Obj_Test* prev = parser->test_active_obj;
	
	if (parser->test_active_value.type != Value_Type::NULL_TYPE) {
		parser->prev_active_value = parser->test_active_value;
	}
	
	parser->test_active_obj = std::get<Json_Obj_Test*>(obj_value.value);
	parser->test_active_value = obj_value;

	parser->token_index++;
	Token current_token = parser->tokens[parser->token_index];
	
	while (current_token.type != Token_Type::CLOSED_CURLY_BRACKET) {
		parse_token(current_token, parser);
		current_token = parser->tokens[parser->token_index];
	}
	
	parser->test_active_obj = prev;
	
	parser->test_active_value = parser->prev_active_value;
	
}

void parse_array(Parser* parser) {
	
	Json_Value arr_value;
	arr_value.type = Value_Type::ARRAY;
	arr_value.value = new Json_Array{};
	
	if (!parser->has_set_root) {
		parser->json_test = arr_value;
		parser->has_set_root = true;
	} else {
		if (parser->test_active_value.type == Value_Type::ARRAY) {
			add_value(parser, arr_value);
		} else if (parser->test_active_value.type == Value_Type::OBJECT) {
			(*parser->test_active_obj)[parser->active_name] = arr_value;
			(*parser->test_active_obj)[parser->active_name] = arr_value;
		}
	}
	
	if (parser->test_active_value.type != Value_Type::NULL_TYPE) {
		parser->prev_active_value = parser->test_active_value;
	}
	
	parser->test_active_arr = arr_value.to_array();
	parser->test_active_value = arr_value;
//	parser->test_active_obj = std::get<Json_Obj_Test*>(obj_value.value);
	
//	if (!parser->has_set_root) {
////		parser->json_test_obj = obj_value;
//		parser->json_test = obj_value;
//		parser->has_set_root = true;
//	}

	parser->token_index++;
	Token current_token = parser->tokens[parser->token_index];
	
	while (current_token.type != Token_Type::CLOSED_SQUARE_BRACKET) {
		parse_token(current_token, parser);
		current_token = parser->tokens[parser->token_index];
	}
	
	parser->test_active_value = parser->prev_active_value;
//
//	while (current_token.type != Token_Type::CLOSED_SQUARE_BRACKET) {
//		if (current_token.type == Token_Type::STRING_VALUE) {
//			Json_Value str_value;
//			str_value.value = std::get<std::string>(current_token.value);
//			str_value.type = Value_Type::STRING;
//			arr.push_back(str_value);
//		}
//	}
	
//	Json_Value value_node;
//
//	switch (token.type) {
//		case Token_Type::STRING_VALUE:
//			value_node.type = Value_Type::STRING;
//			value_node.value = std::get<std::string>(token.value);
//			parser->current_obj->array.push_back(value_node);
//			break;
//		case Token_Type::NUMBER:
//			value_node.type = Value_Type::NUMBER;
//			value_node.value = std::get<float>(token.value);
//			parser->current_obj->array.push_back(value_node);
//			break;
//		case Token_Type::BOOL:
//			value_node.type = Value_Type::BOOL;
//			value_node.value = std::get<bool>(token.value);
//			parser->current_obj->array.push_back(value_node);
//			break;
//		case Token_Type::CLOSED_SQUARE_BRACKET:
//			parser->current_obj = parser->current_obj->parent;
//			break;
//		case Token_Type::OPEN_SQUARE_BRACKET: {
//			Json_Obj* new_array_node = new Json_Obj;
//			new_array_node->type = AST_Node_Type::ARRAY;
//			new_array_node->name = "ARRAY";
//			new_array_node->parent = parser->current_obj;
//
//			value_node.type = Value_Type::ARRAY;
//			value_node.value = new_array_node;
//
//			// since we are in array section push as new item in array
//			assert(parser->current_obj->type == AST_Node_Type::ARRAY);
//			parser->current_obj->array.push_back(value_node);
//
//			parser->current_obj = new_array_node;
//
//			break;
//		}
//		case Token_Type::OPEN_CURLY_BRACKET: {
//			Json_Obj* new_object_node = new Json_Obj;
//			new_object_node->type = AST_Node_Type::OBJECT;
//			new_object_node->name = "UNNAMED";
//			new_object_node->parent = parser->current_obj;
//
//			value_node.type = Value_Type::OBJECT;
//			value_node.value = new_object_node;
//			parser->current_obj->array.push_back(value_node);
//
//			parser->current_obj = new_object_node;
//			break;
//		}
//		default:
//			break;
//	}
}

Json_Value parse_tokens(std::vector<Token>& tokens, Parser* parser, bool print_error) {
//	Json_Value json;
//	int token_index = 0;
	Token current_token = tokens[parser->token_index];
	AST_Pair_Node* current_pair_node = nullptr;
	std::string err_msg;
	
//	Json_Value json_test;
	
	while (current_token.type != Token_Type::END_OF_FILE) {
		
		parse_token(current_token, parser);
		current_token = parser->tokens[parser->token_index];
		
//		if (!is_valid_syntax(tokens, token_index, err_msg, print_error)) {
//			json.type = Value_Type::ERROR;
//			json.value = err_msg;
//			return json;
//		}
//
//		// handle setting the root value
//		if (json.type == Value_Type::NULL_TYPE) {
//			Json_Value root_value = set_root(current_token, parser);
//			if (root_value.type == Value_Type::ERROR) {
//				return json;
//			} else {
//				json = root_value;
//			}
//
//		} // handle arrays
//		else if (parser->current_obj && parser->current_obj->type == AST_Node_Type::ARRAY) {
//
//			parse_array(current_token, parser);
//
//			// handle everything else
//		} else {
//			if (current_token.type == Token_Type::OPEN_CURLY_BRACKET) { // object node
//				assert(parser->current_obj);
//				AST_Pair_Node* pair_node = parser->current_obj->properties[parser->current_obj->properties.size() - 1];
//				pair_node->value_node.type = Value_Type::OBJECT;
//				current_pair_node = pair_node;
//
//				Json_Obj* new_object_node = new Json_Obj;
//				new_object_node->type = AST_Node_Type::OBJECT;
//				new_object_node->name = pair_node->key;
//				new_object_node->parent = parser->current_obj;
//				pair_node->value_node.value = new_object_node;
//
//				parser->current_obj = new_object_node;
//			} else if (current_token.type == Token_Type::NAME) { // name node
//				if (parser->current_obj->type != AST_Node_Type::OBJECT) assert(false);
//
//				AST_Pair_Node* pair_node = new AST_Pair_Node;
//				pair_node->key = std::get<std::string>(current_token.value);
//				pair_node->parent = parser->current_obj;
//				parser->current_obj->properties.push_back(pair_node);
//				current_pair_node = pair_node;
//			} else if (current_token.type == Token_Type::STRING_VALUE) {
//				if (parser->current_obj->type != AST_Node_Type::OBJECT) assert(false);
//
//				AST_Pair_Node* pair_node = parser->current_obj->properties[parser->current_obj->properties.size() - 1];
//
//				Json_Value value_node;
//				value_node.type = Value_Type::STRING;
//				value_node.value = std::get<std::string>(current_token.value);
//
//				pair_node->value_node = value_node;
//			} else if (current_token.type == Token_Type::NUMBER) {
//				if (parser->current_obj->type != AST_Node_Type::OBJECT) assert(false);
//
//				AST_Pair_Node* pair_node = parser->current_obj->properties[parser->current_obj->properties.size() - 1];
//
//				Json_Value value_node;
//				value_node.type = Value_Type::NUMBER;
//				value_node.value = std::get<float>(current_token.value);
//
//				pair_node->value_node = value_node;
//			} else if (current_token.type == Token_Type::BOOL) {
//				if (parser->current_obj->type != AST_Node_Type::OBJECT) assert(false);
//
//				AST_Pair_Node* pair_node = parser->current_obj->properties[parser->current_obj->properties.size() - 1];
//
//				Json_Value value_node;
//				value_node.type = Value_Type::BOOL;
//				value_node.value = std::get<bool>(current_token.value);
//
//				pair_node->value_node = value_node;
//			} else if (current_token.type == Token_Type::OPEN_SQUARE_BRACKET) {
//
//				Json_Obj* new_array_node = new Json_Obj;
//				new_array_node->type = AST_Node_Type::ARRAY;
//				new_array_node->name = "ARRAY";
//				new_array_node->parent = parser->current_obj;
//
//				current_pair_node->value_node.type = Value_Type::ARRAY;
//				current_pair_node->value_node.value = new_array_node;
//
//				parser->current_obj = new_array_node;
//			} else if (current_token.type == Token_Type::CLOSED_SQUARE_BRACKET) {
//				assert(false); // we shouldn't hit here becuase we should deal w/ this in the in_array section
//			} else if (current_token.type == Token_Type::CLOSED_CURLY_BRACKET) {
//				// end of object, set the parent as the current node
//				if (parser->current_obj->parent) {
//					parser->current_obj = parser->current_obj->parent;
//				} else {
//					if (parser->current_obj->name.compare("ROOT") != 0) {
//						// every time we close an object it should have a parent that we revert to unless its the ROOT
//						assert(false);
//					}
//				}
//			} else if (current_token.type == Token_Type::COLON || current_token.type == Token_Type::COMMA) {
//				// ignore
//			} else {
//				assert(false); // we should not reach this point, if we do it means wh have a token type that we are not checking
//			}
//		}
//
//		token_index++;
//		current_token = tokens[token_index];
	}
//
	return parser->json_test;
}

void create_tokens(Parser* parser) {
	while (!parser->eof) {
		consume(parser);
	}
}

Json_Value parse(std::string str, bool print_error) {
	Parser* parser = new Parser { .str = str };
	
	create_tokens(parser);
	Json_Value json = parse_tokens(parser->tokens, parser, print_error);
	
//	parser->json = json;
//	m_parse_tokens(parser->tokens);
	
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


// use this overload to access data withing a Json_Data object
// check for keys that match the string and return their value node
// if there is no matching key then return an empty value
Json_Value& Json_Value::operator[](std::string key) {
	
	Json_Value value;
	
	if (this->type == Value_Type::OBJECT) {
		Json_Obj_Test* obj = this->to_obj();
		value = (*obj)[key];
		
		Json_Value& test = obj->at(key);
		return test;
//		auto test = obj->find(key);
//		return &test->second;
		
		int a = 0;
		(void)a;
	} else {
		assert(false);
	}

	int a = 0;
	(void)a;
	return value;
//	// if we are trying to set a value in a Json object that was just created
//	// it will be of type NULL_TYPE, we need to convert it to an OBJECT type
//	if (this->type == Value_Type::NULL_TYPE) {
//		Json_Obj* new_object_node = new Json_Obj;
//		new_object_node->type = AST_Node_Type::OBJECT;
//		new_object_node->name = "ROOT";
//
//		this->type = Value_Type::OBJECT;
//		this->value = new_object_node;
//	}
//
//	Json_Obj* current_node = nullptr;
//	if (this->type == Value_Type::OBJECT) {
//		current_node = this->to_obj();
//	} else {
//		assert(false);
//	}
//
//	for (AST_Pair_Node* pair_node : current_node->properties) {
//		if (pair_node->key.compare(key) == 0) {
//			return pair_node->value_node;
//		}
//	}
//
//	// if we don't find a matching property name then create a empty property with the key
//	AST_Pair_Node* pair_node = new AST_Pair_Node;
//	pair_node->key = key;
//	pair_node->parent = current_node;
//	current_node->properties.push_back(pair_node);
//
//	Json_Value value_node;
//	value_node.type = Value_Type::EMPTY;
//	value_node.value = 0;
//
//	pair_node->value_node = value_node;
//
//	return pair_node->value_node;
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

// convert json_value to actual c++ type
//
const std::string Json_Value::to_str() { return std::get<std::string>(this->value); }
float Json_Value::to_float() { return std::get<float>(this->value); }
int Json_Value::to_int() { return (int)std::get<float>(this->value); }
//std::vector<Json_Value> Json_Value::to_array() {
//	Json_Obj* array_node = std::get<Json_Obj*>(this->value);
//	return array_node->array;
//}
Json_Array* Json_Value::to_array() { return std::get<Json_Array*>(this->value); }
Json_Obj_Test* Json_Value::to_obj() { return std::get<Json_Obj_Test*>(this->value); }
bool Json_Value::to_bool() { return std::get<bool>(this->value); }


void json_free(Json_Value& value) {
	if (value.type == Value_Type::OBJECT) {
		for (auto& it : (*value.to_obj())) {
			if (it.second.type == Value_Type::OBJECT) {
				json_free(it.second);
			}
		}
		delete value.to_obj();
	} else if (value.type == Value_Type::ARRAY) {
		for (Json_Value& it : (*value.to_array())) {
			if (it.type == Value_Type::OBJECT) {
				json_free(it);
			}
		}
		delete value.to_array();
	}
//	if (value.type == Value_Type::OBJECT) {
//		for (auto property : value.to_obj()->properties) {
//			if (property->value_node.type == Value_Type::OBJECT || property->value_node.type == Value_Type::ARRAY) {
//				json_free(property->value_node);
//			}
//		}
//		auto obj = value.to_obj();
//		delete obj;
//	} else if (value.type == Value_Type::ARRAY) {
//		for (auto item : value.to_array()) {
//			if (item.type == Value_Type::OBJECT || item.type == Value_Type::ARRAY) {
//				json_free(item);
//			}
//		}
//		auto arr = value.to_obj();
//		delete arr;
//	}
}

void Json_Value::operator=(int num) {
	std::string str = std::to_string(num);
	Json_Value json = parse(str);
	
	this->type = Value_Type::NUMBER;
	this->value = json.value;
}

void Json_Value::operator=(std::string str) {
	std::string wrapped_str = '\"' + str + '\"'; // we need to wrap the string in quotes for the parser to view it as a string
	Json_Value json = parse(wrapped_str);
	
	this->type = Value_Type::STRING;
	this->value = json.value;
}

//void Json_Value::operator=(Json_Array arr) {
//	this->type = Value_Type::ARRAY;
////	this->value = arr;
////	Json_Array new_arr;
////	for (Json_Value value : arr) {
////
////	}
//}

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

//std::string get_string_from_value(Json_Value value, int indent) {
//	std::string str;
//	if (value.type == Value_Type::OBJECT) {
//		indent++;
//		str += "{\n";
//		for (auto& prop : value.to_obj()->properties) {
//			do_indent(str, indent);
//			str += "\"";
//			str += prop->key;
//			str += "\"";
//			str += ": ";
//			str += get_string_from_value(prop->value_node, indent);
//
//			if (prop != value.to_obj()->properties.back()) {
//				str += ",\n";
//			}
//		}
//		indent--;
//		str += "\n";
//		do_indent(str, indent);
//		str += '}';
//	} else if (value.type == Value_Type::STRING) {
//		str += "\"";
//		str += value.to_str();
//		str += "\"";
//	} else if (value.type == Value_Type::NUMBER) {
//		if (value.to_float() == ceil(value.to_float())) {
//			str += std::to_string(value.to_int());
//		} else {
//			str += std::to_string(value.to_float());
//		}
//	} else if (value.type == Value_Type::ARRAY) {
//		str += "[ ";
//		Json_Array array = value.to_array();
//		for (int i = 0; i < array.size(); i++) {
//			str += get_string_from_value(array[i], indent);
//			if (i < array.size() - 1) {
//				str += ", ";
//			}
//		}
//		str += " ]";
//	} else if (value.type == Value_Type::BOOL) {
//		if (value.to_bool() == false) {
//			str += "false";
//		} else {
//			str += "true";
//		}
//	} else {
//		assert(false);
//	}
//
//	return str;
//}

//std::string json_to_string(const Json_Value& json) {
//	Json_Value current_value = json;
//	std::string result = get_string_from_value(current_value, 0);
//	printf("%s\n", result.c_str());
//	return result;
//}

//void m_parse_object(std::vector<Token>& tokens, int start_index) {
//	for (int i = start_index; i < tokens.size(); i++) {
//		Token& token = tokens[i];
//		if (token.type == Token_Type::NUMBER) {
//
//		}
//	}
//}
//
//void m_parse_tokens(std::vector<Token>& tokens) {
//	m_json_value json;
//	m_json_value current_value;
//
//	for (int i = 0; i < tokens.size(); i++) {
//		Token& token = tokens[i];
//		if (token.type == Token_Type::OPEN_CURLY_BRACKET) {
//			m_parse_object(tokens, i);
//		}
//	}
//}


/*
 switch (token.type) {
	 case Token_Type::OPEN_CURLY_BRACKET: {
		 m_json_obj new_obj;
		 current_value.type = Value_Type::OBJECT;
		 current_value.value = new_obj;
		 json.type = Value_Type::OBJECT;
		 json.value = new_obj;
	 } break;
	 case Token_Type::CLOSED_CURLY_BRACKET: {
//				assert(false);
//				json.value = current_value;
	 } break;
	 case Token_Type::STRING_VALUE: {
		 assert(false);
	 } break;
	 case Token_Type::NAME: {
		 m_json_obj obj = json.to_obj();
		 std::string name = std::get<std::string>(token.value);
		 obj[name] = m_json_value{};
	 } break;
	 case Token_Type::NUMBER: {
//				json.
//				assert(false);
	 } break;
	 case Token_Type::BOOL: {
		 assert(false);
	 } break;
	 case Token_Type::COLON: {
//				assert(false);
	 } break;
	 case Token_Type::OPEN_SQUARE_BRACKET: {
		 assert(false);
	 } break;
	 case Token_Type::CLOSED_SQUARE_BRACKET: {
		 assert(false);
	 } break;
	 case Token_Type::COMMA: {
		 assert(false);
	 } break;
	 case Token_Type::END_OF_FILE: {
//				assert(false);
	 } break;
	 case Token_Type::COMMENT: {
		 assert(false);
	 } break;
	 case Token_Type::UNTERMINATED_STRING: {
		 assert(false);
	 } break;
	 default:
		 break;
 }
 */
