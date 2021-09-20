//
//  main.cpp
//  json_parser
//
//  Created by Michael Donlan on 8/21/21.
//

//#include <iostream>


#include "Json.h"
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "Tests.h"

#define DEVMODE 1

//#include <iostream>
//#include <map>
//#include <filesystem>
//namespace fs = std::filesystem;

int main(int argc, const char * argv[]) {
	
//	Json json = parse(load_json_from_file("ship_test_2.json"));

#if DEVMODE
	int result = Catch::Session().run(argc, argv);
	return result;
#endif
	return 0;
}
