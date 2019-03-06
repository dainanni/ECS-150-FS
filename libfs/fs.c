#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

/* TODO: Phase 1 */

struct Superblock{

	char signature[8]; //8 bytes of signature
	uint16_t blockTotal; //2 bytes of total amount of virtual disk blocks
	uint16_t rootIndex; //2 bytes root dir index
	uint16_t dataIndex; //2 bytes of data block start index
	uint16_t dataTotal; //2 bytes of total data block amount 
	uint8_t fatBlock; // 1 byte os FAT block amount
	char superPadding[4079]; //4079 unused bytes of padding

} __attribute__((packed));


struct Fatblock{

	uint16_t wordFat; //16 bit unsigned words	

} __attribute__((packed));


struct Root{

	char filename[16]; //16 byte file name string
	uint32_t fileSize; //4 bytes
	uint16_t dataIndex; //2 bytes for index of first data block
	char rootPadding[10]; //10 unused bytes of padding

}__attribute__((packed)); //packed attributes to pack memory blocks into one continuous chunk


struct Filedes{

	int rootIndex; //Root index position
	size_t offset; //file offset

}__attribute__((packed));




static struct Superblock super;// superblock
static struct Fatblock* Fatarray; //FAT array to store all FAT blocks
static struct Root root[FS_FILE_MAX_COUNT]; //Root dir array
static struct Filedes Fdarray[FS_OPEN_MAX_COUNT]; //File descriptor table array
static int diskCheck = 0; //Global static variable to check 




/*Helper Functions*/


int fd_valid(int fd)
{
	if(fd <0 || fd >31) //Check if fd is invalid
                return -1;// Failure
	
	else if(Fdarray[fd].rootIndex == -1) // file descriptor not opened
		return -1;

	return 0; //File descriptor found and is valid

}



/*Main Functions*/


int fs_mount(const char *diskname)
{
	/* TODO: Phase 1 */
	char sigCheck[8] = "ECS150FS"; //Array signature check
	
	if(block_disk_open(diskname) == -1) //If opening virtual disk block fails
		return -1; //Failure in mounting FS
	
	else if(diskCheck != 0)
		return -1;

	else
	{
		if(block_read(0, (void*)&super) == -1) //Reading content of 1st virtual block into superblock
			return -1;//failure

		for(int i=0; i<8; i++) //traversal to compare signature of superblock with required ECS150FS file system signature
			if(!(super.signature[i] == sigCheck[i]))
				return -1; //Failure in finding right file system
		
		if(!(block_disk_count() == super.blockTotal)) //Total amount of virtual blocks check
			return -1; //Failed check
		
		Fatarray = (struct Fatblock*) malloc((super.fatBlock)*BLOCK_SIZE); //Allocating memory for the Fatarray to store an array of structs of BLOCKSIZE with number of fatblocks from superblock 	
		
		//root =  (struct Root*) malloc(sizeof(struct Root)*FS_FILE_MAX_COUNT); //Allocating memory for roo dir

		for(int index=0; index<super.fatBlock; index++)
			if((block_read((index+1),(void*)(&Fatarray[(BLOCK_SIZE/2)*index]))) == -1) //Reading content of virtual block into fatblock array
				return -1;//failure

		if((block_read((super.rootIndex),(void*)&root)) == -1) //Reading content of virtual block into root
			return -1;//failure

		for(int i=0; i<FS_OPEN_MAX_COUNT; i++)//FDarray initialization
			Fdarray[i].rootIndex = -1; //initialize fd to invalid
		
	}
	
	diskCheck++;// Set flag to 1 to indicate Mount done
	return 0;//Success
}


