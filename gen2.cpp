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
    
    std::cout << "Wygenerowano " << liczbaPunktow << " punktow oraz " << mst.size() << " drog" << std::endl;

    return 0;
}
