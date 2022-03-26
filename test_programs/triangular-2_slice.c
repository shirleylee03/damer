#include<pthread.h>
#include<malloc.h>
void reach_error ( ) { 
} 

void abort ( ) { 
} 

extern void abort ( void ) ; 
void reach_error ( ) { 
} 

int i = 3 , j = 6 ; 
void * t1 ( void * arg ) { 
int k ; 
k = 0 ; 
while ( k < 5 ) { 
i = j + 1 ; 
k ++ ; 
} 

} 

void * t2 ( void * arg ) { 
int k ; 
k = 0 ; 
while ( k < 5 ) { 
j = i + 1 ; 
k ++ ; 
} 

} 

int main ( int argc , char * * argv ) { 
pthread_t id1 , id2 ; 
int condI ; 
int condJ ; 
pthread_create ( & id1 , 0 , t1 , 0 ) ; 
pthread_create ( & id2 , 0 , t2 , 0 ) ; 
condI = i >= ( 2 * 5 + 6 ) ; 
condJ = j >= ( 2 * 5 + 6 ) ; 
if ( condI || condJ ) { 
reach_error ( ) ; 
abort ( ) ; 
} 

} 

