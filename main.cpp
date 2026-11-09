
#include "DepartmentTree.h"
#include <iostream>

using namespace std;

int main() {

    cout << "========== NORMAL DEPARTMENT TESTS ==========" << endl;

    DepartmentTree tree;

    tree.loadDepartments("data/departments.txt");

    tree.displayTree();
    tree.displayPreorder();
    tree.displayPostorder();
    tree.displayLevelOrder();
    tree.displayStats();

    cout << "\n========== ERROR-CASE TESTS ==========" << endl;

    // Test 1: Search for a department that does not exist.
    cout << "\nTest 1: Search for missing department" << endl;
    tree.searchDept("Business Administration");

    // Test 2: Attempt to add a duplicate department.
    cout << "\nTest 2: Add duplicate department" << endl;
    tree.addDept("Computer Science", "College of Engineering");

    // Test 3: Attempt to add a department with an unknown parent.
    cout << "\nTest 3: Add department with unknown parent" << endl;
    tree.addDept("Artificial Intelligence", "Unknown College");

    // Test 4: Test an empty department file.
    cout << "\nTest 4: Load empty department file" << endl;
    DepartmentTree emptyTree;
    emptyTree.loadDepartments("data/empty_departments.txt");
    emptyTree.displayTree();
    emptyTree.displayStats();

    // Test 5: Test a file containing invalid records.
    cout << "\nTest 5: Load file with invalid records" << endl;
    DepartmentTree invalidTree;
    invalidTree.loadDepartments("data/invalid_departments.txt");
    invalidTree.displayTree();

    return 0;
}

