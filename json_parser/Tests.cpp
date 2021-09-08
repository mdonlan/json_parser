//
//  Tests.cpp
//  json_parser
//
//  Created by Michael Donlan on 8/23/21.
//

#include <stdio.h>
#include "Json.h"
#include "Tests.h"
#include "catch.hpp"


TEST_CASE( "\nBasic Test\n", "[basic]" ) {
	Json json = parse(load_json_from_file("json_test.json"));
	AST_Node* root_node = get_object(json.value);

	REQUIRE(root_node != nullptr);
	REQUIRE(root_node->name.compare("ROOT") == 0);

	AST_Pair_Node* pair_node = root_node->properties[0];
	REQUIRE(pair_node->key.compare("userId") == 0);
	REQUIRE(pair_node->value_node.type == Value_Type::NUMBER);
	REQUIRE(std::get<float>(pair_node->value_node.value) == 1);

	pair_node = root_node->properties[1];
	REQUIRE(pair_node->key.compare("id") == 0);
	REQUIRE(pair_node->value_node.type == Value_Type::NUMBER);
	REQUIRE(std::get<float>(pair_node->value_node.value) == 1);

	pair_node = root_node->properties[2];
	REQUIRE(pair_node->key.compare("title") == 0);
	REQUIRE(pair_node->value_node.type == Value_Type::STRING);
	REQUIRE(std::get<std::string>(pair_node->value_node.value).compare("delectus aut autem") == 0);
}

TEST_CASE("ARRAY TESTS") {
	Json json = parse(std::string{R"(
		{
			"nested_array": [[]]
		}
	)"});
	
//	AST_Node* root_node = json_data.ast->root;
	AST_Node* root_node = get_object(json.value);
	
	REQUIRE(root_node != nullptr);
	REQUIRE(root_node->name.compare("ROOT") == 0);
	REQUIRE(root_node->properties.size() == 1);
	
	AST_Pair_Node* pair_node = root_node->properties[0];
	REQUIRE(pair_node->key.compare("nested_array") == 0);
	REQUIRE(pair_node->value_node.type == Value_Type::ARRAY);
	REQUIRE(std::get<AST_Node*>(pair_node->value_node.value)->array.size() == 1);
}

TEST_CASE("OBJECT IN ARRAY") {
	Json json = parse(std::string{R"(
		{
			"object_in_arr": [
				{
					"blah": "object_1"
				}
			]
		}
	)"});

//	AST_Node* root_node = json_data.ast->root;
	AST_Node* root_node = get_object(json.value);

	REQUIRE(root_node != nullptr);
	REQUIRE(root_node->name.compare("ROOT") == 0);
	REQUIRE(root_node->properties.size() == 1);

	AST_Pair_Node* pair_node = root_node->properties[0];
	REQUIRE(pair_node->key.compare("object_in_arr") == 0);
	REQUIRE(pair_node->value_node.type == Value_Type::ARRAY);

	V_Node_List& current_array = std::get<AST_Node*>(pair_node->value_node.value)->array;
	REQUIRE(current_array[0].type == Value_Type::OBJECT);

	AST_Node* object = std::get<AST_Node*>(current_array[0].value);
	REQUIRE(object->name.compare("UNNAMED") == 0);
	REQUIRE(object->properties[0]->key.compare("blah") == 0);
	REQUIRE(object->properties[0]->value_node.type == Value_Type::STRING);
	REQUIRE(std::get<std::string>(object->properties[0]->value_node.value).compare("object_1") == 0);
}

TEST_CASE("COMPLEX") {
	Json json = parse(std::string{R"(
		{
			"name": "json_test_ship",
			"tiles": [
				{
					"x": 0,
					"y": 0,
					"room": 0,
					"walls": {
						"left": false,
						"right": true,
						"top": false,
						"bot": true
					}
				},
				{
					"x": 1,
					"y": 0,
					"room": 0,
					"walls": {
						"left": false,
						"right": true,
						"top": false,
						"bot": true
					}
				},
			],
			"rooms": [
				{
					"id": 0,
					"has_system": true,
					"system_name": "shields"
				}
			]
		}
	)"});

	AST_Node* root_node = get_object(json.value);

	REQUIRE(root_node != nullptr);
	REQUIRE(root_node->name.compare("ROOT") == 0);
	REQUIRE(root_node->properties.size() == 3);
//	REQUIRE(json_data["name"].value.as_string())
}

//TEST_CASE("STRING") {
//	SECTION("lonely_string") {
//		Json json = parse("abc");
//		int a = 0;
//	}
//}
