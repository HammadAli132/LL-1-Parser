#include "leftrecursion.h"

leftrecursion::leftrecursion(unordered_map<string, vector<string>> &prod) : productions(prod) {}

leftrecursion::~leftrecursion() {
	cout << "Destructing Left Recursion" << endl;
}

void leftrecursion::removeLeftRecursion() {
    vector<string> non_terminals;
    for (const auto& prod : this->productions)
        non_terminals.push_back(prod.first);

    // Process each nonterminal.
    for (const auto& nt : non_terminals) {
        vector<string> prods = this->productions[nt];
        vector<string> beta;  // Group for non-left recursive productions
        string alpha = "";    // Only one left-recursive production expected

        // Separate productions: those with left recursion and those without.
        for (auto& p : prods) {
            // Check if production starts with the nonterminal (left recursion)
            if (p.substr(0, nt.size()) == nt) {
                // Only one left recursive production is assumed
                alpha = p.substr(nt.size());
            }
            else {
                beta.push_back(p);
            }
        }

        // If left recursion was found (i.e. alpha is non-empty)
        if (!alpha.empty()) {
            // Create a new nonterminal (e.g., A' for A)
            string new_NT = nt + "'";
            vector<string> new_beta;         // Updated productions for nt: A -> βA'
            vector<string> new_NT_productions; // Productions for new nonterminal A'

            // For each non-left recursive production β, add the new nonterminal to the end.
            for (auto& b : beta) {
                new_beta.push_back(b + new_NT);
            }
            // For the left recursive production, remove the left recursion,
            // and add A' at the end. Also include epsilon to allow termination.
            new_NT_productions.push_back(alpha + new_NT);
            new_NT_productions.push_back("ε");

            // Update the original nonterminal's productions and add the new nonterminal.
            this->productions[nt] = new_beta;
            this->productions[new_NT] = new_NT_productions;
        }
        // Else, no left recursion was found; leave productions unchanged.
    }
}