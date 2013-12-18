#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

// https://en.wikipedia.org/wiki/Fowler_Noll_Vo_hash
uint64_t hash(const char *s) {
	uint64_t h = 0xcbf29ce484222325;
	for (; *s; s++) {
		h *= 0x100000001b3;
		h ^= *s;
	}
	return h;
}

typedef struct hash_table_entry {
	char *key;
	int value;
} hash_table_entry;

hash_table_entry *hash_table_entry_new(const char *key, int value) {
	hash_table_entry *b = malloc(sizeof(hash_table_entry));
	b->key = strdup(key);
	b->value = value;
	return b;
}

void hash_table_entry_free(hash_table_entry *b) {
	free(b->key);
	free(b);
}

typedef struct hash_table {
	size_t entries;
	vector buckets;
} hash_table;

#define HASH_TABLE_DEFAULT_BUCKET_SIZE 2
#define HASH_TABLE_DEFAULT_BUCKETS 16
#define HASH_TABLE_MAX_LOAD_FACTOR 0.75

hash_table hash_table_new_size(size_t buckets) {
	hash_table t = {0, vector_new_capacity(buckets)};
	for (size_t i = 0; i < buckets; i++) {
		vector *bucket = malloc(sizeof(vector));
		*bucket = vector_new_capacity(HASH_TABLE_DEFAULT_BUCKET_SIZE);
		vector_append(&t.buckets, bucket);
	}
	return t;
}

hash_table hash_table_new() {
	return hash_table_new_size(HASH_TABLE_DEFAULT_BUCKETS);
}

void hash_table_free(hash_table *t) {
	vector_free(&t->buckets);
}

vector *hash_table_get_bucket(hash_table *t, const char *key) {
	return vector_get(&t->buckets, hash(key) % vector_size(&t->buckets));
}

hash_table_entry *hash_table_get_entry(vector *bucket, const char *key) {
	for (size_t i = 0; i < vector_size(bucket); i++) {
		hash_table_entry *e = vector_get(bucket, i);
		if (strcmp(e->key, key) == 0) { return e; }
	}
	return NULL;
}

double hash_table_get_load(hash_table *t) {
	return (double)t->entries / vector_size(&t->buckets);
}

void hash_table_adopt(hash_table *t, hash_table *r) {
	hash_table_free(t);
	t->entries = r->entries;
	t->buckets = r->buckets;
}

void hash_table_set(hash_table *t, const char *key, int value) {
	if (hash_table_get_load(t) >= HASH_TABLE_MAX_LOAD_FACTOR) {
		hash_table replacement = hash_table_new_size(vector_size(&t->buckets) * 2);
		for (size_t i = 0; i < vector_size(&t->buckets); i++) {
			vector *bucket = vector_get(&t->buckets, i);
			for (size_t j = 0; j < vector_size(bucket); j++) {
				hash_table_entry *e = vector_get(bucket, j);
				replacement.entries++;
				vector_append(hash_table_get_bucket(&replacement, e->key), e);
			}
		}
		hash_table_adopt(t, &replacement);
	}
	vector *b = hash_table_get_bucket(t, key);
	hash_table_entry *e = hash_table_get_entry(b, key);
	if (e) {
		e->value = value;
	} else {
		t->entries++;
		vector_append(b, hash_table_entry_new(key, value));
	}
}

int *hash_table_get(hash_table *t, const char *key) {
	vector *b = hash_table_get_bucket(t, key);
	hash_table_entry *e = hash_table_get_entry(b, key);
	if (e) {
		return &e->value;
	} else {
		return NULL;
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s number_below_10000\n", argv[0]);
		exit(1);
	}

	hash_table table = hash_table_new();

	for (size_t i = 0; i < 10000; i++) {
		char *key;
		asprintf(&key, "%zu", i);
		hash_table_set(&table, key, i);
		free(key);
	}

	int *entry = hash_table_get(&table, argv[1]);
	if (entry) {
		printf("%d\n", *entry);
	} else {
		fprintf(stderr, "Not found\n");
	}
}
