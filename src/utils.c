#include "vcs.h"
#include <openssl/sha.h>
#include <pwd.h>

// Compute SHA-1 hash
void compute_sha1(const void *data, size_t len, unsigned char *hash) {
    SHA1((const unsigned char *)data, len, hash);
}

// Convert SHA-1 binary to hex string
void sha1_to_hex(const unsigned char *sha1, char *hex) {
    for (int i = 0; i < SHA1_SIZE; i++) {
        sprintf(hex + (i * 2), "%02x", sha1[i]);
    }
    hex[SHA1_HEX_SIZE] = '\0';
}

// Convert hex string to SHA-1 binary
void hex_to_sha1(const char *hex, unsigned char *sha1) {
    for (int i = 0; i < SHA1_SIZE; i++) {
        sscanf(hex + (i * 2), "%2hhx", &sha1[i]);
    }
}

// Check if file exists
int file_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISREG(st.st_mode));
}

// Check if directory exists
int dir_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

// Create directory
int create_dir(const char *path) {
    if (mkdir(path, 0755) == -1) {
        if (errno != EEXIST) {
            perror("mkdir");
            return -1;
        }
    }
    return 0;
}

// Create directory recursively
int create_dir_recursive(const char *path) {
    char tmp[MAX_PATH];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    if (tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            if (create_dir(tmp) == -1 && errno != EEXIST) {
                return -1;
            }
            *p = '/';
        }
    }
    return create_dir(tmp);
}

// Read entire file into memory
char *read_file(const char *path, size_t *size) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *data = malloc(*size + 1);
    if (!data) {
        fclose(fp);
        return NULL;
    }

    if (fread(data, 1, *size, fp) != *size) {
        free(data);
        fclose(fp);
        return NULL;
    }

    data[*size] = '\0';
    fclose(fp);
    return data;
}

// Write data to file
int write_file(const char *path, const void *data, size_t size) {
    FILE *fp = fopen(path, "wb");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    if (fwrite(data, 1, size, fp) != size) {
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

// Get current time as string
void get_current_time(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Get user information
char *get_user_info(void) {
    static char info[512];
    struct passwd *pw = getpwuid(getuid());
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    
    if (pw) {
        snprintf(info, sizeof(info), "%s <%s@%s>", 
                 pw->pw_gecos[0] ? pw->pw_gecos : pw->pw_name,
                 pw->pw_name, hostname);
    } else {
        snprintf(info, sizeof(info), "unknown <unknown@%s>", hostname);
    }
    
    return info;
}
