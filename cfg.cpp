#include "cfg.h"
#include "stringParser.h" // Include the new header

cfg::cfg(string filename) {
	this->filename = filename;
	this->lfc = nullptr;
	this->lfr = nullptr;
	this->parser = nullptr;
	this->production_lines.assign(0, "");
}

cfg::~cfg() {
	delete(this->lfc);
	delete(this->lfr);
	delete(this->parser);
}

// Helper function to trim whitespace from both ends of a string.
static inline string trim(const string& s) {
	size_t start = s.find_first_not_of(" \t\r\n");
	if (start == string::npos)
		return "";
	size_t end = s.find_last_not_of(" \t\r\n");
	return s.substr(start, end - start + 1);
}

void cfg::preprocessCFG() {
	for (const auto& prod : this->production_lines) {
		// Using " -> " (space-tilde-space) as the delimiter to separate the nonterminal and the RHS.
		size_t arrowPos = prod.find(" -> ");
		if (arrowPos == string::npos)
			continue;  // Skip if format is unexpected.

		string non_terminal = trim(prod.substr(0, arrowPos));
		string rhs = trim(prod.substr(arrowPos + 4)); // Skip over " -> "

		// Now, split the RHS on " | " (space-pipe-space) to get individual alternatives.
		vector<vector<string>> alternatives;
		size_t start = 0;
		while (true) {
			size_t pipePos = rhs.find(" | ", start);
			string alt;
			if (pipePos != string::npos) {
				alt = rhs.substr(start, pipePos - start);
				start = pipePos + 3; // Skip over " | "
			}
			else {
				alt = rhs.substr(start);
			}
			alt = trim(alt);
			// Tokenize the alternative production by splitting on whitespace.
			istringstream iss(alt);
			vector<string> tokens;
			string token;
			while (iss >> token)
				tokens.push_back(token);
			alternatives.push_back(tokens);

			if (pipePos == string::npos)
				break;
		}

		// Insert or append the alternatives into the productions map.
		bool inserted = false;
		for (auto& prod : this->productions) {
			if (prod.first == non_terminal) {
				for (const auto& alt : alternatives)
					prod.second.push_back(alt);
				inserted = true;
				break;
			}
		}
		if (!inserted)
			this->productions.push_back(make_pair(non_terminal, alternatives));
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
		if (!line.empty())
			this->production_lines.push_back(line);

	file.close();

	return true;
}

void cfg::displayProductions(bool flag = false) {
	if (flag)
		cout << "Initial Productions:" << endl;
	else
		cout << "Final Productions:" << endl;

	// Iterate over the map where each key is a nonterminal and the value is a vector of tokenized productions.
	for (const auto& entry : this->productions) {
		cout << entry.first << " -> ";
		for (size_t i = 0; i < entry.second.size(); ++i) {
			// Each production alternative is a vector<string>.
			for (size_t j = 0; j < entry.second[i].size(); ++j) {
				cout << entry.second[i][j];
				if (j != entry.second[i].size() - 1)
					cout << " "; // Separate tokens with a space.
			}
			if (i != entry.second.size() - 1)
				cout << " | "; // Separate alternatives with a vertical bar.
		}
		cout << endl;
	}
	cout << endl;
}

void cfg::build() {
	if (!this->readFile()) return;
	this->preprocessCFG();
	this->displayProductions(true);
	this->lfc = new leftfactoring(this->productions);
	this->lfc->applyLeftFactoring();
	this->lfr = new leftrecursion(this->productions);
	this->lfr->removeLeftRecursion();
	this->parser = new ll_1_parser(this->productions);
	this->parser->parse();
	this->displayProductions();
	this->parser->display();
}

// New method to parse input string using the parsing table
bool cfg::parseInputStrings(const string& input_filename) {
	if (!this->parser) {
		cerr << "Error: Parser not initialized. Call build() first." << endl;
		return false;
	}

	// Get the parse table from the LL(1) parser
	auto parse_table = this->parser->getParseTable();
	string start_symbol = this->parser->getStartSymbol();

	// Create a string parser
	string_parser strParser(parse_table, start_symbol, input_filename);

	// Parse all strings in the input file
	return strParser.parseInputFile();
}