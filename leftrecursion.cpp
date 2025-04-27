#include "leftrecursion.h"

leftrecursion::leftrecursion(vector<pair<string, vector<vector<string>>>>& prod) : productions(prod) {}

leftrecursion::~leftrecursion() {
	cout << "Destructing Left Recursion" << endl;
}

void leftrecursion::removeLeftRecursion() {
    // Collect all nonterminals from the productions map.
    vector<string> non_terminals;
    for (const auto& prod : this->productions)
        non_terminals.push_back(prod.first);

    // Process each nonterminal.
    for (const auto& nt : non_terminals) {
        // Get productions for nt (each production is now a vector<string>).
        vector<vector<string>> prods;
        for (const auto& prod : this->productions) {
            if (prod.first == nt) {
                prods = prod.second;
                break;
            }
        }
        vector<vector<string>> beta;      // Group for non-left recursive productions.
        vector<string> alpha;             // Left recursive production suffix (only one expected).

        // Separate productions: those with left recursion and those without.
        for (const auto& p : prods) {
            // Check if the first token equals the nonterminal.
            if (!p.empty() && p[0] == nt) {
                // Left recursive production found.
                // Remove the nonterminal from the beginning to obtain α.
                if (p.size() > 1)
                    alpha = vector<string>(p.begin() + 1, p.end());
                else
                    alpha.clear(); // If no token remains, alpha remains empty.
            }
            else {
                beta.push_back(p);
            }
        }

        // If left recursion was found (i.e. alpha is non-empty).
        if (!alpha.empty()) {
            // Create a new nonterminal, e.g., A' for A.
            string new_NT = nt + "'";
            vector<vector<string>> new_beta;         // Updated productions for nt: A -> β A'
            vector<vector<string>> new_NT_productions; // Productions for new nonterminal A'

            // For each non-left recursive production β, append the new nonterminal.
            for (auto& b : beta) {
                vector<string> production = b;
                production.push_back(new_NT);
                new_beta.push_back(production);
            }

            // For the left recursive production, remove left recursion:
            // Create a production for A' of the form: α A'
            vector<string> production_alpha = alpha;
            production_alpha.push_back(new_NT);
            new_NT_productions.push_back(production_alpha);
            // Also add an epsilon production for termination.
            new_NT_productions.push_back(vector<string>{"?"});

            // Update the productions for the original nonterminal.
            for (auto& prod : this->productions) {
                if (prod.first == nt) {
                    prod.second = new_beta;
                    break;
                }
            }
            // Add the new nonterminal and its productions.
            for (auto& prod : this->productions) {
                if (prod.first == new_NT) {
                    prod.second = new_NT_productions;
                    break;
                }
            }
        }
        // If no left recursion was found, leave productions unchanged.
    }
}