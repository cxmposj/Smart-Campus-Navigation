// Implements an ordered adjacency list and queue-based BFS for campus buildings.
// Author: Adeoluwa Olukotun
#include "CampusGraph.h"

#include <algorithm>
#include <fstream>
#include <ostream>
#include <queue>

namespace {
// Removes outer whitespace, including the carriage return in Windows text files.
std::string trim(const std::string& value) {
    const std::string whitespace = " \t\r\n\f\v";
    const std::size_t first = value.find_first_not_of(whitespace);
    if (first == std::string::npos) return "";
    const std::size_t last = value.find_last_not_of(whitespace);
    return value.substr(first, last - first + 1);
}

// Normalizes ASCII letters for comparison without changing internal whitespace.
std::string nameKey(const std::string& value) {
    std::string key = trim(value);
    for (char& letter : key) {
        if (letter >= 'A' && letter <= 'Z') letter += 'a' - 'A';
    }
    return key;
}

// Splits the complete line, retaining empty fields including a trailing field.
std::vector<std::string> splitFields(const std::string& line) {
    std::vector<std::string> fields;
    std::size_t first = 0;
    while (true) {
        const std::size_t separator = line.find('|', first);
        if (separator == std::string::npos) {
            fields.push_back(trim(line.substr(first)));
            return fields;
        }
        fields.push_back(trim(line.substr(first, separator - first)));
        first = separator + 1;
    }
}

// Only a first non-whitespace '#' marks a comment; embedded '#' is a name part.
bool isIgnoredLine(const std::string& line) {
    const std::string cleaned = trim(line);
    return cleaned.empty() || cleaned[0] == '#';
}

// Records one rejected input record with the physical file line number.
void rejectRecord(const std::string& filePath, std::size_t lineNumber,
                  const std::string& reason, CampusGraph::LoadSummary& summary,
                  std::ostream& out) {
    ++summary.rejected;
    out << "Warning: " << filePath << ':' << lineNumber << ": " << reason << '\n';
}

// Prints a summary even when the file is empty or cannot be opened.
void printSummary(const std::string& filePath,
                  const CampusGraph::LoadSummary& summary, std::ostream& out) {
    out << filePath << ": " << summary.accepted << " accepted, "
        << summary.rejected << " rejected\n";
}
} // namespace

std::size_t CampusGraph::findIndex(const std::string& name) const {
    const std::string key = nameKey(name);
    for (std::size_t i = 0; i < buildings_.size(); ++i) {
        if (buildings_[i].key == key) return i;
    }
    return buildings_.size();
}

bool CampusGraph::findBuilding(const std::string& name,
                               std::string& canonicalName) const {
    canonicalName.clear();
    const std::size_t index = findIndex(name);
    if (index == buildings_.size()) return false;
    canonicalName = buildings_[index].name;
    return true;
}

bool CampusGraph::addBuilding(const std::string& name, std::string& error) {
    error.clear();
    const std::string cleaned = trim(name);
    if (cleaned.empty()) {
        error = "Building name must not be blank.";
        return false;
    }
    if (cleaned.find('|') != std::string::npos) {
        error = "Building name must not contain the field separator '|'.";
        return false;
    }
    if (findIndex(cleaned) != buildings_.size()) {
        error = "Duplicate building: " + cleaned;
        return false;
    }
    Building building;
    building.name = cleaned;
    building.key = nameKey(cleaned);
    buildings_.push_back(building);
    return true;
}

bool CampusGraph::addConnection(const std::string& first,
                                 const std::string& second, std::string& error) {
    error.clear();
    const std::string firstName = trim(first);
    const std::string secondName = trim(second);
    if (firstName.empty() || secondName.empty()) {
        error = "Both connection building names are required.";
        return false;
    }
    const std::size_t a = findIndex(firstName);
    const std::size_t b = findIndex(secondName);
    if (a == buildings_.size() || b == buildings_.size()) {
        error = "Unknown building: " + (a == buildings_.size() ? firstName : secondName);
        return false;
    }
    if (a == b) {
        error = "Self-connection is not allowed: " + buildings_[a].name;
        return false;
    }
    const std::vector<std::size_t>& neighbors = buildings_[a].neighbors;
    if (std::find(neighbors.begin(), neighbors.end(), b) != neighbors.end()) {
        error = "Duplicate connection: " + buildings_[a].name + " | " + buildings_[b].name;
        return false;
    }

    // Store both directions in arrival order, but count the walkway once.
    buildings_[a].neighbors.push_back(b);
    buildings_[b].neighbors.push_back(a);
    ++connectionCount_;
    return true;
}

