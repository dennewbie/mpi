/*
* @Author: krocki
* @Date:   2017-02-02 20:25:02
* @Last Modified by:   krocki
* @Last Modified time: 2017-02-02 21:04:49
*/

#include <stdio.h>
#include <mpi.h>
#include <stdio.h>

int main() {

	// Initialize the MPI environment
	MPI_Init(NULL, NULL);

	// Get the number of processes
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// Get the rank of the process
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Status status;
	int tag = 0x666;
	int val = 99;

	// rank of the other node
	int other = world_size-1;
	int root = 0;

	double start_time = MPI_Wtime();

	//receive
	if (rank == other && world_size > 1) {

		MPI_Recv(&val, 1, MPI_INT, root, tag, MPI_COMM_WORLD, &status);
		MPI_Send(&val, 1, MPI_INT, root, tag, MPI_COMM_WORLD);

	}

	if (rank == root) {

		MPI_Send(&val, 1, MPI_INT, other, tag, MPI_COMM_WORLD);
		MPI_Recv(&val, 1, MPI_INT, other, tag, MPI_COMM_WORLD, &status);

		double end_time = MPI_Wtime();
		printf("Roundtrip time between %d and %d = %f s\n", root, other, end_time - start_time);
	}
  MPI_Finalize();
	return 0;
}
