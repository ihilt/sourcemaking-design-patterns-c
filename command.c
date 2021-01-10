#include <glib.h>
#include <stdio.h>

typedef struct Command {
	struct Command* base;
	void (*execute)(void*);
} Command;

typedef struct {
	Command* base;
	char* payload;
} SimpleCommand;

typedef struct Invoker {
	Command* on_start;
	Command* on_finish;
	void (*set_on_start)(struct Invoker*, Command*);
	void (*set_on_finish)(struct Invoker*, Command*);
	void (*do_something_important)(struct Invoker*);
} Invoker;

typedef struct {
	void (*do_something)(char*);
	void (*do_something_else)(char*);
} Receiver;

typedef struct {
	Command* base;
	char* a;
	char* b;
	Receiver* receiver;
} ComplexCommand;

void do_something(char* a) {
	printf("Receiver: Working on (%s.)\n", a);
}

void do_something_else(char* b) {
	printf("Receiver: also working on (%s.)\n", b);
}

void set_on_start(Invoker* self, Command* command) {
	self->on_start = command;
}

void set_on_finish(Invoker* self, Command* command) {
	self->on_finish = command;
}

void do_something_important(Invoker* self) {
	printf("Invoker: Does anybody want something done before I begin?\n");
	if (self->on_start != NULL) {
		self->on_start->base->execute(self->on_start);
	}
	printf("Invoker: ...doing something really important...\n");
	printf("Invoker: Does anybody want something done after I finish?\n");
	if (self->on_finish) {
		self->on_finish->base->execute(self->on_finish);
	}
}

Invoker* invoker_new() {
	Invoker* self = g_new(Invoker, 1);
	self->set_on_start = set_on_start;
	self->set_on_finish = set_on_finish;
	self->do_something_important = do_something_important;
	return self;
}

Receiver* receiver_new() {
	Receiver* self = g_new(Receiver, 1);
	self->do_something = do_something;
	self->do_something_else = do_something_else;
	return self;
}

void simple_command(void* memory) {
	SimpleCommand* command = (SimpleCommand*)memory;
	printf("SimpleCommand: See, I can do simple things like printing (%s)\n", command->payload);
	g_free(command->payload);
}
SimpleCommand* simple_command_new(char* a) {
	SimpleCommand* self = g_new(SimpleCommand, 1);
	self->base = g_new(Command, 1);
	self->base->execute = simple_command;
	self->payload = g_strdup(a);
	return self;
}

void complex_command(void* memory) {
	ComplexCommand* command = (ComplexCommand*)memory;
	printf("ComplexCommand: Complex stuff should be done by a receiver object.\n");
	command->receiver->do_something(command->a);
	command->receiver->do_something_else(command->b);
	g_free(command->a);
	g_free(command->b);
}
ComplexCommand* complex_command_new(Receiver* receiver, char* a, char* b) {
	ComplexCommand* self = g_new(ComplexCommand, 1);
	self->base = g_new(Command, 1);
	self->base->execute = complex_command;
	self->receiver = receiver;
	self->a = g_strdup(a);
	self->b = g_strdup(b);
	return self;
}

int main(void) {
	Invoker* invoker = invoker_new();
	invoker->set_on_start(invoker, (Command*)simple_command_new((char*)"Say Hi!"));
	Receiver* receiver = receiver_new();
	invoker->set_on_finish(invoker,
			(Command*)complex_command_new(receiver, "Send email", "Save report"));
	invoker->do_something_important(invoker);

	g_free(invoker->on_start->base);
	g_free(invoker->on_start);
	g_free(invoker->on_finish->base);
	g_free(invoker->on_finish);
	g_free(invoker);
	g_free(receiver);

	return 0;
}
