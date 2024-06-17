// compile command: g++ -std=c++17 -o "problem1" "problem1.cpp" -IC:\SDL2-devel-2.0.22-mingw\SDL2-2.0.22\x86_64-w64-mingw32\include\SDL2 -LC:\SDL2-devel-2.0.22-mingw\SDL2-2.0.22\x86_64-w64-mingw32\lib -lmingw32 -lSDL2main -lSDL2 -mwindows -IC:\SDL2_ttf-devel-2.22.0-mingw\SDL2_ttf-2.22.0\x86_64-w64-mingw32\include\SDL2 -LC:\SDL2_ttf-devel-2.22.0-mingw\SDL2_ttf-2.22.0\x86_64-w64-mingw32\lib -lSDL2_ttf
//  wymagana wersja kompilatora: C++17 i zainstalowane biblioteki SDL2(do grafiki) oraz SDL2 TTF(do tekstu w oknie graficznym)
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "helpers.h"
#define cfg "config.txt"
#define pts_file "data/prob1/pts.txt"
#define rds_file "data/prob1/drogi.txt"
#define hull_file "data/prob1/hull.txt"
#define prt_file "data/prob1/tragarze.txt"
#define res_file "data/prob1/result.txt"
#define cost_file "data/prob1/koszt.txt"
#define pairs_file "data/prob1/pary_tragarzy.txt"
using namespace std;

extern double minX, minY;

struct Point {
    double x, y;
    int label;
};

struct Road {
    Point start;
    Point end;
};

struct Porter {
    int nr;
    string typ;
    vector<int> preferences;
};

struct Edge {
    int target;
    double weight;
};

