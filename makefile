headers = proj1.h
objects = main.o parser.o expand_argument.o build_execution_list.o string_list.o \
	execute_list_node.o

shell: $(objects)
	$(CC) -o $@ $^

clean:
	rm -f *.o shell
