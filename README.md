# plaszczaki-git-chlopaki

DEADLINE 15 czerwca, do tej daty należy zrobić wszystko łącznie z dokumentacją, prezentacją z opisem jak rozumiemy poszczególne problemy oraz screenami z testów w prezentacji, przetestować przypadki brzegowe, np. wszystkie płaszczaki z rękami z przodu i żadnego z rękami z tyłu, odcinki otoczki wypukłej idealnie pionowe lub poziome

generator danych do problemów 1 i 2 zrobiony - Miłosz

problem nr 1 rozwiązany z wykorzystaniem bibliotek graficznych oraz algorytmu Grahama do obliczenia otoczki wypukłej - Miłosz

problem nr 2 rozwiązany - jest zamiana poli na boli itp. wg słownika, jest gotowe kodowanie, kompresja, dekompresja i dekodowanie, dodałem dynamiczną długość bitową liczb oznaczających numery słów ze słownika, zwiększyło to kompresję o 5% (obecna średnia efektywność 38%, z takim configiem jak w przykładzie, z innymi ustawieniami może się to mocno różnić, ale w każdym przypadku poprawia to stopień kompresji), dodałem drugi algorytm kompresji - Huffmana (średnia efektywność 16%) - Miłosz

Dodałem prymitywny makefile oraz zmieniłem trochę strukture plików - jest trochę czyściej imo
dodałem też makra do filepathow w generatorze i problemie2 - łatwiej się teraz zmienia, idealnie powinna być w configu
problemu1 jeszcze nie tykałem przez SDLa na linuxie - inaczej się go obsługuje - do ogarnięcia

kompilacja wszystkich programow:
```console
make
```
kompilacja i/lub uruchomienie poszczególnych programów (generator sie kompiluje przy kazdym jesli jest potrzeba):
```console
make prob1 # jeszcze nie dziala
make prob2
```
czyszczenie plikow:
```console
make clean
```
obv musicie miec zainstalowanego make'a
-Filip 

DO ZROBIENIA:
- poprawa komentarzy - Kamil
- poprawa/optymalizacja kodu, dodanie #define i innych skracających kod instrukcji, przyspieszenie kodu (np. instrukcjami z C++20 lub C++23) - Kamil/Miłosz
- problem nr 3 - Mateusz
- dokumentacja - ?
- opis poprawności rozwiązania - ?
- raporty z przeprowadzonych testów, szczególnie przypadków brzegowych - ?
- dopracować kod, usunąć zbędne fragmenty - Kamil
- OPCJONALNIE (jeśli zdążymy) popracować nad przyspieszeniem działania, np. z użyciem makr z assemblera i innych "ciekawostek" - Kamil/Miłosz

Dopiszcie pod odpowiednimi punktami co kto będzie robić
