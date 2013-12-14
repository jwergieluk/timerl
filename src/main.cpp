//  copyright (C) 2012-2013 Julian Wergieluk  <julian@wergieluk.com>

using namespace std;

#include "Events.h"
#include "DTime.h"
#include "Cmd.h"


int main(int ac, char** av) {
	try {
		Cmd c(ac, av);

		Events e;
		e.readJournal("/home/julian/.journal");
		e.tagsForToday();
		e.tagsForLastNDays(7);
		e.tagsForLastNDays(30);

	} catch(int& e) {
		return e;
	}
	return 0;
}
