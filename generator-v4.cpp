//compile command: g++ -O3 -std=c++17 -o "%e" "%f"
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
#include <sstream>
#include <ctime>
#include <unordered_map>

std::string trim(const std::string& str){
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    if(first == std::string::npos || last == std::string::npos){
        return "";
    }
    return str.substr(first, (last - first + 1));
}

// Funkcja pomocnicza do obliczania odległości między pointami
double distance(int x1, int y1, int x2, int y2){
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Klasa do obsługi struktury Union-Find
class UnionFind  {
public:
    std::vector<int> parent;
    UnionFind(int size) : parent(size){
        std::iota(parent.begin(), parent.end(), 0);
    }
    int find(int x){
        if(parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    }
    void unite(int x, int y){
        int rootX = find(x);
        int rootY = find(y);
        if(rootX != rootY){
            parent[rootY] = rootX;
        }
    }
};

// Funkcja do konwersji stringa na lowercase
std::string toLower(std::string s){
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

// Funkcja do generowania Lorem Ipsum
std::string returnLoremIpsum(){
	
}

// Funkcja usuwająca znaki interpunkcyjne z ciągu znaków
std::string usunInterpunkcje(const std::string& tekst){
    std::string wynik;
    std::remove_copy_if(tekst.begin(), tekst.end(), std::back_inserter(wynik), [](char c){
        return std::ispunct(static_cast<unsigned char>(c));
    });
    return wynik;
}

// Funkcja tworząca mapę możliwych odwróceń liter i ich odbić lustrzanych
std::unordered_map<char, std::set<char>> utworzMapeOdwracen(){
    std::unordered_map<char, std::set<char>> mapa;
    mapa['a'] =  {'e'};
    mapa['b'] =  {'q', 'd', 'p'};
    mapa['c'] =  {'u'};
    mapa['d'] =  {'b', 'p'};
    mapa['e'] =  {'a'};
    mapa['h'] =  {'y'};
    mapa['m'] =  {'w'};
    mapa['n'] =  {'u'};
    mapa['p'] =  {'b', 'q', 'd'};
    mapa['q'] =  {'b', 'p', 'd'};
    mapa['u'] =  {'n', 'c'};
    mapa['w'] =  {'m'};
    mapa['y'] =  {'h'};
    return mapa;
}

// Funkcja losowego wyboru zamiennika z zestawu
char losowyZamiennik(const std::set<char>& zamienniki){
    int index = rand() % zamienniki.size();
    auto it = zamienniki.begin();
    std::advance(it, index);
    return *it;
}

// Funkcja do zamiany liter w tekście
std::string zamienLitery(const std::string& tekst, const std::unordered_map<char, std::set<char>>& mapa, double procent){
    std::string wynik = tekst;
    std::vector<size_t> indeksy;

    // Zbieranie indeksów liter, które można zamienić
    for (size_t i = 0; i < tekst.size(); i++){
        if(mapa.find(tekst[i]) != mapa.end()){
            indeksy.push_back(i);
        }
    }

    std::random_shuffle(indeksy.begin(), indeksy.end());								// Losowe tasowanie indeksów
    
    size_t liczbaDoZamiany = static_cast<size_t>(procent * indeksy.size());				// Obliczanie liczby liter do zamiany


    for (size_t i = 0; i < liczbaDoZamiany; i++){										// Zamiana liter na ich odpowiedniki
        char orig = tekst[indeksy[i]];
        char zamiennik = losowyZamiennik(mapa.at(orig));
        wynik[indeksy[i]] = zamiennik;
    }

    return wynik;
}

int main(){
    std::map<std::string, int> config;
    std::ifstream config_file("config.txt");
    if(!config_file.is_open()){
        std::cerr << "Nie można otworzyć pliku konfiguracyjnego." << '\n';
        return 1;
    }

    std::string line;
    while(std::getline(config_file, line)){
        size_t pos = line.find("=");
        if(pos != std::string::npos){
            std::string key = trim(line.substr(0, pos));
            int value = std::stoi(trim(line.substr(pos + 1)));
            config[key] = value;
        }
    }
    config_file.close();

    std::srand(std::time(nullptr));

    std::vector<std::pair<int, int>> points;
    std::ofstream points_file("pts.txt");
    if(!points_file.is_open()){
        std::cerr << "Nie można otworzyć pliku do zapisu pointów." << '\n';
        return 1;
    }
    
    const int p = config["TEXT_ERR_PERCENTAGE"];

    int liczbapointow = std::rand() % (config["MAX_ILOSC_pointOW"] - config["MIN_ILOSC_pointOW"] + 1) + config["MIN_ILOSC_pointOW"];
    points_file << liczbapointow << '\n';
    
    for (int i = 0; i < liczbapointow; i++){
        int x = std::rand() % (config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"] + 1) + config["MIN_WSPOLRZEDNA"];
        int y = std::rand() % (config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"] + 1) + config["MIN_WSPOLRZEDNA"];
        points.push_back( {x, y});
        points_file << x << " " << y << '\n';
    }
    points_file.close();

    // Generowanie dróg
    std::vector<std::tuple<double, int, int>> edges; // (dystans, i, j)
    for (int i = 0; i < liczbapointow; i++){
        for (int j = i + 1; j < liczbapointow; j++){
            double dist = distance(points[i].first, points[i].second, points[j].first, points[j].second);
            edges.emplace_back(dist, i, j);
        }
    }

    // Sortowanie krawędzi po dystansie
    std::sort(edges.begin(), edges.end());

    // Algorytm Kruskala
    UnionFind uf(liczbapointow);
    std::vector<std::tuple<int, int, int, int>> mst; // Minimalne drzewo rozpinające (x1, y1, x2, y2)

    for (auto& [dist, i, j] : edges){
        if(uf.find(i) != uf.find(j)){
            uf.unite(i, j);
            mst.emplace_back(points[i].first, points[i].second, points[j].first, points[j].second);
        }
    }
    
    // Additional road generation without using Union-Find to prevent cycle check
    double nearThreshold = (config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"]) * (config["NEAR_POINTS_PERCENTAGE"] / 100.0);
    std::vector<std::pair<int, int>> allPairs;

    for (int i = 0; i < liczbapointow; i++){
        for (int j = i + 1; j < liczbapointow; j++){
            if(distance(points[i].first, points[i].second, points[j].first, points[j].second) <= nearThreshold){
                allPairs.emplace_back(i, j);
            }
        }
    }

    std::shuffle(allPairs.begin(), allPairs.end(), std::default_random_engine(std::random_device {}()));
    int extraRoadsCount = allPairs.size() * (config["EXTRA_ROADS_PERCENTAGE"] / 100.0);

    // Append extra roads directly
    for (int i = 0; i < extraRoadsCount; i++){
        int a = allPairs[i].first;
        int b = allPairs[i].second;
        mst.emplace_back(points[a].first, points[a].second, points[b].first, points[b].second);
    }

    std::ofstream roads_file("drogi.txt");
    if(!roads_file.is_open()){
        std::cerr << "Nie można otworzyć pliku do zapisu dróg." << '\n';
        return 1;
    }

    roads_file << mst.size() << '\n';
    for (auto& [x1, y1, x2, y2] : mst){
        roads_file << x1 << " " << y1 << " " << x2 << " " << y2 << '\n';
    }
    roads_file.close();
    std::cout << "Wygenerowano " << liczbapointow << " pointow oraz " << mst.size() << " drog.\n";

    // Generowanie tragarzy
    std::ofstream tragarze_file("tragarze.txt");
    if(!tragarze_file.is_open()){
        std::cerr << "Nie można otworzyć pliku do zapisu tragarzy.\n";
        return 1;
    }

    int minTragarze = config["MIN_ILOSC_TRAGARZY"];
    int maxTragarze = config["MAX_ILOSC_TRAGARZY"];
    int liczbaTragarzy = std::rand() % (maxTragarze - minTragarze + 1) + minTragarze;
    tragarze_file << liczbaTragarzy << '\n';

    // We'll store the preferences of each tragarz in a vector of sets to ensure mutual preferences
    std::vector<std::set<int>> preferences(liczbaTragarzy + 1);

    for (int i = 1; i <= liczbaTragarzy; i++){
        std::string typ = (std::rand() % 2 == 0) ? "przedni" : "tylny";
        tragarze_file << i << " " << typ << " ";
        std::vector<int> sympatie;
        for (int j = i + 1; j <= liczbaTragarzy; j++){
            if(std::rand() % 2 == 0){ // Randomly decide if they like each other
                preferences[i].insert(j);
                preferences[j].insert(i);
            }
        }
        for (int sympatia : preferences[i]){
            tragarze_file << sympatia << " ";
        }
        tragarze_file << ";" << '\n';
    }
    tragarze_file.close();
    std::cout << "Wygenerowano " << liczbaTragarzy << " tragarzy." << '\n';
    
    int size = config["TEXT_KB"] / 0.625;
    size *= 1024;

    std::ofstream file("text.txt");
    if(!file){
        std::cerr << "Failed to create file." << '\n';
        return 1;
    }

    std::string loremIpsum = returnLoremIpsum();

    // Pisanie do pliku aż osiągnięty zostanie żądany rozmiar
    while(size > 0){
        if(size > loremIpsum.size()){
            file << loremIpsum;
            size -= loremIpsum.size();
        } else{
            file << loremIpsum.substr(0, size);
            size = 0;
        }
    }

    file.close();
    std::cout << "Wygenerowano opowiesc-melodie 'text.txt'." << '\n';
    
        // Usuwanie znaków interpunkcyjnych
    std::string tekstBezInterpunkcji = usunInterpunkcje(returnLoremIpsum());

    // Zbiór do przechowywania unikalnych słów
    std::set<std::string> unikalneSlowa;

    // String stream do podziału tekstu na słowa
    std::stringstream ss(tekstBezInterpunkcji);
    std::string slowo;

    // Podział tekstu na słowa i dodanie ich do zbioru
    while(ss >> slowo){
        unikalneSlowa.insert(slowo);
    }

    // Otwarcie pliku do zapisu
    std::ofstream plik("slownik.txt");

    // Zapis słów do pliku, każde w nowej linii
    if(plik.is_open()){
        for (const auto& s : unikalneSlowa){
            plik << s << '\n';
        }
        plik.close();
        std::cout << "Slowa zostaly zapisane do pliku slownik.txt." << '\n';
    } else{
        std::cerr << "Nie można otworzyć pliku do zapisu." << '\n';
    }
    
    // Wczytywanie tekstu
    std::ifstream plikTekst("text.txt");
    std::ofstream plikWynik("tekstzpoli.txt");

    if(!plikTekst.is_open()){
        std::cerr << "Nie można otworzyć pliku text.txt." << '\n';
        return 1;
    }

    if(!plikWynik.is_open()){
        std::cerr << "Nie mozna otworzyc pliku tekstzpoli.txt." << '\n';
        return 1;
    }

    std::string linia;
	double procentZamiany = p / 100.0;
    // Tworzenie mapy odwróceń liter
    auto mapaOdwracen = utworzMapeOdwracen();

    // Przetwarzanie każdej linii z tekstu
    while(std::getline(plikTekst, linia)){
        std::string liniaZamieniona = zamienLitery(linia, mapaOdwracen, procentZamiany);
        plikWynik << liniaZamieniona << '\n';
    }

    plikTekst.close();
    plikWynik.close();

    std::cout << "Przetwarzanie zakonczone. Wynik zapisany w pliku tekstzpoli.txt." << '\n';

    return 0;
}
