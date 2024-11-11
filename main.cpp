#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <cctype>
#include <chrono>
#include <string.h>

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
	int titles, i = 0;
	bool PRINT_TIME = false, GET_USERNAMES = false;
	std::string filter, search, url, outName = "output.txt";
	std::ofstream outputFile;
	if (argc > 1) {
		
		for (i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "-T") == 0) {
				PRINT_TIME = true;
				continue;
			}
			if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "-U") == 0) {
				GET_USERNAMES = true;
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

		url = "https://kemono.su/api/v1/posts?q=";
		url += search;
		url += "&o=";
		url += std::to_string(i*50);
		std::cout << "Searching page " << i + 1 << "." << std::endl;
		const auto startTime{ch::system_clock::now()};
		std::ofstream myFile;
		myFile.open("website.json");
		try
		{
			// That's all that is needed to do cleanup of used resources (RAII style).
			curlpp::Cleanup myCleanup;

			// Our request to be sent.
			curlpp::Easy myRequest;

			// Set the URL.
			myRequest.setOpt<curlpp::options::Url>(url);

			// Set output file stream
			myRequest.setOpt(cURLpp::Options::WriteStream(&myFile));

			// Send request and get a result.
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
		const auto getTime{ch::system_clock::now()};
		ch::duration<double> getElapsed{getTime-startTime};
		if (PRINT_TIME) std::cout << "Time for get: " << getElapsed.count() * 1000 << "ms" << std::endl;
		myFile.close();

		titles = 0;

		std::ifstream myFileI("website.json");
		std::string buf, service, id, user, username, title, titleLow;
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
				service = buf;
			}
			if (buf == "title") {
				getline(myFileI, buf, '"');
				getline(myFileI, buf, '"');
				title = buf;
				titleLow = stringToLower(title);
				titles++;

				//	Get username
				if (titleLow.find(filter) != std::string::npos) {
					
					if (GET_USERNAMES) {
						const auto userStart {ch::system_clock::now()};
						std::ofstream userFile;
						userFile.open("user.json");

						url = "https://kemono.su/api/v1/patreon/user/";
						url += user;
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
							myRequest.setOpt(cURLpp::Options::WriteStream(&userFile));

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
						const auto userEnd {ch::system_clock::now()};
						ch::duration<double> userElapsed {userEnd - userStart};
						if (PRINT_TIME) std::cout << "User get cost: " << userElapsed.count()*1000 << "ms" << std::endl;

						std::cout << "Title: " << title << "		User: " << username << std::endl;
						outputFile << "\nPage: " << i + 1 << "	Title: " << title << "		User: " << username << std::endl;
					}
					else {
						std::cout << "Title: " << title << std::endl;
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

	std::cout << "Finished parsing!" << std::endl;

	outputFile.close();
	return 0;
}
