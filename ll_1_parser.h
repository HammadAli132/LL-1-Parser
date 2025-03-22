#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

class ll_1_parser {
private:
	map<string, vector<string>> productions;
	map<string, set<string>> first_sets;
	map<string, set<string>> follow_sets;
	map<string, map<string, string>> parse_table;

	void constructFirstSets();
	void constructFollowSets();
	void constructParseTable();

public:
	ll_1_parser(map<string, vector<string>>);
	~ll_1_parser();
	void parse();
	void display();
};

