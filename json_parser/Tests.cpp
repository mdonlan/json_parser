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

TEST_CASE("\nEscaped Characters\n") {

	Json_Value json = parse(std::string{ R"(
			{"model":"llama3.2","created_at":"2025-05-24T02:36:43.7301091Z","response":"Imagine a digital ledger book that's shared among a group of people, like a big team effort. This ledger book is called a \"blockchain\".\n\nHere's how it works:\n\n1. When someone wants to add new information (like a transaction) to the ledger, they create a \"block\" with that info.\n2. The block is then sent to all the other members of the group, who verify that the info is correct and valid.\n3. Once verified, each member adds their own unique \"code\" (called a \"hash\") to the block, which connects it to the previous block in the chain.\n4. This creates a long, unbreakable chain of blocks, which is what we call a blockchain.\n\nThe key benefits of blockchain are:\n\n* **Security**: The code (hashes) connecting each block makes it virtually impossible to alter or manipulate the information.\n* **Transparency**: All members of the group can see the entire history of transactions and updates.\n* **Decentralized**: No single person or organization controls the blockchain – it's a shared, collaborative effort.\n\nBlockchain is used for many things, like:\n\n* Cryptocurrencies (like Bitcoin)\n* Secure online transactions\n* Supply chain management\n* Identity verification\n\nIn simple terms, blockchain is a secure, transparent way to record and share information among a group of trusted individuals.","done":true,"done_reason":"stop","context":[128006,9125,128007,271,38766,1303,33025,2696,25,6790,220,2366,18,271,128009,128006,882,128007,271,849,21435,18428,304,4382,3878,13,128009,128006,78191,128007,271,52157,264,7528,48033,2363,430,596,6222,4315,264,1912,315,1274,11,1093,264,2466,2128,5149,13,1115,48033,2363,374,2663,264,330,4677,8995,11690,8586,596,1268,433,4375,1473,16,13,3277,4423,6944,311,923,502,2038,320,4908,264,7901,8,311,279,48033,11,814,1893,264,330,4677,1,449,430,3630,627,17,13,578,2565,374,1243,3288,311,682,279,1023,3697,315,279,1912,11,889,10356,430,279,3630,374,4495,323,2764,627,18,13,9843,24884,11,1855,4562,11621,872,1866,5016,330,1889,1,320,44982,264,330,8460,909,311,279,2565,11,902,34161,433,311,279,3766,2565,304,279,8957,627,19,13,1115,11705,264,1317,11,653,9137,481,8957,315,10215,11,902,374,1148,584,1650,264,18428,382,791,1401,7720,315,18428,527,1473,9,3146,15712,96618,578,2082,320,8460,288,8,21583,1855,2565,3727,433,21907,12266,311,11857,477,37735,279,2038,627,9,3146,3246,21979,96618,2052,3697,315,279,1912,649,1518,279,4553,3925,315,14463,323,9013,627,9,3146,5005,46186,1534,96618,2360,3254,1732,477,7471,11835,279,18428,1389,433,596,264,6222,11,40806,5149,382,99998,374,1511,369,1690,2574,11,1093,1473,9,38547,511,20180,320,4908,13428,340,9,35602,2930,14463,198,9,30909,8957,6373,198,9,27638,23751,271,644,4382,3878,11,18428,374,264,9966,11,18300,1648,311,3335,323,4430,2038,4315,264,1912,315,22542,7931,13],"total_duration":1850466800,"load_duration":25181500,"prompt_eval_count":32,"prompt_eval_duration":2081500,"eval_count":276,"eval_duration":1823203800}
	)" });
	
	Json_Object& obj = json.to_obj();
	REQUIRE(obj.size() == 12);

	REQUIRE(obj["model"].to_str() == "llama3.2");

	json_free(json);
}


TEST_CASE("\nBasic Test\n", "[basic]") {

	Json_Value json = parse(load_json_from_file("test_files/json_testing.json"));
	REQUIRE(json.type == Value_Type::OBJECT);

	Json_Object& obj = json.to_obj();
	REQUIRE(obj.size() == 6);

	REQUIRE(json["id"].to_int() == 1);
	
//	float testing = json["id"];

	Json_Value value = json["id"];
	value.value = 2;

	json["id"] = 3;

//	auto testing = json["id"].to_int();
	REQUIRE(json["id"].to_int() == 3);
	REQUIRE(json["id"].type == Value_Type::NUMBER);

	Json_Array& arr = json["world"].to_array();
	REQUIRE(arr.size() == 3);

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
		REQUIRE(json.to_obj().size() == 1);
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
		REQUIRE(arr_value.to_array().size() == 1);

		json_free(json);
	}

	SECTION("Naked Array") {
		Json_Value json = parse(std::string{R"(
			[]
		)"});

		REQUIRE(json.type == Value_Type::ARRAY);
		REQUIRE(json.to_array().size() == 0);

		json_free(json);
	}

	SECTION("Naked Array w/ data") {
		Json_Value json = parse(std::string{R"(
			[
				{
					"a": "test"
				},
				{
					"b": "blah"
				}
			]
		)"});

		REQUIRE(json.type == Value_Type::ARRAY);
		REQUIRE(json.to_array().size() == 2);

		json_free(json);
	}

	SECTION("Array of numbers") {
		Json_Value json = parse(std::string{R"(
			[ 1, 2, 3]
		)"});

		REQUIRE(json.to_array().size() == 3);
		REQUIRE(json.to_array()[0].type == Value_Type::NUMBER);
	}
}

TEST_CASE("OBJECT IN ARRAY") {
	Json_Value json = parse(std::string{R"(
		{
			"foo": [
				{
					"blah": "object_1"
				}
			]
		}
	)"});
	REQUIRE(json.to_obj().size() == 1);
	REQUIRE(json["foo"].type == Value_Type::ARRAY);
	REQUIRE(json["foo"][0].type == Value_Type::OBJECT);
	REQUIRE(json["foo"][0]["blah"].type == Value_Type::STRING);
	REQUIRE(json["foo"][0]["blah"].to_str() == "object_1");

	json_free(json);
}

TEST_CASE("COMPLEX") {
	Json_Value json = parse(std::string{R"(
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

	REQUIRE(json.to_obj().size() == 3);
	REQUIRE(json["tiles"].to_array().size() == 2);
	REQUIRE(json["tiles"][0]["walls"].to_obj().size() == 4);

	json_free(json);
}

TEST_CASE("STRING") {
	SECTION("lonely_string") {
		Json_Value json = parse(std::string{R"("abc")"});
		REQUIRE(json.type == Value_Type::STRING);
		REQUIRE(json.to_str() == "abc");
		json_free(json);
	}
}

TEST_CASE("Expected Errors") {
	SECTION("Invalid Starting Token") {
		Json_Value json = parse(":", false);
		REQUIRE(json.type == Value_Type::ERROR);

		json_free(json);
	}

	SECTION("Unterminated String") {
		Json_Value json = parse(R"("hello)", false);
		REQUIRE(json.type == Value_Type::ERROR);

		json_free(json);
	}
//
	SECTION("Unterminated String Part 2") {
		Json_Value json = parse(R"(
			{
				"a": "test
			}
		)");
		REQUIRE(json.type == Value_Type::ERROR);

		json_free(json);
	}
}

struct Person {
	std::string name;
	int age;
};

Json_Value to_json(Person person) {
	Json_Value value;
	value["name"] = person.name;
	value["age"] = person.age;
	return value;
}

Person from_json(Json_Value value) {
	Person person;
	person.name = value["name"].to_str();
	person.age = value["age"].to_int();
	return person;
}


TEST_CASE("SERIALIZE") {
	SECTION("SHIP") {
		// test serialize of the ship object
		Json_Value json = parse(load_json_from_file("./test_files/ship_test.json"));

		struct Tile {
			int x;
			int y;
			int room;
			std::map<std::string, bool> walls;
		};

		struct Room {
			int id;
			bool has_system;
			std::string system_name;
		};

		struct Ship {
			std::string name;
			std::vector<Tile> tiles;
			std::vector<Room> rooms;
		};

		Ship ship;
		ship.name = json["name"].to_str();

		Json_Array& tiles_arr = json["tiles"].to_array();
		for (Json_Value tile_data : tiles_arr) {
			Tile tile;
			tile.x = tile_data["x"].to_int();
			tile.y = tile_data["y"].to_int();
			tile.room = tile_data["room"].to_int();

			Json_Object& walls_obj = tile_data["walls"].to_obj();
			for (auto wall_data : walls_obj) {
				tile.walls[wall_data.first] = wall_data.second.to_bool();
			}

			ship.tiles.push_back(tile);
		}

		auto rooms_arr = json["rooms"].to_array();
		for (auto room_data : rooms_arr) {
			Room room;
			room.id = room_data["id"].to_int();
			room.has_system = room_data["has_system"].to_bool();
			room.system_name = room_data["system_name"].to_str();

			ship.rooms.push_back(room);
		}

		REQUIRE(ship.name.compare("json_test_ship") == 0);
		REQUIRE(ship.tiles.size() == 2);
		REQUIRE(ship.tiles[0].walls.size() == 4);
		REQUIRE(ship.rooms.size() == 1);
		REQUIRE(ship.rooms[0].id == 0);

		json_free(json);
	}
	
	
	
	
	SECTION("Basic") {
		/*
			use structs/functions above
		*/
		
		Person person{"Jim Smith", 21};
		Json_Value person_json = to_json(person);
		
		REQUIRE(person_json.type == Value_Type::OBJECT);
		REQUIRE(person_json.to_obj().size() == 2);
		REQUIRE(person_json["name"].to_str() == "Jim Smith");
		REQUIRE(person_json["age"].to_int() == 21);
		
		Person person_from_json = from_json(person_json);
		
		REQUIRE(person_from_json.name == "Jim Smith");
		REQUIRE(person_from_json.age == 21);
	}
}
//
TEST_CASE("NUMBERS") {
	SECTION("Naked number") {
		Json_Value json = parse(std::string(R"(3)"));
		REQUIRE(json.type == Value_Type::NUMBER);
		REQUIRE(json.to_int() == 3);
	}

	SECTION("INT vs float") {
		Json_Value json = parse(std::string(R"({"test_float": 7.25, "test_int": 2})"));
		REQUIRE(json["test_float"].type == Value_Type::NUMBER);
		REQUIRE(json["test_float"].to_float() == 7.25);
		REQUIRE(json["test_int"].type == Value_Type::NUMBER);
		REQUIRE(json["test_int"].to_int() == 2);
	}

	SECTION("Negative Numbers") {
		Json_Value json = parse(std::string(R"({"x": -1})"));
		REQUIRE(json["x"].type == Value_Type::NUMBER);
		REQUIRE(json["x"].to_int() == -1);
	}
}

TEST_CASE("BOOLS") {
	SECTION("Naked bool") {
		Json_Value json = parse(std::string{R"(
			true
		)"});

		REQUIRE(json.type == Value_Type::BOOL);
		REQUIRE(json.to_bool() == true);

		json_free(json);
	}

	SECTION("array of bools") {
		Json_Value json = parse(std::string{R"(
			[
				false,
				true,
				true
			]
		)"});

		REQUIRE(json.type == Value_Type::ARRAY);
		auto arr = json.to_array();
		REQUIRE(arr.size() == 3);
		REQUIRE(arr[0].to_bool() == false);

		json_free(json);
	}
}

TEST_CASE("Large Files") {
	/*SECTION("Parse Large File") {
		Json_Value json = parse(load_json_from_file("./test_files/large_test_file_2.json"));
		json_free(json);
	}*/

//	SECTION("Test memory free") {
//		Json json = parse(std::string{R"(
//			{
//				"test": "blah",
//				"foo": "bar",
//				{
//					"hello": 1,
//					"world": "yes"
//				}
//			}
//		)"});
//
//		json_free(json.value);
//	}

	SECTION("Test Memory Leak") {
//		for (int i = 0; i < 15; i++) {
//			Json json = parse(load_json_from_file("large_test_file_2.json"));
//			json_free(json.value);
//		}
////		int a = 0;
	}
}

TEST_CASE("Write/Edit Json") {
	SECTION("Write Numbers") {
		Json_Value json;
		json["foo"] = 3;
		REQUIRE(json["foo"].to_int() == 3);
		REQUIRE(json.type == Value_Type::OBJECT);
		REQUIRE(json.to_obj().size() == 1);
	}

	SECTION("Write Strings") {
		Json_Value json;

		json["foo"] = "hello world";
		REQUIRE(json["foo"].to_str() == "hello world");

		REQUIRE(json.type == Value_Type::OBJECT);
		REQUIRE(json.to_obj().size() == 1);

		Json_Value value = json["foo"];

		REQUIRE(value.type == Value_Type::STRING);
		REQUIRE(json["foo"].to_str().compare("hello world") == 0);
	}
	
	SECTION("Write Objects") {
		Json_Value json = new Json_Object;
	}

//	SECTION("Write Nested Values") {
//		Json_Value json = parse("{}");
////		json["foo"] = ;
//	}

//	SECTION("Write Json_Array") {
//		Json_Value json = parse("[ 1, 2, 3 ]");
//		Json_Array json_arr = json.to_array();
//
//		Json_Value other_json;
//		other_json.type = Value_Type::ARRAY;
//		other_json.value = &json_arr;
//
////		other_json.to_array();
//
////		int a = 0;
//	}

}

TEST_CASE("FILE I/O") {
	SECTION("Write out to file") {
		Json_Value json = parse(std::string{R"(
			{
				"test": "blah",
				"foo": {
					"hello": 1,
					"world": "yes"
				},
				"test_arr": [ 1, 2, 3 ],
				"test_arr_of_objs": [
					{
						"a": 1,
						"b": 2,
						"c": [ 1, 2, 3]
					},
					  {
						  "a": 3,
						  "b": 4,
						"c": [ 1, 2, 3]
					  },
					 {
						 "a": 5,
						 "b": 6,
					"c": [ 1, 2, 3]
					 },
				],
				"test_bool": false
			}
		)"});


//		Json_Value json = parse(load_json_from_file("ship_data.json"));
//
		write_json(json_to_string(json), "write_json_test.json");
	}
}

TEST_CASE("Stuff for Medieval Merchant Game") {
	// FIXED: issue in parse_array in regards to removing parent at end of parse_array
	// trying to save and load the world and having issues
	// currently the save looks like its mostly working, but the load seems like its
	// only loading one faction then stopping
	Json_Value json = parse(std::string{R"(
	{
		 "factions": [
			{
				"towns": []
			},
			{
				"test": "blah"
			}
		]
	 }
	)"});
	
	
}


TEST_CASE("Implicit Conversions") {
	Json_Value json = parse(std::string{R"(
	{
		"id": 7,
		"name": "test name",
		"is_true": true
	}
	)"});
	
	float float_number = json["id"];
//
	REQUIRE(float_number == 7);
//
	int int_number = json["id"];
	REQUIRE(int_number == 7);
	
	std::string name = json["name"];
//
	REQUIRE(name == "test name");
}

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
