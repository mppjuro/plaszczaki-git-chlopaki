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
#include <map>
#include <cctype>
#include <sstream>

using namespace std;

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    if (first == std::string::npos || last == std::string::npos) {
        return "";
    }
    return str.substr(first, (last - first + 1));
}

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
	std::map<std::string, int> config;
    std::ifstream config_file("config.txt");
    if (!config_file.is_open()) {
        std::cerr << "Nie można otworzyć pliku konfiguracyjnego." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(config_file, line)) {
        size_t pos = line.find("=");
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            int value = std::stoi(trim(line.substr(pos + 1)));
            config[key] = value;
        }
    }
    config_file.close();
    
	const int n = config["MIN_WORD_QUANTITY"]; //n wystapien
	const int m = config["MIN_WORD_LENGTH"]; //min dlugosc
	const int k = config["DICT_SIZE"]; //max ilosc w slowniku
	
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
	//cout<< content<<endl;
    unordered_map<string, int> substringCount;
    vector<string> words;
    size_t start = 0;
    while (start < content.size()) {
        size_t end = content.find(' ', start);
        if (end == string::npos) {
            end = content.length();
        }
        string word = content.substr(start, end - start);
        size_t newlinePos = word.find('\n');
        while (newlinePos != string::npos) {
            word.replace(newlinePos, 1, "@");
            newlinePos = word.find('\n', newlinePos + 1);
        }
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
    int counter = 1;

    for (auto& pair : substringCount) {
        if (pair.second > n && counter <= k) {
            dictionary[pair.first] = counter;
            dictionaryVector.push_back(pair.first);
            replacementPairs.emplace_back(pair.first, to_string(counter));
            counter++;
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
    unordered_map<string, string> newMapping;
    int newCounter = 1;
    for (const auto& seq : usedSubstrings) {
        finalDictionary.push_back(seq);
        newMapping[seq] = to_string(newCounter++);
    }

    for (auto& pair : replacementPairs) {
        if (newMapping.find(pair.first) != newMapping.end()) {
            pair.second = newMapping[pair.first];
        }
    }

    transformedText = content; // Resetting the transformedText to original
    for (auto& pair : replacementPairs) {
        size_t pos = transformedText.find(pair.first);
        while (pos != string::npos) {
            transformedText.replace(pos, pair.first.length(), pair.second);
            pos = transformedText.find(pair.first, pos + pair.second.length());
        }
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
        {'y', "11000"}, {'z', "11001"}, {'@', "11010"}, {'#', "11011"}, // @ to nowa linia, # to nowa linia wewnątrz słownika
        {',', "11100"}, {'.', "11101"}, {' ', "11110"}
    };
    
    std::ifstream inputFile("text.txt", std::ios::in);
    std::ofstream outputFile("kodowany.txt", std::ios::out);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        cerr << "Error opening file for input or output." << endl;
        return 1;
    }

    string transformedLine;
    while (getline(inputFile, transformedLine)) {
		transformedLine += "@";
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
    
    std::ifstream inputFileout("out.txt", std::ios::in);
    std::ofstream outputFilekodowanyout("kodowanyout.txt", std::ios::out);

    if (!inputFileout.is_open() || !outputFilekodowanyout.is_open()) {
        cerr << "Error opening file for input or output." << endl;
        return 1;
    }

    transformedLine;
    while (getline(inputFileout, transformedLine)) {
		transformedLine += "@";
        size_t index = 0;
        while (index < transformedLine.size()) {
            if (isdigit(transformedLine[index])) {
                size_t num_start = index;
                while (index < transformedLine.size() && isdigit(transformedLine[index])) {
                    index++;
                }
                int num = stoi(transformedLine.substr(num_start, index - num_start));
                string binary = std::bitset<10>(num).to_string();
                outputFilekodowanyout << "11111" << binary;
            } else {
                if (encoding.find(transformedLine[index]) != encoding.end()) {
                    outputFilekodowanyout << encoding[transformedLine[index]];
                } else {
                    cerr << "Unsupported character found: " << transformedLine[index] << endl;
                }
                index++;
            }
        }
    }

    inputFileout.close();
    outputFilekodowanyout.close();
    
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

    while (getline(inputKodowany, line)) {
        // Procesujemy ciąg bitów i zapisujemy każde 8 bitów jako jeden bajt
        for (size_t i = 0; i < line.size(); i += 8) {
            // Upewnij się, że masz wystarczającą liczbę bitów do przetworzenia
            string byteString = line.substr(i, 8);
            if (byteString.length() < 8) {
                byteString.append(8 - byteString.length(), '1'); // Dopełnij zerami, jeśli nie ma pełnego bajtu
            }
            // Konwersja ciągu binarnego na bajt
            char byte = static_cast<char>(std::bitset<8>(byteString).to_ulong());
            outputKodowany.write(&byte, sizeof(byte));
        }
    }

    inputKodowany.close();
    outputKodowany.close();
	/*
    std::ifstream inputFileKomp("out.txt", std::ios::in);
    std::ofstream outputFileKomp("kodowanykomp.txt", std::ios::out);

    if (!inputFileKomp.is_open() || !outputFileKomp.is_open()) {
        cerr << "Error opening file for input or output." << endl;
        return 1;
    }

    transformedLine;
    while (getline(inputFileKomp, transformedLine)) {
		transformedLine += "x";
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
    */
   ifstream inputKodowanyKomp("kodowanyout.txt");
    if (!inputKodowanyKomp.is_open()) {
        cerr << "Error opening file kodowanyout.txt" << endl;
        return 1;
    }

    ofstream outputKodowanyKomp("outputkomp.bin", ios::out | ios::binary);
    if (!outputKodowanyKomp.is_open()) {
        cerr << "Error opening file outputkomp.bin" << endl;
        return 1;
    }

    while (getline(inputKodowanyKomp, line)) {
        // Procesujemy ciąg bitów i zapisujemy każde 8 bitów jako jeden bajt
        for (size_t i = 0; i < line.size(); i += 8) {
            // Upewnij się, że masz wystarczającą liczbę bitów do przetworzenia
            string byteString = line.substr(i, 8);
            if (byteString.length() < 8) {
                byteString.append(8 - byteString.length(), '1'); // Dopełnij zerami, jeśli nie ma pełnego bajtu
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
    
ifstream binaryInput("outputkomp.bin", ios::binary);
ofstream textOutput("kodowanykomp2.txt");

if (!binaryInput.is_open()) {
    cerr << "Error opening outputkomp.bin for reading." << endl;
    return 1;
}

if (!textOutput.is_open()) {
    cerr << "Error opening kodowanykomp2.txt for writing." << endl;
    return 1;
}

char byte;
while (binaryInput.read(&byte, sizeof(byte))) {
    textOutput << bitset<8>(byte);
}

binaryInput.close();
textOutput.close();


    // Mapa dekodowania
    unordered_map<string, char> decoding = {
        {"00000", 'a'}, {"00001", 'b'}, {"00010", 'c'}, {"00011", 'd'},
        {"00100", 'e'}, {"00101", 'f'}, {"00110", 'g'}, {"00111", 'h'},
        {"01000", 'i'}, {"01001", 'j'}, {"01010", 'k'}, {"01011", 'l'},
        {"01100", 'm'}, {"01101", 'n'}, {"01110", 'o'}, {"01111", 'p'},
        {"10000", 'q'}, {"10001", 'r'}, {"10010", 's'}, {"10011", 't'},
        {"10100", 'u'}, {"10101", 'v'}, {"10110", 'w'}, {"10111", 'x'},
        {"11000", 'y'}, {"11001", 'z'}, {"11010", '@'}, {"11011", '#'}, // @ to nowa linia
        {"11100", ','}, {"11101", '.'}, {"11110", ' '}
    };

    ifstream input("kodowanykomp2.txt", ios::binary);
    ofstream output("out2.txt");
    if (!input.is_open() || !output.is_open()) {
        cerr << "Error opening files." << endl;
        return 1;
    }

    stringstream buffer;
    buffer << input.rdbuf();
    string bits = buffer.str();
    input.close();

    // Decode the dictionary size
    string numberBits = bits.substr(5, 10);
    bitset<10> bitsetNumber(numberBits);
    int dictionarySize = static_cast<int>(bitsetNumber.to_ulong());
    output << dictionarySize;

    // Decode words from the dictionary
    size_t pos = 15;
    string decodedWord;
    for (int i = 0; pos + 5 <= bits.size() && i < dictionarySize; ++i) {
        string bitString = bits.substr(pos, 5);
        pos += 5;
        if (decoding[bitString] == '@') {
            output << decodedWord << endl;
            decodedWord.clear();
        } else {
            decodedWord += decoding[bitString];
        }
    }

    // Decode the remaining part of the file
    while (pos + 5 <= bits.size()) {
        string decodedText;
        while (pos + 5 <= bits.size() && bits.substr(pos, 5) != "11111") {
            string bitString = bits.substr(pos, 5);
            if (decoding.find(bitString) != decoding.end()) {
                if (decoding[bitString] == '@') {
                    output << decodedText << endl;
                    decodedText.clear();
                } else {
                    decodedText += decoding[bitString];
                }
            }
            pos += 5;
        }
        // Handle numeric values after special marker "11111"
        if (pos + 15 <= bits.size() && bits.substr(pos, 5) == "11111") {
            pos += 5; // skip the marker
            string numberBits = bits.substr(pos, 10);
            bitset<10> bitsetNumber(numberBits);
            int number = static_cast<int>(bitsetNumber.to_ulong());
            decodedText += to_string(number);
            pos += 10;
        }
        if (pos + 5 <= bits.size() && pos + 15 > bits.size()) {
			break;
		}
        output << decodedText;
    }

    output.close();	
	////////////////////////////////////////////////////////////////////////////////////////////
	ifstream inFileOut2("out2.txt");
    if (!inFileOut2.is_open()) {
        cerr << "Nie można otworzyć pliku 'out2.txt'." << endl;
        return 1;
    }

    // Wczytanie pierwszej linii, która zawiera ilość rekordów w słowniku
    getline(inFileOut2, line);
    stringstream(line) >> dictionarySize;

    // Wczytanie rekordów słownika
    vector<string> dictionarydecode(dictionarySize);
    for (int i = 0; i < dictionarySize; i++) {
        getline(inFileOut2, line);
        dictionarydecode[i] = line;
    }

    // Przetworzenie reszty pliku, zamieniając indeksy na tekst z słownika
    ofstream outFileText2("text2.txt");
    if (!outFileText2.is_open()) {
        cerr << "Nie można otworzyć pliku 'text2.txt' do zapisu." << endl;
        return 1;
    }
    getline(inFileOut2, line);
    while (true) {
        stringstream ss(line);
        int first = 0;
        string word;
        if (!(ss >> word)) {
			break;
		}
        do {
			if (first) {
				outFileText2 << " ";
			}
			first++;
            try {
                int index = stoi(word);
                if (index > 0 && index <= dictionarySize) {
                    outFileText2 << dictionarydecode[index - 1];
                } else {
                    cerr << "Indeks poza zakresem słownika: " << index << endl;
                }
            } catch (invalid_argument&) {
                outFileText2 << word;
            }
        } while (ss >> word);
        if (getline(inFileOut2, line)) {
			outFileText2 << "\n";
		}
		else {
			break;
		}
    }

    inFileOut2.close();
    outFileText2.close();

    cout << "Przetwarzanie zakończone." << endl;
    

    return 0;
}
