
.PHONY: all
all: hb_tree hn_tree

hb_tree:
	gcc -g src/hb_tree.c test/hb_test.c -o hb_test

hn_tree:
	gcc -g src/hn_* test/hn_test.c -o hn_test

clean:
	rm -rf hb_test hn_test lib obj