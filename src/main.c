#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TEST_TYPE 1
#define BENCHMARK

#if TEST_TYPE == 0
#define HTABLE_KEY_EQ str_eq
#define HTABLE_KEY_TYPE char*
#define HTABLE_VALUE_TYPE int
#define HTABLE_HASH_FUNCTION hash_string
#define HTABLE_ELEM_DELETE str_bucket_delete
#include "../hashtable.h"
#endif

size_t hash(int *key)
{
	unsigned int x = *key;
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = (x >> 16) ^ x;
	return x;
}
size_t hash_string(char **str)
{
	char *s = *str;
	unsigned long hash = 5381;
	int c;

	while ((c = *s++)) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
}


void str_bucket_delete(char *key, int value)
{
	return;
}

void bucket_delete(int key, int value)
{
	return;
}

bool int_eq(int a, int b)
{
	return a == b;
}

bool str_eq(char *a, char *b)
{
	return strcmp(a, b) == 0;
}

#if TEST_TYPE == 1
#define HTABLE_KEY_EQ int_eq
#define HTABLE_KEY_TYPE int
#define HTABLE_VALUE_TYPE int
#define HTABLE_HASH_FUNCTION hash
#define HTABLE_ELEM_DELETE bucket_delete
#include "../hashtable.h"
#endif

#ifdef BENCHMARK
#if TEST_TYPE == 1

void print_elapsed(clock_t start, clock_t end, char *benchmark_name)
{
	printf("%s Finished: %f\n", benchmark_name, (double)(end - start) / CLOCKS_PER_SEC);
}

void insert_n(int32_t n)
{
	HTable htable = htable_new();

	KV *table = calloc(n, sizeof(KV));
	for (int32_t j = 0; j < 50; ++j) {
		for (int32_t i = 0; i < n; ++i) {
			table[i] = (KV) { i, i };
			int *v = htable_insert(&htable, &table[i]);

			if (v != NULL) printf("key %d, but got %d\n", i, *v);
		}
		size_t cleared = htable_clear(&htable);
		assert(cleared == n);
	}

	htable_delete(&htable);
	free(table);
}

void insert_1000000(void)
{
	clock_t start = clock();
	/* Insert 1_000_000 elements into table and clear 50 times */
	insert_n(1000000);
	clock_t end = clock();

	print_elapsed(start, end, (char*)__func__);
}

void insert_10000(void)
{
	clock_t start = clock();
	/* Insert 10_000 elements into table and clear 50 times */
	insert_n(10000);
	clock_t end = clock();

	print_elapsed(start, end, (char*)__func__);
}

void insert_1000(void)
{
	clock_t start = clock();
	/* Insert 1_000 elements into table and clear 50 times */
	insert_n(1000);
	clock_t end = clock();

	print_elapsed(start, end, (char*)__func__);
}

void insert_100(void)
{
	clock_t start = clock();
	/* Insert 100 elements into table and clear 50 times */
	insert_n(100);
	clock_t end = clock();

	print_elapsed(start, end, (char*)__func__);
}

#endif
#endif

