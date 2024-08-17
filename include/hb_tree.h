#ifndef _HB_TREE_H
#define _HB_TREE_H


#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>



typedef enum
{
	SUCCESS = 0,
	ERR_OS = 2,	//OS's fault
	ERR_BAD_USE = 3,		//User fault
	ERR_UNEXPECTED = 4,		//Very Very unexpected error that shopuldn't be there

	F_ATOMIC = 8,
	F_NONATOMIC = 9,
}
Status;




typedef struct
{
	uint64_t hash;
	void * children[2];
	void * parent_ptr;

	char * value;
	char * key;
	size_t keylen;
	
	#ifdef HB_Alloc
	uint32_t raw_page_offset;
	#endif
}
HB_Node;




#ifdef HB_Alloc
typedef struct
{
    void * writing_raw_page;
    void * write_ptr;
    void * free_page;
	
    size_t nodes_left;
}
HB_Allocator;
#endif



typedef struct 
{
	HB_Node * root_node;
	HB_Node * leaves[2];
	HB_Node * next_free_node;
	size_t n_free_nodes;
}
HB_Tree;

void hb_init_tree ( HB_Tree * tree );
Status hb_set_item ( HB_Tree * tree , char * key , size_t keylen , char * value );
int hb_create_item ( HB_Tree * tree , char * key , size_t keylen , char * value );
char * hb_get_item ( HB_Tree * tree , char * key , size_t keylen );


#endif