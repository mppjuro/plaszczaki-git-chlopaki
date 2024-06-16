#include "huffman.h"
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

class Node{
	public:
	    char ch;
	    int freq;
	    Node *left, *right;
	    Node(char ch, int freq, Node *left = nullptr, Node *right = nullptr)
	        : ch(ch), freq(freq), left(left), right(right){}
};

struct Compare{
    bool operator()(Node *l, Node *r){
        return l->freq > r->freq;
    }
};

void generateCodes(Node *root, string code, unordered_map<char, string>& huffmanCode){
    if(root == nullptr){
    	return;	
	}
    if(!root->left && !root->right){
        huffmanCode[root->ch] = code;
    }
    generateCodes(root->left, code + "0", huffmanCode);
    generateCodes(root->right, code + "1", huffmanCode);
}

Node *buildHuffmanTree(const string &text){
    unordered_map<char, int> freq;
    for(char ch : text){
        freq[ch]++;
    }
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for(auto pair : freq){
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

string encodeHuffman(const string& text, unordered_map<char, string>& huffmanCode){							//Funkcja kodująca dane
    string encoded;
    for(char ch : text){
        encoded += huffmanCode[ch];
    }
    return encoded;
}

void decodeHuffmanToFile(const string &inputFilename, const string &outputFilename, Node *root){			//Funkcja dekodująca dane Huffmana z pliku binarnego
    ifstream file(inputFilename, ios::binary);
    ofstream outputFile(outputFilename);
    if(!file.is_open() || !outputFile.is_open()){
        cerr << "Unable to open.\n";
        return;
    }

    string encodedBits, decodedText;
    char byte;
    while(file.read(&byte, sizeof(char))){
        bitset<8> b(byte);
        encodedBits += b.to_string();
    }
    file.close();

    Node *current = root;
    for(char bit : encodedBits){
        current = (bit == '0') ? current->left : current->right;
        if(!current->left && !current->right){
            decodedText += current->ch;
            current = root;
        }
    }

    outputFile << decodedText;
    outputFile.close();
}