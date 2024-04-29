#include <SDL.h>
#include <SDL_ttf.h>
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
	std::cout << "elo." << std::endl;
    std::map<std::string, int> config;
    std::cout << "xd." << std::endl;
    std::ifstream config_file("config.txt");
    if (!config_file.is_open()) {
        std::cout << "Error: Could not open the configuration file." << std::endl;
        return 1;
    } else {
        std::cout << "Success: Configuration file opened successfully." << std::endl;
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

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "Error: SDL could not initialize - " << SDL_GetError() << std::endl;
        return 1;
    } else {
        std::cout << "Success: SDL initialized successfully." << std::endl;
    }

    if (TTF_Init() == -1) {
        std::cout << "Error: SDL_ttf could not initialize - " << TTF_GetError() << std::endl;
        return 1;
    } else {
        std::cout << "Success: SDL_ttf initialized successfully." << std::endl;
    }

    TTF_Font* font = TTF_OpenFont("LiberationSans-Regular.ttf", 12);
    if (!font) {
        std::cout << "Error: Failed to load font - " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    } else {
        std::cout << "Success: Font loaded successfully." << std::endl;
    }

    int window_width = config["WINDOW_WIDTH"];
    int window_height = config["WINDOW_HEIGHT"];
    int margin = config["MARGIN"];
    SDL_Window* window = SDL_CreateWindow("Mapa Punkty", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width + 2 * margin, window_height + 2 * margin, 0);
    if (!window) {
        std::cout << "Error: Window could not be created - " << SDL_GetError() << std::endl;
        TTF_CloseFont(font);
        SDL_Quit();
        return 1;
    } else {
        std::cout << "Success: Window created successfully." << std::endl;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Error: Renderer could not be created - " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        SDL_Quit();
        return 1;
    } else {
        std::cout << "Success: Renderer created successfully." << std::endl;
    }

    std::ifstream plik("punkty.txt");
    if (!plik.is_open()) {
        std::cout << "Error: Could not open the points file." << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        SDL_Quit();
        return 1;
    } else {
        std::cout << "Success: Points file opened successfully." << std::endl;
    }

    int liczbaPunktow;
    plik >> liczbaPunktow;
    int x, y;
    int point_size = 5;  // Size of each point

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Czerwony
while (plik >> x >> y) {
    int render_x = (int)(margin + ((x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
    int render_y = (int)(-margin + ((y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);

    // Aby przeskalować i umieścić wszystko w oknie, zamiast używać bezpośrednio render_y, możesz zastosować:
    render_y = window_height - render_y; // Odwrócenie osi Y, aby wartości wyższe były niżej

    SDL_Rect rect = {render_x - point_size / 2, render_y - point_size / 2, point_size, point_size};
    SDL_RenderFillRect(renderer, &rect);

    // Renderowanie tekstu
    SDL_Color textColor = {255, 255, 255};  // Biały kolor tekstu
    std::string text = std::to_string(x) + ", " + std::to_string(y);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {render_x + 10, render_y - 10, textSurface->w, textSurface->h}; // Przesunięcie tekstu, aby nie nakładał się bezpośrednio na punkt
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
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

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
