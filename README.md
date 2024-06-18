# plaszczaki-git-chlopaki
## Wymagania
- CMake min. wersja 3.10
- Kompilator C++ obsługujący C++17
## [Dokumentacja](/Dokumentacja.pdf)

## Instrukcja obsługi
1. Klonowanie repozytorium:
```console
git clone https://github.com/mppjuro/plaszczaki-git-chlopaki.git
cd plaszczaki-git-chlopaki
```
2. Kompilacja skryptem
```console
./build.sh
```
3. Uruchomienie programu z głównego katalogu
```console
./plaszczaki (ewentualnie plaszczaki.exe na windowsie)
```
Możliwa też jest ręczna kompilacja:
```console
cd src
g++ main.cpp helpers.cpp huffman.cpp -lSDL2 -lSDL2_ttf
```