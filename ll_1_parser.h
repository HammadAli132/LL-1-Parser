#ifndef LL_1_PARSER_H
#define LL_1_PARSER_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

class ll_1_parser {
private:
	vector<pair<string, vector<vector<string>>>> productions;
	unordered_map<string, set<string>> first_sets;
	unordered_map<string, set<string>> follow_sets;
	unordered_map<string, unordered_map<string, string>> parse_table;

public:
	ll_1_parser(vector<pair<string, vector<vector<string>>>> prod);
	~ll_1_parser();

	void constructFirstSets();
	void constructFollowSets();
	void constructParseTable();
	void parse();
	void display();

	// New methods to access parser components
	unordered_map<string, unordered_map<string, string>> getParseTable() const {
		return parse_table;
	}

	string getStartSymbol() const {
		// Assuming the start symbol is the first production in the grammar
		if (!productions.empty()) {
			return productions.begin()->first;
		}
		return "";
	}
};

#endif