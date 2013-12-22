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


#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */




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

	string active_tag;
public:

	bool close(const string& file) {
		if( active_tag=="nothing") {
			printf("ERROR: no tag is active.\n");
			return false;
		}
		FILE* out;
		if ((out = fopen( file.c_str(), "a"))==NULL) return false;

		char buf[500];
		double n=DTime::now();
		snprintf(buf, 500, "%04d-%02d-%02d %02d:%02d|nothing\n", DTime::getYear(n), DTime::getMonth(n), DTime::getDay(n),
				DTime::getHour(n), DTime::getMinute(n) );
		string new_line=buf;

		if( fprintf(out, "%s", new_line.c_str()) <0) { fclose(out); return false; }
		fclose(out);

		printf("[Adding the closing tag]\n");
		return true;
	}

	bool add(const string& file, const vector<string>& args) {
		if( active_tag==args[0] ) {
			printf("[Tag already active]\n");
			return false;
		}
		if( args[0]=="nothing" ){
			printf("ERROR: This tag name is reserved.\n");
			return false;
		}

		FILE* out;
		if ((out = fopen( file.c_str(), "a"))==NULL) return false;

		char buf[500];
		double n=DTime::now();
		snprintf(buf, 500, "%04d-%02d-%02d %02d:%02d|", DTime::getYear(n), DTime::getMonth(n), DTime::getDay(n),
				DTime::getHour(n), DTime::getMinute(n) );
		string new_line=buf;
		new_line += args[0]+ "\n";

		if( fprintf(out, "%s", new_line.c_str()) <0) { fclose(out); return false; }
		fclose(out);

		printf("%s[Add]%s %02d:%02d %s\n", BOLDGREEN, RESET,DTime::getHour(DTime::now()), DTime::getMinute(DTime::now()), args[0].c_str());
		return true;
	}

	const string& activeTag() { return active_tag; }

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
			active_tag=tag;
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

	void printTail() {
		printf("# tail -n 7 %s\n", journal_file.c_str());
		for(auto i=max((int)0, (int)(journal_raw.size()-7)); i<(int)journal_raw.size(); i++) printf("%s\n", journal_raw[i].c_str());
	}
};




#endif /* EVENTS_H_ */
