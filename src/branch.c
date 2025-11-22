#include "vcs.h"

// Create branch
int create_branch(const char *branch_name, const char *commit_sha1) {
    if (branch_exists(branch_name)) {
        fprintf(stderr, "Error: Branch '%s' already exists\n", branch_name);
        return -1;
    }

    return write_ref(branch_name, commit_sha1);
}

// Delete branch
int delete_branch(const char *branch_name) {
    char *current = get_current_branch();
    if (current && strcmp(current, branch_name) == 0) {
        fprintf(stderr, "Error: Cannot delete current branch '%s'\n", branch_name);
        return -1;
    }

    char ref_path[MAX_PATH];
    snprintf(ref_path, sizeof(ref_path), "%s/%s", REFS_HEADS_DIR, branch_name);

    if (unlink(ref_path) != 0) {
        perror("unlink");
        return -1;
    }

    return 0;
}

// List all branches
int list_branches(void) {
    DIR *dir = opendir(REFS_HEADS_DIR);
    if (!dir) {
        perror("opendir");
        return -1;
    }

    char *current = get_current_branch();
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }

        if (current && strcmp(entry->d_name, current) == 0) {
            printf("* %s\n", entry->d_name);
        } else {
            printf("  %s\n", entry->d_name);
        }
    }

    closedir(dir);
    return 0;
}

// Check if branch exists
int branch_exists(const char *branch_name) {
    char ref_path[MAX_PATH];
    snprintf(ref_path, sizeof(ref_path), "%s/%s", REFS_HEADS_DIR, branch_name);
    return file_exists(ref_path);
}
