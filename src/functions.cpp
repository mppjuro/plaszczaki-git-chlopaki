#include "functions.h"

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    if (first == std::string::npos || last == std::string::npos) {
        return "";
    }
    return str.substr(first, (last - first + 1));
}

// Funkcja usuwająca znaki interpunkcyjne z ciągu znaków
std::string usunInterpunkcje(const std::string& tekst) {
    std::string wynik;
    std::remove_copy_if(tekst.begin(), tekst.end(), std::back_inserter(wynik), [](char c) {
        return std::ispunct(static_cast<unsigned char>(c));
    });
    return wynik;
}

// Funkcja tworząca mapę możliwych odwróceń liter i ich odbić lustrzanych
std::unordered_map<char, std::set<char>> utworzMapeOdwracen() {
    std::unordered_map<char, std::set<char>> mapa;
    mapa['a'] = {'a', 'e'};
    mapa['b'] = {'b', 'q', 'd', 'p'};
    mapa['c'] = {'c', 'u'};
    mapa['d'] = {'d', 'b', 'p', 'q'};
    mapa['e'] = {'e', 'a'};
    mapa['h'] = {'h', 'y'};
    mapa['m'] = {'m', 'w'};
    mapa['n'] = {'n', 'u'};
    mapa['p'] = {'p', 'b', 'q', 'd'};
    mapa['q'] = {'q', 'b', 'p', 'd'};
    mapa['u'] = {'u', 'n', 'c'};
    mapa['w'] = {'w', 'm'};
    mapa['y'] = {'y', 'h'};
    return mapa;
}