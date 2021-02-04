// pageReplacement.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "memoryManagement.h"

/* ------------------------------------------------------------------------ */
/*		               Declarations of local variables						*/

/* list of empty frames available to allocate*/
unsigned emptyFrameCounter;						// number of empty Frames 
frameList_t emptyFrameList = NULL;				// the beginning of EMPTY framelist
frameListEntry_t *emptyFrameListTail = NULL;	// end of the EMPTY frame list

/*
// list of backup frames, only to be used in desperation
unsigned backupFrameCoutner;	// a counter to ensure there are always x amount of frames as backup
frameList_t backupList = NULL;	
*/

/*
unsigned lowThreshCounter;		// um größer der List zu ermittlen
unsigned backupCounter;			
unsigned highThreshCounter;
*/

/* ------------------------------------------------------------------------ */
/*		               Declarations of local helper functions				*/


Boolean isPagePresent(unsigned pid, unsigned page);
/* Predicate returning the present/absent status of the page in memory		*/

Boolean storeEmptyFrame(int frame);
/*	Store the frame number in the data structure of empty frames				
	and update emptyFrameCounter											*/

int getEmptyFrame(void);
/*	Returns the frame number of an empty frame.								
	A return value of -1 indicates that no empty frame exists. In this case	
	a page replacement algorithm must be called to free evict a page and		
	thus clear one frame */

/*
Boolean storeBackupFrame(int frame);
/* adds a frame to a pool of backup frames*/
*/

Boolean storeUsedFrame(unsigned frameNo, unsigned page, unsigned pid);
/*	Einlagern der Frame, und zugehörigen information in der List alle der	
	benutzten Rahmen */
// BOTH STORING AND REMOVING USED FRAME MUST BE CHANGED TO GO THROUGH THE PROCESS TABLE AND ACCESS THE LIST OF EACH PROCESS
//	Ibrahim
Boolean removeUsedFrame(int frameNo, unsigned page, unsigned pid);


// THIS NEEDS TO TAKE IN PID SO IT CAN REFERENCE THE SAME PROCESS TABLE
frameList_t sortUsedFrameList(unsigned const char point, frameList_t list);
/*	Ein Hilsfuntkion zur Sortierung der usedFrameList eines Prozess, der wird		
	bevor eine Seite verdrängt werden, aufgerufen, damit die erste Seite (die mit	
	dem kleinsten Aging-wert) in der Liste verdrängt wurde							
	Die Sortierung ist modelliert nach Radix sort							*/

Boolean movePageOut(unsigned pid, unsigned page, int frame); 
/*	Creates an empty frame at the given location.							
	Copies the content of the frame occupid by the given page to secondary	
	storage.																	
	The exact location in seocondary storage is found and alocated by		
	this function.															
	The page table entries are updated to reflect that the page is no longer 
	present in RAM, including its location in seondary storage				
	Returns TRUE on success and FALSE on any error							*/

Boolean movePageIn(unsigned pid, unsigned page, unsigned frame);
/* Returns TRUE on success and FALSE on any error							*/

Boolean updatePageEntry(unsigned pid, action_t action);
/*	updates the data relevant for page replacement in the page table entry,	
	e.g. set reference and modify bit.										
	In this simulation this function has to cover also the required actions	
	nornally done by hardware, i.e. it summarises the actions of MMu and OS  
	when accessing physical memory.											
	Returns TRUE on success and FALSE on any error							*/

Boolean pageReplacement(unsigned *pid, unsigned *page, int *frame); 
/*	===== The page replacement algorithm								======	
	In the initial implementation the frame to be cleared is chosen			
	globaly and randomly, i.e. a frame is chosen at random regardless of the	
	process that is currently using it.										
	The values of pid and page number passed to the function may be used by  
	local replacement strategies 
	OUTPUT: 
	The frame number, the process ID and the page currently assigned to the	
	frame that was chosen by this function as the candidate that is to be	
	moved out and replaced by another page is returned via the call-by-		
	reference parameters.													
	Returns TRUE on success and FALSE on any error							*/



