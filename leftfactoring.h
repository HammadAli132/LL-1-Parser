#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

class leftfactoring {
private:
	vector<pair<string, vector<vector<string>>>>& productions;

public:
	leftfactoring(vector<pair<string, vector<vector<string>>>>& prod);
	~leftfactoring();

	void applyLeftFactoring();
};

