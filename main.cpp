#include "cfg.h"
#include <iostream>
using namespace std;

int main() {
	string filename = "D:\\VS Projects\\CC Assignment 2\\cfg4.txt";

	cfg *cfgObj = new cfg(filename);
	cfgObj->build();
	delete(cfgObj);

	return 0;
}