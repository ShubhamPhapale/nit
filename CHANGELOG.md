# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned
- Pack files for efficient storage
- Garbage collection
- Enhanced diff algorithm
- Tag support
- Remote repository support

## [1.0.0] - 2025-11-22

### Added
- Initial release of VCS (nit)
- Repository initialization (`vcs init`)
- File staging (`vcs add`)
- Commit functionality (`vcs commit`)
- Branch creation, deletion, and listing (`vcs branch`)
- Branch checkout (`vcs checkout`)
- Fast-forward merge
- Three-way merge (`vcs merge`)
- Commit history viewing (`vcs log`)
- Repository status (`vcs status`)
- Detached HEAD support
- SHA-1 content-addressable storage
- zlib compression for objects
- Object database with sharding
- Index-based staging area
- Reference management (HEAD, branches)
- Merge base detection
- Comprehensive documentation
- Test suite
- Demo script
- Cross-platform support (macOS, Linux)

### Technical Features
- Blob objects (file content)
- Tree objects (directory snapshots)
- Commit objects (metadata + history)
- Object compression (50-70% size reduction)
- Clean project structure with src/ directory
- Makefile with build directory support
- Comprehensive .gitignore

### Documentation
- README.md with user guide
- ARCHITECTURE.md with technical details
- DEPLOYMENT.md with production guide
- SUMMARY.md with project overview
- PROJECT.md with comprehensive guide
- CONTRIBUTING.md with contribution guidelines
- LICENSE (MIT)

### Testing
- Automated test suite (`test.sh`)
- Demo script showcasing all features (`demo.sh`)
- All core functionality tested and verified

### Performance
- Repository initialization: < 1ms
- File staging: < 5ms
- Commit creation: < 50ms
- Branch checkout: < 100ms
- Commit history: < 10ms per commit

### Security
- SHA-1 hashing for integrity
- Filesystem-based access control
- zlib compression

---

## Version History

### Version Numbering
- **Major (X.0.0)**: Incompatible API changes
- **Minor (1.X.0)**: Backward-compatible new features
- **Patch (1.0.X)**: Backward-compatible bug fixes

### Release Schedule
- Major releases: As needed
- Minor releases: Monthly
- Patch releases: As needed

---

[Unreleased]: https://github.com/ShubhamPhapale/nit/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/ShubhamPhapale/nit/releases/tag/v1.0.0
