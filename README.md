# plaszczaki-git-chlopaki

DEADLINE 15 czerwca, do tej daty należy zrobić wszystko łącznie z dokumentacją, prezentacją z opisem jak rozumiemy poszczególne problemy oraz screenami z testów w prezentacji, przetestować przypadki brzegowe, np. wszystkie płaszczaki z rękami z przodu i żadnego z rękami z tyłu, odcinki otoczki wypukłej idealnie pionowe lub poziome

generator danych do problemów 1 i 2 zrobiony - Miłosz

problem nr 1 rozwiązany z wykorzystaniem bibliotek graficznych oraz algorytmu Grahama do obliczenia otoczki wypukłej - Miłosz

problem nr 2 rozwiązany - jest zamiana poli na boli itp. wg słownika, jest gotowe kodowanie, kompresja, dekompresja i dekodowanie, dodałem dynamiczną długość bitową liczb oznaczających numery słów ze słownika, zwiększyło to kompresję o 5% (obecna średnia efektywność 38%, z takim configiem jak w przykładzie, z innymi ustawieniami może się to mocno różnić, ale w każdym przypadku poprawia to stopień kompresji), dodałem drugi algorytm kompresji - Huffmana (średnia efektywność 16%) - Miłosz

Jest fajniejszy makefile  
Oba problemy juz dzialaja ale tylko na Linuxie

kompilacja wszystkich programow:
```console
make
```
czyszczenie plikow:
```console
make clean
```
obv musicie miec zainstalowanego make'a

binarki odpalajcie z katalogu glownego np:
```console
bin/generator
```
-Filip 



problem nr 3 rozwiazany - uzytko algorytmu zachlannego, ktory wyszukuje najwieksza mozliwą wartość do przejścia, tak aby zminimalizować przystanki, na których strażnik musi czekać.

DO ZROBIENIA:
- poprawa komentarzy - Kamil
- poprawa/optymalizacja kodu, dodanie #define i innych skracających kod instrukcji, przyspieszenie kodu (np. instrukcjami z C++20 lub C++23) - Kamil/Miłosz
- Zamiana problemu 3 na jezyk angielski i dodanie wczytywania energii straznikow z pliku
- dokumentacja - ?
- opis poprawności rozwiązania - ?
- raporty z przeprowadzonych testów, szczególnie przypadków brzegowych - ?
- dopracować kod, usunąć zbędne fragmenty - Kamil
- OPCJONALNIE (jeśli zdążymy) popracować nad przyspieszeniem działania, np. z użyciem makr z assemblera i innych "ciekawostek" - Kamil/Miłosz

Dopiszcie pod odpowiednimi punktami co kto będzie robić
