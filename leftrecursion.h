#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class leftrecursion {
private:
	unordered_map<string, vector<string>>& productions;

public:
	leftrecursion(unordered_map<string, vector<string>>& prod);
	~leftrecursion();

	void removeLeftRecursion();
};

