//  copyright (C) 2013-2013 Julian Wergieluk  <julian@wergieluk.com>

#ifndef JOURNAL_H_
#define JOURNAL_H_

#include <vector>
#include <string>

using namespace std;

class Journal {
private:
	string jfile;
	vector<string> entries;
public:

	bool read(const string&);
	const vector<string>& getEntries() const { return entries; };
	bool addEntry(const string&);
	bool detagEntry(const double) { return true; };
	bool closeProject();

	void printTail();
};


#endif /* JOURNAL_H_ */
