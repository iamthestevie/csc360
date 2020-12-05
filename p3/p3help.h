// GLOBAL VARIABLES

#define SECTOR_SIZE 512
#define TRUE 1
#define	FALSE 0

// FUNCTIONS

int get_fat(int n, char *p);								// returns the value of the FAT at n
int get_total_disk_size(char *p);							// returns total disk image size
int get_free_disk_size(int diskSize, char *p);				// returns the amount of free space on disk
int get_file_size(char *fileName, char *p);					// returns file size (bytes) if the file exists
int get_first_logical_cluster(char *fileName, char *p);		// returns first logical cluster of the file
int disk_contains_file(char *fileName, char *p);			// returns true if the disk contains the file