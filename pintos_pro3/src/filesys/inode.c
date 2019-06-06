#include "filesys/inode.h"
#include "filesys/cache.h"
#include <list.h>
#include <debug.h>
#include <round.h>
#include <string.h>
#include "filesys/filesys.h"
#include "filesys/free-map.h"
#include "threads/malloc.h"

/* Identifies an inode. */
#define INODE_MAGIC 0x494e4f44
#define DIRECT_INDEX_NUM 124
#define INDIRECT_INDEX_SECTOR_NUM 128
#define DOUBLY_INDIRECT_INDEX_SECTOR_NUM 128 *128

/* On-disk inode.
   Must be exactly BLOCK_SECTOR_SIZE bytes long. */
struct inode_disk
  {
    // block_sector_t start;               /* First data sector. */
    off_t length;                       /* File size in bytes. */
    block_sector_t direct_index[DIRECT_INDEX_NUM];       /* For direct indexes. */
		block_sector_t indirect_index;                       /* For indirect indexes. */ 
		block_sector_t doubly_indirect_index;								 /* For doubly indirect indexes*/
    unsigned magic;                     /* Magic number. */
    // uint32_t unused[110];               /* Not used. */
  };

struct indirect_index_sector 
  {
    block_sector_t blocks[INDIRECT_INDEX_SECTOR_NUM];
  };

	static bool inode_allocate(struct inode_disk *disk_inode);
	static bool inode_deallocate(struct inode *inode);

/* Returns the number of sectors to allocate for an inode SIZE
   bytes long. */
static inline size_t
bytes_to_sectors (off_t size, size_t unit)
{
  return DIV_ROUND_UP (size, unit);
}

/* In-memory inode. */
struct inode 
  {
    struct list_elem elem;              /* Element in inode list. */
    block_sector_t sector;              /* Sector number of disk location. */
    int open_cnt;                       /* Number of openers. */
    bool removed;                       /* True if deleted, false otherwise. */
    int deny_write_cnt;                 /* 0: writes ok, >0: deny writes. */
    struct inode_disk data;             /* Inode content. */
  };

/* Returns the block device sector that contains byte offset POS
   within INODE.
   Returns -1 if INODE does not contain data for a byte at offset
   POS. */
static block_sector_t
byte_to_sector (const struct inode *inode, off_t pos) 
{
  ASSERT (inode != NULL);
  // if (pos < inode->data.length)
  //   return inode->data.start + pos / BLOCK_SECTOR_SIZE;
	if(0 <= pos && pos < inode -> data.length){
		off_t index = pos / BLOCK_SECTOR_SIZE;
    struct inode_disk * id = &inode->data;
    off_t offset = index;
		off_t base_index = 0;
    off_t cum_index = 0;
    block_sector_t res;
    // direct blocks
    cum_index += DIRECT_INDEX_NUM;
    if(offset < cum_index){
      return id->direct_index[offset];
    }
    base_index = cum_index;
    // indirect blocks
    cum_index += INDIRECT_INDEX_SECTOR_NUM;
    if(offset < cum_index){
      off_t index_level_1 = (offset - base_index);
      struct indirect_index_sector *iis = calloc(1, sizeof(struct indirect_index_sector));
      cache_read(id->indirect_index, iis); 
      res = iis->blocks[index_level_1];
      free(iis);
      return res;
    }
    base_index = cum_index;
    // doubly indirect blocks
    cum_index += DOUBLY_INDIRECT_INDEX_SECTOR_NUM;
    if(offset < cum_index){
      off_t index_level_1 = (offset - base_index) / INDIRECT_INDEX_SECTOR_NUM;
      off_t index_level_2 = (offset - base_index) % INDIRECT_INDEX_SECTOR_NUM;
      struct indirect_index_sector *iis = calloc(1, sizeof(struct indirect_index_sector));
      cache_read(id->doubly_indirect_index, iis);
      cache_read(iis->blocks[index_level_1], iis);
      res = iis->blocks[index_level_2];
      return res;
    }
    return -1;
	}
  else
    return -1;
}

/* List of open inodes, so that opening a single inode twice
   returns the same `struct inode'. */
static struct list open_inodes;

/* Initializes the inode module. */
void
inode_init (void) 
{
  list_init (&open_inodes);
}

/* Initializes an inode with LENGTH bytes of data and
   writes the new inode to sector SECTOR on the file system
   device.
   Returns true if successful.
   Returns false if memory or disk allocation fails. */
