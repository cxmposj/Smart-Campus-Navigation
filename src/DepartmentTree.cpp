
#include "DepartmentTree.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <queue>

using namespace std;

// Removes whitespace from the beginning and end of a string.
namespace {
    string trim(const string& value) {
        size_t first = value.find_first_not_of(" \t\r\n");

        if (first == string::npos) {
            return "";
        }

        size_t last = value.find_last_not_of(" \t\r\n");

        return value.substr(first, last - first + 1);
    }

    // Converts a name to lowercase for case-insensitive comparisons.
    string normalize(const string& value) {
        string result = trim(value);

        transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) {
                return static_cast<char>(tolower(c));
            });

        return result;
    }

    bool sameName(const string& first, const string& second) {
        return normalize(first) == normalize(second);
    }
}

// Creates an empty department tree.
DepartmentTree::DepartmentTree() {
    root = nullptr;
}

// Deletes the entire tree when the object is destroyed.
DepartmentTree::~DepartmentTree() {
    deleteTree(root);
}

// Recursively deletes every department node.
void DepartmentTree::deleteTree(Department* cur) {
    if (cur == nullptr) {
        return;
    }

    for (Department* child : cur->children) {
        deleteTree(child);
    }

    delete cur;
}

// Recursively searches for a department by name.
Department* DepartmentTree::findDept(
    Department* cur, string deptName) const {

    if (cur == nullptr) {
        return nullptr;
    }

    if (sameName(cur->deptName, deptName)) {
        return cur;
    }

    for (Department* child : cur->children) {
        Department* result = findDept(child, deptName);

        if (result != nullptr) {
            return result;
        }
    }

    return nullptr;
}

// Finds the parent department's name.
string DepartmentTree::findParent(
    Department* cur, Department* target) const {

    if (cur == nullptr || target == nullptr) {
        return "";
    }

    for (Department* child : cur->children) {
        if (child == target) {
            return cur->deptName;
        }

        string result = findParent(child, target);

        if (!result.empty()) {
            return result;
        }
    }

    return "";
}

// Finds the depth of a department.
// The root has a depth of 0.
int DepartmentTree::findDepth(
    Department* cur, Department* target, int depth) const {

    if (cur == nullptr || target == nullptr) {
        return -1;
    }

    if (cur == target) {
        return depth;
    }

    for (Department* child : cur->children) {
        int result = findDepth(child, target, depth + 1);

        if (result != -1) {
            return result;
        }
    }

    return -1;
}

// Adds a department to the tree.
bool DepartmentTree::addDept(
    string deptName, string parentName) {

    deptName = trim(deptName);
    parentName = trim(parentName);

    if (deptName.empty()) {
        cout << "Error: Department name cannot be empty." << endl;
        return false;
    }

    // Prevent duplicate department names.
    if (findDept(root, deptName) != nullptr) {
        cout << "Error: Department already exists: "
             << deptName << endl;
        return false;
    }

    // If the tree is empty, create the root.
    if (root == nullptr) {
        if (!parentName.empty() && !sameName(parentName, "NONE")) {
            cout << "Error: Root department cannot have a parent."
                 << endl;
            return false;
        }

        root = new Department;
        root->deptName = deptName;

        cout << "Added root department: " << deptName << endl;
        return true;
    }

    // Find the requested parent.
    Department* parent = findDept(root, parentName);

    if (parent == nullptr) {
        cout << "Error: Parent department not found: "
             << parentName << endl;
        return false;
    }

    // Create the new department and attach it to its parent.
    Department* newDept = new Department;
    newDept->deptName = deptName;

    parent->children.push_back(newDept);

    cout << "Added department: " << deptName
         << " under " << parentName << endl;

    return true;
}

// Loads departments from a text file.
// Expected format: DepartmentName|ParentName
void DepartmentTree::loadDepartments(string fileName) {
    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cout << "Error: Could not open file: "
             << fileName << endl;
        return;
    }

    string line;
    int lineNumber = 0;

    while (getline(inputFile, line)) {
        lineNumber++;

        line = trim(line);

        // Skip blank lines.
        if (line.empty()) {
            continue;
        }

        // Skip comment lines.
        if (line[0] == '#') {
            continue;
        }

        // Find the separator.
        size_t separator = line.find('|');

        if (separator == string::npos ||
            line.find('|', separator + 1) != string::npos) {

            cout << "Warning: " << fileName
                 << ", line " << lineNumber
                 << ": Invalid format. Expected DepartmentName|ParentName."
                 << endl;
            continue;
        }

        string deptName = trim(line.substr(0, separator));
        string parentName = trim(line.substr(separator + 1));

        if (deptName.empty() || parentName.empty()) {
            cout << "Warning: " << fileName
                 << ", line " << lineNumber
                 << ": Department name or parent name is empty."
                 << endl;
            continue;
        }

        // The first valid department must have NONE as its parent.
        if (root == nullptr && !sameName(parentName, "NONE")) {
            cout << "Warning: " << fileName
                 << ", line " << lineNumber
                 << ": First department must have parent NONE."
                 << endl;
            continue;
        }

        // Every department after the root must have an existing parent.
        if (root != nullptr && sameName(parentName, "NONE")) {
            cout << "Warning: " << fileName
                 << ", line " << lineNumber
                 << ": Only the root department can have parent NONE."
                 << endl;
            continue;
        }

        if (!addDept(deptName, parentName)) {
            cout << "Warning: " << fileName
                 << ", line " << lineNumber
                 << ": Department was not added."
                 << endl;
        }
    }

    inputFile.close();
}

