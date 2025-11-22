# Release Checklist for v1.0.0

This checklist will guide you through creating the v1.0.0 release.

## Pre-Release Verification

- [x] Version information added to codebase (`src/vcs.h`)
- [x] Version command implemented (`nit version`, `nit --version`, `nit -v`)
- [x] All tests pass
- [x] Build completes without errors (warnings are acceptable)
- [x] Security scan complete (no vulnerabilities found)
- [x] Release workflow created (`.github/workflows/release.yml`)
- [x] Release documentation created (`RELEASE.md`)

## Current Version Information

**Version**: 1.0.0
**Date**: November 22, 2025
**Branch**: main

## To Create the Release

Once this PR is merged to main, follow these steps:

### 1. Verify Main Branch

```bash
git checkout main
git pull origin main
```

### 2. Create and Push Git Tag

```bash
# Create annotated tag
git tag -a v1.0.0 -m "Release version 1.0.0"

# Verify tag
git tag -l v1.0.0

# Push tag to GitHub
git push origin v1.0.0
```

### 3. Monitor Release Workflow

1. Go to: https://github.com/ShubhamPhapale/nit/actions
2. Look for "Release" workflow
3. Monitor the following jobs:
   - Build Linux (x86_64)
   - Build macOS (ARM64)
   - Build macOS (x86_64)
   - Create Release

The workflow will:
- Build binaries on all platforms
- Run tests on all platforms
- Create a GitHub release
- Upload binaries to the release

### 4. Verify Release

1. Go to: https://github.com/ShubhamPhapale/nit/releases
2. Verify the release exists with:
   - Release notes
   - Installation instructions
   - Three binary files:
     - `nit-linux-x86_64`
     - `nit-macos-arm64`
     - `nit-macos-x86_64`

### 5. Test Downloaded Binaries

```bash
# Linux
wget https://github.com/ShubhamPhapale/nit/releases/download/v1.0.0/nit-linux-x86_64
chmod +x nit-linux-x86_64
./nit-linux-x86_64 version

# macOS ARM64
wget https://github.com/ShubhamPhapale/nit/releases/download/v1.0.0/nit-macos-arm64
chmod +x nit-macos-arm64
./nit-macos-arm64 version

# macOS x86_64
wget https://github.com/ShubhamPhapale/nit/releases/download/v1.0.0/nit-macos-x86_64
chmod +x nit-macos-x86_64
./nit-macos-x86_64 version
```

Expected output:
```
nit version 1.0.0
A fully functional version control system implemented in C
```

## Post-Release Tasks

- [ ] Update GitHub repository description to include version
- [ ] Update README badges if needed
- [ ] Announce release on GitHub Discussions
- [ ] Create social media posts (if applicable)
- [ ] Monitor for issues from users

## Troubleshooting

### If Release Workflow Fails

1. Check GitHub Actions logs for errors
2. Verify all dependencies are available
3. Check if secrets/tokens are configured
4. Re-run failed jobs if it's a temporary issue

### If Manual Release is Needed

Follow the instructions in `RELEASE.md` under "Manual Release (Fallback)"

## Files Modified in This PR

1. `src/vcs.h` - Added version constants
2. `src/main.c` - Added version command
3. `.github/workflows/release.yml` - Release automation workflow
4. `RELEASE.md` - Release process documentation
5. `RELEASE_CHECKLIST.md` - This file

## Next Steps After v1.0.0

- Plan v1.0.1 or v1.1.0 features
- Update `CHANGELOG.md` with "Unreleased" section
- Monitor GitHub issues for bug reports
- Review and merge community contributions

---

**Ready to Release**: ✅ Yes
**Security Scan**: ✅ Passed
**Tests**: ✅ All passing
**Documentation**: ✅ Complete
