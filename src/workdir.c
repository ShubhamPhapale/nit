#include "vcs.h"

// Add file to staging area
int add_file(const char *path) {
    if (!file_exists(path)) {
        fprintf(stderr, "Error: File '%s' does not exist\n", path);
        return -1;
    }

    // Read file content
    size_t size;
    char *content = read_file(path, &size);
    if (!content) {
        fprintf(stderr, "Error: Failed to read file '%s'\n", path);
        return -1;
    }

    // Write blob object
    char sha1[SHA1_HEX_SIZE + 1];
    if (write_object(content, size, OBJ_BLOB, sha1) != 0) {
        free(content);
        fprintf(stderr, "Error: Failed to write object\n");
        return -1;
    }
    free(content);

    // Get file stats
    struct stat st;
    if (stat(path, &st) != 0) {
        perror("stat");
        return -1;
    }

    // Load index, add entry, save
    Index *idx = index_new();
    if (!idx) {
        fprintf(stderr, "Error: Failed to create index\n");
        return -1;
    }

    if (index_load(idx) != 0) {
        index_free(idx);
        return -1;
    }

    if (index_add_entry(idx, path, sha1, st.st_mtime, st.st_size) != 0) {
        index_free(idx);
        fprintf(stderr, "Error: Failed to add entry to index\n");
        return -1;
    }

    if (index_save(idx) != 0) {
        index_free(idx);
        return -1;
    }

    index_free(idx);
    printf("Added '%s'\n", path);
    return 0;
}

// Add all files in current directory
int add_all(void) {
    DIR *dir = opendir(".");
    if (!dir) {
        perror("opendir");
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files and VCS directory
        if (entry->d_name[0] == '.' || strcmp(entry->d_name, VCS_DIR) == 0) {
            continue;
        }

        struct stat st;
        if (stat(entry->d_name, &st) == 0 && S_ISREG(st.st_mode)) {
            add_file(entry->d_name);
        }
    }

    closedir(dir);
    return 0;
}

// Show repository status
int vcs_status(void) {
    if (!is_vcs_repo()) {
        fprintf(stderr, "Error: Not a VCS repository\n");
        return -1;
    }

    // Get current branch
    char *branch = get_current_branch();
    if (branch) {
        printf("On branch %s\n", branch);
    } else if (is_head_detached()) {
        char *head = get_head_commit();
        printf("HEAD detached at %.*s\n", 7, head ? head : "unknown");
    } else {
        printf("On branch master (no commits yet)\n");
    }

    printf("\n");

    // Load index
    Index *idx = index_new();
    if (!idx) {
        return -1;
    }
    index_load(idx);

    if (idx->count > 0) {
        printf("Changes to be committed:\n");
        for (size_t i = 0; i < idx->count; i++) {
            printf("  modified:   %s\n", idx->entries[i].path);
        }
        printf("\n");
    } else {
        printf("No changes staged for commit\n\n");
    }

    // Check for untracked files
    DIR *dir = opendir(".");
    if (dir) {
        int has_untracked = 0;
        struct dirent *entry;
        
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] == '.' || strcmp(entry->d_name, VCS_DIR) == 0) {
                continue;
            }

            struct stat st;
            if (stat(entry->d_name, &st) == 0 && S_ISREG(st.st_mode)) {
                if (!index_find_entry(idx, entry->d_name)) {
                    if (!has_untracked) {
                        printf("Untracked files:\n");
                        has_untracked = 1;
                    }
                    printf("  %s\n", entry->d_name);
                }
            }
        }
        closedir(dir);
        
        if (has_untracked) {
            printf("\n");
        }
    }

    index_free(idx);
    return 0;
}

// Show commit log
int vcs_log(int limit) {
    if (!is_vcs_repo()) {
        fprintf(stderr, "Error: Not a VCS repository\n");
        return -1;
    }

    char *head_sha1 = get_head_commit();
    if (!head_sha1) {
        printf("No commits yet\n");
        return 0;
    }

    char current_sha1[SHA1_HEX_SIZE + 1];
    strcpy(current_sha1, head_sha1);

    int count = 0;
    while (strlen(current_sha1) > 0 && (limit == 0 || count < limit)) {
        Commit *commit = read_commit(current_sha1);
        if (!commit) {
            break;
        }

        printf("commit %s\n", current_sha1);
        printf("Author: %s\n", commit->author);
        
        char time_buf[64];
        struct tm *tm_info = localtime(&commit->timestamp);
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);
        printf("Date:   %s\n", time_buf);
        printf("\n    %s\n\n", commit->message);

        strcpy(current_sha1, commit->parent_sha1);
        commit_free(commit);
        count++;
    }

    return 0;
}

// Show diff (simplified version)
int vcs_diff(const char *commit_sha1) {
    if (!is_vcs_repo()) {
        fprintf(stderr, "Error: Not a VCS repository\n");
        return -1;
    }

    printf("Diff functionality - comparing with commit %s\n", 
           commit_sha1 ? commit_sha1 : "HEAD");
    
    // Load index
    Index *idx = index_new();
    if (!idx) {
        return -1;
    }
    index_load(idx);

    for (size_t i = 0; i < idx->count; i++) {
        printf("File: %s (SHA-1: %s)\n", idx->entries[i].path, idx->entries[i].sha1);
    }

    index_free(idx);
    return 0;
}
