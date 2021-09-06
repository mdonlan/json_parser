//
//  json.cpp
//  json_parser
//
//  Created by Michael Donlan on 8/23/21.
//

#include "json.h"
#include <fstream>

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
		// start of array of items
		Token token;
		token.type = Token_Type::OPEN_SQUARE_BRACKET;
		token.value = '[';
		parser->tokens.push_back(token);
		parser->index++;
	} else if (c == ']') {
		// end of array of items
		Token token;
		token.type = Token_Type::CLOSED_SQUARE_BRACKET;
		token.value = ']';
		parser->tokens.push_back(token);
		parser->index++;
	} else if (c == '}') {
		// handle end of object, do we actually need to do anything here?
//		assert(false);
		Token token;
		token.type = Token_Type::CLOSED_CURLY_BRACKET;
		token.value = '}';
		parser->tokens.push_back(token);
		parser->index++;
	} else if (c == '"') { // name/decl token or string value token
		Token token;
		
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
			if (!isdigit(pc)) {
				found_end_of_number = true;
			} else {
				num_str += pc;
				p_index++;
			}
		}
		
		token.value = (float)std::stoi(num_str);
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
				parser->index++;
				parser->cache.clear();
			} else if (parser->cache.compare("true") == 0) {
				Token token;
				token.type = Token_Type::BOOL;
				token.value = true;
				parser->tokens.push_back(token);
				parser->index++;
				parser->cache.clear();
			}
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
	while (isspace(c)) {
		parser->index++;
		c = parser->str[parser->index];
	}
//	while (c == ' ' || c == '\n' || c == '\t') {
//		parser->index++;
//		c = parser->str[parser->index];
//	}
	
}

AST* create_ast(std::vector<Token>& tokens) {
	AST* ast = new AST;
	
	int token_index = 0;
	Token current_token = tokens[token_index];
	
//	AST_Node* root_node = new AST_Node;
//	root_node->type = AST_Node_Type::ROOT;
//	ast->root = root_node;
	
	AST_Node* current_node = nullptr;
	AST_Pair_Node* current_pair_node = nullptr;
//	bool in_array = false;
//	std::vector<AST_Value_Node>* current_array;
//	AST_Node* current_array_node = nullptr;
	
	while (current_token.type != Token_Type::END_OF_FILE) {
		
		// handle arrays
		if (current_node && current_node->type == AST_Node_Type::ARRAY) {
			AST_Value_Node value_node;
//			current_array = std::get<std::vector<AST_Value_Node>>(current_pair_node->value_node.value);
			
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
//					assert(false);
//					current_node->array_nest_level--;
//					assert(current_node->array_nest_level >= 0);
//					if (current_node == 0) {
//						current_node->in_array = false;
//					}
//					current_node->in_array = false;
					break;
				case Token_Type::OPEN_SQUARE_BRACKET: {
//					value_node.type = Value_Type::ARRAY;
//					current_array->push_back(value_node);
//					current_array = &std::get<std::vector<AST_Value_Node>>(current_pair_node->value_node.value);
					AST_Node* new_array_node = new AST_Node;
					new_array_node->type = AST_Node_Type::ARRAY;
					new_array_node->name = "ARRAY";
					new_array_node->parent = current_node;
					
					value_node.type = Value_Type::ARRAY;
					value_node.value = new_array_node;
					
					
					// since we are in array section push as new item in array
					assert(current_node->type == AST_Node_Type::ARRAY);
					current_node->array.push_back(value_node);
					
					current_node = new_array_node;
					
//					current_node->array_nest_level++;
//					value_node.type = Value_Type::ARRAY;
//					value_node.value = std::vector<AST_Value_Node>{};
//					current_array->push_back(value_node);
//					current_array = &std::get<std::vector<AST_Value_Node>>(current_array->back().value);
					break;
				}
				case Token_Type::OPEN_CURLY_BRACKET: {
					AST_Node* new_object_node = new AST_Node;
					new_object_node->type = AST_Node_Type::OBJECT;
					new_object_node->name = "UNNAMED";
					new_object_node->parent = current_node;
//					pair_node->value_node.value = new_object_node;
					
		//			current_node->children.push_back(node);
					
					
					value_node.type = Value_Type::OBJECT;
					value_node.value = new_object_node;
					current_node->array.push_back(value_node);
					
					current_node = new_object_node;
//					AST_Pair_Node* pair_node = current_node->properties[current_node->properties.size() - 1];
//					pair_node->value_node.type = Value_Type::OBJECT;
//					current_pair_node = pair_node;
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
					AST_Node* root_node = new AST_Node;
					root_node->type = AST_Node_Type::OBJECT;
					root_node->name = "ROOT";
					ast->root = root_node;
					current_node = root_node;
				} else {
					AST_Pair_Node* pair_node = current_node->properties[current_node->properties.size() - 1];
					pair_node->value_node.type = Value_Type::OBJECT;
					current_pair_node = pair_node;
					
					AST_Node* new_object_node = new AST_Node;
					new_object_node->type = AST_Node_Type::OBJECT;
					new_object_node->name = pair_node->key;
					new_object_node->parent = current_node;
					pair_node->value_node.value = new_object_node;
					
		//			current_node->children.push_back(node);
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
				
				AST_Value_Node value_node;
				value_node.type = Value_Type::STRING;
				value_node.value = std::get<std::string>(current_token.value);
				
				pair_node->value_node = value_node;
			} else if (current_token.type == Token_Type::NUMBER) {
				if (current_node->type != AST_Node_Type::OBJECT) assert(false);
				
				AST_Pair_Node* pair_node = current_node->properties[current_node->properties.size() - 1];
				
				AST_Value_Node value_node;
				value_node.type = Value_Type::NUMBER;
				value_node.value = std::get<float>(current_token.value);
				
				pair_node->value_node = value_node;
			} else if (current_token.type == Token_Type::BOOL) {
				if (current_node->type != AST_Node_Type::OBJECT) assert(false);
				
				AST_Pair_Node* pair_node = current_node->properties[current_node->properties.size() - 1];
				
				AST_Value_Node value_node;
				value_node.type = Value_Type::BOOL;
				value_node.value = std::get<bool>(current_token.value);
				
				pair_node->value_node = value_node;
			} else if (current_token.type == Token_Type::OPEN_SQUARE_BRACKET) {
				
				AST_Node* new_array_node = new AST_Node;
				new_array_node->type = AST_Node_Type::ARRAY;
				new_array_node->name = "ARRAY";
				new_array_node->parent = current_node;
				
//				value_node.type = Value_Type::OBJECT;
//				value_node.value = new_array_node;
				
				
//				current_node->in_array = true;
//				current_node->array_nest_level++;
				current_pair_node->value_node.type = Value_Type::ARRAY;
				current_pair_node->value_node.value = new_array_node;
				
				current_node = new_array_node;
//				current_array = &std::get<std::vector<AST_Value_Node>>(current_pair_node->value_node.value);
	//			AST_Pair_Node* pair_node = current_node->properties[current_node->properties.size() - 1];
	//			pair_node->value_node.type = Value_Type::ARRAY;
	////			node->type = AST_Node_Type::ar
	//			assert(false);
			} else if (current_token.type == Token_Type::CLOSED_SQUARE_BRACKET) {
				assert(false); // we shouldn't hit here becuase we should deal w/ this in the in_array section
//				assert(in_array); // we should not be trying to close out of a square bracket if not inside an array
//				in_array = false;
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
			} else {
				assert(false); // we should not reach this point, if we do it means wh have a token type that we are not checking
			}
		}
		
		
		
		token_index++;
		current_token = tokens[token_index];
	}
	
	return ast;
}

