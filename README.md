# Smart-Campus-Navigation
Project 2 - Smart Campus Navigation and Facility Analytics System 

# Project Description
This project is a Smart Campus Navigation and Facility Analytics System that organizes campus buildings, facilities, connections, and departments. It allows users to search for facilities, navigate between buildings, find reachable locations, and view department information. It also provides different reports to show information about the facilities and campus connections. 

# Project Overview
The system uses campus information stored in text files. It organizes facility records, building connections, and department information to provide different campus navigation and searching features. The program uses different classes to handle the information and connects them through the main menu.

# Team Member Contributions
1. Jose Campos - Developed the facility record class and custom hash table to organize campus facility information. Added functions for storing facility, handling duplicate IDs, and looking up facilities by using their IDs. Also made certain the facility information was properly stored and accessible through the hash table. For the final, added more ways to search for facilities, including searching by name and category. Also worked on displaying all facility records and creating the facility category report, which shows the number and percentage of facilities in each category. 
2. Adeoluwa Olukotun - Created the campus to represent buildings and their connections. Added the BFS traversal to determine which buildings can be reached from a starting location and helped ensure the graph features function correctly. Also added a feature to find the shortest path between two buildings by using BFS. The graph can display the buildings and their connections, along with information like the total number of buildings, connections, and the degree of each building. A graph connectivity report was added to show the buildings are connected and whether they can reach one another. 
4. Rj Amuebie - Worked on the department hierarchy by organizing departments into a tree structure. Added functionality for creating departments, searching for department information, and displaying the hierarchy through preorder traversal. I added postorder and level-order traversals to give the user more ways to view the department hierarchy. The department statistics provide additional information about the tree, total number of departments, number of leaf nodes, and the height of the tree. These features help show how the departments are organized and provide more information about the department structure. 

# Main Features
- Display all campus facilities
- Search facilities by ID using a custom hash table
- Search facilities by name and category
- Display facility category reports
- Show campus buildings and connections
- Display graph counts and building degrees
- Find reachable buildings using BFS
- Find the shortest path between buildings
- Add and search departments
- Display the department hierarchy
- Perform preorder, postorder, and level-order traversals 
- Display department statistics 
- Display graph connectivity reports 

# How to Compile and Run
VsCode terminal: g -I. -Iinclude src/DepartmentTree.cpp src/CampusGraph.cpp Facility.cpp HashTable.cpp main.cpp -o CampusNavigation

UNT Terminal: g++ -std=c++17 -Wall DepartmentTree.cpp CampusGraph.cpp Facility.cpp HashTable.cpp main.cpp -o CampusNavigation

Ran:

.\CampusNavigation.exe

# Input files
The program uses four input files provided for the project:

- buildings.txt
- connections.txt
- facilities.txt
- departments.txt

These files are located in the data folder and are used to load the campus information when the program starts. 

# Project Organization

The project is organized into different folders and files to keep the code and campus information together. 

- Header files contain the class declarations.
- Source files contain the class functions and the program code.
- Main.cpp connects the different classes and displays the main menu.
- The data folder contains the four input files.
- README explains the project and how to run our code.
- Design_document.txt explains how the system is organized.
- Contributions.txt lists the work completed by each team member.
- Test_results.txt contains the tests of our code and our results. 
  
# Testing
The system is tested using the provided input files and additional test data to check facility searching, hash table collisions, graph traversal, department operations, reports, and invalid input handling. 

# GitHub Repository
https://github.com/cxmposj/Smart-Campus-Navigation
