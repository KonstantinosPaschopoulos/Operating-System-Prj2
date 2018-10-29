OBJS 	= myfind.o
SOURCE	= myfind.c
# HEADER  =  OrderedLinkedList.h
OUT  	= myfind
CC	= gcc
FLAGS   = -g -c 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

myfind.o: myfind.c
	$(CC) $(FLAGS) myfind.c

clean:
	rm -f $(OBJS) $(OUT)

count:
	wc $(SOURCE) $(HEADER)
