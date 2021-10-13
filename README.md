# suspension_bridge_problem
Demonstrates a variation of the 'readers and writers' synchronization problem using semaphores

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
