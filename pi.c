/*
* @Author: Kamil Rocki
* @Date:   2017-02-03 12:42:21
* @Last Modified by:   Kamil Rocki
* @Last Modified time: 2017-02-03 13:15:58
*/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

double monte_carlo_pi ( size_t n_samples, long seed ) {

	size_t i = 0;
	double result = 0;
	srand ( seed );
	
	double x, y, z;
	
	for ( ; i < n_samples; i++ ) {
	
		// generate a random point in 2D space {[0,1], [0,1]}
		x = ( double ) random() / ( double ) RAND_MAX;
		y = ( double ) random() / ( double ) RAND_MAX;
		z = sqrt ( x * x + y * y );
		
		// check if that point is within a circle
		// true if sqrt(x^2 + y^2) <= 1
		if ( z <= 1.0 )
			result += 1.0;
			
	}
	
	result /= n_samples;
	
	return 4.0 * result;
}

double get_time ( void ) {

	struct timeval tv;
	gettimeofday ( &tv, NULL );
	double t;
	t = tv.tv_sec + tv.tv_usec * 1e-6;
	return t;
	
}

int main() {

	size_t N = 100000000;
	
	double serial_pi, mpi_pi;
	size_t i;
	double tic, time_sequential, time_parallel;
	
	/* SET UP MPI */
	int rank;
	MPI_Init ( NULL, NULL );
	
	int world_size;
	MPI_Comm_size ( MPI_COMM_WORLD, &world_size );
	
	MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
	
	MPI_Status status;
	
	MPI_Barrier ( MPI_COMM_WORLD );
	
	if ( rank == 0 ) {
	
		printf ( "Running sequential PI on rank %d...\n", rank );
		tic = get_time();
		serial_pi = monte_carlo_pi ( N, rank );
		time_sequential = get_time() - tic;
		
	}
	
	MPI_Barrier ( MPI_COMM_WORLD );
	
	double parallel_start_time = MPI_Wtime();
	
	double parallel_pi;
	
	printf ( "Running MPI PI on rank %d...\n", rank );
	parallel_pi = monte_carlo_pi ( N / world_size, rank );
	
	double parallel_acc_pi;
	
	MPI_Reduce ( &parallel_pi, &parallel_acc_pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );
	
	time_parallel = MPI_Wtime() - parallel_start_time;
	
	if ( rank == 0 ) {
	
		parallel_acc_pi /= ( double ) world_size;
		
		printf ( "\nSerial_pi = %f\n", serial_pi );
		printf ( "Parallel_pi = %f\n\n", parallel_acc_pi );
		
		printf ( "time sequential = %.9f s\n", time_sequential );
		printf ( "time MPI = %.9f s\n", time_parallel );
		printf ( "speedup = %.2f x\n", time_sequential / time_parallel );
		printf ( "diff = %f\n", fabs ( parallel_acc_pi - serial_pi ) );
		
	}

	MPI_Finalize();
	return 0;
}
