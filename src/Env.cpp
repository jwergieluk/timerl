//  copyright (C) 2013-2014 Julian Wergieluk  <julian@wergieluk.com>

#include "Env.h"
#include <cstdlib>



void Env::editJournal(const string& arg) {
	std::system((string("vim ") + arg).c_str());
}




