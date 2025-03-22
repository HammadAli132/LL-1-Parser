#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class leftfactoring {
private:
	unordered_map<string, vector<string>> &productions;

public:
	leftfactoring(unordered_map<string, vector<string>>& prod);
	~leftfactoring();

	void applyLeftFactoring();
};

