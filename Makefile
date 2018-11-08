all: 	myfind \
	root \
	splitterMerger \
	leaf

myfind: myfind.c
	gcc -g -Wall myfind.c -o ../myfind

root: root.c
	gcc -g -Wall root.c -o root

splitterMerger: splitterMerger.c
	gcc -g -Wall splitterMerger.c -o splitterMerger

leaf: leaf.c
	gcc -g -Wall leaf.c -o leaf

clean:
	rm -f 	\
		../myfind \
		root \
		splitterMerger \
		leaf
