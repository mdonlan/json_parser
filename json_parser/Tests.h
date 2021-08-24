//
//  Tests.h
//  json_parser
//
//  Created by Michael Donlan on 8/23/21.
//

#ifndef Tests_h
#define Tests_h
#include <string.h>

void do_tests();

void Assert_True(void* ptr, bool expected, const std::string& str);
void Assert_True(const std::string& test_str, const std::string& expected, const std::string& str);

#endif /* Tests_h */
