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
void * thread_function_datarace ( void * arg ) { 
int i , j ; 
i = 0 ; 
while ( i < 20 ) { 
j = myglobal ; 
j = j + 1 ; 
printf ( "\nIn thread_function_datarace..\t" ) ; 
myglobal = j ; 
i ++ ; 
} 

} 

int main ( void ) { 
pthread_t mythread ; 
int i ; 
if ( pthread_create ( & mythread , 0 , thread_function_datarace , 0 ) ) { 
exit ( - 1 ) ; 
} 

i = 0 ; 
while ( i < 20 ) { 
myglobal = myglobal + 1 ; 
i ++ ; 
} 

if ( pthread_join ( mythread , 0 ) ) { 
exit ( - 1 ) ; 
} 

__VERIFIER_assert ( myglobal != 40 ) ; 
printf ( "\nValue of myglobal in thread_function_datarace is :  %d\n" , myglobal ) ; 
exit ( 0 ) ; 
} 

