#include <SDL.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    if (first == std::string::npos || last == std::string::npos) {
        return "";
    }
    return str.substr(first, (last - first + 1));
}

int main(int argc, char* argv[]) {
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

    int window_width = config["WINDOW_WIDTH"];
    int window_height = config["WINDOW_HEIGHT"];
    int min_x = config["MIN_WSPOLRZEDNA"];
    int max_x = config["MAX_WSPOLRZEDNA"];
    int min_y = min_x;  // Assuming the same range for y for simplicity
    int max_y = max_x;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Mapa Punkty", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    std::ifstream plik("punkty.txt");
    if (!plik.is_open()) {
        std::cerr << "Nie można otworzyć pliku do wczytania." << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int liczbaPunktow;
    plik >> liczbaPunktow;
    int x, y;

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Czerwony
    while (plik >> x >> y) {
        int render_x = (int)(((x - min_x) / (double)(max_x - min_x)) * window_width);
        int render_y = (int)(((y - min_y) / (double)(max_y - min_y)) * window_height);
        SDL_RenderDrawPoint(renderer, render_x, render_y);
    }
    SDL_RenderPresent(renderer);

    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
