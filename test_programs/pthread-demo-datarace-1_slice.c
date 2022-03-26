#include<pthread.h>
#include<malloc.h>
void reach_error ( ) { 
} 

void abort ( ) { 
} 

extern void abort ( void ) ; 
void reach_error ( ) { 
} 

void __VERIFIER_assert ( int cond ) { 
if ( ! ( cond ) ) { 
reach_error ( ) ; 
abort ( ) ; 
} 

} 

void exit ( int code ) { 
} 

int myglobal ; 
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER ; 
void * thread_function_mutex ( void * arg ) { 
int i , j ; 
i = 0 ; 
while ( i < 20 ) { 
pthread_mutex_lock ( & mymutex ) ; 
j = myglobal ; 
j = j + 1 ; 
printf ( "\nIn thread_function_mutex..\t" ) ; 
myglobal = j ; 
pthread_mutex_unlock ( & mymutex ) ; 
i ++ ; 
} 

} 

int main ( void ) { 
pthread_t mythread ; 
int i ; 
myglobal = 0 ; 
if ( pthread_create ( & mythread , 0 , thread_function_mutex , 0 ) ) { 
exit ( - 1 ) ; 
} 

i = 0 ; 
while ( i < 20 ) { 
pthread_mutex_lock ( & mymutex ) ; 
myglobal = myglobal + 1 ; 
pthread_mutex_unlock ( & mymutex ) ; 
i ++ ; 
} 

if ( pthread_join ( mythread , 0 ) ) { 
exit ( - 1 ) ; 
} 

__VERIFIER_assert ( myglobal == 40 ) ; 
exit ( 0 ) ; 
} 

