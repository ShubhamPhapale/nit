#include "vcs.h"

// Write reference
int write_ref(const char *ref_name, const char *sha1) {
    char ref_path[MAX_PATH];
    snprintf(ref_path, sizeof(ref_path), "%s/%s", REFS_HEADS_DIR, ref_name);

    // Create directory if needed
    char dir_path[MAX_PATH];
    strncpy(dir_path, ref_path, sizeof(dir_path));
    char *last_slash = strrchr(dir_path, '/');
    if (last_slash) {
        *last_slash = '\0';
        create_dir_recursive(dir_path);
    }

    FILE *fp = fopen(ref_path, "w");
    if (!fp) {
        perror("fopen ref");
        return -1;
    }

    fprintf(fp, "%s\n", sha1);
    fclose(fp);
    return 0;
}

// Read reference
char *read_ref(const char *ref_name) {
    static char sha1[SHA1_HEX_SIZE + 1];
    char ref_path[MAX_PATH];
    snprintf(ref_path, sizeof(ref_path), "%s/%s", REFS_HEADS_DIR, ref_name);

    FILE *fp = fopen(ref_path, "r");
    if (!fp) {
        return NULL;
    }

    if (fgets(sha1, sizeof(sha1), fp) == NULL) {
        fclose(fp);
        return NULL;
    }

    // Remove newline
    size_t len = strlen(sha1);
    if (len > 0 && sha1[len - 1] == '\n') {
        sha1[len - 1] = '\0';
    }

    fclose(fp);
    return sha1;
}

// Update HEAD
int update_head(const char *ref_or_sha1) {
    FILE *fp = fopen(HEAD_FILE, "w");
    if (!fp) {
        perror("fopen HEAD");
        return -1;
    }

    // Check if it's a branch reference or a direct SHA-1
    if (strlen(ref_or_sha1) == SHA1_HEX_SIZE) {
        // Detached HEAD (direct SHA-1)
        fprintf(fp, "%s\n", ref_or_sha1);
    } else {
        // Branch reference
        fprintf(fp, "ref: refs/heads/%s\n", ref_or_sha1);
    }

    fclose(fp);
    return 0;
}

// Get HEAD commit SHA-1
char *get_head_commit(void) {
    static char sha1[SHA1_HEX_SIZE + 1];
    
    FILE *fp = fopen(HEAD_FILE, "r");
    if (!fp) {
        return NULL;
    }

    char line[MAX_LINE];
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return NULL;
    }
    fclose(fp);

    // Remove newline
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    // Check if HEAD is a reference
    if (strncmp(line, "ref: ", 5) == 0) {
        // Extract branch name
        const char *ref = line + 5;
        const char *branch_name = strrchr(ref, '/');
        if (branch_name) {
            branch_name++;
            char *commit_sha1 = read_ref(branch_name);
            if (commit_sha1) {
                strcpy(sha1, commit_sha1);
                return sha1;
            }
        }
        return NULL;
    } else {
        // HEAD is a direct SHA-1 (detached)
        strcpy(sha1, line);
        return sha1;
    }
}

// Check if HEAD is detached
int is_head_detached(void) {
    FILE *fp = fopen(HEAD_FILE, "r");
    if (!fp) {
        return 0;
    }

    char line[MAX_LINE];
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return 0;
    }
    fclose(fp);

    return strncmp(line, "ref:", 4) != 0;
}

// Get current branch name
char *get_current_branch(void) {
    static char branch[256];
    
    FILE *fp = fopen(HEAD_FILE, "r");
    if (!fp) {
        return NULL;
    }

    char line[MAX_LINE];
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return NULL;
    }
    fclose(fp);

    // Remove newline
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    // Check if HEAD is a reference
    if (strncmp(line, "ref: ", 5) == 0) {
        const char *ref = line + 5;
        const char *branch_name = strrchr(ref, '/');
        if (branch_name) {
            strcpy(branch, branch_name + 1);
            return branch;
        }
    }

    return NULL;
}
