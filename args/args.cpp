#include <string>
#include <algorithm>
#include "args.h"

/*
 * Course: CS100
 * First Name: Will
 * Last Name: Usher
 * Username: wushe001
 * email address: wushe001@ucr.edu
 *
 * Assignment: Varies.
 *
 * I hereby certify that the contents of this file represent
 * my own original individual work. Nowhere herein is there
 * code from any outside resources such as another individual,
 * a website, or publishings unless specifically designated as
 * permissible by the instructor or TA
 */

bool flag(char **begin, char **end, const std::string &f){
	return std::find(begin, end, f) != end;
}
template<>
std::string getParam(char **beg, char **end, const std::string &f){
	char **it = std::find(beg, end, f);
	if (it != end && ++it != end){
		return std::string(*it);
	}
	return std::string("");
}
template<>
int getParam(char **beg, char **end, const std::string &f){
	char **it = std::find(beg, end, f);
	if (it != end && ++it != end){
			return std::stoi(*it);
	}
	return 0;
}
template<>
float getParam(char **beg, char **end, const std::string &f){
	char **it = std::find(beg, end, f);
	if (it != end && ++it != end){
		return std::stof(*it);
	}
	return 0;
}