bool
inode_create (block_sector_t sector, off_t length)
{
  struct inode_disk *disk_inode = NULL;
  bool success = false;

  ASSERT (length >= 0);

  /* If this assertion fails, the inode structure is not exactly
     one sector in size, and you should fix that. */
  ASSERT (sizeof *disk_inode == BLOCK_SECTOR_SIZE);

  disk_inode = calloc (1, sizeof *disk_inode);
  
  if (disk_inode != NULL)
    {
      // size_t sectors = bytes_to_sectors (length);
      disk_inode->length = length;
      disk_inode->magic = INODE_MAGIC;
      if (inode_allocate (disk_inode)) 
        {
          cache_write(sector, disk_inode);
          success = true; 
        } 
      free (disk_inode);
    }
  return success;
}

/* Reads an inode from SECTOR
   and returns a `struct inode' that contains it.
   Returns a null pointer if memory allocation fails. */
struct inode *
inode_open (block_sector_t sector)
{
  struct list_elem *e;
  struct inode *inode;

  /* Check whether this inode is already open. */
  for (e = list_begin (&open_inodes); e != list_end (&open_inodes);
       e = list_next (e)) 
    {
      inode = list_entry (e, struct inode, elem);
      if (inode->sector == sector) 
        {
          inode_reopen (inode);
          return inode; 
        }
    }

  /* Allocate memory. */
  inode = malloc (sizeof *inode);
  if (inode == NULL)
    return NULL;

  /* Initialize. */
  list_push_front (&open_inodes, &inode->elem);
  inode->sector = sector;
  inode->open_cnt = 1;
  inode->deny_write_cnt = 0;
  inode->removed = false;
  // block_read (fs_device, inode->sector, &inode->data);
  cache_read(inode->sector, &inode->data);
  return inode;
}

/* Reopens and returns INODE. */
struct inode *
inode_reopen (struct inode *inode)
{
  if (inode != NULL)
    inode->open_cnt++;
  return inode;
}

/* Returns INODE's inode number. */
block_sector_t
inode_get_inumber (const struct inode *inode)
{
  return inode->sector;
}

/* Closes INODE and writes it to disk.
   If this was the last reference to INODE, frees its memory.
   If INODE was also a removed inode, frees its blocks. */
void
inode_close (struct inode *inode) 
{
  /* Ignore null pointer. */
  if (inode == NULL)
    return;

  /* Release resources if this was the last opener. */
  if (--inode->open_cnt == 0)
    {
      /* Remove from inode list and release lock. */
      list_remove (&inode->elem);
 
      /* Deallocate blocks if removed. */
      if (inode->removed) 
        {
          free_map_release (inode->sector, 1); 
					inode_deallocate(inode);
        }

      free (inode); 
    }
}

/* Marks INODE to be deleted when it is closed by the last caller who
   has it open. */
void
inode_remove (struct inode *inode) 
{
  ASSERT (inode != NULL);
  inode->removed = true;
}

/* Reads SIZE bytes from INODE into BUFFER, starting at position OFFSET.
   Returns the number of bytes actually read, which may be less
   than SIZE if an error occurs or end of file is reached. */
off_t
inode_read_at (struct inode *inode, void *buffer_, off_t size, off_t offset) 
{
  uint8_t *buffer = buffer_;
  off_t bytes_read = 0;
  uint8_t *bounce = NULL;

  while (size > 0) 
    {
      /* Disk sector to read, starting byte offset within sector. */
      block_sector_t sector_idx = byte_to_sector (inode, offset);
      int sector_ofs = offset % BLOCK_SECTOR_SIZE;

      /* Bytes left in inode, bytes left in sector, lesser of the two. */
      off_t inode_left = inode_length (inode) - offset;
      int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
      int min_left = inode_left < sector_left ? inode_left : sector_left;

      /* Number of bytes to actually copy out of this sector. */
      int chunk_size = size < min_left ? size : min_left;
      if (chunk_size <= 0)
        break;

      if (sector_ofs == 0 && chunk_size == BLOCK_SECTOR_SIZE)
        {
          /* Read full sector directly into caller's buffer. */
          // block_read (fs_device, sector_idx, buffer + bytes_read);
          cache_read (sector_idx, buffer + bytes_read);
        }
      else 
        {
          /* Read sector into bounce buffer, then partially copy
             into caller's buffer. */
          if (bounce == NULL) 
            {
              bounce = malloc (BLOCK_SECTOR_SIZE);
              if (bounce == NULL)
                break;
            }
          // block_read (fs_device, sector_idx, bounce);
          cache_read (sector_idx, bounce);
          memcpy (buffer + bytes_read, bounce + sector_ofs, chunk_size);
        }
      
      /* Advance. */
      size -= chunk_size;
      offset += chunk_size;
      bytes_read += chunk_size;
    }
  free (bounce);

  return bytes_read;
}


