/**
 * Finding Filesystems
 * CS 241 - Spring 2020
 */
#include "minixfs.h"
#include "minixfs_utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Virtual paths:
 *  Add your new virtual endpoint to minixfs_virtual_path_names
 */
char *minixfs_virtual_path_names[] = {"info", /* add your paths here*/};

/**
 * Forward declaring block_info_string so that we can attach unused on it
 * This prevents a compiler warning if you haven't used it yet.
 *
 * This function generates the info string that the virtual endpoint info should
 * emit when read
 */
static char *block_info_string(ssize_t num_used_blocks) __attribute__((unused));
static char *block_info_string(ssize_t num_used_blocks) {
    char *block_string = NULL;
    ssize_t curr_free_blocks = DATA_NUMBER - num_used_blocks;
    asprintf(&block_string, "Free blocks: %zd\n"
                            "Used blocks: %zd\n",
             curr_free_blocks, num_used_blocks);
    return block_string;
}

// Don't modify this line unless you know what you're doing
int minixfs_virtual_path_count =
    sizeof(minixfs_virtual_path_names) / sizeof(minixfs_virtual_path_names[0]);

int minixfs_chmod(file_system *fs, char *path, int new_permissions) {
    // Thar she blows!
    inode* empty_node = get_inode(fs, path);
    if (empty_node < 0) {
        errno = ENOENT;
        return -1;
    }
    empty_node->mode = ((empty_node->mode >> RWX_BITS_NUMBER) << RWX_BITS_NUMBER) | new_permissions;
    clock_gettime(CLOCK_REALTIME, &empty_node->ctim);
    return 0;
}

int minixfs_chown(file_system *fs, char *path, uid_t owner, gid_t group) {
    // Land ahoy!
    inode* curr_node = get_inode(fs, path);
    if (curr_node < 0) {
        errno = ENOENT;
        return -1;
    }
    if (owner != ((uid_t)-1)) {
        curr_node->uid = owner;
    }
    if (group != ((gid_t)-1)) {
        curr_node->gid = group;
    }
    clock_gettime(CLOCK_REALTIME, &curr_node->ctim);
    return 0;
}

inode *minixfs_create_inode_for_path(file_system *fs, const char *path) {
    // Land ahoy!
    if (!valid_filename(path)) {
        return NULL;
    }
    printf("%s\n", path);
    inode* curr_node = get_inode(fs, path);
    if (curr_node != NULL) {
        return NULL;
    }
    inode_number index = first_unused_inode(fs);
    const char* file_name;
    inode* parent = parent_directory(fs, path, &file_name);
    printf("%s\n", file_name);
    if (!valid_filename(file_name)) {
        return NULL;
    }
    init_inode(parent, &(fs->inode_root[index]));
    minixfs_dirent parent_dir; 
    parent_dir.inode_num = index;
    parent_dir.name = strdup(file_name);
    size_t offset = parent->size%sizeof(data_block);
    int inode_index = parent->size/sizeof(data_block);
    if (inode_index < NUM_DIRECT_BLOCKS) {
        make_string_from_dirent(fs->data_root[parent->direct[inode_index]].data+offset, parent_dir);
    } 
    free(parent_dir.name);
    return fs->inode_root+index;
}

ssize_t minixfs_virtual_read(file_system *fs, const char *path, void *buf,
                             size_t count, off_t *off) {
    if (!strcmp(path, "info")) {
        // TODO implement the "info" virtual file here
        char* map = GET_DATA_MAP(fs->meta);
        ssize_t used_block = 0;
        for (size_t i = 0; i < fs->meta->dblock_count; i++) {
            if (map[i] != 0) {
                used_block++;
            }
        }
        char* info_string = block_info_string(used_block);
        if ((unsigned long) *off > strlen(info_string)) {
            return 0;
        }
        if (strlen(info_string) - *off < count) {
            count = strlen(info_string) - *off;
        }
        memcpy(buf, info_string + *off, count);
        *off += count;
        return count;

    }
    // TODO implement your own virtual file here
    errno = ENOENT;
    return -1;
}

ssize_t minixfs_write(file_system *fs, const char *path, const void *buf,
                      size_t count, off_t *off) {
    // X marks the spot
    if (count + *off > sizeof(data_block) * (NUM_INDIRECT_BLOCKS + NUM_DIRECT_BLOCKS)) {
        errno = ENOSPC;
        return -1;
    }
    if (minixfs_min_blockcount(fs, path, (count + *off) / sizeof(data_block) + 1) == -1) {
        errno = ENOSPC;
        return -1;
    } 
    inode* curr_node = get_inode(fs, path);
    if (curr_node == NULL) {
        curr_node = minixfs_create_inode_for_path(fs, path);
    }
    int index_offset = *off%sizeof(data_block);
    int index = *off/sizeof(data_block);
    size_t write_ct = 0;
    data_block_number block_index;
    while (write_ct < count) {
        if (index > NUM_DIRECT_BLOCKS) {
            break;
        }
        block_index = curr_node->direct[index];
        size_t read_len = count - write_ct;
        if (sizeof(data_block) - index_offset < count - write_ct) {
            read_len = sizeof(data_block) - index_offset;
        }
        if (index_offset != 0) {
            memcpy(fs->data_root[block_index].data+index_offset, buf+write_ct, read_len);
        } else {
            memcpy(fs->data_root[block_index].data, buf+write_ct, read_len);
        }
        write_ct += read_len;
        index_offset = 0;
        index++;
    }
    *off += count;
    clock_gettime(CLOCK_REALTIME, &(curr_node->mtim));
    clock_gettime(CLOCK_REALTIME, &(curr_node->atim));
    curr_node->size = *off;
    return count;
}

ssize_t minixfs_read(file_system *fs, const char *path, void *buf, size_t count,
                     off_t *off) {
    // 'ere be treasure!
    const char *virtual_path = is_virtual_path(path);
    if (virtual_path)
        return minixfs_virtual_read(fs, virtual_path, buf, count, off);
    inode* curr_node = get_inode(fs, path);
    if (curr_node == NULL) {
        errno = ENOENT;
        return -1;
    }
    if (count > curr_node->size - *off) {
        count = curr_node->size - *off;
    }
    int index_offset = *off%sizeof(data_block);
    int index = *off/sizeof(data_block);
    size_t read_ct = 0;
    data_block_number block_index;
    while (read_ct < count) {
        if (index > NUM_DIRECT_BLOCKS) {
            break;
        }
        block_index = curr_node->direct[index];
        size_t read_len = count - read_ct;
        if (sizeof(data_block) - index_offset < count - read_ct) {
            read_len = sizeof(data_block) - index_offset;
        }
        if (index_offset != 0) {
            memcpy(buf+read_ct, fs->data_root[block_index].data+index_offset, read_len);
        } else {
            memcpy(buf+read_ct, fs->data_root[block_index].data, read_len);
        }
        read_ct += read_len;
        index_offset = 0;
        index++;
    }
    *off += read_ct;
    clock_gettime(CLOCK_REALTIME, &(curr_node->atim));
    return read_ct;
    // return -1;
}
