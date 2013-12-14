//  copyright (C) 2012-2013 Julian Wergieluk  <julian@wergieluk.com>

using namespace std;

#include "Events.h"
#include "DTime.h"


int main() {


    Events e;
    e.readJournal("/home/julian/.journal");
    e.tagsForToday();
    e.tagsForLastNDays(8);
    e.tagsForLastNDays(31);


	return 0;
}
