/*
	University of Victoria - Fall 2020
	CSC 360 Assignment 3 - Part IV
	Steve Muir - V00347783
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "p3help.h"

void updateRootDirectory(char* fileName, int fileSize, int firstLogicalSector, char* p) {
	// Find free root dir address
	p += SECTOR_SIZE * 19;
	while (p[0] != 0x00) {
		p += 32;
	}

	// Set filename and extension
	int i;
	int done = -1;
	for (i = 0; i < 8; i++) {
		char character = fileName[i];
		if (character == '.') {
			done = i;
		}
		p[i] = (done == -1) ? character : ' ';
	}
	for (i = 0; i < 3; i++) {
		p[i+8] = fileName[i+done+1];
	}

	// Set attributes
	p[11] = 0x00;

	// Set create date/time
	time_t t = time(NULL);
	struct tm *now = localtime(&t);
	int year = now->tm_year + 1900;
	int month = (now->tm_mon + 1);
	int day = now->tm_mday;
	int hour = now->tm_hour;
	int minute = now->tm_min;
	p[14] = 0;
	p[15] = 0;
	p[16] = 0;
	p[17] = 0;
	p[17] |= (year - 1980) << 1;
	p[17] |= (month - ((p[16] & 0b11100000) >> 5)) >> 3;
	p[16] |= (month - (((p[17] & 0b00000001)) << 3)) << 5;
	p[16] |= (day & 0b00011111);
	p[15] |= (hour << 3) & 0b11111000;
	p[15] |= (minute - ((p[14] & 0b11100000) >> 5)) >> 3;
	p[14] |= (minute - ((p[15] & 0b00000111) << 3)) << 5;

	// Set first logical cluster
	p[26] = (firstLogicalSector - (p[27] << 8)) & 0xFF;
	p[27] = (firstLogicalSector - p[26]) >> 8;

	// Set fileSize
	p[28] = (fileSize & 0x000000FF);
	p[29] = (fileSize & 0x0000FF00) >> 8;
	p[30] = (fileSize & 0x00FF0000) >> 16;
	p[31] = (fileSize & 0xFF000000) >> 24;
}

/*
	p: a pointer to the mapped memory
	Returns the next index in the FAT containing 0x00 (free)
*/
int getNextFreeFatIndex(char* p) {
	p += SECTOR_SIZE;

	int n = 2;
	while (get_fat(n, p) != 0x000) {
		n++;
	}

	return n;
}

/*
	n: the FAT entry to update
	value: the value to set FAT at index n
	p: a pointer to the mapped memory
	Sets FAT at entry n to value
*/
void setFatEntry(int n, int value, char* p) {
	p += SECTOR_SIZE;

	if ((n % 2) == 0) {
		p[SECTOR_SIZE + ((3*n) / 2) + 1] = (value >> 8) & 0x0F;
		p[SECTOR_SIZE + ((3*n) / 2)] = value & 0xFF;
	} else {
		p[SECTOR_SIZE + (int)((3*n) / 2)] = (value << 4) & 0xF0;
		p[SECTOR_SIZE + (int)((3*n) / 2) + 1] = (value >> 4) & 0xFF;
	}
}

/*
	p: a pointer to the mapped memory
	p2: a pointer to the mapped file to write to
	fileName: the file to copy from the disk image
	fileSize: the size in bytes of the file to copy
	Writes a file from a disk image to the local directory
*/
void copy_file(char* p, char* p2, char* fileName, int fileSize) {
	if (!disk_contains_file(fileName, p + SECTOR_SIZE * 19)) {
		int bytesRemaining = fileSize;
		int current = getNextFreeFatIndex(p);
		updateRootDirectory(fileName, fileSize, current, p);

		while (bytesRemaining > 0) {
			int physicalAddress = SECTOR_SIZE * (31 + current);
			
			int i;
			for (i = 0; i < SECTOR_SIZE; i++) {
				if (bytesRemaining == 0) {
					setFatEntry(current, 0xFFF, p);
					return;
				}
				p[i + physicalAddress] = p2[fileSize - bytesRemaining];
				bytesRemaining--;
			}
			setFatEntry(current, 0x69, p);
			int next = getNextFreeFatIndex(p);
			setFatEntry(current, next, p);
			current = next;
		}
	} 
}

int main (int argc, char *argv[]) {

	// check that input format is correct ./diskget '<file system image> <path/to/file>'
	if (argc < 3)
	{
		printf("Error: execute as follows ./diskput '<file system image>'\n");
		exit(1);
	}

	// open the disk
	// Please remember to give write permission to your argv[1] (the image you want map) 
	// by using chmod (if it doest not have the write permission by default), 
	// otherwise you will fail the map.
	int fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		printf("Error: failed to read disk image\n");
		exit(1);
	}

	// get the stat structure for the file and crete virtual mapping
	struct stat buf;
	fstat(fd, &buf);
	
	char* p = mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (p == MAP_FAILED)
	{
		printf("Error: failed to map memory\n");
		exit(1);
	}

	// open the file
	// Please remember to give write permission to your argv[1] (the image you want map) 
	// by using chmod (if it doest not have the write permission by default), 
	// otherwise you will fail the map.
	int fd2 = open(argv[2], O_RDWR);
	if (fd2 < 0)
	{
		printf("Error: unable to find file.\n");
		exit(1);
	}

	// get the stat structure for the file and crete virtual mapping
	struct stat buf2;
	fstat(fd2, &buf2);
	
	int fileSize = buf2.st_size;
	
	char* p2 = mmap(NULL, fileSize, PROT_READ, MAP_SHARED, fd2, 0);
	if (p2 == MAP_FAILED) {
		printf("Error: failed to file into memory\n");
		exit(1);
	}

	// Copy file from local directory to disk image if there's space
	int totalDiskSize = get_total_disk_size(p);
	int freeDiskSize = get_free_disk_size(totalDiskSize, p);
	if (freeDiskSize >= fileSize) {
		copy_file(p, p2, argv[2], fileSize);
	} else {
		printf("%d %d\n", freeDiskSize, fileSize);
		printf("Error: Not enough free space in the disk image.\n");
	}

	// Clean up
	munmap(p, buf.st_size);
	munmap(p2, fileSize);
	close(fd);
	close(fd2);

	return 0;
}