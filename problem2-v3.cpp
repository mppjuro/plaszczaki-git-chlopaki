// metoda kompresji - własna wariacja algorytmu LZ78 - słownikowej bezstratnej kompresji danych
// polecenie budowania: g++ -std=c++17 -o "%e" "%f"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <bitset>

using namespace std;

const int n = 3; //n wystapien
const int m = 4; //min dlugosc
const int k = 1000; //max ilosc w slowniku

void printFileContent(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Nie można otworzyc pliku do odczytu: " << filename << std::endl;
        return;
    }

    std::cout << "Zawartosc pliku " << filename << ":" << std::endl;
    
    // Wyświetlanie pierwszych 8 bajtów w formacie binarnym
    char c;
    int count = 0;
    std::cout << "Pierwsze 8 bajtow w formacie binarnym: ";
    while (file.get(c)) {
        std::cout << std::bitset<8>(static_cast<unsigned char>(c)) << " ";
        count++;
    }
    std::cout << std::endl;
    std::cout << std::endl << "Koniec pliku." << std::endl;
    file.close();
}

void saveDictionaryAndTransformedText(const string& filename, const vector<string>& dictionary, const string& text) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "Error opening file " << filename << endl;
        return;
    }
    outFile << dictionary.size() << endl;
    for (const string& seq : dictionary) {
        outFile << seq << endl;
    }
    outFile << text;
    outFile.close();
}

