#include <SDL.h>
#include <SDL_ttf.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>

struct Point {
    int x, y;
};

struct Road {
    Point start;
    Point end;
};

bool comparePoint(const Point& a, const Point& b) {
    return a.x < b.x || (a.x == b.x && a.y < b.y);
}

int crossProduct(const Point& O, const Point& A, const Point& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

std::vector<Point> computeConvexHull(std::vector<Point>& points) {
    if (points.size() < 3) return {};
    std::sort(points.begin(), points.end(), comparePoint);
    std::vector<Point> hull;

    // Lower hull
    for (auto& point : points) {
        while (hull.size() >= 2 && crossProduct(hull[hull.size() - 2], hull[hull.size() - 1], point) <= 0) {
            hull.pop_back();
        }
        hull.push_back(point);
    }

    // Upper hull
    int t = hull.size() + 1;
    for (int i = points.size() - 2; i >= 0; i--) {
        while (hull.size() >= t && crossProduct(hull[hull.size() - 2], hull[hull.size() - 1], points[i]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }

    hull.pop_back();
    return hull;
}

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
        std::cerr << "Error: Could not open the configuration file." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(config_file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            int value = std::stoi(trim(line.substr(pos + 1)));
            config[key] = value;
        }
    }
    config_file.close();

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Error: SDL could not initialize - " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "Error: SDL_ttf could not initialize - " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("LiberationSans-Regular.ttf", 12);
    if (!font) {
        std::cerr << "Error: Failed to load font - " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    int margin = config["MARGIN"];
    int window_width = config["WINDOW_WIDTH"];
    int window_height = config["WINDOW_HEIGHT"];
    SDL_Window* window = SDL_CreateWindow("Mapa Punkty", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width + 2 * margin, window_height + 2 * margin, 0);
    if (!window) {
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    std::vector<Point> points;
    std::ifstream points_file("punkty.txt");
    int liczbaPunktow;
    int x, y;
    if (!(points_file >> liczbaPunktow)) {
        std::cerr << "Error reading number of points from punkty.txt" << std::endl;
        return 1; // or handle error appropriately
    }

    while (liczbaPunktow-- > 0 && points_file >> x >> y) {
        points.push_back({x, y});
    }
    points_file.close();

    std::vector<Point> hull = computeConvexHull(points);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red color
    // Render points and text
    for (auto& point : points) {
        int render_x = margin + (int)(((point.x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
        int render_y = margin + (int)(((point.y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
        render_y = window_height + 2 * margin - render_y; // Invert y-axis

        SDL_Rect rect = {render_x - 5, render_y - 5, 10, 10}; // Draw slightly larger points
        SDL_RenderFillRect(renderer, &rect);

        // Render text
        SDL_Color textColor = {255, 255, 255}; // White color for the text
        std::string text = std::to_string(point.x) + ", " + std::to_string(point.y);
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {render_x + 10, render_y - 10, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    // Draw the convex hull
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green color for the hull
    for (int i = 0; i < hull.size(); i++) {
        int next = (i + 1) % hull.size();
        int start_x = margin + (int)(((hull[i].x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
        int start_y = margin + (int)(((hull[i].y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
        start_y = window_height + 2 * margin - start_y; // Invert y-axis

        int end_x = margin + (int)(((hull[next].x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
        int end_y = margin + (int)(((hull[next].y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
        end_y = window_height + 2 * margin - end_y; // Invert y-axis

        SDL_RenderDrawLine(renderer, start_x, start_y, end_x, end_y);
    }
    
    //render roads
        std::vector<Road> roads;
    std::ifstream roads_file("drogi.txt");
    int x1, y1, x2, y2;
    while (roads_file >> x1 >> y1 >> x2 >> y2) {
        roads.push_back({{x1, y1}, {x2, y2}});
    }
    roads_file.close();
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  // Blue color for roads
    for (const auto& road : roads) {
        int start_x = margin + (int)(((road.start.x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
        int start_y = margin + (int)(((road.start.y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
        start_y = window_height + 2 * margin - start_y; // Invert y-axis

        int end_x = margin + (int)(((road.end.x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
        int end_y = margin + (int)(((road.end.y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
        end_y = window_height + 2 * margin - end_y; // Invert y-axis

        SDL_RenderDrawLine(renderer, start_x, start_y, end_x, end_y);
    }
	
	//display
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
