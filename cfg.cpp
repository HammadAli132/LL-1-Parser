#include "cfg.h"

cfg::cfg(string filename) {
	this->filename = filename;
	this->lfc = nullptr;
	this->lfr = nullptr;
	this->production_lines.assign(0, "");
}

cfg::~cfg() {
	delete(this->lfc);
	delete(this->lfr);
}

void cfg::preprocessCFG() {
	for (const auto &prod : this->production_lines) {
		if (prod.size() < 4) continue;

		char delimiter = '~';
		istringstream iss1(prod);
		string non_terminal, production;
		getline(iss1, non_terminal, delimiter);
		getline(iss1, production, delimiter);

		delimiter = '|';
		istringstream iss2(production);
		string token;
		vector<string> tokens;

		while (getline(iss2, token, delimiter)) // splitting the production on '|'
			tokens.push_back(token);

		if (this->productions.find(non_terminal) != this->productions.end()) {
			// appending the production to non-terminal
			for (const auto &t : tokens)
				this->productions[non_terminal].push_back(t);
		}
		else {
			// adding a new non-terminal with its production
			this->productions.emplace(non_terminal, tokens);
		}
	}	
}

bool cfg::readFile() {
	ifstream file(this->filename, ios::in);
	string line;

	if (!file) {
		cerr << "Error while opening the CFG file" << endl;
		return false;
	}

	while (getline(file, line))
		this->production_lines.push_back(line);
	
	file.close();

	return true;
}

void cfg::displayFirstSet() {

}

void cfg::displayFollowSet() {

}

void cfg::displayParsingTable() {

}

void cfg::displayProductions() {
	cout << "Productions:" << endl;
	// Iterate over the map where each key is a non-terminal and the value is a vector of productions.
	for (const auto& entry : this->productions) {
		cout << entry.first << " -> ";
		for (size_t i = 0; i < entry.second.size(); ++i) {
			cout << entry.second[i];
			if (i != entry.second.size() - 1)
				cout << "|";
		}
		cout << endl;
	}
}

void cfg::build() {
	if (!this->readFile()) return;
	this->preprocessCFG();
	this->lfc = new leftfactoring(this->productions);
	this->lfc->applyLeftFactoring();
	this->lfr = new leftrecursion(this->productions);
	this->lfr->removeLeftRecursion();
	this->displayProductions();
}