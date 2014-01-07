//  copyright (C) 2013-2014 Julian Wergieluk  <julian@wergieluk.com>

#include "Msg.h"
#include "DTime.h"
#include "io.h"
#include <map>
#include <cmath>




void Msg::toggleMute() { mute=true; }

void Msg::error(const string& err) {
	if(!mute) {
		printf("%sERROR%s: %s\n", BOLDRED, RESET, err.c_str());
	}
}

void Msg::warning(const string& err) {
	if(!mute) {
		printf("%sWARNING%s: %s\n", BOLDYELLOW, RESET, err.c_str());
	}
}

void Msg::info(const string& err) {
	if(!mute) {
		printf("%sINFO%s: %s\n", BOLDGREEN, RESET, err.c_str());
	}
}

void Msg::par() { printf("\n"); }

void Msg::printTags(int N, const vector<string>& tags, const vector<double>& hours) {
	double total=0.;
	for(auto i=0; i<hours.size(); i++) total+=hours[i];
	printf("D%02d  %s%s%s  ", N, BOLDWHITE, DTime::hoursToStr(total).c_str(), RESET );
	for(auto i=0; i<tags.size(); i++) printf("%s %s  ", tags[i].c_str(), DTime::hoursToStr(hours[i]).c_str() );
	printf("\n");
}

void Msg::printActiveNotes(const string& proj, const vector<int>& id ,const vector<string>& notes) {
	if( notes.size()>0 ) {
		printf("Tagged lines for the active project %s+%s%s\n", GREEN, proj.c_str(), RESET);
		for(auto i=0; i<notes.size(); i++) {
			printf("[%d] ", id[i]);
			printLine(notes[i]);
			printf("\n");
		}
		printf("\n");
		printf("  (Use 'k' to remove a note from this view)\n");
		printf("  (Use 'n' to add a new note or list notes for all projects)\n");
	} else {
		printf("No notes for the active project %s+%s%s found.\n", GREEN, proj.c_str(), RESET);
		printf("  (Use 'n' to add a new note or list notes for all projects)\n");
	}
}

void Msg::printNotes(const vector<int>& id ,const vector<string>& notes) {
	if( id.size()==0 ) return;
	printf("Tagged lines in all projects are\n");
	for(auto i=0; i<notes.size(); i++) {
		printf("[%d] ", id[i]);
		printLine(notes[i]);
		printf("\n");
	}
}

void Msg::printLine(const string& line) {
	vector<string> words;
	words = Io::s2vs(line, " ");
	for(const auto& w : words ) {
		if( w[0]== TAG_CHAR ) {
			printf("%s%s%s ", BLUE, w.c_str(), RESET);
			continue;
		}
		if( w[0]== PROJ_CHAR ) {
			printf("%s%s%s ", GREEN, w.c_str(), RESET);
			continue;
		}
		if( w[0]== REF_CHAR ) {
			printf("%s%s%s ", RED, w.c_str(), RESET);
			continue;
		}
		printf("%s ", w.c_str());
	}
}

void Msg::kill(const string& killed_line) {
	printf("%sLine killed%s: ", RED, RESET);
	printLine(killed_line);
	printf("\n");
}

void Msg::printTs(const string& proj, const vector<double>& dates, const vector<double>& hours ) {
	if( dates.size()< 4 ) {
		info("Not enough data collected.");
		return;
	}
	map< pair<int, int>, char > m;
	int h=0;
	for(auto i=0; i< dates.size(); i++ ) {
		int y = (int)floor(hours[i]);
		h = max(h, y);
		double frac = hours[i]-floor(hours[i]);
		if( y<0.01 ) { m[ pair<int, int>(y, i) ] = '.'; continue; }
		if( frac < 0.25 ) { m[ pair<int, int>(y, i) ] = '*'; continue; }
		if( frac < 0.75 ) { m[ pair<int, int>(y, i) ] = '*'; continue; }
		if( frac < 1.0 ) { m[ pair<int, int>(y, i) ] = '*'; continue; }
	}

	printf("Duration graph for project %s\n\n", proj.c_str());
	for(auto r=h; r>=0; r-- ) {
		printf("%2d ", r);
		for( auto c=0; c<dates.size(); c++) {
			auto key = pair<int, int>(r,c);
			if( m.find(key) != m.end() ) {
				printf("%c", m[key]);
			} else {
				printf(" ");
			}
		}
		printf("%2d\n", r);
	}
	printf("   ");
	for(auto i=0; i<dates.size(); i++ ) {
		int weekDay= DTime::weekDay((int)floor(dates[i]) );
		if( weekDay==1 ) { printf("M"); continue; }
		printf(" ");
	}
	printf("\n");
}

void Msg::printUsage() {
	printf("timerl (c) 2013-2014  Julian Wergieluk\n");
}
