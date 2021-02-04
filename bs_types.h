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

/* Liste, die Prozessen enthält*/
typedef struct threshListEntry {
	pid_t pid;
	struct threshListEntry* next;
} threshListEntry_t; 

typedef threshListEntry_t* threshList_t;

threshList_t lowThresh; // Liste für Prozessen die unter eine untere Schränke an Seitenfehlerrate liegt
threshList_t backupThresh; // eine Kopie der Liste für Prozessen die unter eine untere Schränke an Seitenfehlerrate lieget
threshList_t highThresh; // Liste für Prozessen die über eine obere Schänke an Seitenfehlerrate liegt

/* data type for a page table entry, the page table is an array of this element type*/
/* this data type represents the information held by a page*/
typedef struct pageTableEntry_struct
{
	Boolean present;	// Flag die markiert, ob die Seite im Hauptspeicher geladen wurde
	Boolean modified;	// Flag die markiert ob eine Seite überschrieben/verändert wurde
	Boolean referenced; // Flag die markiert ob eine Seite referenziert wurde
	unsigned char agingVal;		// Aging Wert
	int frame;			// Adresse im physikalischen Speicher (wenn present)
	int swapLocation;	// Adresse im Sekundärspeicher (wenn nicht present)
} page_t;

/* list type used by the OS to keep track of the currently available frames	*/
/* in physical memory. Used for allocating additional and freeing used		*/
/* pyhsical memory for/by processes											*/
typedef struct frameListStruct
{
	int frame;						
	Boolean used;					//	markiert, ob der Rahmen in Benutzung ist
	page_t* residentPage;
	struct frameListStruct* next;	
} frameListEntry_t;

typedef frameListEntry_t* frameList_t; // points to the list of frames

/* data type for the Process Control Block */
/* +++ this might need to be extended to support future features	+++ */
/* +++ like additional schedulers or advanced memory management		+++ */
typedef struct PCB_struct
{
	Boolean valid;				
	pid_t pid;					
	pid_t ppid;					
	unsigned ownerID;			
	unsigned start;				//	Startzeitpunkt -> könnte für den log hilfreich sein
	unsigned duration;			//	Zeit zwischen Start und Terminierung
	unsigned usedCPU;			// 
	processType_t type;			/* NOT USED ANYWHERE DELETE BEFORE ABGABE*/
	status_t status;			/* NOT USED ANYWHERE DELETE BEFORE ABGABE*/
	simInfo_t simInfo;			/* NOT USED ANYWHERE DELETE BEFORE ABGABE*/
	unsigned size;				// size of logical process memory in pages
	page_t *pageTable;		

	frameList_t usedFrames;		// Zeigt zum head der zum Prozess gehörigen usedFrames Liste	

	unsigned faultCount;		//	zählt die Seitenfehler des Prozess
	unsigned timeSince;			//	Zeit seit letztem Seitenfehler

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


#endif  /* __BS_TYPES__ */ 