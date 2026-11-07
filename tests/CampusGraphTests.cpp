// Purpose: Independent Member 2 graph acceptance tests; run outside the main menu.
// Author: Adeoluwa Olukotun
#include "CampusGraph.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {
std::size_t checks = 0;

// Stop the current test group when a condition fails and preserve its explanation.
void require(bool condition, const std::string& message) {
    ++checks;
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// Join the provided directory and a fixture filename without assuming an absolute path.
std::string pathIn(const std::string& directory, const std::string& filename) {
    return directory + "/" + filename;
}

// Write exact bytes so CRLF, blank fields, and trailing separators survive unchanged.
void writeFixture(const std::string& path, const std::string& contents) {
    std::ofstream file(path.c_str(), std::ios::binary);
    require(static_cast<bool>(file), "Cannot create test fixture: " + path);
    file << contents;
    require(static_cast<bool>(file), "Cannot write test fixture: " + path);
}

// Find one complete output line while allowing explanatory headers elsewhere.
bool hasLine(const std::string& text, const std::string& expected) {
    std::istringstream input(text);
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }
        if (line == expected) {
            return true;
        }
    }
    return false;
}

// Add a known-valid vertex and include the reported error if setup fails.
void addVertex(CampusGraph& graph, const std::string& name) {
    std::string error;
    const bool added = graph.addBuilding(name, error);
    require(added, "Could not add building " + name + ": " + error);
}

// Add a known-valid undirected edge for a test fixture.
void addEdge(CampusGraph& graph, const std::string& a, const std::string& b) {
    std::string error;
    const bool added = graph.addConnection(a, b, error);
    require(added, "Could not connect " + a + " to " + b + ": " + error);
}

// Capture the public graph display without accessing its representation.
std::string graphText(const CampusGraph& graph) {
    std::ostringstream output;
    graph.displayGraph(output);
    return output.str();
}

// Check BFS results against an independently stated order and distances.
void expectReachable(const CampusGraph& graph, const std::string& start,
                     const std::vector<std::pair<std::string, std::size_t> >& expected) {
    std::vector<CampusGraph::ReachableBuilding> actual;
    std::string error;
    const bool found = graph.findReachable(start, actual, error);
    require(found, "BFS failed from " + start + ": " + error);
    require(actual.size() == expected.size(), "Wrong reachable count from " + start);
    for (std::size_t i = 0; i < expected.size(); ++i) {
        require(actual[i].name == expected[i].first,
                "Wrong BFS name/order from " + start + " at index " + std::to_string(i));
        require(actual[i].distance == expected[i].second,
                "Wrong BFS distance for " + expected[i].first);
    }
}

// Confirm that a warning names the file and physical source line, with a reason.
void expectWarning(const std::string& output, const std::string& filename,
                   std::size_t lineNumber) {
    const std::string location = filename + ":" + std::to_string(lineNumber) + ":";
    std::istringstream lines(output);
    std::string line;
    while (std::getline(lines, line)) {
        const std::size_t position = line.find(location);
        if (line.find("Warning: ") == 0 && position != std::string::npos) {
            const std::string reason = line.substr(position + location.size());
            require(reason.find_first_not_of(" \t\r") != std::string::npos,
                    "Warning has no reason for " + location);
            return;
        }
    }
    require(false, "Missing warning for " + location);
}

// Count warnings so ignored comments and blank lines cannot inflate rejection counts.
std::size_t warningCount(const std::string& output) {
    std::istringstream lines(output);
    std::string line;
    std::size_t count = 0;
    while (std::getline(lines, line)) {
        if (line.find("Warning: ") == 0) {
            ++count;
        }
    }
    return count;
}

