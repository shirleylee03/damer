#include<pthread.h>
#include<malloc.h>
void reach_error ( ) { 
} 

void abort ( ) { 
} 

pthread_mutex_t atomic_lock = PTHREAD_MUTEX_INITIALIZER ; 
extern void abort ( void ) ; 
void assume_abort_if_not ( int cond ) { 
if ( ! cond ) { 
abort ( ) ; 
} 

} 

extern void abort ( void ) ; 
void reach_error ( ) { 
} 

int w = 0 , r = 0 , x , y ; 
void __VERIFIER_atomic_take_write_lock ( ) { 
pthread_mutex_lock ( & atomic_lock ) ; 
assume_abort_if_not ( w == 0 && r == 0 ) ; 
w = 1 ; 
pthread_mutex_unlock ( & atomic_lock ) ; 
} 

void __VERIFIER_atomic_take_read_lock ( ) { 
pthread_mutex_lock ( & atomic_lock ) ; 
assume_abort_if_not ( w == 0 ) ; 
r = r + 1 ; 
pthread_mutex_unlock ( & atomic_lock ) ; 
} 

void __VERIFIER_atomic_release_read_lock ( ) { 
pthread_mutex_lock ( & atomic_lock ) ; 
r = r - 1 ; 
pthread_mutex_unlock ( & atomic_lock ) ; 
} 

void * writer ( void * arg ) { 
__VERIFIER_atomic_take_write_lock ( ) ; 
x = 3 ; 
w = 0 ; 
} 

void * reader ( void * arg ) { 
int l ; 
__VERIFIER_atomic_take_read_lock ( ) ; 
l = x ; 
y = l ; 
if ( ! ( y == x ) ) reach_error ( ) ; 

} 

int main ( ) { 
pthread_t t1 , t2 , t3 , t4 ; 
pthread_create ( & t1 , 0 , writer , 0 ) ; 
pthread_create ( & t2 , 0 , reader , 0 ) ; 
pthread_create ( & t3 , 0 , writer , 0 ) ; 
pthread_create ( & t4 , 0 , reader , 0 ) ; 
pthread_join ( t1 , 0 ) ; 
pthread_join ( t2 , 0 ) ; 
pthread_join ( t3 , 0 ) ; 
pthread_join ( t4 , 0 ) ; 
} 
