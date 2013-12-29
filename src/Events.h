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

	string journal_file;
	vector<string> journal_raw;

	vector<double> len_vec;
	vector<string> ord_projects;
	set<string> proj_set;

	double ts_start, ts_end, now;
	vector<double > ts_dates;
	map< string, vector<double> > ts;

	map<string, vector<pair<string, double> > > tagged_lines;
	map< pair<string, double>, double > proj_day_duration;

	string active_proj;
public:

	Events(Msg& m): msg(m) {
		ts_start=0.; ts_end=0.;
		now=DTime::now();
	};

	string activeProj() const {
		if( active_proj== NOTHING ) return "";
		return active_proj;
	}

	bool addEntry(const string&);

	bool closed() { return active_proj==NOTHING; }

	void readJournal(const vector<string>&);
	void reset();
	void refreshJournal();

	vector<string> getNotes(const string& proj) {
		vector<string> notes;
		for(auto i=tagged_lines[proj].begin(); i!=tagged_lines[proj].end(); i++) {
			notes.push_back((*i).first);
//			printf(">> (%0.6f) %s\n", (*i).second ,(*i).first.c_str());
		}
		return notes;
	}

	const string getLastLine() {
		if( journal_raw.size()==0 ) {
			return "";
		} else {
			return journal_raw[ journal_raw.size()-1];
		}
	}

	vector<string> getActiveNotes() { return getNotes(active_proj); }

	double queryTag(const string&);

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

	void tagsForToday() {
		vector<string> tags; vector<double> hours;

		double total=0.;
		for(auto it=ts.begin(); it!=ts.end(); it++ ) {
			if( it->first != NOTHING) {
				double h = it->second[it->second.size()-1];
				if( h>0.001 ) {
					tags.push_back( it->first );
					hours.push_back( h );
					total += h;
				}
			}
		}

		printf("[today    ]  total %.1f  ", total);
		for(auto i=0; i<tags.size(); i++) printf("%s %.2f  ", tags[i].c_str(), hours[i]);
		printf("\n");
	}

	void tagsForLastNDays(int N) {
		vector<string> tags; vector<double> hours;

		double total=0.;
		for(auto it=ts.begin(); it!=ts.end(); it++ ) {
			if( it->first != NOTHING) {
				double h=0.;
				for(int k=max(0, (int)(it->second.size()-N)); k< it->second.size(); k++) {
					h += it->second[k];
				}
				if( h>0.001 ) {
					tags.push_back( it->first );
					hours.push_back( h );
					total += h;
				}
			}
		}

		printf("[%2d days  ]  total %.1f  ", N, total);
		for(auto i=0; i<tags.size(); i++) printf("%s %.2f  ", tags[i].c_str(), hours[i]);
		printf("\n");
	}

};




#endif /* EVENTS_H_ */