// Test canonical names, matching rules, and failure output clearing.
void testNames() {
    CampusGraph graph;
    addVertex(graph, " \tLibrary\r ");
    addVertex(graph, "Student Union");
    addVertex(graph, "Student  Union");
    addVertex(graph, "Room #5");
    std::string error;
    require(!graph.addBuilding("lIbRaRy", error), "Case-only duplicate building accepted");
    require(!error.empty(), "Duplicate building has no reason");
    require(!graph.addBuilding(" \t\r\n", error), "Blank building accepted");
    require(graph.buildingCount() == 4, "Rejected building changed vertex count");
    std::string canonical = "stale";
    require(graph.findBuilding("\tliBRARY\r", canonical), "Trimmed/case-insensitive lookup failed");
    require(canonical == "Library", "First accepted building spelling was lost");
    require(graph.findBuilding(" student  union ", canonical), "Internal spaces were collapsed");
    require(canonical == "Student  Union", "Lookup returned the wrong internal-space variant");
    require(graph.findBuilding("room #5", canonical), "Inline # was treated as a comment");
    require(canonical == "Room #5", "Inline # name spelling changed");
    require(!graph.findBuilding("Missing", canonical), "Unknown building found");
    require(canonical.empty(), "Failed lookup left a stale canonical name");
    canonical = "stale";
    require(!graph.findBuilding(" \t\r", canonical), "Blank lookup succeeded");
    require(canonical.empty(), "Blank lookup left a stale canonical name");
}

// Check both endpoints, distinct-neighbor degrees, and atomic rejection of bad edges.
void testEdges() {
    CampusGraph graph;
    addVertex(graph, "Alpha");
    addVertex(graph, "Beta");
    addVertex(graph, "Island");
    addEdge(graph, " alpha ", "BETA\r");
    const std::string before = graphText(graph);
    require(hasLine(before, "Alpha: Beta"), "First endpoint lacks its neighbor");
    require(hasLine(before, "Beta: Alpha"), "Edge is not undirected");
    require(hasLine(before, "Island: (no connections)"), "Missing isolated-building display");
    std::string error;
    require(!graph.addConnection("Alpha", "Beta", error), "Duplicate edge accepted");
    require(!graph.addConnection(" beta ", "aLpHa", error), "Reversed duplicate edge accepted");
    require(!graph.addConnection("Alpha", " ALPHA ", error), "Case-variant self-edge accepted");
    require(!graph.addConnection("Alpha", "Missing", error), "Unknown destination accepted");
    require(error.find("Missing") != std::string::npos, "Unknown destination not identified");
    require(!graph.addConnection("Missing", "Beta", error), "Unknown source accepted");
    require(!graph.addConnection("\t ", "Beta", error), "Blank endpoint accepted");
    require(graph.connectionCount() == 1, "Rejected edges changed connection count");
    require(graph.buildingCount() == 3, "An edge implicitly created a building");
    require(graphText(graph) == before, "Rejected edge partially changed adjacency lists");
    std::ostringstream stats;
    graph.displayCountsAndDegrees(stats);
    require(hasLine(stats.str(), "Total buildings: 3"), "Wrong building total display");
    require(hasLine(stats.str(), "Total connections: 1"), "Undirected edge counted twice");
    require(hasLine(stats.str(), "Alpha: degree 1"), "Wrong first endpoint degree");
    require(hasLine(stats.str(), "Beta: degree 1"), "Wrong reverse endpoint degree");
    require(hasLine(stats.str(), "Island: degree 0"), "Wrong isolated degree");
}

