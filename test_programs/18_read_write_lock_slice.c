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
void assert ( int cond ) { 
if ( ! cond ) { 
abort ( ) ; 
} 

} 

void reach_error ( ) { 
assert ( 0 ) ; 
} 

extern void __VERIFIER_atomic_begin ( void ) ; 
extern void __VERIFIER_atomic_end ( void ) ; 
int w = 0 , r = 0 , x , y ; 
void __VERIFIER_atomic_w ( ) { 
pthread_mutex_lock ( & atomic_lock ) ; 
assume_abort_if_not ( w == 0 ) ; 
assume_abort_if_not ( r == 0 ) ; 
w = 1 ; 
pthread_mutex_unlock ( & atomic_lock ) ; 
} 

void * thr1 ( void * arg ) { 
__VERIFIER_atomic_w ( ) ; 
x = 3 ; 
w = 0 ; 
} 

void __VERIFIER_atomic_r ( ) { 
pthread_mutex_lock ( & atomic_lock ) ; 
assume_abort_if_not ( w == 0 ) ; 
r = r + 1 ; 
pthread_mutex_unlock ( & atomic_lock ) ; 
} 

void * thr2 ( void * arg ) { 
int l ; 
int ly ; 
int lx ; 
int lr ; 
__VERIFIER_atomic_r ( ) ; 
l = x ; 
y = l ; 
ly = y ; 
lx = x ; 
if ( ! ( ly == lx ) ) { 
reach_error ( ) ; 
abort ( ) ; 
} 

lr = r ; 
r = lr - 1 ; 
} 

int main ( ) { 
pthread_t t ; 
pthread_create ( & t , 0 , thr1 , 0 ) ; 
thr2 ( 0 ) ; 
} 
