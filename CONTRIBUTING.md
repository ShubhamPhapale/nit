# Contributing to nit (nit)

Thank you for your interest in contributing to VCS! This document provides guidelines and instructions for contributing.

## 🌟 How to Contribute

### Reporting Bugs

1. Check if the bug has already been reported in [Issues](https://github.com/ShubhamPhapale/nit/issues)
2. If not, create a new issue with:
   - Clear, descriptive title
   - Detailed description of the bug
   - Steps to reproduce
   - Expected vs actual behavior
   - Environment details (OS, compiler version)
   - Any relevant logs or screenshots

### Suggesting Features

1. Check if the feature has been suggested in [Issues](https://github.com/ShubhamPhapale/nit/issues)
2. Create a new issue with:
   - Clear description of the feature
   - Use cases and benefits
   - Possible implementation approach (optional)

### Code Contributions

#### Development Workflow

We follow the Git Flow branching model:

- **main** - Production-ready code (protected)
- **develop** - Integration branch for features
- **feature/\*** - Feature branches
- **hotfix/\*** - Urgent fixes for production

#### Steps to Contribute Code

1. **Fork the Repository**
   ```bash
   # Click "Fork" on GitHub
   git clone https://github.com/YOUR_USERNAME/nit.git
   cd nit
   ```

2. **Set Up Development Environment**
   ```bash
   # Add upstream remote
   git remote add upstream https://github.com/ShubhamPhapale/nit.git
   
   # Checkout develop branch
   git checkout develop
   
   # Install dependencies (macOS)
   brew install openssl zlib
   
   # Or on Linux
   sudo apt-get install libssl-dev zlib1g-dev
   
   # Build
   make
   ```

3. **Create a Feature Branch**
   ```bash
   # Always branch from develop
   git checkout develop
   git pull upstream develop
   git checkout -b feature/your-feature-name
   ```

4. **Make Your Changes**
   - Write clean, readable code
   - Follow existing code style
   - Add comments for complex logic
   - Update documentation if needed

5. **Test Your Changes**
   ```bash
   # Build
   make clean && make
   
   # Run tests
   ./scripts/test.sh
   
   # Test manually if needed
   ./nit init
   ./nit add .
   ./nit commit -m "Test"
   ```

6. **Commit Your Changes**
   ```bash
   git add .
   git commit -m "feat: add your feature description"
   ```

7. **Push to Your Fork**
   ```bash
   git push origin feature/your-feature-name
   ```

8. **Create Pull Request**
   - Go to GitHub and create a Pull Request
   - Base: `develop` ← Compare: `feature/your-feature-name`
   - Fill in the PR template
   - Link any related issues

## 📋 Code Style Guidelines

### C Code Style

- **Indentation**: 4 spaces (no tabs)
- **Naming**:
  - Functions: `snake_case` (e.g., `write_object()`)
  - Variables: `snake_case` (e.g., `commit_sha1`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_PATH`)
  - Structs: `PascalCase` (e.g., `IndexEntry`)
- **Braces**: K&R style
  ```c
  if (condition) {
      // code
  } else {
      // code
  }
  ```
- **Comments**: Use `//` for single-line, `/* */` for multi-line
- **Line length**: Max 100 characters

### Example

```c
// Good
int write_commit(Commit *commit, char *sha1_out) {
    if (!commit) {
        fprintf(stderr, "Error: NULL commit\n");
        return -1;
    }
    
    // Build commit data
    char data[MAX_LINE];
    int len = snprintf(data, sizeof(data), "tree %s\n", commit->tree_sha1);
    
    return write_object(data, len, OBJ_COMMIT, sha1_out);
}
```

### Commit Message Format

Follow [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types**:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, etc.)
- `refactor`: Code refactoring
- `test`: Adding or updating tests
- `chore`: Maintenance tasks

**Examples**:
```bash
feat(merge): add three-way merge algorithm
fix(object): correct SHA-1 hash calculation
docs(readme): update installation instructions
test(branch): add branch creation tests
```

## 🧪 Testing Guidelines

### Running Tests

```bash
# Run full test suite
./scripts/test.sh

# Run demo
./scripts/demo.sh

# Manual testing
mkdir test-dir && cd test-dir
../nit init
echo "test" > file.txt
../nit add file.txt
../nit commit -m "Test"
../nit log
```

### Writing Tests

When adding new features:
1. Add test cases to `scripts/test.sh`
2. Ensure all existing tests pass
3. Test edge cases and error conditions
4. Test on multiple platforms if possible

## 📝 Documentation

### When to Update Documentation

Update docs when you:
- Add new features
- Change existing behavior
- Fix bugs that affect usage
- Add new commands or options

### Documentation Files

- **README.md** - User-facing documentation
- **docs/ARCHITECTURE.md** - Technical architecture
- **docs/DEPLOYMENT.md** - Deployment guide
- **Code comments** - Inline documentation

## 🔍 Code Review Process

### What We Look For

- **Functionality**: Does it work as intended?
- **Code Quality**: Is it clean and maintainable?
- **Tests**: Are there adequate tests?
- **Documentation**: Is it well-documented?
- **Style**: Does it follow our style guide?
- **Performance**: Any performance concerns?
- **Security**: Any security implications?

### Review Timeline

- Initial review: 1-3 days
- Follow-up reviews: 1-2 days
- We'll merge once approved and all checks pass

## 🐛 Debugging Tips

### Compile with Debug Symbols

```bash
make CFLAGS="-Wall -Wextra -std=c11 -g -O0"
```

### Use Valgrind for Memory Leaks

```bash
valgrind --leak-check=full ./nit init
```

### Enable Verbose Logging

Add debug prints in code:
```c
#ifdef DEBUG
fprintf(stderr, "[DEBUG] Variable value: %d\n", value);
#endif
```

## 📜 License

By contributing, you agree that your contributions will be licensed under the MIT License.

## 🙏 Recognition

Contributors will be acknowledged in:
- CONTRIBUTORS.md file
- Release notes
- GitHub contributors page

## 💬 Communication

- **Issues**: Bug reports and feature requests
- **Discussions**: General questions and ideas
- **Pull Requests**: Code contributions

## ❓ Questions?

If you have questions:
1. Check existing documentation
2. Search closed issues
3. Ask in GitHub Discussions
4. Create a new issue

Thank you for contributing to nit! 🎉
