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

constexpr bool DEV_MODE = true;

int main(int argc, const char * argv[]) {
	if (DEV_MODE) {
		int result = Catch::Session().run(argc, argv);
		return result;
	} else {
		return 0;
	}
}
