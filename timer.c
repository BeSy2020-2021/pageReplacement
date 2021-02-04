/* Implementation of the timer handler function								*/
/* for comments on the global functions see the associated .h-file			*/
#include "bs_types.h"
#include "global.h"
#include "timer.h"

void timerEventHandler(void)
/* The event Handler (aka ISR) of the timer event.
	Updates the data structures used by the page replacement algorithm 		*/

{
	logGeneric("Processing Timer Event Handler: resetting R-Bits");
	/*	da jeder Prozess einen usedFrameList besitzt: durch alle laufende Prozessen und deren
		usedFrameList iterieren. falls eine Seite während eines Timer-intervals referenziert wurde,
		dann wird die Linkste Bit der Aging Wert der Seite Bitfolge auf 1 gesetzt
		andernfalls verschiebt jeder Bit der Aging Wert eine Stelle rechts		*/
	for (unsigned pid = 1; pid < MAX_PROCESSES; pid++)	// iterating through the process table
	{
		if ((processTable[pid].valid) && (processTable[pid].pageTable != NULL)) // if the process is valid and has a page table
		{
			frameList_t iterator = processTable[pid].usedFrames; // iterate through its usedFrameList	
			while (iterator != NULL)
			{
				iterator->residentPage->agingVal >> 1;	// shift the agingvalue's bits, one place left for each stored page 
				if (iterator->residentPage->referenced)
				{
					iterator->residentPage->agingVal |= 0x80;	// if the page was referenced in the last interval set left most bit to 1
					iterator->residentPage->referenced = FALSE; // and reset the Rbit
				}
				iterator = iterator->next;
			}
		}
	}

}