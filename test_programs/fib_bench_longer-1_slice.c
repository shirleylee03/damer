#include<pthread.h>
#include<malloc.h>
void reach_error ( ) { 
} 

void abort ( ) { 
} 

extern void abort ( void ) ; 
void reach_error ( ) { 
} 

int i = 1 , j = 1 ; 
void * t1 ( void * arg ) { 
int k = 0 ; 
k = 0 ; 
while ( k < 6 ) { 
i = i + j ; 
k ++ ; 
} 

} 

void * t2 ( void * arg ) { 
int k = 0 ; 
k = 0 ; 
while ( k < 6 ) { 
j = j + i ; 
k ++ ; 
} 

} 

int main ( int argc , char * * argv ) { 
pthread_t id1 , id2 ; 
int condI ; 
int condJ ; 
pthread_create ( & id1 , 0 , t1 , 0 ) ; 
pthread_create ( & id2 , 0 , t2 , 0 ) ; 
condI = i > 377 ; 
condJ = j > 377 ; 
if ( condI || condJ ) { 
reach_error ( ) ; 
abort ( ) ; 
} 

} 

