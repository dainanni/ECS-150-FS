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
	uint16_t dataIndex; //2 bytes for ind ex of first data block
	char rootPadding[10]; //10 unused bytes of padding

}__attribute__((packed)); //packed attributes to pack memory blocks into one continuous chunk


struct Superblock super;// superblock
struct Fatblock* Fatarray; //FAT array to store all FAT blocks
struct Root root[128];
static int diskCheck = 0; //Global static variable to check 



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
		
	}
	
	diskCheck++;// Set flag to 1 to indicate Mount done
	return 0;//Success
}


int fs_umount(void)
{
	/* TODO: Phase 1 */

	if(diskCheck != 1)//If no disk mounted
		return -1;

	if(block_write(0, (void*)&super) == -1) //Writing out superblock to virtual disk
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

	


	return 0;
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */



	return 0;
}

int fs_ls(void)
{
	/* TODO: Phase 2 */


	return 0;

}

int fs_open(const char *filename)
{
	/* TODO: Phase 3 */


	return 0;
}

int fs_close(int fd)
{
	/* TODO: Phase 3 */

	return 0;
}

int fs_stat(int fd)
{
	/* TODO: Phase 3 */

	return 0;
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */

	return 0;
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */

	return 0;
}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */

	return 0;
}

