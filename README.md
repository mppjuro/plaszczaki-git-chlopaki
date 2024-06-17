# plaszczaki-git-chlopaki
## Wymagania
- CMake min. wersja 3.10
- Kompilator C++ obsługujący C++17
## Instrukcja obsługi
(polecam użyć skryptu build.sh)
1. Klonowanie repozytorium:
```console
git clone https://github.com/mppjuro/plaszczaki-git-chlopaki.git
cd plaszczaki-git-chlopaki
```
2. Utworzenie katalogu 'build'
```console
mkdir build
cd build
```
3. Uruchomienie CMake i kompilacja
```console
cmake ..
cmake --build .
```
4. Uruchomienie programu z głównego katalogu
```console
cd ..
./plaszczaki (ewentualnie plaszczaki.exe na windowsie)
```
Możliwa też jest ręczna kompilacja:
```console
cd src
g++ main.cpp helpers.cpp huffman.cpp -lSDL2 -lSDL2_ttf
```

DEADLINE 15 czerwca, do tej daty należy zrobić wszystko łącznie z dokumentacją, prezentacją z opisem jak rozumiemy poszczególne problemy oraz screenami z testów w prezentacji, przetestować przypadki brzegowe, np. wszystkie płaszczaki z rękami z przodu i żadnego z rękami z tyłu, odcinki otoczki wypukłej idealnie pionowe lub poziome

generator danych do problemów 1 i 2 zrobiony - Miłosz

problem nr 1 rozwiązany z wykorzystaniem bibliotek graficznych oraz algorytmu Grahama do obliczenia otoczki wypukłej - Miłosz

problem nr 2 rozwiązany - jest zamiana poli na boli itp. wg słownika, jest gotowe kodowanie, kompresja, dekompresja i dekodowanie, dodałem dynamiczną długość bitową liczb oznaczających numery słów ze słownika, zwiększyło to kompresję o 5% (obecna średnia efektywność 38%, z takim configiem jak w przykładzie, z innymi ustawieniami może się to mocno różnić, ale w każdym przypadku poprawia to stopień kompresji), dodałem drugi algorytm kompresji - Huffmana (średnia efektywność 16%) - Miłosz

problem nr 3 rozwiazany - uzytko algorytmu zachlannego, ktory wyszukuje najwieksza mozliwą wartość do przejścia, tak aby zminimalizować przystanki, na których strażnik musi czekać.

DO ZROBIENIA:
- poprawa komentarzy - Kamil
- poprawa/optymalizacja kodu, dodanie #define i innych skracających kod instrukcji, przyspieszenie kodu (np. instrukcjami z C++20 lub C++23) - Kamil/Miłosz
- Zamiana problemu 3 na jezyk angielski i dodanie wczytywania energii straznikow z pliku
- dokumentacja - ?
- opis poprawności rozwiązania - ?
- raporty z przeprowadzonych testów, szczególnie przypadków brzegowych - ?
- dopracować kod, usunąć zbędne fragmenty - Kamil
- zrobic prezentacje - ?
- OPCJONALNIE (jeśli zdążymy) popracować nad przyspieszeniem działania, np. z użyciem makr z assemblera i innych "ciekawostek" - Kamil/Miłosz
- wypełnić ankietę na moodle 1 dzień PRZED prezentacją, czyli do 18 czerwca - KAŻDY

Dopiszcie pod odpowiednimi punktami co kto będzie robić
