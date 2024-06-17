#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "helpers.h"

#define cfg "config.txt"
#define out_pts "data/prob1/pts.txt"
#define reg_text "data/prob2/text.txt"
#define flawed_text "data/prob2/tekstzpoli.txt"
#define rds "data/prob1/drogi.txt"
#define prt_file "data/prob1/tragarze.txt"
#define dict "data/prob2/slownik.txt"

using namespace std;

class UnionFind {
   public:
    vector<int> parent;
    explicit UnionFind(int size) : parent(size) {
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
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

// string trim(const string& str) {  // Funkcja pomocnicza, pozbywa sie zbednych bialych znakow
//     size_t first = str.find_first_not_of(" \t\n\r");
//     size_t last = str.find_last_not_of(" \t\n\r");
//     if (first == string::npos || last == string::npos) {
//         return "";
//     }
//     return str.substr(first, (last - first + 1));
// }

double distance(int x1, int y1, int x2, int y2) {  // Funkcja pomocnicza, oblicza dystans pomiedzy punktem 1 i 2
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

string to_lower(string s) {  // Funkcja do konwersji stringa na taki, ktory sklada sie wylacznie z malych liter
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return tolower(c); });
    return s;
}

string return_lorem_ipsum() {  // Funkcja do wczytywania Lorem Ipsum
    std::ifstream t("src/loremipsum.txt");
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

// string rmv_interpunction(const string& tekst) {  // Funkcja usuwajaca znaki interpunkcyjne z ciagu znakow
//     string result;
//     remove_copy_if(tekst.begin(), tekst.end(), back_inserter(result), [](char c) {
//         return ispunct(static_cast<unsigned char>(c));
//     });
//     return result;
// }

// unordered_map<char, set<char>> mirrored_letters() {  // Funkcja tworzaca mape mozliwych odwrocen liter i ich odbic lustrzanych
//     unordered_map<char, set<char>> mapa;
//     mapa['a'] = {'e'};
//     mapa['b'] = {'q', 'd', 'p'};
//     mapa['c'] = {'u'};
//     mapa['d'] = {'b', 'p'};
//     mapa['e'] = {'a'};
//     mapa['h'] = {'y'};
//     mapa['m'] = {'w'};
//     mapa['n'] = {'u'};
//     mapa['p'] = {'b', 'q', 'd'};
//     mapa['q'] = {'b', 'p', 'd'};
//     mapa['u'] = {'n', 'c'};
//     mapa['w'] = {'m'};
//     mapa['y'] = {'h'};
//     return mapa;
// }

char random_spare(const set<char>& spares) {  // Funkcja losowego wyboru zamiennika z zestawu
    int index = rand() % spares.size();
    auto it = spares.begin();
    advance(it, index);
    return *it;
}
// Funkcja do zamiany liter w tekscie
string swap_letters(const string& tekst, const unordered_map<char, set<char>>& mapa, double percent) {
    string result = tekst;
    vector<size_t> indexes;

    for (size_t i = 0; i < tekst.size(); i++) {  // Zbieranie indeksow liter, ktore mozna zamienic
        if (mapa.find(tekst[i]) != mapa.end()) {
            indexes.push_back(i);
        }
    }

    std::shuffle(indexes.begin(), indexes.end(), default_random_engine(random_device{}()));  // Losowe tasowanie indeksow

    size_t number_to_swap = static_cast<size_t>(percent * indexes.size());  // Obliczanie liczby liter do zamiany

    for (size_t i = 0; i < number_to_swap; i++) {  // Zamiana liter na ich odpowiedniki
        char orig = tekst[indexes[i]];
        char spare_piece = random_spare(mapa.at(orig));
        result[indexes[i]] = spare_piece;
    }

    return result;
}

int generator() {
    ios::sync_with_stdio(false);

    map<string, int> config;
    ifstream config_file(cfg);
    if (!config_file.is_open()) {
        cerr << "Couldn't open config file!\n";
        return 1;
    }
    string line_2;
    while (getline(config_file, line_2)) {
        size_t pos = line_2.find("=");
        if (pos != string::npos) {
            string key = trim(line_2.substr(0, pos));
            int value = stoi(trim(line_2.substr(pos + 1)));
            config[key] = value;
        }
    }
    config_file.close();

    srand(time(nullptr));

    vector<pair<int, int>> points;
    ofstream points_file(out_pts);
    if (!points_file.is_open()) {
        cerr << "Unable to open point save file.\n";
        return 1;
    }

    const int p = config["TEXT_ERR_PERCENTAGE"];

    int number_of_points = rand() % (config["MAX_ILOSC_PUNKTOW"] - config["MIN_ILOSC_PUNKTOW"] + 1) + config["MIN_ILOSC_PUNKTOW"];
    points_file << number_of_points << '\n';

    for (int i = 0; i < number_of_points; i++) {
        int x = rand() % (config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"] + 1) + config["MIN_WSPOLRZEDNA"];
        int y = rand() % (config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"] + 1) + config["MIN_WSPOLRZEDNA"];
        points.push_back({x, y});
        points_file << x << " " << y << '\n';
    }
    points_file.close();

    // Generowanie drog
    vector<tuple<double, int, int>> edges;  //(dystans, i, j)
    for (int i = 0; i < number_of_points; i++) {
        for (int j = i + 1; j < number_of_points; j++) {
            double dist = distance(points[i].first, points[i].second, points[j].first, points[j].second);
            edges.emplace_back(dist, i, j);
        }
    }

    sort(edges.begin(), edges.end());  // Sortowanie krawedzi po dystansie

    UnionFind uf(number_of_points);         // Algorytm Kruskala
    vector<tuple<int, int, int, int>> mst;  // Minimalne drzewo rozpinajace (x1, y1, x2, y2)

    for (auto& [dist, i, j] : edges) {
        if (uf.find(i) != uf.find(j)) {
            uf.unite(i, j);
            mst.emplace_back(points[i].first, points[i].second, points[j].first, points[j].second);
        }
    }

    double nearThreshold = (config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"]) * (config["NEAR_POINTS_PERCENTAGE"] / 100.0);
    vector<pair<int, int>> allPairs;

    for (int i = 0; i < number_of_points; i++) {
        for (int j = i + 1; j < number_of_points; j++) {
            if (distance(points[i].first, points[i].second, points[j].first, points[j].second) <= nearThreshold) {
                allPairs.emplace_back(i, j);
            }
        }
    }

    shuffle(allPairs.begin(), allPairs.end(), default_random_engine(random_device{}()));
    int extraRoadsCount = allPairs.size() * (config["EXTRA_ROADS_PERCENTAGE"] / 100.0);

    for (int i = 0; i < extraRoadsCount; i++) {  // Dodatkowe rozszerzenie drog
        int a = allPairs[i].first;
        int b = allPairs[i].second;
        mst.emplace_back(points[a].first, points[a].second, points[b].first, points[b].second);
    }

    ofstream roads_file(rds);
    if (!roads_file.is_open()) {
        cerr << "Unable to open file for road saving.";
        return 1;
    }

    roads_file << mst.size() << '\n';
    for (auto& [x1, y1, x2, y2] : mst) {
        roads_file << x1 << " " << y1 << " " << x2 << " " << y2 << '\n';
    }
    roads_file.close();
    cout << "Generated " << number_of_points << " points and " << mst.size() << " roads.\n";

    ofstream porters_file(prt_file);  // Generowanie tragarzy
    if (!porters_file.is_open()) {
        cerr << "Unable to open porters file.\n";
        return 1;
    }

    int min_porters = config["MIN_ILOSC_TRAGARZY"];
    int max_porters = config["MAX_ILOSC_TRAGARZY"];
    int number_of_porters = rand() % (max_porters - min_porters + 1) + min_porters;
    porters_file << number_of_porters << '\n';

    vector<set<int>> preferences(number_of_porters + 1);

    for (int i = 1; i <= number_of_porters; i++) {
        string typ = (rand() % 2 == 0) ? "przedni" : "tylny";
        porters_file << i << " " << typ << " ";
        vector<int> sympatie;
        for (int j = i + 1; j <= number_of_porters; j++) {
            if (rand() % 2 == 0) {  // Losowo decydujemy o sympatiach tragarzy
                preferences[i].insert(j);
                preferences[j].insert(i);
            }
        }
        for (int preference : preferences[i]) {
            porters_file << preference << " ";
        }
        porters_file << ";" << '\n';
    }
    porters_file.close();
    cout << "Generated " << number_of_porters << " of porters.\n";

    int size = config["TEXT_KB"] / 0.625;
    size *= 1024;

    ofstream file(reg_text);
    if (!file) {
        cerr << "Failed to create file.\n";
        return 1;
    }

    string loremIpsum = return_lorem_ipsum();

    while (size > 0) {  // Pisanie do pliku az osiagniety zostanie zadany rozmiar
        if (size > loremIpsum.size()) {
            file << loremIpsum;
            size -= loremIpsum.size();
        } else {
            file << loremIpsum.substr(0, size);
            size = 0;
        }
    }
    file.close();
    cout << "Generated the melodic story at 'text.txt'.\n'";

    string text_without_interpunction = rmv_interpunction(return_lorem_ipsum());  // Usuwanie znakow interpunkcyjnych

    set<string> unique_words;  // Zbior do przechowywania unikalnych słow

    stringstream ss(text_without_interpunction);  // String stream do podziału tekstu na słowa
    string word;

    while (ss >> word) {  // Podział tekstu na słowa i dodanie ich do zbioru
        unique_words.insert(word);
    }

    ofstream plik(dict);

    if (plik.is_open()) {  // Zapis słow do pliku, kazde w nowej linii
        for (const auto& s : unique_words) {
            plik << s << '\n';
        }
        plik.close();
        cout << "Words saved to slownik.txt.\n";
    } else {
        cerr << "Unable to open the file.\n";
    }

    ifstream file_with_text(reg_text);
    ofstream output_file(flawed_text);

    if (!file_with_text.is_open()) {
        cerr << "Unable to open text.txt.\n";
        return 1;
    }

    if (!output_file.is_open()) {
        cerr << "Unable to open tekstzpoli.txt.\n";
        return 1;
    }

    string line;
    double swap_percentage = p / 100.0;
    auto swap_map = mirrored_letters();  // Tworzenie mapy odwrocen liter

    while (getline(file_with_text, line)) {  // Przetwarzanie kazdej linii z tekstu
        string swapped_line = swap_letters(line, swap_map, swap_percentage);
        output_file << swapped_line << '\n';
    }

    file_with_text.close();
    output_file.close();

    cout << "Processing completed. Output saved to tekstzpoli.txt.\n";

    return 0;
}
