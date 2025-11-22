# Release Process

This document describes the process for creating a new release of nit.

## Version Numbering

nit follows [Semantic Versioning](https://semver.org/):
- **Major (X.0.0)**: Incompatible API changes
- **Minor (1.X.0)**: Backward-compatible new features
- **Patch (1.0.X)**: Backward-compatible bug fixes

Current version: **1.0.0** (defined in `src/vcs.h`)

## Release Checklist

### 1. Update Version Number

Update the version in `src/vcs.h`:

```c
#define NIT_VERSION "1.0.0"
#define NIT_VERSION_MAJOR 1
#define NIT_VERSION_MINOR 0
#define NIT_VERSION_PATCH 0
```

### 2. Update CHANGELOG.md

- Move items from `[Unreleased]` section to a new version section
- Add release date
- Update version links at the bottom

Example:
```markdown
## [1.0.1] - 2025-11-23

### Fixed
- Bug fixes here

[1.0.1]: https://github.com/ShubhamPhapale/nit/releases/tag/v1.0.1
```

### 3. Update README.md

If needed, update:
- Version badge
- Documentation
- Installation instructions

### 4. Build and Test

```bash
# Clean build
make clean
make

# Run tests
./scripts/test.sh

# Test version command
./nit version
```

### 5. Commit Changes

```bash
git add src/vcs.h CHANGELOG.md README.md
git commit -m "Bump version to X.Y.Z"
git push origin main
```

### 6. Create Git Tag

```bash
# Create annotated tag
git tag -a vX.Y.Z -m "Release version X.Y.Z"

# Push tag to GitHub
git push origin vX.Y.Z
```

### 7. Automated Release

Once the tag is pushed, GitHub Actions will automatically:
1. Create a GitHub release
2. Build binaries for:
   - Linux (x86_64)
   - macOS (ARM64)
   - macOS (x86_64)
3. Upload binaries to the release
4. Run tests on all platforms

### 8. Verify Release

After the workflow completes:
1. Go to: https://github.com/ShubhamPhapale/nit/releases
2. Verify the release exists
3. Download and test each binary
4. Update release notes if needed

## Release Workflow

The release workflow (`.github/workflows/release.yml`) is triggered by tags matching `v*.*.*`.

### Workflow Steps:
1. **Create Release**: Creates GitHub release with release notes
2. **Build Linux**: Builds and tests on Ubuntu
3. **Build macOS ARM64**: Builds and tests on macOS ARM64
4. **Build macOS x86_64**: Builds and tests on macOS x86_64

### Artifacts:
- `nit-linux-x86_64` - Linux binary
- `nit-macos-arm64` - macOS ARM64 binary
- `nit-macos-x86_64` - macOS Intel binary

## Manual Release (Fallback)

If automated release fails:

### 1. Build Binaries

**Linux:**
```bash
docker run -v $(pwd):/src ubuntu:20.04 bash -c "
  apt-get update && \
  apt-get install -y build-essential libssl-dev zlib1g-dev && \
  cd /src && make clean && make && \
  cp nit nit-linux-x86_64
"
```

**macOS:**
```bash
make clean && make
cp nit nit-macos-$(uname -m)
```

### 2. Create Release Manually

1. Go to: https://github.com/ShubhamPhapale/nit/releases/new
2. Choose the tag you created
3. Fill in release notes
4. Upload binaries
5. Publish release

## Release Notes Template

```markdown
## nit Version Control System - vX.Y.Z

A fully functional version control system implemented in C.

### Installation

#### Linux (Ubuntu/Debian)
\`\`\`bash
wget https://github.com/ShubhamPhapale/nit/releases/download/vX.Y.Z/nit-linux-x86_64
chmod +x nit-linux-x86_64
sudo mv nit-linux-x86_64 /usr/local/bin/nit
\`\`\`

#### macOS
\`\`\`bash
# For ARM64 (M1/M2)
wget https://github.com/ShubhamPhapale/nit/releases/download/vX.Y.Z/nit-macos-arm64
chmod +x nit-macos-arm64
sudo mv nit-macos-arm64 /usr/local/bin/nit

# For x86_64 (Intel)
wget https://github.com/ShubhamPhapale/nit/releases/download/vX.Y.Z/nit-macos-x86_64
chmod +x nit-macos-x86_64
sudo mv nit-macos-x86_64 /usr/local/bin/nit
\`\`\`

### What's New

[Copy relevant sections from CHANGELOG.md]

### Quick Start
\`\`\`bash
nit version
nit init
nit add .
nit commit -m "Initial commit"
\`\`\`

See [CHANGELOG.md](https://github.com/ShubhamPhapale/nit/blob/main/CHANGELOG.md) for full details.
```

## Hotfix Release Process

For urgent fixes:

1. Create hotfix branch from main: `git checkout -b hotfix/X.Y.Z main`
2. Make fixes and commit
3. Update version and CHANGELOG
4. Merge to main: `git checkout main && git merge hotfix/X.Y.Z`
5. Follow normal release process
6. Delete hotfix branch: `git branch -d hotfix/X.Y.Z`

## Post-Release

1. Announce on:
   - GitHub Discussions
   - Project documentation
   - Social media (if applicable)
2. Update project website (if applicable)
3. Monitor for issues
4. Start planning next release

## Rollback

If critical issues are found:

1. Document the issue
2. Create hotfix release (patch version bump)
3. If necessary, mark release as "pre-release" on GitHub
4. Add warning to release notes

## Version History

- **v1.0.0** (2025-11-22) - Initial release

## Support

For questions about releases:
- 🐛 [Issue Tracker](https://github.com/ShubhamPhapale/nit/issues)
- 💬 [Discussions](https://github.com/ShubhamPhapale/nit/discussions)