int main(void)
{
#ifndef BENCHMARK
	HTable htable = htable_new();
#if TEST_TYPE == 0
	htable_insert(&htable, &(KV){
		(K){ "hello" },
		(V){ 1 },
	});
	htable_insert(&htable, &(KV){
		(K){ "world" },
		(V){ 2 },
	});

	printf("Key: %s => Value: %d\n", "hello", *htable_get(&htable, &(K){ "hello" }));
	printf("Key: %s => Value: %d\n", "world", *htable_get(&htable, &(K){ "world" }));
	printf("Key: %s => Value: %p\n", "wow", (void *)htable_get(&htable, &(K){ "wow" }));

	assert(htable_clear(&htable) == 2);
	assert(htable->len == 0);
#endif

#if TEST_TYPE == 1
	htable_insert(&htable, &(KV){
		(K){ 1 },
		(V){ 1 },
	});
	htable_insert(&htable, &(KV){
		(K){ 2 },
		(V){ 2 },
	});
	htable_insert(&htable, &(KV){
		(K){ 3 },
		(V){ 3 },
	});

	printf("Key: %d => Value: %d\n", 1, *htable_get(&htable, &(K){ 1 }));
	printf("Key: %d => Value: %d\n", 2, *htable_get(&htable, &(K){ 2 }));
	printf("Key: %d => Value: %d\n", 3, *htable_get(&htable, &(K){ 3 }));

	printf("Key: %d => Value: %p\n", 4, (void *)htable_get(&htable, &(K){ 4 }));

	printf("Key: %d => Value: %d\n", 3, *htable_remove(&htable, &(K){ 3 }));
	printf("Key: %d => Value: %p\n", 3, (void *)htable_get(&htable, &(K){ 3 }));
	printf("Key: %d => Value: %d\n", 2, *htable_remove(&htable, &(K){ 2 }));
	printf("Key: %d => Value: %p\n", 2, (void *)htable_get(&htable, &(K){ 2 }));
	printf("Key: %d => Value: %d\n", 1, *htable_remove(&htable, &(K){ 1 }));
	printf("Old value: %p\n", (void *)htable_insert(&htable, &(KV){
		(K){ 1 },
		(V){ 1 },
	}));
	printf("Old value: %d\n", htable_insert(&htable, &(KV){
		(K){ 1 },
		(V){ 3 },
	}));

	printf("Key: %d => Value: %p\n", 69, (void *)htable_get(&htable, &(K){ 69 }));

	htable_insert(&htable, &(KV){
		(K){ 5 },
		(V){ 5 },
	});

	htable_insert(&htable, &(KV){
		(K){ 6 },
		(V){ 6 },
	});

	htable_insert(&htable, &(KV){
		(K){ 7 },
		(V){ 7 },
	});

	htable_insert(&htable, &(KV){
		(K){ 8 },
		(V){ 8 },
	});

	htable_insert(&htable, &(KV){
		(K){ 9 },
		(V){ 9 },
	});

	htable_insert(&htable, &(KV){
		(K){ 11 },
		(V){ 11 },
	});
	htable_insert(&htable, &(KV){
		(K){ 12 },
		(V){ 12 },
	});
	htable_insert(&htable, &(KV){
		(K){ 13 },
		(V){ 13 },
	});
	htable_insert(&htable, &(KV){
		(K){ 14 },
		(V){ 14 },
	});
	htable_insert(&htable, &(KV){
		(K){ 15 },
		(V){ 15 },
	});
	htable_insert(&htable, &(KV){
		(K){ 16 },
		(V){ 16 },
	});
	htable_insert(&htable, &(KV){
		(K){ 17 },
		(V){ 17 },
	});
	htable_insert(&htable, &(KV){
		(K){ 18 },
		(V){ 18 },
	});
	htable_insert(&htable, &(KV){
		(K){ 19 },
		(V){ 19 },
	});
	htable_insert(&htable, &(KV){
		(K){ 20 },
		(V){ 20 },
	});
	htable_insert(&htable, &(KV){
		(K){ 21 },
		(V){ 21 },
	});
	htable_insert(&htable, &(KV){
		(K){ 22 },
		(V){ 22 },
	});
	htable_insert(&htable, &(KV){
		(K){ 23 },
		(V){ 23 },
	});
	htable_insert(&htable, &(KV){
		(K){ 24 },
		(V){ 24 },
	});
	htable_insert(&htable, &(KV){
		(K){ 25 },
		(V){ 25 },
	});
	htable_insert(&htable, &(KV){
		(K){ 26 },
		(V){ 26 },
	});
	htable_insert(&htable, &(KV){
		(K){ 27 },
		(V){ 27 },
	});
	htable_insert(&htable, &(KV){
		(K){ 28 },
		(V){ 28 },
	});
	htable_insert(&htable, &(KV){
		(K){ 29 },
		(V){ 29 },
	});
	htable_insert(&htable, &(KV){
		(K){ 30 },
		(V){ 30 },
	});
	htable_insert(&htable, &(KV){
		(K){ 31 },
		(V){ 31 },
	});
	htable_insert(&htable, &(KV){
		(K){ 32 },
		(V){ 32 },
	});
	htable_insert(&htable, &(KV){
		(K){ 33 },
		(V){ 33 },
	});
	htable_insert(&htable, &(KV){
		(K){ 34 },
		(V){ 34 },
	});
	htable_insert(&htable, &(KV){
		(K){ 34 },
		(V){ 34 },
	});
	htable_delete(&htable);
	printf("After delete: %p\n", (void *)htable);
#endif

#endif

	#ifdef BENCHMARK
	insert_100();
	insert_1000();
	insert_10000();
	insert_1000000();
	#endif
}
