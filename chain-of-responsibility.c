#include <glib.h>
#include <stdio.h>

typedef struct BaseHandler {
	struct BaseHandler* (*set_next)(struct BaseHandler*, struct BaseHandler*);
	char* (*handle)(struct BaseHandler*, char*);
	struct BaseHandler* next_handler;
} Handler;

typedef struct {
	Handler* handler;
} MonkeyHandler;

typedef struct {
	Handler* handler;
} SquirrelHandler;

typedef struct {
	Handler* handler;
} DogHandler;

Handler* set_next(Handler* self, Handler* next) {
	self->next_handler = next;
	return next;
}

char* monkey_handle(Handler* handler, char* request) {
	if (g_strcmp0(request, "Banana") == 0) {
		return "Monkey: I'll eat the Banana.\n";
	} else {
		if (handler->next_handler) {
			return handler->next_handler->handle(handler->next_handler, request);
		}
	}
	return NULL;
}

MonkeyHandler* monkey_handler_new() {
	MonkeyHandler* self = g_new(MonkeyHandler, 1);
	self->handler = g_new(Handler, 1);
	self->handler->set_next = set_next;
	self->handler->handle = monkey_handle;
	self->handler->next_handler = NULL;
	return self;
}

char* squirrel_handle(Handler* handler, char* request) {
	if (g_strcmp0(request, "Nut") == 0) {
		return "Squirrel: I'll eat the Nut.\n";
	} else {
		if (handler->next_handler) {
			return handler->next_handler->handle(handler->next_handler, request);
		}
	}
	return NULL;
}

SquirrelHandler* squirrel_handler_new() {
	SquirrelHandler* self = g_new(SquirrelHandler, 1);
	self->handler = g_new(Handler, 1);
	self->handler->set_next = set_next;
	self->handler->handle = squirrel_handle;
	self->handler->next_handler = NULL;
	return self;
}

char* dog_handle(Handler* handler, char* request) {
	if (g_strcmp0(request, "MeatBall") == 0) {
		return "Dog: I'll eat the MeatBall.\n";
	} else {
		if (handler->next_handler) {
			return handler->next_handler->handle(handler->next_handler, request);
		}
	}
	return NULL;
}

DogHandler* dog_handler_new() {
	DogHandler* self = g_new(DogHandler, 1);
	self->handler = g_new(Handler, 1);
	self->handler->set_next = set_next;
	self->handler->handle = dog_handle;
	self->handler->next_handler = NULL;
	return self;
}

void client_code(Handler* handler) {
	char* food[] = {"Nut", "Banana", "Cup of coffee"};

	for (int i = 0; i < 3; i++) {
		printf("Client: Who wants a %s\n", food[i]);

		char* result = handler->handle(handler, food[i]);

		if (result != NULL) {
			printf("%s\n", result);
		} else {
			printf("  %s was left untouched.\n", food[i]);
		}
	}
}

int main() {

	MonkeyHandler* monkey = monkey_handler_new();
	SquirrelHandler* squirrel = squirrel_handler_new();
	DogHandler* dog = dog_handler_new();

	monkey->handler->set_next(monkey->handler, squirrel->handler)
		->set_next(squirrel->handler, dog->handler);

	printf("Chain: Monkey > Squirrel > Dog\n\n");

	client_code(monkey->handler);

	printf("\n");
	printf("Subchain: Squirrel > Dog\n\n");
	
	client_code(squirrel->handler);

	g_free(monkey->handler);
	g_free(squirrel->handler);
	g_free(dog->handler);
	g_free(monkey);
	g_free(squirrel);
	g_free(dog);

	return 0;
}
