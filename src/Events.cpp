//  copyright (C) 2013-2013 Julian Wergieluk  <julian@wergieluk.com>

#include "catch.hpp"
#include "Events.h"

bool Events::addEntry(const string& e) {
	if( e.length()==0 ) return false;
	vector<string> words;
	words = Io::s2vs(e, " ");
	if( words.size()==0) return false;

	string proj="";
	for(auto i=0; i<words.size(); i++ ) {
		if( words[i].length()==1 && ( words[i][0]=='#' || words[i][0]=='+' )) {
			msg.error("Illegal use of special characters.");
			return false;
		}
		if( words[i].length()>1 && words[i][0]=='+' ) {
			if( proj!="" ) {
				msg.error("Only one active project at a time allowed.");
				return false;
			}
			proj=words[i].substr(1);
		}
	}
	if( proj=="" ) {
		msg.error("No project name provided.");
		return false;
	}

	if( active_proj==proj ) {
		msg.error("Project is already active");
		return false;
	}
	if( proj==NOTHING ){
		msg.error("This project name is reserved.");
		return false;
	}
	return true;
}

void Events::readJournal(const vector<string>& j) {
	journal_raw=j;
	journal_file="/dev/null";

	if( journal_raw.size()==0) {
		msg.info("Journal file is empty.");
		throw 0;
	}

	double prevStamp=0.;
	for(auto i=0; i<journal_raw.size(); i++ ) {
		vector<string> fields;
		fields = Io::s2vs(journal_raw[i], " ");

		if( fields.size() == 0 ) { continue; }

		double d;
		if( !Io::s2d_(d, fields[0]) ) {
			msg.warning("Malformed journal line: " + journal_raw[i]);
			continue;
		}
		if( d<prevStamp ) {
			msg.warning("Chronology destroyed by the following line: " + journal_raw[i]);
			continue;
		}

		string proj=NOTHING, note="";
		bool tagged=false;
		for( auto j=1; j<fields.size(); j++) {
			if( fields[j].size()>0) {
				if( fields[j][0]=='#' ) tagged=true;
				if( fields[j][0]=='+' ) {
					if( proj== NOTHING) {
						proj= fields[j].substr(1);
					} else {
						msg.warning("Malformed journal line: " + journal_raw[i]);
					}
				}
				note += fields[j] + " ";
			}
		}
		if( tagged && proj!= NOTHING ) {
			tagged_lines[proj].push_back( pair<string, double>(note, d));
		}

		timeStamps.push_back( d );
		lengths.push_back(0.);
		projects.push_back( proj  );
		projSet.insert( proj );
		prevStamp=d;
		active_proj=proj;
	}

	for(auto i=0; i<timeStamps.size()-1; i++) {
		lengths[i]=DTime::lenInDays(timeStamps[i], timeStamps[i+1]);
//			printf("%f %s   %f\n", timeStamps[i], msgs[i].c_str(), lengths[i]);
	}
	lengths[lengths.size()-1] = DTime::lenInDays( timeStamps[ timeStamps.size()-1  ], DTime::now() );

	for(auto i=0; i<timeStamps.size(); i++) {
		pair<string,double> key( projects[i], floor(timeStamps[i]) );
		if( m.find(key)!= m.end() ) {
			m[key] += lengths[i];
		} else {
			m[key] = lengths[i];
		}
	}

	for(auto it=projSet.begin(); it!=projSet.end(); it++) {
		ord_projects.push_back(*it);
	}

	ts_start = timeStamps[0];
	ts_end = DTime::now();

	for(double d= floor(ts_start); d<=ts_end; d=DTime::nextDay(d) ) {
		ts_dates.push_back(d);
		for(auto i=0; i<ord_projects.size(); i++) {
			double l= 24.*m[ pair<string,double>(ord_projects[i], d) ];
			ts[ ord_projects[i] ].push_back(l);
		}
	}
}

double Events::queryTag(const string& tag) {
	if( find(ord_projects.begin(), ord_projects.end(), tag ) == ord_projects.end() ) {
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





TEST_CASE("Parsing new entries", "[Events]") {

	Msg m; m.toggleMute();
	Events e(m);

	REQUIRE( e.addEntry( "two  +different  +projects" ) == false );
	REQUIRE( e.addEntry( "no project name provided" ) == false );

}







