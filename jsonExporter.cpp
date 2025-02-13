#include "jsonExporter.h"
#include <fstream>
#include "globalVariables.h"

int exportJsonData(std::vector<User> users) {
	
	std::ofstream jsonFile(JSON_NAME);

	jsonFile << '[';

	bool firstUser = true;

	for (auto user : users) {
		
		if (user.entries.size() == 0) continue;

		if (!firstUser) {
			jsonFile << ',';
		}

		firstUser = false;

		jsonFile << "{";

		jsonFile << "\"id\":\"" << user.id << "\",";
		jsonFile << "\"name\":\"" << user.name << "\",";
		
		jsonFile << "\"entries\":[";
		
		bool firstEntry = true;

		for (auto entry : user.entries) {
			if (!firstEntry) {
				jsonFile << ',';
			}
			firstEntry = false;
			jsonFile << entry;
	
		}

		jsonFile << "]";

		jsonFile << "}";
	}
	

	jsonFile << ']';

	jsonFile.close();

	return 0;
};
