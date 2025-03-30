#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// region Using directives
using std::cout;
using std::cerr;
using std::endl;
using std::exit;
using std::string;
using std::stringstream;
using std::unitbuf;
// endregion

std::string read_file_contents(const std::string& filename);

int main(int argc, char *argv[]) {
    // Disable output buffering
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cerr << "Logs from your program will appear here!" << std::endl;

    if (argc < 3) {
        std::cerr << "Usage: ./your_program tokenize <filename>" << std::endl;
        return 1;
    }

    const std::string command = argv[1];

    if (command == "tokenize") {
        std::string file_contents = read_file_contents(argv[2]);
        
        // Uncomment this block to pass the first stage
        // 
         if (!file_contents.empty()) {
             for (auto byte: file_contents){
                 switch (byte){
                     case '(':
                         cout << "LEFT_PAREN ( null" << endl;
                         break;
                     case ')':
                         cout << "RIGHT_PAREN ) null" << endl;
                         break;
                 }
             }
         }
         std::cout << "EOF  null" << std::endl; // Placeholder, remove this line when implementing the scanner
        
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}

std::string read_file_contents(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error reading file: " << filename << std::endl;
        std::exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}
