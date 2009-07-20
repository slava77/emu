#ifndef RegEx_hh
#define RegEx_hh

#ifndef __cplusplus
#error "This file should be compiled by C++ compiler"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

//  stdlib++ interface to standard regex. Searches through the whole string
//  Result is a map array with matched patterns as keys and array of selected
//  subpatterns as a value
//  Example:
//	map< string, vector<string> > result = RegExSearch("(\\w+) (\\w+)", "This is a test");
//  result.begin()->first == "This is";
//  result.begin()->second[0] = "This";
//  result.begin()->second[1] = "is";
//  result.end()->first == "a test";
//  result.end()->second[0] = "a";
//  result.end()->second[1] = "test";
std::map< std::string, std::vector<std::string> > RegExSearch(const char *pattern, const char *target);

//  Perl's $var =~ s/(\\w)\\.(\\w)/$2.$1/gx can be written as
//  var = RegExSubstitude("(\\w)\\.(\\w)", "$2.$1", "first.second"); (var == "second.first")
std::string RegExSubstitute(const char *pattern_from, const char *pattern_to, const char* target);

#endif
