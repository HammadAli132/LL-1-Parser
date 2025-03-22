#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class leftfactoring {
private:
	map<string, vector<string>> &productions;

public:
	leftfactoring(map<string, vector<string>>& prod);
	~leftfactoring();

	void applyLeftFactoring();
};

