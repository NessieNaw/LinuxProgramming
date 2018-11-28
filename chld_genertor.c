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

double potomek_bez_c( float tab );
double rodzic_bez_p( pid_t p, float min );

int main( int argc, char* argv[] )
{
	int opt;
	char* path1 = NULL;
	char* path2 = NULL;
	int c = 0;
	int p = 0;
	float tab[argc];
	float tablica[argc];
	char* wsk;
	while(( opt = getopt( argc, argv, "p:c:" )) != -1 )
	{
		switch( opt )
		{
			case 'c':
				path1 = optarg;
				c = 1;
				break;
			case 'p':
				path2 = optarg;
				p = 1;
				break;
			default: /* '?' */
				fprintf(stderr, "Usage: %s [-c path] [-p path] <floats>\n",argv[0]);
				exit(EXIT_FAILURE);

		}
	}
	int il = argc;
	int j = 1;
	float min = FLT_MAX;

	if( ( c == 1 && p == 0 ) || ( c == 0  && p == 1 ) )
		j = 3;
	if( c == 1 && p  == 1 )
		j = 5;
	int y = 0;
	for( int i = j, y; i < il; i++, y++ )
	{
		tablica[i] += strtof( argv[i], &wsk );
		if( !wsk )
		{
			perror( "Conversion strtof failed #0.1 \n " );
			exit( EXIT_FAILURE );
		}
		if( tablica[i] < min )
			min = tablica[i];
		tab[y] = tablica[i];
	}
	if( optind >= argc ) 
	{
		fprintf(stderr, "Expected argument after options\n");
		exit(EXIT_FAILURE);
	}
	printf( "minimum: %g \n ", min );

	printf(" optint %d, argc = %d \n", optind, argc );

	pid_t pid;
	int grupa;
	pid_t pd;
	for( int k = 0; k < (argc-optind); k++ )
	{
		pid = fork();
		if( pid == -1 )
		{
			perror( "Fork did not succeed somehow \n" );
			k-=1;
			exit(EXIT_FAILURE);
		}

		if( pid > 0 )
        {
	        if( k == 0 )
		        grupa = getpid();
		    setpgid( pid, grupa );
		    printf( "Child: %d was born. Its group nr: %d \n", pid, getpgid( pid ));
			pd = pid;
		}

		if( pid == 0 )
		{
			pd = getpid();
			if( c == 0 )
				potomek_bez_c( tab[k] );
			else
			{
				char bm1 [100];
				snprintf( bm1,100, "%g", tab[k]);

				printf( " path1: %s \n", path1 );
				execl( path1, path1,bm1, NULL );
				perror( "Exec did not succeed \n" );
				exit( EXIT_FAILURE );
			}
		}
	}

	printf( " ilosc %d \n", argc-optind);

	if( p == 0 )
	{
			rodzic_bez_p( grupa, min );
	}
	else
	{
	
	printf("sfdfedsc\n");	
		char bm [100];
		snprintf( bm,100, "-t %g", min);
	    char bm1[100];
		int a = argc-optind;
		printf("%d\n",a);
		snprintf(bm1, 100, "%d", a);
		char bm2[100];
		snprintf( bm2, 100, "%d", grupa );
		printf(" %s\n %s\n %s \n", bm, bm1, bm2 );
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
    //printf( "Random numner: %d \n", los );
    exit( los );
																				}
//==========================================
double rodzic_bez_p( pid_t p, float min  )
{

	pid_t w;
    siginfo_t st;
	
	while(1)
	{
		w = waitid( P_PGID, p, &st, WEXITED | WNOHANG );
		if( w == -1 )
		{
			perror( "Something happened to waitid \n " );
			exit(EXIT_FAILURE);
		}
		if( st.si_pid != 0 )
			printf( "Child: %d died[*], killer code: %d, with status:%d \n", st.si_pid, st.si_code, st.si_status );
	
  	    double liczba2 = ( int )min/2;
		struct timespec req1 = {0};	
		req1.tv_sec = ( int )( liczba2/100 );
		req1.tv_nsec = ( liczba2 - ( long )req1.tv_sec*100 )* 10000000;
		
		nanosleep( &req1, ( struct timespec* )NULL );


	}
	exit( EXIT_SUCCESS );

}


