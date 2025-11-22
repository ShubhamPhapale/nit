#include "vcs.h"
#include <zlib.h>

// Write object to disk with compression
int write_object(const void *data, size_t size, ObjectType type, char *sha1_out) {
    // Create header
    const char *type_str;
    switch (type) {
        case OBJ_BLOB: type_str = "blob"; break;
        case OBJ_TREE: type_str = "tree"; break;
        case OBJ_COMMIT: type_str = "commit"; break;
        default: return -1;
    }

    // Format: "type size\0data"
    char header[64];
    int header_len = snprintf(header, sizeof(header), "%s %zu", type_str, size);
    header[header_len] = '\0';
    header_len++;

    // Combine header and data
    size_t total_size = header_len + size;
    unsigned char *full_data = malloc(total_size);
    if (!full_data) {
        return -1;
    }
    memcpy(full_data, header, header_len);
    memcpy(full_data + header_len, data, size);

    // Compute SHA-1
    unsigned char sha1[SHA1_SIZE];
    compute_sha1(full_data, total_size, sha1);
    sha1_to_hex(sha1, sha1_out);

    // Check if object already exists
    char obj_path[MAX_PATH];
    snprintf(obj_path, sizeof(obj_path), "%s/%c%c", OBJECTS_DIR, 
             sha1_out[0], sha1_out[1]);
    create_dir_recursive(obj_path);
    snprintf(obj_path, sizeof(obj_path), "%s/%c%c/%s", OBJECTS_DIR,
             sha1_out[0], sha1_out[1], sha1_out + 2);

    if (file_exists(obj_path)) {
        free(full_data);
        return 0; // Object already exists
    }

    // Compress data
    void *compressed;
    size_t compressed_size;
    if (compress_data(full_data, total_size, &compressed, &compressed_size) != 0) {
        free(full_data);
        return -1;
    }

    // Write to file
    int ret = write_file(obj_path, compressed, compressed_size);
    
    free(full_data);
    free(compressed);
    return ret;
}

// Read object from disk with decompression
void *read_object(const char *sha1, size_t *size, ObjectType *type) {
    char obj_path[MAX_PATH];
    snprintf(obj_path, sizeof(obj_path), "%s/%c%c/%s", OBJECTS_DIR,
             sha1[0], sha1[1], sha1 + 2);

    if (!file_exists(obj_path)) {
        return NULL;
    }

    // Read compressed data
    size_t compressed_size;
    char *compressed = read_file(obj_path, &compressed_size);
    if (!compressed) {
        return NULL;
    }

    // Decompress
    void *decompressed;
    size_t decompressed_size;
    if (decompress_data(compressed, compressed_size, &decompressed, &decompressed_size) != 0) {
        free(compressed);
        return NULL;
    }
    free(compressed);

    // Parse header
    char *data = (char *)decompressed;
    char *space = strchr(data, ' ');
    char *null = strchr(data, '\0');
    
    if (!space || !null) {
        free(decompressed);
        return NULL;
    }

    *space = '\0';
    const char *type_str = data;
    *size = atoll(space + 1);

    if (strcmp(type_str, "blob") == 0) {
        *type = OBJ_BLOB;
    } else if (strcmp(type_str, "tree") == 0) {
        *type = OBJ_TREE;
    } else if (strcmp(type_str, "commit") == 0) {
        *type = OBJ_COMMIT;
    } else {
        free(decompressed);
        return NULL;
    }

    // Extract data
    size_t header_len = null - data + 1;
    void *obj_data = malloc(*size + 1);
    if (!obj_data) {
        free(decompressed);
        return NULL;
    }
    memcpy(obj_data, data + header_len, *size);
    ((char *)obj_data)[*size] = '\0';

    free(decompressed);
    return obj_data;
}

// Check if object exists
int object_exists(const char *sha1) {
    char obj_path[MAX_PATH];
    snprintf(obj_path, sizeof(obj_path), "%s/%c%c/%s", OBJECTS_DIR,
             sha1[0], sha1[1], sha1 + 2);
    return file_exists(obj_path);
}

// Get object path
char *get_object_path(const char *sha1) {
    static char obj_path[MAX_PATH];
    snprintf(obj_path, sizeof(obj_path), "%s/%c%c/%s", OBJECTS_DIR,
             sha1[0], sha1[1], sha1 + 2);
    return obj_path;
}

// Compress data using zlib
int compress_data(const void *src, size_t src_len, void **dst, size_t *dst_len) {
    *dst_len = compressBound(src_len);
    *dst = malloc(*dst_len);
    if (!*dst) {
        return -1;
    }

    if (compress2(*dst, dst_len, src, src_len, Z_DEFAULT_COMPRESSION) != Z_OK) {
        free(*dst);
        return -1;
    }

    return 0;
}

// Decompress data using zlib
int decompress_data(const void *src, size_t src_len, void **dst, size_t *dst_len) {
    *dst_len = src_len * 4; // Initial guess
    *dst = malloc(*dst_len);
    if (!*dst) {
        return -1;
    }

    int ret;
    while ((ret = uncompress(*dst, dst_len, src, src_len)) == Z_BUF_ERROR) {
        *dst_len *= 2;
        void *new_dst = realloc(*dst, *dst_len);
        if (!new_dst) {
            free(*dst);
            return -1;
        }
        *dst = new_dst;
    }

    if (ret != Z_OK) {
        free(*dst);
        return -1;
    }

    return 0;
}
