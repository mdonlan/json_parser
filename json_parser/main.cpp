//
//  main.cpp
//  json_parser
//
//  Created by Michael Donlan on 8/21/21.
//

//#include <iostream>


#include "json.h"
#include "Tests.h"
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include <iostream>


int main(int argc, const char * argv[]) {
	
//	std::string json_str = R"(
//		{
//			"userId": 1,
//			"id": 1,
//			"title": "delectus aut autem",
//			"completed": false,
//			"test_bool": true,
//			"test_obj": {
//				"blah": 5
//			}
//		}
//	)";
	
	const std::string& json_str = load_json_from_file("json_test.json");
	Json_Data json_data = parse(json_str);
	
//	AST_Value_Node* value_node = json_data["id"];
	AST_Value_Node value = json_data["id"];
	int a = 0;
	
//	Json_Data blah;
//	blah["id"];
	
	print_ast(json_data.ast);
	
	int result = Catch::Session().run(argc, argv);
	return result;
}
