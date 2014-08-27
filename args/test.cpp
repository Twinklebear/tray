#include <iostream>
#include <string>
#include "args.h"

int main(int argc, char **argv){
	if (argc < 8){
		std::cout << "Usage for testing: ./exe -f some_string -a -c an_int -g a_float\n"
			<< "Will continue and try to find any of previous flags that were passed\n";
	}

	if (!flag(argv, argv + argc, "-f")){
		std::cout << "Did not find expected flag '-f'\n";
	}
	else {
		std::string file = getParam<std::string>(argv, argv + argc, "-f");
		std::cout << "-f param: " << file << "\n";
	}
	if (!flag(argv, argv + argc, "-a")){
		std::cout << "Did not find expected flag '-a'\n";
	}
	else {
		std::cout << "Found flag '-a'\n";
	}
	if (!flag(argv, argv + argc, "-c")){
		std::cout << "Did not find expected flag '-c'\n";
	}
	else {
		int i = getParam<int>(argv, argv + argc, "-c");
		std::cout << "-c param: " << i << "\n";
	}
	if (!flag(argv, argv + argc, "-g")){
		std::cout << "Did not find expected flag '-g'\n";
	}
	else {
		float f = getParam<float>(argv, argv + argc, "-g");
		std::cout << "-g param: " << f << "\n";
	}

	return 0;
}

