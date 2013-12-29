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
			if( !e.closed() ) {
				m.printActiveNotes( e.activeProj(), e.getActiveNotes() );
			}
		}


		if( c.is("ts") ) e.printTimeSeries();
		if( c.is("q") ) {
			e.queryTag( c.getArgs() );
		}


		if( c.is("a") && c.getArgsVs().size()==0 ) {
			if( e.closed() ) {
				m.info("No project currently active.");
				throw 0;
			}
			e.addEntry("");
			if( !j.addEntry( e.getLastLine() ) ) {
				m.error("Writing to journal file.");
				throw 255;
			}
			printf("[Adding the closing tag]\n");
		}

		if( c.is("a") && c.getArgsVs().size()>0 ) {
			if( e.addEntry(c.getArgs()) ) {
				if( !j.addEntry( e.getLastLine() ) ) {
					m.error("Writing to journal file.");
					throw 255;
				}
				printf("[Switching project] %s\n", e.activeProj().c_str());
//				printf("%s[Add]%s %02d:%02d %s\n", BOLDGREEN, RESET,DTime::getHour(DTime::now()), DTime::getMinute(DTime::now()), args[0].c_str());
			} else {
				throw 255;
			}
		}

		if( c.is("n") && c.getArgsVs().size()>0 ) {
			if( c.getArgsVs().size()==0 ) {
				m.info("Skipping empty note.");
				throw 0;
			}
			string active_proj=e.activeProj();
			if( e.addEntry(c.getArgs()) ) {
				if( !j.addEntry( e.getLastLine() ) ) {  m.error("Writing to journal file."); throw 255; }
			} else {
				throw 255;
			}
			e.addEntry(active_proj);
			if( !j.addEntry( e.getLastLine() ) ) {  m.error("Writing to journal file."); throw 255; }
		}


		if( c.is("t")) j.printTail();

		if( c.is("test") ) {
			Catch::Session s;
			s.run();
		}

	} catch(int& e) {
		return e;
	}

	return 0;
}
