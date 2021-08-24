//
//  main.cpp
//  json_parser
//
//  Created by Michael Donlan on 8/21/21.
//

//#include <iostream>


#include "json.h"
#include "Tests.h"

int main(int argc, const char * argv[]) {
	Parser* parser = new Parser;
	
	std::string test_json = R"(
		{
			"userId": 1,
			"id": 1,
			"title": "delectus aut autem",
			"completed": false,
			"test_bool": true,
			"test_obj": {
				"blah": 5
			}
		}
	)";
	
	Json_Data* json_data = parse(parser, test_json);
	
	print_ast(json_data->ast);
	do_tests();
	
	return 0;
}
