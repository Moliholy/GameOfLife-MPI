#include "GameOfLife.h"

cellStatus* currentStatus = 0;
int* neighbors = 0;
int me = -1;
int np = 0;
int nColumns = 0;
int nRows = 0;

inline cellStatus getCellStatus(int position) {
	return position >= 0 && position < nColumns ? currentStatus[position] : DEAD;
}

void refreshStatus() {
	for (int i = 0; i < nRows * nColumns; ++i) {
		if (currentStatus[i] == ALIVE && (neighbors[i] < 2 || neighbors[i] > 3))
			currentStatus[i] = DEAD;
		else if (currentStatus[i] == DEAD && neighbors[i] == 3)
			currentStatus[i] = ALIVE;
	}
}

int countAlives() {
	int count = 0;
	for (int i = 0; i < nRows * nColumns; ++i) {
		if (currentStatus[i] == ALIVE)
			count++;
	}
	return count;
}

void GOLInit(int rows, int columns, float aliveProbability) {
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	MPI_Comm_rank(MPI_COMM_WORLD, &me);

	nRows = rows;
	nColumns = columns / np;

	if (columns % np != 0) {
		if (nColumns * np < columns) {
			nColumns++;
		} else {
			nColumns--;
		}
	}

	currentStatus = new cellStatus[nRows * nColumns];
	neighbors = new int[nRows * nColumns];

	//setting the seed for this thread, which will be different from the others
	srand(me);

	//generation of the first population
	for (int i = 0; i < rows * nColumns; ++i) {
		currentStatus[i] =
				((float) rand()) / (float) RAND_MAX < aliveProbability ?
						ALIVE : DEAD;
	}
}

/**
 * Executes an iteration
 */
void GOLSimulate(int numTimeSteps, int* numLiveCells) {
	int rbuf_left[DEFAULT_BUFFER_SIZE];
	int rbuf_right[DEFAULT_BUFFER_SIZE];
	MPI_Request rreq_left, rreq_right, sreq_left, sreq_right;
	int* pointer_left = (int*) currentStatus; //first position
	int* pointer_right = (int*) (currentStatus + nRows * nColumns - 1 - nRows); //last column
	int size = nRows * sizeof(int); //size of buffer in bytes
	numLiveCells[0] = countAlives(); //initial count

	for (int k = 0; k < numTimeSteps; k++) {
		if (me > 0) {
			//ask the process me-1 for its results if I am not the first one
			MPI_Irecv(rbuf_left, size, MPI_INT, me - 1, 0, MPI_COMM_WORLD,
					&rreq_left);
			MPI_Isend(pointer_left, size, MPI_INT, me - 1, 0, MPI_COMM_WORLD,
					&sreq_left);
		}
		if (me < np - 1) {
			//ask the process me+1 for its results if I am not the last one
			MPI_Irecv(rbuf_right, size, MPI_INT, me + 1, 0, MPI_COMM_WORLD,
					&rreq_right);
			MPI_Isend(pointer_right, size, MPI_INT, me + 1, 0, MPI_COMM_WORLD,
					&sreq_right);
		}
		//start checking every avaliable cell for this process. Meanwhile we are waiting
		//for the messages to be received.
		for (int i = 0; i < nRows * nColumns; ++i) {
			neighbors[i] = 0; //initialization

			neighbors[i] += getCellStatus(i - nRows - 1); //UP LEFT
			neighbors[i] += getCellStatus(i - nRows); //LEFT
			neighbors[i] += getCellStatus(i - nRows + 1); //DOWN LEFT

			neighbors[i] += getCellStatus(i - 1); //UP
			neighbors[i] += getCellStatus(i + 1); //DOWN

			neighbors[i] += getCellStatus(i + nRows - 1); //RIGHT LEFT
			neighbors[i] += getCellStatus(i + nRows); //RIGHT
			neighbors[i] += getCellStatus(i + nRows + 1); //RIGHT LEFT
		}

		if (me > 0) {
			//firstly wait -> left
			MPI_Wait(&rreq_left, MPI_STATUS_IGNORE);
			for (int i = 0; i < nRows; ++i) {
				if (i - 1 >= 0)
					neighbors[i] += rbuf_left[i - 1];

				neighbors[i] += rbuf_left[i];

				if (i + 1 < nRows)
					neighbors[i] += rbuf_left[i + 1];
			}
		}

		if (me < np - 1) {
			//secondly wait -> right
			MPI_Wait(&rreq_right, MPI_STATUS_IGNORE);
			int it = 0;
			for (int i = nRows * nColumns - 1 - nRows; i < nRows * nColumns;
					++i) {
				if (it - 1 >= 0)
					neighbors[i] += rbuf_right[it - 1];

				neighbors[i] += rbuf_right[it];

				if (it + 1 < nRows)
					neighbors[i] += rbuf_right[it + 1];
				it++;
			}
		}

		//refreshing the status
		refreshStatus();

		//counting the number of ALIVES we have now on the grid
		numLiveCells[k + 1] = countAlives();

		/* IMPORTANT: we have to wait for the other processes in order to receive
		 * refreshed data
		 */
		MPI_Barrier(MPI_COMM_WORLD);
	}
}

/**
 * Reset all parameters
 */
void GOLFinalize(void) {
	delete[] currentStatus;
	delete[] neighbors;
	currentStatus = 0;
	neighbors = 0;
	me = -1;
	np = 0;
	nColumns = 0;
	nRows = 0;
}
