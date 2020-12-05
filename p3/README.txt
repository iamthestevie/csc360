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
				or :	./diskput disk.IMA foo.txt 	
