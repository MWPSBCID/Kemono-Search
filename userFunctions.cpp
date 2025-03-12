#include <vector>
#include "entryTypes.h"
#include "globalVariables.h"
#include <iostream>


int readUsers(std::vector<User>& knownUsers) {
	if (GET_USERNAMES) {
		std::ifstream usersFile("users.txt");
		User u;
		std::string buf;
		while (true) {
			if(!(usersFile >> u)) std::cout << "User read failed.";
			if (usersFile.fail()) break;
			knownUsers.push_back(u);
			getline(usersFile, buf);
		}
		usersFile.close();
	}


	return 0;
}

int saveUsers(std::vector<User>& knownUsers) {

	if (GET_USERNAMES) {
		std::ofstream usersFile("users.txt");
		for (User u : knownUsers) {
			usersFile << u << std::endl;
		}
		usersFile.close();
	}

	return 0;
}

int printUserDebug(std::vector<User>& knownUsers) {

	if (DEBUG_INFO) {
		for (auto u : knownUsers) {
			std::cout << u << ": " << u.entries.size() <<  std::endl;
			for (auto e : u.entries) {

				std::cout << "Entry: " << "	" << e << std::endl;
			}
		}
	}

	return 0;
}
