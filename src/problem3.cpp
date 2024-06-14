#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

struct Plaszczaki {
    int numer;
    int energia;
};

struct Punkty {
    int swiatlosc;
    int x;
    int y;
};

/// Wybor straznikow z najwieksza iloscia energii do obchodzenia plotu
void wybierz_najsilniejszych(vector<Plaszczaki> &straznicy) {
    sort(straznicy.begin(), straznicy.end(), [](const Plaszczaki &a, const Plaszczaki &b) {
        return a.energia > b.energia;
    });
}

/// Wyznaczamy koordynaty przejscia straznika przez plot
void wyznacz_droge(int rozmiar, int tab[], vector<Punkty> &punkty) {
    int skok = 3;
    int ile_przystankow = 0;
    int aktualny = tab[0];
    vector<pair<int, int>> wektor;
    vector<pair<int, int>> przystanki;

    int i = 0;
    while (i < rozmiar - 1) {
        wektor.push_back({aktualny, i});  /// wrzucamy pozycje startowa
        int max_val = -1;
        int max_idx = -1;

        /// przeszukujemy skok- elementow do przodu
        for (int j = 1; j <= skok && i + j < rozmiar; ++j) {
            if (tab[i + j] <= aktualny && tab[i + j] > max_val) {
                max_val = tab[i + j];
                max_idx = i + j;
            }
        }

        if (max_idx != -1) {
            /// znaleziono maxa
            aktualny = max_val;
            i = max_idx;
        } else {
            /// nie znaleziono maxa
            int next_stop = min(i + skok, rozmiar - 1);
            przystanki.push_back({tab[next_stop], next_stop});
            aktualny = tab[next_stop];
            i = next_stop;
        }
    }
    /// Dodanie ostatniego elementu do wektoraa
    wektor.push_back({tab[rozmiar - 1], rozmiar - 1});

    cout << "Straznik ten musi przejsc przez nastepujace punkty: \n\n";
    for (const auto &p : wektor) {
        cout << "x: " << punkty[p.second].x << ", y: " << punkty[p.second].y << ", swiatlosc punktu: " << p.first
             << endl;
    }
    cout << endl;

    cout << "Niestety zatrzymac musi sie na przystankach: \n";
    for (const auto &p : przystanki) {
        cout << "x: " << punkty[p.second].x << ", y: " << punkty[p.second].y << ", swiatlosc punktu: " << p.first
             << endl;
        ile_przystankow++;
    }
    cout << endl;
    cout << "Straznik musial zatrzymac sie na " << ile_przystankow << " przystankach\n";
}

int problem3() {
    srand(time(nullptr));

    vector<Plaszczaki> straznicy;
    vector<Punkty> punkty;

    Plaszczaki straznik;
    Punkty punkt;

    int ile_wczytywanych_punktow;
    int pierwszy_x;
    int pierwszy_y;
    int pierwsza_swiatlosc;
    int otoczka[100];

    ifstream odczyt;
    odczyt.open("punkty_otoczka_wypukla.txt");
    if (!odczyt) {
        cerr << "Nie można otworzyć pliku do odczytu" << endl;
        return 1;
    }
    odczyt >> ile_wczytywanych_punktow;

    /// losujemy energie straznikom (TU MOZE TEZ ZROBIMY ZACIAGNIECIE TYCH ENERGII Z PLIKU ZEBY BYLY TAKIE SAME ZA KAZDYM ODPALENIEM)
    for (int i = 0; i < 10; i++) {
        straznik.numer = i;
        straznik.energia = rand() % 10 + 1;
        straznicy.push_back(straznik);
    }

    ///  Wczytujemy koordynaty z pliku do struktury punktow
    for (int i = 0; i < ile_wczytywanych_punktow; i++) {
        int x, y;
        odczyt >> x >> y;
        punkt.x = x;
        punkt.y = y;
        punkt.swiatlosc = rand() % 100 + 1;
        if (i == 0) {
            pierwszy_x = x;
            pierwszy_y = y;
            pierwsza_swiatlosc = punkt.swiatlosc;
        }
        punkty.push_back(punkt);
    }
    punkt.x = pierwszy_x;
    punkt.y = pierwszy_y;
    punkt.swiatlosc = pierwsza_swiatlosc;
    punkty.push_back(punkt);

    odczyt.close();

    /// Przepisujemy jasnosc z vectora do tablicy, bo tak dziala nasz algorytm wyznaczania drogi (MOZE POZNIEJ ZROBIMY ZEBY OD RAZU NA VECTORZE DZIALALO)
    for (int i = 0; i <= ile_wczytywanych_punktow; i++) {
        otoczka[i] = punkty[i].swiatlosc;
    }
    otoczka[ile_wczytywanych_punktow -
            1] = otoczka[0];  /// dodajemy sztucznie ostatni punkt taki sam jak pierwszy aby zaczal i konczyl w tym samym miejscu (na potrzeby algorytmu)

    wybierz_najsilniejszych(straznicy);
    int indeksy_punktow[100];
    for (int i = 0; i <= ile_wczytywanych_punktow; i++) {
        indeksy_punktow[i] = 0;
    }
    for (int i = 0; i < 7; i++) {
        cout << "<--------------------------------->\n";
        cout << "Straznik numer " << straznicy[i].numer << ", posiada " << straznicy[i].energia << " energii\n";
        wyznacz_droge(straznicy[i].energia, otoczka, punkty);
        cout << endl;
    }

    cout << "Straznicy obeszli caly plot, nie znaleziono zadnych zagrozen z innych wymiarow :)\n";
    return 0;
}