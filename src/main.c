#include "vcs.h"

// Function declarations for command handlers
static int cmd_init(int argc, char *argv[]);
static int cmd_add(int argc, char *argv[]);
static int cmd_commit(int argc, char *argv[]);
static int cmd_status(int argc, char *argv[]);
static int cmd_log(int argc, char *argv[]);
static int cmd_branch(int argc, char *argv[]);
static int cmd_checkout(int argc, char *argv[]);
static int cmd_merge(int argc, char *argv[]);
static int cmd_diff(int argc, char *argv[]);
static void print_usage(void);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    const char *command = argv[1];

    if (strcmp(command, "init") == 0) {
        return cmd_init(argc - 1, argv + 1);
    } else if (strcmp(command, "add") == 0) {
        return cmd_add(argc - 1, argv + 1);
    } else if (strcmp(command, "commit") == 0) {
        return cmd_commit(argc - 1, argv + 1);
    } else if (strcmp(command, "status") == 0) {
        return cmd_status(argc - 1, argv + 1);
    } else if (strcmp(command, "log") == 0) {
        return cmd_log(argc - 1, argv + 1);
    } else if (strcmp(command, "branch") == 0) {
        return cmd_branch(argc - 1, argv + 1);
    } else if (strcmp(command, "checkout") == 0) {
        return cmd_checkout(argc - 1, argv + 1);
    } else if (strcmp(command, "merge") == 0) {
        return cmd_merge(argc - 1, argv + 1);
    } else if (strcmp(command, "diff") == 0) {
        return cmd_diff(argc - 1, argv + 1);
    } else {
        fprintf(stderr, "Error: Unknown command '%s'\n", command);
        print_usage();
        return 1;
    }
}

static int cmd_init(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    return vcs_init();
}

static int cmd_add(int argc, char *argv[]) {
    if (!is_vcs_repo()) {
        fprintf(stderr, "Error: Not a VCS repository (or any parent directory)\n");
        return 1;
    }

    if (argc < 2) {
        fprintf(stderr, "Usage: vcs add <file> | vcs add .\n");
        return 1;
    }

    if (strcmp(argv[1], ".") == 0) {
        return add_all();
    } else {
        for (int i = 1; i < argc; i++) {
            if (add_file(argv[i]) != 0) {
                return 1;
            }
        }
        return 0;
    }
}

static int cmd_commit(int argc, char *argv[]) {
    if (!is_vcs_repo()) {
        fprintf(stderr, "Error: Not a VCS repository\n");
        return 1;
    }

    if (argc < 3 || strcmp(argv[1], "-m") != 0) {
        fprintf(stderr, "Usage: vcs commit -m \"message\"\n");
        return 1;
    }

    const char *message = argv[2];

    // Load index
    Index *idx = index_new();
    if (!idx) {
        fprintf(stderr, "Error: Failed to create index\n");
        return 1;
    }

    if (index_load(idx) != 0) {
        index_free(idx);
        return 1;
    }

    if (idx->count == 0) {
        fprintf(stderr, "Error: No changes to commit\n");
        index_free(idx);
        return 1;
    }

    // Create tree from index
    Tree *tree = tree_new();
    if (!tree) {
        index_free(idx);
        return 1;
    }

    if (tree_from_index(idx, tree) != 0) {
        tree_free(tree);
        index_free(idx);
        return 1;
    }

    char tree_sha1[SHA1_HEX_SIZE + 1];
    if (write_tree(tree, tree_sha1) != 0) {
        tree_free(tree);
        index_free(idx);
        return 1;
    }

    tree_free(tree);
    index_free(idx);

    // Create commit object
    Commit *commit = commit_new();
    if (!commit) {
        return 1;
    }

    strcpy(commit->tree_sha1, tree_sha1);
    
    char *head_commit = get_head_commit();
    if (head_commit) {
        strcpy(commit->parent_sha1, head_commit);
    } else {
        commit->parent_sha1[0] = '\0';
    }

    char *user = get_user_info();
    strncpy(commit->author, user, sizeof(commit->author) - 1);
    commit->author[sizeof(commit->author) - 1] = '\0';
    strncpy(commit->committer, user, sizeof(commit->committer) - 1);
    commit->committer[sizeof(commit->committer) - 1] = '\0';
    commit->timestamp = time(NULL);
    strncpy(commit->message, message, sizeof(commit->message) - 1);
    commit->message[sizeof(commit->message) - 1] = '\0';

    char commit_sha1[SHA1_HEX_SIZE + 1];
    if (write_commit(commit, commit_sha1) != 0) {
        commit_free(commit);
        return 1;
    }

    commit_free(commit);

    // Update branch reference or HEAD
    char *current_branch = get_current_branch();
    if (current_branch) {
        if (write_ref(current_branch, commit_sha1) != 0) {
            return 1;
        }
        printf("[%s %.*s] %s\n", current_branch, 7, commit_sha1, message);
    } else {
        if (update_head(commit_sha1) != 0) {
            return 1;
        }
        printf("[detached HEAD %.*s] %s\n", 7, commit_sha1, message);
    }

    return 0;
}

