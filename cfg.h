#ifndef CFG_H
#define CFG_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include "leftfactoring.h"
#include "leftrecursion.h"
#include "ll_1_parser.h"

using namespace std;

class cfg {
private:
	string filename;
	vector<string> production_lines;
	vector<pair<string, vector<vector<string>>>> productions;
	leftfactoring* lfc;
	leftrecursion* lfr;
	ll_1_parser* parser;

public:
	cfg(string filename);
	~cfg();

	void preprocessCFG();
	bool readFile();
	void displayProductions(bool flag);
	void build();

	// New method to parse input strings
	bool parseInputStrings(const string& input_filename);
};

#endif