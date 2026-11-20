
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
#include <cctype>
#include <iomanip>

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

// Loads facilities into the hash table and the facility list.
void loadFacilities(HashTable& table, const CampusGraph& graph, vector<Facility>& facilities) {
    ifstream file("data/facilities.txt");

    facilities.clear();

    string line;
    int loaded = 0;
    int rejected = 0;
    int lineNumber = 0;

    if (!file.is_open()) {
        cout << "Error: Could not open data/facilities.txt" << endl;
    }
    else {
        while (getline(file, line)) {
            lineNumber++;

            line = trimText(line);

            if (line.empty() || line[0] == '#') {
            continue;
            }

        vector<string> fields = splitFacilityLine(line);

        if (fields.size() == 4 && fields[0] == "FacilityID" &&        //This would skip the facility file header
            fields[1] == "FacilityName") {
            continue;
        }

        if (fields.size() != 4) {                        
            cout << "Warning: Invalid facility record in facilities.txt on line "
                 << lineNumber << "." << endl;
            rejected++;
            continue;
        }

        string facilityID = fields[0];
        string facilityName = fields[1];
        string buildingName = fields[2];
        string category = fields[3];

        if (facilityID.empty() || facilityName.empty() ||
            buildingName.empty() || category.empty()) {
            cout << "Warning: Missing facility information in facilities.txt on line "
                 << lineNumber << "." << endl;
            rejected++;
            continue;
        }

        for (int i = 0; i < facilityID.length(); i++) {            //It changes the facility ID to uppercase
            facilityID[i] = toupper((unsigned char)facilityID[i]);
        }

        if(facilityID.length() != 4 || facilityID[0] != 'F' ||
            !isdigit((unsigned char)facilityID[1]) ||
            !isdigit((unsigned char)facilityID[2]) ||
            !isdigit((unsigned char)facilityID[3])) {
        
        cout << "Warning: Invalid facility ID in facilities.txt on line "
            << lineNumber << "." << endl;
            rejected++;
            continue;
        }

        string canonicalBuilding;

        if (!graph.findBuilding(buildingName, canonicalBuilding)) {
            cout << "Warning: Building not found for facility "
                 << facilityID << " in facilities.txt on line " << lineNumber << "." << endl;
            rejected++;
            continue;
        }

        Facility facility(
            facilityID,
            facilityName,
            canonicalBuilding,
            category
        );

        if (table.searchFac(facilityID) != nullptr) {            //This is used to check for duplicate facility IDs
            cout << "Warning: Duplicate facility ID in facilities.txt on line " 
                << lineNumber << "." << endl;
            rejected++;
            continue;
        }

        if (table.addFac(facility)) {
            facilities.push_back(facility);
            loaded++;
        } else {
            cout << "Warning: Could not add facility "
                 << facilityID << " from facilities.txt on line " << lineNumber << "." << endl;
            rejected++;
        }
    }
}

    cout << "\nFacility loading complete.\n";
    cout << "File: facilities.txt" << endl;
    cout << "Facilities loaded: " << loaded << endl;
    cout << "Facilities rejected: " << rejected << endl;
    cout << "Table capacity: " << table.getTableSize() << endl;
    cout << "Facilities stored: " << table.getFacCount() << endl;
    cout << "Total collisions: " << table.getColCount() << endl;
    
}

// Displays the main menu.
void displayMenu() {
    cout << "\n========== SMART CAMPUS NAVIGATION ==========\n";
    cout << "1. View all facilities\n";
    cout << "2. Search facility by ID\n";
    cout << "3. Search facility by name\n";
    cout << "4. Searcn facility by category\n";
    cout << "5. View campus graph\n";
    cout << "6. Display graph counts and degrees\n";
    cout << "7. Find reachable buildings (BFS)\n";
    cout << "8. Find shortest path (BFS)\n";
    cout << "9. Add department\n";
    cout << "10. Search department\n";
    cout << "11. Display department hierachy\n";
    cout << "12. Preorder traversal\n";
    cout << "13. Postorder traversal\n";
    cout << "14. Level-order traversal\n";
    cout << "15. Department statistics\n";
    cout << "16. Facility category report\n";
    cout << "17. Graph connectivity report\n";    
    cout << "18. Exit\n";
    cout << "Enter your choice: ";
}

