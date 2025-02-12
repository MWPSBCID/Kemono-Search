#include "globalVariables.h"
#include <string.h>
#include <iostream>



bool PRINT_TIME, GET_USERNAMES, DEBUG_INFO;
std::string JSON_NAME, OUT_NAME;

int setGlobalsFromArguments(int argc, char* argv[]) {
	int i = 0;
	PRINT_TIME = false;
	GET_USERNAMES = false;
	DEBUG_INFO = false;
	OUT_NAME = "output.txt";
	JSON_NAME = "output.json";
	if (argc > 1) {
		
		for (i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "-T") == 0) {
				PRINT_TIME = true;
				continue;
			}
			if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "-D") == 0) {
				DEBUG_INFO = true;
				continue;
			}
			if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "-U") == 0) {
				GET_USERNAMES = true;
				continue;
			}
			if (strcmp(argv[i], "-j") == 0 || strcmp(argv[i], "-J") == 0) {
				i++;
				if (i == argc) {
					std::cout << "No JSON file." << std::endl;
					return -1;
				}
				if (argv[i][0] == '-') {
					std::cout << "JSON file may not start with -" << std::endl;
					return -1;
				}
				JSON_NAME = argv[i];
				continue;
			}
			if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-O") == 0) {
				i++;
				if (i == argc) {
					std::cout << "No output file." << std::endl;
					return -1;
				}
				if (argv[i][0] == '-') {
					std::cout << "Output file may not start with -" << std::endl;
					return -1;
				}
				OUT_NAME = argv[i];
				continue;
			}
			
		}

	}
	return 0;
}
