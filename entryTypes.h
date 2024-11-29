#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <fstream>

struct Entry {
	std::string title;
	std::string link;
	std::string published;
	std::string service;

};


inline std::ostream& operator<< (std::ostream& os, const Entry& entry) {

	os << "{\"title\":\"" << entry.title << "\",\"link\":\"" << entry.link << "\",\"published\":\"" << entry.published << "\",\"service\":\"" << entry.service << "\"}";
	return os;

};

struct User {
	std::string id;
	std::string name;
	std::vector<Entry> entries;


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


