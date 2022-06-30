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
//#include <type_traits>
//#include <variant>
#define DEVMODE 1



int main(int argc, const char * argv[]) {
	
//	Json_Value value;
//	value = 5;
//
//	int test = value.value;
//
//	value.value.get<int>()
//
//	value = "hello";
//
//	printf("value: %s", value.to_str().c_str());
//	std::string blah = value.value;
//
//	int a = 0;
//	(void)a;
////
//	Variant  v{"hello"};
//
//	std::string l = v;  // compile and works run-time
//
//   std::string s = v;
	
//	test_variant test = 1;
//	test = "Hello";
//
//	Value blah = 5;
//	blah = 7;
//	blah = "hello";
//
////	int testing = test.get();
//
//	int a = 0;
//	(void)a;
//	Result<int> test = 5;
//	printf("%d\n", test.get());
//	Result<std::string> test2 = std::string("hello");
//	printf("%s\n", test2.get().c_str());
	
//	Json_Value json = parse(load_json_from_file("json_testing.json"));
//
////	std::string, int, float, bool, Json_Obj*, Json_Array*, Json_Obj_Test*
//	auto blah = std::visit(
//		Overload{
//			[] (std::string k) { return k; },
//			[] (int k) { return k; },
//			[] (float k) { return k; },
//			[] (bool k) { return k; },
//			[] (Json_Obj* k) { return k; },
//			[] (Json_Array* k) { return k; },
//			[] (Json_Obj_Test* k) {return k; }
////			[] (bool k) { /* deal with k here */ },
////			[] (std::string k) { /* deal with k here */ }
//		},
//	   json.value
//	);
	
//	int i = json["id"].value;
	
//	Json_Value json = parse(load_json_from_file("json_testing.json"));
//
//	int a = 0;
//	(void)a;
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
