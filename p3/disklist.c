/*
	University of Victoria - Fall 2020
	CSC 360 Assignment 3 - Part II
	Steve Muir - V00347783
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "p3help.h"

// 	print_directories
//	purpose: 	prints the root directory listing of the image
//	input: 		p - a pointer to the mapped memory
//	output:		void
void print_directories(char *p) {

	while (p[0] != 0x00)
	{
		char file;
		// check the attribute bits
		// if the volume label bit is set then its of type directory 'D'
		if ((p[11] & 0b00010000) == 0b00010000)
		{
			file = 'D';
		}
		// otherwise the file is of type file 'F'
		else
		{
			file = 'F';
		}
		// next we want to get the filename from the image
		char *fileName = malloc(sizeof(char));
		int i;
		for (i = 0; i < 8; i++)
		{
			if (p[i] == ' ')
			{
				break;
			}
			fileName[i] = p[i];
		}

		// now we get the file extension
		char *fileExtension = malloc(sizeof(char));
		for (i = 0; i < 3; i++)
		{
			fileExtension[i] = p[i + 8];
		}

		// concatenate the filename and extension together
		strcat(fileName, ".");
		strcat(fileName, fileExtension);

		// get time file creation data
		// the year is stored as a value since 1980
		// the year is stored in the high seven bits
		int year = ((p[17] & 0b11111110) >> 1) + 1980;
		//the month is stored in the middle four bits
		int month = ((p[16] & 0b11100000) >> 5) + ((p[17] & 0b00000001) << 3);
		// the day is stored in the low five bits
		int day = (p[16] & 0b00011111);
		// the hour is stored in the high five bits
		int hour = ((p[15] & 0b11111000) >> 3);
		//the minutes are stored in the middle 6 bits
		int minute = ((p[14] & 0b11100000) >> 5) + ((p[15] & 0b00000111) << 3);
		// get the file size
		int fileSize = get_file_size(fileName, p);
		// check the attribute bits (subdirectory and unused) are not set
		if ((p[11] & 0b00000010) == 0 && (p[11] & 0b00001000) == 0)
		{
			printf("%c %10d %20s %d-%d-%d %03d:%02d\n", file, fileSize, fileName, year, month, day, hour, minute);
		}
		else if ((p[11] & 0b00010000) == 0b00010000)
		{
			printf("%s\n", fileName);
			printf("==================\n");
		}

		p += 32;
	}
}

int main (int argc, char *argv[]) {

	// check the input ./disklist '<file system image>'
	if (argc < 2)
	{
		printf("Error: execute as follows ./diskinfo '<file system image>'\n");
		exit(1);
	}

	// open the disk
	// Please remember to give write permission to your argv[1] (the image you want map) 
	// by using chmod (if it doest not have the write permission by default), 
	// otherwise you will fail the map.
	int fd;
	struct stat buf;

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		printf("Error: failed to read disk image\n");
		exit(1);
	}

	fstat(fd, &buf);
	char* p = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (p == MAP_FAILED)
	{
		printf("Error: failed to map memory\n");
		exit(1);
	}

	// List the contents of all directories in the disk image file system
	print_directories(p + SECTOR_SIZE * 19);

	// clean up
	munmap(p, buf.st_size);
	close(fd);


	return 0;
}