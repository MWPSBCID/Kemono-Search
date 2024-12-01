#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>
#include <string>
#include <cctype>
#include <chrono>
#include <string.h>
#include <vector>
#include "entryTypes.h"
#include "parser.h"
#include "jsonExporter.h"

namespace ch = std::chrono;


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



int main(int argc, char* argv[])
{
	std::vector<User> knownUsers;
	int titles, i = 0;
	bool PRINT_TIME = false, GET_USERNAMES = false, DEBUG_INFO = false;
	std::string filter, search, url, outName = "output.txt", jsonName = "output.json";
	std::ofstream outputFile;
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
				jsonName = argv[i];
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
				outName = argv[i];
				continue;
			}
			
		}

	}
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
	std::cout << outName << std::endl;
	std::cout << "Enter search term: ";
	getline(std::cin, search);
	search = stringToLower(search);
	replaceSpaces(search);
	std::cout << "Search term: " << search << std::endl;
	std::cout << "Enter filter term: ";
	getline(std::cin, filter);
	filter = stringToLower(filter);
	outputFile.open(outName);

	while (true) {
		std::ofstream myFile;
		const auto startTime{ch::system_clock::now()};
		getPage(myFile, search, i, startTime, PRINT_TIME);
		myFile.close();

		titles = 0;

		std::ifstream myFileI("website.json");
		std::string buf, service, published = "unknown", id, user, username = "", title, titleLow;
		Entry entry;
		while (true) {
			getline(myFileI, buf, '"');
			if (myFileI.eof())
				break;
			if (buf == "id") {
				getline(myFileI, buf, '"');
				getline(myFileI, buf, '"');
				id = buf;
			}
			if (buf == "user") {
				getline(myFileI, buf, '"');
				getline(myFileI, buf, '"');
				user = buf;
			}
			if (buf == "service") {
				getline(myFileI, buf, '"');
				getline(myFileI, buf, '"');
				entry.service = buf;
			}
			if (buf == "published") {
				getline(myFileI, buf, '"');
				getline(myFileI, buf, '"');
				entry.published = buf;
			} 
			if (buf == "title") {
				getline(myFileI, buf, '"');
				bool escapeChar;
				entry.title = "";
				do {
					escapeChar = false;
				getline(myFileI, buf, '"');
				entry.title += buf;
				if (entry.title[entry.title.length() - 1] == '\\') {
					entry.title += '\"';
					escapeChar = true;
				}
				} while (escapeChar);
				titleLow = stringToLower(entry.title);
				titles++;
				entry.link = "https://kemono.su/patreon/user/" + user + "/post/" + id;
				//	Get username
				if (titleLow.find(filter) != std::string::npos) {
					std::cout << "Filter match" << std::endl;	
					if (GET_USERNAMES) {

						const auto userStart {ch::system_clock::now()};
						if (!searchUsers(username, knownUsers, user)) {
							std::ofstream userFile;
							userFile.open("user.json");
							getUser(userFile, user, username, knownUsers, entry.service);
							userFile.close();
							//	Get username end
							std::ifstream userFileI("user.json");
						
							while (true) {
								getline(userFileI, buf, '"');
								if (buf == "name") {
									getline(userFileI, buf, '"');
									getline(userFileI, buf, '"');
									username = buf;
									break;
								}
								if (myFileI.eof()) {
									std::cout << "Error getting username";
									outputFile << "Error getting username";
									break;
								}
								
							}
							userFileI.close();
							if (username != "") {
								User u {user, username};
								knownUsers.push_back(u);
							}
						}
						const auto userEnd {ch::system_clock::now()};
						ch::duration<double> userElapsed {userEnd - userStart};
						if (PRINT_TIME) std::cout << "User get cost: " << userElapsed.count()*1000 << "ms" << std::endl;
						
						if (addEntry(knownUsers, user, entry)) {
							std::cout << "Error on adding entry to user: " << user;
							return -1;
						}

						std::cout << "Title: " << entry.title << "		User: " << username << std::endl;
						outputFile << "\nPage: " << i + 1 << "	Title: " << title << "		User: " << username << std::endl;
					}
					else {
						std::cout << "Title: " << entry.title << std::endl;
						outputFile << "\nPage: " << i + 1 << "	Title: " << title << std::endl;
					}
					std::cout << "https://kemono.su/patreon/user/" << user << "/post/" << id << std::endl;
					outputFile << "https://kemono.su/patreon/user/" << user << "/post/" << id << std::endl;
				}
			}
		}
		const auto endTime{ch::system_clock::now()};
		const ch::duration<double> elapsedTime{endTime - startTime};
		if (PRINT_TIME) std::cout << "Finished page in " << elapsedTime.count() * 1000 << " milliseconds" << std::endl;

		if (titles == 0)
			break;

		i++;
		myFileI.close();
	}
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

	exportJsonData(knownUsers, jsonName);

	outputFile.close();
	return 0;
}
