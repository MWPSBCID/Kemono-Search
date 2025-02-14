#include <iostream>
#include <ostream>
#include <string>
#include <cctype>
#include <vector>
#include "entryTypes.h"
#include "parser.h"
#include "jsonExporter.h"
#include "stringFuncs.h"
#include "userFunctions.h"



int getUserInput() {

	std::string filter, search;

	std::cout << OUT_NAME << std::endl;
	if (SEARCH_TERM == "") {
		std::cout << "Enter search term: ";
		getline(std::cin, search);
	} else search = SEARCH_TERM;
	SEARCH_TERM = stringToLower(search);
	replaceSpaces(SEARCH_TERM);
	std::cout << "Search term: " << SEARCH_TERM << std::endl;
	if (FILTER_TERM == "") {
	std::cout << "Enter filter term: ";
	getline(std::cin, filter);
	} else filter = FILTER_TERM;
	FILTER_TERM = stringToLower(filter);
	std::cout << "Filter term: " << FILTER_TERM << std::endl;

	return 0;
}



int main(int argc, char* argv[])
{
	std::vector<User> knownUsers;

	setGlobalsFromArguments(argc, argv);

	readUsers(knownUsers);

	getUserInput();


	runParsingLoop(knownUsers);

	saveUsers(knownUsers);

	std::cout << "Finished parsing!" << std::endl;
	
	printUserDebug(knownUsers);

	exportJsonData(knownUsers);

	return 0;
}