/* ------------------------------------------------------------------------ */
/*                Start of public Implementations							*/


Boolean initMemoryManager(void)
{
	printf("Initializing memory manager...");
	// mark all frames of the physical memory as empty 
	for (int i = 0; i < MEMORYSIZE; i++) {
		storeEmptyFrame(i);
	}
	printf("...initialized");
	return TRUE;
}


int accessPage(unsigned pid, action_t action)
/* handles the mapping from logical to physical address, i.e. performs the	
	task of the MMU and parts of the OS in a computer system				
	Returns the number of the frame on success, also in case of a page fault 
	Returns a negative value on error										*/
{
	int frame = INT_MAX;		// the frame the page resides in on return of the function
	unsigned outPid = pid;
	unsigned outPage= action.page;
	// check if page is present
	if (isPagePresent(pid, action.page))
	{// yes: page is present
		// look up frame in page table and we are done
		printf("\t page present in memory, fetching...\n");
		frame = processTable[pid].pageTable[action.page].frame;
	}
	else
	{// no: page is not present
		logPid(pid, "Pagefault");
		// check for an empty frame
		logPid(pid, "Running replacement algorithm");
		pageReplacement(&outPid, &outPage, &frame);
		// move candidate frame out to secondary storage
		movePageOut(outPid, outPage, frame);
		movePageIn(pid, action.page, frame);
	}
	// update page table for replacement algorithm
	updatePageEntry(pid, action);
	return frame;
}


Boolean createPageTable(unsigned pid)
/* Create and initialise the page table	for the given process				*/
/* Information on max. process size must be already stored in the PCB		*/
/* Returns TRUE on success, FALSE otherwise									*/
{
	page_t *pTable = NULL;
	// create and initialise the page table of the process
	printf("\initializing process Table of process %d size %d pages\n", pid, processTable[pid].size);
	pTable = malloc(processTable[pid].size * sizeof(page_t));		// creates an array of pages a process holds
	if (pTable == NULL) return FALSE; 
	// initialise the page table
	for (unsigned i = 0; i < processTable[pid].size; i++)
	{
		pTable[i].present = FALSE;		// indicates that Page is not stored in 
		pTable[i].frame = -1;
		pTable[i].swapLocation = -1;
	}
	processTable[pid].pageTable = pTable; 
	return TRUE;
}

// TODO remove all page entries/frame entries from the usedFrameList that belong to this process!!
// alle seiten/rahmen einträge der usedFramelist, die zu dem Prozess gehört, entfernen!!
Boolean deAllocateProcess(unsigned pid)
/* free the physical memory used by a process, destroy the page table		*/
/* returns TRUE on success, FALSE on error									*/
{
	// iterate the page table and mark all currently used frames as free
	page_t *pTable = processTable[pid].pageTable;
	for (unsigned i = 0; i < processTable[pid].size; i++)
	{
		if (pTable[i].present == TRUE)
		{	// page is in memory, so free the allocated frame
			storeEmptyFrame(pTable[i].frame);	// add to pool of empty frames
			// update the simulation accordingly !! DO NOT REMOVE !!
			sim_UpdateMemoryMapping(pid, (action_t) { deallocate, i }, pTable[i].frame);
		}
	}
	free(processTable[pid].pageTable);	// free the memory of the page table
	return TRUE;
}

Boolean allocateOnStart(const int initFrames, unsigned pid)
/*	Allocates a proportional amount of frames to a process
	TODO: What happends when there are < ttlFrames free? 
		Process will not start
	process*/
{
	int fraction = processTable[pid].size * 0.2; // 20% of the size will be added to the initial value of 4 frames
	int ttlFrame = initFrames + fraction; 
	if (ttlFrame > emptyFrameCounter) {
		printf("\t !!Speicher nicht genug, prozess %d wird nicht gestartet\n", pid);
		return FALSE;
	}
	for (int i = 0; i < ttlFrame; i++) {
		int frame = getEmptyFrame();
		movePageIn(pid, i, frame);
	}
	return TRUE;
}


/* ---------------------------------------------------------------- */
/*                Implementation of local helper functions          */

