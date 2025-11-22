#include "vcs.h"

// Initialize VCS repository
int vcs_init(void) {
    if (dir_exists(VCS_DIR)) {
        fprintf(stderr, "Error: Repository already initialized\n");
        return -1;
    }

    // Create directory structure
    if (create_dir(VCS_DIR) != 0 ||
        create_dir(OBJECTS_DIR) != 0 ||
        create_dir(REFS_DIR) != 0 ||
        create_dir(REFS_HEADS_DIR) != 0) {
        fprintf(stderr, "Error: Failed to create repository structure\n");
        return -1;
    }

    // Create initial HEAD (pointing to master branch)
    FILE *fp = fopen(HEAD_FILE, "w");
    if (!fp) {
        perror("fopen HEAD");
        return -1;
    }
    fprintf(fp, "ref: refs/heads/master\n");
    fclose(fp);

    // Create empty index
    fp = fopen(INDEX_FILE, "w");
    if (!fp) {
        perror("fopen INDEX");
        return -1;
    }
    fclose(fp);

    // Create config file
    fp = fopen(CONFIG_FILE, "w");
    if (!fp) {
        perror("fopen CONFIG");
        return -1;
    }
    fprintf(fp, "[core]\n");
    fprintf(fp, "\trepositoryformatversion = 0\n");
    fprintf(fp, "\tfilemode = true\n");
    fclose(fp);

    printf("Initialized empty VCS repository in %s/%s/\n", getcwd(NULL, 0), VCS_DIR);
    return 0;
}

// Check if current directory is a VCS repository
int is_vcs_repo(void) {
    return dir_exists(VCS_DIR) && file_exists(HEAD_FILE);
}

// Get VCS root directory
char *get_vcs_root(void) {
    static char root[MAX_PATH];
    char cwd[MAX_PATH];
    
    if (!getcwd(cwd, sizeof(cwd))) {
        return NULL;
    }

    strcpy(root, cwd);
    
    while (1) {
        char vcs_path[MAX_PATH];
        snprintf(vcs_path, sizeof(vcs_path), "%s/%s", root, VCS_DIR);
        
        if (dir_exists(vcs_path)) {
            return root;
        }

        char *last_slash = strrchr(root, '/');
        if (!last_slash || last_slash == root) {
            break;
        }
        *last_slash = '\0';
    }

    return NULL;
}
