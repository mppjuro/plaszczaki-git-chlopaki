#include <iostream>
#include <SDL2/SDL.h>
#include "generator.cpp"
//#include "problem1.cpp"
#include "problem2.cpp"
#include "problem3.cpp"

int WinMain(int argc, char **argv) {
    char selection = -1;

    std::cout << "Witaj w świecie płaszczaków!" << std::endl;
    do {
        std::cout << "Co chcesz zrobić?" << std::endl;
        std::cout << "0. Wygeneruj przykładowe dane" << std::endl;
        std::cout << "1. Uruchom problem 1" << std::endl;
        std::cout << "2. Uruchom problem 2" << std::endl;
        std::cout << "3. Uruchom problem 3" << std::endl;
        std::cout << "4. Zakończ program" << std::endl;

        std::cin >> selection;

        switch (selection) {
            case '0':
                generator();
                break;
            case '1':
                //problem1();
                break;
            case '2':
                problem2();
                break;
            case '3':
                problem3();
                break;
            default:
                break;
        }
    } while (selection != '4');

    return 0;
}