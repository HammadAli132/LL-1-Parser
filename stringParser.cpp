#include "stringParser.h"

string_parser::string_parser(unordered_map<string, unordered_map<string, string>> parse_table,
    string start_symbol,
    const string& input_filename)
    : parse_table(parse_table), start_symbol(start_symbol), input_filename(input_filename) {

    error_count = 0;
}

string_parser::~string_parser() {
    cout << "Destructing String Parser" << endl;
}

string string_parser::classifyToken(const string& token) {
    // Check if token is a number
    if (isdigit(token[0]) ||
        (token.size() > 1 && token[0] == '-' && isdigit(token[1]))) {
        return "num";
    }

    // Check if token is a reserved keyword
    if (token == "if" || token == "else" || token == "while") {
        return token;
    }

    // Check if token is a special symbol/operator
    if (token == "=" || token == "+" || token == "-" || token == "*" ||
        token == "/" || token == ";" || token == "(" || token == ")" ||
        token == "{" || token == "}" || token == "==" || token == "!=" ||
        token == "<" || token == ">" || token == "<=" || token == ">=") {
        return token;
    }

    // If not a number, keyword, or special symbol, it's an identifier
    return "id";
}

bool string_parser::parseInputFile() {
    ifstream file(input_filename);
    if (!file) {
        cerr << "Error: Could not open input file: " << input_filename << endl;
        return false;
    }

    string line;
    int line_number = 0;

    while (getline(file, line)) {
        line_number++;
        cout << "\n---------- Parsing Line " << line_number << ": " << line << " ----------\n";
        parseString(line, line_number);
    }

    file.close();

    cout << "\n==========================================\n";
    if (error_count == 0) {
        cout << "Parsing completed successfully with no errors.\n";
    }
    else {
        cout << "Parsing completed with " << error_count << " error(s).\n";
    }

    return error_count == 0;
}

void string_parser::parseString(const string& input_line, int line_number) {
    // Tokenize the input string
    istringstream iss(input_line);
    vector<string> tokens;
    vector<string> original_tokens; // Keep original tokens for error reporting
    string token;

    while (iss >> token) {
        original_tokens.push_back(token);
        tokens.push_back(classifyToken(token));
    }
    tokens.push_back("$"); // Add end marker
    original_tokens.push_back("$");

    // Initialize parsing stack
    stack<string> parsing_stack;
    parsing_stack.push("$"); // Push end marker first
    parsing_stack.push(start_symbol); // Push start symbol

    size_t input_pos = 0; // Current position in the input
    bool error_mode = false;
    bool recovery_attempt = false;

    cout << "Step\tStack\t\t\tInput\t\t\tAction\n";
    cout << "----\t-----\t\t\t-----\t\t\t------\n";

    int step = 1;

    // Continue parsing until either stack is empty or input is fully consumed
    while (!parsing_stack.empty() && input_pos < tokens.size()) {
        string stack_top = parsing_stack.top();
        string current_input = tokens[input_pos];

        // Display stack contents
        string stack_str = displayStack(parsing_stack);
        string input_str = displayInput(tokens, input_pos);
        string orig_input_str = displayInput(original_tokens, input_pos);

        if (stack_top == "$" && current_input == "$") {
            // End of parsing - success
            cout << step << "\t" << stack_str << "\t\t" << orig_input_str << "\t\tAccept\n";
            break;
        }

        if (stack_top == current_input) {
            // Terminal match - pop stack and advance input
            cout << step << "\t" << stack_str << "\t\t" << orig_input_str << "\t\tMatch and advance\n";
            parsing_stack.pop();
            input_pos++;
            error_mode = false;
            recovery_attempt = false;
        }
        else if (isTerminal(stack_top)) {
            // Terminal mismatch - error
            cout << step << "\t" << stack_str << "\t\t" << orig_input_str << "\t\tError: Expected '" << stack_top
                << "', found '" << original_tokens[input_pos] << "'\n";

            // Error recovery: Pop the expected terminal from stack (skip it)
            cout << "Line " << line_number << ": Syntax Error: Expected '" << stack_top << "' before '"
                << original_tokens[input_pos] << "'\n";
            parsing_stack.pop();
            error_count++;
            error_mode = true;
        }
        else {
            // Non-terminal on top of stack
            if (parse_table.find(stack_top) != parse_table.end() &&
                parse_table[stack_top].find(current_input) != parse_table[stack_top].end()) {

                // Production exists - expand
                string production = parse_table[stack_top][current_input];
                cout << step << "\t" << stack_str << "\t\t" << orig_input_str << "\t\tExpand: " << stack_top << " -> " << production << "\n";

                parsing_stack.pop(); // Remove the non-terminal

                // Parse the production right-hand side
                vector<string> rhs = tokenizeProduction(production);

                // Push RHS in reverse order onto the stack
                for (int i = rhs.size() - 1; i >= 0; i--) {
                    if (rhs[i] != "?" && rhs[i] != "ε") { // Skip epsilon
                        parsing_stack.push(rhs[i]);
                    }
                }

                error_mode = false;
                recovery_attempt = false;
            }
            else {
                // No production rule - error
                cout << step << "\t" << stack_str << "\t\t" << orig_input_str << "\t\tError: No production for "
                    << stack_top << " with input '" << original_tokens[input_pos] << "'\n";

                cout << "Line " << line_number << ": Syntax Error: Unexpected token '" << original_tokens[input_pos]
                    << "' for non-terminal '" << stack_top << "'\n";

                    // Error recovery: Try skipping current input token
                    if (!recovery_attempt) {
                        input_pos++;
                        recovery_attempt = true;
                    }
                    else {
                        // If already attempted recovery by skipping input, try popping stack
                        parsing_stack.pop();
                        recovery_attempt = false;
                    }

                    error_count++;
                    error_mode = true;
            }
        }

        step++;

        // If in error mode and we've processed too many steps, break to avoid infinite loop
        if (error_mode && step > tokens.size() * 3) {
            cout << "Too many parsing steps after error - stopping parse for this line\n";
            break;
        }
    }

    // Check if we've successfully parsed the input
    if (parsing_stack.size() == 1 && parsing_stack.top() == "$" && input_pos == tokens.size()) {
        cout << "Line " << line_number << " parsed successfully!\n";
    }
    else if (!error_mode) {
        if (input_pos < tokens.size() - 1) { // Subtract 1 to account for $ we added
            cout << "Line " << line_number << ": Syntax Error: Unexpected trailing input\n";
            error_count++;
        }
        else if (parsing_stack.size() > 1) {
            cout << "Line " << line_number << ": Syntax Error: Unexpected end of input\n";
            error_count++;
        }
    }
}

string string_parser::displayStack(stack<string> stk) {
    // Create a copy of the stack to display it
    string result;
    vector<string> items;

    while (!stk.empty()) {
        items.push_back(stk.top());
        stk.pop();
    }

    reverse(items.begin(), items.end());

    for (const auto& item : items) {
        result += item + " ";
    }

    return result;
}

string string_parser::displayInput(const vector<string>& tokens, size_t pos) {
    string result;
    for (size_t i = pos; i < tokens.size(); i++) {
        result += tokens[i] + " ";
    }
    return result;
}

bool string_parser::isTerminal(const string& symbol) {
    // A symbol is a terminal if it's not in the parse table as a nonterminal
    return parse_table.find(symbol) == parse_table.end();
}

vector<string> string_parser::tokenizeProduction(const string& production) {
    // Split the production rule into tokens
    istringstream iss(production);
    vector<string> tokens;
    string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}