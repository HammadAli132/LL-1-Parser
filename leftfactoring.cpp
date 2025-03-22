#include "leftfactoring.h"

leftfactoring::leftfactoring(unordered_map<string, vector<string>> &prod) : productions(prod) {}

leftfactoring::~leftfactoring() {
	cout << "Destructing Left Factoring" << endl;
}

// Helper function to compute the longest common prefix between two strings.
static string commonPrefix(const string& s1, const string& s2) {
	size_t n = min(s1.size(), s2.size());
	size_t i = 0;
	while (i < n && s1[i] == s2[i])
		++i;
	return s1.substr(0, i);
}

void leftfactoring::applyLeftFactoring() {
    // Make a list of nonterminals to iterate over.
    vector<string> non_terminals;
    for (const auto& entry : this->productions)
        non_terminals.push_back(entry.first);

    // Process each nonterminal's productions.
    for (string nt : non_terminals) {
        vector<string> prods = this->productions[nt];
        // Sort productions so that those sharing prefixes are adjacent.
        sort(prods.begin(), prods.end());
        vector<string> new_prods; // Will hold the updated productions for nt.
        size_t i = 0;
        string new_NT = nt;
        while (i < prods.size()) {
            // Start a new group with the current production.
            string group_common = prods[i];
            vector<string> group;
            group.push_back(prods[i]);
            size_t j = i + 1;
            // Extend the group with adjacent productions that share a common prefix.
            while (j < prods.size()) {
                string new_common = commonPrefix(group_common, prods[j]);
                if (!new_common.empty()) {
                    group_common = new_common;
                    group.push_back(prods[j]);
                    j++;
                }
                else
                    break;
            }
            // If more than one production shares a common prefix, factor them.
            if (group.size() > 1 && group_common.size() > 0) {
                // Generate a new nonterminal
                string new_non_terminal = new_NT + '`';
                new_NT = new_non_terminal;

                vector<string> new_NT_productions; // Productions for the new nonterminal.
                for (auto& p : group) {
                    string suffix = p.substr(group_common.size());
                    if (suffix.empty())
                        suffix = "ε"; // Use epsilon to denote an empty production.
                    new_NT_productions.push_back(suffix);
                }
                // For the original nonterminal, add the factored production.
                new_prods.push_back(group_common + new_non_terminal);

                // Save the new nonterminal's productions in the map.
                this->productions[new_non_terminal] = new_NT_productions;
            }
            else {
                // Group contains only one production; add it unchanged.
                new_prods.push_back(group[0]);
            }
            i = j;
        }
        // Update the productions for nt with the newly factored productions.
        this->productions[nt] = new_prods;
    }
}