#include <bits/stdc++.h>
#define infile "./outputs/pts.txt"
//
//	ej zbiku
// czym jest skok xDDDD
//
//
using namespace std;

struct Flatlanders{
    int number;
    int stamina;
};

struct Pts{
    int brightness;
    int x;
    int y;
};

void hire_the_guards(vector<Flatlanders> &guards){									//Wybor straznikow z najwieksza iloscia energii do obchodzenia plotu
    sort(guards.begin(), guards.end(), [](const Flatlanders &a, const Flatlanders &b){
        return a.stamina > b.stamina;
    });
}

void prepare_path(int size, int arr[], vector<Pts>& pts){							//Wyznaczamy koordynaty przejscia straznika przez plot

    int skok = 3;
    int number_of_stops = 0;
    int curr = arr[0];
    vector<pair<int, int>> vec;
    vector<pair<int, int>> stops;

    int i = 0;
    while(i < size - 1){
        vec.push_back( {curr, i}); 													//Ustalamy pozycje startowa
        int max_val = -1;
        int max_idx = -1;

        for (int j = 1; j <= skok && i + j < size; j++){							//Przeszukujemy skok-elementow do przodu
            if(arr[i + j] <= curr && arr[i + j] > max_val){
                max_val = arr[i + j];
                max_idx = i + j;
            }
        }

        if(max_idx != -1){											
            curr = max_val;
            i = max_idx;
        } 
		else{
            int next_stop = min(i + skok, size - 1);
            stops.push_back( {arr[next_stop], next_stop});
            curr = arr[next_stop];
            i = next_stop;
        }
    }
    vec.push_back({arr[size - 1], size - 1});


    cout << "Guard has to travel through following points: \n\n";
    for (const auto& p : vec){
        cout << "x: " << pts[p.second].x << ", y: " << pts[p.second].y << ", brightness of the point: " << p.first << '\n';
    }
    cout << '\n';

    cout << "The guard has to stop at following stops:\n";
    for (const auto& p : stops){
        cout << "x: " << pts[p.second].x << ", y: " << pts[p.second].y << ", brightness of the point: " << p.first << '\n';
        number_of_stops++;
    }
    cout << '\n';
    cout << "Guard had to stop at " << number_of_stops << " stops.\n";
}


int main(){
	
	ios::sync_with_stdio(false);
    srand(time(NULL));

    vector<Flatlanders> guards;
    vector<Pts> pts;

    Flatlanders guard;
    Pts point;

    int how_many_pts;
    int first_x;
    int first_y;
    int first_brightness;
    int hull[100];


    ifstream input(infile);
    if(!input){
        cerr << "Cannot open the input file!\n";
        return 1;
    }
    input >> how_many_pts;


    for(int i = 0; i < 10; i++){					//Losujemy energie Plaszczakow, ktore moga zostac straznikami 
        guard.number = i;
        guard.stamina = rand() % 10 + 1;
        guards.push_back(guard);
    }


    for(int i = 0; i < how_many_pts; i++){			// Wczytujemy koordynaty z pliku jako punkty na trasach straznikow
        int x, y;
        input >> x >> y;
        point.x = x;
        point.y = y;
        point.brightness = rand() % 100 + 1;
        if(i == 0){
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

    for(int i = 0; i <= how_many_pts; i++){			//Przepisujemy jasnosc z vectora do arrlicy
        hull[i] = pts[i].brightness;
    }
    hull[how_many_pts -1] = hull[0]; 				//Dodajemy ostatni punkt taki sam jak pierwszy aby zaczal i konczyl w tym samym miejscu

    
	int pts_indexes[100];
    for(int i = 0; i <= how_many_pts; i++){
        pts_indexes[i]= 0;
    }
    
    hire_the_guards(guards);						//Przystepujemy do wyboru straznikow, ktorzy maja najwiecej energii
    
    for(int i = 0; i < 7; i++){
        cout << "<--------------------------------->\n";
        cout << "Guard number " << guards[i].number << ", has " << guards[i].stamina << " points of stamina.\n";
        prepare_path(guards[i].stamina, hull, pts);
        cout << '\n';
    }

    cout << "Guards have patrolled the whole border and didn't find any signs of outer-dimensional threats.'\n";
    
    return 0;
}