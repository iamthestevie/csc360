/*
	University of Victoria - Fall 2020
	CSC 360 Assignment 3 - Part I
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "p3help.h"


// 	get_os_name
//	purpose: 	Reads the OS name from the FAT12 disk image
//	input: 		osName - a char array to store the name of the OS
//				p - a pointer to the mapped memory
//	output:		void
void get_os_name(char *osName, char *p) {

	// osName starts at byte 3 of the boot sector
	int i;
	for(i = 0; i < 8; i++)
	{
		osName[i] = p[i+3];
	}
}

// 	get_disk_label
//	purpose: 	Reads the disk label from the FAT12 disk image
//	input: 		diskLabel - a char array to store the name of the disk label
//				p - a pointer to the mapped memory
//	output:		void
void get_disk_label(char *diskLabel, char *p) {
	
	int i;
	for (i = 0; i < 8; i++) // may need to change to i < 11.
	{
		diskLabel[i] = p[i+43];
	}

	if (diskLabel[0] == ' ')
	{
		p += SECTOR_SIZE * 19;

		while (p[0] != 0x00)
		{
			if (p[11] == 8)
			{
				for (i = 0; i < 8; i++)
				{
					diskLabel[i] = p[i];
				}
			}
			p += 32;
		}
	}
}

int main (int argc, char* argv[]) {
	
	// check that input format is correct ./diskinfo '<file system image>'
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

	// READ AND OUTPUT THE INFORMATION FROM DISK

	// osname
	char *osName = malloc(sizeof(char));
	get_os_name(osName, p);

	// label of the disk
	char *diskLabel = malloc(sizeof(char));
	get_disk_label(diskLabel, p);



	// clean up
	munmap(p, buf.st_size);
	close(fd);
	free(osName);
	free(diskLabel);

	return 0;
}