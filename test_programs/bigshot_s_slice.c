#include<pthread.h>
#include<malloc.h>
void reach_error ( ) { 
} 

void abort ( ) { 
} 

extern void abort ( void ) ; 
void reach_error ( ) { 
} 

void __VERIFIER_assert ( int expression ) { 
if ( ! expression ) { 
reach_error ( ) ; 
abort ( ) ; 
} 

} 

char * v ; 
void * thread1 ( void * arg ) { 
v = malloc ( sizeof ( char ) * 8 ) ; 
} 

void * thread2 ( void * arg ) { 
if ( v ) strcpy ( v , "Bigshot" ) ; 

} 

int main ( ) { 
pthread_t t1 , t2 ; 
pthread_create ( & t1 , 0 , thread1 , 0 ) ; 
pthread_join ( t1 , 0 ) ; 
pthread_create ( & t2 , 0 , thread2 , 0 ) ; 
pthread_join ( t2 , 0 ) ; 
__VERIFIER_assert ( ! v || v [ 0 ] == 'B' ) ; 
} 
