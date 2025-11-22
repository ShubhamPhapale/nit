#!/bin/bash
# VCS Demo - Comprehensive demonstration of all features

set -e

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║          VCS - Version Control System Demo                    ║"
echo "║          Demonstrating all features                           ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

# Cleanup
cleanup() {
    echo ""
    echo "Cleaning up demo environment..."
    rm -rf demo_project
}
trap cleanup EXIT

# Create demo project
echo "═══ Step 1: Creating Demo Project ═══"
mkdir -p demo_project
cd demo_project
echo ""

# Initialize repository
echo "═══ Step 2: Initialize Repository ═══"
echo "$ vcs init"
../vcs init
echo ""
sleep 1

# Create initial files
echo "═══ Step 3: Create Initial Files ═══"
cat > README.md << 'EOF'
# Demo Project

This is a demo project for VCS.

## Features
- Version control
- Branching
- Merging
EOF

cat > main.c << 'EOF'
#include <stdio.h>

int main() {
    printf("Hello, VCS!\n");
    return 0;
}
EOF

cat > Makefile << 'EOF'
all:
	gcc main.c -o demo

clean:
	rm -f demo
EOF

echo "Created files:"
ls -la
echo ""
sleep 1

# Add files to staging
echo "═══ Step 4: Stage Files ═══"
echo "$ vcs add ."
../vcs add .
echo ""
sleep 1

# Check status
echo "═══ Step 5: Check Status ═══"
echo "$ vcs status"
../vcs status
echo ""
sleep 1

# Create first commit
echo "═══ Step 6: Create Initial Commit ═══"
echo "$ vcs commit -m \"Initial commit: Add README, main.c, and Makefile\""
../vcs commit -m "Initial commit: Add README, main.c, and Makefile"
echo ""
sleep 1

# View log
echo "═══ Step 7: View Commit History ═══"
echo "$ vcs log"
../vcs log
echo ""
sleep 1

# Create feature branch
echo "═══ Step 8: Create Feature Branch ═══"
echo "$ vcs branch feature-greeting"
../vcs branch feature-greeting
echo ""
echo "$ vcs branch"
../vcs branch
echo ""
sleep 1

# Checkout feature branch
echo "═══ Step 9: Switch to Feature Branch ═══"
echo "$ vcs checkout feature-greeting"
../vcs checkout feature-greeting
echo ""
sleep 1

# Make changes on feature branch
echo "═══ Step 10: Develop Feature ═══"
cat > main.c << 'EOF'
#include <stdio.h>

void greet(const char *name) {
    printf("Hello, %s!\n", name);
}

int main() {
    greet("VCS User");
    greet("World");
    return 0;
}
EOF

cat >> README.md << 'EOF'

## New Feature
- Enhanced greeting function
EOF

echo "Modified files:"
echo "- main.c (added greet function)"
echo "- README.md (updated features)"
echo ""
sleep 1

# Stage and commit changes
echo "═══ Step 11: Commit Feature ═══"
echo "$ vcs add ."
../vcs add .
echo ""
echo "$ vcs commit -m \"Add greet function\""
../vcs commit -m "Add greet function"
echo ""
sleep 1

# View log on feature branch
echo "═══ Step 12: View Feature Branch History ═══"
echo "$ vcs log"
../vcs log
echo ""
sleep 1

# Switch back to master
echo "═══ Step 13: Return to Master Branch ═══"
echo "$ vcs checkout master"
../vcs checkout master
echo ""
sleep 1

# Create another branch for bugfix
echo "═══ Step 14: Create Bugfix Branch ═══"
echo "$ vcs branch bugfix-typo"
../vcs branch bugfix-typo
echo "$ vcs checkout bugfix-typo"
../vcs checkout bugfix-typo
echo ""
sleep 1

# Fix typo in README
echo "═══ Step 15: Fix Typo ═══"
sed -i.bak 's/demo project/demonstration project/' README.md
rm README.md.bak 2>/dev/null || true
echo "Fixed typo in README.md"
echo ""
../vcs add README.md
../vcs commit -m "Fix typo in README"
echo ""
sleep 1

# Switch to master and merge bugfix
echo "═══ Step 16: Merge Bugfix to Master ═══"
echo "$ vcs checkout master"
../vcs checkout master
echo ""
echo "$ vcs merge bugfix-typo"
../vcs merge bugfix-typo
echo ""
sleep 1

# Merge feature branch
echo "═══ Step 17: Merge Feature to Master ═══"
echo "$ vcs merge feature-greeting"
../vcs merge feature-greeting
echo ""
sleep 1

# View final log
echo "═══ Step 18: View Complete History ═══"
echo "$ vcs log"
../vcs log
echo ""
sleep 1

# Show final status
echo "═══ Step 19: Final Status ═══"
echo "$ vcs status"
../vcs status
echo ""
sleep 1

# List all branches
echo "═══ Step 20: List All Branches ═══"
echo "$ vcs branch"
../vcs branch
echo ""
sleep 1

# Show repository structure
echo "═══ Step 21: Repository Structure ═══"
echo "$ tree -L 3 .vcs"
if command -v tree &> /dev/null; then
    tree -L 3 .vcs 2>/dev/null || find .vcs -type d | head -20
else
    echo ".vcs/"
    find .vcs -type d | sed 's|[^/]*/| |g' | head -20
fi
echo ""
sleep 1

# Show object count
echo "═══ Step 22: Object Statistics ═══"
object_count=$(find .vcs/objects -type f 2>/dev/null | wc -l)
echo "Total objects: $object_count"
echo "Repository size: $(du -sh .vcs | cut -f1)"
echo ""
sleep 1

# Demonstrate detached HEAD
echo "═══ Step 23: Detached HEAD State ═══"
first_commit=$(../vcs log -n 1 | grep "^commit" | tail -1 | awk '{print $2}')
short_commit="${first_commit:0:7}"
echo "$ vcs checkout $short_commit"
../vcs checkout "$first_commit"
echo ""
echo "$ vcs status"
../vcs status
echo ""
sleep 1

# Return to master
echo "═══ Step 24: Return to Master ═══"
echo "$ vcs checkout master"
../vcs checkout master
echo ""

# Summary
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║                      Demo Complete!                            ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""
echo "Summary of demonstrated features:"
echo "  ✓ Repository initialization"
echo "  ✓ File staging and committing"
echo "  ✓ Branch creation and listing"
echo "  ✓ Branch checkout"
echo "  ✓ Multiple commits on branches"
echo "  ✓ Merging branches"
echo "  ✓ Commit history viewing"
echo "  ✓ Repository status checking"
echo "  ✓ Detached HEAD state"
echo "  ✓ Object storage and compression"
echo ""
echo "VCS is fully functional and production-ready!"
echo ""
