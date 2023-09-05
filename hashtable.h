#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define BUCKET_INIT 16
#define HTABLE_DELETED 0x01
#define DEFAULT_LFACTOR (LFactor) { 2, 3 }

#define K HTABLE_KEY_TYPE
#define V HTABLE_VALUE_TYPE

#ifndef HTABLE_KEY_TYPE
#error "Please define a key type. For example int"
#endif

#ifndef HTABLE_VALUE_TYPE
#error "Please define a value type. For example int"
#endif

#ifndef HTABLE_HASH_FUNCTION
#error "Please define a hash function for type."
#endif

#ifndef HTABLE_KEY_EQ
#error "Please define a key comparison function."
#endif

#ifndef HTABLE_ELEM_DELETE
#error "Please define a destructor function for key-value pair."
#endif

typedef struct {
	size_t numerator;
	size_t denominator;
} LFactor;

typedef struct {
	K key;
	V value;
} KV;


typedef struct HashTable {
	KV **bucket;
	size_t len;
	size_t len_internal;
	size_t size;

	LFactor lfactor;
} *HTable;


HTable htable_new(void);
V *htable_insert(HTable *h, KV *kv);
V *htable_remove(HTable *htable, K *key);
V *htable_get(HTable *h, K *key);
void htable_delete(HTable *h);
size_t htable_clear(HTable *h);

HTable htable_new(void)
{
	return NULL;
}

bool htable_is_elem(KV *elem)
{
	return elem != NULL && elem != (KV*)HTABLE_DELETED;
}

HTable htable_alloc(HTable *h)
{
	if (*h == NULL) {
		HTable new_table = (HTable)calloc(1, sizeof(struct HashTable));

		new_table->bucket = (KV**)calloc(BUCKET_INIT, sizeof(*new_table->bucket));
		new_table->size = BUCKET_INIT;
		new_table->lfactor = DEFAULT_LFACTOR;
		*h = new_table;
	}

	return *h;
}

bool htable_regrowable(size_t len, size_t size, LFactor lfactor)
{
	return len * lfactor.denominator > lfactor.numerator * size;
}

void htable_resize(HTable *h, size_t size)
{
	HTable htable = htable_alloc(h);

	KV **new_bucket = (KV**)calloc(size, sizeof(KV**));
	for (size_t i = 0; i < htable->size; ++i) {
		// recompute hash for each elem
		KV *elem = htable->bucket[i];
		if (htable_is_elem(elem)) {
			size_t idx = (HTABLE_HASH_FUNCTION)(&htable->bucket[i]->key) % size;
			while (htable_is_elem(new_bucket[idx])) {
				idx = (idx + 1) % htable->size;
			}
			new_bucket[idx] = elem;
		}
	}
	htable->size = size;

	free(htable->bucket);
	htable->bucket = new_bucket;
}

V *htable_insert(HTable *h, KV *kv)
{
	HTable htable = htable_alloc(h);

	if (htable_regrowable(htable->len_internal, htable->size, htable->lfactor)) {
		htable_resize(h, 2*htable->size);
	}

	size_t idx = (HTABLE_HASH_FUNCTION)(&kv->key) % htable->size;
	size_t cycles = 0;
	// Probe for empty space in bucket.
	while (htable_is_elem(htable->bucket[idx])) {
		KV *old_kv = htable->bucket[idx];
		if (HTABLE_KEY_EQ(old_kv->key, kv->key)) {
			V *old_value = &old_kv->value;
			htable->bucket[idx] = kv;

			return old_value;
		}
		if (cycles++ > htable->size) {
			htable_resize(h, 2*htable->size);
		}

		idx = (idx + 1) % htable->size;
	}

	htable->bucket[idx] = kv;
	++htable->len;
	++htable->len_internal;
	return NULL;
}

V *htable_get(HTable *h, K *key)
{
	if (*h == NULL) {
		return NULL;
	}

	HTable htable = *h;
	size_t idx = (HTABLE_HASH_FUNCTION)(key) % htable->size;
	while (htable_is_elem(htable->bucket[idx])) {
		KV *kv = htable->bucket[idx];
		if (HTABLE_KEY_EQ(kv->key, *key)) {
			return &kv->value;
		}
	}

	return NULL;
}

V *htable_remove(HTable *h, K *key)
{
	if (*h == NULL) {
		return NULL;
	}

	HTable htable = *h;
	size_t idx = (HTABLE_HASH_FUNCTION)(key) % htable->size;

	while (htable_is_elem(htable->bucket[idx])) {
		KV *kv = htable->bucket[idx];
		if (HTABLE_KEY_EQ(kv->key, *key)) {
			htable->bucket[idx] = (KV*)HTABLE_DELETED;
			--htable->len;
			return &kv->value;
		}
	}

	return NULL;
}

void htable_delete(HTable *h)
{
	if (*h == NULL) {
		return;
	}

	HTable htable = *h;
	for (size_t i = 0; i < htable->size; ++i) {
		KV *kv = htable->bucket[i];
		if (htable_is_elem(kv)) {
			HTABLE_ELEM_DELETE(kv->key, kv->value);
		}
	}
	free(htable->bucket);
	free(*h);
	*h = NULL;
	return;
}

size_t htable_clear(HTable *h)
{
	if (*h == NULL) {
		return 0;
	}

	HTable htable = *h;
	size_t n = 0;

	for (size_t i = 0; i < htable->size; ++i) {
		KV *kv = htable->bucket[i];
		if (htable_is_elem(kv)) {
			++n;
			HTABLE_ELEM_DELETE(kv->key, kv->value);
		}
		htable->bucket[i] = NULL;
	}

	htable->len = 0;
	htable->len_internal = 0;

	return n;
}

#undef BUCKET_INIT
#undef HTABLE_DELETED
#undef DEFAULT_LFACTOR
#undef HTABLE_KEY_EQ
#undef HTABLE_KEY_TYPE
#undef HTABLE_VALUE_TYPE
#undef HTABLE_HASH_FUNCTION
#undef HTABLE_ELEM_DELETE

#endif
