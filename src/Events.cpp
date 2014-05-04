//  copyright (C) 2013-2014 Julian Wergieluk  <julian@wergieluk.com>

#include "Events.h"

bool Events::addEntry(const string& e) {
	if( e.length()==0 ) {
		char buf[500];
		snprintf(buf, 500, "%.6f ", now);
		journal_raw.push_back( buf );
		refreshJournal();
		return true;
	}
	vector<string> words;
	words = Io::s2vs(e, " ");
	if( words.size()==0) return false;

	if( words.size()==1 ) {
		if( words[0].length()>0 ) {
			if( words[0][0]!=PROJ_CHAR && words[0][0]!= TAG_CHAR && words[0][0]!= REF_CHAR) {
				char buf[500];
				snprintf(buf, 500, "%.6f %s", now, (PROJ_CHAR + words[0]).c_str() );
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
		if( words[i].length()==1 && ( words[i][0]==TAG_CHAR || words[i][0]==PROJ_CHAR || words[i][0]==REF_CHAR )) {
			msg.error("Illegal use of special characters.");
			return false;
		}
		if( words[i].length()>1 && words[i][0]==PROJ_CHAR ) {
			if( proj!="" ) {
				msg.error("It is not allowed to have two active projects at the same time.");
				return false;
			}
			proj=words[i].substr(1);
		}
	}
	if( proj=="" ) {
		msg.error("No project name provided.");
		return false;
	}

//	if( active_proj==proj ) {
//		msg.error("Project is already active");
//		return false;
//	}
	if( proj==NOTHING ){
		msg.error("This project name is reserved.");
		return false;
	}

	char buf[500];
	snprintf(buf, 500, "%.6f %s", now, e.c_str() );
	journal_raw.push_back( buf );

	refreshJournal();
	now+=0.000001;   // Add one second to the actual time.
	return true;
}

bool Events::addNote(const string& e) {
	vector<string> words;
	words = Io::s2vs(e, " ");
	if( words.size()==0) return false;

	string proj="";
	for(auto i=0; i<words.size(); i++ ) {
		if( words[i].length()==1 && ( words[i][0]==TAG_CHAR || words[i][0]==PROJ_CHAR || words[i][0]==REF_CHAR )) {
			msg.error("Illegal use of special characters.");
			return false;
		}
		if( words[i].length()>1 && words[i][0]==PROJ_CHAR ) {
			if( proj!="" ) {
				msg.error("It is not allowed to have two active projects at the same time.");
				return false;
			}
			proj=words[i].substr(1);
		}
	}

	if( proj==NOTHING ){
		msg.error("This project name is reserved.");
		return false;
	}

	if( active_proj != NOTHING ) {
		words.push_back(active_proj);
	}

	char buf[500];
	snprintf(buf, 500, "%.6f %s", now, e.c_str() );
	journal_raw.push_back( buf );

	refreshJournal();
	now+=0.000001;   // Add one second to the actual time.
	return true;
}

void Events::readJournal(const vector<string>& j) {
	journal_raw=j;
	refreshJournal();
}

void Events::reset() {
	time_vec.clear();
	ref_vec.clear();
	tag_duration_vec.clear();

	tag_no_vec.clear();
	id_vec.clear();
	proj_vec.clear();
	line_vec.clear();

	line_no.clear();
	id_to_line_no.clear();

	ord_projects.clear();
	proj_set.clear();
	ts_dates.clear();
	ts.clear();
	proj_day_duration.clear();
}

void Events::refreshJournal() {
	reset();
	if( journal_raw.size()==0) {
		msg.info("Journal file is empty.");
		return;
	}

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
					// TODO check if valid tag name
					tag_no++;
				}
				if( fields[j][0]==PROJ_CHAR ) {
					// TODO check if a valid proj name
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
		if( err) continue; // TODO print an error msg

		id_vec.push_back(0);
		time_vec.push_back(time);
		ref_vec.push_back(ref);
		proj_vec.push_back(proj);
		tag_no_vec.push_back(tag_no);
		line_vec.push_back(line);
		proj_len_vec.push_back(0.);

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

	int m=1;
	for(auto i=0; i<time_vec.size(); i++) {
		if( tag_no_vec[i] > 0) {
			id_to_line_no[m] = i;
			id_vec[i]=m;
			m++;
		}
	}

	for(auto i=0; i<time_vec.size()-1; i++) {
		proj_len_vec[i]=DTime::lenInDays(time_vec[i], time_vec[i+1]);
//			printf("%f %s   %f\n", timeStamps[i], msgs[i].c_str(), lengths[i]);
	}
	proj_len_vec[proj_len_vec.size()-1] = DTime::lenInDays( time_vec[ time_vec.size()-1  ], now );

	for(auto i=0; i<time_vec.size(); i++) {
		pair<string,double> key( proj_vec[i], floor(time_vec[i]) );
		if( proj_day_duration.find(key)!= proj_day_duration.end() ) {
			proj_day_duration[key] += proj_len_vec[i];
		} else {
			proj_day_duration[key] = proj_len_vec[i];
		}
	}

	for(auto it=proj_set.begin(); it!=proj_set.end(); it++) {
		ord_projects.push_back(*it);
	}

	for(double d= floor(time_vec[0]); d<=now; d=DTime::nextDay(d) ) {
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

bool Events::tagExists(const string& tag) {
	return !(find(ord_projects.begin(), ord_projects.end(), tag ) == ord_projects.end());
}

void Events::tagsForLastNDays(int N, vector<string>& tags, vector<double>& hours) {
	tags.clear(); hours.clear();
	for(auto it=ts.begin(); it!=ts.end(); it++ ) {
		if( it->first != NOTHING) {
			double h=0.;
			for(int k=max(0, (int)(it->second.size()-N)); k< it->second.size(); k++) {
				h += it->second[k];
			}
			if( h>0.001 ) {
				tags.push_back( it->first );
				hours.push_back( h );
			}
		}
	}
}

bool Events::kill(int id, string& killed_line) {
	if( id_to_line_no.find(id)!= id_to_line_no.end() ) {
		killed_line=line_vec[ id_to_line_no[id] ];
		char buf[50];
		snprintf(buf, 50, "%.6f %c%s %c%.6f", now, PROJ_CHAR, active_proj.c_str(), REF_CHAR, time_vec[ id_to_line_no[id] ]);
		journal_raw.push_back(buf);
		refreshJournal();
		return true;
	} else {
		return false;
	}
}

void Events::printDebugInfo() {
	for(auto i=0; i<time_vec.size(); i++) {
		printf("%d[%d]: %f  proj_vec %10s  len_vec %f  &%f  tag_no %d  tag_dur %f  line_vec %s\n", i, id_vec[i], time_vec[i], proj_vec[i].c_str(), proj_len_vec[i], ref_vec[i], tag_no_vec[i], tag_duration_vec[i], line_vec[i].c_str());
	}
}


#ifdef UNIT_TESTS
#include "catch.hpp"

TEST_CASE("Parsing new entries", "[Events]") {

	Msg m; m.toggleMute();
	Events e(m);

	REQUIRE( e.addEntry( "two  +different  +projects" ) == false );
	REQUIRE( e.addEntry( "no project name provided" ) == false );

}

#endif