static int cmd_status(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    return vcs_status();
}

static int cmd_log(int argc, char *argv[]) {
    int limit = 0;
    
    if (argc >= 3 && strcmp(argv[1], "-n") == 0) {
        limit = atoi(argv[2]);
    }
    
    return vcs_log(limit);
}

static int cmd_branch(int argc, char *argv[]) {
    if (!is_vcs_repo()) {
        fprintf(stderr, "Error: Not a VCS repository\n");
        return 1;
    }

    if (argc == 1) {
        // List branches
        return list_branches();
    } else if (argc == 2) {
        // Create branch
        char *head_commit = get_head_commit();
        if (!head_commit) {
            fprintf(stderr, "Error: No commits yet\n");
            return 1;
        }
        
        if (create_branch(argv[1], head_commit) == 0) {
            printf("Created branch '%s'\n", argv[1]);
            return 0;
        }
        return 1;
    } else if (argc == 3 && strcmp(argv[1], "-d") == 0) {
        // Delete branch
        if (delete_branch(argv[2]) == 0) {
            printf("Deleted branch '%s'\n", argv[2]);
            return 0;
        }
        return 1;
    } else {
        fprintf(stderr, "Usage: vcs branch [<name>] [-d <name>]\n");
        return 1;
    }
}

static int cmd_checkout(int argc, char *argv[]) {
    if (!is_vcs_repo()) {
        fprintf(stderr, "Error: Not a VCS repository\n");
        return 1;
    }

    if (argc < 2) {
        fprintf(stderr, "Usage: vcs checkout <branch|commit>\n");
        return 1;
    }

    const char *target = argv[1];

    // Check if it's a branch
    if (branch_exists(target)) {
        return checkout_branch(target);
    }

    // Check if it's a commit SHA-1
    if (strlen(target) >= 7 && strlen(target) <= SHA1_HEX_SIZE) {
        // Try to find matching commit (simplified)
        if (object_exists(target)) {
            return checkout_commit(target);
        }
    }

    fprintf(stderr, "Error: Branch or commit '%s' not found\n", target);
    return 1;
}

static int cmd_merge(int argc, char *argv[]) {
    if (!is_vcs_repo()) {
        fprintf(stderr, "Error: Not a VCS repository\n");
        return 1;
    }

    if (argc < 2) {
        fprintf(stderr, "Usage: vcs merge <branch>\n");
        return 1;
    }

    return merge_branch(argv[1]);
}

static int cmd_diff(int argc, char *argv[]) {
    if (!is_vcs_repo()) {
        fprintf(stderr, "Error: Not a VCS repository\n");
        return 1;
    }

    const char *commit = NULL;
    if (argc >= 2) {
        commit = argv[1];
    }

    return vcs_diff(commit);
}

static void print_usage(void) {
    printf("VCS - Version Control System\n\n");
    printf("Usage: vcs <command> [<args>]\n\n");
    printf("Commands:\n");
    printf("  init                Initialize a new repository\n");
    printf("  add <file>          Add file to staging area\n");
    printf("  add .               Add all files to staging area\n");
    printf("  commit -m <msg>     Create a new commit\n");
    printf("  status              Show working tree status\n");
    printf("  log [-n <num>]      Show commit logs\n");
    printf("  branch [<name>]     List or create branches\n");
    printf("  branch -d <name>    Delete a branch\n");
    printf("  checkout <branch>   Switch to a branch or commit\n");
    printf("  merge <branch>      Merge a branch into current branch\n");
    printf("  diff [<commit>]     Show differences\n");
}
