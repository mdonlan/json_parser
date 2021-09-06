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
	
	std::string json_str = R"(
		{
			"test": [
				1, 2, 3
			]
		}
	)";
	
//	const std::string& json_str = load_json_from_file("ship_test.json");
	Json_Data json_data = parse(json_str);
	
	AST_Value_Node value = json_data["name"];
	AST_Value_Node tiles = json_data["tiles"][1];
	
//	print_ast(json_data.ast);
	
	
	int result = Catch::Session().run(argc, argv);
	return result;
}
