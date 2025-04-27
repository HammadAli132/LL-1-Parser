#ifndef STRING_PARSER_H
#define STRING_PARSER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <algorithm>
#include <cctype>  // For isdigit()

using namespace std;

class string_parser {
private:
    unordered_map<string, unordered_map<string, string>> parse_table;
    string start_symbol;
    string input_filename;
    int error_count;

    // Helper methods
    string displayStack(stack<string> stk);
    string displayInput(const vector<string>& tokens, size_t pos);
    bool isTerminal(const string& symbol);
    vector<string> tokenizeProduction(const string& production);
    string classifyToken(const string& token);  // New method for token classification

    // Parse a single string
    void parseString(const string& input_line, int line_number);

public:
    string_parser(unordered_map<string, unordered_map<string, string>> parse_table,
        string start_symbol,
        const string& input_filename);
    ~string_parser();

    // Parse all strings in the input file
    bool parseInputFile();
};

#endif // STRING_PARSER_H