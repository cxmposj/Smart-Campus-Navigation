// Standalone graph menu for testing Member 2's module before full integration.
// Author: Adeoluwa Olukotun
#include "CampusGraph.h"

#include <iostream>
#include <sstream>
#include <string>

// Loads only graph files and exercises Member 2's Final menu options.
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
                  << "5. View campus graph\n"
                  << "6. Display graph counts and degrees\n"
                  << "7. Find reachable buildings (BFS)\n"
                  << "8. Find shortest path (BFS)\n"
                  << "17. Graph connectivity report\n"
                  << "0. Exit\n"
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
        if (choice == 0) {
            std::cout << "Goodbye!\n";
            return 0;
        }
        if (choice == 5) {
            graph.displayGraph(std::cout);
        } else if (choice == 6) {
            graph.displayCountsAndDegrees(std::cout);
        } else if (choice == 7) {
            std::cout << "Starting building: ";
            std::string start;
            if (!std::getline(std::cin, start)) return 0;
            graph.displayReachable(start, std::cout);
        } else if (choice == 8) {
            std::string start, destination;
            std::cout << "Starting building: ";
            if (!std::getline(std::cin, start)) return 0;
            if (start.find_first_not_of(" \t\r\n\f\v") == std::string::npos) {
                std::cout << "Error: Starting building must not be blank.\n";
                continue;
            }
            std::cout << "Destination building: ";
            if (!std::getline(std::cin, destination)) return 0;
            graph.displayShortestPath(start, destination, std::cout);
        } else if (choice == 17) {
            graph.displayConnectivityReport(std::cout);
        } else {
            std::cout << "Error: Choose 5, 6, 7, 8, 17, or 0.\n";
        }
    }
}