Boolean isPagePresent(unsigned pid, unsigned page)
/* Predicate returning the present/absent status of the page in memory		*/
{
	return processTable[pid].pageTable[page].present; 
}

Boolean storeEmptyFrame(int frame)
/* Store the frame number in the data structure of empty frames				*/
/* and update emptyFrameCounter												*/
{
	frameListEntry_t* newEntry = NULL;
	newEntry = malloc(sizeof(frameListEntry_t));
	if (newEntry != NULL)
	{
		// create new entry for the frame passed
		newEntry->next = NULL;
		newEntry->frame = frame;
		newEntry->used = FALSE;
		newEntry->residentPage = NULL;
		if (emptyFrameList == NULL)			// first entry in the list
		{
			emptyFrameList = newEntry;
		}
		else								// appent do the list
			emptyFrameListTail->next = newEntry;
		emptyFrameListTail = newEntry;
		emptyFrameCounter++;				// one more free frame
	}
	return (newEntry != NULL);
}

int getEmptyFrame(void)
/* Returns the frame number of an empty frame.								*/
/* A return value of -1 indicates that no empty frame exists. In this case	*/
/* a page replacement algorithm must be called to evict a page and thus 	*/
/* clear one frame															*/
{
	frameListEntry_t *toBeDeleted = NULL;
	int emptyFrameNo = -1;
	if (emptyFrameList == NULL) return -1;	// no empty frame exists
	emptyFrameNo = emptyFrameList->frame;	// get number of empty frame
	// remove entry of that frame from the list
	toBeDeleted = emptyFrameList;			
	emptyFrameList = emptyFrameList->next; 
	free(toBeDeleted);						// will likely not need to free this as static number of ListEntries which will be managed between the three lists
	emptyFrameCounter--;					// one empty frame less
	
	return emptyFrameNo; 
}

Boolean storeUsedFrame(unsigned frameNo, unsigned page, unsigned pid) {
	printf("\tStoring page in ussedFrameList of process %d...\n", pid);
	frameList_t frameList = processTable[pid].usedFrames;	// a copy of the pointer we use to access the local usedFrameList
	frameList_t newEntry = NULL;
	newEntry = malloc(sizeof(frameList_t));					// create a new list entry
	if (newEntry != NULL) {									// and fill it with the relevant info
		newEntry->frame = frameNo;	
		newEntry->next = NULL;
		newEntry->used = TRUE; // brauche ich das?
		newEntry->residentPage = &(processTable[pid].pageTable[page]);
		if (frameList == NULL) {		// Special-case: this is the first entry of the usedFrameList
			printf("\tList empty, creating new list\n");
			processTable[pid].usedFrames = newEntry;
		}
		else {
			printf("\t	 prepending new entry\n");
			newEntry->next = frameList;					// newEntry will point to the old list as its next value, becoming the head
			processTable[pid].usedFrames = newEntry;	// of the process' new list! i.e. Prepended
		}
	}	
	return (newEntry != NULL);
}

Boolean removeUsedFrame(int frameNo, unsigned page, unsigned pid) {
	frameList_t iterator = processTable[pid].usedFrames;
	if (iterator == NULL) return FALSE;
	if (iterator->frame == frameNo) {	// Spezialfall: erste Eintrag der Liste ist die gesuchte usedFrameListEntry
		processTable[pid].usedFrames = iterator->next;
		return TRUE;
	}
	while (iterator->next != NULL) {	// Normalverlauf: iterieren durch die Liste bis gesuchte Eintrag gefunden
		if (iterator->next->frame == frameNo) {
			iterator->next = iterator->next->next; // einbinden der vom Ziel Eintrag, mit der nach dem Ziel Eintrag
			printf("\tFrame found, removing page %d of process %d from page %d\n", page, pid, frameNo);
			return TRUE;
		}
		// test test 
		iterator = iterator->next;
	}
	printf("\tFrame %d containing page %d of Process %d not found\n", frameNo, page, pid);
	return FALSE;	// Wenn der gesuchte Eintrag nicht gefunden ist,  FALSE zurückliefern
}