void displayCategoryReport(const vector<Facility>& facilities) {

    cout << "\nFacility Category Report\n";

    if (facilities.empty()) {
        cout << "No facilities available.\n";
        cout << "Total facilities: 0\n";
        return;
    }
    vector<string> categories;
    vector<int> categoryCounts;

    for (int i = 0; i < facilities.size(); i++) {

        string currentCategory = facilities[i].getFacCategory();

        for (int j = 0; j < currentCategory.length(); j++) {
            currentCategory[j] = tolower((unsigned char)currentCategory[j]);
        }
    bool found = false;

    for (int j = 0; j < categories.size(); j++) {
        string storedCategory = categories[j];

    for (int k = 0; k < storedCategory.length(); k++) {
        storedCategory[k] = tolower((unsigned char)storedCategory[k]);
    }
    if (currentCategory == storedCategory) {
        categoryCounts[j]++;
        found = true;
        break;
    }
 }
    if (!found) {
        categories.push_back(facilities[i].getFacCategory());
        categoryCounts.push_back(1);
    }
}
    for (int i = 0; i < categories.size(); i++) {
        double percentage = (double)categoryCounts[i]/facilities.size() * 100;
    cout << categories[i] << ": " << categoryCounts[i] << " (" << fixed << setprecision(1)
         << percentage << "%)" << endl;
    }

    cout << "Total facilities: " << facilities.size() << endl;
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
    vector<Facility> facilities;

    cout << "\nLoading campus facilities...\n";
    loadFacilities(facilityTable, campusGraph, facilities);

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
                cout << "\nAll Campus Facilities\n";

                if (facilities.empty()) {
                    cout << "No facilities are currently loaded.\n";
                }
                else {
                    for (int i = 0; i < facilities.size(); i++){
                        facilities[i].displayFacility();
                    }
                    cout << "Total facilities: " << facilities.size() << endl;
                }
                break;

            case 2: {
                string facilityID;

                cout << "Enter facility ID: ";
                getline(cin, facilityID);

                facilityID = trimText(facilityID);        // This would remove any extra spaces

                for (int i = 0; i < facilityID.length(); i++) {    // It changes the ID to uppercase
                    facilityID[i] = toupper((unsigned char)facilityID[i]);
                }

                if(facilityID.length() != 4 || facilityID[0] != 'F' ||        //Checks if it follows the format
                    !isdigit((unsigned char)facilityID[1]) || 
                    !isdigit((unsigned char)facilityID[2]) ||
                    !isdigit((unsigned char)facilityID[3])) {

                    cout << "\nInvalid facility ID. Please use F followed by 3 digits.\n";
                    break;
            }

            Facility* facility = facilityTable.searchFac(facilityID);

            if (facility != nullptr) {
                cout << "\nFacility Found\n";
                facility->displayFacility();
            }
            else {
                cout << "\nFacilityID " << facilityID << " was not found.\n";
            }
            break;
        }

            case 3: {                                    // This searches for a facility by name
                string facilityName;

                cout << "Enter facility name: ";
                getline(cin, facilityName);

                facilityName = trimText(facilityName);

                if (facilityName.empty()) {
                    cout << "\nPlease enter a facility name.\n";
                    break;
                }

                for (int i = 0; i < facilityName.length(); i++) {             // It changes the search name to lowercase   
                    facilityName[i] = tolower((unsigned char)facilityName[i]);
                }

                int foundCount = 0;

                for (int i = 0; i < facilities.size(); i++) {
                    string currentName = facilities[i].getFacName();

                    for (int j = 0; j < currentName.length(); j++) {
                        currentName[j] = tolower((unsigned char)currentName[j]);
                    }
                    if (currentName.find(facilityName) != string::npos) {
                        cout << "\nFacility Found\n";
                        facilities[i].displayFacility();
                        foundCount++;
                    }
                }
                if (foundCount == 0){
                    cout << "\nNo facilities found with that name.\n";
                }
                else {
                    cout << "Total facilities found: " << foundCount << endl;
                }

                break;
            }
        
            case 4: {
                string category;

                cout << "Enter facility category: ";
                getline(cin, category);

                category = trimText(category);

                if (category.empty()) {
                    cout << "\nPlease enter a facility category.\n";
                    break;
                }

                for (int i = 0; i < category.length(); i++) {
                    category[i] = tolower((unsigned char)category[i]);
                }

                int foundCount = 0;

                for (int i = 0; i < facilities.size(); i++) {
                    string currentCategory = facilities[i].getFacCategory();

                    for (int j = 0; j < currentCategory.length(); j++) {
                        currentCategory[j] = tolower((unsigned char)currentCategory[j]);
                    }
                    if (currentCategory == category) {
                        cout << "\nFacility Found\n";
                        facilities[i].displayFacility();
                        foundCount++;
                    }
                }
                if (foundCount == 0) {
                    cout << "\nNo facilities found in that category.\n";
                }
                else {
                    cout << "Total facilities found: " << foundCount << endl;
                }

                break;
            }

            case 5: 
                cout << "\nCampus Buildings and Connections\n";
                campusGraph.displayGraph(cout);
                break;

            case 6: 
                cout << "\nBuilding Statistics\n";
                campusGraph.displayCountsAndDegrees(cout);
                break;

            case 7: {
                string buildingName;

                cout << "Enter starting building: ";
                getline(cin, buildingName);

                cout << "\nReachable Buildings\n";
                campusGraph.displayReachable(buildingName, cout);
                break;
            }

            case 8: {                                 // Find shortest path (BFS)

                break;
            }

            case 9: {                        // Add the department displaymenu

                break;
            }

            case 10: {
                string departmentName;

                cout << "Enter department name: ";
                getline(cin, departmentName);

                departmentTree.searchDept(departmentName);
                break;
            }

            case 11: 
                cout << "\nDepartment Hierarchy\n";
                departmentTree.displayTree();
                break;

            case 12: {                                    // For preorder traversal

                break;
            }

            case 13: {                                    // This is for the postorder traversal

                break;
            }

            case 14: {                                    // The level-order traversal displaymenu

                break;
            }

            case 15: {                                    // The department statistics
                
                break;
            }

            case 16: 
                displayCategoryReport(facilities);
                break;
            
            case 17: {                                    //This is the graph connectivity report case on displaymenu
                break;
            }
            
            case 18:
                cout << "\nThank you for using Smart Campus Navigation!\n";
                return 0;

            default:
                cout << "\nInvalid choice. Please select 1-9.\n";
        }
    }

    return 0;
}
