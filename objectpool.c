#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * TODO: use a list for Resources. If the list is empty, get_resource will
 * return a newly created Resource.  Otherwise it will pop one from the front
 * of the list for use. return_resource will reset the Resource and push it to
 * back of the list.
 */

typedef struct {
	int value;
	int (*get_value)(void*);
	void (*set_value)(void*, int);
} Resource;

typedef struct {
	int value;
	Resource* (*get_resource)(void);
	void* (*return_resource)(Resource*);
} ObjectPool;

int get_value(void* arg) {
	Resource* res = (Resource*)arg;
	return res->value;
}

void set_value(void* arg, int value) {
	Resource* res = (Resource*)arg;
	res->value = value;
}

Resource* get_resource() {
	Resource* resource = malloc(sizeof(Resource));
	memset(resource, 0, sizeof(Resource));
	resource->get_value = &get_value;
	resource->set_value = &set_value;
	return resource;
}

void* return_resource(Resource* resource) {
	free(resource);
	return NULL;
}

ObjectPool* get_instance() {
	static bool created = false;
	static ObjectPool* pool;

	if (created) return pool;

	pool = malloc(sizeof(ObjectPool));
	memset(pool, 0, sizeof(ObjectPool));

	pool->get_resource = &get_resource;
	pool->return_resource = &return_resource;

	created = true;

	return pool;
}

int main(void) {
	ObjectPool* pool;
	pool = get_instance();

	Resource* one;
	Resource* two;

	/* Resources will be created*/
	one = pool->get_resource();
	one->set_value(one, 10);
	printf("one = %d\n", one->get_value(one));
	two = pool->get_resource();
	two->set_value(two, 20);
	printf("two = %d\n", two->get_value(two));

	pool->return_resource(one);
	pool->return_resource(two);

	/* Resources will be reused.
	 * Notice that the value of both resources were reset back to zero.
	 */
	one = pool->get_resource();
	printf("one = %d\n", one->get_value(one));
	two = pool->get_resource();
	printf("two = %d\n", two->get_value(two));

	pool->return_resource(one);
	pool->return_resource(two);

	free(pool);

	return 0;
}
