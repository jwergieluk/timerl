//  copyright (C) 2013-2013 Julian Wergieluk  <julian@wergieluk.com>

#ifndef CMD_H_
#define CMD_H_

#include <vector>
#include <string>
using namespace std;

#include "io.h"

class Cmd {
public:

	Cmd(int ac, char** av) {
		vector<string> av_;
		for(int c=1; c<ac; c++) {
			av_.push_back( Io::trim(av[c]) );
		}
	}

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
