#include <iostream>

int main(int, char**){
#ifdef __AVX__
	std::cout << "AVX available\n";
#else
	std::cout << "No AVX\n";
#endif
	return 0;
}

