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
	NAME
};

struct Token {
	Token_Type type;
	
	std::string str;
	
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
	} else if (c == '"') { // name/decl token or string value token
		Token token;
//		token.type = STRING;
		
		std::string str;
		unsigned int p_index = parser->index;
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
	}
	
	parser->index++;
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
