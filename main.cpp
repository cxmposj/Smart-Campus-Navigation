
#include "DepartmentTree.h"
#include "CampusGraph.h"
#include "Facility.h"
#include "HashTable.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>

using namespace std;

// Removes whitespace from the beginning and end of a string.
string trimText(const string& value) {
    const string whitespace = " \t\r\n";
    size_t first = value.find_first_not_of(whitespace);

    if (first == string::npos) {
        return "";
    }

    size_t last = value.find_last_not_of(whitespace);
    return value.substr(first, last - first + 1);
}

// Splits a facility record using the | separator.
vector<string> splitFacilityLine(const string& line) {
    vector<string> fields;
    string field;
    stringstream stream(line);

    while (getline(stream, field, '|')) {
        fields.push_back(trimText(field));
    }

    return fields;
}

// Loads facilities into the hash table.
void loadFacilities(HashTable& table, const CampusGraph& graph) {
    ifstream file("data/facilities.txt");

    if (!file.is_open()) {
        cout << "Error: Could not open data/facilities.txt\n";
        return;
    }

    string line;
    int loaded = 0;
    int rejected = 0;
    int lineNumber = 0;

    while (getline(file, line)) {
        lineNumber++;

        line = trimText(line);

        if (line.empty() || line[0] == '#') {
            continue;
        }

        vector<string> fields = splitFacilityLine(line);

        if (fields.size() != 4) {
            cout << "Warning: Invalid facility record on line "
                 << lineNumber << ".\n";
            rejected++;
            continue;
        }

        string facilityID = fields[0];
        string facilityName = fields[1];
        string buildingName = fields[2];
        string category = fields[3];

        if (facilityID.empty() || facilityName.empty() ||
            buildingName.empty() || category.empty()) {
            cout << "Warning: Missing facility information on line "
                 << lineNumber << ".\n";
            rejected++;
            continue;
        }

        string canonicalBuilding;

        if (!graph.findBuilding(buildingName, canonicalBuilding)) {
            cout << "Warning: Building not found for facility "
                 << facilityID << ".\n";
            rejected++;
            continue;
        }

        Facility facility(
            facilityID,
            facilityName,
            canonicalBuilding,
            category
        );

        if (table.addFac(facility)) {
            loaded++;
        } else {
            cout << "Warning: Could not add facility "
                 << facilityID << ".\n";
            rejected++;
        }
    }

    cout << "\nFacility loading complete.\n";
    cout << "Facilities loaded: " << loaded << endl;
    cout << "Facilities rejected: " << rejected << endl;
}

// Displays the main menu.
void displayMenu() {
    cout << "\n========== SMART CAMPUS NAVIGATION ==========\n";
    cout << "1. Display department hierarchy\n";
    cout << "2. Search for a department\n";
    cout << "3. Display campus buildings and connections\n";
    cout << "4. Display building statistics\n";
    cout << "5. Find reachable buildings\n";
    cout << "6. Display all facilities\n";
    cout << "7. Search for a facility by ID\n";
    cout << "8. Display hash table statistics\n";
    cout << "9. Exit\n";
    cout << "Enter your choice: ";
}

int main() {

    // Initialize the department tree.
    DepartmentTree departmentTree;

    cout << "Loading departments...\n";
    departmentTree.loadDepartments("data/departments.txt");

    // Initialize the campus graph.
    CampusGraph campusGraph;

    cout << "\nLoading campus buildings...\n";
    campusGraph.loadBuildings("data/buildings.txt", cout);

    cout << "\nLoading campus connections...\n";
    campusGraph.loadConnections("data/connections.txt", cout);

    // Initialize the facility hash table.
    HashTable facilityTable(101);

    cout << "\nLoading campus facilities...\n";
    loadFacilities(facilityTable, campusGraph);

    int choice = 0;

    while (true) {

        displayMenu();

        if (!(cin >> choice)) {
            if (cin.eof()) {
                cout << "\nInput ended. Exiting program.\n";
                break;
            }

            cout << "\nInvalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {

            case 1:
                cout << "\nDepartment Hierarchy\n";
                departmentTree.displayTree();
                break;

            case 2: {
                string departmentName;

                cout << "Enter department name: ";
                getline(cin, departmentName);

                departmentTree.searchDept(departmentName);
                break;
            }

            case 3:
                cout << "\nCampus Buildings and Connections\n";
                campusGraph.displayGraph(cout);
                break;

            case 4:
                cout << "\nBuilding Statistics\n";
                campusGraph.displayCountsAndDegrees(cout);
                break;

            case 5: {
                string buildingName;

                cout << "Enter starting building: ";
                getline(cin, buildingName);

                cout << "\nReachable Buildings\n";
                campusGraph.displayReachable(buildingName, cout);
                break;
            }

            case 6:
                cout << "\nAll Campus Facilities\n";
                facilityTable.displayTable();

                if (facilityTable.getFacCount() == 0) {
                    cout << "No facilities are currently loaded.\n";
                }
                break;

            case 7: {
                string facilityID;

                cout << "Enter facility ID: ";
                getline(cin, facilityID);

                Facility* facility = facilityTable.searchFac(facilityID);

                if (facility != nullptr) {
                    cout << "\nFacility Found\n";
                    facility->displayFacility();
                } else {
                    cout << "Facility not found.\n";
                }

                break;
            }

            case 8:
                cout << "\nHash Table Statistics\n";
                cout << "Facilities stored: "
                     << facilityTable.getFacCount() << endl;
                cout << "Table capacity: "
                     << facilityTable.getTableSize() << endl;
                cout << "Total collisions: "
                     << facilityTable.getColCount() << endl;
                break;

            case 9:
                cout << "\nThank you for using Smart Campus Navigation!\n";
                return 0;

            default:
                cout << "\nInvalid choice. Please select 1-9.\n";
        }
    }

    return 0;
}
