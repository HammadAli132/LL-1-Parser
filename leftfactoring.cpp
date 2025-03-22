#include "leftfactoring.h"

leftfactoring::leftfactoring(unordered_map<string, vector<vector<string>>> &prod) : productions(prod) {}

leftfactoring::~leftfactoring() {
	cout << "Destructing Left Factoring" << endl;
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

// Updated left factoring function.
void leftfactoring::applyLeftFactoring() {
    // Collect a list of nonterminals to iterate over.
    vector<string> non_terminals;
    for (const auto& entry : productions)
        non_terminals.push_back(entry.first);

    // Process each nonterminal.
    for (const auto& nt : non_terminals) {
        vector<vector<string>> prods = productions[nt];
        // Sort production alternatives lexicographically.
        sort(prods.begin(), prods.end(), [](const vector<string>& a, const vector<string>& b) {
            // Compare by joining tokens with a space.
            string sa, sb;
            for (const auto& s : a) { sa += s + " "; }
            for (const auto& s : b) { sb += s + " "; }
            return sa < sb;
            });

        vector<vector<string>> new_prods; // Updated productions for nt.
        size_t i = 0;
        string currentNT = nt; // Base nonterminal.

        while (i < prods.size()) {
            // Start a new group using the i-th alternative.
            vector<string> group_common = prods[i];  // Initially, the whole alternative.
            vector<vector<string>> group;
            group.push_back(prods[i]);
            size_t j = i + 1;
            // Extend the group with alternatives that share a common prefix.
            while (j < prods.size()) {
                vector<string> new_common = commonPrefixTokens(group_common, prods[j]);
                if (!new_common.empty()) {
                    group_common = new_common;
                    group.push_back(prods[j]);
                    j++;
                }
                else {
                    break;
                }
            }

            // If more than one production shares the common prefix and the prefix is not the entire production:
            if (group.size() > 1 && group_common.size() < group[0].size()) {
                // Generate a new nonterminal by appending a backtick.
                string new_non_terminal = currentNT + "`";

                vector<vector<string>> newNT_productions; // Productions for the new nonterminal.
                // For each production in the group, extract the suffix (after the common prefix).
                for (const auto& prod : group) {
                    vector<string> suffix;
                    if (prod.size() > group_common.size()) {
                        suffix.assign(prod.begin() + group_common.size(), prod.end());
                    }
                    // If the suffix is empty, use epsilon.
                    if (suffix.empty())
                        suffix.push_back("?");
                    newNT_productions.push_back(suffix);
                }
                // For the original nonterminal, create a factored production:
                // group_common followed by the new nonterminal.
                vector<string> factored = group_common;
                factored.push_back(new_non_terminal);
                new_prods.push_back(factored);

                // Save the new nonterminal's productions.
                productions[new_non_terminal] = newNT_productions;
            }
            else {
                // Group contains a single production; add it unchanged.
                new_prods.push_back(group[0]);
            }
            i = j;
        }
        // Update the productions for nt.
        productions[nt] = new_prods;
    }

    // Optionally, display the left factored productions.
    cout << "Left Factored Productions:" << endl;
    for (const auto& entry : productions) {
        cout << entry.first << " -> ";
        for (size_t i = 0; i < entry.second.size(); ++i) {
            // Join the tokens of each production alternative.
            for (size_t j = 0; j < entry.second[i].size(); ++j) {
                cout << entry.second[i][j];
                if (j != entry.second[i].size() - 1)
                    cout << " ";
            }
            if (i != entry.second.size() - 1)
                cout << " | ";
        }
        cout << endl;
    }
    cout << endl;
}