/* Geschrieben von Cheang Yi Cherng*/
frameList_t sortUsedFrameList(const unsigned char point, frameList_t list)
/*	Diese Sortierung funktionert nach der Prinzip eines Radix sort, d.h:
	es evaluiert jeder Aging Value nicht nach dem gesamten Wert sondern nach
	der Wert jeder Bitstelle. Diese Sort ist rekursive und wird die Liste 
	in Teil-liste zerkleinen. Diese Teil-listen werden dan rekursive sortiert
	ansteigend nach der Agingwert und zusammengefügt, wenn alle Bitstellen
	evaluiert sind.
	EINSCHRÄNKUNGEN:
	wenn es mehr als zwei Rahmen/Frames gibt, die den gleichen Agingwer hat
	wird dieses Algorithmus nicht erkennen welche Rahmen erst in einem Timer-
	Interval referenziert wurde.*/
{
	if (list == NULL || list->next == NULL) {
		return list;
	}	// spezialfälle: nur ein Eintrag in der Liste oder Liste ist leer
	// sublists OR buckets erstellen
	frameList_t zeroes = NULL;		// die Subliste wo die geprüfte Bit 0 ist
	frameList_t zeroesLast = NULL;	// Zeiger auf die Tail der oberige Liste
	frameList_t ones = NULL;		// die Subliste wo die geprüfte Bit 1 ist
	frameList_t onesLast = NULL;	// Zeiger auf die Tail der oberige Liste
	frameList_t iterator = list;	// copy der Zeiger auf die Liste, wird als Iterator benutzt
	while (iterator != NULL) {		
		if ((iterator->residentPage->agingVal & point)) { // z.b (0101 0001) & (1000 0000) = 0 -nächsterekursion-> (0101 0001) & (0100 0000) = 1
			if (ones == NULL) {		// Erste eintrag der Sublist wo geprüfte Bit = 1
				ones = iterator;	
				onesLast = iterator;
			}
			else {
				onesLast->next = iterator; // normales Appendieren
				onesLast = iterator;
			}
		} else {
			if (zeroes == NULL) {	// Erste eintrag der Sublist wo geprüfte Bit = 0
				zeroes = iterator;
				zeroesLast = iterator;
			}
			else {
				zeroesLast->next = iterator; // normales Appendieren
				zeroesLast = iterator;
			}
		}
		iterator = iterator->next;
	}
	/*if (onesLast->next != NULL) {	// sichern dass die Sublisten auf Null endet
		onesLast->next = NULL;		// hier wird's Warnungen gegeben. da onesLast und zeroesLast immer auf NULL zeigen sollten. 
	}								// kann eig. ausgelassen, aber für die Sicherheit
	if (zeroesLast->next != NULL) {
		zeroesLast->next = NULL;
	}*/
	if (point >> 1) {   // Rekrusive aufruf der Funktion, wobei die Nächste Bitstelle überprüft wird
		frameList_t lhs = sortUsedFrameList(point >> 1, zeroes);
		frameList_t rhs = sortUsedFrameList(point >> 1, ones);
		if (lhs != NULL) {
			iterator = lhs;
			if (rhs == NULL) {
				return lhs;
			}
			while (iterator->next != NULL) {
				iterator = iterator->next;
			}
			iterator->next = rhs;
			return lhs;
		}
		else {
			return rhs;
		}
	}
	if (zeroes != NULL) {
		zeroesLast->next = ones;
		return zeroes;
	}
	else {
		return ones;
	}
}
// Neviyn: this name is bullshit, it doesn't actually move contents, it might as well be called updateMemoryMap
Boolean movePageIn(unsigned pid, unsigned page, unsigned frame)
/* Returns TRUE on success ans FALSE on any error							*/
{
	// copy of the content of the page from secondary memory to RAM not simulated
	// update the page table: mark present, store frame number, clear statistics
	// *** This must be extended for advanced page replacement algorithms ***
	// goes directly to the process and its page, recording info of the frame the page is stored in
	processTable[pid].pageTable[page].frame = frame;		
	processTable[pid].pageTable[page].present = TRUE;
	// page was just moved in, reset all statistics on usage, R- and P-bit at least
	processTable[pid].pageTable[page].modified = FALSE;
	processTable[pid].pageTable[page].referenced = FALSE;
	processTable[pid].pageTable[page].agingVal &= 0x80;	// set left most r bit to 1
	
	// append the new frame to the list 
	storeUsedFrame(frame, page, pid);	// stores the newly loaded page in the used frame list
	// Statistics for advanced replacement algorithms need to be reset here also 

	// update the simulation accordingly !! DO NOT REMOVE !!
	sim_UpdateMemoryMapping(pid, (action_t) { allocate, page }, frame);
	return TRUE;
}

