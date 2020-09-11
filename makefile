CFLAGS += -g

headers = proj1.h
objects = main.o parser.o expand_argument.o build_execution_list.o string_list.o \
	execute_list_node.o lookup_executable.o

shell: $(objects)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(objects) shell