// Searches for a department and displays its information.
void DepartmentTree::searchDept(string deptName) const {
    Department* found = findDept(root, deptName);

    if (found == nullptr) {
        cout << "Department not found: " << deptName << endl;
        return;
    }

    string parentName = findParent(root, found);
    int depth = findDepth(root, found, 0);

    cout << "\nDepartment Information" << endl;
    cout << "Name: " << found->deptName << endl;

    if (found == root) {
        cout << "Parent: None" << endl;
    } else {
        cout << "Parent: " << parentName << endl;
    }

    cout << "Number of children: "
         << found->children.size() << endl;

    cout << "Depth: " << depth << endl;
}

// Recursively displays the department hierarchy.
void DepartmentTree::showTree(
    Department* cur, int depth) const {

    if (cur == nullptr) {
        return;
    }

    for (int i = 0; i < depth; i++) {
        cout << "    ";
    }

    cout << cur->deptName << endl;

    for (Department* child : cur->children) {
        showTree(child, depth + 1);
    }
}

// Displays the complete department tree.
void DepartmentTree::displayTree() const {
    if (root == nullptr) {
        cout << "Department tree is empty." << endl;
        return;
    }

    cout << "\nDepartment Hierarchy" << endl;
    showTree(root, 0);
}

// Prints departments in preorder.
void DepartmentTree::preorder(Department* cur) const {
    if (cur == nullptr) {
        return;
    }

    cout << cur->deptName << endl;

    for (Department* child : cur->children) {
        preorder(child);
    }
}

// Displays preorder traversal.
void DepartmentTree::displayPreorder() const {
    if (root == nullptr) {
        cout << "Department tree is empty." << endl;
        return;
    }

    cout << "\nPreorder Traversal" << endl;
    preorder(root);
}

// Prints departments in postorder.
void DepartmentTree::postorder(Department* cur) const {
    if (cur == nullptr) {
        return;
    }

    for (Department* child : cur->children) {
        postorder(child);
    }

    cout << cur->deptName << endl;
}

// Displays postorder traversal.
void DepartmentTree::displayPostorder() const {
    if (root == nullptr) {
        cout << "Department tree is empty." << endl;
        return;
    }

    cout << "\nPostorder Traversal" << endl;
    postorder(root);
}

// Displays departments level by level using a queue.
void DepartmentTree::displayLevelOrder() const {
    if (root == nullptr) {
        cout << "Department tree is empty." << endl;
        return;
    }

    cout << "\nLevel-Order Traversal" << endl;

    queue<Department*> departments;
    departments.push(root);

    while (!departments.empty()) {
        Department* current = departments.front();
        departments.pop();

        cout << current->deptName << endl;

        for (Department* child : current->children) {
            departments.push(child);
        }
    }
}

// Counts all department nodes.
int DepartmentTree::countNodes(Department* cur) const {
    if (cur == nullptr) {
        return 0;
    }

    int count = 1;

    for (Department* child : cur->children) {
        count += countNodes(child);
    }

    return count;
}

// Counts departments that do not have children.
int DepartmentTree::countLeaves(Department* cur) const {
    if (cur == nullptr) {
        return 0;
    }

    if (cur->children.empty()) {
        return 1;
    }

    int count = 0;

    for (Department* child : cur->children) {
        count += countLeaves(child);
    }

    return count;
}

// Finds the tree height.
// A root-only tree has a height of 0.
int DepartmentTree::getHeight(
    Department* cur, int depth) const {

    if (cur == nullptr) {
        return -1;
    }

    int maxDepth = depth;

    for (Department* child : cur->children) {
        int childHeight = getHeight(child, depth + 1);

        if (childHeight > maxDepth) {
            maxDepth = childHeight;
        }
    }

    return maxDepth;
}

// Displays department tree statistics.
void DepartmentTree::displayStats() const {
    if (root == nullptr) {
        cout << "Department tree is empty." << endl;
        return;
    }

    cout << "\nDepartment Tree Statistics" << endl;
    cout << "Total departments: "
         << countNodes(root) << endl;

    cout << "Leaf departments: "
         << countLeaves(root) << endl;

    cout << "Tree height: "
         << getHeight(root, 0) << endl;
}