void print_array(std::vector<AST_Value_Node> array, int indent) {
	printf("[\n");
	for (int i = 0; i < indent + 1; i++) {
		printf("	");
	}
	for (int i = 0; i < array.size(); i++) {
		AST_Value_Node& node = array[i];
//		for (int i = 0; i < indent; i++) {
//			printf("	");
//		}
		print_value(node);
		if (i < array.size() - 1) {
			printf(", ");
		}
		
//		if (node.type == Value_Type::NUMBER) {
//			Print_Data data{.number = node.number};
//			pretty_print(indent, Print_Type::NUMBER, data);
//		} else if (node.type == Value_Type::STRING) {
//			Print_Data data{.str = node.str};
//			pretty_print(indent, Print_Type::STRING, data);
//		}
	}
	printf("\n");
	for (int i = 0; i < indent; i++) {
		printf("	");
	}
	printf("]\n");
//	Print_Data data{.str = "]"};
//	pretty_print(indent - 1, Print_Type::STRING, data);
}

//void pretty_print(int indent, Print_Type type, Print_Data data, bool new_line) {
//	for (int i = 0; i < indent; i++) {
//		printf("	");
//	}
//
//	if (type == Print_Type::STRING) {
//		printf("%s", data.str.c_str());
//	} else if (type == Print_Type::NUMBER) {
//		printf("%f", data.number);
//	} else if (type == Print_Type::BOOL) {
//		bool bool_val = data.bool_val;
//		if (bool_val) printf("true");
//		else printf("false");
//	}
//
//	if (new_line) printf("\n");
//}

void print_key(int indent, std::string_view name) {
	for (int i = 0; i < indent; i++) {
		printf("	");
	}
	
	printf("%s: ", name);
}

void print_value(AST_Value_Node value_node, int indent) {
	if (indent > 0) {
		for (int i = 0; i < indent; i++) {
			printf("	");
		}
	}
	if (value_node.type == Value_Type::STRING) {
		printf("%s", std::get<std::string>(value_node.value).c_str());
	} else if (value_node.type == Value_Type::NUMBER) {
		printf("%f", std::get<float>(value_node.value));
	} else if (value_node.type == Value_Type::BOOL) {
		bool bool_val = std::get<bool>(value_node.value);
		if (bool_val) printf("true");
		else printf("false");
	}
	
//	printf("\n");
}

