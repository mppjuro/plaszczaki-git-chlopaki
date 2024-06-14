#include <bits/stdc++.h>
using namespace std;

struct Flatlanders {
    int numer;
    int stamina;
};

struct Pts {
    int brightness;
    int x;
    int y;
};

// Wybor guardow z najwieksza iloscia energii do obchodzenia plotu
void wybierz_najsilniejszych(vector<Flatlanders> &guards){
    sort(guards.begin(), guards.end(), [](const Flatlanders &a, const Flatlanders &b){
        return a.stamina > b.stamina;
    });
}

// Wyznaczamy koordynaty przejscia guarda przez plot
void wyznacz_droge(int rozmiar, int tab[], vector<Pts>& pts){

    int skok = 3;
    int ile_przystankow = 0;
    int curr = tab[0];
    vector<pair<int, int>> vec;
    vector<pair<int, int>> przystanki;

    int i = 0;
    while(i < rozmiar - 1){
        vec.push_back( {curr, i}); //wrzucamy pozycje startowa
        int max_val = -1;
        int max_idx = -1;

        // przeszukujemy skok- elementow do przodu
        for (int j = 1; j <= skok && i + j < rozmiar; j++){
            if(tab[i + j] <= curr && tab[i + j] > max_val){
                max_val = tab[i + j];
                max_idx = i + j;
            }
        }

        if(max_idx != -1){
            // znaleziono maxa
            curr = max_val;
            i = max_idx;
        } else{
            //nie znaleziono maxa
            int next_stop = min(i + skok, rozmiar - 1);
            przystanki.push_back( {tab[next_stop], next_stop});
            curr = tab[next_stop];
            i = next_stop;
        }
    }
    //Dodanie ostatniego elementu do vecaa
    vec.push_back( {tab[rozmiar - 1], rozmiar - 1});


    cout << "guard ten musi przejsc przez nastepujace pts: \n\n";
    for (const auto& p : vec){
        cout << "x: " << pts[p.second].x << ", y: " << pts[p.second].y << ", brightness pointu: " << p.first << endl;
    }
    cout << endl;

    cout << "Niestety zatrzymac musi sie na przystankach: \n";
    for (const auto& p : przystanki){
        cout << "x: " << pts[p.second].x << ", y: " << pts[p.second].y << ", brightness pointu: " << p.first << endl;
        ile_przystankow++;
    }
    cout << endl;
    cout << "guard musial zatrzymac sie na " << ile_przystankow << " przystankach\n";
}


int main(){

    srand(time(nullptr));

    vector<Flatlanders> guards;
    vector<Pts> pts;

    Flatlanders guard;
    Pts point;

    int how_many_pts;
    int first_x;
    int first_y;
    int first_brightness;
    int hull[100];


    ifstream odczyt;
    odczyt.open("pts_hull_wypukla.txt");
    if(!odczyt){
        cerr << "Nie można otworzyć pliku do odczytu" << endl;
        return 1;
    }
    odczyt >> how_many_pts;



//losujemy energie guardom (TU MOZE TEZ ZROBIMY ZACIAGNIECIE TYCH ENERGII Z PLIKU ZEBY BYLY TAKIE SAME ZA KAZDYM ODPALENIEM)
    for(int i = 0; i < 10; i++){
        guard.numer = i;
        guard.stamina = rand() % 10 + 1;
        guards.push_back(guard);
    }



    //  Wczytujemy koordynaty z pliku do struktury pointow
    for(int i = 0; i < how_many_pts; i++){
        int x, y;
        odczyt >> x >> y;
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

    odczyt.close();

    // Przepisujemy jasnosc z vectora do tablicy, bo tak dziala nasz algorytm wyznaczania drogi (MOZE POZNIEJ ZROBIMY ZEBY OD RAZU NA VECTORZE DZIALALO)
    for(int i = 0; i <= how_many_pts; i++){
        hull[i] = pts[i].brightness;

    }
    hull[how_many_pts -1] = hull[0]; // dodajemy sztucznie ostatni point taki sam jak first aby zaczal i konczyl w tym samym miejscu (na potrzeby algorytmu)

    wybierz_najsilniejszych(guards);
    int pts_indexes[100];
    for(int i = 0; i <= how_many_pts; i++){
        pts_indexes[i]= 0;
    }
    for(int i = 0; i < 7; i++){
        cout << "<--------------------------------->\n";
        cout << "guard numer " << guards[i].numer << ", posiada " << guards[i].stamina << " energii\n";
        wyznacz_droge(guards[i].stamina ,hull, pts);
        cout << endl;
    }

    cout << "guards obeszli caly plot, nie znaleziono zadnych zagrozen z innych wymiarow :)\n";
    return 0;
}