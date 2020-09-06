headers = proj1.h
objects = main.o parser.o expand_argument.o build_execution_list.o string_list.o

proj1: $(objects)
	$(CC) -o $@ $^

clean:
	rm -f *.o proj1
