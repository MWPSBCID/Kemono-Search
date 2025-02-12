#pragma once
#include <string>

extern bool PRINT_TIME, GET_USERNAMES, DEBUG_INFO;
extern std::string JSON_NAME, OUT_NAME;




int setGlobalsFromArguments(int argc, char* argv[]);
