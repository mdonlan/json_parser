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
	
	Json json = parse("{}");
	
	
	// problem
	// we want create a new property w/ the name test and the int value 7
	// however current our code does
	// 1. json["test"] -> returns the Basic Value from the property w/ name "test",
	//		however there is currently no property w/ that name
	// 2. it then attempts to set the value of that Basic Value to 7
//	json["test"] = 7;
	
//	Basic_Value* value = json["test"];
	json["test"] = 7;
	
	
	int a = 0;
	
#if DEVMODE
	int result = Catch::Session().run(argc, argv);
	return result;
#endif
	return 0;
}
