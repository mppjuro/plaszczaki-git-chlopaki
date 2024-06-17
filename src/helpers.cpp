#include "helpers.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

#ifndef MIN_GLOBALS_H
#define MIN_GLOBALS_H
#endif  // MIN_GLOBALS_H

double minX, minY;

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

bool compare_point(const Point& a, const Point& b) {
    return a.x < b.x || (a.x == b.x && a.y < b.y);
}

int cross_product(const Point& O, const Point& A, const Point& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

bool are_segments_equal(const Point& a1, const Point& b1, const Point& a2, const Point& b2) {
    return (a1.x == a2.x && a1.y == a2.y && b1.x == b2.x && b1.y == b2.y) || (a1.x == b2.x && a1.y == b2.y && b1.x == a2.x && b1.y == a2.y);
}

double calc_distance(const Point& p1, const Point& p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

void compute_min_xy(const vector<Point>& points, double& minX, double& minY) {
    minX = numeric_limits<double>::max();
    minY = numeric_limits<double>::max();

    for (const auto& point : points) {
        if (point.x < minX) {
            minX = point.x;
        }
        if (point.y < minY) {
            minY = point.y;
        }
    }
}

bool compare_by_angle(const Point& p1, const Point& p2) {
    double angle1 = atan2(p1.y - minY, p1.x - minX);  // Obliczamy kat wzgledem osi
    double angle2 = atan2(p2.y - minY, p2.x - minX);

    if (angle1 < 0) angle1 += 2 * M_PI;  // Upewniamy sie ze kazdy kat miesci sie w przedziale 0-2PI
    if (angle2 < 0) angle2 += 2 * M_PI;

    return angle1 < angle2;  // Sortujemy zgodnie z ruchem wskazowek zegara
}

double distance_to_origin(const Point& point) {
    return sqrt(pow(point.x, 2) + pow(point.y, 2));
}
// Tworzenie grafu, zakladamy ze jest nieskierowany
Graph create_graph(const vector<Point>& points, const vector<Road>& roads) {
    Graph graph;
    for (int i = 0; i < points.size(); i++) {
        graph[i] = {};
    }

    for (const auto& road : roads) {
        int start_index = -1, end_index = -1;
        for (int i = 0; i < points.size(); i++) {
            if (points[i].x == road.start.x && points[i].y == road.start.y) {
                start_index = i;
            }
            if (points[i].x == road.end.x && points[i].y == road.end.y) {
                end_index = i;
            }
        }
        if (start_index != -1 && end_index != -1) {
            double dist = calc_distance(road.start, road.end);
            graph[start_index].push_back({end_index, dist});
            graph[end_index].push_back({start_index, dist});
        }
    }
    return graph;
}

// Funkcja do obliczania przeciecia okregu i linii
vector<Point> intersect_circle_line(Point circle_center, double radius, Point line_start, Point line_end) {
    double cx = circle_center.x, cy = circle_center.y;
    double x1 = line_start.x, y1 = line_start.y;
    double x2 = line_end.x, y2 = line_end.y;

    double dx = x2 - x1, dy = y2 - y1;
    double A = dx * dx + dy * dy;
    double B = 2 * (dx * (x1 - cx) + dy * (y1 - cy));
    double C = (x1 - cx) * (x1 - cx) + (y1 - cy) * (y1 - cy) - radius * radius;

    double det = B * B - 4 * A * C;
    vector<Point> intersections;
    if (A <= 0.0000001 || det < 0) {  // Brak rozwiazan. Znajdujemy punkt najblizszy srodkowi
        double t = -B / (2 * A);
        double closest_x = x1 + t * dx;
        double closest_y = y1 + t * dy;
        intersections.push_back({closest_x,
                                 closest_y});
    } else if (det == 0) {  // Istnieje jedno rozwiazanie
        double t = -B / (2 * A);
        intersections.push_back({x1 + t * dx,
                                 y1 + t * dy});
    } else {  // Istnieja dwa rozwiazania
        double t1 = (-B + sqrt(det)) / (2 * A);
        double t2 = (-B - sqrt(det)) / (2 * A);
        intersections.push_back({x1 + t1 * dx,
                                 y1 + t1 * dy});
        intersections.push_back({x1 + t2 * dx,
                                 y1 + t2 * dy});
    }

    vector<Point> onSegment;  // Filtrujemy punkty, ktore nie leza na danym segmencie
    for (const auto& pt : intersections) {
        if (min(x1, x2) <= pt.x && pt.x <= max(x1, x2) && min(y1, y2) <= pt.y && pt.y <= max(y1, y2)) {
            onSegment.push_back(pt);
        }
    }

    return onSegment;
}

int find_index_in_pts(const Point& point, const vector<Point>& points) {
    for (int i = 0; i < points.size(); i++) {
        if (points[i].x == point.x && points[i].y == point.y) {
            return i;
        }
    }
    return -1;
}
// Wyznaczamy sciezke miedzy punktami algorytmem Dijkstry
vector<int> dijkstra(const Graph& graph, int start_vertex, int target_vertex) {
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;
    vector<double> distances(graph.size(), numeric_limits<double>::max());
    vector<int> previous(graph.size(), -1);
    vector<bool> visited(graph.size(), false);

    pq.push({0, start_vertex});
    distances[start_vertex] = 0;

    while (!pq.empty()) {
        int current = pq.top().second;
        pq.pop();

        if (visited[current]) continue;
        visited[current] = true;

        if (current == target_vertex) break;

        for (const auto& edge : graph.at(current)) {
            int neighbor = edge.target;
            double weight = edge.weight;
            double distanceThroughU = distances[current] + weight;
            if (distanceThroughU < distances[neighbor]) {
                distances[neighbor] = distanceThroughU;
                previous[neighbor] = current;
                pq.push({distances[neighbor], neighbor});
            }
        }
    }

    vector<int> path;  // Rekonstruujemy sciezke
    for (int at = target_vertex; at != -1; at = previous[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());
    if (path.size() == 1 && path[0] != start_vertex) {
        path.clear();
    }
    return path;
}
// Funkcja rysuje linie, w ktorej dwa kolory postepuja po sobie wzajemnie
void draw_dashed_line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, const SDL_Color& color1, const SDL_Color& color2, double seg_length, int thickness) {
    double totalDistance = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    double dx = (x2 - x1) / totalDistance;
    double dy = (y2 - y1) / totalDistance;

    double currentLength = 0.0;
    bool useColor1 = true;

    int thickness_offset = thickness / 2;

    while (currentLength < totalDistance) {
        double currentseg_length = min(seg_length, totalDistance - currentLength);
        double endX = x1 + (currentLength + currentseg_length) * dx;
        double endY = y1 + (currentLength + currentseg_length) * dy;

        SDL_SetRenderDrawColor(renderer, useColor1 ? color1.r : color2.r,  // Kanal alfa
                               useColor1 ? color1.g : color2.g, useColor1 ? color1.b : color2.b, 255);

        for (double i = -thickness_offset; i <= thickness_offset; i += 0.05) {
            SDL_RenderDrawLine(renderer,
                               int(x1 + currentLength * dx + i * dy),
                               int(y1 + currentLength * dy - i * dx),
                               int(endX + i * dy),
                               int(endY - i * dx));
        }

        currentLength += currentseg_length;
        useColor1 = !useColor1;  // Zmieniamy kolor dla nastepnego segmentu
    }
}

void draw_thick_line(SDL_Renderer* renderer, int x0, int y0, int x1, int y1, int thickness, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    int thickness_offset = thickness / 2;

    while (true) {
        for (int i = -thickness_offset; i <= thickness_offset; i++) {  // Pogrubiamy linie dorysowujac wokol niej piksele
            for (int j = -thickness_offset; j <= thickness_offset; j++) {
                SDL_RenderDrawPoint(renderer, x0 + i, y0 + j);
            }
        }

        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

vector<Point> compute_convex_hull(vector<Point>& points) {
    if (points.size() < 3) return {};
    sort(points.begin(), points.end(), compare_point);
    vector<Point> hull;

    for (auto& point : points) {  // Otoczka dolna
        while (hull.size() >= 2 && cross_product(hull[hull.size() - 2], hull[hull.size() - 1], point) <= 0) {
            hull.pop_back();
        }
        hull.push_back(point);
    }

    int t = hull.size() + 1;  // Otoczka gorna
    for (int i = points.size() - 2; i >= 0; i--) {
        while (hull.size() >= t && cross_product(hull[hull.size() - 2], hull[hull.size() - 1], points[i]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }

    hull.pop_back();
    return hull;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    if (first == string::npos || last == string::npos) {
        return "";
    }
    return str.substr(first, (last - first + 1));
}

int find_next_index_on_hull(const vector<Point>& hull, int curr_index) {
    return (curr_index + 1) % hull.size();
}

Point create_pt_at_dstnc(const Point& start, const Point& end) {  // Korekcja punktu gdyby znalazł sie poza otoczka i dynamiczne wyznaczenie kierunku
    double dx = end.x - start.x;
    double dy = end.y - start.y;
    double dist = sqrt(dx * dx + dy * dy);
    double unitDist = 1.0;

    double normX = dx / dist;
    double normY = dy / dist;

    return {
        start.x + normX * unitDist,
        start.y + normY * unitDist};
}
string rmv_interpunction(const string& tekst) {
    string wynik;
    remove_copy_if(tekst.begin(), tekst.end(), back_inserter(wynik), [](char c) {
        return ispunct(static_cast<unsigned char>(c));
    });
    return wynik;
}
unordered_map<char, set<char>> mirrored_letters() {
    unordered_map<char, set<char>> mirrored_letters_map;
    mirrored_letters_map['a'] = {'a', 'e'};
    mirrored_letters_map['b'] = {'b', 'q', 'd', 'p'};
    mirrored_letters_map['c'] = {'c', 'u'};
    mirrored_letters_map['d'] = {'d', 'b', 'p', 'q'};
    mirrored_letters_map['e'] = {'e', 'a'};
    mirrored_letters_map['h'] = {'h', 'y'};
    mirrored_letters_map['m'] = {'m', 'w'};
    mirrored_letters_map['n'] = {'n', 'u'};
    mirrored_letters_map['p'] = {'p', 'b', 'q', 'd'};
    mirrored_letters_map['q'] = {'q', 'b', 'p', 'd'};
    mirrored_letters_map['u'] = {'u', 'n', 'c'};
    mirrored_letters_map['w'] = {'w', 'm'};
    mirrored_letters_map['y'] = {'y', 'h'};
    return mirrored_letters_map;
}