// Use a cycle and shared descendants to expose sorting and duplicate BFS discoveries.
void testOrderingAndRepeatedBfs() {
    CampusGraph graph;
    const char* names[] = {"Root", "A", "B", "X", "Z", "Island"};
    for (std::size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
        addVertex(graph, names[i]);
    }
    addEdge(graph, "B", "X");
    addEdge(graph, "Root", "B");
    addEdge(graph, "A", "Root");
    addEdge(graph, "B", "Z");
    addEdge(graph, "A", "X");
    addEdge(graph, "A", "Z");
    addEdge(graph, "X", "Z");
    const std::string displayed = graphText(graph);
    const char* expectedLines[] = {
        "Root: B, A", "A: Root, X, Z", "B: X, Root, Z",
        "X: B, A, Z", "Z: B, A, X", "Island: (no connections)"
    };
    std::size_t previous = 0;
    for (std::size_t i = 0; i < sizeof(expectedLines) / sizeof(expectedLines[0]); ++i) {
        require(hasLine(displayed, expectedLines[i]), "Incorrect neighbor order: " + std::string(expectedLines[i]));
        const std::size_t position = displayed.find(expectedLines[i]);
        require(i == 0 || position > previous, "Buildings were reordered in graph display");
        previous = position;
    }
    expectReachable(graph, " root ", {{"Root", 0}, {"B", 1}, {"A", 1}, {"X", 2}, {"Z", 2}});
    expectReachable(graph, "B", {{"B", 0}, {"X", 1}, {"Root", 1}, {"Z", 1}, {"A", 2}});
    expectReachable(graph, "Root", {{"Root", 0}, {"B", 1}, {"A", 1}, {"X", 2}, {"Z", 2}});
    expectReachable(graph, "Island", {{"Island", 0}});
    std::ostringstream output;
    require(graph.displayReachable("ROOT", output), "Reachability display failed");
    require(hasLine(output.str(), "Root (distance 0)"), "Start distance display is wrong");
    require(hasLine(output.str(), "X (distance 2)"), "Nontrivial distance display is wrong");
    require(hasLine(output.str(), "Reachable: 5 of 6 buildings"), "Reachability total excludes start or includes island");
}

// Exercise success/error/success sequences and ensure errors do not leave stale results.
void testBfsErrors() {
    CampusGraph graph;
    addVertex(graph, "Single Building");
    std::vector<CampusGraph::ReachableBuilding> result;
    std::string error;
    require(graph.findReachable("Single Building", result, error), "Initial BFS failed");
    require(!graph.findReachable("Missing Building", result, error), "Unknown BFS start accepted");
    require(result.empty(), "Unknown BFS left stale results");
    require(error.find("Missing Building") != std::string::npos, "Unknown BFS start not identified");
    require(graph.findReachable("Single Building", result, error), "BFS did not recover after error");
    require(!graph.findReachable(" \t\r", result, error), "Blank BFS start accepted");
    require(result.empty(), "Blank BFS left stale results");
    require(!error.empty(), "Blank BFS start has no error reason");
    std::ostringstream unknown;
    require(!graph.displayReachable("Missing Building", unknown), "Unknown display returned success");
    require(unknown.str().find("Error:") != std::string::npos, "Unknown display lacks Error prefix");
    require(unknown.str().find("Missing Building") != std::string::npos, "Unknown display omits building name");
    std::ostringstream isolated;
    require(graph.displayReachable("single building", isolated), "Isolated display failed after an error");
    require(hasLine(isolated.str(), "Single Building (distance 0)"), "Isolated start missing");
    require(hasLine(isolated.str(), "Reachable: 1 of 1 buildings"), "Isolated reachable count wrong");
    CampusGraph empty;
    require(!empty.findReachable("Single Building", result, error), "Empty graph BFS succeeded");
    require(result.empty(), "Empty graph BFS left stale results");
}

// Test physical line numbers, CRLF, first spelling, malformed fields, and graph reset.
void testBuildingLoader(const std::string& directory) {
    const std::string filename = "buildings-adversarial.txt";
    const std::string path = pathIn(directory, filename);
    writeFixture(path,
        "\t # ignored comment\r\n" " \t\r\n" " Library \t\r\n"
        "library\r\n" "Student Union\r\n" "Student  Union\r\n"
        "Room #5\r\n" "Other|Extra\r\n" "|Trailing\r\n" " \r\n"
        "  # another comment\r\n" "Room #5\r\n" "\tOBSERVATORY\t\r\n"
        "Broken|\r\n" "|\r\n");
    CampusGraph graph;
    addVertex(graph, "Old A");
    addVertex(graph, "Old B");
    addEdge(graph, "Old A", "Old B");
    std::ostringstream output;
    const CampusGraph::LoadSummary summary = graph.loadBuildings(path, output);
    require(summary.opened, "Existing buildings fixture reported missing");
    require(summary.accepted == 5 && summary.rejected == 6, "Wrong buildings acceptance/rejection counts");
    require(graph.buildingCount() == 5 && graph.connectionCount() == 0, "Building reload did not reset old graph");
    const std::size_t rejectedLines[] = {4, 8, 9, 12, 14, 15};
    for (std::size_t i = 0; i < sizeof(rejectedLines) / sizeof(rejectedLines[0]); ++i) {
        expectWarning(output.str(), filename, rejectedLines[i]);
    }
    require(warningCount(output.str()) == 6, "Wrong number of building warnings");
    require(output.str().find(filename + ": 5 accepted, 6 rejected") != std::string::npos,
            "Buildings summary missing or incorrect");
    std::string canonical;
    require(graph.findBuilding("library", canonical) && canonical == "Library", "Loader lost first accepted spelling");
    require(graph.findBuilding("Room #5", canonical), "Loader stripped inline #");
    require(graph.findBuilding("Student  Union", canonical), "Loader collapsed internal spaces");
    require(!graph.findBuilding("Old A", canonical), "Building reload retained old vertex");
    require(!graph.findBuilding("Other", canonical), "Malformed building was partially accepted");
    expectReachable(graph, "observatory", {{"OBSERVATORY", 0}});
}

