//  copyright (C) 2012-2013 Julian Wergieluk  <julian@wergieluk.com>

#ifndef EVENTS_H_
#define EVENTS_H_

#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include "Msg.h"
#include "io.h"
#include "DTime.h"

#include "config.h"

using namespace std;

class Events {
private:
	Msg& msg;

	// The following vectors have same length equal to the number of lines in
	// the journal file.
	vector<string> journal_raw;    // lines of the journal file

	vector<double> time_vec,		// time stamps of the lines
				ref_vec, 			// time referenced in the line. 0. if no reference present.
				tag_duration_vec;
	vector<int> tag_no_vec, 		// number of tags found in each line
				id_vec;				// lines with tags get extra line number and it is stored here. 0 means no extra number
	vector<string> proj_vec, 		// project names in lines
			line_vec;				// lines without time stamps
	vector<double> proj_len_vec;	// project duration in days counted from current time stamp to the time stamp in th next line


	map< double, int> line_no;		// time stamp  to  line number  map
	map< int, int> id_to_line_no;

	set<string> proj_set;			// set of all project names
	vector<string> ord_projects;	// project names in some fixed order

	vector<double > ts_dates;
	map< string, vector<double> > ts;	// time series of daily project durations. length of ts[proj] is the length of ts_dates

	map< pair<string, double>, double > proj_day_duration;   // (proj_name, day) -> fraction of the day spent on proj

	double now;
	string active_proj;
public:

	Events(Msg& m): msg(m) {
//		ts_start=0.; ts_end=0.;
		now=DTime::now();
	};

	string activeProj() const {
		if( active_proj== NOTHING ) return "";
		return active_proj;
	}

	bool addEntry(const string&);
	bool addNote(const string&);

	bool closed() { return active_proj==NOTHING; }
	bool isEmpty() { return journal_raw.size()==0; }

	void readJournal(const vector<string>&);
	void reset();
	void refreshJournal();

	void getNotes(const string& proj, vector<int>& id, vector<string>& notes) {
		for(auto i=0; i<time_vec.size(); i++) {
			if( proj_vec[i]==proj && tag_no_vec[i]>0 ) {
				id.push_back( id_vec[i] );
				notes.push_back( line_vec[i] );
			}
		}
	}

	void getNotes(vector<int>& id, vector<string>& notes) {
		for(auto i=0; i<time_vec.size(); i++) {
			if( tag_no_vec[i]>0 ) {
				id.push_back( id_vec[i] );
				notes.push_back( line_vec[i] );
			}
		}
	}

	const string getLastLine() {
		if( journal_raw.size()==0 ) {
			return "";
		} else {
			return journal_raw[ journal_raw.size()-1];
		}
	}

	void getActiveNotes(vector<int>& id, vector<string>& notes) {
		getNotes(active_proj, id, notes);
	}

	double queryTag(const string&);
	bool tagExists(const string&);

	void printTimeSeries() {
		printf("#date ");
		for(auto k=0; k<ord_projects.size(); k++) { printf("%s ", ord_projects[k].c_str());  }
		printf("\n");
		for(auto i=0; i<ts_dates.size(); i++) {
			printf("%.0f ", ts_dates[i]);
			for(auto k=0; k<ord_projects.size(); k++) {
				printf("%f ", ts[ord_projects[k]][i]);
			}
			printf("\n");
		}
	}

	bool getTs(const string& proj, vector<double>& dates, vector<double>& hours) {
		if( proj_set.find(proj)== proj_set.end() ) return false;
		dates = ts_dates;
		hours = ts[proj];
		return true;
	}

	void tagsForLastNDays(int N, vector<string>& tags, vector<double>& hours);

	bool kill(int id, string& killed_line);

//	bool undo(string& erased_line) {
//		if( journal_raw.size()==0) return false;
//		erased_line = journal_raw.pop_back();
//		refreshJournal();
//		return true;
//	}

	void printDebugInfo();

};




#endif /* EVENTS_H_ */
