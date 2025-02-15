#pragma once
#include <string>

extern bool PRINT_TIME, GET_USERNAMES, DEBUG_INFO, USE_REGEX, REGEX_ICASE;
extern std::string JSON_NAME, OUT_NAME, SEARCH_TERM, FILTER_TERM;




int setGlobalsFromArguments(int argc, char* argv[]);
