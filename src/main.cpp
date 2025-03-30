#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include "tokenizer.hpp"

// region Using directives
using std::cout;
using std::cerr;
using std::endl;
using std::exit;
using std::ifstream;
using std::string;
using std::stringstream;
using std::unitbuf;
// endregion

string read_file_contents(const string& filename);

int main(int argc, char *argv[]) {
    // Disable output buffering
    cout << unitbuf;
    cerr << unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    cerr << "Logs from your program will appear here!" << endl;

    if (argc < 3) {
        cerr << "Usage: ./your_program tokenize <filename>" << endl;
        return 1;
    }

    const string command = argv[1];

    if (command == "tokenize") {
        string file_contents = read_file_contents(argv[2]);
        
        // Uncomment this block to pass the first stage
        // 
         if (!file_contents.empty()) {
             tokenizer::tokenize(file_contents);
         }
         cout << "EOF  null" << endl; // Placeholder, remove this line when implementing the scanner
        
    } else {
        cerr << "Unknown command: " << command << endl;
        return 1;
    }

    return 0;
}

string read_file_contents(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error reading file: " << filename << endl;
        exit(1);
    }

    stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}
