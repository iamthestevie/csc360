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

// 	next_free_fat_index
//	purpose: 	finds the next free sector in the FAT (0x00 equals free)
//	input: 		p - a pointer to the mapped memory
//	output:		int value of the next free FAT index
int next_free_fat_index(char* p) {
	
	// p += 512
	p += SECTOR_SIZE;

	// 0 and 1 reserved
	int n = 2;
	while (get_fat(n, p) != 0x000) 
	{
		n++;
	}

	return n;
}

// 	update_root_dir
//	purpose: 	creates a file on the image root section
//	input: 		fileName - the name of the file being copied
//				fileSize - the bytes being copies
//				p - a pointer to the mapped memory
//	output:		none
void update_root_dir(char* fileName, int fileSize, int firstLogicalCluster, char* p) {
	
	// Advance memory pointer to the root directory of the disk image
	p += SECTOR_SIZE * 19;
	
	// search for a free sector
	while (p[0] != 0x00) 
	{
		p += 32;
	}

	// once we have a free sector, set filename and extension
	int i;
	int end = -1;
	
	for (i = 0; i < 8; i++) 
	{
		char ch = fileName[i];
		
		if (ch == '.') 
		{
			end = i;
		}

		p[i] = (end == -1) ? ch : ' ';
	}

	for (i = 0; i < 3; i++) 
	{
		p[i + 8] = fileName[i + end + 1];
	}

	// Set attribute bits
	p[11] = 0b00000000;

	// Set create time information
	time_t t = time(NULL);
	
	// get 'now'
	struct tm *now = localtime(&t);
	
	// from now extract all relevant time/date info 
	int year = now->tm_year + 1900;
	int month = (now->tm_mon + 1);
	int day = now->tm_mday;
	int hour = now->tm_hour;
	int minute = now->tm_min;
	
	// offset 14/15 - creation time
	p[14] = 0;
	p[15] = 0;

	// offset 16/17 - creation date
	p[16] = 0;
	p[17] = 0;
	
	// convert year to a value since 1980 and store in high 7 bits
	p[17] |= (year - 1980) << 1;
	
	// month is stored in middle 4 bits
	p[17] |= (month - ((p[16] & 0b11100000) >> 5)) >> 3;
	p[16] |= (month - (((p[17] & 0b00000001)) << 3)) << 5;
	
	// day is stored in the low five bits
	p[16] |= (day & 0b00011111);
	
	// hour is stored in the high five bits
	p[15] |= (hour << 3) & 0b11111000;
	
	// minutes are stored in the middle 6 bits
	p[15] |= (minute - ((p[14] & 0b11100000) >> 5)) >> 3;
	p[14] |= (minute - ((p[15] & 0b00000111) << 3)) << 5;

	// set first logical cluster
	p[26] = (firstLogicalCluster - (p[27] << 8)) & 0xFF;
	p[27] = (firstLogicalCluster - p[26]) >> 8;

	//  and finally set fileSize
	p[28] = (fileSize & 0x000000FF);
	p[29] = (fileSize & 0x0000FF00) >> 8;
	p[30] = (fileSize & 0x00FF0000) >> 16;
	p[31] = (fileSize & 0xFF000000) >> 24;
}

// 	set_fat_entry
//	purpose: 	fat packing algorithm from tutorial to set FAT entry at n to val.
//	input: 		n - FAT to update
//				val - value to set FAT at index n
//				p - a pointer to the mapped memory
//	output:		int value of the next free FAT index
void set_fat_entry(int n, int val, char* p) {
	
	p += SECTOR_SIZE;

	if ((n % 2) == 0) 
	{
		p[SECTOR_SIZE + ((3 * n) / 2) + 1] = (val >> 8) & 0x0F;
		p[SECTOR_SIZE + ((3 * n) / 2)] = val & 0xFF;
	} 
	else 
	{
		p[SECTOR_SIZE + (int)((3 * n) / 2)] = (val << 4) & 0xF0;
		p[SECTOR_SIZE + (int)((3 * n) / 2) + 1] = (val >> 4) & 0xFF;
	}
}

// 	copy_file
//	purpose: 	copies a file from working directory to the disk image
//	input: 		p - a pointer to the mapped memory
//				p2 - a pointer to the file we wish to write to
//				fileName - the name of the file to copy
//				fileSize - the size of the file to copy in bytes
//	output:		void
void copy_file(char* p, char* p2, char* fileName, int fileSize) {
	
	// first we check if the file we wish to copy exists
	if (!disk_contains_file(fileName, p + SECTOR_SIZE * 19)) 
	{
		// we don't want to modify bytesRemaining so we copy value to new variable
		int bytesRemaining = fileSize;
		
		// get the next free index in the FAT
		int current = next_free_fat_index(p);
		
		update_root_dir(fileName, fileSize, current, p);

		while (bytesRemaining > 0) 
		{
			// the physical address conversion in the FAT spec
			int physicalAddress = SECTOR_SIZE * (31 + current);
			
			int i;
			for (i = 0; i < SECTOR_SIZE; i++) 
			{
				if (bytesRemaining == 0) 
				{
					set_fat_entry(current, 0xFFF, p);
					return;
				}
				
				p[i + physicalAddress] = p2[fileSize - bytesRemaining];
				bytesRemaining--;
			}
			
			set_fat_entry(current, 0x69, p);
			int next = next_free_fat_index(p);
			set_fat_entry(current, next, p);
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

	// Copy file from local directory to disk image
	// first get the total space available on disk
	int totalDiskSize = get_total_disk_size(p);
	
	// then find out how much of this total is available
	int freeDiskSize = get_free_disk_size(totalDiskSize, p);
	
	// simple comparison to see if there's any space available
	if (freeDiskSize >= fileSize) 
	{
		// if so copy the file
		copy_file(p, p2, argv[2], fileSize);
	}

	else 
	{
		// otherwise inform user there's not enough space available
		printf("%d %d\n", freeDiskSize, fileSize);
		printf("Error: Not enough free space in the disk image.\n");
	}

	// unmap memory and close connections
	munmap(p, buf.st_size);
	munmap(p2, fileSize);
	close(fd);
	close(fd2);

	return 0;
}