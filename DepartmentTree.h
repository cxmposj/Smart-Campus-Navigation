#ifndef DEPARTMENTTREE_H
#define DEPARTMENTTREE_H

#include <string>
#include <vector>
uaing namespace std;

struct Department {                     // This stores a department and its child departments
    string deptName;
    vector<Department*> children;
};

class DepartmentTree {                 // It handles the department tree and its operations
    private:
        Department* root;

        Department* findDept(Department* cur, string deptName) const;
        string findParent(Department* cur, Department* target) const;           //Searches the tree, finds the parent and depth of a department
        int findDepth(Department* cur, Department* target, int depth) const;

        void showTree(Department* cur, int depth) const;
        void preorder(Department* cur) const;           // Dislays, prints in preorder, and then deletes all nodes in the tree
        void deleteTree(Department* cur);
   
    public:
        DepartmentTree();                   // Creates an empty department tree and the bottom one deletes it
        ~DepartmentTree();

        void loadDepartments(string fileName);              // Loads and adds a department
        bool addDept(string deptName, string parentName);

        void searchDept(string deptName) const;         // This searches for the department and display the information
        void displayTree() const;                  
        void displayPreorder() const;           // Displays the department hierarchy and preorder traversal 

};

#endif
