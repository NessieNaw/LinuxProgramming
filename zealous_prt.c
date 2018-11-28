#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <signal.h>
#include <errno.h>
#include <memory.h>

struct Que
{
	siginfo_t sgnfo;
	struct Que* next;
};

struct Que* que = NULL;
struct Que* newKid = NULL;
struct Que* Kid = NULL;

void QEnque( siginfo_t sgnfo );
void QDeque();

static volatile int ilsc = 0;
static volatile int kids = 0;

pid_t grpa = 0;

static void sigHandler( int sig/*, siginfo_t* status, void* sth */)
{

	int w;	
	siginfo_t st;
	printf("handler!\n");
	memset( &st, 0, sizeof(st) );

	while( !( w = waitid( P_PGID, grpa, &st, WSTOPPED | WEXITED | WNOHANG | WCONTINUED )) )
	{
	printf("handler inside while \n");
	    if( w == -1 )
	    {
		     perror( "Something happened to handler waitid \n " );
		     exit( EXIT_FAILURE );
	   	}
		if( st.si_code == CLD_CONTINUED ) 
		{
			kids+=1;
			printf("\t\tkid continued \n");
		}
		else if ( st.si_code == CLD_STOPPED )
		{
			printf("\t\tkid stopped \n");
			kids-=1;
		}
		else if( st.si_pid == CLD_KILLED )
		{
			printf(" cld_killed \n");
			kids-=1;
			ilsc-=1;
			QEnque( st );
		}
		else
			 break;
	}
}	

int main( int argc, char* argv[] )
{
	double pi = M_PI;
	int opt;
	double ee = exp(1);
	char* wsk = NULL;
	while(( opt = getopt( argc, argv, "t:" )) != -1 )
    {
		switch( opt )
		{
            case 't': 
				ee = strtod( optarg, &wsk );
				if( *wsk )
				{
					perror( "Conversion strtod went wrong \n");
					exit( EXIT_FAILURE );
				}
			    break;
		    default: /* '?' */
			    fprintf(stderr, "Usage: %s [-t vale] <floats>\n",argv[0] );
				exit( EXIT_FAILURE );
		}
	}
	ilsc = strtod( argv[optind], &wsk );
	if( *wsk )
	{
		perror( "Conversion of kids amount (strtod) went wrong \n");
		exit( EXIT_FAILURE );
	}
	grpa = strtod( argv[optind+1], &wsk );
	if( *wsk )
	{
		perror( "Conversion 2 strtod went wrong \n");
		exit( EXIT_FAILURE );
	}
	;

	for( int i = 0; i <argc; i++ )
		printf("argv[%d] = %s\n", i, argv[i]);
	printf(" ilsoc: %d, grupa: %d optind %d \n", ilsc, grpa,optind );
	pid_t w;
	siginfo_t st;
	int d = 0;
	do
     {
		memset( &st, 0, sizeof(siginfo_t) );
		w = waitid( P_PGID, grpa, &st, WSTOPPED | WEXITED | WNOHANG );
	    if( w == -1 )
	    {
		     perror( "Something happened to waitid 1 \n " );
			 break;
        }
		//printf("&&& %d sisigno %d errno %d \n", w, st.si_signo, errno );
		//printf("signo %d, sistatus %d, si code %d\n", st.si_signo, st.si_status, st.si_code);
		if( st.si_pid != 0 )
		{
			printf( "Child: %d is asleep \n", st.si_pid );
			d++;
			//printf("signo %d, sistatus %d, si code %d\n", st.si_signo, st.si_status, st.si_code);
	
		}
	}  while( d < ilsc );

	
	struct sigaction sa;

	sigemptyset( &sa.sa_mask );
	sa.sa_flags = SA_SIGINFO;
	sa.sa_handler = sigHandler;
	if( sigaction( SIGCHLD, &sa, NULL ) == -1 )
	{
		perror( "sigaction failed \n");
		exit( EXIT_FAILURE );
	}
	
	int kll = kill( -grpa, SIGCONT );
	if( kll == -1 )
	{
		perror(" Kids did not wake up #1 \n");
		exit(EXIT_FAILURE);
	}
	printf("RISE AND SHINE #1! \n");
	siginfo_t si;
	sigset_t allSigs;
	do
	{
		printf("drugi while \n");
		pause(); //sigwaitinfo( &allSigs, &si );
		while( que )
		{
			printf(" NEKROLOGI! \n ");
			QDeque();
		}
		printf(" \t\t\t ile kids = %d, ilsc = %d \n", kids, ilsc );
		if(( kids == 0 ) && ( ilsc > 0 ))
		{/*
			double liczba2 = ( int )( ee *( 1/( pi*pi )));
			struct timespec req1 = {0};
			req1.tv_sec = ( int )( liczba2/100 );
			req1.tv_nsec = ( liczba2 - ( long )req1.tv_sec*100 )* 10000000;
		
			nanosleep( &req1, ( struct timespec* )NULL );
*/
			sleep(1);
			int kl = kill( -grpa, SIGCONT ); 
		 	if( kl == -1 )
			{
				perror(" Kids did not wake up #2 \n");
			  	exit( EXIT_FAILURE );
		 	}
			printf("RISE AND SHINE! \n");
		}

	} while( ilsc > 0 );

	return 0;
}
//======================================================

void QEnque( siginfo_t sgnfo )
{	
	newKid = ( struct Que* )malloc( sizeof( struct Que ));
	( *newKid ).sgnfo = sgnfo;
	if( que == NULL )
		Kid = newKid;
	( *que ).next = newKid;
	que = newKid;
}

//======================================================

void QDeque()
{
	if( Kid == NULL )
		perror( " There is no more kids \n" );
	newKid = ( *Kid ).next;
	printf( "Child: %d died, ( killer code: %d ) with status:%d \n", Kid->sgnfo.si_pid, Kid->sgnfo.si_code, Kid->sgnfo.si_status  );
	free( Kid );
	Kid = newKid;
	if( Kid == NULL )
		que = NULL;


}
