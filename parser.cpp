#include "parser.h"
#include <iostream>
#include <chrono>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <vector>
#include "globalVariables.h"

namespace ch = std::chrono;


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

