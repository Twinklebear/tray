#include <string>
#include <algorithm>
#include "args.h"

bool flag(char **begin, char **end, const std::string &f){
	return std::find(begin, end, f) != end;
}
template<>
std::string get_param(char **beg, char **end, const std::string &f){
	char **it = std::find(beg, end, f);
	if (it != end && ++it != end){
		return std::string(*it);
	}
	return std::string("");
}
template<>
int get_param(char **beg, char **end, const std::string &f){
	char **it = std::find(beg, end, f);
	if (it != end && ++it != end){
			return std::stoi(*it);
	}
	return 0;
}
template<>
float get_param(char **beg, char **end, const std::string &f){
	char **it = std::find(beg, end, f);
	if (it != end && ++it != end){
		return std::stof(*it);
	}
	return 0;
}

