#include <bits/stdc++.h>
using namespace std;

struct Plaszczaki{
    int numer;
    int energia;
    int predkosc;
};

struct Punkty{
    int swiatlosc;
    int x;
    int y;
};

void wybierz_najsilniejszych(vector<Plaszczaki> &straznicy){
    ///sortujemy wzgledem energii
    sort(straznicy.begin(), straznicy.end(), [](const Plaszczaki &a, const Plaszczaki &b) {
        return a.energia > b.energia;
    });
    /*for(int i = 0; i < 7; i++){
        cout << straznicy[i].numer << " " << straznicy[i].energia << endl;
    } */
}



int main(){
///generator liczb losowych
srand(time(nullptr));

///kontenery na straznikow i punkty
vector<Plaszczaki> straznicy;
vector<Punkty> punkty;

Plaszczaki straznik;
Punkty punkt;

///losujemy energie straznikom
for(int i = 0; i < 100; i++) {
        straznik.numer = i;
        straznik.energia = rand() % 100 + 1;
        straznik.predkosc = rand() % 10 + 1;
        straznicy.push_back(straznik);
    }

///losujemy koordynaty i swiatlosc punktow
for(int i = 0; i < 30; i++) {
        punkt.x = rand() % 50 + 0;
        punkt.y = rand() % 50 + 0;
        punkt.swiatlosc = rand() % 100 + 1;
        punkty.push_back(punkt);
    }



/*wybierz_najsilniejszych(straznicy);
    for(int i = 0; i < 7; i++){
        cout << "Numer: " << straznicy[i].numer << " Energia: " << straznicy[i].energia << endl;
    }
*/

///wyswietlanie punktow
for(int i = 0; i < 30; i++){
    cout << "x: "  <<punkty[i].x << " y: "<< punkty[i].y << " swiatlosc: "<<punkty[i].swiatlosc << endl;
}


return 0;
}
