#include "parser.h"
#include <cstdio>
#include <iostream>
#include <chrono>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <vector>
#include "globalVariables.h"
#include "stringFuncs.h"
#include <regex>

namespace ch = std::chrono;


bool filterCheck(std::string& title) {
	if (USE_REGEX) {
		using namespace std::regex_constants;
		if (REGEX_ICASE) return std::regex_search(title, std::regex(FILTER_TERM, ECMAScript | icase));
		else return std::regex_search(title, std::regex(FILTER_TERM));
	} else {
		title = stringToLower(title);
		return title.find(FILTER_TERM) != std::string::npos;
	}
	std::cout << "filterCheck() failure." << std::endl;
	return false;
}


bool searchUsers(std::string& username, const std::vector<User>& userVector, const std::string userId) {
	
	for (User u : userVector) {
		if (u.id == userId) {
			username = u.name;
			return true;
		}
	}
	return false;
}


int getPage(std::ofstream& file, std::string search, int page, ch::system_clock::time_point startTime) {
	std::string url;
	url = "https://kemono.su/api/v1/posts?q=";
	url += search;
	url += "&o=";
	url += std::to_string(page*50);
	std::cout << "Searching page " << page + 1 << "." << std::endl;
	file.open("website.json");
	try
	{
		// That's all that is needed to do cleanup of used resources (RAII style).
		curlpp::Cleanup myCleanup;

		// Our request to be sent.
		curlpp::Easy myRequest;

		// Set the URL.
		myRequest.setOpt<curlpp::options::Url>(url);

		// Set output file stream
		myRequest.setOpt(cURLpp::Options::WriteStream(&file));

		// Send request and get a result.
		myRequest.perform();
	}
	catch (curlpp::RuntimeError& e)
	{
		std::cout << e.what() << std::endl;
		return -1;
	}
	catch (curlpp::LogicError& e)
	{
		std::cout << e.what() << std::endl;
		return -1;
	}
	const auto getTime{ch::system_clock::now()};
	ch::duration<double> getElapsed{getTime-startTime};
	if (PRINT_TIME) std::cout << "Time for get: " << getElapsed.count() * 1000 << "ms" << std::endl;

	return 0;
}

int getUser(std::ofstream& file, std::string userId, std::string& username, std::vector<User> knownUsers, std::string service) {


		std::string url = "https://kemono.su/api/v1/" + service + "/user/";
		url += userId;
		url += "/profile";
		try
		{
			// That's all that is needed to do cleanup of used resources (RAII style).
			curlpp::Cleanup myCleanup;

			// Our request to be sent.
			curlpp::Easy myRequest;

			// Set the URL.
			myRequest.setOpt<curlpp::options::Url>(url);

			// Set output file stream
			myRequest.setOpt(cURLpp::Options::WriteStream(&file));

			// Send request and get a result.
			// By default the result goes to standard output.
			myRequest.perform();
		}

		catch (curlpp::RuntimeError& e)
		{
			std::cout << e.what() << std::endl;
		}

		catch (curlpp::LogicError& e)
		{
			std::cout << e.what() << std::endl;
		}
	return 0;
}

int addEntry(std::vector<User>& users, std::string userId, Entry entry) {
	
	for (User& user : users) {
		if (user.id == userId) {
			user.entries.push_back(entry);
			return 0;
		}
	}

	return -1;
};


int runParsingLoop(std::vector<User>& knownUsers) {

	int titles, i = 0;
	std::ofstream outputFile;
	outputFile.open(OUT_NAME);

	while (true) {
		std::ofstream myFile;
		const auto startTime{ch::system_clock::now()};
		getPage(myFile, SEARCH_TERM, i, startTime);
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
				titles++;
				entry.link = "https://kemono.su/patreon/user/" + user + "/post/" + id;
				//	Get username
				if (filterCheck(entry.title)) {
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

	outputFile.close();

	return 0;
}