// Reject invalid/reversed edges without disturbing acceptance order or old vertices.
void testConnectionLoader(const std::string& directory) {
    const std::string filename = "connections-adversarial.txt";
    const std::string path = pathIn(directory, filename);
    writeFixture(path,
        "# comment\r\n" " A | B \r\n" "b|a\r\n" "A|a\r\n"
        "A|Missing\r\n" "A|B|C\r\n" "A|\r\n" "|B\r\n"
        " \t\r\n" " B | C\r\n" "C|D\r\n" "Room #5|A\r\n"
        " C | D\r\n" "  # tail comment\r\n" "D|Room #5|\r\n"
        "| \r\n" "OnlyOne\r\n");
    CampusGraph graph;
    const char* names[] = {"A", "B", "C", "D", "Room #5"};
    for (std::size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
        addVertex(graph, names[i]);
    }
    addEdge(graph, "A", "D");
    std::ostringstream output;
    const CampusGraph::LoadSummary summary = graph.loadConnections(path, output);
    require(summary.opened, "Existing connections fixture reported missing");
    require(summary.accepted == 4 && summary.rejected == 10, "Wrong connection acceptance/rejection counts");
    require(graph.buildingCount() == 5 && graph.connectionCount() == 4, "Connection reload retained old edge or changed vertices");
    const std::size_t rejectedLines[] = {3, 4, 5, 6, 7, 8, 13, 15, 16, 17};
    for (std::size_t i = 0; i < sizeof(rejectedLines) / sizeof(rejectedLines[0]); ++i) {
        expectWarning(output.str(), filename, rejectedLines[i]);
    }
    require(warningCount(output.str()) == 10, "Wrong number of connection warnings");
    require(output.str().find(filename + ": 4 accepted, 10 rejected") != std::string::npos,
            "Connections summary missing or incorrect");
    require(output.str().find("Missing") != std::string::npos, "Unknown file reference not identified");
    require(hasLine(graphText(graph), "A: B, Room #5"), "Connection loader changed insertion order");
    require(hasLine(graphText(graph), "Room #5: A"), "Connection loader stripped inline # or reverse endpoint");
    expectReachable(graph, "A", {{"A", 0}, {"B", 1}, {"Room #5", 1}, {"C", 2}, {"D", 3}});
}

