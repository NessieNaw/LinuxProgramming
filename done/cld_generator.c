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
#include <errno.h>

double potomek_bez_c( float tab );
double rodzic_bez_p( pid_t p, float min );

int main( int argc, char* argv[] )
{
	int opt;
	char* path1 = NULL;
	char* path2 = NULL;
	char* wsk;
	while(( opt = getopt( argc, argv, "p:c:" )) != -1 )
	{
		switch( opt )
		{
			case 'c':
				path1 = optarg;
				break;
			case 'p':
				path2 = optarg;
				break;
			default: /* '?' */
				fprintf( stderr, "Usage: %s [-c path] [-p path] <floats>\n",argv[0] );
				exit( EXIT_FAILURE );

		}
	}
	int i = optind;
	float min = FLT_MAX;
	pid_t pid;
	pid_t grupa = 0;
	int ilsc = 0;
	do
	{
		wsk = NULL;
		float flt = strtof( argv[i], &wsk );
		if( *wsk != '\0' )
			break;
		ilsc+=1;
		if( flt < min )
			min = flt;

		pid = fork();
		if( pid == -1 )
		{
			perror( "Fork did not succeed somehow \n" );
			exit( EXIT_FAILURE );
		}

		if( pid > 0 )
        {
	        if( grupa == 0 )
		        grupa = getpid();
		    int set = setpgid( pid, grupa );
			if( set == -1 )
			{
				perror( " Could not set the group \n" );
				exit( EXIT_FAILURE );
			}
		    printf( "Child: %d was born. Its group nr: %d \n", pid, getpgid( pid ));
		}

		if( pid == 0 )
		{
			if( path1 == NULL )
				potomek_bez_c( flt );
			else
			{
				execl( path1, path1, argv[i], NULL );
				perror( "Exec did not succeed \n" );
				exit( EXIT_FAILURE );
			}
		}
		i+=1;
	} while( argv[i] != NULL );

	if( path2 == NULL )
		rodzic_bez_p( grupa, min );
	else
	{
		char bm [100];
		snprintf( bm,100, "-t %g", min );
	    char bm1[100];
		snprintf(bm1, 100, "%d", ilsc );
		char bm2[100];
		snprintf( bm2, 100, "%d", grupa );
		
		execl( path2, path2, bm, bm1, bm2, NULL );
	
		perror( "Exec2 did not succeed \n" );
	}
	return 0;

}
//===========================================
double potomek_bez_c( float tab )
{
	double liczba = ( int )tab;
	struct timespec req = {0};
	req.tv_sec = ( int )( liczba/100 );
	req.tv_nsec = ( liczba - ( long )req.tv_sec*100 )* 10000000;


	unsigned int seed;
    FILE* urandom = fopen( "/dev/urandom", "r" );
	fread( &seed, sizeof( int ), 1, urandom );
	fclose( urandom );
	
	nanosleep( &req, (struct timespec*)NULL);
    srand( seed );
	int los = 1 + (int)( 128.0*( rand() / ( RAND_MAX + 1.0 )));
    exit( los );
																				}
//==========================================
double rodzic_bez_p( pid_t p, float min  )
{
	pid_t w;
    siginfo_t st;
	
	double liczba2 = ( int )min/2;
	struct timespec req1 = {0};	
	req1.tv_sec = ( int )( liczba2/100 );
	req1.tv_nsec = ( liczba2 - ( long )req1.tv_sec*100 )* 10000000;
	
	while(1)
	{
		w = waitid( P_PGID, p, &st, WEXITED | WNOHANG );
		if( w == -1 )
		{
			if( errno == 10 )
			{
				perror( "No more kids to wait for ... \n " );
				exit( EXIT_SUCCESS );
			}
			perror( "Something happened to waitid \n " );
			exit( EXIT_FAILURE );
		}
		if( st.si_pid != 0 )
			printf( "Child: %d died,( killer code: %d) with status:%d \n", st.si_pid, st.si_code, st.si_status );
  
		nanosleep( &req1, ( struct timespec* )NULL );


	}
	exit( EXIT_SUCCESS );

}


