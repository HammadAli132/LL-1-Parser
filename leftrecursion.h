#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class leftrecursion {
private:
	map<string, vector<string>>& productions;

public:
	leftrecursion(map<string, vector<string>>& prod);
	~leftrecursion();

	void removeLeftRecursion();
};

