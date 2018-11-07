all: 	myfind \
	root \
	splitterMerger \
	leaf

myfind: myfind.c
	gcc -g myfind.c -o ../myfind

root: root.c
	gcc -g root.c -o root

splitterMerger: splitterMerger.c
	gcc -g splitterMerger.c -o splitterMerger

leaf: leaf.c
	gcc -g leaf.c -o leaf

clean:
	rm -f 	\
		../myfind \
		root \
		splitterMerger \
		leaf
