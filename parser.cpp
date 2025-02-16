#include "parser.h"
#include <cstdio>
#include <ftxui/component/screen_interactive.hpp>
#include <iostream>
#include <chrono>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <string>
#include <vector>
#include "globalVariables.h"
#include "stringFuncs.h"
#include <regex>

namespace ch = std::chrono;

extern std::string guiOutputText;
extern ftxui::ScreenInteractive screen;

bool filterCheck(std::string& title) {
	if (USE_REGEX) {
		using namespace std::regex_constants;
		if (REGEX_ICASE) return std::regex_search(title, std::regex(FILTER_TERM, ECMAScript | icase));
		else return std::regex_search(title, std::regex(FILTER_TERM));
	} else {
		title = stringToLower(title);
		return title.find(FILTER_TERM) != std::string::npos;
	}
	if (NO_GUI) std::cout << "filterCheck() failure." << std::endl;
	else guiOutputText.insert(0, "filterCheck() failure.\n");
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
	if (NO_GUI) std::cout << "Searching page " << page + 1 << "." << std::endl;
	else { 
		guiOutputText.insert(0, "Searching page " + std::to_string(page+1) + ".\n");
		screen.PostEvent(ftxui::Event::Custom);
	}
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
	if (PRINT_TIME) {
		if (NO_GUI) std::cout << "Time for get: " << getElapsed.count() * 1000 << "ms" << std::endl;
		else {
			guiOutputText.insert(0, "Time for get: " + std::to_string(getElapsed.count() * 1000) + "ms\n");
			screen.PostEvent(ftxui::Event::Custom);
		}
	}

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
					if (NO_GUI) std::cout << "Filter match" << std::endl;
					else {
						guiOutputText.insert(0, "Filter match\n");
						screen.PostEvent(ftxui::Event::Custom);
					}
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
									if (NO_GUI) std::cout << "Error getting username" << std::endl;
									else {
										guiOutputText.insert(0, "Error getting username\n");
										screen.PostEvent(ftxui::Event::Custom);
									}
									outputFile << "Error getting username" << std::endl;
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
						if (PRINT_TIME){
							if (NO_GUI) std::cout << "User get cost: " << userElapsed.count()*1000 << "ms" << std::endl;
							else guiOutputText.insert(0,"User get cost: " + std::to_string(userElapsed.count()*1000) + "ms\n");
						}

						if (addEntry(knownUsers, user, entry)) {
							if (NO_GUI) std::cout << "Error on adding entry to user: " << user << std::endl;
							else guiOutputText.insert(0, "Error on adding entry to user: " + user + '\n');
							return -1;
						}

						if (NO_GUI) std::cout << "Title: " << entry.title << "		User: " << username << std::endl;
						else guiOutputText.insert(0,"Title: " + entry.title + "		User: " + username + '\n');
						outputFile << "\nPage: " << i + 1 << "	Title: " << title << "		User: " << username << std::endl;
					}
					else {
						if (NO_GUI) std::cout << "Title: " << entry.title << std::endl;
						else guiOutputText.insert(0, "Title: " + entry.title + '\n');
						outputFile << "\nPage: " << i + 1 << "	Title: " << title << std::endl;
					}
					if (NO_GUI) std::cout << "https://kemono.su/patreon/user/" << user << "/post/" << id << std::endl;
					else {
						guiOutputText.insert(0, "https://kemono.su/patreon/user/" + user + "/post/" + id + '\n');
						guiOutputText = guiOutputText.substr(0, 2000);
						screen.PostEvent(ftxui::Event::Custom);
					}
					outputFile << "https://kemono.su/patreon/user/" << user << "/post/" << id << std::endl;
				}
			}
		}
		const auto endTime{ch::system_clock::now()};
		const ch::duration<double> elapsedTime{endTime - startTime};
		if (PRINT_TIME){
			if (NO_GUI) std::cout << "Finished page in " << elapsedTime.count() * 1000 << " milliseconds." << std::endl;
			else guiOutputText.insert(0,"Finished page in " + std::to_string(elapsedTime.count() * 1000) + " milliseconds.\n");
		}
		if (titles == 0)
			break;

		i++;
		myFileI.close();
	}

	outputFile.close();

	return 0;
}

