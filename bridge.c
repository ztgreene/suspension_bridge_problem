// The Nordvik Suspension Bridge Problem
// Author: Zac Greene 
// Date: 2021-08

//To run make and run the file:
// navigate to the directory with the code and, at the terminal:
// "make clean" then "make" then "make run"
// alternatively, "./bridge" will run the program

// This program demonstrates a variation of the 'readers and writers
// synchronisation problem. 
// The road to Nordvik carries traffic (cars and trucks) in both irections.
// However, at a one-lane suspension bridge, the road is only wide enough
// to carry traffic in one direction at a time. Once cars from one direction
// are on the bridge, no vehicles traveling in the other direction can enter.
// Further, ue to their weight, when trucks are crossing, they must be the
// only vehicle on the bridge at that time.
// This program creates threads to represent each vehicle aiming to cross
// the bridge, and uses semaphores to synchronise their crossing.
//
// Note: Due to an earlier engineering oversight, an infinite number of cars 
// was (theoretically) allowed to cross the bridge simultaneously (as long as
// they all did so from the same direction). I have realised that cars do,
// in fact, have a not-insignificant mass, and so have implemented a variable
// which can be used to limit the number of them using the bridge at one time.

#include "headers.h"
// constants/parameters
// Can be adjusted to experiment
#define MAXWAIT 20			//Maximum arrival delay (seconds)
#define CROSSINGTIME 4	//Time taken to cross the bridge (seconds)

#define MAXCARS 4   		//number of cars allowed on bridge concurrently
#define EASTCARS 7			//set number of cars travelling East
#define WESTCARS 7			// and West
#define EASTTRUCKS 3		//set number of trucks travelling East
#define WESTTRUCKS 3		// and West

// Global variables
// These are used to 'lock' variables during thread operations
// used to lock the  direction counters
int eastCount;
int westCount;
//to track cars in each direction
int eastCars;
int westCars;
//limits number of cars concurrently on bridge
int eastLimiter;
int westLimiter;
//locks the bridge
int lock;

//deletes all the semaphores
void deleteAllSemaphores(void){
	semDelete(lock);
	semDelete(eastLimiter);
	semDelete(westLimiter);
	semDelete(eastCount);
	semDelete(westCount);
	exit(1);
}

//Vehicle functions
//Trucks immediately 'lock' entry to the bridge up entering themselves
void *eastTruck(void * arg){
	int *selfID = (int *) arg; 
	//random delay to make it spicier
	sleep(rand() % MAXWAIT);

 //cross when possible, and lock behind
	down(lock);

	printf(""YELLOW"---> TRUCK %d going east on the bridge\n"END"", *selfID);
	sleep(CROSSINGTIME);
	printf(""YELLOW"TRUCK %d going east off the bridge--->\n"END"", *selfID);

  //unlock
	up(lock);
	pthread_exit(NULL);
}

void *westTruck(void * arg){
	int *selfID = (int *) arg; 
  //delay for randomness
	sleep(rand() % MAXWAIT);

  //enter bridge, lock it behind you
	down(lock);
	printf(""BLUE"TRUCK %d going west on the bridge <---\n"END"", *selfID);
	sleep( CROSSINGTIME );
	printf(""BLUE"<--- TRUCK %d going west off the bridge\n"END"", *selfID);;
	
  //unlock
	up(lock);
	pthread_exit(NULL);
}

//cars can follow other cars heading in the same direction
//there can be any amount of cars up to the MAXCARS limit
void *eastCar(void * arg){
	int *selfID = (int *) arg;

	//delay for interest's sake
	sleep(rand() % MAXWAIT );
	
	//Cross bridge if enough space
	down(eastLimiter);
	//disable counter
	down(eastCount);

	//increment counter
	eastCars++;
	//this is first car on bridge; lock to vehicles traveling other way
	if(eastCars == 1)
		down(lock);
	//unlock counter
	up(eastCount);

	printf(""CYAN"---> Car %d going east on the bridge\n"END"", *selfID );
	sleep( CROSSINGTIME );
	printf(""CYAN"Car %d going east off the bridge--->\n"END"", *selfID );

	//block counter
	down(eastCount);
	//car was last on bridge so unlock everything
	eastCars--;
	if(eastCars == 0)
		up(lock);
	//unlock counter and weight limit
	up(eastCount);
	up(eastLimiter);

	pthread_exit(NULL);
}

