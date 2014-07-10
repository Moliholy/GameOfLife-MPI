#ifndef GAMEOFLIFE_H_
#define GAMEOFLIFE_H_

#include "cstdlib"
#include <iostream>

/*********************************** WARNING *******************************/
/* In case this include does not compiles try with the one commented below */
/***************************************************************************/
//#include <mpi.h>
#include "mpi.h"

#define DEFAULT_BUFFER_SIZE 10000

//defining all possible states
typedef enum {
	DEAD = 0, ALIVE = 1
} cellStatus;

extern int me;

void GOLInit(int rows, int columns, float aliveProbability);

void GOLSimulate(int numTimeSteps, int* numLiveCells);

void GOLFinalize(void);

#endif /* GAMEOFLIFE_H_ */
