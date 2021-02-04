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
	/*	Da jeder Prozess eine usedFrameList besitzt: durch alle laufende Prozessen und deren
		usedFrameLists iterieren. Falls eine Seite während eines Timer-intervals referenziert wurde,
		dann wird die Linkste Bit der Aging Wert der Seite Bitfolge auf 1 gesetzt,
		anderenfalls verschiebt jeder Bit den Aging Wert um eine Stelle nach rechts.	*/
	for (unsigned pid = 1; pid < MAX_PROCESSES; pid++)	//	iteriert durch den process table
	{
		if ((processTable[pid].valid) && (processTable[pid].pageTable != NULL))
		{
			frameList_t iterator = processTable[pid].usedFrames; // iteriere durch die usedFrames	
			while (iterator != NULL)
			{
				iterator->residentPage->agingVal >> 1;	//	Verschiebe die Bits des Aging Werts um eine Stelle nach rechts -> halbiert den Wert
				if (iterator->residentPage->referenced)
				{
					iterator->residentPage->agingVal |= 0x80;	//	wenn referenziert: Setze den am meisten links stehenden bit auf 1
					iterator->residentPage->referenced = FALSE; //	setze den r-bit zurück
				}
				iterator = iterator->next;
			}
		}
	}

}