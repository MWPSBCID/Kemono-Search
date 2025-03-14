#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include "entryTypes.h"

namespace ch = std::chrono;





bool searchUsers(std::string& username, const std::vector<User>& userVector, const std::string userId);

int getPage(std::ofstream& file, std::string search, int page, ch::time_point<ch::system_clock> startTime);

int getUser(std::ofstream& file, std::string userId, std::string& username, std::vector<User> user, std::string service);

int addEntry(std::vector<User>& users, std::string userId, Entry entry);

int runParsingLoop(std::vector<User>& knownUsers);
