OBJS 	= myfind.o tree_interface.o
SOURCE	= myfind.c tree_interface.c
HEADER  = tree_interface.h mytypes.h
OUT  	= ../myfind
CC	= gcc
FLAGS   = -g -c

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

myfind.o: myfind.c
	$(CC) $(FLAGS) myfind.c

tree_interface.o: tree_interface.c
	$(CC) $(FLAGS) tree_interface.c

clean:
	rm -f $(OBJS) $(OUT)

count:
	wc $(SOURCE) $(HEADER)
