/*
	University of Victoria - Fall 2020
	CSC 360 Assignment 3 - Part III
	Steve Muir - V00347783
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "p3help.h"

// 	copy_file
//	purpose: 	copies a file from the image to current workign directory
//	input: 		p - a pointer to the mapped memory
//	output:		int number of sectors per FAT
void copyFile(char* p, char* p2, char* fileName) {
	int firstLogicalSector = get_first_logical_cluster(fileName, p + SECTOR_SIZE * 19);
	int n = firstLogicalSector;
	int fileSize = get_file_size(fileName, p + SECTOR_SIZE * 19);
	int bytesRemaining = fileSize;
	int physicalAddress = SECTOR_SIZE * (31 + n);

	do {
		n = (bytesRemaining == fileSize) ? firstLogicalSector : get_fat(n, p);
		physicalAddress = SECTOR_SIZE * (31 + n);

		int i;
		for (i = 0; i < SECTOR_SIZE; i++) {
			if (bytesRemaining == 0) {
				break;
			}
			p2[fileSize - bytesRemaining] = p[i + physicalAddress];
			bytesRemaining--;
		}
	} while (get_fat(n, p) != 0xFFF);
}

int main () {

	// check that input format is correct ./diskget '<file system image> <file name>'
	if (argc < 3)
	{
		printf("Error: execute as follows ./diskget '<file system image> <file name>'\n");
		exit(1);
	}

	// Open disk image and map memory
	int fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		printf("Error: failed to read disk image\n");
		exit(1);
	}
	struct stat buf;
	fstat(fd, &buf);
	char* p = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (p == MAP_FAILED) 
	{
		printf("Error: failed to map disk image memory\n");
		close(fd);
		exit(1);
	}

	int fileSize = get_file_size(argv[2], p + SECTOR_SIZE * 19);
	if (fileSize > 0) 
	{
		// Create new file to be written
		int fd2 = open(argv[2], O_RDWR | O_CREAT, 0666);
		if (fd2 < 0) 
		{
			printf("Error: failed to open file\n");
			close(fd);
			exit(1);
		}

		// Seek to the last byte and write \0 to "stretch" the file
		int result = lseek(fd2, fileSize-1, SEEK_SET);
		if (result == -1) 
		{
			munmap(p, buf.st_size);
			close(fd);
			close(fd2);
			printf("Error: failed to seek to end of file\n");
			exit(1);
		}
		result = write(fd2, "", 1);
		if (result != 1) 
		{
			munmap(p, buf.st_size);
			close(fd);
			close(fd2);
			printf("Error: failed to write last byte\n");
			exit(1);
		}

		// Map memory for file to be written
		char* p2 = mmap(NULL, fileSize, PROT_WRITE, MAP_SHARED, fd2, 0);
		if (p2 == MAP_FAILED) {
			printf("Error: failed to map file memory\n");
			exit(1);
		}

		// Copy file from disk image to local directory
		copyFile(p, p2, argv[2]);

		munmap(p2, fileSize);
		close(fd2);
	} else {
		printf("File not found.\n");
	}

	// Clean up
	munmap(p, buf.st_size);
	close(fd);
	return 0;
}