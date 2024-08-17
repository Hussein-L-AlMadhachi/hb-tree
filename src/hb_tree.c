#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <string.h>
#include <sys/mman.h>

#include "../include/hb_tree.h"
// #include "../include/hb_alloc.h"
#include "../include/rapidhash.h"


typedef int bool;


/**
 *	CONVETION:
 *	"LL" means linked list
 *
**/



void hb_init_tree ( HB_Tree * tree )
{
	tree->root_node = (void *) NULL;
	tree->n_free_nodes = 0;
	//hb_init_alloc( &tree->allocator );
}



Status hb_set_item ( HB_Tree * tree , char * key , size_t keylen , char * value )
{
	uint64_t hash = rapidhash( key , keylen );
	uint64_t bit_selector = 1;

	if ( tree->root_node == (void *) 1 )
	{
		return ERR_BAD_USE;
	}

	HB_Node * current_node = tree->root_node;
	bool is_one;


	while( current_node != NULL )
	{		
		if ( current_node->hash == hash &&
			current_node->keylen == keylen &&
			memcmp(key , current_node->key,keylen) == 0 )
		{
			current_node->value = value;
			return SUCCESS;
		}

		is_one = (hash & bit_selector) != 0;
		bit_selector <<= 1;
		current_node = current_node->children[ is_one ];
	}

	return ERR_BAD_USE | F_ATOMIC;
}



/**
 *	returns 0 on success when new key is created
 *	returns 1 on error when the key already exist
 *	returns -1 on error when it encounters memory allocation failure
 */
// doesn't change items that already exist
int hb_create_item ( HB_Tree * tree , char * key , size_t keylen , char * value )
{
	uint64_t hash = rapidhash( key , keylen );
	uint64_t bit_selector = 1;

	if ( tree->root_node == (void *) NULL )
	{
		tree->root_node = malloc( sizeof(HB_Node) );
		tree->root_node->children[0] = NULL;
		tree->root_node->children[1] = NULL;
		tree->root_node->key = key;
		tree->root_node->keylen = keylen;
		tree->root_node->hash = hash;
		tree->root_node->value = value;

		return SUCCESS;
	}

	HB_Node * current_node = tree->root_node;
	HB_Node * current_parent = NULL;
	HB_Node * current_grand_parent = NULL;

	int is_on_right_half = hash & bit_selector;
	bool is_one = is_on_right_half;

	while( current_node != NULL )
	{
		if ( current_node->keylen == keylen &&
			current_node->hash == hash &&
			memcmp(key , current_node->key,keylen) == 0 )
		{
			return ERR_BAD_USE;
		}

		is_one = (hash & bit_selector) != 0;
		bit_selector <<= 1;

		current_grand_parent = current_parent;
		current_parent = current_node;
		current_node = current_node->children[ is_one ];
	}

	/** WARNING: current_node now is equal to NULL so can't be used **/

	current_parent->children[ is_one ] = (void *) malloc( sizeof(HB_Node) );
	if (current_parent->children[is_one] == NULL)	return -1;

	current_parent->parent_ptr = current_grand_parent;

	tree->leaves[is_on_right_half] = current_parent->children[ is_one ];

	( (HB_Node*)  current_parent->children[is_one] )->key = key;
	( (HB_Node*)  current_parent->children[is_one] )->keylen = keylen;
	( (HB_Node*)  current_parent->children[is_one] )->hash = hash;
	( (HB_Node*)  current_parent->children[is_one] )->value = value;

	return SUCCESS;	
}



/**
 *	returns NULL when no found
 *  returns the expected value as (char*) on success
 */
char * hb_get_item ( HB_Tree * tree , char * key , size_t keylen )
{
	uint64_t hash = rapidhash( key , keylen );
	uint64_t bit_selector = 1;

	if ( tree->root_node == (void *) NULL )
	{
		return NULL;
	}

	HB_Node * current_node = tree->root_node;

	int is_on_right_half = hash & bit_selector;
	bool is_one = is_on_right_half;

	while( current_node != NULL )
	{
		if ( current_node->keylen == keylen &&
			current_node->hash == hash &&
			memcmp(key , current_node->key,keylen) == 0 )
		{
			return current_node->value;
		}

		is_one = (hash & bit_selector) != 0;
		bit_selector <<= 1;

		current_node = current_node->children[ is_one ];
	}

	return NULL;
}



