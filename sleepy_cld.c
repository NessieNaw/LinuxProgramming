#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

int pills();

int main( int argc, char* argv[] )
{
	if( argc != 2 )
	{
		perror( "Usage: <param> (float) \n" );
		exit( -1 );
	}
	char* wsk;
	float flt = strtof( argv[1], &wsk);
	if( *wsk )
	{
		perror( "Conversion string to float did not succeed \n" );
		exit( -1 );
    }
	raise( SIGSTOP ); 
	siginfo_t st;
	int w =0;
	while( 1 )
	{
		waitid( P_ALL, 0, &st, WCONTINUED );
		if( w == -1)
		{
			perror("waitid \n" );
			exit(EXIT_FAILURE);
		}
		double liczba = ( int )flt;
  	    struct timespec req = {0};
		req.tv_sec = ( int )( liczba/100 );
		req.tv_nsec = ( liczba - ( long )req.tv_sec*100 )* 10000000;
		nanosleep( &req, (struct timespec*) NULL );
		raise( pills());
	}

	return 0;
}

int pills()
{	
	int los;
	unsigned int seed; 
	FILE* urandom = fopen( "/dev/urandom", "r" ); 
	fread( &seed, sizeof( int ), 1, urandom );
    fclose( urandom );
		
	srand( seed );
	los = 1 + (int)( 2.0*( rand() / ( RAND_MAX + 1.0 )));

	if( los == 2 )
		return SIGTERM;
	else if( los == 1 )
		return SIGSTOP;
	return SIGTSTP;

}


