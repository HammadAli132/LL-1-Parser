#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>

using namespace std;

class ll_1_parser {
private:
	unordered_map<string, vector<vector<string>>> productions;
	unordered_map<string, set<string>> first_sets;
	unordered_map<string, set<string>> follow_sets;
	unordered_map<string, unordered_map<string, string>> parse_table;

	void constructFirstSets();
	void constructFollowSets();
	void constructParseTable();

public:
	ll_1_parser(unordered_map<string, vector<vector<string>>> prod);
	~ll_1_parser();
	void parse();
	void display();
};

