//
//  json.cpp
//  json_parser
//
//  Created by Michael Donlan on 8/23/21.
//

#include "json.h"

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

AST* create_ast(std::vector<Token>& tokens) {
	AST* ast = new AST;
	
	int token_index = 0;
	Token current_token = tokens[token_index];
	
//	AST_Node* root_node = new AST_Node;
//	root_node->type = AST_Node_Type::ROOT;
//	ast->root = root_node;
	
	AST_Node* current_node = nullptr;
	AST_Pair_Node* current_pair_node = nullptr;
	
	while (current_token.type != Token_Type::END_OF_FILE) {
		
		if (current_token.type == Token_Type::OPEN_BRACKET) { // object node
			
			// setup root node
			if (!current_node) {
				AST_Node* root_node = new AST_Node;
				root_node->type = AST_Node_Type::OBJECT;
				root_node->name = "ROOT";
				ast->root = root_node;
				current_node = root_node;
			} else {
				AST_Pair_Node* pair_node = current_node->properties[current_node->properties.size() - 1];
				
//				AST_Node* node = new AST_Node;
				pair_node->value_node.type = Value_Type::OBJECT;
				
				AST_Node* new_object_node = new AST_Node;
				new_object_node->type = AST_Node_Type::OBJECT;
				new_object_node->name = pair_node->key;
				new_object_node->parent = current_node;
				pair_node->value_node.object = new_object_node;
				
	//			current_node->children.push_back(node);
				current_node = new_object_node;
			}
			
			
		} else if (current_token.type == Token_Type::NAME) { // name node
			if (current_node->type != AST_Node_Type::OBJECT) assert(false);
			
			AST_Pair_Node* pair_node = new AST_Pair_Node;
			pair_node->key = current_token.str;
			pair_node->parent = current_node;
			current_node->properties.push_back(pair_node);
		} else if (current_token.type == Token_Type::STRING_VALUE) {
			if (current_node->type != AST_Node_Type::OBJECT) assert(false);
			
			AST_Pair_Node* pair_node = current_node->properties[current_node->properties.size() - 1];
			
			AST_Value_Node value_node;
			value_node.type = Value_Type::STRING;
			value_node.str = current_token.str;
			
			pair_node->value_node = value_node;
		} else if (current_token.type == Token_Type::NUMBER) {
			if (current_node->type != AST_Node_Type::OBJECT) assert(false);
			
			AST_Pair_Node* pair_node = current_node->properties[current_node->properties.size() - 1];
			
			AST_Value_Node value_node;
			value_node.type = Value_Type::NUMBER;
			value_node.number = current_token.number;
			
			pair_node->value_node = value_node;
		} else if (current_token.type == Token_Type::BOOL) {
			if (current_node->type != AST_Node_Type::OBJECT) assert(false);
			
			AST_Pair_Node* pair_node = current_node->properties[current_node->properties.size() - 1];
			
			AST_Value_Node value_node;
			value_node.type = Value_Type::BOOL;
			value_node.bool_val = current_token.bool_val;
			
			pair_node->value_node = value_node;
		}
		
		token_index++;
		current_token = tokens[token_index];
	}
	
	return ast;
}



void pretty_print(int indent, Print_Type type, Print_Data data, bool new_line) {
	for (int i = 0; i < indent; i++) {
		printf("	");
	}
	
	if (type == Print_Type::STRING) {
		printf("%s", data.str.c_str());
	} else if (type == Print_Type::NUMBER) {
		printf("%f", data.number);
	} else if (type == Print_Type::BOOL) {
		bool bool_val = data.bool_val;
		if (bool_val) printf("true");
		else printf("false");
	}
	
	if (new_line) printf("\n");
}

void print_object(AST_Node* node, int indent) {
	for (AST_Pair_Node* pair_node : node->properties) {
		Print_Data key_data{.str = pair_node->key.c_str()};
		if (pair_node->value_node.type == Value_Type::OBJECT) {
			// if an the value type is object add a new line and increase indent
			pretty_print(indent, Print_Type::STRING, key_data, true);
			indent++;
		}
		else pretty_print(indent, Print_Type::STRING, key_data, false);

		if (pair_node->value_node.type == Value_Type::NUMBER) {
			Print_Data data{.number = pair_node->value_node.number};
			pretty_print(indent, Print_Type::NUMBER, data);
		} else if (pair_node->value_node.type == Value_Type::STRING) {
			Print_Data data{.str = pair_node->value_node.str};
			pretty_print(indent, Print_Type::STRING, data);
		} else if (pair_node->value_node.type == Value_Type::BOOL) {
			Print_Data data{.bool_val = pair_node->value_node.bool_val};
			pretty_print(indent, Print_Type::BOOL, data);
		} else if (pair_node->value_node.type == Value_Type::OBJECT) {
			print_object(pair_node->value_node.object, indent);
		}
	}
}

void print_ast(AST* ast) {
	printf("Printing AST\n\n");
	int indent = 0;
	
	AST_Node* current_node = ast->root;
	Print_Data root_data{.str = current_node->name.c_str()};
	pretty_print(indent, Print_Type::STRING, root_data);
	indent++;
	
	print_object(current_node, indent);
	
//	for (AST_Pair_Node* pair_node : current_node->properties) {
//		Print_Data key_data{.str = pair_node->key.c_str()};
//		pretty_print(indent, Print_Type::STRING, key_data, false);
//
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
//			current_node = pair_node->value_node.object;
//		}
//	}
	
	
	
}

void lex(Parser* parser) {
	while (!parser->eof) {
		consume(parser);
	}
}

Json_Data* parse(Parser* parser, std::string str) {
	parser->str = str;
	lex(parser);
	Json_Data* json_data = new Json_Data;
	json_data->ast = create_ast(parser->tokens);
	return json_data;
}
