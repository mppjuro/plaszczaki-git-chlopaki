#ifndef helpers
#define helpers
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
using namespace std;

struct Point;
struct Road;
struct Porter;
struct Edge;

using Graph = unordered_map < int, vector < Edge >> ;
double distance_to_origin(const Point & point);
bool compare_point(const Point & a, const Point & b);
int cross_product(const Point & O, const Point & A, const Point & B);
bool are_segments_equal(const Point & a1, const Point & b1, const Point & a2, const Point & b2);
double calc_distance(const Point & p1, const Point & p2);
void compute_min_xy(const vector < Point > & points, double & minX, double & minY);
bool compare_by_angle(const Point & p1, const Point & p2);
double distance_to_origin(const Point & point);
Graph create_graph(const vector < Point > & points, const vector < Road > & roads);
vector < Point > intersect_circle_line(Point circle_center, double radius, Point line_start, Point line_end);
int find_index_in_pts(const Point & point, const vector < Point > & points);
vector < int > dijkstra(const Graph & graph, int start_vertex, int target_vertex);
void draw_dashed_line(SDL_Renderer * renderer, int x1, int y1, int x2, int y2, const SDL_Color & color1, const SDL_Color & color2, double seg_length, int thickness);
void draw_thick_line(SDL_Renderer * renderer, int x0, int y0, int x1, int y1, int thickness, SDL_Color color);
vector < Point > compute_convex_hull(vector < Point > & points);
string trim(const string & str);
int find_next_index_on_hull(const vector < Point > & hull, int curr_index);
Point create_pt_at_dstnc(const Point & start, const Point & end);
using Graph = unordered_map < int, vector < Edge >> ;
double distance_to_origin(const Point & point);
#endif