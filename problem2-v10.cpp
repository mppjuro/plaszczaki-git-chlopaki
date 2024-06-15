// metoda kompresji - własna wariacja algorytmu LZ78 - słownikowej bezstratnej kompresji danych
// polecenie budowania: g++ -O3 -std=c++17 -o "%e" "%f"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <bitset>
#include <map>
#include <set>
#include <cctype>
#include <sstream>
#include <iterator>
#include <queue>
#include <cmath>
#include <functional>
#include <ctime>

using namespace std;
int iloscZamian = 0;
/*
// HUFFMANN START

class Node {
public:
    char ch;
    int freq;
    Node *left, *right;

    Node(char ch, int freq, Node *left = nullptr, Node *right = nullptr)
        : ch(ch), freq(freq), left(left), right(right){}
};

struct Compare {
    bool operator()(Node* l, Node* r){
        return l->freq > r->freq;
    }
};

void generateCodes(Node* root, string code, unordered_map<char, string>& huffmanCode){
    if(root == nullptr)
        return;

    if(!root->left && !root->right){
        huffmanCode[root->ch] = code;
    }

    generateCodes(root->left, code + "0", huffmanCode);
    generateCodes(root->right, code + "1", huffmanCode);
}

Node* buildHuffmanTree(const string& text){
    unordered_map<char, int> freq;
    for (char ch : text){
        freq[ch]++;
    }

    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto pair : freq){
        pq.push(new Node(pair.first, pair.second));
    }

    while(pq.size() != 1){
        Node *left = pq.top(); pq.pop();
        Node *right = pq.top(); pq.pop();

        int sum = left->freq + right->freq;
        pq.push(new Node('\0', sum, left, right));
    }

    return pq.top();
}

string encodeHuffman(const string& text, unordered_map<char, string>& huffmanCode){
    string encoded;
    for (char ch : text){
        encoded += huffmanCode[ch];
    }
    return encoded;
}

string decodeHuffman(const string& encodedText, Node* root){
    string decoded;
    Node* current = root;
    for (char bit : encodedText){
        if(bit == '0') current = current->left;
        else current = current->right;

        if(!current->left && !current->right){
            decoded += current->ch;
            current = root;
        }
    }
    return decoded;
}

// Funkcja dekodująca dane Huffmana z pliku binarnego
void decodeHuffmanToFile(const string& inputFilename, const string& outputFilename, Node* root){
    ifstream file(inputFilename, ios::binary);
    ofstream outputFile(outputFilename);
    if(!file.is_open() || !outputFile.is_open()){
        cerr << "Nie można otworzyć pliku do odczytu lub zapisu." << '\n';
        return;
    }

    string encodedBits, decodedText;
    char byte;
    while(file.read(&byte, sizeof(char))){
        bitset<8> b(byte);
        encodedBits += b.to_string();
    }
    file.close();

    Node* current = root;
    for (char bit : encodedBits){
        current = (bit == '0') ? current->left : current->right;
        if(!current->left && !current->right){
            decodedText += current->ch;
            current = root;
        }
    }

    outputFile << decodedText;
    outputFile.close();
}

// HUFFMANN END
*/
string trim(const string& str){
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    if(first == string::npos || last == string::npos){
        return "";
    }
    return str.substr(first, (last - first + 1));
}

void printFileContent(const string& filename){
    ifstream file(filename, ios::binary);
    if(!file.is_open()){
        cerr << "Nie można otworzyc pliku do odczytu: " << filename << '\n';
        return;
    }

    cout << "Zawartosc pliku " << filename << ":" << '\n';
    
    // Wyświetlanie firstch 8 bajtów w formacie binarnym
    char c;
    int count = 0;
    cout << "Pierwsze 8 bajtow w formacie binarnym: ";
    while(file.get(c)){
        cout << bitset<8>(static_cast<unsigned char>(c)) << " ";
        count++;
    }
    cout << '\n';
    cout << '\n' << "Koniec pliku." << '\n';
    file.close();
}

void saveDictionaryAndTransformedText(const string& filename, const vector<string>& dictionary, const string& text){
    ofstream outFile(filename);
    if(!outFile.is_open()){
        cerr << "Error opening file " << filename << '\n';
        return;
    }
    outFile << dictionary.size() << '\n';
    for (const string& seq : dictionary){
        outFile << seq << '\n';
    }
    outFile << text;
    outFile.close();
}

