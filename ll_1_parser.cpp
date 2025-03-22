#include "ll_1_parser.h"

ll_1_parser::ll_1_parser(map<string, vector<string>> prod) : productions(prod) {}

ll_1_parser::~ll_1_parser() {
	cout << "Destructing LL(1) Parser" << endl;
}

static vector<string> tokenizeProduction(const string& production) {
    vector<string> tokens;
    size_t i = 0;
    while (i < production.size()) {
        char c = production[i];
        // If uppercase, assume a nonterminal.
        if (isupper(c)) {
            string token;
            token.push_back(c);
            i++;
            // Append subsequent apostrophes or tildes.
            while (i < production.size() && (production[i] == '\'' || production[i] == '`')) {
                token.push_back(production[i]);
                i++;
            }
            tokens.push_back(token);
        }
        else {
            // Otherwise, assume it's a terminal (single lowercase character).
            tokens.push_back(string(1, c));
            i++;
        }
    }
    return tokens;
}

static set<string> computeFirstForTokens(const vector<string>& tokens, const map<string, vector<string>>& productions, const map<string, set<string>>& first_sets) {
    set<string> result;
    bool allHaveEpsilon = true;

    for (const auto& symbol : tokens) {
        // If the symbol is terminal (i.e. not found as a key), add it and stop.
        if (productions.find(symbol) == productions.end()) {
            result.insert(symbol);
            allHaveEpsilon = false;
            break;
        }
        else {
            // Symbol is nonterminal: add FIRST(symbol) except epsilon.
            for (const auto& s : first_sets.at(symbol)) {
                if (s != "ε")
                    result.insert(s);
            }
            // If the FIRST set of this nonterminal does not contain epsilon, stop.
            if (first_sets.at(symbol).find("ε") == first_sets.at(symbol).end()) {
                allHaveEpsilon = false;
                break;
            }
        }
    }
    if (allHaveEpsilon)
        result.insert("ε");
    return result;
}

void ll_1_parser::constructFirstSets() {
    // Initialize FIRST sets for every nonterminal.
    for (const auto& prod : this->productions)
        this->first_sets[prod.first] = set<string>();

    bool changed = true;
    // Iteratively update FIRST sets until no changes occur.
    while (changed) {
        changed = false;
        // For each nonterminal A.
        for (const auto& entry : productions) {
            string A = entry.first;
            // For each production A -> α.
            for (const auto& production : entry.second) {
                vector<string> tokens = tokenizeProduction(production);
                bool allCanBeEpsilon = true;
                // Process tokens one by one.
                for (const auto& symbol : tokens) {
                    // If symbol is terminal.
                    if (productions.find(symbol) == productions.end()) {
                        if (first_sets[A].find(symbol) == first_sets[A].end()) {
                            first_sets[A].insert(symbol);
                            changed = true;
                        }
                        allCanBeEpsilon = false;
                        break;
                    }
                    else {
                        // Symbol is nonterminal: add FIRST(symbol) except epsilon.
                        for (const auto& s : first_sets[symbol]) {
                            if (s != "ε" && first_sets[A].find(s) == first_sets[A].end()) {
                                first_sets[A].insert(s);
                                changed = true;
                            }
                        }
                        // If symbol’s FIRST set does not contain epsilon, stop.
                        if (first_sets[symbol].find("ε") == first_sets[symbol].end()) {
                            allCanBeEpsilon = false;
                            break;
                        }
                    }
                }
                // If all tokens can derive epsilon, add epsilon.
                if (allCanBeEpsilon) {
                    if (first_sets[A].find("ε") == first_sets[A].end()) {
                        first_sets[A].insert("ε");
                        changed = true;
                    }
                }
            }
        }
    }
}