// Verify empty/missing connection reloads clear edges but retain isolated buildings.
void testConnectionReloads(const std::string& directory) {
    CampusGraph graph;
    addVertex(graph, "A");
    addVertex(graph, "B");
    addEdge(graph, "A", "B");
    const std::string emptyPath = pathIn(directory, "connections-empty.txt");
    writeFixture(emptyPath, "# no records\n\n \t\r\n");
    std::ostringstream emptyOutput;
    const CampusGraph::LoadSummary empty = graph.loadConnections(emptyPath, emptyOutput);
    require(empty.opened && empty.accepted == 0 && empty.rejected == 0, "Empty connection summary wrong");
    require(graph.buildingCount() == 2 && graph.connectionCount() == 0, "Empty connections did not preserve isolated vertices");
    require(emptyOutput.str().find("connections-empty.txt: 0 accepted, 0 rejected") != std::string::npos,
            "Empty connection summary was not printed");
    expectReachable(graph, "A", {{"A", 0}});
    addEdge(graph, "A", "B");
    const std::string missingPath = pathIn(directory, "connections-does-not-exist.txt");
    std::remove(missingPath.c_str());
    std::ostringstream missingOutput;
    const CampusGraph::LoadSummary missing = graph.loadConnections(missingPath, missingOutput);
    require(!missing.opened && missing.accepted == 0 && missing.rejected == 0, "Missing connection summary wrong");
    require(graph.buildingCount() == 2 && graph.connectionCount() == 0, "Missing connections did not clear only edges");
    require(missingOutput.str().find("Error:") != std::string::npos, "Missing connection file error not printed");
    require(missingOutput.str().find("connections-does-not-exist.txt") != std::string::npos,
            "Missing connection error omits filename");
    require(missingOutput.str().find("connections-does-not-exist.txt: 0 accepted, 0 rejected") != std::string::npos,
            "Missing connection summary not printed");
    expectReachable(graph, "B", {{"B", 0}});
}

// Verify empty/missing building reloads and rejection of references afterward.
void testBuildingReloads(const std::string& directory) {
    CampusGraph graph;
    addVertex(graph, "A");
    addVertex(graph, "B");
    addEdge(graph, "A", "B");
    const std::string emptyPath = pathIn(directory, "buildings-empty.txt");
    writeFixture(emptyPath, "");
    std::ostringstream emptyOutput;
    const CampusGraph::LoadSummary empty = graph.loadBuildings(emptyPath, emptyOutput);
    require(empty.opened && empty.accepted == 0 && empty.rejected == 0, "Empty building summary wrong");
    require(graph.buildingCount() == 0 && graph.connectionCount() == 0, "Empty buildings retained graph state");
    require(!graphText(graph).empty(), "Empty graph display gives no message");
    std::ostringstream stats;
    graph.displayCountsAndDegrees(stats);
    require(hasLine(stats.str(), "Total buildings: 0"), "Empty statistics missing zero building count");
    require(hasLine(stats.str(), "Total connections: 0"), "Empty statistics missing zero connection count");
    require(emptyOutput.str().find("buildings-empty.txt: 0 accepted, 0 rejected") != std::string::npos,
            "Empty buildings summary was not printed");
    addVertex(graph, "A");
    addVertex(graph, "B");
    addEdge(graph, "A", "B");
    const std::string missingPath = pathIn(directory, "buildings-does-not-exist.txt");
    std::remove(missingPath.c_str());
    std::ostringstream missingOutput;
    const CampusGraph::LoadSummary missing = graph.loadBuildings(missingPath, missingOutput);
    require(!missing.opened && missing.accepted == 0 && missing.rejected == 0, "Missing building summary wrong");
    require(graph.buildingCount() == 0 && graph.connectionCount() == 0, "Missing building reload retained old graph");
    require(missingOutput.str().find("Error:") != std::string::npos, "Missing building error not printed");
    require(missingOutput.str().find("buildings-does-not-exist.txt: 0 accepted, 0 rejected") != std::string::npos,
            "Missing building summary not printed");
    const std::string connectionsPath = pathIn(directory, "connections-no-buildings.txt");
    writeFixture(connectionsPath, "A|B\nA|C\n");
    std::ostringstream connectionOutput;
    const CampusGraph::LoadSummary connections = graph.loadConnections(connectionsPath, connectionOutput);
    require(connections.opened && connections.accepted == 0 && connections.rejected == 2,
            "Connections created vertices after missing building file");
    require(graph.buildingCount() == 0 && graph.connectionCount() == 0, "Unknown references mutated empty graph");
}

