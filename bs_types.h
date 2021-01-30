/* Include-file defining elementary data types used by the 			*/
/* operating system */
#ifndef __BS_TYPES__
#define __BS_TYPES__

typedef enum { FALSE = 0, TRUE } Boolean; 


/* data type for storing of process IDs		*/
typedef unsigned pid_t;

/* NOT USED ANYWHERE DELETE BEFORE ABGABE*/
/* data type for the possible types of processes */
/* the process type determines the IO-characteristic */
typedef enum
{
	os, interactive, batch, background, foreground
} processType_t;

/* NOT USED ANYWHERE DELETE BEFORE ABGABE*/
/* data type for the process-states used for scheduling and life-	*/
/* cycle manegament of the processes 								*/
typedef enum
{
	init, running, ready, blocked, ended

} status_t;

/* NOT USED ANYWHERE DELETE BEFORE ABGABE*/
/* data type for the different events that cause the scheduler to	*/
/* become active */
typedef enum
{
	completed, io, quantumOver

} schedulingEvent_t;

/* NOT USED ANYWHERE DELETE BEFORE ABGABE*/
/* data type for the simulation environment */
/* the information contained ion this struct are not available to the os */
typedef struct simInfo_struct
{
	unsigned IOready;	// simulation time when IO is complete, may be used in the future
} simInfo_t;



/* data type for a page table entry, the page table is an array of this element type*/
/* this data type represents the information held by a page*/
typedef struct pageTableEntry_struct
{
	Boolean present;	// flag for checking if a page is loaded (in frame)
	Boolean modified;	// flag stating a page has been written on/modified during some interval
	Boolean referenced; // flag stating a page has been referenced during some interval -> causes rbits to be set
	unsigned char agingVal;		// R bit, durch 8 bit stellig Bitfolge vom Char repräsentiert 
	int frame;			// physical memory address, if present
	int swapLocation;	// if page is not present, this indicates it's location in secondary memory
						// as the content of the pages is not used in this simulation, it is unused
} page_t;

// Geschrieben von Yi Cherng Cheang # 07.01.21 //
/* Datentyp für einlagerung und Kontrolle eingelagerten Seiten		*/
/* Unsortierte verketete Liste die für jeden Prozess verfügbar ist	*/
typedef struct usedFrameListEntry_Struct
{
	int frame;		// Frame no.
	page_t* residentPage;		// pointer to page entry, holding all relative info including frame no, rbit and page no
	unsigned char* age;			// statt durch das pageTableEntry durchzugehen, einfach der Aging wert hier abspeichern
	struct usedFrameListEntry_Struct* next;	// zeiger fürs Iterieren der Liste	
}usedFrameListEntry_t;

typedef usedFrameListEntry_t* usedFrameList_t; // points to the list of USED frames


/* data type for the Process Control Block */
/* +++ this might need to be extended to support future features	+++ */
/* +++ like additional schedulers or advanced memory management		+++ */
typedef struct PCB_struct
{
	Boolean valid;				// flag for valid process
	pid_t pid;					// process' id number
	pid_t ppid;					// parent process' id number
	unsigned ownerID;			
	unsigned start;				// time of start -> can be used in some log function showing how long 
	unsigned duration;			// time between start and termination
	unsigned usedCPU;			// 
	processType_t type;			/* NOT USED ANYWHERE DELETE BEFORE ABGABE*/
	status_t status;			/* NOT USED ANYWHERE DELETE BEFORE ABGABE*/
	simInfo_t simInfo;			/* NOT USED ANYWHERE DELETE BEFORE ABGABE*/
	unsigned size;				// size of logical process memory in pages
	page_t *pageTable;// pointer to a page table entry of process with this pid

	usedFrameList_t usedFrames; // pointer to the head list of frames this process occupies	

	Boolean overFaultCeil;		// flags to show the process has had many page faults in the last x intervals
	Boolean underFaultFloor;	// flags to show the process has had very little page faults in the last x intervals

} pcb_t;

/* data type for the possible actions wrt. memory usage by a process		*/
/* This data type is used to trigger the respective action in the memory	*/
/* management system														*/
typedef enum
{
	start, end, read, write, allocate, deallocate, error
} operation_t;

/* data type for possible memory use by a process, combining the action with*/
/* the page that is used for this action, e.g. for reading */
/* is the action does not require a page number (i.e. a location in virtual	*/
/* memory), the value of 'page' is not used an may be undefined				*/		
typedef struct action_struct
{
	operation_t op;
	unsigned page;
} action_t;

/* data type for an event, descrtibing a cartain action performad by a		*/
/* process at agiven point in time.											*/
/* It is used for modelling the activities of a process during it's			*/
/* execution wrt. accessing the memory										*/
typedef struct event_struct
{
	unsigned time; 
	pid_t pid;
	action_t action;
} memoryEvent_t;

/* list type used by the OS to keep track of the currently available frames	*/ 
/* in physical memory. Used for allocating additional and freeing used		*/
/* pyhsical memory for/by processes											*/
typedef struct frameListEntry_struct
{
	int frame; 
	struct frameListEntry_struct *next;
} frameListEntry_t;

typedef frameListEntry_t* frameList_t; // points to the list of EMPTY frames



#endif  /* __BS_TYPES__ */ 