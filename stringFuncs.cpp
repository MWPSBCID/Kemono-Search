

#include "stringFuncs.h"


int replaceSpaces(std::string& input) {
	int len = input.length();
	int i;
	for (i = 0; i < len; i++) {
		if (input[i] == ' ')
			input[i] = '+';
	}
	return 0;
}

std::string stringToLower(std::string input) {
	for (char& c : input) {
		c = std::tolower(c);
	}
	return input;
}