std::size_t CampusGraph::buildingCount() const { return buildings_.size(); }

std::size_t CampusGraph::connectionCount() const { return connectionCount_; }

CampusGraph::LoadSummary CampusGraph::loadBuildings(const std::string& filePath,
                                                   std::ostream& out) {
    buildings_.clear();
    connectionCount_ = 0;
    LoadSummary summary;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        out << "Error: Could not open " << filePath << '\n';
        printSummary(filePath, summary, out);
        return summary;
    }
    summary.opened = true;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        if (isIgnoredLine(line)) continue;
        const std::vector<std::string> fields = splitFields(line);
        std::string error;
        if (fields.size() != 1) {
            error = "Expected one building name field.";
        } else if (addBuilding(fields[0], error)) {
            ++summary.accepted;
            continue;
        }
        rejectRecord(filePath, lineNumber, error, summary, out);
    }
    printSummary(filePath, summary, out);
    return summary;
}

CampusGraph::LoadSummary CampusGraph::loadConnections(const std::string& filePath,
                                                     std::ostream& out) {
    for (Building& building : buildings_) building.neighbors.clear();
    connectionCount_ = 0;
    LoadSummary summary;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        out << "Error: Could not open " << filePath << '\n';
        printSummary(filePath, summary, out);
        return summary;
    }
    summary.opened = true;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        if (isIgnoredLine(line)) continue;
        const std::vector<std::string> fields = splitFields(line);
        std::string error;
        if (fields.size() != 2) {
            error = "Expected two building fields separated by '|'.";
        } else if (addConnection(fields[0], fields[1], error)) {
            ++summary.accepted;
            continue;
        }
        rejectRecord(filePath, lineNumber, error, summary, out);
    }
    printSummary(filePath, summary, out);
    return summary;
}

void CampusGraph::displayGraph(std::ostream& out) const {
    if (buildings_.empty()) {
        out << "No buildings are loaded.\n";
        return;
    }
    for (const Building& building : buildings_) {
        out << building.name << ": ";
        if (building.neighbors.empty()) out << "(no connections)";
        for (std::size_t i = 0; i < building.neighbors.size(); ++i) {
            if (i != 0) out << ", ";
            out << buildings_[building.neighbors[i]].name;
        }
        out << '\n';
    }
}

void CampusGraph::displayCountsAndDegrees(std::ostream& out) const {
    out << "Total buildings: " << buildingCount() << '\n';
    out << "Total connections: " << connectionCount() << '\n';
    if (buildings_.empty()) out << "No buildings are loaded.\n";
    for (const Building& building : buildings_) {
        out << building.name << ": degree " << building.neighbors.size() << '\n';
    }
}

bool CampusGraph::findReachable(const std::string& start,
                               std::vector<ReachableBuilding>& result,
                               std::string& error) const {
    result.clear();
    error.clear();
    if (trim(start).empty()) {
        error = "Starting building name must not be blank.";
        return false;
    }
    if (buildings_.empty()) {
        error = "No buildings are loaded.";
        return false;
    }
    const std::size_t startIndex = findIndex(start);
    if (startIndex == buildings_.size()) {
        error = "Unknown building: " + trim(start);
        return false;
    }

    std::vector<bool> visited(buildings_.size(), false);
    std::vector<std::size_t> distances(buildings_.size(), 0);
    std::queue<std::size_t> pending;
    visited[startIndex] = true;
    pending.push(startIndex);
    while (!pending.empty()) {
        const std::size_t current = pending.front();
        pending.pop();
        result.push_back(ReachableBuilding{buildings_[current].name, distances[current]});
        for (std::size_t neighbor : buildings_[current].neighbors) {
            if (!visited[neighbor]) {
                // Mark before enqueueing so cycles cannot queue a building twice.
                visited[neighbor] = true;
                distances[neighbor] = distances[current] + 1;
                pending.push(neighbor);
            }
        }
    }
    return true;
}

bool CampusGraph::displayReachable(const std::string& start, std::ostream& out) const {
    std::vector<ReachableBuilding> result;
    std::string error;
    if (!findReachable(start, result, error)) {
        out << "Error: " << error << '\n';
        return false;
    }
    for (const ReachableBuilding& building : result) {
        out << building.name << " (distance " << building.distance << ")\n";
    }
    out << "Reachable: " << result.size() << " of " << buildingCount() << " buildings\n";
    return true;
}
