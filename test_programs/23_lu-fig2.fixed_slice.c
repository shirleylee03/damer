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

int mThread = 0 ; 
int start_main = 0 ; 
int mStartLock = 0 ; 
int __COUNT__ = 0 ; 
void __VERIFIER_atomic_acquire ( ) { 
pthread_mutex_lock ( & atomic_lock ) ; 
assume_abort_if_not ( mStartLock == 0 ) ; 
mStartLock = 1 ; 
pthread_mutex_unlock ( & atomic_lock ) ; 
} 

void __VERIFIER_atomic_release ( ) { 
pthread_mutex_lock ( & atomic_lock ) ; 
assume_abort_if_not ( mStartLock == 1 ) ; 
mStartLock = 0 ; 
pthread_mutex_unlock ( & atomic_lock ) ; 
} 

void __VERIFIER_atomic_thr1 ( int PR_CreateThread__RES ) { 
pthread_mutex_lock ( & atomic_lock ) ; 
if ( __COUNT__ == 0 ) { 
} 
else { 
{ 
if ( ! ( 0 ) ) { 
reach_error ( ) ; 
abort ( ) ; 
} 

} 
; 
} 

pthread_mutex_unlock ( & atomic_lock ) ; 
} 

void * thr1 ( void * arg ) { 
int PR_CreateThread__RES = 1 ; 
__VERIFIER_atomic_acquire ( ) ; 
start_main = 1 ; 
__VERIFIER_atomic_thr1 ( PR_CreateThread__RES ) ; 
__VERIFIER_atomic_release ( ) ; 
} 

void __VERIFIER_atomic_thr2 ( int self ) { 
pthread_mutex_lock ( & atomic_lock ) ; 
if ( __COUNT__ == 1 ) { 
} 
else { 
{ 
if ( ! ( 0 ) ) { 
reach_error ( ) ; 
abort ( ) ; 
} 

} 
; 
} 

pthread_mutex_unlock ( & atomic_lock ) ; 
} 

void * thr2 ( void * arg ) { 
int self ; 
self = mThread ; 
while ( start_main == 0 ) ; 

__VERIFIER_atomic_acquire ( ) ; 
__VERIFIER_atomic_release ( ) ; 
__VERIFIER_atomic_thr2 ( self ) ; 
} 

int main ( ) { 
pthread_t t ; 
pthread_create ( & t , 0 , thr1 , 0 ) ; 
thr2 ( 0 ) ; 
} 

