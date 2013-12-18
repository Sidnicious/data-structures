#include <inttypes.h>
#include <math.h>
#include <vector>
#include <assert.h>
#include "util.h"

template <typename T>
uint64_t hash(T);

template <typename K, typename V>
class hash_table {

	std::vector<std::vector<std::pair<K, V>>> buckets;
	size_t entries;
	double max_load_factor;

	protected:

	auto get_bucket(const K &key) -> decltype(buckets.back())& {
		return buckets[hash(key) % buckets.size()];
	}

	public:
	explicit hash_table(size_t size, double _max_load_factor = 0.75) :
		entries(0), max_load_factor(_max_load_factor)
	{
		buckets.resize(size);
	}

	hash_table() : hash_table(1) {}

	void resize(size_t new_size) {
		dlog("resize(", new_size, ") start");
		assert(new_size >= entries);
		new_size = pow(2, ceil(log(new_size)/log(2)));
		auto replacement = hash_table(new_size, entries);
		for (const auto &b : buckets) {
			for (const auto &v : b) {
				replacement.insert(v.first, v.second);
			}
		}
		std::swap(*this, replacement);
		dlog("resize(", new_size, ") end");
	}

	template <typename T>
	V &insert(const K &key, T &&value) {
		dlog("insert(", key, ", ", value, ") load factor: ", ((double)entries + 1) / buckets.size());
		if ((entries + 1) / buckets.size() > max_load_factor) {
			resize(buckets.size() * 2);
		}
		entries++;
		auto &bucket = get_bucket(key);
		for (auto &slot : bucket) {
			if (slot.first == key) {
				return (slot.second = value);
			}
		}
		bucket.emplace_back(key, value);
		return bucket.back().second;
	}

	class entry_ref {
		hash_table &table;
		K key;
		V *value;

		public:
		entry_ref(hash_table &_table, const K &_key, V *_value) :
			table(_table), key(_key), value(_value)
		{}

		V &operator *() { return *value; }
		operator bool() { return value; }

		template <typename T>
		entry_ref &operator =(T &&v) {
			if (value) {
				*value = v;
			} else {
				table.insert(key, std::forward<T>(v));
			}
			return *this;
		}
	};

	entry_ref operator [](const K &key) {
		V *value{nullptr};

		for (auto &slot : get_bucket(key)) {
			if (slot.first == key) {
				value = &slot.second;
				break;
			}
		}

		return entry_ref{*this, key, value};
	}
};

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

int main() {
	hash_table<std::string, int> table;

	for (int i = 0; i < 2000; i++) {
		std::ostringstream oss;
		oss << i;
		table[oss.str()] = i;
	}

	std::cout << *table["5"] << std::endl;
	std::cout << *table["100"] << std::endl;
}
