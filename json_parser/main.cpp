//
//  main.cpp
//  json_parser
//
//  Created by Michael Donlan on 8/21/21.
//

#include "Json.h"
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "Tests.h"

#define DEVMODE 0

int main(int argc, const char * argv[]) {
	
	Json_Value json = parse(load_json_from_file("json_testing.json"));
	
	int a = 0;
	(void)a;
//	json["foo"] = 3;
//	json["bar"] = "hello world";
//	
//	Json_Value other_json = json;
	
	// how to create a new object?
//	json["blah"] = Basic_Value{.type = Value_Type::OBJECT};
	
//	int a = 0;
	
//
//	print_value(json["bar"]);
//	print_value(json["foo"]);
	
//	Json json = parse(R"(
//		{
//			"foo": "hello world!",
//			"blah": "it is wednesday",
//			"yeah": {
//				"test": "blah",
//				"what": 4,
//				"why": 5.7
//			}
//		}
//	)");
//
//	std::string json_str = json_to_string(json);
//	write_json(json_str, "write_json_test.json");
	
//
	
#if DEVMODE
	int result = Catch::Session().run(argc, argv);
	return result;
#endif
	return 0;
}
