#include <iostream>

#include "generator.cpp"
#include "problem1.cpp"
#include "problem2.cpp"
#include "problem3.cpp"

int main(int argc, char **argv) {
    char selection = -1;

    std::cout << "Welcome to the world of flatlanders!" << std::endl;
    do {
        std::cout << "What will you do today?" << std::endl;
        std::cout << "0. Generate random data" << std::endl;
        std::cout << "1. Solve problem 1" << std::endl;
        std::cout << "2. Solve problem 2" << std::endl;
        std::cout << "3. Solve problem 3" << std::endl;
        std::cout << "4. Exit program" << std::endl;

        std::cin >> selection;

        switch (selection) {
            case '0':
                generator();
                break;
            case '1':
                problem1();
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