void print_object(AST_Node* node, int indent) {
	for (AST_Pair_Node* pair_node : node->properties) {
		
		// print key, if its an object then print with a new line after it and increase indent
//		Print_Data key_data{.str = pair_node->key.c_str()};
//		if (pair_node->value_node.type == Value_Type::OBJECT) {
//			pretty_print(indent, Print_Type::STRING, key_data, false);
//			printf(": \n");
//			indent++;
//		}
//		else pretty_print(indent, Print_Type::STRING, key_data, false);
		print_key(indent, pair_node->key);
		
		if (pair_node->value_node.type == Value_Type::OBJECT) {
			indent++;
			printf("\n");
		}
		else if (pair_node->value_node.type == Value_Type::ARRAY) {
			print_array(std::get<std::vector<AST_Value_Node>>(pair_node->value_node.value), indent);
		} else {
			print_value(pair_node->value_node, indent);
			printf("\n");
		}
		
//		// print the pair_node value based on its type
//		if (pair_node->value_node.type == Value_Type::NUMBER) {
//			Print_Data data{.number = pair_node->value_node.number};
//			pretty_print(indent, Print_Type::NUMBER, data);
//		} else if (pair_node->value_node.type == Value_Type::STRING) {
//			Print_Data data{.str = pair_node->value_node.str};
//			pretty_print(indent, Print_Type::STRING, data);
//		} else if (pair_node->value_node.type == Value_Type::BOOL) {
//			Print_Data data{.bool_val = pair_node->value_node.bool_val};
//			pretty_print(indent, Print_Type::BOOL, data);
//		} else if (pair_node->value_node.type == Value_Type::OBJECT) {
//			print_object(pair_node->value_node.object, indent);
//		} else if (pair_node->value_node.type == Value_Type::ARRAY) {
//			print_array(pair_node->value_node.array, indent + 1);
//		}
	}
}

void print_ast(AST* ast) {
	printf("Printing AST\n\nSTART AST\n");
	int indent = 1;
	
	AST_Node* current_node = ast->root;
	Print_Data root_data{.str = current_node->name.c_str()};
//	pretty_print(indent, Print_Type::STRING, root_data);
	indent++;
	print_object(ast->root, indent);
	
//	print_object(current_node, indent);
	
	printf("END AST\n\n");
}

void lex(Parser* parser) {
	while (!parser->eof) {
		consume(parser);
	}
}

Json_Data parse(std::string str) {
	Parser* parser = new Parser;
	parser->str = str;
	lex(parser);
	Json_Data json_data;
	json_data.ast = create_ast(parser->tokens);
	json_data.tokens = parser->tokens;
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
AST_Value_Node Json_Data::operator[](std::string key) {
	AST_Value_Node value_node;
	//find key
	bool searching_for_key = true;
	AST_Node* current_node = this->ast->root;
	int attempts = 0;
	int max_attempts = 100;
	while (searching_for_key) {
		for (AST_Pair_Node* pair_node : current_node->properties) {
			if (pair_node->key.compare(key) == 0) {
				value_node = pair_node->value_node;
				searching_for_key = false;
			}
		}
		attempts++;
		if (attempts > max_attempts) searching_for_key = false;
	}
	
	return value_node;
}

// use this overload to access data withing a AST_Value_Node object
// check for keys that match the string and return their value node
AST_Value_Node AST_Value_Node::operator[](std::string key) {
	AST_Value_Node value_node;
	
	if (this->type == Value_Type::ARRAY) {
		
	} else {
		assert(false);
	}
	
	int a = 0;
	
	return value_node;
//	AST_Value_Node value_node;
//	//find key
//	bool searching_for_key = true;
//	AST_Node* current_node = this->ast->root;
//	int attempts = 0;
//	int max_attempts = 100;
//	while (searching_for_key) {
//		for (AST_Pair_Node* pair_node : current_node->properties) {
//			if (pair_node->key.compare(key) == 0) {
//				value_node = pair_node->value_node;
//				searching_for_key = false;
//			}
//		}
//		attempts++;
//		if (attempts > max_attempts) searching_for_key = false;
//	}
//
//	return value_node;
}

AST_Value_Node AST_Value_Node::operator[](int i) {
	if (this->type == Value_Type::ARRAY) {
		assert(false);
	} else if (this->type == Value_Type::OBJECT) {
		assert(false);
	} else if (this->type == Value_Type::NUMBER){
		AST_Value_Node value_node;
		value_node.type = Value_Type::ERROR;
		return value_node;
	}
	
	V_Node_List list = std::get<V_Node_List>(this->value);
	return list[i];
}
