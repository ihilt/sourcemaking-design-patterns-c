#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

typedef struct {
	int value;
	int (*get_value)(void*);
	void (*set_value)(void*, int);
	void (*reset)(void*);
} Resource;

typedef struct {
	Resource* (*get_resource)(GPtrArray*);
	void (*return_resource)(Resource*, GPtrArray*);
	GPtrArray* resources;
} ObjectPool;

static int get_value(void* arg) {
	Resource* res = (Resource*)arg;
	return res->value;
}

static void set_value(void* arg, int value) {
	Resource* resource = (Resource*)arg;
	resource->value = value;
}

static void reset(void* arg) {
	Resource* resource = (Resource*)arg;
	resource->set_value(resource, 0);
}

Resource* get_resource(GPtrArray* resources) {
	printf("# of resources: %d\n", resources->len);
	if (resources->len == 0) {
		printf("Create resource\n");
		Resource* resource = g_new(Resource, 1);
		resource->get_value = get_value;
		resource->set_value = set_value;
		resource->reset     = reset;
		return resource;
	} else {
		printf("Reuse resource\n");
		return g_ptr_array_steal_index(resources, 0);
	}
}

static void return_resource(Resource* resource, GPtrArray* resources) {
	resource->reset(resource);
	g_ptr_array_add(resources, resource);
}

static void resources_free_func(void* arg) {
	Resource* resource = (Resource*)arg;
	g_free(resource);
}

ObjectPool* get_instance() {
	static bool created = false;
	static ObjectPool* pool;

	if (created) return pool;

	pool = g_new(ObjectPool, 1);
	pool->get_resource = get_resource;
	pool->return_resource = return_resource;
	pool->resources = g_ptr_array_new_with_free_func(resources_free_func);

	created = true;

	return pool;
}

int main(void) {
	ObjectPool* pool = get_instance();
	Resource* one = pool->get_resource(pool->resources);
	Resource* two = pool->get_resource(pool->resources);

	/* Resources will be created*/
	one->set_value(one, 10);
	printf("one = %d\n", one->get_value(one));
	
	two->set_value(two, 20);
	printf("two = %d\n", two->get_value(two));

	pool->return_resource(one, pool->resources);
	pool->return_resource(two, pool->resources);

	/* Resources will be reused. Notice that the value of both resources
	 * were reset back to zero.
	 */
	one = pool->get_resource(pool->resources);
	printf("one = %d\n", one->get_value(one));

	two = pool->get_resource(pool->resources);
	printf("two = %d\n", two->get_value(two));

	pool->return_resource(one, pool->resources);
	pool->return_resource(two, pool->resources);

	return 0;
}
