#ifndef FILESYS_CACHE_H
#define FILESYS_CACHE_H

#include "devices/block.h"

void cache_init(void);
void cache_close(void);

void cache_read(block_sector_t sector, void *target);
void cache_write(block_sector_t sector, void *source);

// static void cache_flush(struct cache_entry *ce);
// static struct cache_entry * cache_find(block_sector_t sector);
// static struct cache_entry * cache_replace_find(void);

#endif