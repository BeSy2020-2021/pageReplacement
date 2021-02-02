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
	for (unsigned pid = 1; pid < MAX_PROCESSES; pid++)
	{
		if ((processTable[pid].valid) && (processTable[pid].pageTable != NULL)) { // checks for valid, intitialized process and page table
			frameList_t iterator = processTable[pid].usedFrames;
			while (iterator != NULL) {
				iterator->residentPage->agingVal >>= 1;
				if (iterator->residentPage->referenced) {
					iterator->residentPage->agingVal |= 0x80;
				}
				iterator++;
			}
		}
	}
}