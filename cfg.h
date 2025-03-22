#pragma once
#include "leftfactoring.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

using namespace std;

class cfg {
private:
	leftfactoring *lfc;
	string filename;
	map<string, vector<string>> productions;
	vector<string> production_lines;

	void preprocessCFG();
	void displayProductions();
	void displayParsingTable();
	void displayFirstSet();
	void displayFollowSet();
	bool readFile();

public:
	cfg(string filename);
	void build();
	~cfg();
};

