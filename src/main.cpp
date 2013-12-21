//  copyright (C) 2012-2013 Julian Wergieluk  <julian@wergieluk.com>

using namespace std;

#include "Events.h"
#include "DTime.h"
#include "Cmd.h"


int main(int ac, char** av) {
	try {
		Cmd c(ac, av);

		string journal_file = "/home/julian/.journal"; //"/home/julian/distributed/workspace/j-test.txt";
		Events e;
		e.readJournal(journal_file);

		if( c.is("")  ) {
			e.tagsForToday();
			e.tagsForLastNDays(7);
			e.tagsForLastNDays(30);
		}


		if( c.is("ts") ) e.printTimeSeries();
		if( c.is("q") ) e.queryTag( c.getArgs() );


		if( c.is("a") && c.getArgsVs().size()==0 ) e.close(journal_file);
		if( c.is("a") && c.getArgsVs().size()>0 ) e.add(journal_file, c.getArgsVs());
		// TODO add close tag

		// TODO monthly, weekly statistics



	} catch(int& e) {
		return e;
	}
	return 0;
}
