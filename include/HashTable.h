#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include "Facility.h"
using namespace std;

class HashTable {                       // This stores the facilities inside the custom hash table
    private:
        Facility* table;
        int tableSize;
        int facCount;
        int colCount;
        int hashFunc(string facID) const;      // This is used to find the position for a facility ID

    public:
        HashTable(int size);                    // It creates an empty hash table and then releases the memory used by the table
        ~HashTable();

        bool addFac(Facility newFacility);     // This adds a facility and searches for it using its ID
        Facility* searchFac(string facID);

        void displayTable() const;              // Used to display all facilities in the hash table

        int getFacCount() const;             
        int getTableSize() const;           // It returns the number of faciltiies stored and total capacity of the hash table

        int getColCount() const;         // Returns the number of the collisions
};

#endif
