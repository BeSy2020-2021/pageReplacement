/* Implementation of the timer handler function								*/
/* for comments on the global functions see the associated .h-file			*/
#include "bs_types.h"
#include "global.h"
#include "timer.h"

/*TODO:*/
/*Datastructure holding all loaded pages now exists, change this to go		*/
/*through only the list and not all proceses and their pages!!				*/	

void timerEventHandler(void)
/* The event Handler (aka ISR) of the timer event. 							*/
/* Updates the data structures used by the page replacement algorithm 		*/

/* xxxx This function is a stub with reduced functionality, it must be xxxx */
/* xxxx extended for advanced memory management function to enable     xxxx */
/* xxxx full functionality of the operating system					   xxxx */
{
	logGeneric("Processing Timer Event Handler: resetting R-Bits");
	// in absence of a data structure indexing the pages that are present, all 
	// running processes and all present pages must be checked. 
	// If the page is present, the R-bit is reset.
	usedFrameList_t iterator = loadedFrameList;
	while (iterator != NULL) {
		iterator->residentPage->agingVal >>= 1;
		iterator = iterator->next;
	}

	/* REMOVED: above function should iterate throught the linked list */
	/* instead of iterating through all processes and their pages	   */
	// this is dumb go thru the processes on the process table,:
	// if it is valid and has a page table initialized
	// then go thru the pages and shift the rbits
	// if referenced set the rbit on the very right to one

	for (unsigned pid = 1; pid < MAX_PROCESSES; pid++)
	{
		if ((processTable[pid].valid) && (processTable[pid].pageTable != NULL))
			for (unsigned page = 0; page < processTable[pid].size; page++) {
				processTable[pid].pageTable[page].referenced >> 1;			// run through all running processes setting r bit one right	
				// now that the bits are set, we must have the algorithm evict the lowest value char
			}																
	}	
	

	// for a more sophisticated memory management systems with reasonable 
	// page replacement, this timer event endler must be improved
}
