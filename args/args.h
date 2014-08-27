#ifndef ARGS_H
#define ARGS_h

#include <string>
#include <sstream>
#include <algorithm>

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

/*
 * Some utilities for parsing command line arguments and
 * conveniently checking selected flags and getting parameter
 * values
 */
/*
 * Check if a flag was passed
 * @param beg The beginning of the args array
 * @param end One past the end of the args array
 * @param f The flag to check for, ex. -f
 * @return true if the flag was passed
 */
bool flag(char **beg, char **end, const std::string &f);
/*
 * Get the parameter passed for some flag. It's best to make sure
 * the flag was passed before trying to retrieve its argument
 * if the flag isn't found T's default value will be returned
 * @param beg The beginning of the args array
 * @param end One past the end of the args array
 * @param f The flag to get the parameter for, ex. -f
 * @return The value of the parameter
 */
template<class T>
T getParam(char **beg, char **end, const std::string &f){
	char **it = std::find(beg, end, f);
	if (it != end && ++it != end){
		std::stringstream ss;
		ss << *it;
		T t;
		ss >> t;
		return t;
	}
	return T();
}
//Some quicker versions of getParam for certain types
template<>
std::string getParam(char **beg, char **end, const std::string &g);
template<>
int getParam(char **beg, char **end, const std::string &g);
template<>
float getParam(char **beg, char **end, const std::string &g);

#endif

