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
	STRING
};

struct Token {
	Token_Type type;
	
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
void peek(Parser* parser);

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
	char c = parser->str[parser->index];
	
	// match against tokens
	if (c == '\0') {
		parser->eof = true;
	} else if (c == '{') {
		Token token;
		token.type = OPEN_BRACKET;
		parser->tokens.push_back(token);
		parser->index++;
	} else if (c == '"') {
		Token token;
		token.type = STRING;
		
		std::string str;
		unsigned int p_index = parser->index;
		bool finding_end_of_string = true;
		
		while (finding_end_of_string) {
//			char pc =
		}
		
	} else {
		parser->index++;
	}
}

void peek(Parser* parser) {
	
}
