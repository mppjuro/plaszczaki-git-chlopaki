#include <algorithm>
#include <bitset>
#include <cctype>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "helpers.h"
#include "huffman.h"

#define cfg "config.txt"
#define dict_file "data/prob2/slownik.txt"
#define flawed_text "data/prob2/tekstzpoli.txt"
#define flawless_text "data/prob2/tekstbezpoli.txt"
#define huff_bin "data/prob2/huffman.bin"
#define output_txt "data/prob2/out.txt"
#define coded_txt "data/prob2/kodowany.txt"
#define bin_out "data/prob2/output.bin"
#define txt2 "data/prob2/text2.txt"
#define coded_out_txt "data/prob2/kodowanyout.txt"
#define out_comp_bin "data/prob2/outputkomp.bin"
#define coded_cmp2 "data/prob2/kodowanykomp2.txt"
#define txt3 "data/prob2/text3.txt"
#define output_2 "data/prob2/out2.txt"

using namespace std;

int iloscZamian = 0;
class Node {
   public:
    char ch;
    int freq;
    Node *left, *right;

    Node(char ch, int freq, Node* left = nullptr, Node* right = nullptr)
        : ch(ch), freq(freq), left(left), right(right) {}
};

struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

void printFileContent(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Nie można otworzyc pliku do odczytu: " << filename << '\n';
        return;
    }

    cout << "Zawartosc pliku " << filename << ":" << '\n';

    // Wyświetlanie pierwszych 8 bajtów w formacie binarnym
    char c;
    int count = 0;
    cout << "Pierwsze 8 bajtow w formacie binarnym: ";
    while (file.get(c)) {
        cout << bitset<8>(static_cast<unsigned char>(c)) << " ";
        count++;
    }
    cout << '\n';
    cout << '\n'
         << "Koniec pliku." << '\n';
    file.close();
}

void saveDictionaryAndTransformedText(const string& filename, const vector<string>& dictionary, const string& text) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "Error opening file " << filename << '\n';
        return;
    }
    outFile << dictionary.size() << '\n';
    for (const string& seq : dictionary) {
        outFile << seq << '\n';
    }
    outFile << text;
    outFile.close();
}

string find_word_to_swap(const string& slowo, const unordered_map<char, set<char>>& mirrored_letters_map, const set<string>& slownik) {  // Funkcja sprawdzająca, czy odwrócenie liter w słowie daje słowo ze słownika
    set<string> processed_words;
    processed_words.insert(slowo);
    queue<string> kolejka;
    kolejka.push(slowo);

    while (!kolejka.empty()) {
        string curr_word = kolejka.front();
        kolejka.pop();

        for (size_t i = 0; i < curr_word.size(); i++) {
            char orig = curr_word[i];
            if (mirrored_letters_map.find(orig) != mirrored_letters_map.end()) {
                for (char zamiennik : mirrored_letters_map.at(orig)) {
                    string new_word = curr_word;
                    new_word[i] = zamiennik;

                    if (slownik.find(new_word) != slownik.end() && new_word.size() == slowo.size()) {
                        cout << "Swapped \"" << slowo << "\" to \"" << new_word << "\"" << '\n';
                        iloscZamian++;
                        return new_word;
                    }

                    if (processed_words.find(new_word) == processed_words.end()) {
                        processed_words.insert(new_word);
                        kolejka.push(new_word);
                    }
                }
            }
        }
    }
    return slowo;  // Jeśli nie znajdzie zamiany, zwraca oryginalne słowo
}

int calculateBitsNeeded(int num) {
    return ceil(log2(num + 1));
}

