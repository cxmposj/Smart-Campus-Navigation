#include "Facility.h"
#include <iostream>
using namespace std;

Facility::Facility() {      // The default constructor of the facility information
   
    facID = "";
    facName = "";
    buidlingName = "";
    facCategory = "" :
}

Facility::Facility(string newID, string newName, string newBuilding, string newCategory) {     //The constructor that saves the details about the facility
    facID = newID;
    facName = newName;
    buildingName = newBuilding;
    facCategory = newCategory;
}

string Facility::getFacID() const {
    return facID;                                   // Returns the ID of the facility
}
string Facility::getFacName() const {              
    return facName;                                 
}
string Facility::getBuildingName() const {        // Gets the name and returns the name 
    return buidlingName;
}
string Facility::getFacCategory() const{          // Gets the category of the facility
    return facCategory;
}

void Facility::displayFacility() const{            //This would display all the stored facility details
    cout << "Facility ID: " << facID << endl;
    cout << "Facility Name: " << facName << endl;
    cout << "Building: " << buildingName << endl;
    cout << "Category: " << facCategory << endl;
}