void *westCar(void * arg){
	int *selfID = (int *) arg;

  //random delay for spice
	sleep(rand() % MAXWAIT);

	//cross the bridge if able
	down(westLimiter);
	//lock counter
	down(westCount);

	//add car to cars traveling west
	westCars++;
	//first car on bridge; lock to vehicles traveling opposite direction
	if(westCars == 1)
		down(lock);
	//unlock counter for cars
	up(westCount);

	printf(""PURPLE"Car %d going west on the bridge <---\n"END"", *selfID );
	sleep(CROSSINGTIME);
	printf(""PURPLE"<--- Car %d going west off the bridge\n"END"", *selfID );;

	//lock counter
	down(westCount);
	//car has left bridge
	westCars--;
	//if last one on bridge, unlock everything
	if(westCars == 0)
		up(lock);	
	up(westCount);
	up(westLimiter);

	pthread_exit(NULL);
}

/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
/// Main function. 
/// Creates threads for each vehicle
int main(void){

	//initilise the semaphores, and set binary semaphores to 'up' positions
	lock = semtran(IPC_PRIVATE);	
	eastCount = semtran(IPC_PRIVATE);
	westCount = semtran(IPC_PRIVATE);
	eastLimiter = semtran(IPC_PRIVATE);
	westLimiter = semtran(IPC_PRIVATE);
	up(eastCount);
	up(westCount);
	up(lock);

	//initialise variables, and arrays
	int i;
	//number of cars simultaneouslyon bridge
	eastCars = 0;
	westCars = 0;

	//arrays of vehicle IDs
	int IDEastTrucks[EASTTRUCKS];
	int IDWestTrucks[WESTTRUCKS];
	int IDEastCars[EASTCARS];
	int IDWestCars[WESTCARS];

	//arrays of vehicle thread IDs
	pthread_t tIDEastTrucks[EASTTRUCKS];
	pthread_t tIDWestTrucks[WESTTRUCKS];
	pthread_t tIDEastCars[EASTCARS];
	pthread_t tIDWestCars[WESTCARS];

	//assign IDs to vehicles
	for(i = 0; i < EASTTRUCKS; i++)
		IDEastTrucks[i] = i;
	for(i = 0; i < WESTTRUCKS; i++)
		IDWestTrucks[i] = i;
	for(i = 0; i < WESTCARS; i++)
		IDWestCars[i] = i;
	for(i = 0; i < EASTCARS; i++)
		IDEastCars[i] = i;

	//set the limits for cars simultaneously on bridge
	for(i = 0; i < MAXCARS; i++)
		up(eastLimiter);
	for(i = 0; i < MAXCARS; i++)
		up(westLimiter);

	//create threads
	for(i = 0; i < EASTTRUCKS; i++)
		if(pthread_create(&tIDEastTrucks[i], NULL, eastTruck, (void *)&IDEastTrucks[i] )){
			perror("error creating East truck thread");
			exit(EXIT_FAILURE);
		}
	for(i = 0; i < WESTTRUCKS; i++)
		if(pthread_create(&tIDWestTrucks[i], NULL, westTruck, (void *)&IDWestTrucks[i] )){
			perror("error creating West truck thread");
			exit(EXIT_FAILURE);
		}
	for(i = 0; i < WESTCARS; i++)
		if(pthread_create(&tIDWestCars[i], NULL, westCar, (void *)&IDWestCars[i] )){
			perror("error creating West car thread");
			exit(EXIT_FAILURE);
		}
	for(i = 0; i < EASTCARS; i++)
		if(pthread_create(&tIDEastCars[i], NULL, eastCar, (void *)&IDEastCars[i] )){
			perror("error creating East car thread");
			exit(EXIT_FAILURE);
		}

	//join threads when done
	for(i = 0; i < WESTCARS; i++)
		if(pthread_join(tIDWestCars[i], NULL )){
			perror("error joining West car thread");
			exit(EXIT_FAILURE);		
		}
	for(i = 0; i < EASTCARS; i++)
		if(pthread_join(tIDEastCars[i], NULL )){
			perror("error joining East car thread");
			exit(EXIT_FAILURE);		
		}
	for(i = 0; i < EASTTRUCKS; i++)
		if(pthread_join(tIDEastTrucks[i], NULL )){
			perror("error joining East truck thread");
			exit(EXIT_FAILURE);		
		}
	for(i = 0; i < WESTTRUCKS; i++)
		if(pthread_join(tIDWestTrucks[i], NULL )){
			perror("error joining Wast truck thread");
			exit(EXIT_FAILURE);		
		}
	
	deleteAllSemaphores();
	return 0;
}
