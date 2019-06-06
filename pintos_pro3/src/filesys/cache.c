#include "string.h"
#include "debug.h"
#include "filesys/cache.h"
#include "filesys/filesys.h"
#include "threads/synch.h"

#define CACHE_SIZE 64

struct cache_entry{
    bool valid; // indicate for valid entry
    block_sector_t disk_sector; 
    uint8_t buffer[BLOCK_SECTOR_SIZE];
    bool dirty; //writen
    bool access; // clock impl
};

static struct cache_entry cache[CACHE_SIZE];
static struct lock cache_lock;

static void
cache_flush(struct cache_entry *ce){
    ASSERT(lock_held_by_current_thread(&cache_lock));
    ASSERT(ce != NULL && ce->valid == 1);
    // flush the cache content into the disk
    if(ce->dirty){
        block_write(fs_device, ce->disk_sector, ce->buffer);
        ce->dirty = 0;
    }
}

static struct cache_entry *
cache_find(block_sector_t sector){
    for(int i=0;i<CACHE_SIZE;i++){
        if(cache[i].valid == false) continue;
        if(cache[i].disk_sector == sector){
            // if find, return immediately
            return &cache[i];
        }
    }
    // not find, return null
    return NULL;
}

static struct cache_entry *
cache_replace_find(void){
    ASSERT(lock_held_by_current_thread(&cache_lock));
    static int clock = 0;
    while(true){
        if(cache[clock].valid == false){
            return &(cache[clock]); 
        }
        // mark flag not equals to zero, give a second chance
        // as the clock replacement algorithm
        if(cache[clock].access != 0){
            cache[clock].access = 0;
        }else{
            // the access equals to 0, break out
            break;
        }
        clock ++;
        clock %= CACHE_SIZE;
    }
    struct cache_entry *ce_replace = &cache[clock];
    if(ce_replace -> dirty){
        cache_flush(ce_replace);
    }
    ce_replace->valid = false;
    return ce_replace;
}

void
cache_init(void){
    lock_init(&cache_lock);
    for(int i=0;i<CACHE_SIZE;i++){
        cache[i].valid = 0;
    }
}

void
cache_close(void){
    lock_acquire(&cache_lock);
    for(int i=0;i<CACHE_SIZE;i++){
        // skip not valid cache parts
        if(cache[i].valid == false){
            continue;
        }
        cache_flush(&cache[i]);
    }
    lock_release(&cache_lock);
}

void 
cache_read(block_sector_t sector, void *target){
    // block_read(fs_device, sector, target);
    lock_acquire(&cache_lock);
    struct cache_entry *ce = cache_find(sector);
    // if cannot find the entry
    if(ce == NULL){
        ce = cache_replace_find();
        ASSERT(ce != NULL && ce->valid == 0);
        ce->valid = 1;
        ce->disk_sector = sector;
        ce->dirty = 0;
        block_read(fs_device, sector, ce->buffer);
    }
    ce->access = 1;
    memcpy(target, ce->buffer, BLOCK_SECTOR_SIZE);
    lock_release(&cache_lock);
}

void
cache_write(block_sector_t sector, void *source){
    // block_write(fs_device, sector, source);
    lock_acquire(&cache_lock);
    struct cache_entry *ce = cache_find(sector);
    // if cannot find the entry
    if(ce == NULL){
        ce = cache_replace_find();
        ASSERT(ce != NULL && ce->valid == 0);
        ce->valid = 1;
        ce->disk_sector = sector;
        ce->dirty = 0;
        block_read(fs_device, sector, ce->buffer);
    }
    ce->access = 1;
    ce->dirty = 1;
    memcpy(ce->buffer, source, BLOCK_SECTOR_SIZE);
    lock_release(&cache_lock);
} 