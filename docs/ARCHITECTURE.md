# nit Architecture Documentation

## Overview

VCS is a distributed version control system implemented in C, following a Git-like architecture with content-addressable object storage, staging area, and branching support.

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                       VCS CLI (main.c)                      │
│  Commands: init, add, commit, branch, checkout, merge, etc. │
└───────────────────────────┬─────────────────────────────────┘
                            │
        ┌───────────────────┼───────────────────┐
        ▼                   ▼                   ▼
┌──────────────┐   ┌──────────────┐   ┌──────────────┐
│  Repository  │   │   Working    │   │   Staging    │
│  Management  │   │  Directory   │   │     Area     │
│   (repo.c)   │   │ (workdir.c)  │   │  (index.c)   │
└──────┬───────┘   └──────┬───────┘   └──────┬───────┘
       │                  │                  │
       └──────────────────┼──────────────────┘
                          │
              ┌───────────┴───────────┐
              ▼                       ▼
      ┌──────────────┐        ┌──────────────┐
      │    Object    │        │  References  │
      │   Storage    │        │   & Refs     │
      │  (object.c)  │        │   (refs.c)   │
      └──────┬───────┘        └──────┬───────┘
             │                       │
      ┌──────┴───────┐        ┌──────┴───────┐
      ▼              ▼        ▼              ▼
  ┌───────┐    ┌───────┐  ┌───────┐    ┌───────┐
  │ Blob  │    │ Tree  │  │ HEAD  │    │Branch │
  │Objects│    │Objects│  │  Ref  │    │ Refs  │
  └───────┘    └───────┘  └───────┘    └───────┘
       │            │          │            │
       └────────────┴──────────┴────────────┘
                    │
                    ▼
            ┌──────────────┐
            │  Commit      │
            │  Objects     │
            │  (commit.c)  │
            └──────────────┘
```

## Core Components

### 1. Object Database (object.c)

**Purpose**: Content-addressable storage for all repository data.

**Object Types**:
- **Blob**: File contents
- **Tree**: Directory snapshots
- **Commit**: Commit metadata and history

**Storage Format**:
```
.vcs/objects/XX/YYYYYYYY...
    │        │   └─ Remaining 38 chars of SHA-1
    │        └─ First 2 chars of SHA-1 (sharding)
    └─ Objects directory
```

**Object Structure**:
```
┌──────────────────────────────────┐
│ Header: "type size\0"            │
├──────────────────────────────────┤
│ Content: actual data             │
└──────────────────────────────────┘
     │
     ▼ SHA-1 hash
     ▼ zlib compression
     ▼ Write to disk
```

**Functions**:
- `write_object()`: Store object with compression
- `read_object()`: Retrieve and decompress object
- `object_exists()`: Check if object exists
- `compress_data()`: zlib compression
- `decompress_data()`: zlib decompression

### 2. Index/Staging Area (index.c)

**Purpose**: Track files staged for next commit.

**Index Structure**:
```c
typedef struct {
    char sha1[41];        // Object hash
    char path[4096];      // File path
    time_t mtime;         // Modification time
    size_t size;          // File size
} IndexEntry;

typedef struct {
    IndexEntry *entries;  // Dynamic array
    size_t count;         // Entry count
    size_t capacity;      // Allocated capacity
} Index;
```

**Storage Format** (.vcs/index):
```
<sha1> <mtime> <size> <path>
<sha1> <mtime> <size> <path>
...
```

**Operations**:
- `index_load()`: Load from disk
- `index_save()`: Persist to disk
- `index_add_entry()`: Add/update entry
- `index_remove_entry()`: Remove entry
- `index_find_entry()`: Lookup entry

### 3. Tree Management (tree.c)

**Purpose**: Represent directory structure snapshots.

**Tree Entry**:
```c
typedef struct {
    char mode[10];        // File mode (e.g., "100644")
    char type[10];        // Object type ("blob", "tree")
    char sha1[41];        // Object hash
    char name[256];       // Filename
} TreeEntry;
```

**Tree Object Format**:
```
<mode> <name>\0<20-byte SHA-1>
<mode> <name>\0<20-byte SHA-1>
...
```

**Functions**:
- `tree_from_index()`: Build tree from index
- `write_tree()`: Serialize and store tree
- `read_tree()`: Deserialize tree object
- `tree_add_entry()`: Add entry to tree

### 4. Commit System (commit.c)

**Purpose**: Store commit metadata and history.

**Commit Structure**:
```c
typedef struct {
    char tree_sha1[41];      // Tree object hash
    char parent_sha1[41];    // Parent commit hash
    char author[256];        // Author info
    char committer[256];     // Committer info
    time_t timestamp;        // Commit time
    char message[1024];      // Commit message
} Commit;
```

**Commit Object Format**:
```
tree <tree-sha1>
parent <parent-sha1>
author <name> <email> <timestamp>
committer <name> <email> <timestamp>

