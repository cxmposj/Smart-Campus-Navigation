// Campus graph loading, display, and BFS reachability for Milestone 1.
// Author: Adeoluwa Olukotun
#ifndef CAMPUS_GRAPH_H
#define CAMPUS_GRAPH_H

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

class CampusGraph {
public:
    struct LoadSummary {
        std::size_t accepted = 0;
        std::size_t rejected = 0;
        bool opened = false;
    };

    struct ReachableBuilding {
        std::string name;
        std::size_t distance;
    };

    // Adds a unique trimmed name, keeping its original spelling and file order.
    bool addBuilding(const std::string& name, std::string& error);

    // Adds one undirected edge; invalid requests leave the graph unchanged.
    bool addConnection(const std::string& first, const std::string& second,
                       std::string& error);

    // Resolves an accepted building for the facility loader; false clears the name.
    bool findBuilding(const std::string& name, std::string& canonicalName) const;

    // Returns the vertex count, including isolated buildings.
    std::size_t buildingCount() const;

    // Returns the number of undirected edges, counting each pair once.
    std::size_t connectionCount() const;

    // Replaces all buildings and connections, even if the file cannot be opened.
    // Prints warnings using physical line numbers, then accepted/rejected totals.
    LoadSummary loadBuildings(const std::string& filePath, std::ostream& out);

    // Replaces connections while keeping buildings; a missing file leaves no edges.
    LoadSummary loadConnections(const std::string& filePath, std::ostream& out);

    // Displays buildings and neighbors in accepted insertion order.
    void displayGraph(std::ostream& out) const;

    // Displays building/connection totals and each building's distinct degree.
    void displayCountsAndDegrees(std::ostream& out) const;

    // Returns BFS discovery order and distances. Each call starts a fresh search.
    // On an invalid start, clears result and returns an explanatory error.
    bool findReachable(const std::string& start,
                       std::vector<ReachableBuilding>& result,
                       std::string& error) const;

    // Displays the BFS result or a clear error; never reads menu input itself.
    bool displayReachable(const std::string& start, std::ostream& out) const;

private:
    struct Building {
        std::string name;
        std::string key;
        std::vector<std::size_t> neighbors;
    };

    std::vector<Building> buildings_;
    std::size_t connectionCount_ = 0;

    // Linear building lookup; buildings_.size() is the not-found sentinel.
    std::size_t findIndex(const std::string& name) const;
};

#endif