void ll_1_parser::constructFollowSets() {
    // Initialize FOLLOW sets for every nonterminal.
    for (const auto& prod : this->productions)
        this->follow_sets[prod.first] = set<string>();

    // Assume the start symbol is the first nonterminal in the map.
    // Add the end-of-input marker '$' to its FOLLOW set.
    if (!productions.empty())
        follow_sets.begin()->second.insert("$");

    bool changed = true;
    while (changed) {
        changed = false;
        // For each production A -> α.
        for (const auto& entry : productions) {
            string A = entry.first;
            for (const auto& production : entry.second) {
                vector<string> tokens = tokenizeProduction(production);
                for (size_t i = 0; i < tokens.size(); i++) {
                    string symbol = tokens[i];
                    // Process only if symbol is a nonterminal.
                    if (productions.find(symbol) != productions.end()) {
                        bool allSuffixCanBeEpsilon = true;
                        // Consider the remainder β after symbol.
                        for (size_t j = i + 1; j < tokens.size(); j++) {
                            string nextSymbol = tokens[j];
                            // If terminal, add it directly.
                            if (productions.find(nextSymbol) == productions.end()) {
                                if (follow_sets[symbol].find(nextSymbol) == follow_sets[symbol].end()) {
                                    follow_sets[symbol].insert(nextSymbol);
                                    changed = true;
                                }
                                allSuffixCanBeEpsilon = false;
                                break;
                            }
                            else {
                                // Add FIRST(nextSymbol) except epsilon.
                                for (const auto& s : first_sets[nextSymbol]) {
                                    if (s != "ε" && follow_sets[symbol].find(s) == follow_sets[symbol].end()) {
                                        follow_sets[symbol].insert(s);
                                        changed = true;
                                    }
                                }
                                // If nextSymbol’s FIRST does not include epsilon, stop.
                                if (first_sets[nextSymbol].find("ε") == first_sets[nextSymbol].end()) {
                                    allSuffixCanBeEpsilon = false;
                                    break;
                                }
                            }
                        }
                        // If every symbol in the suffix can derive epsilon, add FOLLOW(A) to FOLLOW(symbol).
                        if (allSuffixCanBeEpsilon) {
                            for (const auto& s : follow_sets[A]) {
                                if (follow_sets[symbol].find(s) == follow_sets[symbol].end()) {
                                    follow_sets[symbol].insert(s);
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void ll_1_parser::constructParseTable() {
    // For each production A -> α.
    for (const auto& entry : productions) {
        string A = entry.first;
        for (const auto& production : entry.second) {
            vector<string> tokens = tokenizeProduction(production);
            // Compute FIRST(α) for the tokenized production.
            set<string> firstAlpha = computeFirstForTokens(tokens, productions, first_sets);
            // For every terminal in FIRST(α) except epsilon, add the production to the table.
            for (const auto& terminal : firstAlpha) {
                if (terminal != "ε")
                    parse_table[A][terminal] = production;
            }
            // If FIRST(α) contains epsilon, add the production for every terminal in FOLLOW(A).
            if (firstAlpha.find("ε") != firstAlpha.end()) {
                for (const auto& terminal : follow_sets[A]) {
                    parse_table[A][terminal] = production;
                }
            }
        }
    }
}

void ll_1_parser::display() {
    // Display FIRST Sets.
    cout << "FIRST Sets:" << endl;
    for (const auto& entry : first_sets) {
        cout << "FIRST(" << entry.first << ") = { ";
        for (const auto& s : entry.second)
            cout << s << " ";
        cout << "}" << endl;
    }
    cout << endl;

    // Display FOLLOW Sets.
    cout << "FOLLOW Sets:" << endl;
    for (const auto& entry : follow_sets) {
        cout << "FOLLOW(" << entry.first << ") = { ";
        for (const auto& s : entry.second)
            cout << s << " ";
        cout << "}" << endl;
    }
    cout << endl;

    // Display Parse Table.
    cout << "Parse Table:" << endl;
    for (const auto& entry : parse_table) {
        string A = entry.first;
        cout << "For " << A << ":" << endl;
        for (const auto& mapping : entry.second) {
            cout << "\t" << mapping.first << " : " << mapping.second << endl;
        }
    }
    cout << endl;
}

void ll_1_parser::parse() {
    this->constructFirstSets();
    this->constructFollowSets();
    this->constructParseTable();
}