// Funkcja usuwająca znaki interpunkcyjne z ciągu znaków
string rmv_interpunction(const string& tekst){
    string wynik;
    remove_copy_if(tekst.begin(), tekst.end(), back_inserter(wynik), [](char c){
        return ispunct(static_cast<unsigned char>(c));
    });
    return wynik;
}

//Funkcja tworząca mapę możliwych odwróceń liter i ich odbić lustrzanych
unordered_map<char, set<char>> mirrored_letters(){
    unordered_map<char, set<char>> mirrored_letters_map;
    mirrored_letters_map['a'] = {'a', 'e'};
    mirrored_letters_map['b'] = {'b', 'q', 'd', 'p'};
    mirrored_letters_map['c'] = {'c', 'u'};
    mirrored_letters_map['d'] = {'d', 'b', 'p', 'q'};
    mirrored_letters_map['e'] = {'e', 'a'};
    mirrored_letters_map['h'] = {'h', 'y'};
    mirrored_letters_map['m'] = {'m', 'w'};
    mirrored_letters_map['n'] = {'n', 'u'};
    mirrored_letters_map['p'] = {'p', 'b', 'q', 'd'};
    mirrored_letters_map['q'] = {'q', 'b', 'p', 'd'};
    mirrored_letters_map['u'] = {'u', 'n', 'c'};
    mirrored_letters_map['w'] = {'w', 'm'};
    mirrored_letters_map['y'] = {'y', 'h'};
    return mirrored_letters_map;
}

// Funkcja sprawdzająca, czy odwrócenie liter w słowie daje słowo ze słownika
string znajdzSlowoOdwracane(const string& slowo, const unordered_map<char, set<char>>& mirrored_letters_map, const set<string>& slownik){
    set<string> przetworzoneSlowa;
    przetworzoneSlowa.insert(slowo);
    queue<string> kolejka;
    kolejka.push(slowo);

    while(!kolejka.empty()){
        string obecneSlowo = kolejka.front();
        kolejka.pop();

        for (size_t i = 0; i < obecneSlowo.size(); i++){
            char orig = obecneSlowo[i];
            if(mirrored_letters_map.find(orig) != mirrored_letters_map.end()){
                for (char zamiennik : mirrored_letters_map.at(orig)){
                    string noweSlowo = obecneSlowo;
                    noweSlowo[i] = zamiennik;

                    if(slownik.find(noweSlowo) != slownik.end() && noweSlowo.size() == slowo.size()){
                        cout << "Zamiana \"" << slowo << "\" na \"" << noweSlowo << "\"" << '\n';
						iloscZamian++;
                        return noweSlowo;
                    }

                    if(przetworzoneSlowa.find(noweSlowo) == przetworzoneSlowa.end()){
                        przetworzoneSlowa.insert(noweSlowo);
                        kolejka.push(noweSlowo);
                    }
                }
            }
        }
    }
    return slowo; // Jeśli nie znajdzie zamiany, zwraca oryginalne słowo
}

int calculateBitsNeeded(int num){
    return ceil(log2(num + 1));
}

