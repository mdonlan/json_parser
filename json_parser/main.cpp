//
//  main.cpp
//  json_parser
//
//  Created by Michael Donlan on 8/21/21.
//

#include <iostream>
#include <string>
#include <vector>


enum Token_Type {
	OPEN_BRACKET,
	STRING_VALUE,
	NAME,
	NUMBER
};

struct Token {
	Token_Type type;
	
	std::string str;
	float number;
	
//	union {
//		float number;
//		std::string str;
//		char c;
//	};
};

struct Parser {
	unsigned int index;
	bool eof = false;
	std::string str;
	std::vector<Token> tokens;
};



void consume(Parser* parser);
char peek(Parser* parser, unsigned int index);
void eat_whitespace(Parser* parser);

int main(int argc, const char * argv[]) {
	Parser* parser = new Parser;
	
	std::string test_json = R"(
		{
		"userId": 1,
		"id": 1,
		"title": "delectus aut autem",
		"completed": false
		}
	)";
	
	parser->str = test_json;
	
	while (!parser->eof) {
		consume(parser);
	}
	
	return 0;
}

void consume(Parser* parser) {
	eat_whitespace(parser);
	char c = parser->str[parser->index];
	
	// match against tokens
	if (c == '\0') {
		parser->eof = true;
	} else if (c == '{') { // left bracket token
		Token token;
		token.type = OPEN_BRACKET;
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
					token.type = NAME;
				} else {
					// this token is a string value
					token.type = STRING_VALUE;
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
		token.type = NUMBER;
		
		std::string num_str(1, c);
		bool found_end_of_number = false;
		unsigned int p_index = parser->index + 1;
		
		while (!found_end_of_number) {
			char pc = peek(parser, p_index);
			if (!isdigit(pc)) {
				found_end_of_number = true;
			} else {
				num_str.push_back(pc);
				p_index++;
			}
		}
		
		token.number = std::stoi(num_str);
		parser->tokens.push_back(token);
		parser->index += num_str.size();
	} else {
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
