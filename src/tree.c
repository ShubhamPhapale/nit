#include "vcs.h"

// Create new tree
Tree *tree_new(void) {
    Tree *tree = malloc(sizeof(Tree));
    if (!tree) {
        return NULL;
    }
    tree->entries = malloc(sizeof(TreeEntry) * 16);
    if (!tree->entries) {
        free(tree);
        return NULL;
    }
    tree->count = 0;
    tree->capacity = 16;
    return tree;
}

// Free tree
void tree_free(Tree *tree) {
    if (tree) {
        free(tree->entries);
        free(tree);
    }
}

// Add entry to tree
int tree_add_entry(Tree *tree, const char *mode, const char *type, 
                   const char *sha1, const char *name) {
    if (tree->count >= tree->capacity) {
        tree->capacity *= 2;
        TreeEntry *new_entries = realloc(tree->entries,
                                         sizeof(TreeEntry) * tree->capacity);
        if (!new_entries) {
            return -1;
        }
        tree->entries = new_entries;
    }

    TreeEntry *entry = &tree->entries[tree->count++];
    strncpy(entry->mode, mode, sizeof(entry->mode) - 1);
    entry->mode[sizeof(entry->mode) - 1] = '\0';
    strncpy(entry->type, type, sizeof(entry->type) - 1);
    entry->type[sizeof(entry->type) - 1] = '\0';
    strncpy(entry->sha1, sha1, SHA1_HEX_SIZE);
    entry->sha1[SHA1_HEX_SIZE] = '\0';
    strncpy(entry->name, name, sizeof(entry->name) - 1);
    entry->name[sizeof(entry->name) - 1] = '\0';

    return 0;
}

// Compare function for sorting tree entries
static int tree_entry_cmp(const void *a, const void *b) {
    const TreeEntry *ea = (const TreeEntry *)a;
    const TreeEntry *eb = (const TreeEntry *)b;
    return strcmp(ea->name, eb->name);
}

// Build tree from index
int tree_from_index(Index *idx, Tree *tree) {
    tree->count = 0;

    for (size_t i = 0; i < idx->count; i++) {
        IndexEntry *entry = &idx->entries[i];
        if (tree_add_entry(tree, "100644", "blob", entry->sha1, entry->path) != 0) {
            return -1;
        }
    }

    // Sort entries
    qsort(tree->entries, tree->count, sizeof(TreeEntry), tree_entry_cmp);
    return 0;
}

// Write tree object
int write_tree(Tree *tree, char *sha1_out) {
    // Calculate total size
    size_t total_size = 0;
    for (size_t i = 0; i < tree->count; i++) {
        TreeEntry *entry = &tree->entries[i];
        total_size += strlen(entry->mode) + 1 + strlen(entry->name) + 1 + SHA1_SIZE;
    }

    // Build tree data
    unsigned char *data = malloc(total_size);
    if (!data) {
        return -1;
    }

    unsigned char *ptr = data;
    for (size_t i = 0; i < tree->count; i++) {
        TreeEntry *entry = &tree->entries[i];
        int len = sprintf((char *)ptr, "%s %s", entry->mode, entry->name);
        ptr += len + 1;

        unsigned char sha1[SHA1_SIZE];
        hex_to_sha1(entry->sha1, sha1);
        memcpy(ptr, sha1, SHA1_SIZE);
        ptr += SHA1_SIZE;
    }

    int ret = write_object(data, total_size, OBJ_TREE, sha1_out);
    free(data);
    return ret;
}

// Read tree object
Tree *read_tree(const char *sha1) {
    size_t size;
    ObjectType type;
    unsigned char *data = read_object(sha1, &size, &type);

    if (!data || type != OBJ_TREE) {
        free(data);
        return NULL;
    }

    Tree *tree = tree_new();
    if (!tree) {
        free(data);
        return NULL;
    }

    unsigned char *ptr = data;
    unsigned char *end = data + size;

    while (ptr < end) {
        TreeEntry entry;
        
        // Parse mode
        char *space = strchr((char *)ptr, ' ');
        if (!space) break;
        *space = '\0';
        strncpy(entry.mode, (char *)ptr, sizeof(entry.mode) - 1);
        entry.mode[sizeof(entry.mode) - 1] = '\0';
        ptr = (unsigned char *)(space + 1);

        // Parse name
        char *null = strchr((char *)ptr, '\0');
        if (!null) break;
        strncpy(entry.name, (char *)ptr, sizeof(entry.name) - 1);
        entry.name[sizeof(entry.name) - 1] = '\0';
        ptr = (unsigned char *)(null + 1);

        // Parse SHA-1
        if (ptr + SHA1_SIZE > end) break;
        sha1_to_hex(ptr, entry.sha1);
        ptr += SHA1_SIZE;

        // Determine type (blob for files)
        strcpy(entry.type, "blob");

        if (tree->count >= tree->capacity) {
            tree->capacity *= 2;
            TreeEntry *new_entries = realloc(tree->entries,
                                             sizeof(TreeEntry) * tree->capacity);
            if (!new_entries) {
                tree_free(tree);
                free(data);
                return NULL;
            }
            tree->entries = new_entries;
        }

        tree->entries[tree->count++] = entry;
    }

    free(data);
    return tree;
}
