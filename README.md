# HB-Tree
version 0.0.1
> This is still a work in progress. I'm working on completing this implementaion and on improving it.

## Introduction
This is my implementation of a silly algorithm that I came up with (although I'm not sure if I'm the first to come up with it) to satisfy my criteria of what a balanced binary tree should look like.

When I read about different implementations of key-value data structures, I wasn't satisfied with how each algorithm worked. whether it was a type of hashtable that needed to be resized (which requires copying old data to a newly allocated hash table and rehashing) or binary trees that needed to be balanced recursively in some cases.

## How to use
On Linux, you need GNU Make and gcc
clone the repo then install it with: 
```bash
git clone https://github.com/Hussein-L-AlMadhachi/hb-tree
cd hb-tree
make
```

If you're using windows just compile `src/hb_tree.c` and `test/hb_test.c` with any C compiler


## How It Works
### Inserting New Nodes
1. Hash the key of your key-value pair and call this hash `H`.
2. Iterate through this hash `H` bit by bit (let's call it `B`) until you reach a NULL pointer.
3. In each iteration, if `B` is One, take the right pointer; otherwise, if `B` is Zero, take the left pointer (you can reverse right and left here; it doesn't matter). Also, if the key matches a key in an existing node, stop the loop and return an error.
4. When the NULL pointer is inevitably reached, insert your new node on the right or the left according to the value of `B`, similar to step 3.

### Fetching Nodes
1. Hash the key of your key-value pair and call this hash `H`.
2. Iterate through this hash `H` bit by bit (let's call it `B`) until you reach a NULL pointer.
3. In each iteration, if `B` is One, take the right pointer; otherwise, if `B` is Zero, take the left pointer (you can reverse right and left here; it doesn't matter). Also, if the key matches a key in an existing node, stop the search and return the node.
4. When a NULL pointer is reached, return an error because the searched node was never found.

### Removing Nodes
Currently, I'm still trying to figure out the best strategy for removing nodes while keeping the tree relatively balanced.

## Notes
* The balance of this binary tree depends on the hash function used; the better the distribution property, the better it works.
* According to my benchmarks, it takes 2.100 seconds to insert and fetch 10 million nodes in a single loop on an Intel Core i5 Gen 11 with 8 CPU cores and 20GB of memory.
