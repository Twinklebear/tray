#include <string>
#include <array>
#include <regex>
#include <iostream>

int main(int , char**){
	std::regex match_file{".*\\.[a-zA-Z]{3}"};
	std::array<std::string, 4> files{
		"checkerboard", "file.ppm",
		"not_file", "chicken.png"
	};
	for (const auto &s : files){
		std::smatch match;
		if (std::regex_match(s.begin(), s.end(), match, match_file)){
			std::cout << "Matched on " << s << std::endl;
		}
	};
	return 0;
}