int problem2() {
    ios::sync_with_stdio(false);
    srand(time(nullptr));
    map<string, int> config;
    ifstream config_file(cfg);
    if (!config_file.is_open()) {
        cerr << "Couldn't open the config file!\n";
        return 1;
    }

    string line;
    while (getline(config_file, line)) {
        size_t pos = line.find("=");
        if (pos != string::npos) {
            string key = trim(line.substr(0, pos));
            int value = stoi(trim(line.substr(pos + 1)));
            config[key] = value;
        }
    }
    config_file.close();

    const int n = config["MIN_WORD_QUANTITY"];  // n wystapien
    const int m = config["MIN_WORD_LENGTH"];    // min dlugosc
    const int k = config["DICT_SIZE"];          // max ilosc w slowniku

    // Wczytywanie słownika
    set<string> slownik;
    ifstream plikSlownik(dict_file);
    string slowo;
    while (plikSlownik >> slowo) {
        slownik.insert(slowo);
    }
    plikSlownik.close();

    // Tworzenie mapy odwróceń liter
    auto mirrored_letters_mapOdwracen = mirrored_letters();

    // Wczytywanie tekstu do przetworzenia
    ifstream plikTekst(flawed_text);
    ofstream plikWynik(flawless_text);
    string linia;
    while (getline(plikTekst, linia)) {
        stringstream ss(linia);
        string wyraz;
        while (ss >> wyraz) {
            string no_interpunction_word = rmv_interpunction(wyraz);
            string result_word = no_interpunction_word;

            if (slownik.find(no_interpunction_word) == slownik.end()) {
                string word_after_swap = find_word_to_swap(no_interpunction_word, mirrored_letters_mapOdwracen, slownik);
                if (!word_after_swap.empty() && word_after_swap.size() == no_interpunction_word.size()) {
                    result_word = word_after_swap;
                }
            }

            // Odtwarzanie interpunkcji w wynikowym słowie
            size_t j = 0;
            for (size_t i = 0; i < wyraz.size(); i++) {
                if (isalpha(wyraz[i])) {
                    wyraz[i] = result_word[j++];
                }
            }
            plikWynik << wyraz << " ";
        }
        plikWynik << '\n';
    }
    plikTekst.close();
    plikWynik.close();

    cout << "Processing complete. Swaps made: " << iloscZamian << ". Saved to tekstbezpoli.txt.\n";

    ifstream inFile(flawless_text);
    if (!inFile.is_open()) {
        cerr << "Error opening file tekstbezpoli.txt" << '\n';
        return 1;
    }

    string content((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    if (content.empty() || content.size() < m || n < 1) {
        cerr << "Invalid content size or parameters." << '\n';
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

    transformedText = content;  // Resetting the transformedText to original
    for (auto& pair : replacementPairs) {
        size_t pos = transformedText.find(pair.first);
        while (pos != string::npos) {
            transformedText.replace(pos, pair.first.length(), pair.second);
            pos = transformedText.find(pair.first, pos + pair.second.length());
        }
    }

    saveDictionaryAndTransformedText(output_txt, finalDictionary, transformedText);

    // Kompresja Huffmana
    unordered_map<char, string> huffmanCode;
    Node* root = buildHuffmanTree(content);
    generateCodes(root, "", huffmanCode);
    string encodedHuffman = encodeHuffman(content, huffmanCode);

    // Zapisz zakodowany tekst Huffmana do pliku binarnego
    ofstream huffmanFile(huff_bin, ios::binary);
    for (size_t i = 0; i < encodedHuffman.length(); i += 8) {
        bitset<8> b(encodedHuffman.substr(i, 8));
        char c = static_cast<char>(b.to_ulong());
        huffmanFile.write(&c, sizeof(char));
    }
    huffmanFile.close();

    // Etap 1: Kodowanie tekstu do formatu binarnego
    unordered_map<char, string> encoding = {
        {'a', "00000"}, {'b', "00001"}, {'c', "00010"}, {'d', "00011"}, {'e', "00100"}, {'f', "00101"}, {'g', "00110"}, {'h', "00111"}, {'i', "01000"}, {'j', "01001"}, {'k', "01010"}, {'l', "01011"}, {'m', "01100"}, {'n', "01101"}, {'o', "01110"}, {'p', "01111"}, {'q', "10000"}, {'r', "10001"}, {'s', "10010"}, {'t', "10011"}, {'u', "10100"}, {'v', "10101"}, {'w', "10110"}, {'x', "10111"}, {'y', "11000"}, {'z', "11001"}, {'@', "11010"}, {'#', "11011"},  // @ to nowa linia, # to nowa linia wewnątrz słownika
        {',', "11100"},
        {'.', "11101"},
        {' ', "11110"}};

    ifstream inputFile(flawless_text, ios::in);
    ofstream outputFile(coded_txt, ios::out);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        cerr << "Error opening file for input or output." << '\n';
        return 1;
    }

    string transformedLine;
    while (getline(inputFile, transformedLine)) {
        transformedLine += "@";
        size_t index = 0;
        while (index < transformedLine.size()) {
            if (encoding.find(transformedLine[index]) != encoding.end()) {
                outputFile << encoding[transformedLine[index]];
            } else {
                cerr << "Unsupported character found: " << transformedLine[index] << '\n';
            }
            index++;
        }
    }

    inputFile.close();
    outputFile.close();

    ifstream inputFileout(output_txt, ios::in);
    ofstream outputFilekodowanyout(coded_out_txt, ios::out);

    if (!inputFileout.is_open() || !outputFilekodowanyout.is_open()) {
        cerr << "Error opening file for input or output." << '\n';
        return 1;
    }

    getline(inputFileout, transformedLine);
    int num = stoi(transformedLine.substr(0, 10));
    int bitsNeeded = calculateBitsNeeded(num);
    string binary = bitset<10>(num).to_string();
    outputFilekodowanyout << binary;
    while (getline(inputFileout, transformedLine)) {
        transformedLine += "@";
        size_t index = 0;
        while (index < transformedLine.size()) {
            if (isdigit(transformedLine[index])) {
                size_t num_start = index;
                while (index < transformedLine.size() && isdigit(transformedLine[index])) {
                    index++;
                }
                num = stoi(transformedLine.substr(num_start, index - num_start));
                string binary = bitset<32>(num).to_string().substr(32 - bitsNeeded, bitsNeeded);
                outputFilekodowanyout << "11111" << binary;
            } else {
                if (encoding.find(transformedLine[index]) != encoding.end()) {
                    outputFilekodowanyout << encoding[transformedLine[index]];
                } else {
                    cerr << "Unsupported character found: " << transformedLine[index] << '\n';
                }
                index++;
            }
        }
    }

    inputFileout.close();
    outputFilekodowanyout.close();

    ifstream inputKodowany(coded_txt);
    if (!inputKodowany.is_open()) {
        cerr << "Error opening file kodowanykomp.txt" << '\n';
        return 1;
    }

    ofstream outputKodowany(bin_out, ios::out | ios::binary);
    if (!outputKodowany.is_open()) {
        cerr << "Error opening file output.bin" << '\n';
        return 1;
    }

    while (getline(inputKodowany, line)) {
        // Procesujemy ciąg bitów i zapisujemy każde 8 bitów jako jeden bajt
        for (size_t i = 0; i < line.size(); i += 8) {
            // Upewnij się, że masz wystarczającą liczbę bitów do przetworzenia
            string byteString = line.substr(i, 8);
            if (byteString.length() < 8) {
                byteString.append(8 - byteString.length(), '1');  // Dopełnij zerami, jeśli nie ma pełnego bajtu
            }
            // Konwersja ciągu binarnego na bajt
            char byte = static_cast<char>(bitset<8>(byteString).to_ulong());
            outputKodowany.write(&byte, sizeof(byte));
        }
    }

    inputKodowany.close();
    outputKodowany.close();
    ifstream inputKodowanyKomp(coded_out_txt);
    if (!inputKodowanyKomp.is_open()) {
        cerr << "Error opening file kodowanyout.txt" << '\n';
        return 1;
    }

    ofstream outputKodowanyKomp(out_comp_bin, ios::out | ios::binary);
    if (!outputKodowanyKomp.is_open()) {
        cerr << "Error opening file outputkomp.bin" << '\n';
        return 1;
    }

    while (getline(inputKodowanyKomp, line)) {
        // Procesujemy ciąg bitów i zapisujemy każde 8 bitów jako jeden bajt
        for (size_t i = 0; i < line.size(); i += 8) {
            // Upewnij się, że masz wystarczającą liczbę bitów do przetworzenia
            string byteString = line.substr(i, 8);
            if (byteString.length() < 8) {
                byteString.append(8 - byteString.length(), '1');  // Dopełnij zerami, jeśli nie ma pełnego bajtu
            }
            // Konwersja ciągu binarnego na bajt
            char byte = static_cast<char>(bitset<8>(byteString).to_ulong());
            outputKodowanyKomp.write(&byte, sizeof(byte));
        }
    }

    inputKodowanyKomp.close();
    outputKodowanyKomp.close();

    // Dekompresja i zapis do pliku text3.txt
    decodeHuffmanToFile(huff_bin, txt3, root);

    cout << "Conversion completed successfully.\n";

    // Otwieranie plików do odczytu, aby sprawdzić ich size
    ifstream inputKodowanyTest(bin_out, ios::binary | ios::ate);
    ifstream inputKodowanyKompTest(out_comp_bin, ios::binary | ios::ate);
    ifstream inputKodowanyKompHuffmanTest(huff_bin, ios::binary | ios::ate);
    if (!inputKodowanyTest.is_open() || !inputKodowanyKompTest.is_open() || !inputKodowanyKompHuffmanTest.is_open()) {
        cerr << "Error opening binary files to read size.\n";
        return 1;
    }

    int a = inputKodowanyTest.tellg();
    int b = inputKodowanyKompTest.tellg();
    int c = inputKodowanyKompHuffmanTest.tellg();
    // Pobieranie i wyświetlanie sizeu plików
    cout << "Size of output.bin: " << a << " bytes.\n";
    cout << "Size of outputkomp.bin (LZ78): " << b << " bytes, making it smaller than the original by " << 1.0 * (a - b) / a * 100 << "%" << '\n';
    cout << "Size of huffman.bin (Huffman): " << c << " bytes, making it smaller than the original by " << 1.0 * (a - c) / a * 100 << "%" << '\n';

    inputKodowany.close();
    inputKodowanyKomp.close();

    ifstream binaryInput(out_comp_bin, ios::binary);
    ofstream textOutput(coded_cmp2);

    if (!binaryInput.is_open()) {
        cerr << "Error opening outputkomp.bin for reading.\n";
        return 1;
    }

    if (!textOutput.is_open()) {
        cerr << "Error opening kodowanykomp2.txt for writing.\n";
        return 1;
    }

    char byte;
    while (binaryInput.read(&byte, sizeof(byte))) {
        textOutput << bitset<8>(byte);
    }

    binaryInput.close();
    textOutput.close();

    // mirrored_letters_map dekodowania
    unordered_map<string, char> decoding = {
        {"00000", 'a'}, {"00001", 'b'}, {"00010", 'c'}, {"00011", 'd'}, {"00100", 'e'}, {"00101", 'f'}, {"00110", 'g'}, {"00111", 'h'}, {"01000", 'i'}, {"01001", 'j'}, {"01010", 'k'}, {"01011", 'l'}, {"01100", 'm'}, {"01101", 'n'}, {"01110", 'o'}, {"01111", 'p'}, {"10000", 'q'}, {"10001", 'r'}, {"10010", 's'}, {"10011", 't'}, {"10100", 'u'}, {"10101", 'v'}, {"10110", 'w'}, {"10111", 'x'}, {"11000", 'y'}, {"11001", 'z'}, {"11010", '@'}, {"11011", '#'},  // @ to nowa linia
        {"11100", ','},
        {"11101", '.'},
        {"11110", ' '}};

    ifstream input(coded_cmp2, ios::binary);
    ofstream output(output_2);
    if (!input.is_open() || !output.is_open()) {
        cerr << "Error opening files.\n";
        return 1;
    }

    stringstream buffer;
    buffer << input.rdbuf();
    string bits = buffer.str();
    input.close();

    // Dekodujemy rozmiar slownika
    string numberBits = bits.substr(0, 10);
    bitset<10> bitsetNumber(numberBits);
    int dictionarySize = static_cast<int>(bitsetNumber.to_ulong());
    output << dictionarySize << '\n';

    int bitsNeededDecode = calculateBitsNeeded(dictionarySize);

    // Dekodujemy slowa ze slownika
    size_t pos = 10;
    string decodedWord;
    for (int i = 0; pos + 5 <= bits.size() && i < dictionarySize; i++) {
        string bitString = bits.substr(pos, 5);
        pos += 5;
        if (decoding[bitString] == '@') {
            output << decodedWord << '\n';
            decodedWord.clear();
        } else {
            decodedWord += decoding[bitString];
        }
    }

    // Dekodujemy reszte
    while (pos + 5 <= bits.size()) {
        string decodedText;
        while (pos + 5 <= bits.size() && bits.substr(pos, 5) != "11111") {
            string bitString = bits.substr(pos, 5);
            if (decoding.find(bitString) != decoding.end()) {
                if (decoding[bitString] == '@') {
                    output << decodedText << '\n';
                    decodedText.clear();
                } else {
                    decodedText += decoding[bitString];
                }
            }
            pos += 5;
        }
        // Handle numeric values after special marker "11111"
        if (pos + 5 + bitsNeededDecode <= bits.size() && bits.substr(pos, 5) == "11111") {
            pos += 5;  // skip the marker
            string numberBits = bits.substr(pos, bitsNeededDecode);
            // Utwórz bitset z odpowiednią liczbą zer jako dopełnienie
            bitset<32> bitsetNumber(string(32 - bitsNeededDecode, '0') + numberBits);
            int number = static_cast<int>(bitsetNumber.to_ulong());
            decodedText += to_string(number);
            pos += bitsNeededDecode;
        }
        output << decodedText;
        if (pos + 5 <= bits.size() && pos + 5 + bitsNeededDecode > bits.size()) {
            break;
        }
    }

    output.close();
    ifstream inFileOut2(output_2);
    if (!inFileOut2.is_open()) {
        cerr << "Unable to open 'out2.txt'.\n";
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
    ofstream outFileText2(txt2);

    if (!outFileText2.is_open()) {
        cerr << "Unable to open 'text2.txt'.\n";
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
                    cerr << "Index out of dictionary bounds: " << index << '\n';
                }
            } catch (invalid_argument&) {
                outFileText2 << word;
            }

        } while (ss >> word);

        if (getline(inFileOut2, line)) {
            outFileText2 << "\n";
        } else {
            break;
        }
    }

    inFileOut2.close();
    outFileText2.close();

    cout << "Processing completed.\n";

    return 0;
}
