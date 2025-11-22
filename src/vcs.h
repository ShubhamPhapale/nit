#ifndef VCS_H
#define VCS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

// Version information
#define NIT_VERSION "1.0.0"
#define NIT_VERSION_MAJOR 1
#define NIT_VERSION_MINOR 0
#define NIT_VERSION_PATCH 0

// Constants
#define VCS_DIR ".vcs"
#define OBJECTS_DIR ".vcs/objects"
#define REFS_DIR ".vcs/refs"
#define REFS_HEADS_DIR ".vcs/refs/heads"
#define HEAD_FILE ".vcs/HEAD"
#define INDEX_FILE ".vcs/index"
#define CONFIG_FILE ".vcs/config"
#define SHA1_HEX_SIZE 40
#define SHA1_SIZE 20
#define MAX_PATH 4096
#define MAX_LINE 8192

// Object types
typedef enum {
    OBJ_BLOB,
    OBJ_TREE,
    OBJ_COMMIT
} ObjectType;

// Index entry structure
typedef struct {
    char sha1[SHA1_HEX_SIZE + 1];
    char path[MAX_PATH];
    time_t mtime;
    size_t size;
} IndexEntry;

// Index structure
typedef struct {
    IndexEntry *entries;
    size_t count;
    size_t capacity;
} Index;

// Tree entry structure
typedef struct {
    char mode[10];
    char type[10];
    char sha1[SHA1_HEX_SIZE + 1];
    char name[256];
} TreeEntry;

// Tree structure
typedef struct {
    TreeEntry *entries;
    size_t count;
    size_t capacity;
} Tree;

// Commit structure
typedef struct {
    char tree_sha1[SHA1_HEX_SIZE + 1];
    char parent_sha1[SHA1_HEX_SIZE + 1];
    char author[256];
    char committer[256];
    time_t timestamp;
    char message[1024];
} Commit;

// Branch structure
typedef struct {
    char name[256];
    char commit_sha1[SHA1_HEX_SIZE + 1];
} Branch;

// Function declarations

// Utility functions
void compute_sha1(const void *data, size_t len, unsigned char *hash);
void sha1_to_hex(const unsigned char *sha1, char *hex);
void hex_to_sha1(const char *hex, unsigned char *sha1);
int file_exists(const char *path);
int dir_exists(const char *path);
int create_dir(const char *path);
int create_dir_recursive(const char *path);
char *read_file(const char *path, size_t *size);
int write_file(const char *path, const void *data, size_t size);
void get_current_time(char *buffer, size_t size);
char *get_user_info(void);

// Repository functions
int vcs_init(void);
int is_vcs_repo(void);
char *get_vcs_root(void);

// Object functions
int write_object(const void *data, size_t size, ObjectType type, char *sha1_out);
void *read_object(const char *sha1, size_t *size, ObjectType *type);
int object_exists(const char *sha1);
char *get_object_path(const char *sha1);

// Index functions
Index *index_new(void);
void index_free(Index *idx);
int index_load(Index *idx);
int index_save(Index *idx);
int index_add_entry(Index *idx, const char *path, const char *sha1, time_t mtime, size_t size);
int index_remove_entry(Index *idx, const char *path);
IndexEntry *index_find_entry(Index *idx, const char *path);

// Tree functions
Tree *tree_new(void);
void tree_free(Tree *tree);
int tree_add_entry(Tree *tree, const char *mode, const char *type, const char *sha1, const char *name);
int tree_from_index(Index *idx, Tree *tree);
int write_tree(Tree *tree, char *sha1_out);
Tree *read_tree(const char *sha1);

// Commit functions
Commit *commit_new(void);
void commit_free(Commit *commit);
int write_commit(Commit *commit, char *sha1_out);
Commit *read_commit(const char *sha1);

// Reference functions
int write_ref(const char *ref_name, const char *sha1);
char *read_ref(const char *ref_name);
int update_head(const char *ref_or_sha1);
char *get_head_commit(void);
int is_head_detached(void);
char *get_current_branch(void);

// Branch functions
int create_branch(const char *branch_name, const char *commit_sha1);
int delete_branch(const char *branch_name);
int list_branches(void);
int branch_exists(const char *branch_name);

// Working directory functions
int add_file(const char *path);
int add_all(void);
int vcs_status(void);
int vcs_log(int limit);
int vcs_diff(const char *commit_sha1);

// Checkout functions
int checkout_branch(const char *branch_name);
int checkout_commit(const char *commit_sha1);

// Merge functions
int merge_branch(const char *branch_name);
char *find_merge_base(const char *commit1_sha1, const char *commit2_sha1);

// Compression functions
int compress_data(const void *src, size_t src_len, void **dst, size_t *dst_len);
int decompress_data(const void *src, size_t src_len, void **dst, size_t *dst_len);

#endif // VCS_H
