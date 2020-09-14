CFLAGS += -g -std=gnu99
COMMIT_LOG := commit_log.txt

headers = proj1.h
objects = main.o parser.o expand_argument.o build_execution_list.o string_list.o \
	execute_list_node.o lookup_executable.o execute_internal_command.o \
	internal_commands_a.o internal_commands_b.o

objects := $(patsubst %.o,src/%.o,$(objects))

all: shell $(COMMIT_LOG)

shell: $(objects)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(objects) shell $(COMMIT_LOG)

$(COMMIT_LOG):
	git --no-pager log > $@
