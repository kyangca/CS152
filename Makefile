CC = g++
CFLAGS = -Wall

private: private.o classes.o parser.o
	$(CC) $(CFLAGS) -o private private.o classes.o parser.o

non_private: parser.o classes.o non_private.o non_private_main.o
	$(CC) $(CFLAGS) -o non_private classes.o parser.o non_private.o non_private_main.o

classes.o: classes.cpp classes.hpp
	$(CC) $(CFLAGS) -c classes.cpp

parser.o: parser.cpp classes.hpp
	$(CC) $(CFLAGS) -c parser.cpp

private.o: private.cpp classes.hpp
	$(CC) $(CFLAGS) -c private.cpp

non_private.o: non_private.cpp classes.hpp
	$(CC) $(CFLAGS) -c non_private.cpp

non_private_main.o: non_private_main.cpp classes.hpp parser.hpp non_private.hpp
	$(CC) $(CFLAGS) -c non_private_main.cpp

analysis: parser.o classes.o non_private.o analysis.o
	$(CC) $(CFLAGS) -o analyze classes.o parser.o non_private.o analysis.o

analysis.o: analysis.cpp classes.hpp parser.hpp non_private.hpp
	$(CC) $(CFLAGS) -c analysis.cpp

clean:
	rm -f *.o *~ non_private analyze