int fs_umount(void)
{
	/* TODO: Phase 1 */

	if(diskCheck != 1)//If no disk mounted
		return -1;

	else if(block_write(0, (void*)&super) == -1) //Writing out superblock to virtual disk
		return -1;

	for(int index=0; index<super.fatBlock; index++)
		if((block_write((index+1),(void*)(&Fatarray[(BLOCK_SIZE/2)*index]))) == -1) //Writing content into virtual block from fatblock array
			return -1;//failure

	if(block_write((super.rootIndex),(void*)&root) == -1) //Writing out rootblock to virtual disk
		return -1; //failure

	free(Fatarray); //Deallocating memory set for Fat array
	//free(root); //Deallocating memory set for root dir array

	if(block_disk_close() == -1) //If virtual disk was not opened 
		return -1; //Failure	

	diskCheck = 0;// set flag back to 0 or unmounted status

	return 0; //Success
}


int fs_info(void)
{
	/* TODO: Phase 1 */

	int zeroCount = 0; //Count for Fat block zeros
	int nullCount = 0; //Count for Root directory null filename entries

	if(diskCheck == 0) //If disk wasn't mounted
		return -1;
	else
	{
		printf("FS Info:\n");
		printf("total_blk_count=%d\n", super.blockTotal);
		printf("fat_blk_count=%d\n", super.fatBlock);
		printf("rdir_blk=%d\n", super.rootIndex);
		printf("data_blk=%d\n", super.dataIndex);
		printf("data_blk_count=%d\n", super.dataTotal);

		for(int i=0; i<super.dataTotal; i++)
		{
			if(Fatarray[i].wordFat == 0)
				zeroCount++;	

		}
		
		for(int j=0; j<FS_FILE_MAX_COUNT; j++)
		{
			if(root[j].filename[0] == '\0')
				nullCount++;
		}

		printf("fat_free_ratio=%d/%d\n", zeroCount, super.dataTotal);
		printf("rdir_free_ratio=%d/%d\n", nullCount, FS_FILE_MAX_COUNT);
	}

	return 0;//success

}

int fs_create(const char *filename)
{
	/* TODO: Phase 2 */

	int fileCount = 0; //used to keep track of number of files in the root dir

	if(!filename) //filename is NULL
		return -1; //Failure

	else if((strlen(filename) + 1) > FS_FILENAME_LEN) //Length of filename exceeds max limit
		return -1;

	for(int i=0; i<FS_FILE_MAX_COUNT; i++)
	{
		if(strcmp(root[i].filename, filename) == 0) //If filename already exists
			return -1;
		
		else if(root[i].filename[0] != '\0')
		{
			fileCount++; //Counts number of files that exist in the fileSystem
			
			if(fileCount >= FS_FILE_MAX_COUNT) //Max file count reached
				return -1;
		}	

		else if(root[i].filename[0] == '\0')//Empty space found
		{
			strcpy(root[i].filename, filename); //Copies given filename to empty space	
			root[i].fileSize = 0; //Empty file created of size 0
			root[i].dataIndex = 0xFFFF; //FAT_EOC assigned to data start index of this file
			break; //end create operation once file has been added
		}
	}

	return 0; //Success
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */
	
	int notCount = 0;//Count of unmatched filenames
	int index = 0; //Flag to keep track of changing indices
	int temp = 0; //Temporary variable to store FAT index info

	if(!filename)//filename is invalid
		return -1;
			
	else if((strlen(filename) + 1) > FS_FILENAME_LEN) //Length of filename exceeds max limit
		return -1;
	
	for(int i=0; i<FS_FILE_MAX_COUNT; i++)
	{
		if(strcmp(root[i].filename, filename) != 0) //If filename not found	
		{
			notCount++;

			if(notCount >= FS_FILE_MAX_COUNT) //If filename isn't found in entire root dir
				return -1; //Failure
		}

		else if(strcmp(root[i].filename, filename) == 0) //If file is found in file system
		{	
			root[i].filename[0] = '\0'; //Setting filename to empty null
		      	
			index = root[i].dataIndex; //Set start data block index

			while(Fatarray[index].wordFat != 0xFFFF)// If FAT_EOC Encountered then break
			{
				Fatarray[index].wordFat = temp;
				Fatarray[index].wordFat = 0;//sets data blocks associated with file to be deleted to 0
				index = temp;// stores index using temp variable				
			}

			break; //Exit loop after delete complete
		}
	}


	return 0; //success
}

