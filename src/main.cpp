//  copyright (C) 2012-2013 Julian Wergieluk  <julian@wergieluk.com>

using namespace std;

#include "Journal.h"
#include "Msg.h"
#include "Events.h"
#include "DTime.h"
#include "Cmd.h"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int ac, char** av) {
	try {
		Cmd c(ac, av);

		string journal_file = "/home/julian/distributed/workspace/timerl/test/1-in-a.txt";

		Msg m;
		Journal j;
		if( !j.read(journal_file) ) {
			m.error("ERROR: Unable to read the journal file.");
			throw 255;
		}


		Events e(m);
		e.readJournal(j.getEntries());

		if( c.is("")  ) {
			e.tagsForToday();
			e.tagsForLastNDays(7);
			e.tagsForLastNDays(30);
			e.printActive();
			e.printNotesActive();
			j.printTail();
		}


		if( c.is("ts") ) e.printTimeSeries();
		if( c.is("q") ) {
			e.queryTag( c.getArgs() );
			e.printNotes(c.getArgs());
		}


		if( c.is("a") && c.getArgsVs().size()==0 ) {
			if( e.closed() ) {
				m.info("No project currently active.");
				throw 0;
			}
			if( !j.closeProject() ) {
				m.error("Writing to journal file.");
				throw 255;
			}
			printf("[Adding the closing tag]\n");
		}

		if( c.is("a") && c.getArgsVs().size()>0 ) {
			if( e.addEntry(c.getArgs()) ) {
				if( !j.addEntry(c.getArgs()) ) {
					m.error("ERROR: Writing to journal file.");
					throw 255;
				}
//				printf("%s[Add]%s %02d:%02d %s\n", BOLDGREEN, RESET,DTime::getHour(DTime::now()), DTime::getMinute(DTime::now()), args[0].c_str());
			} else {
				throw 255;
			}
		}

		if( c.is("t")) j.printTail();

	} catch(int& e) {
		return e;
	}

	printf("\n");
	Catch::Session s; s.run();

	return 0;
}
