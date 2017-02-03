/*
* @Author: krocki
* @Date:   2017-02-02 20:25:02
* @Last Modified by:   krocki
* @Last Modified time: 2017-02-02 20:45:04
*/

#include <stdio.h>
#include <mpi.h>
#include <stdio.h>

// this implements a barrier
// a message needs to go through through all nodes
// finished when node 0 receives the original message
// (the roundtrip is complete)

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
	int val;

	if (rank == 0) {

		val = rand();
		printf("Node 0: val = %d\n", val);

	}

	// rank of the next node in ring
	int next = (rank + 1) % world_size;
	int prev = (rank - 1 + world_size ) % world_size;

	//receive from previous
	if (rank != 0) {
		MPI_Recv(&val, 1, MPI_INT, prev, tag, MPI_COMM_WORLD, &status);
		printf("Node %d received %d from %d\n", rank, val, prev);
	}

	//send to next
	printf("Node %d is sending %d to %d\n", rank, val, next);
	MPI_Send(&val, 1, MPI_INT, next, tag, MPI_COMM_WORLD);

	if (rank == 0) {

		MPI_Recv(&val, 1, MPI_INT, prev, tag, MPI_COMM_WORLD, &status);
		printf("Node %d received %d from %d\n", rank, val, prev);

	}

	return 0;
}
