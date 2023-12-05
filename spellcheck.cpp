#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <cctype>
#include <cstdlib>

using namespace std;

const int ALPHA_SIZE = 26;
bool interactiveMode = false;
bool suggestionsMode = true;

struct TrieNode {
  TrieNode *children[ALPHA_SIZE];
  bool isEndOfWord;

  TrieNode() {
    isEndOfWord = false;
    for(int i = 0; i < ALPHA_SIZE; i++) {
      children[i] = NULL;
    }
  }
};

void insert(TrieNode *root, string word) {
  TrieNode *node = root;
  for(char c : word) {
    int index = c - 'a';
    if(!node->children[index]) {
      node->children[index] = new TrieNode(); 
    }
    node = node->children[index];
  }
  node->isEndOfWord = true;
}

bool search(TrieNode *root, string word) {
  TrieNode *node = root;
  for(char c : word) {
    int index = c - 'a';
    if(!node->children[index]) {
      return false;
    }
    node = node->children[index];
  }
  return (node != NULL && node->isEndOfWord);
}

void loadDictionary(TrieNode* root, string filename) {
  ifstream file(filename);
  if (!file.is_open()) {
    cout << "Error: Could not open dictionary file '" << filename << "'. Please make sure the file exists and is readable." << endl;
    exit(1);
  }
  string word;
  while(file >> word) {
    transform(word.begin(), word.end(), word.begin(), ::tolower);
    insert(root, word);
  }
}

void suggestions(TrieNode* root, string word) {
  
  // missing char
  for(char c = 'a'; c <= 'z'; c++) {
    string newWord = word + c;
    if(search(root, newWord)) {
      cout << newWord << endl;
    }
  }
  
  // extra char
  for(int i = 0; i < word.length(); i++) {
    string newWord = word;
    newWord.erase(newWord.begin() + i);
    if(search(root, newWord)) {
      cout << newWord << endl;  
    }
  }

  // swapped char
  for(int i = 0; i < word.length()-1; i++) {
    string newWord = word;
    swap(newWord[i], newWord[i+1]);
    if(search(root, newWord)) {
      cout << newWord << endl;
    }
  }

  // for doubles
  for (int i = 0; i < word.length() - 1; i++) {
    if (word[i] == word[i + 1]) {
      string new_word = word.substr(0, i) + word.substr(i + 2);
      if (search(root, new_word)) {
        cout << new_word << endl;
      }
    }
  }

  // transposed letters
  for (int i = 0; i < word.length() - 1; i++) {
    string new_word = word;
    swap(new_word[i], new_word[i + 1]);
    if (search(root, new_word)) {
      cout << new_word << endl;
    }
  }
}

void saveTrieNode(TrieNode* node, ofstream& file);

void saveLearnedWords(TrieNode* root) {
  // Open a file
  ofstream file("learned_words.txt");
  saveTrieNode(root, file);
  file.close();
}

void saveTrieNode(TrieNode* node, ofstream& file) {
  file << node->isEndOfWord << endl;
  // save the node's children.
  for (int i = 0; i < ALPHA_SIZE; i++) {
    if (node->children[i] != NULL) {
      saveTrieNode(node->children[i], file);
    }
  }
}

void printUsage() {
  cout << "Usage: " << endl;
  cout << "  -i: Interactive mode" << endl;
  cout << "  -b <file>: Batch mode, check spellings in <file>" << endl;
  cout << "  -s: Enable/disable suggestions" << endl;
}

void printMenu() {
cout << "+---------------------------------------+" << endl;
cout << "| Spell Checker Menu                    |" << endl;
cout << "+---------------------------------------+" << endl;
cout << "| 1. Check spelling                     |" << endl;
cout << "| 2. Exit                               |" << endl;
cout << "| 3. Suggestions                        |" << endl;
cout << "+---------------------------------------+" << endl;
}

int main(int argc, char** argv) {

  if(argc < 2) {
    printUsage();
    return 1;
  }

  for(int i=1; i<argc; i++) {
    if(string(argv[i]) == "-i") {
      interactiveMode = true; 
    }
    else if(string(argv[i]) == "-b") {
      // open file
      i++;
    }
    else if(string(argv[i]) == "-s") {
      suggestionsMode = !suggestionsMode;
    }
    else {
      printUsage();
      return 1;
    }
  }  

  TrieNode* root = new TrieNode();

  loadDictionary(root, "words.txt");
  
  string word;

  printMenu();
  cout << "Enter word to check or 'quit' to exit: ";
  getline(cin, word);

  while(word != "quit") {

    if( word == "quit" ) {
      break;
    }

    transform(word.begin(), word.end(), word.begin(), ::tolower);

    if(search(root, word)) {
      cout << "Spelling is correct." << endl;
      
    }
    else {
      cout << endl;
      cout << "Spelling is wrong. Possible right spellings are given below:-" << endl;
      cout << endl;

      //print sugg.
      suggestions(root, word);

      cout << endl;
      
    }

    cout << endl << "Enter word to check or 'quit' to exit: ";
    getline(cin, word);
  }
  saveLearnedWords(root);

  return 0;
}
