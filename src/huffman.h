#ifndef huffman
#define huffman

#include <cstring>
#include <map>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class Node;

struct Compare;

void generateCodes(Node* root, string code, unordered_map<char, string>& huffmanCode);

Node* buildHuffmanTree(const string& text);

string encodeHuffman(const string& text, unordered_map<char, string>& huffmanCode);

void decodeHuffmanToFile(const string& inputFilename, const string& outputFilename, Node* root);

#endif