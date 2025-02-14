#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <cctype>
#include <vector>
#include "entryTypes.h"
#include "parser.h"
#include "jsonExporter.h"
#include "stringFuncs.h"
#include "globalVariables.h"


int getUserInput(std::string& search, std::string& filter) {

	std::cout << OUT_NAME << std::endl;
	if (SEARCH_TERM == "") {
		std::cout << "Enter search term: ";
		getline(std::cin, search);
	} else (search = SEARCH_TERM);
	SEARCH_TERM = stringToLower(search);
	replaceSpaces(SEARCH_TERM);
	std::cout << "Search term: " << SEARCH_TERM << std::endl;
	if (FILTER_TERM == "") {
	std::cout << "Enter filter term: ";
	getline(std::cin, filter);
	} else (filter = FILTER_TERM);
	FILTER_TERM = stringToLower(filter);
	std::cout << "Filter term: " << FILTER_TERM << std::endl;

	return 0;
}



int main(int argc, char* argv[])
{
	std::vector<User> knownUsers;
	int i = 0;
	std::string filter, search, url;

	setGlobalsFromArguments(argc, argv);

	if (GET_USERNAMES) {
		std::ifstream usersFile("users.txt");
		User u;
		std::string buf;
		while (true) {
			usersFile >> u;
			if (usersFile.fail()) break;
			knownUsers.push_back(u);
			getline(usersFile, buf);
		}
		usersFile.close();
	}
	i = 0;

	getUserInput(search, filter);


	runParsingLoop(knownUsers);

	if (GET_USERNAMES) {
		std::ofstream usersFile("users.txt");
		for (User u : knownUsers) {
			usersFile << u << std::endl;
		}
		usersFile.close();
	}
	std::cout << "Finished parsing!" << std::endl;
	
	if (DEBUG_INFO) {
		for (auto u : knownUsers) {
			std::cout << u << ": " << u.entries.size() <<  std::endl;
			for (auto e : u.entries) {

				std::cout << "Entry: " << "	" << e << std::endl;
			}
		}
	}

	exportJsonData(knownUsers);

	return 0;
}