// Exercise dynamic growth and distances well beyond small fixed classroom examples.
void testLargeGraph() {
    CampusGraph graph;
    const std::size_t count = 1200;
    for (std::size_t i = 0; i < count; ++i) {
        addVertex(graph, "Building " + std::to_string(i));
        if (i > 0) {
            addEdge(graph, "Building " + std::to_string(i - 1), "Building " + std::to_string(i));
        }
    }
    require(graph.buildingCount() == count && graph.connectionCount() == count - 1,
            "Large graph lost buildings or connections");
    std::vector<CampusGraph::ReachableBuilding> result;
    std::string error;
    require(graph.findReachable("Building 0", result, error), "Large graph BFS failed");
    require(result.size() == count, "Large graph BFS stopped early");
    require(result.back().name == "Building 1199" && result.back().distance == count - 1,
            "Large graph endpoint distance wrong");
}

// Verify the unmodified instructor sample through public loaders and BFS output.
void testStarterData(const std::string& directory) {
    CampusGraph graph;
    std::ostringstream output;
    const CampusGraph::LoadSummary buildings = graph.loadBuildings(pathIn(directory, "buildings.txt"), output);
    const CampusGraph::LoadSummary connections = graph.loadConnections(pathIn(directory, "connections.txt"), output);
    require(buildings.opened && buildings.accepted == 7 && buildings.rejected == 0,
            "Starter buildings did not load as seven valid records");
    require(connections.opened && connections.accepted == 6 && connections.rejected == 0,
            "Starter connections did not load as six valid records");
    expectReachable(graph, " library ", {{"Library", 0}, {"Student Union", 1},
        {"Business Building", 1}, {"Science Building", 2}, {"Gym", 2}, {"Engineering Building", 3}});
    expectReachable(graph, "OBSERVATORY", {{"Observatory", 0}});
    std::ostringstream reachable;
    require(graph.displayReachable("Library", reachable), "Starter reachability display failed");
    require(hasLine(reachable.str(), "Reachable: 6 of 7 buildings"), "Starter reachable summary wrong");
    std::ostringstream stats;
    graph.displayCountsAndDegrees(stats);
    const char* expected[] = {"Total buildings: 7", "Total connections: 6", "Library: degree 2",
        "Student Union: degree 3", "Science Building: degree 2", "Engineering Building: degree 1",
        "Business Building: degree 2", "Gym: degree 2", "Observatory: degree 0"};
    for (std::size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); ++i) {
        require(hasLine(stats.str(), expected[i]), "Starter statistics missing: " + std::string(expected[i]));
    }
}

// Run one independent group while allowing later groups to expose additional failures.
template <typename Function>
void runGroup(const std::string& name, Function function, std::size_t& passed, std::size_t& failed) {
    try {
        function();
        ++passed;
        std::cout << "PASS: " << name << '\n';
    } catch (const std::exception& error) {
        ++failed;
        std::cerr << "FAIL: " << name << ": " << error.what() << '\n';
    }
}
} // namespace

// Require an existing writable fixture directory and the starter data directory.
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: CampusGraphTests TEMP_TEST_DIRECTORY STARTER_DATA_DIRECTORY\n";
        return 2;
    }
    const std::string testDirectory = argv[1];
    const std::string starterDirectory = argv[2];
    std::size_t passed = 0;
    std::size_t failed = 0;
    runGroup("building matching and canonical names", testNames, passed, failed);
    runGroup("undirected edges and atomic rejection", testEdges, passed, failed);
    runGroup("insertion ordering, cycles, repeated BFS", testOrderingAndRepeatedBfs, passed, failed);
    runGroup("BFS failures and recovery", testBfsErrors, passed, failed);
    runGroup("adversarial building loader", [&]() { testBuildingLoader(testDirectory); }, passed, failed);
    runGroup("adversarial connection loader", [&]() { testConnectionLoader(testDirectory); }, passed, failed);
    runGroup("empty/missing connection reloads", [&]() { testConnectionReloads(testDirectory); }, passed, failed);
    runGroup("empty/missing building reloads", [&]() { testBuildingReloads(testDirectory); }, passed, failed);
    runGroup("dynamic graph growth", testLargeGraph, passed, failed);
    runGroup("instructor starter data", [&]() { testStarterData(starterDirectory); }, passed, failed);
    std::cout << passed << " groups passed, " << failed << " groups failed; " << checks << " checks evaluated.\n";
    return failed == 0 ? 0 : 1;
}
