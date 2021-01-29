# pageReplacement_bs_ws2020_
 Simulation of page replacement of pages of a process into page frames 

 The goal of this project is to simulate how an operating system could manage processes in and out of main memory
 through an approximation of the LRU (algorithm), the Aging algorithm. This project aims to create the following:
 
 A modularly programmed C project wich contains all functions, data structures to simulate page replacement and 
 additionally a log function which gives out all relevant data: changes to a process, which page from which process
 occupies a frame, how large such frames are and etc,
 
 Frames will be assigned by a locally static basis per process, and as such, frames will be assigned relative to 
 process size. The downside of this is that processes of smaller size may take longer to run as frames are 
 actively swapped on demand.
 