int main() {
    ifstream inFile("text.txt");
    if (!inFile.is_open()) {
        cerr << "Error opening file text.txt" << endl;
        return 1;
    }

    string content((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    if (content.empty() || content.size() < m || n < 1) {
        cerr << "Invalid content size or parameters." << endl;
        return 1;
    }

    unordered_map<string, int> substringCount;
    vector<string> words;
    size_t start = 0;
    while (start < content.size()) {
        size_t end = content.find(' ', start);
        if (end == string::npos) {
            end = content.length();
        }
        string word = content.substr(start, end - start);
        if (word.length() >= m) {
            words.push_back(word);
        }
        start = end + 1;
    }

    for (const auto& word : words) {
        substringCount[word]++;
    }

    vector<pair<string, string>> replacementPairs;
    unordered_map<string, int> dictionary;
    unordered_set<string> usedSubstrings;
    vector<string> dictionaryVector;
    int counter = 0;

    for (auto& pair : substringCount) {
        if (pair.second > n && counter < k) {
            dictionary[pair.first] = counter;
            dictionaryVector.push_back(pair.first);
            counter++;
            replacementPairs.emplace_back(pair.first, to_string(counter));
        }
    }

    sort(replacementPairs.begin(), replacementPairs.end(), [](const pair<string, string>& a, const pair<string, string>& b) {
        return a.first.size() > b.first.size();
    });

    string transformedText = content;
    for (auto& pair : replacementPairs) {
        size_t pos = transformedText.find(pair.first);
        while (pos != string::npos) {
            transformedText.replace(pos, pair.first.length(), pair.second);
            usedSubstrings.insert(pair.first);
            pos = transformedText.find(pair.first, pos + pair.second.length());
        }
    }

    vector<string> finalDictionary;
    for (const auto& seq : usedSubstrings) {
        finalDictionary.push_back(seq);
    }

    saveDictionaryAndTransformedText("out.txt", finalDictionary, transformedText);

    // Etap 1: Kodowanie tekstu do formatu binarnego
    std::unordered_map<char, std::string> encoding = {
        {'a', "00000"}, {'b', "00001"}, {'c', "00010"}, {'d', "00011"},
        {'e', "00100"}, {'f', "00101"}, {'g', "00110"}, {'h', "00111"},
        {'i', "01000"}, {'j', "01001"}, {'k', "01010"}, {'l', "01011"},
        {'m', "01100"}, {'n', "01101"}, {'o', "01110"}, {'p', "01111"},
        {'q', "10000"}, {'r', "10001"}, {'s', "10010"}, {'t', "10011"},
        {'u', "10100"}, {'v', "10101"}, {'w', "10110"}, {'x', "10111"},
        {'y', "11000"}, {'z', "11001"}, {'\n', "11010"}, {' ', "11011"},
        {',', "11100"}, {'.', "11101"}, {'"', "11110"}
    };
    
    std::ifstream inputFile("text.txt", std::ios::in);
    std::ofstream outputFile("kodowany.txt", std::ios::out);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        cerr << "Error opening file for input or output." << endl;
        return 1;
    }

    string transformedLine;
    while (getline(inputFile, transformedLine)) {
        size_t index = 0;
        while (index < transformedLine.size()) {
            if (isdigit(transformedLine[index])) {
                size_t num_start = index;
                while (index < transformedLine.size() && isdigit(transformedLine[index])) {
                    index++;
                }
                int num = stoi(transformedLine.substr(num_start, index - num_start));
                string binary = std::bitset<10>(num).to_string();
                outputFile << "11111" << binary;
            } else {
                if (encoding.find(transformedLine[index]) != encoding.end()) {
                    outputFile << encoding[transformedLine[index]];
                } else {
                    cerr << "Unsupported character found: " << transformedLine[index] << endl;
                }
                index++;
            }
        }
    }

    inputFile.close();
    outputFile.close();
    
   ifstream inputKodowany("kodowany.txt");
    if (!inputKodowany.is_open()) {
        cerr << "Error opening file kodowanykomp.txt" << endl;
        return 1;
    }

    ofstream outputKodowany("output.bin", ios::out | ios::binary);
    if (!outputKodowany.is_open()) {
        cerr << "Error opening file output.bin" << endl;
        return 1;
    }

    string line;
    while (getline(inputKodowany, line)) {
        // Procesujemy ciąg bitów i zapisujemy każde 8 bitów jako jeden bajt
        for (size_t i = 0; i < line.size(); i += 8) {
            // Upewnij się, że masz wystarczającą liczbę bitów do przetworzenia
            string byteString = line.substr(i, 8);
            if (byteString.length() < 8) {
                byteString.append(8 - byteString.length(), '0'); // Dopełnij zerami, jeśli nie ma pełnego bajtu
            }
            // Konwersja ciągu binarnego na bajt
            char byte = static_cast<char>(std::bitset<8>(byteString).to_ulong());
            outputKodowany.write(&byte, sizeof(byte));
        }
    }

    inputKodowany.close();
    outputKodowany.close();

    std::ifstream inputFileKomp("out.txt", std::ios::in);
    std::ofstream outputFileKomp("kodowanykomp.txt", std::ios::out);

    if (!inputFileKomp.is_open() || !outputFileKomp.is_open()) {
        cerr << "Error opening file for input or output." << endl;
        return 1;
    }

    transformedLine;
    while (getline(inputFileKomp, transformedLine)) {
        size_t index = 0;
        while (index < transformedLine.size()) {
            if (isdigit(transformedLine[index])) {
                size_t num_start = index;
                while (index < transformedLine.size() && isdigit(transformedLine[index])) {
                    index++;
                }
                int num = stoi(transformedLine.substr(num_start, index - num_start));
                string binary = std::bitset<10>(num).to_string();
                outputFileKomp << "11111" << binary;
            } else {
                if (encoding.find(transformedLine[index]) != encoding.end()) {
                    outputFileKomp << encoding[transformedLine[index]];
                } else {
                    cerr << "Unsupported character found: " << transformedLine[index] << endl;
                }
                index++;
            }
        }
    }

    inputFileKomp.close();
    outputFileKomp.close();
    
   ifstream inputKodowanyKomp("kodowanykomp.txt");
    if (!inputKodowanyKomp.is_open()) {
        cerr << "Error opening file kodowanykomp.txt" << endl;
        return 1;
    }

    ofstream outputKodowanyKomp("outputkomp.bin", ios::out | ios::binary);
    if (!outputKodowanyKomp.is_open()) {
        cerr << "Error opening file output.bin" << endl;
        return 1;
    }

    while (getline(inputKodowanyKomp, line)) {
        // Procesujemy ciąg bitów i zapisujemy każde 8 bitów jako jeden bajt
        for (size_t i = 0; i < line.size(); i += 8) {
            // Upewnij się, że masz wystarczającą liczbę bitów do przetworzenia
            string byteString = line.substr(i, 8);
            if (byteString.length() < 8) {
                byteString.append(8 - byteString.length(), '0'); // Dopełnij zerami, jeśli nie ma pełnego bajtu
            }
            // Konwersja ciągu binarnego na bajt
            char byte = static_cast<char>(std::bitset<8>(byteString).to_ulong());
            outputKodowanyKomp.write(&byte, sizeof(byte));
        }
    }

    inputKodowanyKomp.close();
    outputKodowanyKomp.close();

    cout << "Conversion completed successfully." << endl;
    
    // Otwieranie plików do odczytu, aby sprawdzić ich rozmiar
    ifstream inputKodowanyTest("output.bin", ios::binary | ios::ate);
    ifstream inputKodowanyKompTest("outputkomp.bin", ios::binary | ios::ate);

    if (!inputKodowanyTest.is_open() || !inputKodowanyKompTest.is_open()) {
        cerr << "Error opening binary files to read size." << endl;
        return 1;
    }

	int a = inputKodowanyTest.tellg();
	int b = inputKodowanyKompTest.tellg();
    // Pobieranie i wyświetlanie rozmiaru plików
    cout << "Rozmiar pliku output.bin: " << a << " bajtow" << endl;
    cout << "Rozmiar pliku outputkomp.bin: " << b << " bajtow, czyli jest mniejszy o " << 1.0*(a-b)/a * 100 << "%" << endl;

    inputKodowany.close();
    inputKodowanyKomp.close();    

    return 0;
}
