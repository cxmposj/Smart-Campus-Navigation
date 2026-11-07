// Standalone graph menu for testing Member 2's module before full integration.
// Author: Adeoluwa Olukotun
#include "CampusGraph.h"

#include <iostream>
#include <sstream>
#include <string>

// Loads only the graph files and exercises the assigned Milestone 1 options.
int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cerr << "Usage: graph_demo [data-directory]\n";
        return 1;
    }
    std::string directory = argc == 2 ? argv[1] : "data";
    if (directory.empty()) {
        std::cerr << "Error: Data directory must not be blank.\n";
        return 1;
    }
    if (directory.back() != '/') directory += '/';

    CampusGraph graph;
    graph.loadBuildings(directory + "buildings.txt", std::cout);
    graph.loadConnections(directory + "connections.txt", std::cout);

    while (true) {
        std::cout << "\nCampus Graph Demonstration\n"
                  << "3. View campus graph\n"
                  << "4. Display graph counts and degrees\n"
                  << "5. Find reachable buildings (BFS)\n"
                  << "0. Exit (10 also exits)\n"
                  << "Enter choice: ";
        std::string line;
        if (!std::getline(std::cin, line)) return 0;
        std::istringstream input(line);
        int choice = -1;
        char extra;
        if (!(input >> choice) || (input >> extra)) {
            std::cout << "Error: Enter a valid menu number.\n";
            continue;
        }
        if (choice == 0 || choice == 10) {
            std::cout << "Goodbye!\n";
            return 0;
        }
        if (choice == 3) {
            graph.displayGraph(std::cout);
        } else if (choice == 4) {
            graph.displayCountsAndDegrees(std::cout);
        } else if (choice == 5) {
            std::cout << "Starting building: ";
            std::string start;
            if (!std::getline(std::cin, start)) return 0;
            graph.displayReachable(start, std::cout);
        } else {
            std::cout << "Error: Choose 3, 4, 5, or an exit option.\n";
        }
    }
}
