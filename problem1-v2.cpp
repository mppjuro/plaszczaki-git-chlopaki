//compile command: g++ -std=c++17 -o "problem1" "problem1.cpp" -IC:\SDL2-devel-2.0.22-mingw\SDL2-2.0.22\x86_64-w64-mingw32\include\SDL2 -LC:\SDL2-devel-2.0.22-mingw\SDL2-2.0.22\x86_64-w64-mingw32\lib -lmingw32 -lSDL2main -lSDL2 -mwindows -IC:\SDL2_ttf-devel-2.22.0-mingw\SDL2_ttf-2.22.0\x86_64-w64-mingw32\include\SDL2 -LC:\SDL2_ttf-devel-2.22.0-mingw\SDL2_ttf-2.22.0\x86_64-w64-mingw32\lib -lSDL2_ttf
// wymagana wersja kompilatora: C++17 i zainstalowane biblioteki SDL2 (do grafiki) oraz SDL2 TTF (do tekstu w oknie graficznym)
#include <SDL.h>
#include <SDL_ttf.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>
#include <random>
#include <unordered_map>
#include <queue>
#include <limits>
#include <functional>

double minX, minY;

struct Point {
  double x, y;
  int label; // Dodajemy etykietÄ™ dla punktu
};

struct Road {
  Point start;
  Point end;
};

struct Porter {
  int nr;
  std::string typ;
  std::vector < int > sympatie;
};

struct Edge {
  int target;
  double weight;
};

using Graph = std::unordered_map < int, std::vector < Edge >> ;

double distanceToOrigin(const Point & point) {
  return std::sqrt(std::pow(point.x, 2) + std::pow(point.y, 2));
}

bool comparePoint(const Point & a,
  const Point & b) {
  return a.x < b.x || (a.x == b.x && a.y < b.y);
}

