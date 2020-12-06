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

// 	get_total_disk_size
//	purpose: 	returns total size of the disk
//	input: 		p - a pointer to the mapped memory
//	output:		int total size of the disk
int get_total_disk_size(char *p) {

	int bytesPerSector = p[11] + (p[12] << 8);

	int totalSectorCount = p[19] + (p[20] << 8);

	int totalDiskSize = bytesPerSector * totalSectorCount;

	return totalDiskSize;
}

// 	get_free_disk_size
//	purpose: 	returns size of the free space on disk
//	input: 		diskSize - total bytes on disk
//				p - a pointer to the mapped memory 
//	output:		int total size of the free space disk in bytes
int get_free_disk_size(int diskSize, char *p) {

	int emptySectors = 0, freeBytes = 0;
	int i;

	for (i = 2; i < (diskSize / SECTOR_SIZE); i++) 
	{
		if (get_fat(i, p) == 0x000)
		{
			emptySectors++;
		}
	}

	freeBytes = SECTOR_SIZE * emptySectors;

	return freeBytes;
}

// 	get_file_size
//	purpose: 	returns size of the file in bytes if the file exists
//				in the root directory of p and - 1 otherwise.
//	input: 		fileName - pointer to the filename to check for
//				p - a pointer to the mapped memory 
//	output:		int total size of the file in bytes if found, -1 otherwise.
int get_file_size(char *fileName, char *p) {

	// while the directory entry is not free
	while (p[0] != 0x00)
	{
		// check the attribute bits
		if ((p[11] & 0b00000010) == 0 && (p[11] & 0b00001000) == 0) 
		{
			char *workingFileName = malloc(sizeof(char));
			char *workingFileExtension = malloc(sizeof(char));
			
			// get the filename
			int i;
			for (i = 0; i < 8; i++)
			{
				if (p[i] == ' ')
				{
					break;
				}

				workingFileName[i] = p[i];
			}

			// get the file extension
			for (i = 0; i < 3; i++)
			{
				workingFileExtension[i] = p[i + 8];
			}

			// put the filename and extension together
			strcat(workingFileName, ".");
			strcat(workingFileName, workingFileExtension);

			// compare the workingFileName with the fileName
			if (strcmp(fileName, workingFileName) == 0)
			{
				int fileSize = (p[28] & 0xFF) + ((p[29] & 0xFF) << 8) + ((p[30] & 0xFF) << 16) + ((p[31] & 0xFF) << 24);

				return fileSize;
			}
		}

		p += 32;
	}

	return -1;
}

// 	get_first_logical_cluster
//	purpose: 	returns the first logical cluster/sector of a file
//				in the root directory of p and - 1 otherwise.
//	input: 		fileName - pointer to the filename to check for
//				p - a pointer to the mapped memory 
//	output:		int first logical cluster
int get_first_logical_cluster(char *fileName, char *p) {

	// while the directory entry is not free
	while (p[0] != 0x00)
	{
		// check the attribute bits
		if ((p[11] & 0b00000010) == 0 && (p[11] & 0b00001000) == 0) 
		{
			char *workingFileName = malloc(sizeof(char));
			char *workingFileExtension = malloc(sizeof(char));
			
			// get the filename
			int i;
			for (i = 0; i < 8; i++)
			{
				if (p[i] == ' ')
				{
					break;
				}

				workingFileName[i] = p[i];
			}

			// get the file extension
			for (i = 0; i < 3; i++)
			{
				workingFileExtension[i] = p[i + 8];
			}

			// put the filename and extension together
			strcat(workingFileName, ".");
			strcat(workingFileName, workingFileExtension);

			if (strcmp(fileName, workingFileName) == 0)
			{
				return p[26] + (p[27] << 8);
			}
		}

		p += 32;
	}

	return -1;
}

// 	disk_contains_file
//	purpose: 	checks if the disk contains a file called fileName
//				in the root directory of p and - 1 otherwise.
//	input: 		fileName - pointer to the filename to check for
//				p - a pointer to the mapped memory 
//	output:		TRUE (1) if the disk contains the file and FALSE (0) otherwise
int disk_contains_file(char *fileName, char *p) {

	while (p[0] != 0x00)
	{
		// check the attribute bits
		if ((p[11] & 0b00000010) == 0 && (p[11] & 0b00001000) == 0) 
		{
			char *workingFileName = malloc(sizeof(char));
			char *workingFileExtension = malloc(sizeof(char));
			
			// get the filename
			int i;
			for (i = 0; i < 8; i++)
			{
				if (p[i] == ' ')
				{
					break;
				}

				workingFileName[i] = p[i];
			}

			// get the file extension
			for (i = 0; i < 3; i++)
			{
				workingFileExtension[i] = p[i + 8];
			}

			// put the filename and extension together
			strcat(workingFileName, ".");
			strcat(workingFileName, workingFileExtension);

			if (strcmp(fileName, workingFileName) == 0)
			{
				return TRUE;
			}
		}

		p += 32;
	}

	return FALSE;
}