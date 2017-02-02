/*
* @Author: krocki
* @Date:   2017-02-01 19:15:48
* @Last Modified by:   krocki
* @Last Modified time: 2017-02-01 21:07:47
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stddef.h>
#include <mpi.h>

double get_time(void) {

	struct timeval tv;
	gettimeofday(&tv, NULL);
	double t;
	t = tv.tv_sec + tv.tv_usec * 1e-6;
	return t;

}

void saxpy(int* result, int* x, int* y, size_t N) {

	size_t i;

	for (i = 0; i < N; i++) {

		result[i] = x[i] + y[i];

	}

}

void saxpy_mpi(int* result, int* x, int* y, size_t N) {

	size_t i;

	for (i = 0; i < N; i++) {

		result[i] = x[i] + y[i];

	}

}

int check_result(int* a, int* b, size_t N) {

	size_t i;

	for (i = 0; i < N; i++) {

		if (a[i] != b[i]) return 0;

	}

	return 1;

}

int main(int argc, char ** argv) {

	srand(get_time());

	size_t N = 100000000;
	int NUM_MAX = 100;

	int * x = (int *) malloc(sizeof(int) * N);
	int * y = (int *) malloc(sizeof(int) * N);
	int * result = (int *) malloc(sizeof(int) * N);
	int * result_mpi = (int *) malloc(sizeof(int) * N);

	size_t i;
	double tic, time_sequential, time_parallel;

	/* SET UP MPI */
	int rank;
	MPI_Init(NULL, NULL);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Status status;

	MPI_Barrier(MPI_COMM_WORLD);

	/* init data */
	if (rank == 0) {

		for (i = 0; i < N; i++)
			x[i] = rand() % NUM_MAX;

		for (i = 0; i < N; i++)
			y[i] = rand() % NUM_MAX;

	}

	if (rank == 0) {

		printf("Running sequential SAXPY on rank %d...\n", rank);
		tic = get_time();
		saxpy(result, x, y, N);
		time_sequential = get_time() - tic;

	}

	MPI_Barrier(MPI_COMM_WORLD);

	/* for scatter-gather */
	int * partial_x = (int *) malloc(sizeof(int) * N / world_size);
	int * partial_y = (int *) malloc(sizeof(int) * N / world_size);
	int * partial_result = (int *) malloc(sizeof(int) * N / world_size);

	if (rank == 0)
		tic = get_time();

	MPI_Scatter(x, N / world_size, MPI_INT, partial_x, N / world_size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(y, N / world_size, MPI_INT, partial_y, N / world_size, MPI_INT, 0, MPI_COMM_WORLD);

	double tic_computation = get_time();
	printf("Running parallel SAXPY on rank %d...\n", rank);
	saxpy_mpi(partial_result, partial_x, partial_y, N / world_size);

	double time_proc = get_time() - tic_computation;
	printf("time rank %d = %.9f s\n", rank, time_proc);

	MPI_Gather(partial_result, N / world_size, MPI_INT, result_mpi, N / world_size, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == 0) {

		time_parallel = get_time() - tic;

		printf("time sequential = %.9f s\n", time_sequential);
		printf("time MPI = %.9f s\n", time_parallel);
		printf("speedup = %.2f x\n", time_sequential / time_parallel);
		printf("check_result() = %d\n", check_result(result, result_mpi, N));

	}

	MPI_Finalize();

	free((void *) partial_x);
	free((void *) partial_y);
	free((void *) partial_result);

	free((void *) x);
	free((void *) y);
	free((void *) result);
	free((void *) result_mpi);

	return 0;
}
