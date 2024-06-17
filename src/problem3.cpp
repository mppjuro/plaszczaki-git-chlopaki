#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#define infile "data/prob1/hull.txt"

using namespace std;

struct Flatlanders {
    int number;
    int stamina;
};

struct Pts {
    int brightness;
    int x;
    int y;
};

void hire_the_guards(vector<Flatlanders> &guards) {  // Wybor straznikow z najwieksza iloscia energii do obchodzenia plotu
    sort(guards.begin(), guards.end(), [](const Flatlanders &a, const Flatlanders &b) {
        return a.stamina > b.stamina;
    });
}

void prepare_path(int size, int arr[], vector<Pts> &pts, int stamina) {  // Wyznaczamy koordynaty przejscia straznika przez plot

    int number_of_stops = 0;
    int number_of_rests = 0;
    int curr = arr[0];
    vector<pair<int, int>> stops;
    int i = 0;
    int tmp = 0;
    int loop_completed = 0;
    int furthest_move = 0;
    int have_to_rest = 1;
    while (i < size) {
        furthest_move = stamina;
        for (int j = 0; j < stamina; j++) {  // Przeszukujemy stamina-elementow do przodu

            if (i + j == size - 1) {  // Ostatni element to tez pierwszy, wiec w tym przypadku konczymy obchod od razu
                have_to_rest = 0;
                loop_completed = 1;
                break;
            }
            tmp = i + j + 1;
            if (curr <= arr[tmp]) {
                furthest_move = j + 1;
                have_to_rest = 0;
            }
        }
        if (have_to_rest) {
            number_of_rests++;
        }
        if (!loop_completed && i + furthest_move < size) {
            number_of_stops++;
            curr = arr[i + furthest_move];
            stops.emplace_back(curr, i + furthest_move);
        }
        i += furthest_move;
        have_to_rest = 1;
    }
    cout << "Guard has to travel through following points: \n\n";
    for (const auto &p : pts) {
        cout << "x: " << p.x << ", y: " << p.y << ", brightness of the point: " << p.brightness << '\n';
    }
    cout << '\n';

    cout << "The guard has to stop at following stops:\n";
    for (const auto &p : stops) {
        cout << "x: " << pts[p.second].x << ", y: " << pts[p.second].y << ", brightness of the point: " << p.first << '\n';
    }
    cout << '\n';
    cout << "Guard had to rest at " << number_of_rests << " stops.\n";
}

int problem3() {
    ios::sync_with_stdio(false);
    srand(time(nullptr));

    vector<Flatlanders> guards;
    vector<Pts> pts;

    Flatlanders guard{};
    Pts point;

    int how_many_pts;
    int first_x;
    int first_y;
    int first_brightness;
    int hull[100];

    ifstream input(infile);
    if (!input) {
        cerr << "Cannot open the input file!\n";
        return 1;
    }
    input >> how_many_pts;

    for (int i = 0; i < 10; i++) {  // Losujemy energie Plaszczakow, ktore moga zostac straznikami
        guard.number = i;
        guard.stamina = rand() % 10 + 1;
        guards.push_back(guard);
    }

    for (int i = 0; i < how_many_pts; i++) {  // Wczytujemy koordynaty z pliku jako punkty na trasach straznikow
        int x, y;
        input >> x >> y;
        point.x = x;
        point.y = y;
        point.brightness = rand() % 100 + 1;
        if (i == 0) {
            first_x = x;
            first_y = y;
            first_brightness = point.brightness;
        }
        pts.push_back(point);
    }
    point.x = first_x;
    point.y = first_y;
    point.brightness = first_brightness;
    pts.push_back(point);
    input.close();

    reverse(pts.begin(), pts.end());  // W pliku "hull.txt" punkty na plocie sa w kierunku odwrotnym do wskazowek zegara, dlatego je odwracamy

    for (int i = 0; i <= how_many_pts; i++) {  // Przepisujemy jasnosc z vectora do tablicy
        hull[i] = pts[i].brightness;
    }
    hull[how_many_pts - 1] = hull[0];  // Dodajemy ostatni punkt taki sam jak pierwszy aby zaczal i konczyl w tym samym miejscu

    int *pts_indexes = (int *)malloc(how_many_pts * sizeof(int));
    for (int i = 0; i < how_many_pts; i++) {
        pts_indexes[i] = 0;
    }
    free(pts_indexes);
    hire_the_guards(guards);  // Przystepujemy do wyboru straznikow, ktorzy maja najwiecej energii
    for (int i = 0; i < 7; i++) {
        cout << "<--------------------------------->\n";
        cout << "Guard number " << guards[i].number << ", has " << guards[i].stamina << " points of stamina.\n";
        prepare_path(how_many_pts, hull, pts, guards[i].stamina);  // Patrolujemy plot
        cout << '\n';
    }

    cout << "Guards have patrolled the whole border and didn't find any signs of outer-dimensional threats :)\n";

    return 0;
}