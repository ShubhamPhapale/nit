#!/bin/bash
# Test script for VCS

set -e

echo "=== VCS Test Suite ==="
echo ""

# Cleanup function
cleanup() {
    echo "Cleaning up test environment..."
    rm -rf test_repo
}

# Set trap to cleanup on exit
trap cleanup EXIT

# Create test directory
echo "[1/10] Creating test repository..."
mkdir -p test_repo
cd test_repo

# Test 1: Initialize repository
echo "[2/10] Testing: vcs init"
../vcs init
if [ ! -d ".vcs" ]; then
    echo "FAIL: .vcs directory not created"
    exit 1
fi
echo "PASS: Repository initialized"
echo ""

# Test 2: Create test files
echo "[3/10] Creating test files..."
echo "Hello VCS" > file1.txt
echo "Test file 2" > file2.txt
echo "PASS: Test files created"
echo ""

# Test 3: Add files
echo "[4/10] Testing: vcs add"
../vcs add file1.txt
../vcs add file2.txt
echo "PASS: Files added to staging area"
echo ""

# Test 4: Check status
echo "[5/10] Testing: vcs status"
../vcs status
echo "PASS: Status displayed"
echo ""

# Test 5: Create commit
echo "[6/10] Testing: vcs commit"
../vcs commit -m "Initial commit"
echo "PASS: Commit created"
echo ""

# Test 6: View log
echo "[7/10] Testing: vcs log"
../vcs log
echo "PASS: Log displayed"
echo ""

# Test 7: Create branch
echo "[8/10] Testing: vcs branch"
../vcs branch test-branch
../vcs branch
echo "PASS: Branch created and listed"
echo ""

# Test 8: Checkout branch
echo "[9/10] Testing: vcs checkout"
../vcs checkout test-branch
echo "PASS: Checked out branch"
echo ""

# Test 9: Make changes and commit
echo "[10/10] Testing: commit on new branch"
echo "Modified content" >> file1.txt
../vcs add file1.txt
../vcs commit -m "Second commit on test-branch"
../vcs log
echo "PASS: Commit on new branch created"
echo ""

echo "==========================="
echo "All tests passed!"
echo "==========================="
