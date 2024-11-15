#include <fstream>
#include <string>
#include <vector>
#include <chrono>

namespace ch = std::chrono;

struct User {
	std::string id;
	std::string name;

};

inline std::ostream& operator<< (std::ostream& os, const User& user) {
	os << '{' << user.id << ':' << user.name << '}';
	return os;
};
inline std::istream& operator>> (std::istream& is, User& user) {
	bool fail = false;
	char ch = '-';
	if (!is.get(ch) || ch != '{') fail = true;
	if (!std::getline(is, user.id, ':')) fail = true;
	if (!std::getline(is, user.name, '}')) fail = true;
	
	if (fail) is.setstate(std::istream::failbit);
	return is;
}


bool searchUsers(std::string& username, const std::vector<User>& userVector, const std::string userId);

int getPage(std::ofstream& file, std::string search, int page, ch::time_point<ch::system_clock> startTime, bool PRINT_TIME);

int getUser(std::ofstream& file, std::string userId, std::string& username, std::vector<User>);
