#include "ll_1_parser.h"

ll_1_parser::ll_1_parser(unordered_map<string, vector<vector<string>>> prod) : productions(prod) {}

ll_1_parser::~ll_1_parser() {
	cout << "Destructing LL(1) Parser" << endl;
}

// Helper function to compute the longest common prefix between two token vectors.
static vector<string> commonPrefixTokens(const vector<string>& t1, const vector<string>& t2) {
    vector<string> result;
    size_t n = min(t1.size(), t2.size());
    for (size_t i = 0; i < n; ++i) {
        if (t1[i] == t2[i])
            result.push_back(t1[i]);
        else
            break;
    }
    return result;
}

// Updated computeFirstForTokens uses productions of type unordered_map<string, vector<vector<string>>>
static set<string> computeFirstForTokens(
    const vector<string>& tokens,
    const unordered_map<string, vector<vector<string>>>& productions,
    const unordered_map<string, set<string>>& first_sets)
{
    set<string> result;
    bool allHaveEpsilon = true;
    for (const auto& symbol : tokens) {
        // If symbol is terminal (i.e. not a key in productions), add it and stop.
        if (productions.find(symbol) == productions.end()) {
            result.insert(symbol);
            allHaveEpsilon = false;
            break;
        }
        else {
            // symbol is nonterminal: add FIRST(symbol) except epsilon.
            for (const auto& s : first_sets.at(symbol)) {
                if (s != "?")
                    result.insert(s);
            }
            // If the FIRST set for this nonterminal does not contain epsilon, stop.
            if (first_sets.at(symbol).find("?") == first_sets.at(symbol).end()) {
                allHaveEpsilon = false;
                break;
            }
        }
    }
    if (allHaveEpsilon)
        result.insert("?");
    return result;
}

void ll_1_parser::constructFirstSets() {
    // Initialize FIRST sets for every nonterminal.
    for (const auto& prod : this->productions)
        this->first_sets[prod.first] = set<string>();

    bool changed = true;
    while (changed) {
        changed = false;
        // For each nonterminal A.
        for (const auto& entry : productions) {
            string A = entry.first;
            // For each production alternative A -> α (already tokenized).
            for (const auto& production : entry.second) {
                bool allCanBeEpsilon = true;
                for (const auto& symbol : production) {
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
                        // symbol is nonterminal: add FIRST(symbol) except epsilon.
                        for (const auto& s : first_sets[symbol]) {
                            if (s != "?" && first_sets[A].find(s) == first_sets[A].end()) {
                                first_sets[A].insert(s);
                                changed = true;
                            }
                        }
                        if (first_sets[symbol].find("?") == first_sets[symbol].end()) {
                            allCanBeEpsilon = false;
                            break;
                        }
                    }
                }
                if (allCanBeEpsilon && first_sets[A].find("?") == first_sets[A].end()) {
                    first_sets[A].insert("?");
                    changed = true;
                }
            }
        }
    }
}

void ll_1_parser::constructFollowSets() {
    // Initialize FOLLOW sets for every nonterminal.
    for (const auto& prod : this->productions)
        this->follow_sets[prod.first] = set<string>();

    // Assume the start symbol is the first nonterminal; add '$' to its FOLLOW set.
    if (!productions.empty())
        follow_sets.begin()->second.insert("$");

    bool changed = true;
    while (changed) {
        changed = false;
        // For each production A -> α.
        for (const auto& entry : productions) {
            string A = entry.first;
            for (const auto& production : entry.second) {
                for (size_t i = 0; i < production.size(); i++) {
                    string symbol = production[i];
                    // Process only if symbol is a nonterminal.
                    if (productions.find(symbol) != productions.end()) {
                        bool allSuffixCanBeEpsilon = true;
                        // Examine the remainder β after symbol.
                        for (size_t j = i + 1; j < production.size(); j++) {
                            string nextSymbol = production[j];
                            // If terminal, add it to FOLLOW(symbol).
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
                                    if (s != "?" && follow_sets[symbol].find(s) == follow_sets[symbol].end()) {
                                        follow_sets[symbol].insert(s);
                                        changed = true;
                                    }
                                }
                                if (first_sets[nextSymbol].find("?") == first_sets[nextSymbol].end()) {
                                    allSuffixCanBeEpsilon = false;
                                    break;
                                }
                            }
                        }
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
            // production is already tokenized (vector<string>).
            set<string> firstAlpha = computeFirstForTokens(production, productions, first_sets);
            // Convert production tokens back to a string for the parse table.
            string productionStr;
            for (size_t i = 0; i < production.size(); i++) {
                productionStr += production[i] + (i < production.size() - 1 ? " " : "");
            }
            for (const auto& terminal : firstAlpha) {
                if (terminal != "?")
                    parse_table[A][terminal] = productionStr;
            }
            if (firstAlpha.find("?") != firstAlpha.end()) {
                for (const auto& terminal : follow_sets[A]) {
                    parse_table[A][terminal] = productionStr;
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