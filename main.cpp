#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

#define GET_USERNAMES false

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

	std::string filter, search, url, outName;
	std::ofstream outputFile;
	if (argc > 1)
		outName = argv[2];
	else
		outName = "output.txt";
	outputFile.open(outName);
	std::cout << outName << std::endl;
	std::cout << "Enter search term: ";
	getline(std::cin, search);
	search = stringToLower(search);
	replaceSpaces(search);
	std::cout << "Search term: " << search << std::endl;
	std::cout << "Enter filter term: ";
	getline(std::cin, filter);
	filter = stringToLower(filter);

	while (true) {

		url = "https://kemono.su/api/v1/posts?q=";
		url += search;
		url += "&o=";
		url += std::to_string(i*50);
		std::cout << "Searching page " << i + 1 << "." << std::endl;

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
				//std::cout << "Found title." << std::endl;
				titles++;

				//	Get username
				if (GET_USERNAMES) {
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
				}
				else
					username = "unknown";
				if (titleLow.find(filter) != std::string::npos) {
					
					if (GET_USERNAMES) {
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

		if (titles == 0)
			break;

		i++;
		myFileI.close();
	}

	std::cout << "Finished parsing!" << std::endl;

	outputFile.close();
	return 0;
}