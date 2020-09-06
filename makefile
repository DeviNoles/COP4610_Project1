headers = proj1.h
objects = main.o parser.o

proj1: $(objects)
	$(CC) -o $@ $^

clean:
	rm -f *.o proj1