/* Writes SIZE bytes from BUFFER into INODE, starting at OFFSET.
   Returns the number of bytes actually written, which may be
   less than SIZE if end of file is reached or an error occurs.
   (Normally a write at end of file would extend the inode, but
   growth is not yet implemented.) */
off_t
inode_write_at (struct inode *inode, const void *buffer_, off_t size,
                off_t offset) 
{
  const uint8_t *buffer = buffer_;
  off_t bytes_written = 0;
  uint8_t *bounce = NULL;

  if (inode->deny_write_cnt)
    return 0;

	if(byte_to_sector(inode, offset + size - 1) == -1){
    inode->data.length = offset + size;
    bool success = inode_allocate (& inode->data);
    if (!success) return 0; 
    inode->data.length = offset + size;
    cache_write (inode->sector, & inode->data);
	}

  while (size > 0) 
    {
      /* Sector to write, starting byte offset within sector. */
      block_sector_t sector_idx = byte_to_sector (inode, offset);
      int sector_ofs = offset % BLOCK_SECTOR_SIZE;

      /* Bytes left in inode, bytes left in sector, lesser of the two. */
      off_t inode_left = inode_length (inode) - offset;
      int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
      int min_left = inode_left < sector_left ? inode_left : sector_left;

      /* Number of bytes to actually write into this sector. */
      int chunk_size = size < min_left ? size : min_left;
      if (chunk_size <= 0)
        break;

      if (sector_ofs == 0 && chunk_size == BLOCK_SECTOR_SIZE)
        {
          /* Write full sector directly to disk. */
          // block_write (fs_device, sector_idx, buffer + bytes_written);
          cache_write(sector_idx, buffer + bytes_written);
        }
      else 
        {
          /* We need a bounce buffer. */
          if (bounce == NULL) 
            {
              bounce = malloc (BLOCK_SECTOR_SIZE);
              if (bounce == NULL)
                break;
            }

          /* If the sector contains data before or after the chunk
             we're writing, then we need to read in the sector
             first.  Otherwise we start with a sector of all zeros. */
          if (sector_ofs > 0 || chunk_size < sector_left) 
            // block_read (fs_device, sector_idx, bounce);
            cache_read (sector_idx, bounce);
          else
            memset (bounce, 0, BLOCK_SECTOR_SIZE);
          memcpy (bounce + sector_ofs, buffer + bytes_written, chunk_size);
          // block_write (fs_device, sector_idx, bounce);
          cache_write (sector_idx, bounce);
        }

      /* Advance. */
      size -= chunk_size;
      offset += chunk_size;
      bytes_written += chunk_size;
    }
  free (bounce);

  return bytes_written;
}

/* Disables writes to INODE.
   May be called at most once per inode opener. */
void
inode_deny_write (struct inode *inode) 
{
  inode->deny_write_cnt++;
  ASSERT (inode->deny_write_cnt <= inode->open_cnt);
}

/* Re-enables writes to INODE.
   Must be called once by each inode opener who has called
   inode_deny_write() on the inode, before closing the inode. */
void
inode_allow_write (struct inode *inode) 
{
  ASSERT (inode->deny_write_cnt > 0);
  ASSERT (inode->deny_write_cnt <= inode->open_cnt);
  inode->deny_write_cnt--;
}

/* Returns the length, in bytes, of INODE's data. */
off_t
inode_length (const struct inode *inode)
{
  return inode->data.length;
}