<commit message>
```

**Functions**:
- `write_commit()`: Create commit object
- `read_commit()`: Parse commit object
- `commit_new()`: Allocate commit structure
- `commit_free()`: Deallocate commit

### 5. Reference Management (refs.c)

**Purpose**: Manage branches and HEAD pointer.

**Reference Types**:

1. **Symbolic Reference** (HEAD):
   ```
   ref: refs/heads/master
   ```

2. **Direct Reference** (branches):
   ```
   <commit-sha1>
   ```

**Directory Structure**:
```
.vcs/
├── HEAD                    # Current branch or commit
└── refs/
    └── heads/
        ├── master         # Main branch
        ├── feature-x      # Feature branch
        └── ...
```

**Functions**:
- `write_ref()`: Create/update reference
- `read_ref()`: Read reference
- `update_head()`: Update HEAD pointer
- `get_head_commit()`: Resolve HEAD to commit
- `is_head_detached()`: Check HEAD state
- `get_current_branch()`: Get active branch

### 6. Branch Management (branch.c)

**Purpose**: Create and manage branches.

**Branch Operations**:
```
create_branch()  ─────┐
                      ├─> write_ref()
delete_branch()  ─────┘

list_branches()  ───> readdir(refs/heads/)

branch_exists()  ───> file_exists(ref_path)
```

**Functions**:
- `create_branch()`: Create new branch
- `delete_branch()`: Remove branch
- `list_branches()`: Show all branches
- `branch_exists()`: Check branch existence

### 7. Working Directory (workdir.c)

**Purpose**: Interact with working tree.

**Operations**:

1. **Add Files**:
   ```
   File → read_file() → compute_sha1() → write_object()
                                       → index_add_entry()
   ```

2. **Status Check**:
   ```
   Index ←───────┐
   Working Tree ─┴─> compare → show differences
   ```

3. **Log Display**:
   ```
   HEAD → commit → parent → parent → ...
   ```

**Functions**:
- `add_file()`: Stage single file
- `add_all()`: Stage all files
- `vcs_status()`: Show status
- `vcs_log()`: Display history
- `vcs_diff()`: Show differences

### 8. Checkout System (checkout.c)

**Purpose**: Switch branches or commits.

**Checkout Process**:
```
1. Validate target (branch/commit)
2. Update HEAD reference
3. (Future: Update working tree)
```

**Functions**:
- `checkout_branch()`: Switch to branch
- `checkout_commit()`: Detach HEAD

### 9. Merge System (merge.c)

**Purpose**: Combine branch histories.

**Merge Strategies**:

1. **Fast-Forward**:
   ```
   master:  A---B
                 \
   feature:      C---D
   
   Result:  A---B---C---D (master)
   ```

2. **Three-Way Merge**:
   ```
   master:  A---B---C
                 \   \
   feature:      D---E---M (merge commit)
   ```

**Merge Base Detection**:
```
find_merge_base()
    │
    ├─> Traverse commit1 history
    │
    └─> Check if commit2 is ancestor
```

**Functions**:
- `merge_branch()`: Perform merge
- `find_merge_base()`: Find common ancestor

## Data Flow

### Commit Creation Flow

```
1. User edits files
   ↓
2. vcs add <file>
   ↓
3. Read file content
   ↓
4. Compute SHA-1 hash
   ↓
5. Compress with zlib
   ↓
6. Write blob object
   ↓
7. Update index
   ↓
8. vcs commit -m "message"
   ↓
9. Build tree from index
   ↓
10. Write tree object
   ↓
11. Create commit object
   ↓
12. Update branch reference
```

### Object Resolution Flow

```
Reference (HEAD/branch)
   ↓
Resolve to commit SHA-1
   ↓
