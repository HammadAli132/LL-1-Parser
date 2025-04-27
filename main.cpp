#include "cfg.h"
#include <iostream>
using namespace std;

int main() {
	string cfg_filename = "D:\\VS Projects\\CC Assignment 2\\cfg10.txt";
	string input_filename = "D:\\VS Projects\\CC Assignment 2\\input_strings.txt";

	cfg* cfgObj = new cfg(cfg_filename);
	cfgObj->build();

	cout << "\n==========================================\n";
	cout << "Starting to parse input strings...\n";
	cfgObj->parseInputStrings(input_filename);

	delete(cfgObj);

	return 0;
}