static void
inode_allocate_indirect (block_sector_t* p_entry, size_t num_sectors, int level)
{
  static char zeros[BLOCK_SECTOR_SIZE];

  ASSERT (level <= 2);

  if (level == 0) {
    // allocate a free sector and put it into the block
    if (*p_entry == 0) {
      free_map_allocate (1, p_entry);
      cache_write (*p_entry, zeros);
    }
    return;
  }

  struct indirect_index_sector iis;
  if(*p_entry == 0) {
    // allocate a sector to be freed, and fill with zero
    free_map_allocate (1, p_entry);
    cache_write (*p_entry, zeros);
  }
  cache_read(*p_entry, &iis);

  size_t unit = (level == 1 ? 1 : INDIRECT_INDEX_SECTOR_NUM);
  size_t l = bytes_to_sectors (num_sectors, unit);

  for (size_t i = 0; i < l; ++ i) {
    size_t per_size = num_sectors < unit ? num_sectors : unit;
    inode_allocate_indirect (& iis.blocks[i], per_size, level - 1);
    num_sectors -= per_size;
  }

  ASSERT (num_sectors == 0);
  cache_write (*p_entry, &iis);
}

static
bool inode_allocate (struct inode_disk *id)
{
  static char zeros[BLOCK_SECTOR_SIZE];

  off_t length = id->length;
	if(length < 0) return false;

	size_t num_sectors = bytes_to_sectors(length, BLOCK_SECTOR_SIZE);

  // direct blocks
	// choose the smaller one as length
  int l = num_sectors < DIRECT_INDEX_NUM ? num_sectors : DIRECT_INDEX_NUM;
  for (int i = 0; i < l; ++ i) {
    if (id->direct_index[i] == 0) {
      free_map_allocate (1, &id->direct_index[i]);
      cache_write (id->direct_index[i], zeros);
    }
  }
  num_sectors -= l;
	// if allocate complete, return immediately
  if(num_sectors == 0) return true;

  // indirect block
  l = num_sectors < INDIRECT_INDEX_SECTOR_NUM ? num_sectors : INDIRECT_INDEX_SECTOR_NUM;
  inode_allocate_indirect (& id->indirect_index, l, 1);
  num_sectors -= l;
  if(num_sectors == 0) return true;

  // doubly indirect block
  l = num_sectors < DOUBLY_INDIRECT_INDEX_SECTOR_NUM ? num_sectors : DOUBLY_INDIRECT_INDEX_SECTOR_NUM;
  inode_allocate_indirect (& id->doubly_indirect_index, l, 2);
  num_sectors -= l;
  if(num_sectors == 0) return true;

  ASSERT (num_sectors == 0);
  return false;
}

static void
inode_deallocate_indirect (block_sector_t entry, size_t num_sectors, int level)
{
  // only supports 2-level indirect block scheme as of now
  ASSERT (level <= 2);

  if (level == 0) {
    free_map_release (entry, 1);
    return;
  }

  struct indirect_index_sector iis;
  cache_read(entry, &iis);

  size_t unit = (level == 1 ? 1 : INDIRECT_INDEX_SECTOR_NUM);
  size_t i, l = bytes_to_sectors (num_sectors, unit);

  for (i = 0; i < l; ++ i) {
    size_t subsize = num_sectors < unit ? num_sectors : unit;
    inode_deallocate_indirect (iis.blocks[i], subsize, level - 1);
    num_sectors -= subsize;
  }

  ASSERT (num_sectors == 0);
  free_map_release (entry, 1);
}

static
bool inode_deallocate (struct inode *inode)
{
  off_t file_length = inode->data.length;
  if(file_length < 0) return false;

  size_t num_sectors = bytes_to_sectors(file_length, BLOCK_SECTOR_SIZE);
  
  // direct blocks
  int l = num_sectors < DIRECT_INDEX_NUM ? num_sectors : DIRECT_INDEX_NUM;
  for (int i = 0; i < l; ++ i) {
    free_map_release (inode->data.direct_index[i], 1);
  }
  num_sectors -= l;

  // indirect block
  l = num_sectors < INDIRECT_INDEX_SECTOR_NUM ? num_sectors : INDIRECT_INDEX_SECTOR_NUM;
  if(l > 0) {
    inode_deallocate_indirect (inode->data.indirect_index, l, 1);
    num_sectors -= l;
  }

  // doubly indirect block
  l = num_sectors < DOUBLY_INDIRECT_INDEX_SECTOR_NUM ? num_sectors : DOUBLY_INDIRECT_INDEX_SECTOR_NUM;
  if(l > 0) {
    inode_deallocate_indirect (inode->data.doubly_indirect_index, l, 2);
    num_sectors -= l;
  }

  ASSERT (num_sectors == 0);
  return true;
}