int problem1() {
    map<string, int> config;
    ifstream config_file(cfg);
    if (!config_file.is_open()) {
        cerr << "Error: Could not open the configuration file." << '\n';
        return 1;
    }

    string line;
    while (getline(config_file, line)) {
        size_t pos = line.find('=');
        if (pos != string::npos) {
            string key = trim(line.substr(0, pos));
            int value = stoi(trim(line.substr(pos + 1)));
            config[key] = value;
        }
    }
    config_file.close();

    SDL_Color convexColor = {// Ustawiamy kolory dla poszczegolnych struktur rysowanych w SDL
                             static_cast<Uint8>(config["CONVEX_COLOR_R"]),
                             static_cast<Uint8>(config["CONVEX_COLOR_G"]),
                             static_cast<Uint8>(config["CONVEX_COLOR_B"]),
                             static_cast<Uint8>(config["CONVEX_COLOR_A"])};
    SDL_Color roadColor = {
        static_cast<Uint8>(config["ROAD_COLOR_R"]),
        static_cast<Uint8>(config["ROAD_COLOR_G"]),
        static_cast<Uint8>(config["ROAD_COLOR_B"]),
        static_cast<Uint8>(config["ROAD_COLOR_A"])};
    SDL_Color pointColor = {
        static_cast<Uint8>(config["POINT_COLOR_R"]),
        static_cast<Uint8>(config["POINT_COLOR_G"]),
        static_cast<Uint8>(config["POINT_COLOR_B"]),
        static_cast<Uint8>(config["POINT_COLOR_A"])};

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "Error: SDL could not initialize - " << SDL_GetError() << '\n';
        return 1;
    }

    if (TTF_Init() == -1) {
        cerr << "Error: SDL_ttf could not initialize - " << TTF_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("include/LiberationSans-Regular.ttf", 12);  // Ustawiamy fonty dla SDL
    if (!font) {
        cerr << "Error: Failed to load font - " << TTF_GetError() << '\n';
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    int margin = config["MARGIN"];  // Ustawiamy wymiary okna i marginesy dla SDL
    int window_width = config["WINDOW_WIDTH"];
    int window_height = config["WINDOW_HEIGHT"];
    SDL_Window* window = SDL_CreateWindow("Mapa Punkty", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width + 2 * margin, window_height + 2 * margin, 0);
    if (!window) {
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    // Tworzymy renderer dla SDL
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    vector<Point> points;  // Odczytujemy punkty z odpowiedniego pliku
    ifstream points_file(pts_file);
    int number_of_points;
    int x, y;
    if (!(points_file >> number_of_points)) {
        cerr << "Error reading number of points from punkty.txt" << '\n';
        return 1;
    }

    while (number_of_points-- > 0 && points_file >> x >> y) {
        points.push_back({double(x),
                          double(y)});
    }
    points_file.close();

    vector<Road> roads;  // Wczytaj drogi
    ifstream roads_file(rds_file);
    if (!roads_file.is_open()) {
        cerr << "Error: Could not open roads file." << '\n';
        return 1;
    }

    int numberOfRoads, x1, y1, x2, y2;
    roads_file >> numberOfRoads;  // Odczytujemy ilosc drog

    while (numberOfRoads-- > 0 && roads_file >> x1 >> y1 >> x2 >> y2) {
        roads.push_back({{double(x1), double(y1)},
                         {double(x2),
                          double(y2)}});
    }
    roads_file.close();

    vector<Point> hull = compute_convex_hull(points);

    ofstream outHullFile(hull_file);
    if (!outHullFile.is_open()) {
        cerr << "Error: Could not open the output file otoczka.txt." << '\n';
        return 1;
    }

    outHullFile << hull.size() << '\n';

    for (const auto& point : hull) {
        outHullFile << point.x << " " << point.y << '\n';
    }

    outHullFile.close();

    SDL_SetRenderDrawColor(renderer, pointColor.r, pointColor.g, pointColor.b, pointColor.a);  // Renderujemy tekst i punkty
    for (auto& point : points) {
        int render_x = margin + (int)(((point.x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
        int render_y = margin + (int)(((point.y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
        render_y = window_height + 2 * margin - render_y;  // Odwracamy os Y

        SDL_Rect rect = {
            render_x - 5,
            render_y - 5,
            10,
            10};
        SDL_RenderFillRect(renderer, &rect);

        SDL_Color textColor = {
            255,
            255,
            255};
        string text = to_string(int(point.x)) + ", " + to_string(int(point.y));
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {
            render_x + 10,
            render_y - 10,
            textSurface->w,
            textSurface->h};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    for (int i = 0; i < hull.size(); i++) {
        int next = (i + 1) % hull.size();
        int start_x = margin + (int)(((hull[i].x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
        int start_y = margin + (int)(((hull[i].y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
        start_y = window_height + 2 * margin - start_y;

        int end_x = margin + (int)(((hull[next].x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
        int end_y = margin + (int)(((hull[next].y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
        end_y = window_height + 2 * margin - end_y;

        bool isRoad = false;
        for (const auto& road : roads) {
            if (are_segments_equal(hull[i], hull[next], road.start, road.end)) {
                draw_dashed_line(renderer, start_x, start_y, end_x, end_y, convexColor, roadColor, config["DASH_LENGTH"], config["LINE_THICKNESS_PX"]);
                isRoad = true;
                break;
            }
        }

        if (!isRoad) {
            draw_thick_line(renderer, start_x, start_y, end_x, end_y, config["LINE_THICKNESS_PX"], convexColor);
        }
    }

    for (const auto& road : roads) {  // Renderujemy drogi, z wyjatkiem tych, ktore znajduja sie na otoczce
        bool isPartOfHull = false;

        for (int i = 0; i < hull.size(); i++) {
            int next = (i + 1) % hull.size();
            if (are_segments_equal(hull[i], hull[next], road.start, road.end)) {
                isPartOfHull = true;
                break;
            }
        }

        if (!isPartOfHull) {
            int start_x = margin + (int)(((road.start.x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
            int start_y = margin + (int)(((road.start.y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
            start_y = window_height + 2 * margin - start_y;

            int end_x = margin + (int)(((road.end.x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
            int end_y = margin + (int)(((road.end.y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
            end_y = window_height + 2 * margin - end_y;
            draw_thick_line(renderer, start_x, start_y, end_x, end_y, config["LINE_THICKNESS_PX"], roadColor);
        }
    }

    ifstream file(prt_file);
    if (!file.is_open()) {
        cerr << "Error: Could not open the porters file.\n";
        return 1;
    }

    int number_of_porters;
    if (!(file >> number_of_porters)) {
        cerr << "Error: Failed to read the number of porters." << '\n';
        file.close();
        return 1;
    }

    vector<Porter> porters;
    getline(file, line);

    for (int i = 0; i < number_of_porters; i++) {
        getline(file, line);
        stringstream ss(line);
        Porter porter;
        ss >> porter.nr >> porter.typ;

        int preference;
        while (ss >> preference) {
            porter.preferences.push_back(preference);
            if (ss.peek() == ';' || ss.peek() == ',')
                ss.ignore();
        }

        porters.push_back(porter);
    }
    file.close();

    vector<pair<int, int>> pairs_of_porters;  // Szukamy par tragarzy o wzajemnej sympatii
    vector<bool> porter_avaliable(porters.size(), true);

    for (size_t i = 0; i < porters.size(); i++) {
        if (!porter_avaliable[i]) {
            continue;  // Jezeli tragarz nie jest dostepny to szukamy nastepnego
        }

        for (size_t j = i + 1; j < porters.size(); j++) {
            if (!porter_avaliable[j]) {
                continue;
            }

            if (porters[i].typ != porters[j].typ) {
                bool preferenceAtoB = false;
                bool preferenceBtoA = false;
                for (int preferenceA : porters[i].preferences) {
                    if (preferenceA == porters[j].nr) {
                        preferenceAtoB = true;
                        break;
                    }
                }
                for (int preferenceB : porters[j].preferences) {
                    if (preferenceB == porters[i].nr) {
                        preferenceBtoA = true;
                        break;
                    }
                }
                if (preferenceAtoB && preferenceBtoA) {
                    pairs_of_porters.push_back({porters[i].nr,
                                                porters[j].nr});
                    porter_avaliable[i] = false;
                    porter_avaliable[j] = false;
                    break;  // Znaleziono pare, przejdz dalej
                }
            }
        }
    }

    ofstream outFile(pairs_file);  // Zapisz pary tragarzy do pliku
    if (!outFile.is_open()) {
        cerr << "Error: Could not open the output file.\n";
        return 1;
    }

    outFile << pairs_of_porters.size() << '\n';  // Zapisz liczbe par tragarzy w pierwszej linii

    for (const auto& para : pairs_of_porters) {
        outFile << para.first << " " << para.second << '\n';  // Zapisz numery portery w parze
    }

    outFile.close();

    Point closestPoint;  // Przygotowujemy drogi z fabryki
    double minDistance = numeric_limits<double>::max();
    for (const auto& point : points) {
        double distance = distance_to_origin(point);
        if (distance < minDistance) {
            minDistance = distance;
            closestPoint = point;
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    int render_x = margin + (int)(((closestPoint.x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
    int render_y = margin + (int)(((closestPoint.y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
    render_y = window_height + 2 * margin - render_y;  // Invert y-axis
    SDL_Rect rect = {
        render_x - 5,
        render_y - 5,
        10,
        10};
    SDL_RenderFillRect(renderer, &rect);
    Point selectedPoint;
    if (!hull.empty()) {
        random_device rd;  // Uzyskujemy losowy numer i uruchamiamy generator
        mt19937 eng(rd());
        uniform_int_distribution<> distr(0, hull.size() - 1);  // Definiujemy zasieg

        int randomIndex = distr(eng);  // Losowo wybieramy punkt z otoczki
        selectedPoint = hull[0];
    }
    Graph graph = create_graph(points, roads);
    int closestIndex = 0, selectedIndex = 0;
    vector<int> path;
    double totalDistance = 0;

    ofstream costFile(cost_file);
    if (costFile.is_open()) {
        compute_min_xy(points, minX, minY);
        selectedIndex = 0;
        double radius = 1.0;
        Point startowy = hull[0];
        Point currentPoint = selectedPoint;
        int nextIndex = (selectedIndex + 1) % hull.size();

        do {
            closestIndex = find_index_in_pts(closestPoint, points);
            graph = create_graph(points, roads);                  // Tworzymy graf z aktualnymi punktami i drogami
            path = dijkstra(graph, closestIndex, selectedIndex);  // Szukamy otoczki
            for (size_t i = 0; i < path.size() - 1; i++) {
                totalDistance += calc_distance(points[path[i]], points[path[i + 1]]);
            }

            Point nextPoint = hull[nextIndex];
            double distanceToNextPoint = calc_distance(currentPoint, nextPoint);
            if (distanceToNextPoint <= 1) {
                roads.push_back({currentPoint,
                                 nextPoint});
                graph = create_graph(points, roads);
                selectedIndex = find_index_in_pts(nextPoint, points);
                currentPoint = nextPoint;
                nextIndex = (nextIndex + 1) % hull.size();
            } else {
                auto intersections = intersect_circle_line(currentPoint, radius, currentPoint, nextPoint);
                if (!intersections.empty()) {
                    Point validNextPoint = intersections.front();
                    // Tworzymy nowa droge
                    if (!(validNextPoint.x == currentPoint.x && validNextPoint.y == currentPoint.y)) {
                        points.push_back(validNextPoint);
                        roads.push_back({currentPoint,
                                         validNextPoint});
                        selectedIndex = points.size() - 1;
                    }

                    currentPoint = validNextPoint;

                    if (currentPoint.x == nextPoint.x && currentPoint.y == nextPoint.y) {
                        selectedIndex = find_index_in_pts(nextPoint, points);
                        nextIndex = (nextIndex + 1) % hull.size();
                        currentPoint = nextPoint;
                    }
                } else {
                    roads.push_back({currentPoint,
                                     nextPoint});
                    selectedIndex = find_index_in_pts(nextPoint, points);
                    nextIndex = (nextIndex + 1) % hull.size();
                    currentPoint = hull[selectedIndex];
                }
            }
            costFile << totalDistance << '\n';
            totalDistance = 0;
        } while (currentPoint.x != startowy.x || currentPoint.y != startowy.y);
    } else {
        cerr << "Unable to open file koszt.txt for writing.\n";
    }
    costFile.close();

    vector<double> distances;
    ifstream inFile(cost_file);
    if (!inFile.is_open()) {
        cerr << "Unable to open koszt.txt.\n";
        return 1;
    }

    double value;
    double totalSum = 0.0;
    int count = 0;

    while (inFile >> value) {     // Odczytujemy sume wszystkich wartosci
        totalSum += (value * 2);  // Tragarz musi jeszcze wrocic, wiec odleglosc jest podwojona
        distances.push_back(value);
        count++;
    }

    inFile.close();

    ofstream outResultFile(res_file, ios::out);
    if (!outResultFile.is_open()) {
        cerr << "Unable to open result.txt file.\n";
        return 1;
    }

    outResultFile << "In order to deliver " << count << " parts (or fragments) the porters have travelled: " << (2 * totalSum) << " units which is the minimal cost of such labor.\n";

    int numberOfPorters = pairs_of_porters.size();
    vector<double> porterSums(numberOfPorters, 0.0);
    // Przydzielanie zadan
    for (int i = 0; i < distances.size(); i++) {
        int porterIndex = i % numberOfPorters;  // Ktory tragarz ma wykonac jakie zadanie
        porterSums[porterIndex] += distances[i];
    }

    // Znajdowanie maksymalnej sumy odleglosci
    double maks = *max_element(porterSums.begin(), porterSums.end());

    // Obliczanie i wypisywanie czasu potrzebnego do wykonania zadan
    outResultFile << "The whole process took " << (maks / config["PORTER_SPEED"]) << '\n';
    outResultFile.close();

    SDL_RenderPresent(renderer);  // Uruchamiamy SDL

    SDL_Event e;
    bool running = true;
    while (running) {
        if (SDL_WaitEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }
    }
    TTF_CloseFont(font);  // Czyscimy obiekty SDL
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
