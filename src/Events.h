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
#include "io.h"
#include "DTime.h"



using namespace std;

class Events {
private:

	string journal_file;
	vector<string> journal_raw;

	vector<double> timeStamps, lengths;
	vector<string> msgs, ord_tags;
	set<string> tags;

	double ts_start, ts_end;
	vector<double > ts_dates;
	map< string, vector<double> > ts;

	map< pair<string, double>, double > m;

public:

	bool close(const string& file) {
		char buf[500];
		snprintf(buf, 500, "%.6f nothing\n", DTime::now());
		string new_line=buf;

		printf(">> %s", new_line.c_str());
		return true;
	}

	bool add(const string& file, const vector<string>& args) {
//		FILE* out; string buf;
//		if ((out = fopen( file.c_str(), "a"))==NULL) return false;

		char buf[500];
		snprintf(buf, 500, "%.6f ", DTime::now());

		string new_line=buf;
		for(auto i=0; i<args.size(); i++) {
			new_line += args[i] + " ";
		}
		new_line += "\n";

		printf(">> %s", new_line.c_str());
//		void dump(const string fileName, const vector<string>& vector, const bool printShape) {
//			Error::print("dump", "dump", fileName);
//			FILE* out; string buf;
//
//			if ((out = fopen( fileName.c_str(), "w"))==NULL)	throw Error("dump", "dump", (boost::format("cannot open %1%") % fileName).str(), 199);
//
//			try {
//				for( int i = 0; i < (int)vector.size(); i++) {
//					fprintf(out, "%s ", vector[i].c_str());
//				}
//				fprintf(out, "\n");
//			} catch (...) {
//				fclose(out);
//				throw;
//			}
//
//			fclose(out);
//		}

		return true;
	}


	void readJournal(const string& file) {
		journal_file=file;
		if( ! Io::file2vs(file.c_str(), journal_raw) ) {
			printf("ERROR: Unable to read the journal file.\n");
			throw 255;
		}
		if( journal_raw.size()==0) {
			printf("INFO: Journal file is empty.\n");
			throw 0;
		}

		double prevStamp=0.;
		for(auto i=0; i<journal_raw.size(); i++ ) {
			vector<string> fields;
			fields = Io::s2vs(journal_raw[i], "|");

			if( fields.size() <2 ) {
				printf("# WARNING: Ignoring the line: %s\n",journal_raw[i].c_str());
				continue;
			}

			double d;
			if( !DTime::fromString(fields[0], d) ) {
				printf("# WARNING: Ignoring the line: %s\n",journal_raw[i].c_str());
				continue;
			}
			if( d<prevStamp ) {
				printf("# WARNING: Chronology destroyed by the following line: %s\n",journal_raw[i].c_str());
				continue;
			}

			string tag=fields[1];

			timeStamps.push_back( d );
			lengths.push_back(0.);
			msgs.push_back( tag  );
			tags.insert( tag );
			prevStamp=d;
		}

		for(auto i=0; i<timeStamps.size()-1; i++) {
			lengths[i]=DTime::lenInDays(timeStamps[i], timeStamps[i+1]);
//			printf("%f %s   %f\n", timeStamps[i], msgs[i].c_str(), lengths[i]);
		}
		lengths[lengths.size()-1] = DTime::lenInDays( timeStamps[ timeStamps.size()-1  ], DTime::now() );

		for(auto i=0; i<timeStamps.size(); i++) {
			pair<string,double> key( msgs[i], floor(timeStamps[i]) );
			if( m.find(key)!= m.end() ) {
				m[key] += lengths[i];
			} else {
				m[key] = lengths[i];
			}
		}

		for(auto it=tags.begin(); it!=tags.end(); it++) {
			ord_tags.push_back(*it);
		}

		ts_start = timeStamps[0];
		ts_end = DTime::now();

		for(double d= floor(ts_start); d<=ts_end; d=DTime::nextDay(d) ) {
			ts_dates.push_back(d);
			for(auto i=0; i<ord_tags.size(); i++) {
				double l= 24.*m[ pair<string,double>(ord_tags[i], d) ];
				ts[ ord_tags[i] ].push_back(l);
			}
		}
	}

	double queryTag(const string& tag) {
		if( find(ord_tags.begin(), ord_tags.end(), tag ) == ord_tags.end() ) {
			printf("# ERROR: Tag '%s' not found in journal.\n", tag.c_str());
			return -1.;
		} else {
			double sum=0.;
			for(auto i=0; i<ts[tag].size(); i++) {
				sum+=ts[tag][i];
			}
			printf("# %s %.2f\n", tag.c_str(), sum);
			return sum;
		}
	}

	void printTimeSeries() {
		printf("#date ");
		for(auto k=0; k<ord_tags.size(); k++) { printf("%s ", ord_tags[k].c_str());  }
		printf("\n");
		for(auto i=0; i<ts_dates.size(); i++) {
			printf("%.0f ", ts_dates[i]);
			for(auto k=0; k<ord_tags.size(); k++) {
				printf("%f ", ts[ord_tags[k]][i]);
			}
			printf("\n");
		}
	}

	void tagsForToday() {
		vector<string> tags; vector<double> hours;

		double total=0.;
		for(auto it=ts.begin(); it!=ts.end(); it++ ) {
			if( it->first != "nothing") {
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
			if( it->first != "nothing") {
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
