//  copyright (C) 2013-2013 Julian Wergieluk  <julian@wergieluk.com>

#ifndef MSG_H_
#define MSG_H_

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


};



#endif /* MSG_H_ */
