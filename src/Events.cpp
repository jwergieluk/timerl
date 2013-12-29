//  copyright (C) 2013-2013 Julian Wergieluk  <julian@wergieluk.com>

#include "catch.hpp"
#include "Events.h"

bool Events::addEntry(const string& e) {
	if( e.length()==0 ) {
		char buf[500];
		snprintf(buf, 500, "%.6f ", DTime::now());
		journal_raw.push_back( buf );
		refreshJournal();
		return true;
	}
	vector<string> words;
	words = Io::s2vs(e, " ");
	if( words.size()==0) return false;

	if( words.size()==1 ) {
		if( words[0].length()>0 ) {
			if( words[0][0]!=PROJ_CHAR && words[0][0]!= TAG_CHAR) {
				char buf[500];
				snprintf(buf, 500, "%.6f %s", DTime::now(), (PROJ_CHAR + words[0]).c_str() );
				journal_raw.push_back( buf );
				refreshJournal();
				return true;
			}
		} else {
			return false;
		}
	}

	string proj="";
	for(auto i=0; i<words.size(); i++ ) {
		if( words[i].length()==1 && ( words[i][0]==TAG_CHAR || words[i][0]==PROJ_CHAR )) {
			msg.error("Illegal use of special characters.");
			return false;
		}
		if( words[i].length()>1 && words[i][0]==PROJ_CHAR ) {
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

	char buf[500];
	snprintf(buf, 500, "%.6f %s", DTime::now(), e.c_str() );
	journal_raw.push_back( buf );

	refreshJournal();
	return true;
}

void Events::readJournal(const vector<string>& j) {
	journal_raw=j;
	refreshJournal();
}

void Events::reset() {
//	timeStamps.clear();
	len_vec.clear();
//	projects.clear();
	ord_projects.clear();
	proj_set.clear();

	ts_dates.clear();
	ts.clear();

	tagged_lines.clear();
	proj_day_duration.clear();
}

void Events::refreshJournal() {
	reset();
	if( journal_raw.size()==0) {
		msg.info("Journal file is empty.");
		throw 0;
	}

	vector<double> time_vec, ref_vec, tag_duration_vec;
	vector<int> tag_no_vec;
	vector<string> proj_vec, line_vec;
	map< double, int> line_no;

	double prevStamp=0.;
	for(auto i=0; i<journal_raw.size(); i++) {
		vector<string> fields;
		fields = Io::s2vs(journal_raw[i], " ");

		if( fields.size() == 0 ) { continue; }

		double time;
		if( !Io::s2d_(time, fields[0]) ) {
			msg.warning("Malformed journal line: " + journal_raw[i]);
			continue;
		}
		if( time<prevStamp ) {
			msg.warning("Chronology destroyed by the following line: " + journal_raw[i]);
			continue;
		}

		string proj=NOTHING, line="";
		double ref=0.;
		int proj_no=0, ref_no=0, tag_no=0;
		bool err=false;
		for(auto j=1; j<fields.size() && !err; j++) {
			if( fields[j].length()==1 &&
					( fields[j][0]==PROJ_CHAR || fields[j][0]==TAG_CHAR || fields[j][0]==REF_CHAR  ) ) {
				msg.warning("Illegal use of special character in line: " + journal_raw[i]);
				err=true;
				continue;
			}

			if( fields[j].size()>1) {
				if( fields[j][0]==TAG_CHAR ) {
					tag_no++;
				}
				if( fields[j][0]==PROJ_CHAR ) {
					proj=fields[j].substr(1);
					proj_no++;
				}
				if( fields[j][0]==REF_CHAR ) {
					if( !Io::s2d_(ref, fields[j].substr(1)) ) {
						msg.error("Bad reference format in line: " + journal_raw[i]);
					} else {
						ref_no++;
					}
				}
			}
			line += fields[j] + " ";
		}
		if( time<ref ) {
			msg.error("Future reference in line: " + journal_raw[i]);
			err=true;
		}

		if( ref>0. ) {
			auto f = line_no.find(ref);
			if( f != line_no.end() ) {
				int rn = line_no[ref];
				tag_no_vec[ rn   ] = -1;
				tag_duration_vec[ rn ] = DTime::lenInDays( time_vec[rn], time )  ;
			} else {
				msg.error("Reference to a unknown line found.");
				err=true;
			}
		}

		if( proj_no>1 ) err=true;
		if( ref_no>1  ) err=true;
		if( err) continue;

		time_vec.push_back(time);
		ref_vec.push_back(ref);
		proj_vec.push_back(proj);
		tag_no_vec.push_back(tag_no);
		line_vec.push_back(line);
		len_vec.push_back(0.);

		proj_set.insert( proj );

		if( tag_no > 0 ) {
			tag_duration_vec.push_back( DTime::lenInDays( time, now )  );
		} else {
			tag_duration_vec.push_back( 0. );
		}

		line_no[time]=i;
		prevStamp=time;
		active_proj=proj;
	}

	for(auto i=0; i<time_vec.size(); i++) {
		if( tag_no_vec[i] > 0) {
			tagged_lines[ proj_vec[i] ].push_back( pair<string, double>(line_vec[i], time_vec[i]));
		}
	}

	for(auto i=0; i<time_vec.size(); i++) {
		printf("%d: %f  %15s  ref %f  tag_no %d  tag_dur %f  %s\n", i, time_vec[i], proj_vec[i].c_str(), ref_vec[i], tag_no_vec[i], tag_duration_vec[i], line_vec[i].c_str());
	}

	for(auto i=0; i<time_vec.size()-1; i++) {
		len_vec[i]=DTime::lenInDays(time_vec[i], time_vec[i+1]);
//			printf("%f %s   %f\n", timeStamps[i], msgs[i].c_str(), lengths[i]);
	}
	len_vec[len_vec.size()-1] = DTime::lenInDays( time_vec[ time_vec.size()-1  ], now );

	for(auto i=0; i<time_vec.size(); i++) {
		pair<string,double> key( proj_vec[i], floor(time_vec[i]) );
		if( proj_day_duration.find(key)!= proj_day_duration.end() ) {
			proj_day_duration[key] += len_vec[i];
		} else {
			proj_day_duration[key] = len_vec[i];
		}
	}

	for(auto it=proj_set.begin(); it!=proj_set.end(); it++) {
		ord_projects.push_back(*it);
	}

	ts_start = time_vec[0];
	ts_end = now;

	for(double d= floor(ts_start); d<=ts_end; d=DTime::nextDay(d) ) {
		ts_dates.push_back(d);
		for(auto i=0; i<ord_projects.size(); i++) {
			double l= 24.*proj_day_duration[ pair<string,double>(ord_projects[i], d) ];
			ts[ ord_projects[i] ].push_back(l);
		}
	}
}

double Events::queryTag(const string& tag) {
	if( find(ord_projects.begin(), ord_projects.end(), tag ) == ord_projects.end() ) {
		msg.error(string("Tag ") + TAG_CHAR + tag + " not found in journal." );
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







