//  copyright (C) 2013-2013 Julian Wergieluk  <julian@wergieluk.com>

#ifndef CMD_H_
#define CMD_H_

#include <vector>
#include <string>
using namespace std;

#include "io.h"

class Cmd {
private:
	string cmd, args_str;
	vector<string> args;

public:

	Cmd(int ac, char** av): cmd(""), args_str("") {
		if( ac>= 2) cmd=Io::trim(av[1]);
		for(int c=2; c<ac; c++) {
			string a= Io::trim(av[c]);
			args.push_back(a);
			if(args_str.length()>0) {
				args_str = args_str + " " + a;
			} else {
				args_str = a;
			}
		}
	}

	bool is(const string& c) {
		return (c==cmd);
	}
	const string& getCmd() {	return cmd; }
	const vector<string>& getArgsVs() {	return args; }
	const string& getArgs() { return args_str; }

	void parse(vector<string> av) {

		for(int c=0; c<av.size(); c++) {
//			string item = ioHelper::trim(av[c]);
//			int pos;
//			if( (pos=item.find("=")) != string::npos ) {
//
//				string key = ioHelper::trim(item.substr(0,pos));
//				if( pos < item.size()-1) {
//					string value = ioHelper::trim(item.substr(pos+1, item.size()));
//					keys.push_back(key);
//					values.push_back(value);
//				} else {
//					insert(key, "");
//				}
//			} else {
//				insert(item, "");
//			}
		}
	}


};




#endif /* CMD_H_ */
