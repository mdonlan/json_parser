//
//  main.cpp
//  json_parser
//
//  Created by Michael Donlan on 8/21/21.
//

//#include <iostream>


#include "Json.h"
#include "Tests.h"
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include <iostream>
#include <map>


int main(int argc, const char * argv[]) {
	
//	std::string json_str = R"(
//		{
//			"test": "blah blah blah"
//		}
//	)";
	
	const std::string& json_str = load_json_from_file("large_test_file_2.json");
	Json json_data = parse(json_str);

//	//
//	//
//	// serialize ship object
//	struct Tile {
//		int x;
//		int y;
//		int room;
//		std::map<std::string, bool> walls;
//	};
//
//	struct Room {
//		int id;
//		bool has_system;
//		std::string system_name;
//	};
//
//	struct Ship {
//		std::string name;
//		std::vector<Tile> tiles;
//		std::vector<Room> rooms;
//	};
//
//	Ship ship;
//	ship.name = get_string(json_data["name"]);
//
//	auto tiles_arr = get_array(json_data["tiles"]);
//	for (auto tile_data : tiles_arr) {
//		Tile tile;
//		tile.x = get_number(tile_data["x"]);
//		tile.y = get_number(tile_data["y"]);
//		tile.room = get_number(tile_data["room"]);
//
//		auto walls_arr = get_object(tile_data["walls"]);
//		for (auto wall_data : walls_arr) {
//			tile.walls[wall_data->key] = get_bool(wall_data->value_node);
//		}
//
//		ship.tiles.push_back(tile);
//	}
//
//	auto rooms_arr = get_array(json_data["rooms"]);
//	for (auto room_data : rooms_arr) {
//		Room room;
//		room.id = get_number(room_data["id"]);
//		room.has_system = get_bool(room_data["has_system"]);
//		room.system_name = get_string(room_data["system_name"]);
//
//		ship.rooms.push_back(room);
//	}
	// end serialization
	
	
	
	int result = Catch::Session().run(argc, argv);
	return result;
}
