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


TEST_CASE("\nBasic Test\n", "[basic]") {
	
	Json_Value json = parse(load_json_from_file("json_testing.json"));
	REQUIRE(json.type == Value_Type::OBJECT);
	
	Json_Obj_Test* obj = json.to_obj();
	REQUIRE(obj->size() == 6);
	
	REQUIRE(json["id"].to_int() == 1);
	
	Json_Value value = json["id"];
	value.value = 2;
	
	json["id"] = 3;
	
	REQUIRE(json["id"].to_int() == 3);
	REQUIRE(json["id"].type == Value_Type::NUMBER);
	
	Json_Array* arr = json["world"].to_array();
	REQUIRE(arr->size() == 3);
	
	REQUIRE(json["can_bool"].type == Value_Type::BOOL);
	REQUIRE(json["can_bool"].to_bool() == false);

	json_free(json);
}

TEST_CASE("ARRAY TESTS") {
	SECTION("Nested Array") {
		Json_Value json = parse(std::string{R"(
			{
				"nested_array": [[]]
			}
		)"});

	//	AST_Node* root_node = json_data.ast->root;
		REQUIRE(json.type == Value_Type::OBJECT);
		REQUIRE(json.to_obj()->size() == 1);
//		Json_Obj* root_node = json.to_obj();
//
//		REQUIRE(root_node != nullptr);
//		REQUIRE(root_node->name.compare("ROOT") == 0);
//		REQUIRE(root_node->properties.size() == 1);

//		AST_Pair_Node* pair_node = root_node->properties[0];
//		REQUIRE(pair_node->key.compare("nested_array") == 0);
//		REQUIRE(pair_node->value_node.type == Value_Type::ARRAY);
//		REQUIRE(std::get<Json_Obj*>(pair_node->value_node.value)->array.size() == 1);
		
		Json_Value arr_value = json["nested_array"];
		REQUIRE(arr_value.type == Value_Type::ARRAY);
		REQUIRE(arr_value.to_array()->size() == 1);

		json_free(json);
	}

//	SECTION("Naked Array") {
//		Json_Value json = parse(std::string{R"(
//			[]
//		)"});
//
//		Json_Obj* array_node = json.to_obj();
//
//		REQUIRE(array_node != nullptr);
//		REQUIRE(array_node->properties.size() == 0);
//
//		auto test = json.to_array();
//
//		json_free(json);
//	}
//
//	SECTION("Naked Array w/ data") {
//		Json_Value json = parse(std::string{R"(
//			[
//				{
//					"a": "test"
//				},
//				{
//					"b": "blah"
//				}
//			]
//		)"});
//
//		Json_Obj* array_node = json.to_obj();
//
//		REQUIRE(array_node != nullptr);
//		REQUIRE(array_node->properties.size() == 0);
//		REQUIRE(array_node->array.size() == 2);
//
//		json_free(json);
//	}
//
//	SECTION("Array of numbers") {
//		Json_Value json = parse(std::string{R"(
//			[ 1, 2, 3]
//		)"});
//
//		Json_Array array = json.to_array();
//
//		REQUIRE(array.size() == 3);
//		REQUIRE(array[0].type == Value_Type::NUMBER);
//	}
}
//
//TEST_CASE("OBJECT IN ARRAY") {
//	Json_Value json = parse(std::string{R"(
//		{
//			"object_in_arr": [
//				{
//					"blah": "object_1"
//				}
//			]
//		}
//	)"});
//
////	AST_Node* root_node = json_data.ast->root;
//	Json_Obj* root_node = json.to_obj();
//
//	REQUIRE(root_node != nullptr);
//	REQUIRE(root_node->name.compare("ROOT") == 0);
//	REQUIRE(root_node->properties.size() == 1);
//
//	AST_Pair_Node* pair_node = root_node->properties[0];
//	REQUIRE(pair_node->key.compare("object_in_arr") == 0);
//	REQUIRE(pair_node->value_node.type == Value_Type::ARRAY);
//
//	auto current_array = std::get<Json_Obj*>(pair_node->value_node.value)->array;
//	REQUIRE(current_array[0].type == Value_Type::OBJECT);
//
//	Json_Obj* object = std::get<Json_Obj*>(current_array[0].value);
//	REQUIRE(object->name.compare("UNNAMED") == 0);
//	REQUIRE(object->properties[0]->key.compare("blah") == 0);
//	REQUIRE(object->properties[0]->value_node.type == Value_Type::STRING);
//	REQUIRE(std::get<std::string>(object->properties[0]->value_node.value).compare("object_1") == 0);
//
//	json_free(json);
//}
//
//TEST_CASE("COMPLEX") {
//	Json_Value json = parse(std::string{R"(
//		{
//			"name": "json_test_ship",
//			"tiles": [
//				{
//					"x": 0,
//					"y": 0,
//					"room": 0,
//					"walls": {
//						"left": false,
//						"right": true,
//						"top": false,
//						"bot": true
//					}
//				},
//				{
//					"x": 1,
//					"y": 0,
//					"room": 0,
//					"walls": {
//						"left": false,
//						"right": true,
//						"top": false,
//						"bot": true
//					}
//				},
//			],
//			"rooms": [
//				{
//					"id": 0,
//					"has_system": true,
//					"system_name": "shields"
//				}
//			]
//		}
//	)"});
//
//	Json_Obj* root_node = json.to_obj();
//
//	REQUIRE(root_node != nullptr);
//	REQUIRE(root_node->name.compare("ROOT") == 0);
//	REQUIRE(root_node->properties.size() == 3);
////	REQUIRE(json_data["name"].value.as_string())
//
//	json_free(json);
//}
//
//TEST_CASE("STRING") {
//	SECTION("lonely_string") {
//		Json_Value json = parse(std::string{R"("abc")"});
//
//		REQUIRE(json.type == Value_Type::STRING);
////		REQUIRE(get_string(json.value).compare("abc") == 0);
//		REQUIRE(json.to_str().compare("abc") == 0);
//		json_free(json);
//	}
//}
//
//TEST_CASE("Expected Errors") {
//	SECTION("Invalid Starting Token") {
//		Json_Value json = parse(":", false);
//		REQUIRE(json.type == Value_Type::ERROR);
//
//		json_free(json);
//	}
//
//	SECTION("Unterminated String") {
//		Json_Value json = parse(R"("hello)", false);
//		REQUIRE(json.type == Value_Type::ERROR);
//
//		json_free(json);
//	}
//
//	SECTION("Unterminated String Part 2") {
//		Json_Value json = parse(R"(
//			{
//				"a": "test
//			}
//		)", false);
////		REQUIRE(json.value.type == Value_Type::ERROR);
//
//		json_free(json);
//	}
//}
//
//
////TEST_CASE("SERIALIZE") {
////	// test serialize of the ship object
////
////	Json_Value json = parse(load_json_from_file("ship_test.json"));
////
////	struct Tile {
////		int x;
////		int y;
////		int room;
////		std::map<std::string, bool> walls;
////	};
////
////	struct Room {
////		int id;
////		bool has_system;
////		std::string system_name;
////	};
////
////	struct Ship {
////		std::string name;
////		std::vector<Tile> tiles;
////		std::vector<Room> rooms;
////	};
////
////	Ship ship;
//////	ship.name = get_string(json["name"]);
////	ship.name = json["name"].to_str();
////
////	auto tiles_arr = json["tiles"].to_array();
////	for (auto tile_data : tiles_arr) {
////		Tile tile;
////		tile.x = tile_data["x"].to_int();
////		tile.y = tile_data["y"].to_int();
////		tile.room = tile_data["room"].to_int();
////
////		Json_Obj* walls_obj = tile_data["walls"].to_obj();
////		for (auto wall_data : walls_obj->properties) {
////			tile.walls[wall_data->key] = wall_data->value_node.to_bool();
////		}
////
////		ship.tiles.push_back(tile);
////	}
////
////	auto rooms_arr = json["rooms"].to_array();
////	for (auto room_data : rooms_arr) {
////		Room room;
////		room.id = room_data["id"].to_int();
////		room.has_system = room_data["has_system"].to_bool();
////		room.system_name = room_data["system_name"].to_str();
////
////		ship.rooms.push_back(room);
////	}
////
////	REQUIRE(ship.name.compare("json_test_ship") == 0);
////	REQUIRE(ship.tiles.size() == 2);
////	REQUIRE(ship.tiles[0].walls.size() == 4);
////	REQUIRE(ship.rooms.size() == 1);
////	REQUIRE(ship.rooms[0].id == 0);
////
////	json_free(json);
////}
//
////TEST_CASE("NUMBERS") {
////	SECTION("Naked number") {
////		Json_Value json = parse(std::string(R"(3)"));
////		REQUIRE(json.type == Value_Type::NUMBER);
////		REQUIRE(json.to_int() == 3);
////	}
////
////	SECTION("INT vs float") {
////		Json_Value json = parse(std::string(R"({"test_float": 7.25, "test_int": 2})"));
////		REQUIRE(json["test_float"].type == Value_Type::NUMBER);
////		REQUIRE(json["test_float"].to_float() == 7.25);
////		REQUIRE(json["test_int"].type == Value_Type::NUMBER);
////		REQUIRE(json["test_int"].to_int() == 2);
////	}
////
////	SECTION("Negative Numbers") {
////		Json_Value json = parse(std::string(R"({"x": -1})"));
////		REQUIRE(json["x"].type == Value_Type::NUMBER);
////		REQUIRE(json["x"].to_int() == -1);
////	}
////}
//
//TEST_CASE("BOOLS") {
//	SECTION("Naked bool") {
//		Json_Value json = parse(std::string{R"(
//			true
//		)"});
//
//		REQUIRE(json.type == Value_Type::BOOL);
//		REQUIRE(json.to_bool() == true);
//
//		json_free(json);
//	}
//
//	SECTION("array of bools") {
//		Json_Value json = parse(std::string{R"(
//			[
//				false,
//				true,
//				true
//			]
//		)"});
//
//		REQUIRE(json.type == Value_Type::ARRAY);
//		auto arr = json.to_array();
//		REQUIRE(arr.size() == 3);
//		REQUIRE(arr[0].to_bool() == false);
//
//		json_free(json);
//	}
//}
//
//TEST_CASE("Large Files") {
//	SECTION("Parse Large File") {
//		Json_Value json = parse(load_json_from_file("large_test_file_2.json"));
//		json_free(json);
//	}
//
////	SECTION("Test memory free") {
////		Json json = parse(std::string{R"(
////			{
////				"test": "blah",
////				"foo": "bar",
////				{
////					"hello": 1,
////					"world": "yes"
////				}
////			}
////		)"});
////
////		json_free(json.value);
////	}
//
//	SECTION("Test Memory Leak") {
////		for (int i = 0; i < 15; i++) {
////			Json json = parse(load_json_from_file("large_test_file_2.json"));
////			json_free(json.value);
////		}
//////		int a = 0;
//	}
//}
//
////TEST_CASE("Write/Edit Json") {
////	SECTION("Write Numbers") {
////		Json_Value json; // setup json without parse();
////
//////		REQUIRE(json.value.type == Value_Type::OBJECT);
//////		REQUIRE(json.value.to_obj()->properties.size() == 0);
////
////		json["foo"] = 3;
////		REQUIRE(json["foo"].to_int() == 3);
////
////		REQUIRE(json.type == Value_Type::OBJECT);
////		REQUIRE(json.to_obj()->properties.size() == 1);
////
////		Json_Value value = json["foo"];
////
////		REQUIRE(value.type == Value_Type::NUMBER);
////		REQUIRE(value.to_int() == 3);
////	}
////
////	SECTION("Write Strings") {
////		Json_Value json = parse("{}");
////
////		REQUIRE(json.type == Value_Type::OBJECT);
////		REQUIRE(json.to_obj()->properties.size() == 0);
////
////		json["foo"] = "hello world";
////		REQUIRE(json["foo"].to_str().compare("hello world") == 0);
////
////		REQUIRE(json.type == Value_Type::OBJECT);
////		REQUIRE(json.to_obj()->properties.size() == 1);
////
////		Json_Value value = json["foo"];
////
////		REQUIRE(value.type == Value_Type::STRING);
////		REQUIRE(json["foo"].to_str().compare("hello world") == 0);
////	}
////
////	SECTION("Write Nested Values") {
////		Json_Value json = parse("{}");
//////		json["foo"] = ;
////	}
////
////	SECTION("Write Json_Array") {
////		Json_Value json = parse("[ 1, 2, 3 ]");
////		Json_Array json_arr = json.to_array();
////
////		Json_Value other_json;
////		other_json.type = Value_Type::ARRAY;
////		other_json.value = &json_arr;
////
//////		other_json.to_array();
////
//////		int a = 0;
////	}
////
////}
//
//TEST_CASE("FILE I/O") {
//	SECTION("Write out to file") {
////		Json_Value json = parse(std::string{R"(
////			{
////				"test": "blah",
////				"foo": {
////					"hello": 1,
////					"world": "yes"
////				},
////				"test_arr": [ 1, 2, 3 ],
////				"test_arr_of_objs": [
////					{
////						"a": 1,
////						"b": 2,
////						"c": [ 1, 2, 3]
////					},
////					  {
////						  "a": 3,
////						  "b": 4,
////						"c": [ 1, 2, 3]
////					  },
////					 {
////						 "a": 5,
////						 "b": 6,
////					"c": [ 1, 2, 3]
////					 },
////				],
////				"test_bool": false
////			}
////		)"});
//
//
////		Json_Value json = parse(load_json_from_file("ship_data.json"));
////
////		write_json(json_to_string(json), "write_json_test.json");
//	}
//}
//
//TEST_CASE("JSON OBJECTS") {
//
//	/*
//		there is currently an issue where an anonymous object is not seen as a syntax error
//
//		it also causes parsing issues if there are other objects after it
//		the below example causes object foo to have another object foo inside of it??
//	*/
//
////	Json json = parse(std::string(R"({ "foo": { "test": 1}, { "blah": 2}, "hello": { "why": 3}} )"));
//
////	int a = 0;
////	REQUIRE(json["test"].to_int() == 1);
////	REQUIRE(json["test"].type == Value_Type::NUMBER);
//
//}
//
////TEST_CASE("TYPES") {
////	Json_Value json = parse(std::string(R"({ "test": 1})"));
////	REQUIRE(json["test"].to_int() == 1);
////	REQUIRE(json["test"].type == Value_Type::NUMBER);
////
////	json = parse(std::string(R"({ "test": "str"})"));
////	REQUIRE(json["test"].to_str().compare("str") == 0);
////	REQUIRE(json["test"].type == Value_Type::STRING);
////
////
////}
//
////TEST_CASE("COMMENTS") {
////	Json_Value json = parse(std::string(R"(
////	  {
////		// this is a comment
////		"test": 1
////	  }
////	  )"));
////	REQUIRE(json.type == Value_Type::OBJECT);
////	REQUIRE(json.to_obj()->properties.size() == 1);
////	REQUIRE(json["test"].to_int() == 1);
////}
