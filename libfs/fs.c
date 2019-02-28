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






int fs_mount(const char *diskname)
{
	/* TODO: Phase 1 */
}

int fs_umount(void)
{
	/* TODO: Phase 1 */
}

int fs_info(void)
{
	/* TODO: Phase 1 */
}

int fs_create(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_ls(void)
{
	/* TODO: Phase 2 */
}

int fs_open(const char *filename)
{
	/* TODO: Phase 3 */
}

int fs_close(int fd)
{
	/* TODO: Phase 3 */
}

int fs_stat(int fd)
{
	/* TODO: Phase 3 */
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

