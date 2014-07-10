#include <iostream>
#include "GameOfLife.h"

/*********************************** WARNING *******************************/
/* In case this include does not compiles try with the one commented below */
/***************************************************************************/
//#include <mpi.h>
#include "mpi.h"

using namespace std;

/*
 * Usage:  ./program rows columns aliveProbability numTimeSteps
 */
int main(int argc, char *argv[]) {
	int rows, columns, numTimeSteps, *numLiveCells, step,
			totalsum[DEFAULT_BUFFER_SIZE];
	float aliveProbability;
	int chunkSize = 10;

	MPI_Init(&argc, &argv);
	rows = atoi(argv[1]);
	columns = atoi(argv[2]);
	aliveProbability = atof(argv[3]);
	numTimeSteps = atoi(argv[4]);
	GOLInit(rows, columns, aliveProbability);
	numLiveCells = new int[chunkSize + 1];

	/* Loop over the time steps in chunks of chunkSize */
	for (step = 0; step < numTimeSteps; step += chunkSize) {
		int numSteps = chunkSize;
		int k;
		if (step + numSteps > numTimeSteps)
			numSteps = numTimeSteps - step;
		/* numSteps is the actual number of steps in this chunk. */
		//MPI_Barrier(MPI_COMM_WORLD);
		GOLSimulate(numSteps, numLiveCells);

		printf("Process %5d: ", me);
		for (k = 0; k <= numSteps; ++k) {
			printf("%5d ", numLiveCells[k]);
		}
		printf("\n");

		for (k = 0; k <= numSteps; ++k)
			MPI_Reduce((numLiveCells + k), (totalsum + k), 1, MPI_INT,
			MPI_SUM, 0,
			MPI_COMM_WORLD);
		//new communication to get the sum of all alive nodes
		if (me == 0) {
			//if I am the root process then I have to print the sum stored in totalsum
			printf("TOTAL:         ");
			for (k = 0; k <= numSteps; ++k) {
				printf("%5d ", totalsum[k]);
			}
			printf("\n\n");
		}

	}
	//deleting resources
	delete[] numLiveCells;

	//finalizes the execution
	GOLFinalize();
	MPI_Finalize();

	return 0;
}

