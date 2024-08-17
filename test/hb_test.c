#include <stdio.h>
#include <stdint.h>
#include <string.h>


#define ITERATIONS 10000000

#include "../include/hb_tree.h"

char * int_2_crazy_str( unsigned number , size_t * len )
{
	unsigned bit_shift=0;
	char * string = malloc(100);

	while( number > 0 )
	{
		if( (number&1) == 1 )
		{
			string[ bit_shift ] = '1';
		} else {
			string[ bit_shift ] = '0';
		}

		number>>=1;
		bit_shift++;
	}

	*len = bit_shift;
	string[ bit_shift ] = '\0';

	return string;
}


int main ()
{
	printf( "sizeof HN_Node: %lu\n" , sizeof( HB_Node ) );

	HB_Tree tree;
	tree.root_node = NULL;

	char * item;
	char n[100];

	size_t str_len = 0;
	
	uint64_t completed_tests = 0;
	for ( uint64_t i=0; i< ITERATIONS ; i++ )
	{
		//sprintf( n , "%lu", i);
		int_2_crazy_str( i , &str_len );
		hb_create_item( &tree , n , str_len , n );

		item = hb_get_item( &tree , n , strlen(n) );

		if( item != n )
		{
			printf( "E: test failed\n" );
			return -1;
		}
		completed_tests++;
	}

	printf( " [*] %lu tests completed successfully  \n" , completed_tests );


	return 0;
}
