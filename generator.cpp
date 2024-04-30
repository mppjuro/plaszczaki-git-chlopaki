//compile command: g++ -std=c++17 -o "generator" "generator.cpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>
#include <tuple>
#include <numeric>
#include <cmath>
#include <set>
#include <random>

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    if (first == std::string::npos || last == std::string::npos) {
        return "";
    }
    return str.substr(first, (last - first + 1));
}

// Funkcja pomocnicza do obliczania odległości między punktami
double distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Klasa do obsługi struktury Union-Find
class UnionFind {
public:
    std::vector<int> parent;
    UnionFind(int size) : parent(size) {
        std::iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    }
    void unite(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);
        if (rootX != rootY) {
            parent[rootY] = rootX;
        }
    }
};

int main() {
    std::map<std::string, int> config;
    std::ifstream config_file("config.txt");
    if (!config_file.is_open()) {
        std::cerr << "Nie można otworzyć pliku konfiguracyjnego." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(config_file, line)) {
        size_t pos = line.find("=");
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            int value = std::stoi(trim(line.substr(pos + 1)));
            config[key] = value;
        }
    }
    config_file.close();

    std::srand(std::time(nullptr));

    std::vector<std::pair<int, int>> points;
    std::ofstream points_file("punkty.txt");
    if (!points_file.is_open()) {
        std::cerr << "Nie można otworzyć pliku do zapisu punktów." << std::endl;
        return 1;
    }

    int liczbaPunktow = std::rand() % (config["MAX_ILOSC_PUNKTOW"] - config["MIN_ILOSC_PUNKTOW"] + 1) + config["MIN_ILOSC_PUNKTOW"];
    points_file << liczbaPunktow << std::endl;
    
    for (int i = 0; i < liczbaPunktow; ++i) {
        int x = std::rand() % (config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"] + 1) + config["MIN_WSPOLRZEDNA"];
        int y = std::rand() % (config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"] + 1) + config["MIN_WSPOLRZEDNA"];
        points.push_back({x, y});
        points_file << x << " " << y << std::endl;
    }
    points_file.close();

    // Generowanie dróg
    std::vector<std::tuple<double, int, int>> edges; // (dystans, i, j)
    for (int i = 0; i < liczbaPunktow; ++i) {
        for (int j = i + 1; j < liczbaPunktow; ++j) {
            double dist = distance(points[i].first, points[i].second, points[j].first, points[j].second);
            edges.emplace_back(dist, i, j);
        }
    }

    // Sortowanie krawędzi po dystansie
    std::sort(edges.begin(), edges.end());

    // Algorytm Kruskala
    UnionFind uf(liczbaPunktow);
    std::vector<std::tuple<int, int, int, int>> mst; // Minimalne drzewo rozpinające (x1, y1, x2, y2)

    for (auto& [dist, i, j] : edges) {
        if (uf.find(i) != uf.find(j)) {
            uf.unite(i, j);
            mst.emplace_back(points[i].first, points[i].second, points[j].first, points[j].second);
        }
    }
    
    // Additional road generation without using Union-Find to prevent cycle check
    double nearThreshold = (config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"]) * (config["NEAR_POINTS_PERCENTAGE"] / 100.0);
    std::vector<std::pair<int, int>> allPairs;

    for (int i = 0; i < liczbaPunktow; ++i) {
        for (int j = i + 1; j < liczbaPunktow; ++j) {
            if (distance(points[i].first, points[i].second, points[j].first, points[j].second) <= nearThreshold) {
                allPairs.emplace_back(i, j);
            }
        }
    }

    std::shuffle(allPairs.begin(), allPairs.end(), std::default_random_engine(std::random_device{}()));
    int extraRoadsCount = allPairs.size() * (config["EXTRA_ROADS_PERCENTAGE"] / 100.0);

    // Append extra roads directly
    for (int i = 0; i < extraRoadsCount; ++i) {
        int a = allPairs[i].first;
        int b = allPairs[i].second;
        mst.emplace_back(points[a].first, points[a].second, points[b].first, points[b].second);
    }

    std::ofstream roads_file("drogi.txt");
    if (!roads_file.is_open()) {
        std::cerr << "Nie można otworzyć pliku do zapisu dróg." << std::endl;
        return 1;
    }

    roads_file << mst.size() << std::endl;
    for (auto& [x1, y1, x2, y2] : mst) {
        roads_file << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
    }
    roads_file.close();

    // Generowanie tragarzy
    std::ofstream tragarze_file("tragarze.txt");
    if (!tragarze_file.is_open()) {
        std::cerr << "Nie można otworzyć pliku do zapisu tragarzy." << std::endl;
        return 1;
    }

    int minTragarze = config["MIN_ILOSC_TRAGARZY"];
    int maxTragarze = config["MAX_ILOSC_TRAGARZY"];
    int liczbaTragarzy = std::rand() % (maxTragarze - minTragarze + 1) + minTragarze;
    tragarze_file << liczbaTragarzy << std::endl;

    // We'll store the preferences of each tragarz in a vector of sets to ensure mutual preferences
    std::vector<std::set<int>> preferences(liczbaTragarzy + 1);

    for (int i = 1; i <= liczbaTragarzy; ++i) {
        std::string typ = (std::rand() % 2 == 0) ? "przedni" : "tylny";
        tragarze_file << i << " " << typ << " ";
        std::vector<int> sympatie;
        for (int j = i + 1; j <= liczbaTragarzy; ++j) {
            if (std::rand() % 2 == 0) { // Randomly decide if they like each other
                preferences[i].insert(j);
                preferences[j].insert(i);
            }
        }
        for (int sympatia : preferences[i]) {
            tragarze_file << sympatia << " ";
        }
        tragarze_file << ";" << std::endl;
    }
    tragarze_file.close();

    std::cout << "Wygenerowano " << liczbaPunktow << " punktow oraz " << mst.size() << " drog." << std::endl;
    std::cout << "Wygenerowano " << liczbaTragarzy << " tragarzy." << std::endl;

    return 0;
}
