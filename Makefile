CC=gcc
CFLAGS=  -g -c -Wall

all:    myfind \
	root \
	splitterMerger \
	leaf

myfind:   myfind.c
	$(CC)  $(CFLAGS) myfind.c
	$(CC)  myfind.o -o myfind

root:   root.c
	$(CC)  $(CFLAGS) root.c
	$(CC)  root.o -lm -o root

splitterMerger:   splitterMerger.c
	$(CC)  $(CFLAGS) splitterMerger.c
	$(CC)  splitterMerger.o -o splitterMerger

leaf: 	leaf.c
	$(CC)  $(CFLAGS) leaf.c
	$(CC)  leaf.o -o leaf

clean:
	rm -f   \
		myfind.o myfind \
		root.o root \
		splitterMerger.o splitterMerger \
		leaf.o leaf