int bh_remove_item ( HB_Tree * tree , char * key , size_t keylen )
{
	uint64_t hash = rapidhash( key , keylen );
	uint64_t bit_selector = 1;

	if ( tree->root_node == (void *) NULL )
	{
		return ERR_BAD_USE | F_ATOMIC;
	}

	HB_Node * current_node = tree->root_node;
	HB_Node * current_parent = NULL;
	HB_Node * current_grand_parent = NULL;


	bool is_one;

	while( current_node != NULL )
	{
		if ( current_node->keylen == keylen &&
			current_node->hash == hash &&
			memcmp(key , current_node->key,keylen) == 0 )
		{
			// is the deleted node a leaf node?
			if ( (current_node->children[0]||current_node->children[1]) == 0 )
			{
				/*
					if yes, append the deleted node to the begining of the free nodes
					linked list directly
				*/
				HB_Node * last_free_node = tree->next_free_node;
				tree->next_free_node = current_node;
				current_node->children[0] = last_free_node;
				
				current_parent->children[ is_one ] = NULL;

				return SUCCESS;
			} else
			{
				/*
					if no, then find the leaf child of the deleted key to replace it with 
					the deleted node.
					
					the algorithm used to pick the leaf child node of the deleted node is the
					regular lookup algorithm to maintain better tree balance when deleting nodes
				*/
				HB_Node * leaf_child = current_node;
				HB_Node * leaf_child_granny;

				while ( leaf_child->children[0] != NULL )
				{
					is_one = (hash & bit_selector) != 0;
					bit_selector <<= 1;
					
					leaf_child_granny = leaf_child;

					// making sure to exit only if we reach a node with no children
					if( leaf_child->children[ is_one ] == NULL )
					{
						if ( leaf_child->children[ !is_one ] == NULL )	break;
						
						leaf_child = leaf_child->children[ !is_one ];
					}

					leaf_child = leaf_child->children[ is_one ];
				}

				// cutoff the leaf node from the tree to be deleted later
				leaf_child_granny->children[ is_one ] = NULL;

				// copy the content of the leaf node to the deleted node			
				current_node->hash = leaf_child->hash;
				current_node->key = leaf_child->key;
				current_node->keylen = leaf_child->keylen;
				current_node->value = leaf_child->value;

				// append the node that is free to the begining of the free nodes linked list
				HB_Node * last_free_node = tree->next_free_node;
				tree->next_free_node = leaf_child;
				leaf_child->children[0] = last_free_node;

				// now the node is marked as deleted
			}
		}

		is_one = (hash & bit_selector) != 0;
		bit_selector <<= 1;

		current_grand_parent = current_parent;
		current_parent = current_node;
		current_node = current_node->children[ is_one ];
	}

	return ERR_BAD_USE;
}




/**
 *	returns NULL on error when not found
 *  returns the expected Node as (HB_Node *) on success
 */
HB_Node * hb_fetch_node ( HB_Tree * tree , char * key , size_t keylen )
{
	uint64_t hash = rapidhash( key , keylen );
	uint64_t bit_selector = 0b111111;

	if ( tree->root_node == (void *) NULL )
	{
		return NULL;
	}

	HB_Node * current_node = tree->root_node;

	int is_on_right_half = hash & bit_selector;
	bool is_one = is_on_right_half;

	while( current_node != NULL )
	{
		if ( current_node->keylen == keylen &&
			current_node->hash == hash &&
			memcmp(key , current_node->key,keylen) == 0 )
		{
			return current_node;
		}

		is_one = (hash & bit_selector) != 0;
		bit_selector <<= 1;

		current_node = current_node->children[ is_one ];
	}

	/** WARNING: current_node now is equal to NULL so can't be used **/

	return NULL;
}