int crossProduct(const Point & O,
  const Point & A,
    const Point & B) {
  return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

bool areSegmentsEqual(const Point & a1,
  const Point & b1,
    const Point & a2,
      const Point & b2) {
  return (a1.x == a2.x && a1.y == a2.y && b1.x == b2.x && b1.y == b2.y) ||
    (a1.x == b2.x && a1.y == b2.y && b1.x == a2.x && b1.y == a2.y);
}

double calculateDistance(const Point & p1,
  const Point & p2) {
  return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

void computeMinXY(const std::vector < Point > & points, double & minX, double & minY) {
  // Initialize minX and minY with maximum possible values
  minX = std::numeric_limits < double > ::max();
  minY = std::numeric_limits < double > ::max();

  // Iterate through all points to find the minimum x and y coordinates
  for (const auto & point: points) {
    if (point.x < minX) {
      minX = point.x;
    }
    if (point.y < minY) {
      minY = point.y;
    }
  }
}

bool compareByAngle(const Point & p1,
  const Point & p2) {
  // Obliczamy kÄ…t kaĹĽdego punktu wzglÄ™dem osi X
  double angle1 = atan2(p1.y - minY, p1.x - minX);
  double angle2 = atan2(p2.y - minY, p2.x - minX);

  // Upewniamy siÄ™, ĹĽe kÄ…ty mieszczÄ… siÄ™ w przedziale od 0 do 2*pi
  if (angle1 < 0) angle1 += 2 * M_PI;
  if (angle2 < 0) angle2 += 2 * M_PI;

  // Sortujemy zgodnie z ruchem wskazĂłwek zegara
  return angle1 < angle2;
}

Graph createGraph(const std::vector < Point > & points,
  const std::vector < Road > & roads) {
  Graph graph;
  for (int i = 0; i < points.size(); ++i) {
    graph[i] = {}; // Initialize the adjacency list for each point
  }

  for (const auto & road: roads) {
    int start_index = -1, end_index = -1;
    for (int i = 0; i < points.size(); ++i) {
      if (points[i].x == road.start.x && points[i].y == road.start.y) {
        start_index = i;
      }
      if (points[i].x == road.end.x && points[i].y == road.end.y) {
        end_index = i;
      }
    }
    if (start_index != -1 && end_index != -1) {
      double dist = calculateDistance(road.start, road.end);
      graph[start_index].push_back({
        end_index,
        dist
      });
      graph[end_index].push_back({
        start_index,
        dist
      }); // Assuming undirected graph
    }
  }
  return graph;
}

// Function to calculate the intersection of a circle and a line segment
std::vector < Point > intersectCircleLine(Point circleCenter, double radius, Point lineStart, Point lineEnd) {
  double cx = circleCenter.x, cy = circleCenter.y;
  double x1 = lineStart.x, y1 = lineStart.y;
  double x2 = lineEnd.x, y2 = lineEnd.y;

  double dx = x2 - x1, dy = y2 - y1;
  double A = dx * dx + dy * dy;
  double B = 2 * (dx * (x1 - cx) + dy * (y1 - cy));
  double C = (x1 - cx) * (x1 - cx) + (y1 - cy) * (y1 - cy) - radius * radius;

  double det = B * B - 4 * A * C;
  std::vector < Point > intersections;
  if (A <= 0.0000001 || det < 0) {
    // No real solutions, find closest point on the line to the circle center
    double t = -B / (2 * A);
    double closest_x = x1 + t * dx;
    double closest_y = y1 + t * dy;
    intersections.push_back({
      closest_x,
      closest_y
    });
  } else if (det == 0) {
    // One solution.
    double t = -B / (2 * A);
    intersections.push_back({
      x1 + t * dx,
      y1 + t * dy
    });
  } else {
    // Two solutions.
    double t1 = (-B + std::sqrt(det)) / (2 * A);
    double t2 = (-B - std::sqrt(det)) / (2 * A);
    intersections.push_back({
      x1 + t1 * dx,
      y1 + t1 * dy
    });
    intersections.push_back({
      x1 + t2 * dx,
      y1 + t2 * dy
    });
  }

  // Filter points that do not lie on the segment
  std::vector < Point > onSegment;
  for (const auto & pt: intersections) {
    if (std::min(x1, x2) <= pt.x && pt.x <= std::max(x1, x2) &&
      std::min(y1, y2) <= pt.y && pt.y <= std::max(y1, y2)) {
      onSegment.push_back(pt);
    }
  }

  return onSegment;
}

int findIndexInPoints(const Point & point,
  const std::vector < Point > & points) {
  for (int i = 0; i < points.size(); ++i) {
    if (points[i].x == point.x && points[i].y == point.y) {
      return i;
    }
  }
  return -1; // ZwrĂłÄ‡ -1 jeĹ›li punkt nie jest znaleziony
}

std::vector < int > dijkstra(const Graph & graph, int startVertex, int targetVertex) {
  std::priority_queue < std::pair < double, int > , std::vector < std::pair < double, int >> , std::greater < >> pq;
  std::vector < double > distances(graph.size(), std::numeric_limits < double > ::max());
  std::vector < int > previous(graph.size(), -1);
  std::vector < bool > visited(graph.size(), false);

  pq.push({
    0,
    startVertex
  });
  distances[startVertex] = 0;

  while (!pq.empty()) {
    int current = pq.top().second;
    pq.pop();

    if (visited[current]) continue;
    visited[current] = true;

    if (current == targetVertex) break;

    for (const auto & edge: graph.at(current)) {
      int neighbor = edge.target;
      double weight = edge.weight;
      double distanceThroughU = distances[current] + weight;
      if (distanceThroughU < distances[neighbor]) {
        distances[neighbor] = distanceThroughU;
        previous[neighbor] = current;
        pq.push({
          distances[neighbor],
          neighbor
        });
      }
    }
  }

  // Reconstruct path
  std::vector < int > path;
  for (int at = targetVertex; at != -1; at = previous[at]) {
    path.push_back(at);
  }
  std::reverse(path.begin(), path.end());
  if (path.size() == 1 && path[0] != startVertex) {
    path.clear(); // no path found
  }
  return path;
}

// Function to draw a line alternating between two colors
void drawDashedLine(SDL_Renderer * renderer, int x1, int y1, int x2, int y2,
  const SDL_Color & color1,
    const SDL_Color & color2, double segmentLength, int thickness) {
  double totalDistance = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
  double dx = (x2 - x1) / totalDistance;
  double dy = (y2 - y1) / totalDistance;

  double currentLength = 0.0;
  bool useColor1 = true;

  int thickness_offset = thickness / 2; // Half thickness to spread the thickness around the line

  while (currentLength < totalDistance) {
    double currentSegmentLength = std::min(segmentLength, totalDistance - currentLength);
    double endX = x1 + (currentLength + currentSegmentLength) * dx;
    double endY = y1 + (currentLength + currentSegmentLength) * dy;

    // Set the color for the current segment
    SDL_SetRenderDrawColor(renderer,
      useColor1 ? color1.r : color2.r,
      useColor1 ? color1.g : color2.g,
      useColor1 ? color1.b : color2.b,
      255); // Alpha channel

    // Draw thick line segment
    for (double i = -thickness_offset; i <= thickness_offset; i += 0.05) {
      SDL_RenderDrawLine(renderer,
        int(x1 + currentLength * dx + i * dy),
        int(y1 + currentLength * dy - i * dx),
        int(endX + i * dy),
        int(endY - i * dx));
    }

    // Prepare for the next segment
    currentLength += currentSegmentLength;
    useColor1 = !useColor1; // Switch colors for the next segment
  }
}

void drawThickLine(SDL_Renderer * renderer, int x0, int y0, int x1, int y1, int thickness, SDL_Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  int dx = std::abs(x1 - x0);
  int dy = -std::abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;

  int thickness_offset = thickness / 2; // Offset to spread the thickness around the line

  while (true) {
    // Draw pixels around the current point to make the line thick
    for (int i = -thickness_offset; i <= thickness_offset; i++) {
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

std::vector < Point > computeConvexHull(std::vector < Point > & points) {
  if (points.size() < 3) return {};
  std::sort(points.begin(), points.end(), comparePoint);
  std::vector < Point > hull;

  // Lower hull
  for (auto & point: points) {
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

std::string trim(const std::string & str) {
  size_t first = str.find_first_not_of(" \t\n\r");
  size_t last = str.find_last_not_of(" \t\n\r");
  if (first == std::string::npos || last == std::string::npos) {
    return "";
  }
  return str.substr(first, (last - first + 1));
}

int findNextIndexOnHull(const std::vector < Point > & hull, int currentIndex) {
  // Return the next index, wrap around if at the end
  return (currentIndex + 1) % hull.size();
}

// Correcting the point creation to ensure it's on the hull and adjusting direction dynamically
Point createPointAtDistance(const Point & start,
  const Point & end) {
  double dx = end.x - start.x;
  double dy = end.y - start.y;
  double dist = sqrt(dx * dx + dy * dy);
  double unitDist = 1.0; // distance to move along the hull edge

  // Normalize the direction
  double normX = dx / dist;
  double normY = dy / dist;

  // Scale by unit distance
  return {
    start.x + normX * unitDist,
    start.y + normY * unitDist
  };
}

int main(int argc, char * argv[]) {
  std::map < std::string, int > config;
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

  SDL_Color convexColor = {
    static_cast < Uint8 > (config["CONVEX_COLOR_R"]),
    static_cast < Uint8 > (config["CONVEX_COLOR_G"]),
    static_cast < Uint8 > (config["CONVEX_COLOR_B"]),
    static_cast < Uint8 > (config["CONVEX_COLOR_A"])
  }; // Kolor dla normalnych odcinkĂłw otoczki
  SDL_Color roadColor = {
    static_cast < Uint8 > (config["ROAD_COLOR_R"]),
    static_cast < Uint8 > (config["ROAD_COLOR_G"]),
    static_cast < Uint8 > (config["ROAD_COLOR_B"]),
    static_cast < Uint8 > (config["ROAD_COLOR_A"])
  }; // Kolor dla normalnych odcinkĂłw otoczki
  SDL_Color pointColor = {
    static_cast < Uint8 > (config["POINT_COLOR_R"]),
    static_cast < Uint8 > (config["POINT_COLOR_G"]),
    static_cast < Uint8 > (config["POINT_COLOR_B"]),
    static_cast < Uint8 > (config["POINT_COLOR_A"])
  }; // Kolor dla normalnych odcinkĂłw otoczki

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Error: SDL could not initialize - " << SDL_GetError() << std::endl;
    return 1;
  }

  if (TTF_Init() == -1) {
    std::cerr << "Error: SDL_ttf could not initialize - " << TTF_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  TTF_Font * font = TTF_OpenFont("LiberationSans-Regular.ttf", 12);
  if (!font) {
    std::cerr << "Error: Failed to load font - " << TTF_GetError() << std::endl;
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  int margin = config["MARGIN"];
  int window_width = config["WINDOW_WIDTH"];
  int window_height = config["WINDOW_HEIGHT"];
  SDL_Window * window = SDL_CreateWindow("Mapa Punkty", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width + 2 * margin, window_height + 2 * margin, 0);
  if (!window) {
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  std::vector < Point > points;
  std::ifstream points_file("punkty.txt");
  int liczbaPunktow;
  int x, y;
  if (!(points_file >> liczbaPunktow)) {
    std::cerr << "Error reading number of points from punkty.txt" << std::endl;
    return 1; // or handle error appropriately
  }

  while (liczbaPunktow--> 0 && points_file >> x >> y) {
    points.push_back({
      double(x),
      double(y)
    });
  }
  points_file.close();

  //wczytaj drogi
  std::vector < Road > roads;
  std::ifstream roads_file("drogi.txt");
  if (!roads_file.is_open()) {
    std::cerr << "Error: Could not open roads file." << std::endl;
    return 1;
  }

  int numberOfRoads, x1, y1, x2, y2;
  roads_file >> numberOfRoads; // Read the number of roads from the first line

  while (numberOfRoads--> 0 && roads_file >> x1 >> y1 >> x2 >> y2) {
    roads.push_back({
      {
        double(x1), double(y1)
      },
      {
        double(x2),
        double(y2)
      }
    });
  }
  roads_file.close();

  std::vector < Point > hull = computeConvexHull(points);

  SDL_SetRenderDrawColor(renderer, pointColor.r, pointColor.g, pointColor.b, pointColor.a);
  // Render points and text
  for (auto & point: points) {
    int render_x = margin + (int)(((point.x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
    int render_y = margin + (int)(((point.y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
    render_y = window_height + 2 * margin - render_y; // Invert y-axis

    SDL_Rect rect = {
      render_x - 5,
      render_y - 5,
      10,
      10
    }; // Draw slightly larger points
    SDL_RenderFillRect(renderer, & rect);

    // Render text
    SDL_Color textColor = {
      255,
      255,
      255
    }; // White color for the text
    std::string text = std::to_string(int(point.x)) + ", " + std::to_string(int(point.y));
    SDL_Surface * textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture * textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {
      render_x + 10,
      render_y - 10,
      textSurface -> w,
      textSurface -> h
    };
    SDL_RenderCopy(renderer, textTexture, NULL, & textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
  }

  for (int i = 0; i < hull.size(); i++) {
    int next = (i + 1) % hull.size();
    int start_x = margin + (int)(((hull[i].x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
    int start_y = margin + (int)(((hull[i].y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
    start_y = window_height + 2 * margin - start_y; // Invert y-axis

    int end_x = margin + (int)(((hull[next].x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
    int end_y = margin + (int)(((hull[next].y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
    end_y = window_height + 2 * margin - end_y; // Invert y-axis

    bool isRoad = false;
    for (const auto & road: roads) {
      if (areSegmentsEqual(hull[i], hull[next], road.start, road.end)) {
        drawDashedLine(renderer, start_x, start_y, end_x, end_y, convexColor, roadColor, config["DASH_LENGTH"], config["LINE_THICKNESS_PX"]);
        isRoad = true;
        break;
      }
    }

    if (!isRoad) {
      //SDL_SetRenderDrawColor(renderer, convexColor.r, convexColor.g, convexColor.b, convexColor.a);
      drawThickLine(renderer, start_x, start_y, end_x, end_y, config["LINE_THICKNESS_PX"], convexColor);
      //SDL_RenderDrawLine(renderer, start_x, start_y, end_x, end_y);
    }
  }

  // Render roads, skipping those that are on the convex hull
  //SDL_SetRenderDrawColor(renderer, roadColor.r, roadColor.g, roadColor.b, roadColor.a);
  for (const auto & road: roads) {
    bool isPartOfHull = false;

    for (int i = 0; i < hull.size(); i++) {
      int next = (i + 1) % hull.size();
      if (areSegmentsEqual(hull[i], hull[next], road.start, road.end)) {
        isPartOfHull = true;
        break;
      }
    }

    if (!isPartOfHull) { // Only draw the road if it is not part of the hull
      int start_x = margin + (int)(((road.start.x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
      int start_y = margin + (int)(((road.start.y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
      start_y = window_height + 2 * margin - start_y; // Invert y-axis

      int end_x = margin + (int)(((road.end.x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
      int end_y = margin + (int)(((road.end.y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
      end_y = window_height + 2 * margin - end_y; // Invert y-axis
      drawThickLine(renderer, start_x, start_y, end_x, end_y, config["LINE_THICKNESS_PX"], roadColor);
      //SDL_RenderDrawLine(renderer, start_x, start_y, end_x, end_y);
    }
  }

  std::ifstream file("tragarze.txt");
  if (!file.is_open()) {
    std::cerr << "Error: Could not open the porters file." << std::endl;
    return 1;
  }

  int liczbaTragarzy;
  if (!(file >> liczbaTragarzy)) {
    std::cerr << "Error: Failed to read the number of porters." << std::endl;
    file.close();
    return 1;
  }

  std::vector < Porter > porters;
  std::getline(file, line); // Clear the newline character

  for (int i = 0; i < liczbaTragarzy; ++i) {
    std::getline(file, line);
    std::stringstream ss(line);
    Porter porter;
    ss >> porter.nr >> porter.typ;

    int sympatia;
    while (ss >> sympatia) {
      porter.sympatie.push_back(sympatia);
      if (ss.peek() == ';' || ss.peek() == ',')
        ss.ignore();
    }

    porters.push_back(porter);
  }
  file.close();

  // ZnajdĹş pary tragarzy o rĂłĹĽnych typach i obustronnej sympatii
  std::vector < std::pair < int, int >> paryTragarzy;
  std::vector < bool > tragarzDostepny(porters.size(), true);

  for (size_t i = 0; i < porters.size(); ++i) {
    if (!tragarzDostepny[i]) {
      continue; // JeĹ›li tragarz nie jest dostÄ™pny, przejdĹş do nastÄ™pnego
    }

    for (size_t j = i + 1; j < porters.size(); ++j) {
      if (!tragarzDostepny[j]) {
        continue; // JeĹ›li tragarz nie jest dostÄ™pny, przejdĹş do nastÄ™pnego
      }

      if (porters[i].typ != porters[j].typ) {
        bool sympatiaAtoB = false;
        bool sympatiaBtoA = false;
        for (int sympatiaA: porters[i].sympatie) {
          if (sympatiaA == porters[j].nr) {
            sympatiaAtoB = true;
            break;
          }
        }
        for (int sympatiaB: porters[j].sympatie) {
          if (sympatiaB == porters[i].nr) {
            sympatiaBtoA = true;
            break;
          }
        }
        if (sympatiaAtoB && sympatiaBtoA) {
          paryTragarzy.push_back({
            porters[i].nr,
            porters[j].nr
          });
          tragarzDostepny[i] = false;
          tragarzDostepny[j] = false;
          break; // Znaleziono parÄ™, przejdĹş do nastÄ™pnego tragarza
        }
      }
    }
  }

  // Zapisz pary tragarzy do pliku
  std::ofstream outFile("pary_tragarzy.txt");
  if (!outFile.is_open()) {
    std::cerr << "Error: Could not open the output file." << std::endl;
    return 1;
  }

  outFile << paryTragarzy.size() << std::endl; // Zapisz liczbÄ™ par tragarzy w pierwszej linii

  for (const auto & para: paryTragarzy) {
    outFile << para.first << " " << para.second << std::endl; // Zapisz numery tragarzy w parze
  }

  outFile.close();

  //zrobienie fabryki
  // Find the point closest to the origin
  Point closestPoint;
  double minDistance = std::numeric_limits < double > ::max();
  for (const auto & point: points) {
    double distance = distanceToOrigin(point);
    if (distance < minDistance) {
      minDistance = distance;
      closestPoint = point;
    }
  }

  // Set the color of the closest point to white
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  int render_x = margin + (int)(((closestPoint.x - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_width);
  int render_y = margin + (int)(((closestPoint.y - config["MIN_WSPOLRZEDNA"]) / (double)(config["MAX_WSPOLRZEDNA"] - config["MIN_WSPOLRZEDNA"])) * window_height);
  render_y = window_height + 2 * margin - render_y; // Invert y-axis
  SDL_Rect rect = {
    render_x - 5,
    render_y - 5,
    10,
    10
  }; // Draw slightly larger points
  SDL_RenderFillRect(renderer, & rect);
  Point selectedPoint;
  if (!hull.empty()) {
    std::random_device rd; // Obtain a random number from hardware
    std::mt19937 eng(rd()); // Seed the generator
    std::uniform_int_distribution < > distr(0, hull.size() - 1); // Define the range

    // Randomly select a point from the convex hull
    int randomIndex = distr(eng);
    //selectedPoint = hull[randomIndex];
    selectedPoint = hull[0];
  }
  Graph graph = createGraph(points, roads);
  // Find indices for closestPoint and selectedPoint
  int closestIndex = 0, selectedIndex = 0;
  std::vector < int > path;
  // Optionally draw the path or display the distance
  double totalDistance = 0;

  // Write the total distance to the file "koszt.txt"
  std::ofstream kosztFile("koszt.txt");
  if (kosztFile.is_open()) {
    computeMinXY(points, minX, minY);
    // Sort the hull points by angle with respect to the x-axis in clockwise order
    //std::sort(hull.begin(), hull.end(), compareByAngle);
    selectedIndex = 0;
    double radius = 1.0; // Radius for the calculation of the next point
    Point startowy = hull[0]; // Starting point
    Point currentPoint = selectedPoint;
    int nextIndex = (selectedIndex + 1) % hull.size();
    // Nadajemy etykiety punktom naleĹĽÄ…cym do otoczki wypukĹ‚ej
    //labelConvexHullPoints(hull, renderer, {255, 255, 255, 255}, font);

    do {
      closestIndex = findIndexInPoints(closestPoint, points);
      graph = createGraph(points, roads); // UtwĂłrz graf z aktualnymi punktami i drogami
      path = dijkstra(graph, closestIndex, selectedIndex); // Szukaj Ĺ›cieĹĽki
      for (size_t i = 0; i < path.size() - 1; ++i) {
        totalDistance += calculateDistance(points[path[i]], points[path[i + 1]]);
      }

      Point nextPoint = hull[nextIndex];
      double distanceToNextPoint = calculateDistance(currentPoint, nextPoint);
      if (distanceToNextPoint <= 1) {
        roads.push_back({
          currentPoint,
          nextPoint
        });
        graph = createGraph(points, roads);
        selectedIndex = findIndexInPoints(nextPoint, points); // Update selectedIndex for the next loop
        currentPoint = nextPoint; // Move currentPoint to nextPoint
        nextIndex = (nextIndex + 1) % hull.size(); // Move to the next index
      } else {
        auto intersections = intersectCircleLine(currentPoint, radius, currentPoint, nextPoint);
        if (!intersections.empty()) {
          Point validNextPoint = intersections.front();

          if (!(validNextPoint.x == currentPoint.x && validNextPoint.y == currentPoint.y)) {
            points.push_back(validNextPoint); // Dodaj nowy punkt do zbioru punktĂłw
            roads.push_back({
              currentPoint,
              validNextPoint
            }); // UtwĂłrz nowÄ… drogÄ™
            selectedIndex = points.size() - 1; // Ustaw nowy indeks dla nowo dodanego punktu
          }

          currentPoint = validNextPoint;

          if (currentPoint.x == nextPoint.x && currentPoint.y == nextPoint.y) {
            selectedIndex = findIndexInPoints(nextPoint, points);
            nextIndex = (nextIndex + 1) % hull.size();
            currentPoint = nextPoint;
          }
        } else {
          roads.push_back({
            currentPoint,
            nextPoint
          });
          selectedIndex = findIndexInPoints(nextPoint, points);
          nextIndex = (nextIndex + 1) % hull.size();
          currentPoint = hull[selectedIndex];
        }
      }
      kosztFile << totalDistance << std::endl;
      totalDistance = 0;
    } while (currentPoint.x != startowy.x || currentPoint.y != startowy.y);
  } else {
    std::cerr << "Unable to open file 'koszt.txt' for writing." << std::endl;
  }
  kosztFile.close();

  std::vector < double > distances;
  std::ifstream inFile("koszt.txt");
  if (!inFile.is_open()) {
    std::cerr << "Nie moĹĽna otworzyÄ‡ pliku koszt.txt do odczytu." << std::endl;
    return 654;
  }

  double value;
  double totalSum = 0.0;
  int count = 0;

  // Odczytaj wszystkie wartoĹ›ci z pliku i oblicz ich sumÄ™
  while (inFile >> value) {
    totalSum += (value * 2); // tragarz musi teĹĽ wrĂłciÄ‡, wiÄ™c odlegĹ‚oĹ›Ä‡ razy 2
    distances.push_back(value);
    count++;
  }

  inFile.close();

  // Teraz otwĂłrz ten sam plik do dopisywania (append mode)
  std::ofstream outResultFile("result.txt", std::ios::out);
  if (!outResultFile.is_open()) {
    std::cerr << "Nie moĹĽna otworzyÄ‡ pliku koszt.txt do zapisu." << std::endl;
    return 561;
  }

  // Dopisz sumÄ™ na koĹ„cu pliku
  outResultFile << "Zeby dostarczyc " << count << " odcinkow (lub ich fragmentow) do otoczenia Strony tragarze przebyli odleglosc: " << (2 * totalSum) << " co jest minimalnym kosztem wykonania tej pracy" << std::endl;

  int numberOfPorters = paryTragarzy.size();
  std::vector < double > porterSums(numberOfPorters, 0.0);

  // Przydzielanie zadaĹ„
  for (int i = 0; i < distances.size(); ++i) {
    int porterIndex = i % numberOfPorters; // KtĂłry tragarz ma wykonaÄ‡ to zadanie
    porterSums[porterIndex] += distances[i];
  }

  // Znajdowanie maksymalnej sumy odlegĹ‚oĹ›ci
  double maks = * std::max_element(porterSums.begin(), porterSums.end());

  // Obliczanie i wypisywanie czasu potrzebnego do wykonania zadaĹ„
  outResultFile << "Zajelo to " << (maks / config["PORTER_SPEED"]) << " czasu" << std::endl;
  outResultFile.close();

  //display
  SDL_RenderPresent(renderer);

  SDL_Event e;
  bool running = true;
  while (running) {
    if (SDL_WaitEvent( & e)) {
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
