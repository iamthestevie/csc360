#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "p3help.h"

// 	get_fat
//	purpose: 	returns the value of the FAT at n
//	input: 		n - an int to store an entry in the FAT
//				p - a pointer to the mapped memory
//	output:		int value of the FAT at n
int get_fat(int n, char *p) {
	
	int entry;
	int byte1;
	int byte2;

	// FAT packing from tutorial:
	if ((n % 2) == 0)
	{
		byte1 = p[SECTOR_SIZE + ((3 * n) / 2) + 1] & 0x0F;
		byte2 = p[SECTOR_SIZE + ((3 * n) / 2)] & 0xFF;
		entry = (byte1 << 8) + byte2;
	}
	else
	{
		byte1 = p[SECTOR_SIZE + (int)((3 * n) / 2)] & 0xF0;
		byte2 = p[SECTOR_SIZE + (int)((3 * n) / 2) + 1] & 0xFF;
		entry = (byte1 >> 4) + (byte2 << 4);
	}

	return entry;
}	


int get_total_disk_size(char *p);							// returns total disk image size
int get_free_disk_size(int diskSize, char *p);				// returns the amount of free space on disk
int get_file_size(char *fileName, char *p);					// returns file size (bytes) if the file exists
int get_first_logical_cluster(char *fileName, char *p);		// returns first logical cluster of the file
int disk_contains_file(char *fileName, char *p);			// returns true if the disk contains the file