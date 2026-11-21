#ifndef FACILITY_H
#define FACILITY_H
#include <string>
using namespace std;

class Facility {
    private:
        string facID;
        string facName;          //Stores the information about each facility 
        string buildingName;
        string facCategory;

    public:
        Facility();       // It creates an empty facility 

        Facility(string newID, string newName, string newBuilding, string newCategory);     //This is used to save the informatiom entered for a facility 

        string getFacID() const;
        string getFacName() const;          //It returns the facility ID, name, and the category
        string getBuildingName() const;
        string getFacCategory() const;

        void displayFacility() const;         //This displays all the facility details 
};

#endif
