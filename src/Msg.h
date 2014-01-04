//  copyright (C) 2013-2013 Julian Wergieluk  <julian@wergieluk.com>

#ifndef MSG_H_
#define MSG_H_

#include "DTime.h"
#include "io.h"
#include "config.h"
#include <string>

using namespace std;

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


class Msg {
private:
	bool mute;

public:
	void toggleMute() { mute=true; }

	void error(const string& err) {
		if(!mute) {
			printf("%sERROR%s: %s\n", BOLDRED, RESET, err.c_str());
		}
	}

	void warning(const string& err) {
		if(!mute) {
			printf("%sWARNING%s: %s\n", BOLDYELLOW, RESET, err.c_str());
		}
	}

	void info(const string& err) {
		if(!mute) {
			printf("%sINFO%s: %s\n", BOLDGREEN, RESET, err.c_str());
		}
	}

	void par() { printf("\n"); }

	void printTags(int N, const vector<string>& tags, const vector<double>& hours) {
		double total=0.;
		for(auto i=0; i<hours.size(); i++) total+=hours[i];
		printf("D%02d  %s%s%s  ", N, BOLDWHITE, DTime::hoursToStr(total).c_str(), RESET );
		for(auto i=0; i<tags.size(); i++) printf("%s %s  ", tags[i].c_str(), DTime::hoursToStr(hours[i]).c_str() );
		printf("\n");
	}

	void printActiveNotes(const string& proj, const vector<int>& id ,const vector<string>& notes) {
//		printf("%s[Active project]%s %s\n", GREEN, RESET, proj.c_str());
		printf("Tagged lines for the active project %s+%s%s\n", GREEN, proj.c_str(), RESET);
		for(auto i=0; i<notes.size(); i++) {
			printf("[%d] ", id[i]);
			printLine(notes[i]);
			printf("\n");
		}
	}

	void printNotes(const vector<int>& id ,const vector<string>& notes) {
		printf("Tagged lines in all projects are\n");
		for(auto i=0; i<notes.size(); i++) {
			printf("[%d] ", id[i]);
			printLine(notes[i]);
			printf("\n");
		}
	}

	void printLine(const string& line) {
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

	void kill(const string& killed_line) {
		printf("%s[Task killed]%s ", RED, RESET);
		printLine(killed_line);
		printf("\n");
	}

	void printTs(const vector<double>& dates, const vector<double>& hours ) {
		for(auto i=0; i< dates.size(); i++ ) printf("%.0f %.2f\n", dates[i], hours[i]);


	}

};



#endif /* MSG_H_ */
