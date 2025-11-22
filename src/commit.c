#include "vcs.h"

// Create new commit
Commit *commit_new(void) {
    Commit *commit = malloc(sizeof(Commit));
    if (!commit) {
        return NULL;
    }
    memset(commit, 0, sizeof(Commit));
    return commit;
}

// Free commit
void commit_free(Commit *commit) {
    free(commit);
}

// Write commit object
int write_commit(Commit *commit, char *sha1_out) {
    char data[MAX_LINE * 4];
    int len = 0;

    // Build commit data
    len += snprintf(data + len, sizeof(data) - len, "tree %s\n", commit->tree_sha1);
    
    if (strlen(commit->parent_sha1) > 0) {
        len += snprintf(data + len, sizeof(data) - len, "parent %s\n", commit->parent_sha1);
    }
    
    len += snprintf(data + len, sizeof(data) - len, 
                   "author %s %ld\n", commit->author, commit->timestamp);
    len += snprintf(data + len, sizeof(data) - len,
                   "committer %s %ld\n", commit->committer, commit->timestamp);
    len += snprintf(data + len, sizeof(data) - len, "\n%s\n", commit->message);

    return write_object(data, len, OBJ_COMMIT, sha1_out);
}

// Read commit object
Commit *read_commit(const char *sha1) {
    size_t size;
    ObjectType type;
    char *data = read_object(sha1, &size, &type);

    if (!data || type != OBJ_COMMIT) {
        free(data);
        return NULL;
    }

    Commit *commit = commit_new();
    if (!commit) {
        free(data);
        return NULL;
    }

    char *line = data;
    char *next_line;

    while ((next_line = strchr(line, '\n')) != NULL) {
        *next_line = '\0';
        
        if (strlen(line) == 0) {
            // Empty line, message follows
            line = next_line + 1;
            strncpy(commit->message, line, sizeof(commit->message) - 1);
            commit->message[sizeof(commit->message) - 1] = '\0';
            // Remove trailing newline if present
            size_t msg_len = strlen(commit->message);
            if (msg_len > 0 && commit->message[msg_len - 1] == '\n') {
                commit->message[msg_len - 1] = '\0';
            }
            break;
        }

        if (strncmp(line, "tree ", 5) == 0) {
            strncpy(commit->tree_sha1, line + 5, SHA1_HEX_SIZE);
            commit->tree_sha1[SHA1_HEX_SIZE] = '\0';
        } else if (strncmp(line, "parent ", 7) == 0) {
            strncpy(commit->parent_sha1, line + 7, SHA1_HEX_SIZE);
            commit->parent_sha1[SHA1_HEX_SIZE] = '\0';
        } else if (strncmp(line, "author ", 7) == 0) {
            char *timestamp_str = strrchr(line + 7, ' ');
            if (timestamp_str) {
                commit->timestamp = atol(timestamp_str + 1);
                *timestamp_str = '\0';
                strncpy(commit->author, line + 7, sizeof(commit->author) - 1);
                commit->author[sizeof(commit->author) - 1] = '\0';
            }
        } else if (strncmp(line, "committer ", 10) == 0) {
            char *timestamp_str = strrchr(line + 10, ' ');
            if (timestamp_str) {
                *timestamp_str = '\0';
                strncpy(commit->committer, line + 10, sizeof(commit->committer) - 1);
                commit->committer[sizeof(commit->committer) - 1] = '\0';
            }
        }

        line = next_line + 1;
    }

    free(data);
    return commit;
}
