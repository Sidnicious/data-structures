#include "util.h"

template <typename T>
struct list {

	struct cell {
		T car;
		cell *cdr;

		~cell() { if (cdr) { delete cdr; } }
	};

	cell *head;

	list() : head(nullptr) {}
	list(const list &) = delete;

	template <typename ...Args>
	explicit list(Args &&...args) : list() {
		emplace_back(std::forward<Args>(args)...);
	}

	~list() {
		if (head) {
			dlog("free head ", head);
			delete head;
		}
	}

	template <typename ...Args>
	T &emplace_back(Args &&...args) {
		cell **next = &head;
		while (*next) { next = &(*next)->cdr; }
		// return (*next = new cell{T{std::forward<Args>(args)...}, nullptr})->car;
		*next = new cell{T{std::forward<Args>(args)...}, nullptr};
		dlog(this, " append at ", next, " ", *next);
		return (*next)->car;
	}

	struct iterator {

		cell *c;

		iterator() : c(nullptr) {}
		iterator(list *l) : c(l->head) {}

		bool operator!=(iterator &other) { return c != other.c; }
		iterator &operator++() { c = c->cdr; return *this; }
		T &operator*() { return c->car; }
	};

	iterator begin() { return iterator(this); }
	iterator end() { return iterator(); }
};
