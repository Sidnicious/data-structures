#include <inttypes.h>
#include "hash_table.hpp"

// https://en.wikipedia.org/wiki/Fowler_Noll_Vo_hash
template<>
uint64_t hash<const char *>(const char *s) {
	uint64_t h = 0xcbf29ce484222325;
	for (; *s; s++) {
		h *= 0x100000001b3;
		h ^= *s;
	}
	return h;
}

// https://en.wikipedia.org/wiki/Fowler_Noll_Vo_hash
template<>
uint64_t hash<std::string>(const std::string s) { return hash(s.c_str()); }

#include <iostream>
#include <sstream>
#include <unistd.h>

int main(int argc, char *argv[]) {
	hash_table<std::string, int> table;

	for (int i = 0; i < 10000; i++) {
		std::ostringstream oss;
		oss << i;
		table[oss.str()] = i;
	}

	if (auto entry = table[argv[1]]) {
		std::cout << *entry << std::endl;
	} else {
		std::cout << "Not found" << std::endl;
	}
}