Read commit object
   ↓
Get tree SHA-1
   ↓
Read tree object
   ↓
Get blob SHA-1s
   ↓
Read blob objects
   ↓
Restore files
```

## Memory Management

### Allocation Strategy
- Dynamic arrays with capacity doubling
- Manual memory management (no GC)
- Free resources after use

### Key Functions
```c
Index *idx = index_new();     // Allocate
index_load(idx);              // Use
index_save(idx);              // Use
index_free(idx);              // Deallocate
```

### Memory Patterns
1. **RAII-style**: Allocate → Use → Free
2. **Static buffers**: For SHA-1 hex strings
3. **Dynamic arrays**: For lists (index, tree)

## Error Handling

### Strategy
- Return codes: 0 = success, -1 = error
- stderr for error messages
- Defensive programming

### Example
```c
if (!file_exists(path)) {
    fprintf(stderr, "Error: File not found\n");
    return -1;
}
```

## Concurrency

**Current State**: Not thread-safe

**Considerations**:
- Single-user, single-process model
- No locking mechanisms
- File operations are atomic

**Future**: Add file locking for multi-process safety

## Performance Characteristics

### Time Complexity
| Operation | Complexity | Notes |
|-----------|-----------|-------|
| Add file | O(n) | n = file size |
| Commit | O(m) | m = index entries |
| Log | O(k) | k = commits to show |
| Branch list | O(b) | b = number of branches |
| Merge | O(m) | Simplified merge |

### Space Complexity
- **Objects**: O(n) where n = total content size
- **Index**: O(m) where m = tracked files
- **References**: O(b) where b = branches

### Compression
- zlib reduces storage by 50-70%
- Objects stored individually (no packs)

## Security Model

### SHA-1 Hashing
- Content-addressable storage
- Integrity verification
- Collision resistance (practical)

### Permissions
- Filesystem-based access control
- No built-in authentication
- Trust filesystem security

### Vulnerabilities
- SHA-1 theoretical weaknesses
- No encryption at rest
- No access logging

## Extensibility

### Adding New Commands
1. Add function declaration in `vcs.h`
2. Implement in appropriate `.c` file
3. Add command handler in `main.c`
4. Update help text

### Adding New Object Types
1. Add enum value to `ObjectType`
2. Implement read/write functions
3. Update object storage logic

### Plugin System
Not implemented. Could add:
- Hook system (pre-commit, post-commit)
- Extension loading
- Configuration-based plugins

## Testing Strategy

### Unit Testing
- Test each module independently
- Verify object storage/retrieval
- Check index operations

### Integration Testing
- Full workflow tests (test.sh)
- Multi-command scenarios
- Error condition handling

### Regression Testing
- Verify backward compatibility
- Check object format stability

## Future Enhancements

### Planned Features
1. **Pack Files**: Efficient bulk storage
2. **Network Protocol**: Remote repositories
3. **Garbage Collection**: Remove unused objects
4. **Submodules**: Nested repositories
5. **Hooks**: Pre/post-commit scripts
6. **Tags**: Named references
7. **Stash**: Temporary changes
8. **Rebase**: History rewriting

### Performance Improvements
1. Object caching
2. Index optimization
3. Parallel compression
4. Delta compression

### Robustness
1. Crash recovery
2. Corruption detection
3. Transaction support
4. Better error messages

## Comparison with Git

| Feature | VCS | Git |
|---------|-----|-----|
| Object storage | ✓ | ✓ |
| SHA-1 hashing | ✓ | ✓ (migrating to SHA-256) |
| Compression | zlib | zlib |
| Pack files | ✗ | ✓ |
| Network | ✗ | ✓ |
| Submodules | ✗ | ✓ |
| Hooks | ✗ | ✓ |
| Stash | ✗ | ✓ |
| Rebase | ✗ | ✓ |
| Size | ~100KB | ~30MB |

## Glossary

- **Blob**: Binary large object (file content)
- **Tree**: Directory snapshot
- **Commit**: Snapshot with metadata
- **Index**: Staging area
- **HEAD**: Current branch/commit pointer
- **Reference**: Named pointer to commit
- **SHA-1**: Secure Hash Algorithm 1
- **Object**: Blob, tree, or commit
- **Working tree**: Current directory state

---

**Version**: 1.0  
**Last Updated**: November 22, 2025
