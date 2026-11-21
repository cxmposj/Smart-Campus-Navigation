
#ifndef DEPARTMENTTREE_H
#define DEPARTMENTTREE_H

#include <string>
#include <vector>

using namespace std;

struct Department {
    string deptName;
    vector<Department*> children;
};

class DepartmentTree {
private:
    Department* root;

    Department* findDept(Department* cur, string deptName) const;
    string findParent(Department* cur, Department* target) const;
    int findDepth(Department* cur, Department* target, int depth) const;

    void showTree(Department* cur, int depth) const;
    void preorder(Department* cur) const;
    void postorder(Department* cur) const;
    void deleteTree(Department* cur);

    int countNodes(Department* cur) const;
    int countLeaves(Department* cur) const;
    int getHeight(Department* cur, int depth) const;

public:
    DepartmentTree();
    ~DepartmentTree();

    void loadDepartments(string fileName);
    bool addDept(string deptName, string parentName);

    void searchDept(string deptName) const;
    void displayTree() const;
    void displayPreorder() const;
    void displayPostorder() const;
    void displayLevelOrder() const;
    void displayStats() const;
};

#endif

