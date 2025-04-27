#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

class leftrecursion {
private:
	vector<pair<string, vector<vector<string>>>>& productions;

public:
	leftrecursion(vector<pair<string, vector<vector<string>>>>& prod);
	~leftrecursion();

	void removeLeftRecursion();
};

