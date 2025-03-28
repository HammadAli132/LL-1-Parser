#pragma once
#include "leftfactoring.h"
#include "leftrecursion.h"
#include "ll_1_parser.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

using namespace std;

class cfg {
private:
	leftfactoring *lfc;
	leftrecursion *lfr;
	ll_1_parser *parser;
	string filename;
	unordered_map<string, vector<vector<string>>> productions;
	vector<string> production_lines;

	void preprocessCFG();
	void displayProductions(bool);
	bool readFile();

public:
	cfg(string filename);
	void build();
	~cfg();
};

