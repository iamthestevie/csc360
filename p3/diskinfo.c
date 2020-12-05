/*
	University of Victoria - Fall 2020
	CSC 360 Assignment 3 - Part I
*/

int main (int argc, char* argv[]) {
	
	// check that input format is correct ./diskinfo '<file system image>'
	if (argc < 2)
	{
		printf("Error: execute as follows ./diskinfo '<file system image>'\n");
		exit(1);
	}

	// open the disk
	// Please remember to give write permission to your argv[1] (the image you want map) 
	// by using chmod (if it doest not have the write permission by default), otherwise you will fail the map.
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

	// read the information from disk
	

	// clean up
	munmap(p, buf.st_size);
	close(fd);
	free(osName);
	free(diskLabel);

	return 0;
}