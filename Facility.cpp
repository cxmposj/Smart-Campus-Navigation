
#include "Facility.h"
#include <iostream>

using namespace std;

Facility::Facility() {
    facID = "";
    facName = "";
    buildingName = "";
    facCategory = "";
}

Facility::Facility(string newID, string newName, string newBuilding, string newCategory) {
    facID = newID;
    facName = newName;
    buildingName = newBuilding;
    facCategory = newCategory;
}

string Facility::getFacID() const {
    return facID;
}

string Facility::getFacName() const {
    return facName;
}

string Facility::getBuildingName() const {
    return buildingName;
}

string Facility::getFacCategory() const {
    return facCategory;
}

void Facility::displayFacility() const {
    cout << "Facility ID: " << facID << endl;
    cout << "Facility Name: " << facName << endl;
    cout << "Building: " << buildingName << endl;
    cout << "Category: " << facCategory << endl;
}
