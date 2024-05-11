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

// Funkcja do konwersji stringa na lowercase
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

// Funkcja do generowania Lorem Ipsum
std::string generateLoremIpsum() {
    return "in the land where horizons are as flat as the lives of the flatlanders are straightforward, the day begins with the melodious story written on the page. the page, this great book, is the heart of the land, and the flatlanders gather around it to listen and work to the rhythm of the story. today, like every day, was meant to be a day of peace, a day of work, a day of melody.\nhowever, this day, the melody that the flatlanders listened to every day was absent. instead of the calming tones of boli that usually flowed from the page, the flatlanders heard the disturbing poli. this minor change transformed everything. the flatlanders, who usually work in the rhythm of the story, stopped, frightened by the new tone.\nthe heretic, who never believed in the simplicity of their world, spoke up. this is an invasion from the third dimension, he said, we must act before our story, our beautiful story is altered forever. so, the computer scientist, equipped with only an old machine that records only zeros and ones, agreed to help. together, they decided to change poli back to boli in the machine, trying at the same time to fit the entire story into the limited memory of the machine.\nmeanwhile, other flatlanders began building a fence, using one meter long segments. each segment had to be carried by a pair of porters, one with arms in front and one with arms behind. the porters, who did not always want to cooperate, had to learn to work together, as building the fence was now more important than their disputes.\nevery day, according to plan, a guard, chosen from among the flatlanders with the highest energy, walked around the fence. this ritual, though exhausting, gave them a sense of security. the guard had to stop at orientational points that marked the route, always ensuring that his last stopping point was brighter than the next. this reduced eye fatigue and allowed the guard to conserve energy for the rest of the journey.\ndays passed, and the melody tale was slowly being recorded by the computer scientist. each letter, each sound was compressed and played back with the greatest precision. in the hearts of the flatlanders, a hope flickered that their beautiful story, the story they listened to every day, would survive thanks to their efforts.\ndespite the difficulties, despite threats from the third dimension, the flatlanders continued their straightforward lives. the story, though endangered, still flowed from the page, bringing with it tales of courage, unity, and resilience, tales that the flatlanders listened to every day. thus, day by day, in the land where life flows as simply as the flat horizons, the flatlanders defended their melodious story, their peace, their land.";
}

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
    std::cout << "Wygenerowano " << liczbaPunktow << " punktow oraz " << mst.size() << " drog." << std::endl;

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
    std::cout << "Wygenerowano " << liczbaTragarzy << " tragarzy." << std::endl;
    
    int size = config["TEXT_KB"];
    size *= 1024;

    std::ofstream file("text.txt");
    if (!file) {
        std::cerr << "Failed to create file." << std::endl;
        return 1;
    }

    std::string loremIpsum = generateLoremIpsum();

    // Pisanie do pliku aż osiągnięty zostanie żądany rozmiar
    while (size > 0) {
        if (size > loremIpsum.size()) {
            file << loremIpsum;
            size -= loremIpsum.size();
        } else {
            file << loremIpsum.substr(0, size);
            size = 0;
        }
    }

    file.close();
    std::cout << "Wygenerowano opowiesc-melodie 'text.txt'." << std::endl;

    return 0;
}
