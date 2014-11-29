CC = g++
CFLAGS = -Wall

non_private: parser.o classes.o non_private.o
	$(CC) $(CFLAGS) -o non_private classes.o parser.o non_private.o

classes.o: classes.cpp classes.hpp
	$(CC) $(CFLAGS) -c classes.cpp

parser.o: parser.cpp classes.hpp
	$(CC) $(CFLAGS) -c parser.cpp
	
non_private.o: non_private.cpp classes.hpp
	$(CC) $(CFLAGS) -c non_private.cpp

clean:
	rm -f *.o *~ non_private