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

#define DEVMODE 1

int main(int argc, const char * argv[]) {
	
//	Json json = parse("{}");
//	json["foo"] = 3;
//
//	json["bar"] = "hello world";
//
//	print_value(json["bar"]);
//	print_value(json["foo"]);
	
	Json json = parse(R"(
		{
			"foo": "hello world!",
			"blah": "it is wednesday",
			"yeah": {
				"test": "blah",
				"what": 4,
				"why": 5.7
			}
		}
	)");
	
	std::string json_str = json_to_string(json);
	write_json(json, "write_json_test.json");
	
#if DEVMODE
	int result = Catch::Session().run(argc, argv);
	return result;
#endif
	return 0;
}
