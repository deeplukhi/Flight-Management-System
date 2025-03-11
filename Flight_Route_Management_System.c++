#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <sstream>
#include <algorithm>

using namespace std;

// Structure to represent a flight route
struct Flight {
    string destination;
    double cost;
    double time;
};

// Graph class to manage flight routes
class FlightGraph {
private:
    unordered_map<string, vector<Flight>> graph; // Adjacency list
    unordered_set<string> cities; // Set of all cities

public:
    // Add a new flight route
    void addFlight(const string& source, const string& destination, double cost, double time) {
        graph[source].push_back({destination, cost, time});
        graph[destination].push_back({source, cost, time}); // Bidirectional
        cities.insert(source);
        cities.insert(destination);
        cout << "Added flight: " << source << " -> " << destination << " (Cost: " << cost << ", Time: " << time << ")\n";
    }

    // Remove a flight route
    void removeFlight(const string& source, const string& destination) {
        auto it = graph.find(source);
        if (it != graph.end()) {
            auto& flights = it->second;
            flights.erase(std::remove_if(flights.begin(), flights.end(), [&](const Flight& f) {
                return f.destination == destination;
            }), flights.end());
        }
        it = graph.find(destination);
        if (it != graph.end()) {
            auto& flights = it->second;
            flights.erase(std::remove_if(flights.begin(), flights.end(), [&](const Flight& f) {
                return f.destination == source;
            }), flights.end());
        }
        cout << "Removed flight: " << source << " -> " << destination << "\n";
    }

    // Dijkstra's algorithm to find the shortest route based on cost or time
    pair<vector<string>, double> findShortestRoute(const string& source, const string& destination, const string& criteria) {
        if (criteria != "cost" && criteria != "time") {
            throw invalid_argument("Criteria must be 'cost' or 'time'.");
        }

        // Priority queue: (total_cost_or_time, current_city, path)
        priority_queue<pair<double, pair<string, vector<string>>>, vector<pair<double, pair<string, vector<string>>>>, greater<>> pq;
        unordered_set<string> visited;

        pq.push({0, {source, {}}});

        while (!pq.empty()) {
            auto top = pq.top();
            double total = top.first;
            string current = top.second.first;
            vector<string> path = top.second.second;
            pq.pop();

            if (current == destination) {
                path.push_back(current);
                return {path, total};
            }

            if (visited.find(current) != visited.end()) continue;
            visited.insert(current);

            for (const auto& flight : graph[current]) {
                if (visited.find(flight.destination) == visited.end()) {
                    double newTotal = total + (criteria == "cost" ? flight.cost : flight.time);
                    vector<string> newPath = path;
                    newPath.push_back(current);
                    pq.push({newTotal, {flight.destination, newPath}});
                }
            }
        }

        return {{}, numeric_limits<double>::infinity()}; // No route found
    }

    // Display all routes between two cities using BFS
    vector<vector<string>> displayAllRoutes(const string& source, const string& destination) {
        vector<vector<string>> allRoutes;
        queue<pair<string, vector<string>>> q;
        q.push({source, {}});

        while (!q.empty()) {
            auto front = q.front();
            string current = front.first;
            vector<string> path = front.second;
            q.pop();

            if (current == destination) {
                path.push_back(current);
                allRoutes.push_back(path);
                continue;
            }

            for (const auto& flight : graph[current]) {
                if (find(path.begin(), path.end(), flight.destination) == path.end()) {
                    vector<string> newPath = path;
                    newPath.push_back(current);
                    q.push({flight.destination, newPath});
                }
            }
        }

        return allRoutes;
    }

    // Save flight routes to a file
    void saveRoutesToFile(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Unable to open file for writing.\n";
            return;
        }

        for (const auto& entry : graph) {
            const string& source = entry.first;
            for (const auto& flight : entry.second) {
                file << source << " " << flight.destination << " " << flight.cost << " " << flight.time << "\n";
            }
        }

        cout << "Saved flight routes to " << filename << "\n";
    }

    // Load flight routes from a file
    void loadRoutesFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Unable to open file for reading.\n";
            return;
        }

        graph.clear();
        cities.clear();

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string source, destination;
            double cost, time;
            ss >> source >> destination >> cost >> time;
            addFlight(source, destination, cost, time);
        }

        cout << "Loaded flight routes from " << filename << "\n";
    }
};

// CLI Interface
void displayMenu() {
    cout << "\n🚀 Flight Route Management System\n";
    cout << "1. Add Flight\n";
    cout << "2. Remove Flight\n";
    cout << "3. Find Shortest Route (Cost)\n";
    cout << "4. Find Fastest Route (Time)\n";
    cout << "5. Display All Routes\n";
    cout << "6. Save Routes to File\n";
    cout << "7. Load Routes from File\n";
    cout << "8. Exit\n";
}

int main() {
    FlightGraph flightGraph;

    while (true) {
        displayMenu();
        int choice;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            string source, destination;
            double cost, time;
            cout << "Enter source city: ";
            cin >> source;
            cout << "Enter destination city: ";
            cin >> destination;
            cout << "Enter cost: ";
            cin >> cost;
            cout << "Enter time: ";
            cin >> time;
            flightGraph.addFlight(source, destination, cost, time);
        } else if (choice == 2) {
            string source, destination;
            cout << "Enter source city: ";
            cin >> source;
            cout << "Enter destination city: ";
            cin >> destination;
            flightGraph.removeFlight(source, destination);
        } else if (choice == 3) {
            string source, destination;
            cout << "Enter source city: ";
            cin >> source;
            cout << "Enter destination city: ";
            cin >> destination;
            auto result = flightGraph.findShortestRoute(source, destination, "cost");
            if (!result.first.empty()) {
                cout << "Shortest Route (Cost): ";
                for (const auto& city : result.first) cout << city << " -> ";
                cout << "Total Cost: " << result.second << "\n";
            } else {
                cout << "No route found.\n";
            }
        } else if (choice == 4) {
            string source, destination;
            cout << "Enter source city: ";
            cin >> source;
            cout << "Enter destination city: ";
            cin >> destination;
            auto result = flightGraph.findShortestRoute(source, destination, "time");
            if (!result.first.empty()) {
                cout << "Fastest Route (Time): ";
                for (const auto& city : result.first) cout << city << " -> ";
                cout << "Total Time: " << result.second << "\n";
            } else {
                cout << "No route found.\n";
            }
        } else if (choice == 5) {
            string source, destination;
            cout << "Enter source city: ";
            cin >> source;
            cout << "Enter destination city: ";
            cin >> destination;
            auto routes = flightGraph.displayAllRoutes(source, destination);
            if (!routes.empty()) {
                cout << "All Available Routes:\n";
                for (size_t i = 0; i < routes.size(); i++) {
                    cout << i + 1 << ". ";
                    for (const auto& city : routes[i]) cout << city << " -> ";
                    cout << "\n";
                }
            } else {
                cout << "No routes found.\n";
            }
        } else if (choice == 6) {
            string filename;
            cout << "Enter filename to save: ";
            cin >> filename;
            flightGraph.saveRoutesToFile(filename);
        } else if (choice == 7) {
            string filename;
            cout << "Enter filename to load: ";
            cin >> filename;
            flightGraph.loadRoutesFromFile(filename);
        } else if (choice == 8) {
            cout << "Exiting...\n";
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }
    
    return 0;
}