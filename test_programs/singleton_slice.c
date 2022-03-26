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
v = malloc ( sizeof ( char ) ) ; 
} 

void * thread2 ( void * arg ) { 
} 

void * thread3 ( void * arg ) { 
} 

void * thread0 ( void * arg ) { 
pthread_t t1 , t2 , t3 , t4 , t5 ; 
pthread_create ( & t1 , 0 , thread1 , 0 ) ; 
pthread_join ( t1 , 0 ) ; 
pthread_create ( & t2 , 0 , thread2 , 0 ) ; 
pthread_create ( & t3 , 0 , thread3 , 0 ) ; 
pthread_create ( & t4 , 0 , thread2 , 0 ) ; 
pthread_create ( & t5 , 0 , thread2 , 0 ) ; 
pthread_join ( t2 , 0 ) ; 
pthread_join ( t3 , 0 ) ; 
pthread_join ( t4 , 0 ) ; 
pthread_join ( t5 , 0 ) ; 
} 

int main ( void ) { 
pthread_t t ; 
pthread_create ( & t , 0 , thread0 , 0 ) ; 
pthread_join ( t , 0 ) ; 
__VERIFIER_assert ( v [ 0 ] == 'X' ) ; 
} 