int fs_ls(void)
{
	/* TODO: Phase 2 */

	if(diskCheck == 0) //Disk wasn't mounted
		return -1;//Failure

	printf("FS Ls:\n");
		
	for(int i=0; i<FS_FILE_MAX_COUNT; i++)
	{
		if(root[i].filename[0] != '\0')//Existing file in root directory
			printf("file: %s, size: %d, data_blk: %d\n", root[i].filename, root[i].fileSize, root[i].dataIndex); //Prints all required ls info

	}

	return 0; //success

}

int fs_open(const char *filename)
{
	/* TODO: Phase 3 */

	int notCount = 0;//Count of unmatched filenames
	int openCount = 0; //Count of open file descriptors
	int FileDesc = -1; // File Descriptor return value

	if(!filename || (strlen(filename) + 1) > FS_FILENAME_LEN) //Filename is invalid or too long
		return -1; //Failure

	for(int i=0; i<FS_FILE_MAX_COUNT; i++)
	{
		if(strcmp(root[i].filename, filename) != 0) //Filename not found
		{
			notCount++;
	
			if(notCount >= FS_FILE_MAX_COUNT) //If filename isn't found in entire root dir
				return -1; //Failure
		}

		else if(strcmp(root[i].filename, filename) == 0) //Filename found
		{
			for(int fd=0; fd < FS_OPEN_MAX_COUNT; fd++)
			{
				if(Fdarray[fd].rootIndex != -1) //If file descriptor is already open
				{
					openCount++;

					if(openCount >= FS_OPEN_MAX_COUNT) //If 32 file descriptors are already open
						return -1; //Failure

				}
				
				else if(Fdarray[fd].rootIndex == -1) //First empty file descriptor found
				{
					Fdarray[fd].rootIndex = i; //Set value of rootIndex at file descriptor position to file index
					Fdarray[fd].offset = 0; //initialize offset to 0
					FileDesc = fd;// Assigning file descriptor for return
					break;
				}

			}			
			
			break;
		}

	}

	return FileDesc; //Return file descriptor upon succesful open
}

int fs_close(int fd)
{
	/* TODO: Phase 3 */

	if(fd_valid(fd) == -1) //Check for validity of file descriptor
		return -1; //Failure

	else
	{

		Fdarray[fd].rootIndex = -1; //Setting rootIndex back to empty/NULL at file descriptor position
		Fdarray[fd].offset = 0; //Setting offset back to 0;

	}

	return 0; //Success in closing file descriptor
}

int fs_stat(int fd)
{
	/* TODO: Phase 3 */

	int size = 0; //File size to be returned

	if(fd_valid(fd) == -1) //Check for validity of file descriptor
		return -1; //Failure
	
	else
		size = root[Fdarray[fd].rootIndex].fileSize; //Assigning file size using index value of file in root directory pointed to by given file descriptor

	return size; //Succesful size return
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */

	if(fd_valid(fd) == -1) //Check for validity of file descriptor
		return -1; //Failure	

	else if(offset > root[Fdarray[fd].rootIndex].fileSize) //Check if offset is out of bounds
		return -1;// Failure

	else
		Fdarray[fd].offset = offset; //Changing file descriptor associated file offset value to given offset value

	return 0; //Success
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
	
	if(fd_valid(fd) == -1) //Check for validity of file descriptor
		return -1; //failure




	return 0;
}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
	
	if(fd_valid(fd) == -1) //Check for validity of file descriptor
		return -1; //Failure

	root[Fdarray[fd].rootIndex] //File we need to read




	return 0;
}

