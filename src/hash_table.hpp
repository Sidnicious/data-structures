#include <utility>
#include <assert.h>
#include "list.hpp"
#include "util.h"
#include "binary.h"

template <typename T>
uint64_t hash(T);

template <typename K, typename V>
struct hash_table {

	struct entry {
		K key;
		V value;
	};

	list<entry> *buckets;

	size_t size;
	size_t entries;
	double max_load_factor;

	auto get_bucket(const K &key) -> decltype(buckets[0])& {
		return buckets[hash(key) % size];
	}

	explicit hash_table(size_t _size = 128, double _max_load_factor = 0.75) :
		buckets(new list<entry>[_size]), size(_size),
		entries(0), max_load_factor(_max_load_factor)
	{}

	hash_table(const hash_table &other) = delete;
	hash_table &operator=(hash_table &&other) {
		std::swap(buckets, other.buckets);
		size = other.size;
		entries = other.entries;
		max_load_factor = other.max_load_factor;
		return *this;
	}
	hash_table(hash_table &&other) : buckets(nullptr) { *this = std::move(other); }

	~hash_table() { if (buckets) { delete[] buckets; } }

	double load() {
	return ((double)entries + 1) / size;
	}

	void resize(size_t new_size) {
		dlog("resize(", new_size, ") start");
		assert(new_size >= entries);
		new_size = min_power_of_two(new_size);
		auto replacement = hash_table(new_size, max_load_factor);
		for (size_t i = 0; i < size; i++) {
			for (const auto &v : buckets[i]) {
				replacement.insert(v.key, std::move(v.value));
			}
		}
		std::swap(*this, replacement);
		dlog("resize(", new_size, ") end");
	}

	template <typename T>
	V &insert(const K &key, T &&value) {
		dlog("insert(", key, ", ", value, ") load factor: ", load());
		if (load() > max_load_factor) {
			resize(size * 2);
		}
		entries++;
		auto &bucket = get_bucket(key);
		for (auto &slot : bucket) {
			if (slot.key == key) {
				return (slot.value = value);
			}
		}
		return bucket.emplace_back(key, value).value;
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
			if (slot.key == key) {
				value = &slot.value;
				break;
			}
		}

		return entry_ref{*this, key, value};
	}
};
