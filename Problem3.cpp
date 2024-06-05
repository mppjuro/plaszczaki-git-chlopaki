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

void Najsilniejsi_straznicy(vector<Plaszczaki> straznicy){
    ///sortujemy wzgledem energii
    sort(straznicy.begin(), straznicy.end(), [](const Plaszczaki &a, const Plaszczaki &b) {
        return a.energia > b.energia;
    });
    for(int i = 0; i<=7; i++){
        cout << straznicy[i].numer << " " << straznicy[i].energia << endl;
    }
}

int main(){
///generator liczb losowych
srand(time(nullptr));

///kontener na straznikow
vector<Plaszczaki> straznicy;

///inicjalizujemy straznika
Plaszczaki straznik;

///losujemy energie straznikom
for(int i = 0; i < 10; i++) {
        straznik.numer = i;
        straznik.energia = rand() % 5 + 1;
        straznik.predkosc = rand() % 10 + 1;
        straznicy.push_back(straznik);
    }


///wypisywanie straznikow i ich mocy
/*for(int i = 0; i< 10; i++){
    cout << "Numer straznika: " <<straznicy[i].numer << " Energia: " << straznicy[i].energia << " Predkosc:" << straznicy[i].predkosc << endl;
} */



return 0;
}
