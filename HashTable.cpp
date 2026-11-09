#include "HashTable.h"
#include <iostream>
#include <cctype>
using namespace std;

int HashTable::hashFunc(string facID) const {               // This would look for the posiion of a facility ID
    int total = 0;
    for (int i = 0; i < facID.length(); i++){
        total += toupper((unsigned char)facID[i]);
    }
    return total % tableSize;
}

HashTable::HashTable(int size) {        // Creates an empty hash table
    tableSize = size;
    facCount = 0;
    colCount = 0;

    if(tableSize > 0) {
        table = new Facility[tableSize];
    }
    else {
        table = nullptr;
        tableSize = 0;
    }
}

HashTable::~HashTable() {         // Deletes the memory used by the table
    delete [] table;
}

bool HashTable::addFac(Facility newFacility) {   // Adds a facility to the table
    
    if(tableSize == 0) {
        return false;
    }

    string facID = newFacility.getFacID();

    for (int i = 0; i < facID.length(); i++) {
        facID[i] = toupper((unsigned char)facID[i]);
    }

    if (facID == "") {                          // It changes the facility ID to uppercase
        return false;
    }

    int index = hashFunc(facID);
    int collisions = 0;

    for (int i = 0; i < tableSize; i++) {
        if (table[index].getFacID() == facID){      // This checks for duplicates and then find an empty position
            return false;
        }
    
    if (table[index].getFacID() == "") {
        Facility fac(facID, newFacility.getFacName(), newFacility.getBuildingName(), newFacility.getFacCategory());

        table[index] = fac;
        facCount++;
        colCount += collisions;                         

        return true;
    }
        collisions++;
        index = (index + 1) % tableSize;
    }
    cout << "Hash table is full. Facility could not be added." << endl;
    return false; 
}

Facility* HashTable::searchFac(string facID) {     // It searches for a facility by using its ID
    if(tableSize == 0) {
        return nullptr;
    }

    for (int i = 0; i < facID.length(); i++) {
        facID[i] = toupper((unsigned char)facID[i]);        // Changes the facility ID to uppercase
    }

    if (facID == "") {
        return nullptr;
    }

    int index = hashFunc(facID);

    for (int i = 0; i < tableSize; i++) {
        if (table[index].getFacID() == "") {
            return nullptr;
        }                                              // It goes through the table for the facility ID

        if (table[index].getFacID() == facID) {
            return &table[index];
        }
        index = (index + 1) % tableSize;
    }
    return nullptr;
}

void HashTable::displayTable() const {
    for (int i = 0; i < tableSize; i++) {              // It displays the facilities stored in the hash table
        if (table[i].getFacID() != "") {
            cout << "Position " << i << ":" << endl;
            table[i].displayFacility();
            cout << endl;
        }
    }
}

int HashTable::getFacCount() const {
    return facCount;                        // Returns the number of the facilities stored
}

int HashTable::getTableSize() const {
    return tableSize;                       // It returns the total capacity of the hash table
}

int HashTable::getColCount() const {
    return colCount;                        // This returns the number of collisions
}
