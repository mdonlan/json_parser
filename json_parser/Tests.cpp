//
//  Tests.cpp
//  json_parser
//
//  Created by Michael Donlan on 8/23/21.
//

#include <stdio.h>
#include "json.h"
#include "Tests.h"
#include "catch.hpp"

void do_tests() {
	printf("\n\nRunning Tests...\n");
	
//	Parser* parser = new Parser;
	
	Json_Data* json_data = parse(std::string{R"({"key": "value"}})"});
	
	AST_Node* root_node = json_data->ast->root;
//	assert(root_node);
	
//	Assert_True(root_node, "Root Node Exists");
//	Assert_Equal(root_node->name.c_str(), "ROOT", "Root Node Name is Root");
//	Assert_Equal(root_node->properties[0]->key, "key", "Key Value Matches");
//
//
//	parser = new Parser;
//	json_data = parse(parser, std::string{R"({ "blah": "value", "key2": "value2", "key3": "value3" }})"});
//	root_node = json_data->ast->root;
//
//	Assert_Equal(root_node->properties.size(), 3, "Property Size");
//	Assert_Equal(root_node->properties[2]->value_node.type, Value_Type::NUMBER, "Value Type for Number is Number");
	
	printf("\n\n\n");
	
}

TEST_CASE( "\nALL TESTS\n", "[all]" ) {
	
//	Parser* parser = new Parser;

//	Json_Data* json_data = parse(std::string{R"(
//		{
//			"test_key": "test_str_value",
//			"test_key_2": 2
//		}
//	)"});
	Json_Data* json_data = parse(load_json_from_file("json_test.json"));
	AST_Node* root_node = json_data->ast->root;
	
	REQUIRE(root_node != nullptr);
	REQUIRE(root_node->name.compare("ROOT") == 0);
	
	AST_Pair_Node* pair_node = root_node->properties[0];
	REQUIRE(pair_node->key.compare("userId") == 0);
	REQUIRE(pair_node->value_node.type == Value_Type::NUMBER);
	REQUIRE(pair_node->value_node.number == 1);
	
	pair_node = root_node->properties[1];
	REQUIRE(pair_node->key.compare("id") == 0);
	REQUIRE(pair_node->value_node.type == Value_Type::NUMBER);
	REQUIRE(pair_node->value_node.number == 1);
	
	pair_node = root_node->properties[2];
	REQUIRE(pair_node->key.compare("title") == 0);
	REQUIRE(pair_node->value_node.type == Value_Type::STRING);
	REQUIRE(pair_node->value_node.str.compare("delectus aut autem") == 0);
}


//void Assert_True(void* ptr, const std::string& str) {
//	printf("%s: ", str.c_str());
//	if (ptr) printf("Passed\n");
//	else printf("Failed\n");
//}
//
//void Assert_False(void* ptr, const std::string& str) {
//	printf("%s: ", str.c_str());
//	if (!ptr) printf("Passed\n");
//	else printf("Failed\n");
//}



//void Assert_Equal(const std::string& test_value, const std::string& expected, const std::string& str) {
//	printf("%s: ", str.c_str());
//	if (test_value.compare(expected) == 0) printf("Passed\n");
//	else printf("Failed\n");
//}

//void Assert_Equal(int test_value, int expected, const std::string& str) {
//	printf("%s: ", str.c_str());
//	if (test_value == expected) printf("Passed\n");
//	else printf("Failed\n");
//}

//template <typename T>
//void Assert_Equal(T value, T expected_value, const std::string& str) {
//
//}

template <typename T>
void Assert_Equal(T value, T expected_value, const std::string& str) {
	printf("%s: ", str.c_str());
	if (value == expected_value) printf("Passed\n");
	else printf("Failed\n");
}