#include "vcs.h"

// Checkout branch
int checkout_branch(const char *branch_name) {
    if (!branch_exists(branch_name)) {
        fprintf(stderr, "Error: Branch '%s' does not exist\n", branch_name);
        return -1;
    }

    char *commit_sha1 = read_ref(branch_name);
    if (!commit_sha1) {
        fprintf(stderr, "Error: Failed to read branch reference\n");
        return -1;
    }

    // Update HEAD to point to branch
    if (update_head(branch_name) != 0) {
        return -1;
    }

    printf("Switched to branch '%s'\n", branch_name);
    return 0;
}

// Checkout commit (detached HEAD)
int checkout_commit(const char *commit_sha1) {
    if (!object_exists(commit_sha1)) {
        fprintf(stderr, "Error: Commit '%s' does not exist\n", commit_sha1);
        return -1;
    }

    // Update HEAD to point directly to commit
    if (update_head(commit_sha1) != 0) {
        return -1;
    }

    printf("HEAD is now at %.*s\n", 7, commit_sha1);
    return 0;
}
