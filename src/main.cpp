//  copyright (C) 2012-2013 Julian Wergieluk  <julian@wergieluk.com>

using namespace std;

#include "Journal.h"
#include "Msg.h"
#include "Events.h"
#include "DTime.h"
#include "Env.h"
#include "Cmd.h"

#ifdef UNIT_TESTS
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#endif

int main(int ac, char** av) {
	try {
		Cmd c(ac, av);

		// TODO read this from environment
		string journal_file = "/home/julian/.journal";

		Msg m;
		Journal j;
		if( !j.read(journal_file) ) {
			m.error("Unable to read the journal file.");
			throw 255;
		}


		Events e(m);
		e.readJournal(j.getEntries());

		if( c.is("")  ) {
			if( e.isEmpty() ) throw 0;
			if( !e.closed() ) {
				printf("Active project is %s+%s%s.\n", GREEN, e.activeProj().c_str(), RESET);
				printf("  (Use 'a' to deactivate or switch to another project)\n\n");
			}

			vector<string> tags; vector<double> hours;
			printf("Project durations:\n");
			e.tagsForLastNDays(1, tags, hours);
			m.printTags(1, tags, hours);
			e.tagsForLastNDays(2, tags, hours);
			m.printTags(2, tags, hours);
			e.tagsForLastNDays(7, tags, hours);
			m.printTags(7, tags, hours);
			e.tagsForLastNDays(30, tags, hours);
			m.printTags(30, tags, hours);
			if( !e.closed() ) {
				vector<int> id; vector<string> notes;
				e.getActiveNotes(id, notes);
				m.par();
				m.printActiveNotes( e.activeProj(), id, notes );
			}
		}

		if( c.is("ts") ) {
			if( e.isEmpty() ) throw 0;
			e.printTimeSeries();
		}

		if( c.is("q") && c.getArgsVs().size()==0 ) {
			m.error("No project name specified.");
		}
		if( c.is("q") && c.getArgsVs().size()>0 ) {
			if( e.isEmpty() ) throw 0;
			vector<double> dates, hours;
			e.getTs( c.getArgs(), dates, hours );
			m.printTs( c.getArgs(), dates, hours);
//			e.queryTag( c.getArgs() );
		}


		if( c.is("a") && c.getArgsVs().size()==0 ) {
			if( e.closed() ) {
				m.info("No project currently active.");
				throw 0;
			}
			string prev_active = e.activeProj();
			e.addEntry("");
			if( !j.addEntry( e.getLastLine() ) ) {
				m.error("Writing to journal file.");
				throw 255;
			}
			printf("Project %s+%s%s deactivated.\n", GREEN, prev_active.c_str() , RESET);
		}

		if( (c.is("a") || c.is("p") )&& c.getArgsVs().size()>0 ) {
			// TODO check tag name
			if( c.is("a") && !e.tagExists(c.getArgs()) ) {
				m.error("Project tag not found in the journal file. Use command \'p\' to start a new project.");
				throw 255;
			}
			if( c.is("p") && e.tagExists(c.getArgs())) {
				m.error("Project tag already in use. Use command \'a\' to activate.");
				throw 255;
			}
			if( e.addEntry(c.getArgs())) {
				if( !j.addEntry( e.getLastLine() ) ) {
					m.error("Writing to journal file.");
					throw 255;
				}
				printf("Switching to the project %s+%s%s.\n", GREEN, e.activeProj().c_str(), RESET);
			} else {
				throw 255;
			}
		}

		if( c.is("n") && c.getArgsVs().size()>0 ) { // TODO no refs in notes and adds!
			string active_proj=e.activeProj();
			if( e.addNote(c.getArgs()) ) {
				if( !j.addEntry( e.getLastLine() ) ) {  m.error("Writing to journal file."); throw 255; }
			} else {
				throw 255;
			}
//			e.addEntry(active_proj);
//			if( !j.addEntry( e.getLastLine() ) ) {  m.error("Writing to journal file."); throw 255; }
		}
		if( c.is("n") && c.getArgsVs().size()==0 ) {
			vector<int> id; vector<string> notes;
			e.getNotes(id, notes);
			m.printNotes(id, notes );
		}

		if( c.is("k") ) {
			if( c.getArgs().length()==0 ) {
				m.error("Line id not specified.");
				throw 255;
			}
			int id; string killed_line;
			if( !Io::s2i(id, c.getArgs()) ) {
				m.error("Unable to parse the id.");
				throw 255;
			}
			if( !e.kill(id, killed_line) ) {
				m.error("Line not found.");
				throw 255;
			}
			if( !j.addEntry( e.getLastLine() ) ) {
				m.error("Writing to journal file.");
				throw 255;
			}
			m.kill(killed_line);
		}

		if( c.is("t")) j.printTail();

		if( c.is("e") ) {
			Env::editJournal(journal_file);
		}

		if( c.is("debug") ) {
			e.printDebugInfo();
		}

#ifdef UNIT_TESTS
		if( c.is("test") ) {
			Catch::Session s;
			s.run();
		}
#endif

		if( c.is("h")) {
			m.printUsage();
		}

	} catch(int& e) {
		return e;
	}

	return 0;
}
