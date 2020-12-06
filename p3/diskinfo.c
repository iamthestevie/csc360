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

// 	get_fat_sectors
//	purpose: 	finds out how many sectors are in each FAT
//	input: 		p - a pointer to the mapped memory	
//	output:		int number of sectors per FAT
int get_fat_sectors(char *p) {
	return p[22] + (p[23] << 8);
}

// 	get_num_root_files
//	purpose: 	finds the number of files in the root directory
//	input: 		p - a pointer to the mapped memory	
//	output:		returns an int amount of the files in the root directory
int get_num_root_files(char *p) {

	// advance p to the beginning of the root directory
	p += SECTOR_SIZE * 19;
	int count = 0;

	while (p[0] != 0x00)
	{
		// check the attribute bits
		// if not a volume label, subdir, nor unused
		if ((p[11] & 0b00000010) == 0 && (p[11] & 0b00001000) == 0 && (p[11] & 0b00010000) == 0) 
		{
			count++;
		}

		p += 32;
	}

	return count;
}

// 	get_num_fats
//	purpose: 	finds out how many FAT copies are on disk
//	input: 		p - a pointer to the mapped memory	
//	output:		int number of FAT copies
int get_num_fats(char *p) {
	
	// byte 16 in the boot sector contains the number of FATs
	return p[16];
}

// 	print_disk_info
//	purpose: 	finds out how many sectors are in each FAT
//	input: 		p - a pointer to the mapped memory	
//	output:		int number of sectors per FAT
void print_disk_info(char *osName, char *diskLabel, int diskSize, int freeSize, int numRootFiles, int numFats, int fatSectors) {
	
	printf("OS Name: %s\n", osName);
	printf("Label of the disk: %s\n", diskLabel);
	printf("Total size of the disk: %d bytes\n", diskSize);
	printf("Free size of the disk: %d bytes\n", freeSize);
	printf("\n==================\n");
	printf("The number of files in the image: %d\n", numRootFiles);
	printf("\n==================\n");
	printf("Number of FAT copies: %d\n", numFats);
	printf("Sectors per FAT: %d\n", fatSectors);

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

	/// get the stat structure for the file and crete virtual mapping
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

	// disk size
	int diskSize = get_total_disk_size(p);

	// free space on disk
	int freeSize = get_free_disk_size(diskSize, p);

	// root directory files
	int numRootFiles = get_num_root_files(p);

	// number of FAT copies
	int numFats = get_num_fats(p);

	// sectors per FAT
	int fatSectors = get_fat_sectors(p);

	// print info
	print_disk_info(osName, diskLabel, diskSize, freeSize, numRootFiles, numFats, fatSectors);

	// clean up
	munmap(p, buf.st_size);
	close(fd);
	free(osName);
	free(diskLabel);

	return 0;
}