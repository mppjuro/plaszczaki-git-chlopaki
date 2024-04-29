#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>

// Funkcja do usuwania białych znaków z początku i końca stringa
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    if (first == std::string::npos || last == std::string::npos) {
        return "";
    }
    return str.substr(first, (last - first + 1));
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

    // Wyświetlanie wczytanych wartości na standardowe wyjście
    for (const auto& [key, value] : config) {
        std::cout << key << " = " << value << std::endl;
    }

    // Inicjalizacja generatora liczb losowych
    std::srand(std::time(nullptr));

    int liczbaPunktow = std::rand() % (config["MAX_ILOSC_PUNKTOW"] - config["MIN_ILOSC_PUNKTOW"] + 1) + config["MIN_ILOSC_PUNKTOW"];
    
    std::ofstream plik("punkty.txt");
    if (!plik.is_open()) {
        std::cerr << "Nie można otworzyć pliku do zapisu." << std::endl;
        return 1;
    }
    
    plik << liczbaPunktow << std::endl;
    
    for (int i = 0; i < liczbaPunktow; ++i) {
        int x = std::rand() % (config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"] + 1) + config["MIN_WSPOLRZEDNA"];
        int y = std::rand() % (config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"] + 1) + config["MIN_WSPOLRZEDNA"];
        plik << x << " " << y << std::endl;
    }
    std::cout << "Wygenerowano " << liczbaPunktow << " punktow" << std::endl;
    plik.close();
    return 0;
}
