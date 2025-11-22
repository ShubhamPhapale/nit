#include "vcs.h"

// Create new index
Index *index_new(void) {
    Index *idx = malloc(sizeof(Index));
    if (!idx) {
        return NULL;
    }
    idx->entries = malloc(sizeof(IndexEntry) * 16);
    if (!idx->entries) {
        free(idx);
        return NULL;
    }
    idx->count = 0;
    idx->capacity = 16;
    return idx;
}

// Free index
void index_free(Index *idx) {
    if (idx) {
        free(idx->entries);
        free(idx);
    }
}

// Load index from disk
int index_load(Index *idx) {
    FILE *fp = fopen(INDEX_FILE, "r");
    if (!fp) {
        idx->count = 0;
        return 0; // Empty index is OK
    }

    char line[MAX_LINE];
    idx->count = 0;

    while (fgets(line, sizeof(line), fp)) {
        IndexEntry entry;
        
        if (sscanf(line, "%40s %ld %zu %[^\n]", 
                   entry.sha1, &entry.mtime, &entry.size, entry.path) == 4) {
            
            if (idx->count >= idx->capacity) {
                idx->capacity *= 2;
                IndexEntry *new_entries = realloc(idx->entries, 
                                                   sizeof(IndexEntry) * idx->capacity);
                if (!new_entries) {
                    fclose(fp);
                    return -1;
                }
                idx->entries = new_entries;
            }
            
            idx->entries[idx->count++] = entry;
        }
    }

    fclose(fp);
    return 0;
}

// Save index to disk
int index_save(Index *idx) {
    FILE *fp = fopen(INDEX_FILE, "w");
    if (!fp) {
        perror("fopen INDEX");
        return -1;
    }

    for (size_t i = 0; i < idx->count; i++) {
        fprintf(fp, "%s %ld %zu %s\n",
                idx->entries[i].sha1,
                idx->entries[i].mtime,
                idx->entries[i].size,
                idx->entries[i].path);
    }

    fclose(fp);
    return 0;
}

// Add entry to index
int index_add_entry(Index *idx, const char *path, const char *sha1, 
                    time_t mtime, size_t size) {
    // Check if entry already exists
    for (size_t i = 0; i < idx->count; i++) {
        if (strcmp(idx->entries[i].path, path) == 0) {
            // Update existing entry
            strncpy(idx->entries[i].sha1, sha1, SHA1_HEX_SIZE);
            idx->entries[i].sha1[SHA1_HEX_SIZE] = '\0';
            idx->entries[i].mtime = mtime;
            idx->entries[i].size = size;
            return 0;
        }
    }

    // Add new entry
    if (idx->count >= idx->capacity) {
        idx->capacity *= 2;
        IndexEntry *new_entries = realloc(idx->entries, 
                                          sizeof(IndexEntry) * idx->capacity);
        if (!new_entries) {
            return -1;
        }
        idx->entries = new_entries;
    }

    IndexEntry *entry = &idx->entries[idx->count++];
    strncpy(entry->sha1, sha1, SHA1_HEX_SIZE);
    entry->sha1[SHA1_HEX_SIZE] = '\0';
    strncpy(entry->path, path, MAX_PATH - 1);
    entry->path[MAX_PATH - 1] = '\0';
    entry->mtime = mtime;
    entry->size = size;

    return 0;
}

// Remove entry from index
int index_remove_entry(Index *idx, const char *path) {
    for (size_t i = 0; i < idx->count; i++) {
        if (strcmp(idx->entries[i].path, path) == 0) {
            // Shift remaining entries
            memmove(&idx->entries[i], &idx->entries[i + 1],
                    sizeof(IndexEntry) * (idx->count - i - 1));
            idx->count--;
            return 0;
        }
    }
    return -1;
}

// Find entry in index
IndexEntry *index_find_entry(Index *idx, const char *path) {
    for (size_t i = 0; i < idx->count; i++) {
        if (strcmp(idx->entries[i].path, path) == 0) {
            return &idx->entries[i];
        }
    }
    return NULL;
}
