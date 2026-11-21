// Final graph tests, including independent shortest-distance checks.
// Author: Adeoluwa Olukotun
#include "CampusGraph.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
std::size_t checks = 0;
void check(bool condition, const std::string& message) {
    ++checks;
    if (!condition) throw std::runtime_error(message);
}
void vertex(CampusGraph& graph, const std::string& name) {
    std::string error;
    check(graph.addBuilding(name, error), "Add building: " + name);
}
void edge(CampusGraph& graph, const std::string& a, const std::string& b) {
    std::string error;
    check(graph.addConnection(a, b, error), "Add connection: " + a + "|" + b);
}
std::string report(const CampusGraph& graph) {
    std::ostringstream out;
    graph.displayConnectivityReport(out);
    return out.str();
}
void route(const CampusGraph& graph, const std::string& a, const std::string& b,
           const std::vector<std::string>& expected) {
    std::vector<std::string> actual = {"stale result"};
    std::string error = "stale error";
    check(graph.findShortestPath(a, b, actual, error), "Route failed: " + a + " to " + b);
    check(actual == expected, "Wrong route: " + a + " to " + b);
    check(error.empty(), "Success retained a stale error");
    std::ostringstream output, wanted;
    wanted << "Shortest path:\n";
    for (const std::string& name : expected) wanted << name << '\n';
    wanted << "Number of connections: " << expected.size() - 1 << '\n';
    check(graph.displayShortestPath(a, b, output), "Route display failed");
    check(output.str() == wanted.str(), "Route display/length differs from expected");
}
void failure(const CampusGraph& graph, const std::string& a, const std::string& b,
             const std::string& reason) {
    std::vector<std::string> path = {"stale result"};
    std::string error;
    check(!graph.findShortestPath(a, b, path, error), "Invalid route was accepted");
    check(path.empty(), "Failed route retained stale output");
    check(error.find(reason) != std::string::npos, "Wrong error: " + error);
    std::ostringstream out;
    check(!graph.displayShortestPath(a, b, out), "Invalid display succeeded");
    check(out.str() == "Error: " + error + "\n", "Wrong error display");
}
void load(CampusGraph& graph, const std::string& directory, std::size_t v, std::size_t e) {
    std::ostringstream out;
    const auto buildings = graph.loadBuildings(directory + "/buildings.txt", out);
    const auto edges = graph.loadConnections(directory + "/connections.txt", out);
    check(buildings.opened && buildings.accepted == v && buildings.rejected == 0, "Building fixture failed");
    check(edges.opened && edges.accepted == e && edges.rejected == 0, "Connection fixture failed");
}
void starter(const std::string& directory) {
    CampusGraph graph;
    load(graph, directory, 7, 6);
    route(graph, " library ", "ENGINEERING BUILDING",
          {"Library", "Student Union", "Science Building", "Engineering Building"});
    route(graph, "Library", "Gym", {"Library", "Student Union", "Gym"});
    route(graph, "Library", " library ", {"Library"});
    route(graph, "Observatory", "Observatory", {"Observatory"});
    failure(graph, "Library", "Observatory", "No path exists");
    check(report(graph) ==
        "Total buildings: 7\nTotal connections: 6\nMaximum degree: 3\n"
        "Buildings with maximum degree:\nStudent Union\n"
        "Isolated buildings:\nObservatory\nConnected components: 2\n",
        "Instructor connectivity report differs from expected");
}
void custom(const std::string& directory) {
    CampusGraph graph;
    load(graph, directory, 7, 5);
    const std::string before = report(graph);
    route(graph, "start hall", "end hall", {"Start Hall", "Zeta Hall", "End Hall"});
    route(graph, "End Hall", "Start Hall", {"End Hall", "Zeta Hall", "Start Hall"});
    route(graph, "annex west", " ANNEX EAST ", {"Annex West", "Annex East"});
    failure(graph, "Start Hall", "Annex West", "No path exists");
    check(before ==
        "Total buildings: 7\nTotal connections: 5\nMaximum degree: 2\n"
        "Buildings with maximum degree:\nStart Hall\nAlpha Hall\nZeta Hall\nEnd Hall\n"
        "Isolated buildings:\nLone Tower\nConnected components: 3\n",
        "Custom ties/components report differs from expected");
    check(report(graph) == before, "Queries changed the graph");
}
void errorsAndRecovery() {
    CampusGraph graph;
    vertex(graph, "North Hall"); vertex(graph, "South Hall"); edge(graph, "North Hall", "South Hall");
    const std::string before = report(graph);
    failure(graph, " \t", "South Hall", "Starting building must not be blank");
    failure(graph, "North Hall", "\r ", "Destination building must not be blank");
    failure(graph, "Unknown", "South Hall", "Starting building not found");
    failure(graph, "North Hall", "Unknown", "Destination building not found");
    failure(graph, "Unknown", "Unknown", "Starting building not found");
    failure(graph, "North  Hall", "South Hall", "Starting building not found");
    route(graph, " north hall ", "SOUTH HALL", {"North Hall", "South Hall"});
    check(report(graph) == before, "Errors changed graph state");
    std::vector<CampusGraph::ReachableBuilding> reachable;
    std::string error;
    check(graph.findReachable("North Hall", reachable, error) && reachable.size() == 2,
          "Existing reachability broke after new operations");
}
void smallReports() {
    CampusGraph graph;
    failure(graph, "A", "B", "No buildings are loaded");
    check(report(graph) ==
        "No buildings are loaded.\nTotal buildings: 0\nTotal connections: 0\nMaximum degree: 0\n"
        "Buildings with maximum degree:\nNone\nIsolated buildings:\nNone\nConnected components: 0\n",
        "Empty report wrong");
    vertex(graph, "A");
    route(graph, "A", "A", {"A"});
    check(report(graph) ==
        "Total buildings: 1\nTotal connections: 0\nMaximum degree: 0\n"
        "Buildings with maximum degree:\nA\nIsolated buildings:\nA\nConnected components: 1\n",
        "Single-vertex report wrong");
    vertex(graph, "B"); vertex(graph, "C");
    check(report(graph) ==
        "Total buildings: 3\nTotal connections: 0\nMaximum degree: 0\n"
        "Buildings with maximum degree:\nA\nB\nC\nIsolated buildings:\nA\nB\nC\nConnected components: 3\n",
        "All-isolated report wrong");
    edge(graph, "A", "B"); edge(graph, "B", "C"); edge(graph, "C", "A");
    check(report(graph) ==
        "Total buildings: 3\nTotal connections: 3\nMaximum degree: 2\n"
        "Buildings with maximum degree:\nA\nB\nC\nIsolated buildings:\nNone\nConnected components: 1\n",
        "Cycle report or maximum-degree ties wrong");
}
// Floyd-Warshall provides a separate oracle for all 64 undirected four-vertex graphs.
void exhaustiveDistances() {
    const int ends[6][2] = {{0,1},{0,2},{0,3},{1,2},{1,3},{2,3}};
    for (int mask = 0; mask < 64; ++mask) {
        CampusGraph graph;
        int distances[4][4];
        bool adjacent[4][4] = {};
        for (int a = 0; a < 4; ++a) {
            vertex(graph, std::to_string(a));
            for (int b = 0; b < 4; ++b) distances[a][b] = a == b ? 0 : 99;
        }
        for (int e = 0; e < 6; ++e) if (mask & (1 << e)) {
            const int a = ends[e][0], b = ends[e][1];
            edge(graph, std::to_string(a), std::to_string(b));
            distances[a][b] = distances[b][a] = 1;
            adjacent[a][b] = adjacent[b][a] = true;
        }
        for (int k = 0; k < 4; ++k)
            for (int a = 0; a < 4; ++a)
                for (int b = 0; b < 4; ++b)
                    distances[a][b] = std::min(distances[a][b], distances[a][k] + distances[k][b]);
        std::size_t components = 0;
        for (int a = 0; a < 4; ++a) {
            bool earlier = false;
            for (int b = 0; b < a; ++b) if (distances[a][b] < 99) earlier = true;
            if (!earlier) ++components;
            for (int b = 0; b < 4; ++b) {
                std::vector<std::string> path;
                std::string error;
                bool found = graph.findShortestPath(std::to_string(a), std::to_string(b), path, error);
                check(found == (distances[a][b] < 99), "Oracle reachability mismatch");
                if (!found) { check(path.empty(), "Unreachable path not empty"); continue; }
                check(path.size() == static_cast<std::size_t>(distances[a][b] + 1), "Oracle distance mismatch");
                check(path.front() == std::to_string(a) && path.back() == std::to_string(b), "Route endpoints wrong");
                for (std::size_t i = 1; i < path.size(); ++i)
                    check(adjacent[std::stoi(path[i-1])][std::stoi(path[i])], "Route uses nonexistent edge");
            }
        }
        check(report(graph).find("Connected components: " + std::to_string(components) + "\n")
              != std::string::npos, "Oracle component count mismatch");
    }
}
void longRoute() {
    CampusGraph graph;
    const std::size_t count = 1200;
    std::vector<std::string> expected;
    for (std::size_t i = 0; i < count; ++i) {
        expected.push_back("Building " + std::to_string(i));
        vertex(graph, expected.back());
        if (i) edge(graph, expected[i-1], expected[i]);
    }
    route(graph, expected.front(), expected.back(), expected);
    check(report(graph).find("Connected components: 1\n") != std::string::npos, "Long chain components wrong");
}
template <typename F> void run(const std::string& name, F test, int& failed) {
    try { test(); std::cout << "PASS: " << name << '\n'; }
    catch (const std::exception& error) { ++failed; std::cerr << "FAIL: " << name << ": " << error.what() << '\n'; }
}
}
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: CampusGraphFinalTests STARTER_DATA_DIRECTORY CUSTOM_GRAPH_DIRECTORY\n";
        return 2;
    }
    int failed = 0;
    run("starter routes, tied routes, isolated building and report", [&] { starter(argv[1]); }, failed);
    run("custom file ordering, maximum-degree ties and three components", [&] { custom(argv[2]); }, failed);
    run("invalid endpoints, cleared results and recovery", errorsAndRecovery, failed);
    run("empty, single, all-isolated and cyclic graphs", smallReports, failed);
    run("all 64 four-building graphs against independent distance oracle", exhaustiveDistances, failed);
    run("1200-building route", longRoute, failed);
    std::cout << 6-failed << " groups passed, " << failed << " groups failed; " << checks << " checks evaluated.\n";
    return failed ? 1 : 0;
}
