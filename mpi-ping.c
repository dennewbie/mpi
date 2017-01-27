/*
* @Author: kmrocki
* @Date:   2017-01-26 10:34:31
* @Last Modified by:   kmrocki
* @Last Modified time: 2017-01-26 21:08:37
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main() {

	// Initialize the MPI environment
	MPI_Init(NULL, NULL);

	// Get the number of processes
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// Get the rank of the process
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Get the name of the processor
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	MPI_Get_processor_name(processor_name, &name_len);

	// Print off a hello world message
	printf("Running on %s, rank %d out of %d processors\n",
	       processor_name, rank, world_size);

	MPI_Status status;

	MPI_Barrier(MPI_COMM_WORLD);

	int bytes = 1 << 30;
	double tv[2], t;
	int from = 1;
	int to = 0;
	int tag = 0x666;
	unsigned char *receive_buf;
	unsigned char *send_buf;

	receive_buf = (unsigned char *) malloc(bytes);
	send_buf = (unsigned char *) malloc(bytes);

	/***********************************/
	tv[0] = MPI_Wtime();

	if (rank == from)
		MPI_Send(send_buf, bytes, MPI_BYTE, to, tag, MPI_COMM_WORLD);
	if (rank == to)
		MPI_Recv(receive_buf, bytes, MPI_BYTE, from, tag, MPI_COMM_WORLD, &status);

	MPI_Barrier(MPI_COMM_WORLD);
	tv[1] = MPI_Wtime();
	/***********************************/
	t = (tv[1] - tv[0]) * 1000000.0;

	printf("%3d pinged %3d: %8d bytes %9.2f uSec %8.2f MB/s\n", from, to, bytes, t, bytes / (t));

	// Finalize the MPI environment.
	MPI_Finalize();

	free(receive_buf);
	free(send_buf);

	return 0;
}