Boolean movePageOut(unsigned pid, unsigned page, int frame)
/* Creates an empty frame at the given location.							*/
/* Copies the content of the frame occupid by the given page to secondary	*/
/* storage.																	*/
/* The exact location in seocondary storage is found and alocated by		*/
/* this function.															*/
/* The page table entries are updated to reflect that the page is no longer */
/* present in RAM, including its location in seondary storage				*/
/* Returns TRUE on success and FALSE on any error							*/
{
	// allocation of secondary memory storage location and copy of page are ommitted for this simulation
	// no distinction between clean and dirty pages made at this point
	// update the page table: mark absent, add frame to pool of empty frames
	// *** This must be extended for advenced page replacement algorithms ***
	processTable[pid].pageTable[page].present = FALSE;
	removeUsedFrame(frame, page, pid); // remove from the list of used frames
	storeEmptyFrame(frame);	// add to pool of empty frames
	// update the simulation accordingly !! DO NOT REMOVE !!
	sim_UpdateMemoryMapping(pid, (action_t) { deallocate, page }, frame);
	return TRUE;
}

Boolean updatePageEntry(unsigned pid, action_t action)
/* updates the data relevant for page replacement in the page table entry,	*/
/* e.g. set reference and modify bit.										*/
/* In this simulation this function has to cover also the required actions	*/
/* normally done by hardware, i.e. it summarises the actions of MMU and OS  */
/* when accessing physical memory.											*/
/* Sets the left most Rbit to 1, this is done on newly stored pages as well */
/* Returns TRUE on success ans FALSE on any error							*/
// *** This must be extended for advences page replacement algorithms ***
{
	processTable[pid].pageTable[action.page].referenced = TRUE;  
	// processTable[pid].pageTable[action.page].agingVal |= 0x80;	// sofort LMB nach 1 setzten? 
	if (action.op == write)
		processTable[pid].pageTable[action.page].modified = TRUE;
	return TRUE; 
}


Boolean pageReplacement(unsigned* outPid, unsigned* outPage, int* outFrame)
/* ===== The page replacement algorithm								======	*/
/*  Occurs due to a page fault, when there are no free frames in memory.
	This function will first sort the list of the frames a process		
	currently occupies, via sortUsedFrameList. Once sorted, this process
	will select the frist frame in the usedFrameList for eviction	*/
/*	OUTPUT: 
	The frame number, the process ID and the page currently assigned to the
	frame that was chosen by this function as the candidate that is to be
	moved out and replaced by another page is returned via the call-by-
	reference parameters.
	Returns TRUE on success and FALSE on any error							*/
{
	Boolean found = FALSE;		// flag to indicate success
	// just for readbility local copies ot the passed values are used:
	unsigned pid = (*outPid);
	unsigned page = (*outPage);
	int frame = *outFrame;

	printf("\tCalling sort for the pages of process %d\n", pid);
	frameList_t victim = sortUsedFrameList(0x80, processTable[pid].usedFrames); // sort the list of used frames of the process
	frame = victim->frame;
	printf("\tvictim found at frame: %d\n", frame);
	found = TRUE;
	if (found)
	{	// assign the current values to the call-by-reference parameters
		(*outPid) = pid;
		(*outPage) = page;
		(*outFrame) = frame;
	}
	return found;
}

