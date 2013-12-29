//  copyright (C) 2013-2013 Julian Wergieluk  <julian@wergieluk.com>

#include <cstdio>
#include "io.h"
#include "DTime.h"

#include "Journal.h"


bool Journal::read(const string& file) {
	jfile=file;
	return Io::file2vs(file.c_str(), entries);
};

bool Journal::addEntry(const string& args) {
	FILE* out;
	if ((out = fopen( jfile.c_str(), "a"))==NULL) return false;

	char buf[500];
	snprintf(buf, 500, "%s\n", args.c_str() );
	if( fprintf(out, "%s", buf) <0) { fclose(out); return false; }
	fclose(out);

	return true;
}

void Journal::printTail() {
	printf("# tail -n 7 %s\n", jfile.c_str());
	for(auto i=max((int)0, (int)(entries.size()-7)); i<(int)entries.size(); i++) printf("%s\n", entries[i].c_str());
}