int main(){
	ios::sync_with_stdio(false);
    srand(time(NULL));
	map<string, int> config;
    ifstream config_file("config.txt");
    if(!config_file.is_open()){
        cerr << "Couldn't open the config file!\n";
        return 1;
    }

    string line;
    while(getline(config_file, line)){
        size_t pos = line.find("=");
        if(pos != string::npos){
            string key = trim(line.substr(0, pos));
            int value = stoi(trim(line.substr(pos + 1)));
            config[key] = value;
        }
    }
    config_file.close();
    
	const int n = config["MIN_WORD_QUANTITY"]; //n wystapien
	const int m = config["MIN_WORD_LENGTH"]; //min dlugosc
	const int k = config["DICT_SIZE"]; //max ilosc w slowniku
	
    // Wczytywanie słownika
    set<string> slownik;
    ifstream plikSlownik("slownik.txt");
    string slowo;
    while(plikSlownik >> slowo){
        slownik.insert(slowo);
    }
    plikSlownik.close();

    // Tworzenie mapy odwróceń liter
    auto mirrored_letters_mapOdwracen = mirrored_letters();

    // Wczytywanie tekstu do przetworzenia
    ifstream plikTekst("tekstzpoli.txt");
    ofstream plikWynik("tekstbezpoli.txt");
    string linia;
    while(getline(plikTekst, linia)){
        stringstream ss(linia);
        string wyraz;
        while(ss >> wyraz){
            string slowoBezInterpunkcji = rmv_interpunction(wyraz);
            string wynikoweSlowo = slowoBezInterpunkcji;

            if(slownik.find(slowoBezInterpunkcji) == slownik.end()){
                string slowoPoOdwracaniu = znajdzSlowoOdwracane(slowoBezInterpunkcji, mirrored_letters_mapOdwracen, slownik);
                if(!slowoPoOdwracaniu.empty() && slowoPoOdwracaniu.size() == slowoBezInterpunkcji.size()){
                    wynikoweSlowo = slowoPoOdwracaniu;
                }
            }

            // Odtwarzanie interpunkcji w wynikowym słowie
            size_t j = 0;
            for (size_t i = 0; i < wyraz.size(); i++){
                if(isalpha(wyraz[i])){
                    wyraz[i] = wynikoweSlowo[j++];
                }
            }
            plikWynik << wyraz << " ";
        }
        plikWynik << '\n';
    }
    plikTekst.close();
    plikWynik.close();

    cout << "Przetwarzanie zakonczone, ilosc zamian " << iloscZamian << ". Wynik zapisany w pliku tekstbezpoli.txt." << '\n';
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
    ifstream inFile("tekstbezpoli.txt");
    if(!inFile.is_open()){
        cerr << "Error opening file tekstbezpoli.txt" << '\n';
        return 1;
    }

    string content((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    if(content.empty() || content.size() < m || n < 1){
        cerr << "Invalid content size or parameters." << '\n';
        return 1;
    }
    unordered_map<string, int> substringCount;
    vector<string> words;
    size_t start = 0;
    while(start < content.size()){
        size_t end = content.find(' ', start);
        if(end == string::npos){
            end = content.length();
        }
        string word = content.substr(start, end - start);
        size_t newlinePos = word.find('\n');
        while(newlinePos != string::npos){
            word.replace(newlinePos, 1, "@");
            newlinePos = word.find('\n', newlinePos + 1);
        }
        if(word.length() >= m){
            words.push_back(word);
        }
        start = end + 1;
    }

    for (const auto& word : words){
        substringCount[word]++;
    }

    vector<pair<string, string>> replacementPairs;
    unordered_map<string, int> dictionary;
    unordered_set<string> usedSubstrings;
    vector<string> dictionaryVector;
    int counter = 1;

    for (auto& pair : substringCount){
        if(pair.second > n && counter <= k){
            dictionary[pair.first] = counter;
            dictionaryVector.push_back(pair.first);
            replacementPairs.emplace_back(pair.first, to_string(counter));
            counter++;
        }
    }

    sort(replacementPairs.begin(), replacementPairs.end(), [](const pair<string, string>& a, const pair<string, string>& b){
        return a.first.size() > b.first.size();
    });

    string transformedText = content;
    for (auto& pair : replacementPairs){
        size_t pos = transformedText.find(pair.first);
        while(pos != string::npos){
            transformedText.replace(pos, pair.first.length(), pair.second);
            usedSubstrings.insert(pair.first);
            pos = transformedText.find(pair.first, pos + pair.second.length());
        }
    }

    vector<string> finalDictionary;
    unordered_map<string, string> newMapping;
    int newCounter = 1;
    for (const auto& seq : usedSubstrings){
        finalDictionary.push_back(seq);
        newMapping[seq] = to_string(newCounter++);
    }

    for (auto& pair : replacementPairs){
        if(newMapping.find(pair.first) != newMapping.end()){
            pair.second = newMapping[pair.first];
        }
    }

    transformedText = content; // Resetting the transformedText to original
    for (auto& pair : replacementPairs){
        size_t pos = transformedText.find(pair.first);
        while(pos != string::npos){
            transformedText.replace(pos, pair.first.length(), pair.second);
            pos = transformedText.find(pair.first, pos + pair.second.length());
        }
    }

    saveDictionaryAndTransformedText("out.txt", finalDictionary, transformedText);
/*
//huffmann START
    // Kompresja Huffmana
    unordered_map<char, string> huffmanCode;
    Node* root = buildHuffmanTree(content);
    generateCodes(root, "", huffmanCode);
    string encodedHuffman = encodeHuffman(content, huffmanCode);

    // Zapisz zakodowany tekst Huffmana do pliku binarnego
    ofstream huffmanFile("huffman.bin", ios::binary);
    for (size_t i = 0; i < encodedHuffman.length(); i += 8){
        bitset<8> b(encodedHuffman.substr(i, 8));
        char c = static_cast<char>(b.to_ulong());
        huffmanFile.write(&c, sizeof(char));
    }
    huffmanFile.close();

//HUFFMANN END
*/
    // Etap 1: Kodowanie tekstu do formatu binarnego
    unordered_map<char, string> encoding = {
        {'a', "00000"}, {'b', "00001"}, {'c', "00010"}, {'d', "00011"},
        {'e', "00100"}, {'f', "00101"}, {'g', "00110"}, {'h', "00111"},
        {'i', "01000"}, {'j', "01001"}, {'k', "01010"}, {'l', "01011"},
        {'m', "01100"}, {'n', "01101"}, {'o', "01110"}, {'p', "01111"},
        {'q', "10000"}, {'r', "10001"}, {'s', "10010"}, {'t', "10011"},
        {'u', "10100"}, {'v', "10101"}, {'w', "10110"}, {'x', "10111"},
        {'y', "11000"}, {'z', "11001"}, {'@', "11010"}, {'#', "11011"}, // @ to nowa linia, # to nowa linia wewnątrz słownika
        {',', "11100"}, {'.', "11101"}, {' ', "11110"}
    };
    
    ifstream inputFile("tekstbezpoli.txt", ios::in);
    ofstream outputFile("kodowany.txt", ios::out);

    if(!inputFile.is_open() || !outputFile.is_open()){
        cerr << "Error opening file for input or output." << '\n';
        return 1;
    }

    string transformedLine;
    while(getline(inputFile, transformedLine)){
		transformedLine += "@";
        size_t index = 0;
        while(index < transformedLine.size()){
			if(encoding.find(transformedLine[index]) != encoding.end()){
				outputFile << encoding[transformedLine[index]];
			} else{
				cerr << "Unsupported character found: " << transformedLine[index] << '\n';
           }
           index++;
        }
    }

    inputFile.close();
    outputFile.close();
    
    ifstream inputFileout("out.txt", ios::in);
    ofstream outputFilekodowanyout("kodowanyout.txt", ios::out);

    if(!inputFileout.is_open() || !outputFilekodowanyout.is_open()){
        cerr << "Error opening file for input or output." << '\n';
        return 1;
    }

    getline(inputFileout, transformedLine);
    int num = stoi(transformedLine.substr(0, 10));
    int bitsNeeded = calculateBitsNeeded(num);
    string binary = bitset<10>(num).to_string();
    outputFilekodowanyout << binary;
    while(getline(inputFileout, transformedLine)){
		transformedLine += "@";
        size_t index = 0;
        while(index < transformedLine.size()){
            if(isdigit(transformedLine[index])){
                size_t num_start = index;
                while(index < transformedLine.size() && isdigit(transformedLine[index])){
                    index++;
                }
                num = stoi(transformedLine.substr(num_start, index - num_start));
                string binary = bitset<32>(num).to_string().substr(32 - bitsNeeded, bitsNeeded);
                outputFilekodowanyout << "11111" << binary;
            } else{
                if(encoding.find(transformedLine[index]) != encoding.end()){
                    outputFilekodowanyout << encoding[transformedLine[index]];
                } else{
                    cerr << "Unsupported character found: " << transformedLine[index] << '\n';
                }
                index++;
            }
        }
    }

    inputFileout.close();
    outputFilekodowanyout.close();
    
   ifstream inputKodowany("kodowany.txt");
    if(!inputKodowany.is_open()){
        cerr << "Error opening file kodowanykomp.txt" << '\n';
        return 1;
    }

    ofstream outputKodowany("output.bin", ios::out | ios::binary);
    if(!outputKodowany.is_open()){
        cerr << "Error opening file output.bin" << '\n';
        return 1;
    }

    while(getline(inputKodowany, line)){
        // Procesujemy ciąg bitów i zapisujemy każde 8 bitów jako jeden bajt
        for (size_t i = 0; i < line.size(); i += 8){
            // Upewnij się, że masz wystarczającą liczbę bitów do przetworzenia
            string byteString = line.substr(i, 8);
            if(byteString.length() < 8){
                byteString.append(8 - byteString.length(), '1'); // Dopełnij zerami, jeśli nie ma pełnego bajtu
            }
            // Konwersja ciągu binarnego na bajt
            char byte = static_cast<char>(bitset<8>(byteString).to_ulong());
            outputKodowany.write(&byte, sizeof(byte));
        }
    }

    inputKodowany.close();
    outputKodowany.close();
   ifstream inputKodowanyKomp("kodowanyout.txt");
    if(!inputKodowanyKomp.is_open()){
        cerr << "Error opening file kodowanyout.txt" << '\n';
        return 1;
    }

    ofstream outputKodowanyKomp("outputkomp.bin", ios::out | ios::binary);
    if(!outputKodowanyKomp.is_open()){
        cerr << "Error opening file outputkomp.bin" << '\n';
        return 1;
    }

    while(getline(inputKodowanyKomp, line)){
        // Procesujemy ciąg bitów i zapisujemy każde 8 bitów jako jeden bajt
        for (size_t i = 0; i < line.size(); i += 8){
            // Upewnij się, że masz wystarczającą liczbę bitów do przetworzenia
            string byteString = line.substr(i, 8);
            if(byteString.length() < 8){
                byteString.append(8 - byteString.length(), '1'); // Dopełnij zerami, jeśli nie ma pełnego bajtu
            }
            // Konwersja ciągu binarnego na bajt
            char byte = static_cast<char>(bitset<8>(byteString).to_ulong());
            outputKodowanyKomp.write(&byte, sizeof(byte));
        }
    }

    inputKodowanyKomp.close();
    outputKodowanyKomp.close();
/*
	//HUFFFMAN DECODE
    // Dekompresja i zapis do pliku text3.txt
    decodeHuffmanToFile("huffman.bin", "text3.txt", root);
	//HUFFAMN DECODE END
*/
    cout << "Conversion completed successfully." << '\n';
    
    // Otwieranie plików do odczytu, aby sprawdzić ich size
    ifstream inputKodowanyTest("output.bin", ios::binary | ios::ate);
    ifstream inputKodowanyKompTest("outputkomp.bin", ios::binary | ios::ate);
    /*
    
    
	ifstream inputKodowanyKompHuffmanTest("huffman.bin", ios::binary | ios::ate);


	*/
    if(!inputKodowanyTest.is_open() || !inputKodowanyKompTest.is_open() /*|| !inputKodowanyKompHuffmanTest.is_open()*/){
        cerr << "Error opening binary files to read size." << '\n';
        return 1;
    }
    

	
	int a = inputKodowanyTest.tellg();
	int b = inputKodowanyKompTest.tellg();
	/*
	
	int c = inputKodowanyKompHuffmanTest.tellg();
    
	*/
	// Pobieranie i wyświetlanie sizeu plików
    cout << "size pliku output.bin: " << a << " bajtow" << '\n';
    cout << "size pliku outputkomp.bin (kompresja to zmodyfikowany LZ78): " << b << " bajtow, czyli jest mniejszy od pierwszego pliku o " << 1.0*(a-b)/a * 100 << "%" << '\n';
    //cout << "size pliku huffman.bin (kompresja to algorytm Huffmana): " << c << " bajtow, czyli jest mniejszy od pierwszego pliku o " << 1.0*(a-c)/a * 100 << "%" << '\n';

    inputKodowany.close();
    inputKodowanyKomp.close();
    
ifstream binaryInput("outputkomp.bin", ios::binary);
ofstream textOutput("kodowanykomp2.txt");

if(!binaryInput.is_open()){
    cerr << "Error opening outputkomp.bin for reading." << '\n';
    return 1;
}

if(!textOutput.is_open()){
    cerr << "Error opening kodowanykomp2.txt for writing." << '\n';
    return 1;
}

char byte;
while(binaryInput.read(&byte, sizeof(byte))){
    textOutput << bitset<8>(byte);
}

binaryInput.close();
textOutput.close();


    // mirrored_letters_map dekodowania
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
    if(!input.is_open() || !output.is_open()){
        cerr << "Error opening files." << '\n';
        return 1;
    }

    stringstream buffer;
    buffer << input.rdbuf();
    string bits = buffer.str();
    input.close();

    // Decode the dictionary size
    string numberBits = bits.substr(0, 10);
    bitset<10> bitsetNumber(numberBits);
    int dictionarySize = static_cast<int>(bitsetNumber.to_ulong());
    output << dictionarySize << '\n';
    
    int bitsNeededDecode = calculateBitsNeeded(dictionarySize);


    // Decode words from the dictionary
    size_t pos = 10;
    string decodedWord;
    for (int i = 0; pos + 5 <= bits.size() && i < dictionarySize; i++){
        string bitString = bits.substr(pos, 5);
        pos += 5;
        if(decoding[bitString] == '@'){
            output << decodedWord << '\n';
            decodedWord.clear();
        } else{
            decodedWord += decoding[bitString];
        }
    }

    // Decode the remaining part of the file
    while(pos + 5 <= bits.size()){
        string decodedText;
        while(pos + 5 <= bits.size() && bits.substr(pos, 5) != "11111"){
            string bitString = bits.substr(pos, 5);
            if(decoding.find(bitString) != decoding.end()){
                if(decoding[bitString] == '@'){
                    output << decodedText << '\n';
                    decodedText.clear();
                } else{
                    decodedText += decoding[bitString];
                }
            }
            pos += 5;
        }
        // Handle numeric values after special marker "11111"
        if(pos + 5 + bitsNeededDecode <= bits.size() && bits.substr(pos, 5) == "11111"){
            pos += 5; // skip the marker
			string numberBits = bits.substr(pos, bitsNeededDecode);
			// Utwórz bitset z odpowiednią liczbą zer jako dopełnienie
			bitset<32> bitsetNumber(string(32 - bitsNeededDecode, '0') + numberBits);
			int number = static_cast<int>(bitsetNumber.to_ulong());
			decodedText += to_string(number);
			pos += bitsNeededDecode;
        }
        output << decodedText;
        if(pos + 5 <= bits.size() && pos + 5 + bitsNeededDecode > bits.size()){
			break;
		}
    }

    output.close();
	ifstream inFileOut2("out2.txt");
    if(!inFileOut2.is_open()){
        cerr << "Nie można otworzyć pliku 'out2.txt'." << '\n';
        return 1;
    }

    // Wczytanie pierwszej linii, która zawiera ilość rekordów w słowniku
    getline(inFileOut2, line);
    stringstream(line) >> dictionarySize;

    // Wczytanie rekordów słownika
    vector<string> dictionarydecode(dictionarySize);
    for (int i = 0; i < dictionarySize; i++){
        getline(inFileOut2, line);
        dictionarydecode[i] = line;
    }

    // Przetworzenie reszty pliku, zamieniając indeksy na tekst z słownika
    ofstream outFileText2("text2.txt");
    
    if(!outFileText2.is_open()){
        cerr << "Nie można otworzyć pliku 'text2.txt' do zapisu." << '\n';
        return 1;
    }
    
    getline(inFileOut2, line);
    
    while(true){
    	
        stringstream ss(line);
        int first = 0;
        string word;
        
        if(!(ss >> word)){
			break;
		}
		
        do{
        	
			if(first){
				outFileText2 << " ";
			}
			first++;
			
            try{
                int index = stoi(word);
                if(index > 0 && index <= dictionarySize){
                    outFileText2 << dictionarydecode[index - 1];
                } 
				else{
                    cerr << "Indeks poza zakresem słownika: " << index << '\n';
                }
            } 
			catch(invalid_argument&){
                outFileText2 << word;
            }
            
        } while(ss >> word);
        
        if(getline(inFileOut2, line)){
			outFileText2 << "\n";
		}
		else{
			break;
		}
	
    }

    inFileOut2.close();
    outFileText2.close();

    cout << "Przetwarzanie zakonczone." << '\n';
    

    return 0;
}
