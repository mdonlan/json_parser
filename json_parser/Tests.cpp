//
//  Tests.cpp
//  json_parser
//
//  Created by Michael Donlan on 8/23/21.
//

#include <stdio.h>
#include "json.h"
#include "Tests.h"

void do_tests() {
	printf("\n\nRunning Tests...\n");
	
	Parser* parser = new Parser;
	
	Json_Data* json_data = parse(parser, std::string{R"({"key": "value"}})"});
	
	AST_Node* root_node = json_data->ast->root;
//	assert(root_node);
	
	Assert_True(root_node, true, "Root Node Exists");
	Assert_True(root_node->name, "ROOT", "Root Node Name is Root");
	
	printf("\n\n\n");
}

void Assert_True(void* ptr, bool expected, const std::string& str) {
	printf("%s: ", str.c_str());
	if (ptr) printf("Passed\n");
	else printf("Failed\n");
}

void Assert_True(const std::string& test_str, const std::string& expected, const std::string& str) {
	printf("%s: ", str.c_str());
	if (test_str.compare(expected) == 0) printf("Passed\n");
	else printf("Failed\n");
}
