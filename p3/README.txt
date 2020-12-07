University of Victoria - Fall 2020
CSC 360 Assignment 3
Steve Muir - V00347783

Instructions to build and run
1. Compile: Type 'make' from the command line.
2. Run: There are 4 files associated with this program. All commands should be executed from the terminal.
	
	1. diskinfo.c:	execute ./diskinfo '<file system image>'
	
	2. disklist.c:	execute ./disklist '<file system image>'
	
	3. diskget.c:	execute ./diskget '<file system image> <file>'
	
	4. diskput.c: 	execute ./diskput '<file system image> <path to file/filename>'
			
			for example:	./diskput disk.IMA /sudir1/subdir2/foo.txt
					or :		./diskput disk.IMA foo.txt 	

Notes:

1. 	There are problems with disklist.c and diskput.c in that I ran out of time to understand how to both list
	subdirectories and copy a file into a subidrectory.

2.	Thanks for all the time spent marking this semester. I'm sure a lot of effort goes into this.