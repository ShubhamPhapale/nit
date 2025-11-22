#include "vcs.h"

// Find common ancestor (merge base) - simplified version
char *find_merge_base(const char *commit1_sha1, const char *commit2_sha1) {
    static char base_sha1[SHA1_HEX_SIZE + 1];
    
    // For simplicity, we'll trace back commit1's history and check if commit2 is an ancestor
    char current[SHA1_HEX_SIZE + 1];
    strcpy(current, commit1_sha1);

    while (strlen(current) > 0) {
        if (strcmp(current, commit2_sha1) == 0) {
            strcpy(base_sha1, current);
            return base_sha1;
        }

        Commit *commit = read_commit(current);
        if (!commit || strlen(commit->parent_sha1) == 0) {
            commit_free(commit);
            break;
        }

        strcpy(current, commit->parent_sha1);
        commit_free(commit);
    }

    // No common ancestor found (in this simplified version)
    return NULL;
}

// Merge branch into current branch
int merge_branch(const char *branch_name) {
    if (!is_vcs_repo()) {
        fprintf(stderr, "Error: Not a VCS repository\n");
        return -1;
    }

    if (!branch_exists(branch_name)) {
        fprintf(stderr, "Error: Branch '%s' does not exist\n", branch_name);
        return -1;
    }

    char *current_branch = get_current_branch();
    if (!current_branch) {
        fprintf(stderr, "Error: Cannot merge with detached HEAD\n");
        return -1;
    }

    if (strcmp(current_branch, branch_name) == 0) {
        fprintf(stderr, "Error: Cannot merge branch with itself\n");
        return -1;
    }

    // Get current commit
    char *current_commit = get_head_commit();
    if (!current_commit) {
        fprintf(stderr, "Error: No commits on current branch\n");
        return -1;
    }

    // Get merge commit
    char *merge_commit_sha1 = read_ref(branch_name);
    if (!merge_commit_sha1) {
        fprintf(stderr, "Error: Failed to read branch reference\n");
        return -1;
    }

    // Check if already up to date
    if (strcmp(current_commit, merge_commit_sha1) == 0) {
        printf("Already up to date.\n");
        return 0;
    }

    // Find merge base
    char *base_sha1 = find_merge_base(current_commit, merge_commit_sha1);
    
    // Fast-forward merge if possible
    if (base_sha1 && strcmp(base_sha1, current_commit) == 0) {
        printf("Fast-forward merge\n");
        if (write_ref(current_branch, merge_commit_sha1) != 0) {
            return -1;
        }
        printf("Merged branch '%s' into '%s'\n", branch_name, current_branch);
        return 0;
    }

    // Three-way merge (simplified - create merge commit)
    printf("Performing three-way merge\n");

    // Load current index
    Index *idx = index_new();
    if (!idx) {
        return -1;
    }
    index_load(idx);

    // Create tree from index
    Tree *tree = tree_new();
    if (!tree) {
        index_free(idx);
        return -1;
    }

    if (tree_from_index(idx, tree) != 0) {
        tree_free(tree);
        index_free(idx);
        return -1;
    }

    char tree_sha1[SHA1_HEX_SIZE + 1];
    if (write_tree(tree, tree_sha1) != 0) {
        tree_free(tree);
        index_free(idx);
        return -1;
    }

    tree_free(tree);
    index_free(idx);

    // Create merge commit
    Commit *commit = commit_new();
    if (!commit) {
        return -1;
    }

    strcpy(commit->tree_sha1, tree_sha1);
    strcpy(commit->parent_sha1, current_commit);
    
    char *user = get_user_info();
    strncpy(commit->author, user, sizeof(commit->author) - 1);
    strncpy(commit->committer, user, sizeof(commit->committer) - 1);
    commit->timestamp = time(NULL);
    
    snprintf(commit->message, sizeof(commit->message),
             "Merge branch '%s' into %s", branch_name, current_branch);

    char commit_sha1[SHA1_HEX_SIZE + 1];
    if (write_commit(commit, commit_sha1) != 0) {
        commit_free(commit);
        return -1;
    }

    commit_free(commit);

    // Update current branch reference
    if (write_ref(current_branch, commit_sha1) != 0) {
        return -1;
    }

    printf("Merged branch '%s' into '%s'\n", branch_name, current_branch);
    printf("Merge commit: %s\n", commit_sha1);
    return 0;
}
