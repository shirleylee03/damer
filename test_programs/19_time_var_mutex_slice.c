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

void __VERIFIER_atomic_acquire ( int * m ) { 
pthread_mutex_lock ( & atomic_lock ) ; 
assume_abort_if_not ( * m == 0 ) ; 
* m = 1 ; 
pthread_mutex_unlock ( & atomic_lock ) ; 
} 

void __VERIFIER_atomic_release ( int * m ) { 
pthread_mutex_lock ( & atomic_lock ) ; 
assume_abort_if_not ( * m == 1 ) ; 
* m = 0 ; 
pthread_mutex_unlock ( & atomic_lock ) ; 
} 

int block ; 
int busy ; 
int m_busy = 0 ; 
void * thr1 ( void * arg ) { 
__VERIFIER_atomic_acquire ( & m_busy ) ; 
busy = 1 ; 
__VERIFIER_atomic_release ( & m_busy ) ; 
block = 1 ; 
if ( ! ( block == 1 ) ) { 
reach_error ( ) ; 
abort ( ) ; 
} 

} 

void * thr2 ( void * arg ) { 
__VERIFIER_atomic_acquire ( & m_busy ) ; 
if ( busy == 0 ) { 
block = 0 ; 
if ( ! ( block == 0 ) ) { 
reach_error ( ) ; 
abort ( ) ; 
} 

} 

__VERIFIER_atomic_release ( & m_busy ) ; 
} 

int main ( ) { 
pthread_t t ; 
pthread_create ( & t , 0 , thr1 , 0 ) ; 
thr2 ( 0 ) ; 
} 

