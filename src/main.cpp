#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "tokenizer.hpp"
#include "parser.hpp"

// region Using directives
using std::cout;
using std::cerr;
using std::endl;
using std::exit;
using std::ifstream;
using std::string;
using std::stringstream;
using std::unitbuf;
using std::vector;

using namespace lox::tokenizer;
using namespace lox::parser;
// endregion

string read_file_contents(const string& filename);

int main(int argc, char *argv[]) {
    // Disable output buffering
    cout << unitbuf;
    cerr << unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    cerr << "Logs from your program will appear here!" << endl;

    if (argc < 3) {
        cerr << "Usage: ./your_program tokenize|parse <filename>" << endl;
        return 1;
    }

    const string command = argv[1];

    if (command == "tokenize") {
        string file_contents = read_file_contents(argv[2]);

        bool contained_errors = false;
        vector<token::Token> tokens = tokenize(file_contents, &contained_errors);

        for (const auto& token: tokens){
            token.show_in_cli();
        }
        if (contained_errors){
            return 65;
        }

    }
    else if (command == "parse"){
        string file_contents = read_file_contents(argv[2]);

        bool contains_errors = parse(file_contents);
        if (contains_errors)
            return 65;
    }
    else{
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
